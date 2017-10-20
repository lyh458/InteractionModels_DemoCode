#ifdef WIN64
#include "imodobjectdetection.h"
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QRect>
#include <QtGui/QVector3D>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <qmath.h>

#include <opencv2/photo.hpp>

#include <Eigen3/Eigen/Core>
#include <Eigen3/Eigen/Eigen>

iModObjectDetection::iModObjectDetection(QObject *parent) :
  QObject(parent),
  ocv_8UC4_rgbr(NULL),
  ocv_8UC3_hsv(NULL),
  ocv_8UC1_obcl(NULL),
  systemtimer(NULL)
{
  is_running = false;
  //cvNamedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
  color_min = QVector3D(45, 60, 80);
  color_max = QVector3D(70, 220, 200);
  erode_cycles = 6;
  delate_cycles = 8;
  showwindow = false;
}

iModObjectDetection::~iModObjectDetection()
{
  cvReleaseImage(&ocv_8UC3_hsv);
  cvReleaseImage(&ocv_8UC1_obcl);
  cvReleaseImage(&ocv_8UC4_rgbr);
}

void iModObjectDetection::slotDetectObject(DepthSpacePoint* dsp, CameraSpacePoint* csp, int nDepthWidth, int nDepthHeight, const RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight)
{
  is_running = true;
  QTime t;
  t.start();
  //int telapsed = 0;
  // The commented stuff is for debugging. Attention: The debugging stuff can still have memory leaks

  // First run stuff
  if(!systemtimer)
  {
    systemtimer = new QTimer();
    connect(systemtimer, &QTimer::timeout, this, &iModObjectDetection::slotSystemTimer);
    systemtimer->start(1000);
    ocv_8UC4_rgbr = cvCreateImage(cvSize(nColorWidth, nColorHeight), 8, 4);

    ocv_8UC4_rgbr_small = cvCreateImage(cv::Size(1280,720), 8, 4);
    ocv_8UC3_hsv        = cvCreateImage(cv::Size(1280,720), 8, 3);
    ocv_8UC1_obcl       = cvCreateImage(cv::Size(1280,720), 8, 1);
  }

  if(!pColorBuffer)
  {
    is_running = false;
    return;
  }

  /// Image processing
  cvSetData(ocv_8UC4_rgbr, (void*) pColorBuffer, ocv_8UC4_rgbr->widthStep);
  cvResize(ocv_8UC4_rgbr, ocv_8UC4_rgbr_small);
  cvCvtColor(ocv_8UC4_rgbr_small, ocv_8UC3_hsv, CV_BGR2HSV);
  cvInRangeS(ocv_8UC3_hsv, cvScalar(color_min.x(), color_min.y(), color_min.z()), cvScalar(color_max.x(), color_max.y(), color_max.z()), ocv_8UC1_obcl);
  cvErode(ocv_8UC1_obcl, ocv_8UC1_obcl, 0, erode_cycles);
  cvDilate(ocv_8UC1_obcl, ocv_8UC1_obcl, 0, delate_cycles);
  //qDebug() << "OCV hsv + color detect:" << t.elapsed() - telapsed;
  //telapsed = t.elapsed();

  /// Edge detection
  cv::Mat canny_output = cv::cvarrToMat(ocv_8UC1_obcl);
  cv::Canny(canny_output, canny_output, 100, 100*2, 3 );
  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(canny_output, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  //qDebug() << "Do Canny edge detection:" << t.elapsed() - telapsed;
  //telapsed = t.elapsed();
  /// Face calculation
  cv::Mat drawing = cv::cvarrToMat(ocv_8UC4_rgbr_small);
  cv::RNG rng(12345);
  std::vector<cv::Vec4i> hierarchy;
  QRect aabb = QRect(0,0,0,0);
  for( int i = 0; i< contours.size(); i++ )
  {
    cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    cv::drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
    int max = 0, mix = nColorWidth, may = 0, miy = nColorHeight;
    for(int j = 0; j < contours[i].size(); j++)
    {
      miy = qMin(miy, contours[i][j].y);
      mix = qMin(mix, contours[i][j].x);
      may = qMax(may, contours[i][j].y);
      max = qMax(max, contours[i][j].x);
    }
    QRect naabb(mix, miy, max - mix, may - miy);
    if((aabb.width() * aabb.height()) < (naabb.width() * naabb.height()))
      aabb = naabb;
  }

  //qDebug() << "Create AABB:" << t.elapsed() - telapsed;
  //telapsed = t.elapsed();
  /// Mapping test
  if(showwindow)
  {
    // Use a scaled window here
    cv::Rect r;
    r.width = aabb.width();
    r.height = aabb.height();
    r.x = aabb.left();
    r.y = aabb.top();
    cv::rectangle(drawing, r, cv::Scalar(0, 0, 0), 2, cv::LINE_8, 0);
    cv::resize(drawing, drawing, cv::Size(1280,720));
    imshow("Color Image", drawing);
  }
  // Fix dimension reduction
  aabb.setLeft(aabb.left() * 1920.0/1280.0);
  aabb.setRight(aabb.right() * 1920.0/1280.0);
  aabb.setTop(aabb.top() * 1080.0/720.0);
  aabb.setBottom(aabb.bottom() * 1080.0/720.0);

  if(aabb.size() == QSize(0,0))
  {
    is_running = false;
    return;
  }

  /// SpaceStream
  CameraSpacePoint* nsp = (CameraSpacePoint*)malloc(sizeof(CameraSpacePoint) * aabb.width() * aabb.height());
  memset(nsp, 0, sizeof(CameraSpacePoint) * aabb.width() * aabb.height());
  float mindistance = std::numeric_limits<UINT16>::max();
  for(int y = 0; y < aabb.height(); y+=nColorHeight/nDepthHeight)
  {
    for(int x = 0; x < aabb.width(); x+=nColorWidth/nDepthWidth)
    {
      DepthSpacePoint p = dsp[(aabb.top()+y) * nColorWidth + aabb.left()+x];
      if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
      {
        int depthX = static_cast<int>(p.X + 0.5f);
        int depthY = static_cast<int>(p.Y + 0.5f);
        nsp[y * aabb.width() + x] = csp[depthY * nDepthWidth + depthX];
        if(nsp[y * aabb.width() + x].Z > 0.0)
          mindistance = qMin(mindistance, nsp[y * aabb.width() + x].Z);
      }
    }
  }

  // Filter data
  int datacount = 0;
  for(int i = 0; i < aabb.width() * aabb.height(); i++)
  {
    if(nsp[i].Z > (mindistance + 0.4)) // clip everything too far away
      nsp[i].Z = 0; // Assume 0 as invalid

    if(nsp[i].Z > 0)
      datacount++;
  }

  //qDebug() << "Create VOI:" << t.elapsed() - telapsed << datacount;
  //telapsed = t.elapsed();

  // PCA
  Eigen::MatrixXd pointcloud;
  pointcloud.resize(3, datacount);
  int c = 0;
  for(int i = 0; i < aabb.width() * aabb.height(); i++)
  {
    if(nsp[i].Z > 0)
    {
      pointcloud(0,c) = nsp[i].X;
      pointcloud(1,c) = nsp[i].Y;
      pointcloud(2,c) = nsp[i].Z;
      c++;
    }
  }

  // Printing point cloud
  /*
  QFile test("t.dat");
  test.open(QIODevice::ReadWrite);
  for(int i = 0;i < datacount; i++)
  {
    test.write(QString(QString::number(pointcloud(0, i)) + ", " +
                       QString::number(pointcloud(1, i)) + ", " +
                       QString::number(pointcloud(2, i)) + "\n").toLatin1());
  }
  test.flush();
  test.close();
  */

  //Get means:
  Eigen::VectorXd means = Eigen::VectorXd::Zero(3);

  for (int i = 0; i < pointcloud.rows(); i++)
    means(i) = pointcloud.row(i).sum() / datacount;

  //Get sample correlation:
  Eigen::MatrixXd covariance = Eigen::MatrixXd::Zero(3, 3);

  for (int i = 0; i < pointcloud.rows(); i++)
  {
    for (int j = 0; j < pointcloud.rows(); j++)
    {
      double val = 0.0;

      for (int k = 0; k < pointcloud.cols(); k++)
      {
        val += (pointcloud(i, k) - means(i)) * (pointcloud(j, k) - means(j));
      }

      val *= (1.0 / (datacount - 1));
      covariance(i, j) = val;
    }
  }

  // compute the eigenvalue on the Cov Matrix
  Eigen::EigenSolver<Eigen::MatrixXd> m_solve(covariance, true);
  Eigen::VectorXd eigenvalues = m_solve.eigenvalues().real();
  Eigen::MatrixXd eigenvectors = m_solve.eigenvectors().real();
  //qDebug() << "PCA:" << t.elapsed() - telapsed;
  //telapsed = t.elapsed();
  QMap<qreal, QVector3D> results;
  for(int i = 0; i < eigenvalues.rows(); i++)
    results.insert(eigenvalues(i), QVector3D(eigenvectors.col(i)(0), eigenvectors.col(i)(1), eigenvectors.col(i)(2)));

  if(results.count() != 3)
  {
    is_running = false;
    free(nsp);
    return;
  }

  QByteArray objectdata;
  objectdata.resize(sizeof(QVector3D) * (results.values().count() + 1 + 1)); // +1: First Vector = Center of Box; + 1 Second Vector = Eigenvalues

  QVector3D center(means(0), means(1), means(2));
  memcpy(objectdata.data(), &center, sizeof(QVector3D));

  //qreal d = qSqrt(qPow(center.x(),2) + qPow(center.y(),2) + qPow(center.z(),2));
  float dist1 = 0;
  float dist2 = 0;
  float dist3 = 0;

  for(int i = 0; i < datacount; i++)
  {
    // Qt internals
    //dist1 = qMax(dist1, qAbs(QVector3D(pointcloud.col(i)(0),pointcloud.col(i)(1),pointcloud.col(i)(2)).distanceToPlane(center, results.values()[2])));
    //dist2 = qMax(dist2, qAbs(QVector3D(pointcloud.col(i)(0),pointcloud.col(i)(1),pointcloud.col(i)(2)).distanceToPlane(center, results.values()[1])));
    //dist3 = qMax(dist3, qAbs(QVector3D(pointcloud.col(i)(0),pointcloud.col(i)(1),pointcloud.col(i)(2)).distanceToPlane(center, results.values()[0])));

    //d=(x-c)*n Hessische Normalform
    dist1 = qMax(dist1, qAbs(QVector3D::dotProduct(QVector3D(pointcloud.col(i)(0),pointcloud.col(i)(1),pointcloud.col(i)(2)) - center, results.values()[2])));
    dist2 = qMax(dist2, qAbs(QVector3D::dotProduct(QVector3D(pointcloud.col(i)(0),pointcloud.col(i)(1),pointcloud.col(i)(2)) - center, results.values()[1])));
    dist3 = qMax(dist3, qAbs(QVector3D::dotProduct(QVector3D(pointcloud.col(i)(0),pointcloud.col(i)(1),pointcloud.col(i)(2)) - center, results.values()[0])));
  }

  center = QVector3D(dist1,dist2,dist3);
  memcpy(objectdata.data() + sizeof(QVector3D), &center, sizeof(QVector3D));

  for(int i = 2; i < results.values().count() + 2; i++)
    memcpy(objectdata.data() + sizeof(QVector3D) * i, &(results.values()[i-2]), sizeof(QVector3D));

  emit this->signalPublishObjectTrackingResult(objectdata);

  free(nsp);

  is_running = false;
  //qDebug() << "Box dimensions:" << t.elapsed() - telapsed;
  fpscounter++;
  lastframetime += t.elapsed();
}

void iModObjectDetection::slotKinectOCVValues(QVector3D cmin, QVector3D cmax, quint8 e, quint8 d, bool s)
{
  color_max = cmax;
  color_min = cmin;
  erode_cycles = e;
  delate_cycles = d;

  if(!showwindow && s)
    cvNamedWindow("Color Image", CV_WINDOW_AUTOSIZE);
  if(showwindow && !s)
    cvDestroyWindow("Color Image");

  showwindow = s;
  qDebug() << "[ObjectDetection] Applying new limits";
}

void iModObjectDetection::slotSystemTimer()
{
  //qDebug() << "Object tracking FPS:" << fpscounter << "| Last frame time average:" << (double)lastframetime/fpscounter;
  fpscounter = 0;
  lastframetime = 0;
}

QMatrix4x4 iModObjectDetection::computeNumerical(QList<QVector3D> _master, QList<QVector3D> _slave)
{
  if(_master.size() != _slave.size())
  {
    qDebug() << "Failed to compute transition (Eigen3)";
    return QMatrix4x4();
  }

  QList<Eigen::Vector3d> master;
  foreach(QVector3D v, _master)
    master.append(Eigen::Vector3d(v.x(), v.y(), v.z()));

  QList<Eigen::Vector3d> slave;
  foreach(QVector3D v, _slave)
    slave.append(Eigen::Vector3d(v.x(), v.y(), v.z()));


  Eigen::MatrixXd solution = Eigen::MatrixXd::Zero(4, 4);
  solution(3, 3) = 1;

  Eigen::MatrixXd t(slave.size(), 4);

  for (int i = 0; i < slave.size(); i++)
  {
    for (int j = 0; j < 3; j++)
      t(i, j) = slave[i](j);

    t(i, 3) = 1;
  }

  for (int i = 0; i < 3; i++)
  {
    Eigen::VectorXd r(master.size());

    for (int j = 0; j < master.size(); j++)
      r(j) = master[j](i);

    Eigen::MatrixXd result = t.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(r);

    for (int j = 0; j < 4; j++)
      solution(i, j) = result(j);
  }

  QMatrix4x4 result = QMatrix4x4(solution(0,0), solution(0,1), solution(0,2), solution(0,3),
                                 solution(1,0), solution(1,1), solution(1,2), solution(1,3),
                                 solution(2,0), solution(2,1), solution(2,2), solution(2,3),
                                 solution(3,0), solution(3,1), solution(3,2), solution(3,3));
  return result;
}
#endif
