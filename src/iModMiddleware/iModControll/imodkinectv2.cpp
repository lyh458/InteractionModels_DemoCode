#ifdef WIN64
#include "imodkinectv2.h"

#include <QtCore/QPoint>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QRect>
#include <QtCore/QFile>
#include <QtCore/QThread>
#include <QtWidgets/QApplication>

#include <profileapi.h>
#include <iostream>

#include "imodcontrol.h"
#include "imodobjectdetection.h"

iModKinectV2::iModKinectV2(iModControl* c, iModSkynet* n) :
  QObject(),
  minrange(0),
  maxrange(8),
  control(c),
  network(n)
{
  m_pBodyFrameReader = NULL;
  m_pKinectSensor = NULL;
  m_pCoordinateMapper = NULL;
}

iModKinectV2::~iModKinectV2()
{
  delete odthread;
  delete m_pColorRGBX;
  delete m_pDepthCoordinates;
}

HRESULT iModKinectV2::startKinect()
{
  m_pDepthCoordinates = new DepthSpacePoint[cColorWidth * cColorHeight];
  m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
  csp = new CameraSpacePoint[cDepthWidth * cDepthHeight];
  odworker = new iModObjectDetection();
  odthread = new QThread();
  systemtimer = new QTimer();
  connect(systemtimer, &QTimer::timeout, this, &iModKinectV2::slotSystemTimer);
  systemtimer->start(1000);

  connect(odthread, &QThread::destroyed, odworker, &iModObjectDetection::deleteLater);
  connect(this, &iModKinectV2::signalProcessObjectTracking, odworker, &iModObjectDetection::slotDetectObject);
  connect(odworker, &iModObjectDetection::signalPublishObjectTrackingResult, control, &iModControl::slotSendObjectTrackingData);
  connect(network, &iModSkynet::signalKinectOpenCVValues, odworker, &iModObjectDetection::slotKinectOCVValues);
  odworker->moveToThread(odthread);
  odthread->start();

  HRESULT hr;

  hr = GetDefaultKinectSensor(&m_pKinectSensor);
  if (FAILED(hr))
  {
    return hr;
  }

  if (m_pKinectSensor)
  {
    // Initialize the Kinect and get coordinate mapper and the frame reader

    if (SUCCEEDED(hr))
    {
      hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
    }

    hr = m_pKinectSensor->Open();

    if (SUCCEEDED(hr))
    {
      hr = m_pKinectSensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Depth |
                                                       FrameSourceTypes::FrameSourceTypes_Color |
                                                       FrameSourceTypes::FrameSourceTypes_Body,
                                                       &m_pMultiSourceFrameReader);
    }
  }

  if (!m_pKinectSensor || FAILED(hr))
  {
    qDebug() << "No ready Kinect found!";
    return E_FAIL;
  }

  return hr;
}

void iModKinectV2::update()
{
  while(true)
  {
    QTime t;
    // Get worker state here
    bool worker_is_ready = !odworker->isRunning();
    if (!m_pMultiSourceFrameReader)
    {
      return;
    }

    IMultiSourceFrame* pMultiSourceFrame = NULL;
    IDepthFrame* pDepthFrame = NULL;
    IColorFrame* pColorFrame = NULL;
    IBodyFrame* pBodyFrame = NULL;

    HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

    if(SUCCEEDED(hr))
    {
      t.start();
    }

    if (SUCCEEDED(hr) && worker_is_ready)
    {
      IDepthFrameReference* pDepthFrameReference = NULL;

      hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
      if (SUCCEEDED(hr))
      {
        hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
      }

      safeRelease(pDepthFrameReference);
    }

    if (SUCCEEDED(hr) && worker_is_ready)
    {
      IColorFrameReference* pColorFrameReference = NULL;

      hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
      if (SUCCEEDED(hr))
      {
        hr = pColorFrameReference->AcquireFrame(&pColorFrame);
      }

      safeRelease(pColorFrameReference);
    }

    if (SUCCEEDED(hr))
    {
      IBodyFrameReference* pBodyFrameReference = NULL;

      hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
      if (SUCCEEDED(hr))
      {
        hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
      }

      safeRelease(pBodyFrameReference);
    }

    if (SUCCEEDED(hr))
    {
      INT64 nTime = 0;
      IFrameDescription* pDepthFrameDescription = NULL;
      int nDepthWidth = 0;
      int nDepthHeight = 0;
      UINT nDepthBufferSize = 0;
      UINT16* pDepthBuffer = NULL;

      IFrameDescription* pColorFrameDescription = NULL;
      int nColorWidth = 0;
      int nColorHeight = 0;
      ColorImageFormat imageFormat = ColorImageFormat_None;
      UINT nColorBufferSize = 0;
      if(worker_is_ready) pColorBuffer = NULL;

      IFrameDescription* pBodyIndexFrameDescription = NULL;
      IBody* ppBodies[BODY_COUNT] = {0};

      // get depth frame data
      hr = pBodyFrame->get_RelativeTime(&nTime);

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pDepthFrameDescription->get_Width(&nDepthWidth);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pDepthFrameDescription->get_Height(&nDepthHeight);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);
      }

      // get color frame data
      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pColorFrameDescription->get_Width(&nColorWidth);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pColorFrameDescription->get_Height(&nColorHeight);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        if (imageFormat == ColorImageFormat_Bgra)
        {
          hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
        }
        else if (m_pColorRGBX)
        {
          pColorBuffer = m_pColorRGBX;
          nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
          hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);
        }
        else
        {
          hr = E_FAIL;
        }
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(nDepthWidth * nDepthHeight, pDepthBuffer, nColorWidth * nColorHeight, m_pDepthCoordinates);
      }

      if (SUCCEEDED(hr) && worker_is_ready)
      {
        memset(csp, 0, sizeof(CameraSpacePoint) * cDepthWidth * cDepthHeight);
        hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(nDepthBufferSize, pDepthBuffer, cDepthWidth * cDepthHeight, csp);

        /*
        // This thing ist BAD! -> ~100ms time consumption
        for(int i = 0; i < cColorWidth * cColorHeight; i++)
        {
          DepthSpacePoint p = m_pDepthCoordinates[i];
          if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
          {
            int depthX = static_cast<int>(p.X + 0.5f);
            int depthY = static_cast<int>(p.Y + 0.5f);
            if ((depthX >= 0 && depthX < nDepthWidth) && (depthY >= 0 && depthY < nDepthHeight))
            {
              m_pCoordinateMapper->MapDepthPointToCameraSpace(m_pDepthCoordinates[i], pDepthBuffer[depthY * nDepthWidth + depthX], csp + i);
            }
          }
        }
        */
      }


      if (SUCCEEDED(hr) && worker_is_ready)
      {
        emit this->signalProcessObjectTracking(m_pDepthCoordinates, csp, nDepthWidth, nDepthHeight, pColorBuffer, nColorWidth, nColorHeight);
      }

      // get body frame data
      if (SUCCEEDED(hr))
      {
        hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
      }

      if(SUCCEEDED(hr))
      {
        processSkeleton(nTime, BODY_COUNT, ppBodies);
      }

      for (int i = 0; i < _countof(ppBodies); ++i)
      {
        safeRelease(ppBodies[i]);
      }

      safeRelease(pDepthFrameDescription);
      safeRelease(pColorFrameDescription);
      safeRelease(pBodyIndexFrameDescription);
    }

    safeRelease(pDepthFrame);
    safeRelease(pColorFrame);
    safeRelease(pBodyFrame);
    safeRelease(pMultiSourceFrame);

    lastframetime += t.elapsed();

    QApplication::processEvents(QEventLoop::AllEvents);
  }
}

void iModKinectV2::slotSetKinectID(quint8 cid, quint8 id)
{
  Q_UNUSED(id);
  kinectid = cid;
}

void iModKinectV2::slotKinectRange(qreal mi_r, qreal ma_r)
{
  minrange = mi_r;
  maxrange = ma_r;
}

void iModKinectV2::slotSystemTimer()
{
  //qDebug() << "Skeletal tracking FPS:" << fps_body_tracking << "| Last frame time average:" << (double)lastframetime/fps_body_tracking;
  fps_body_tracking = 0;
  lastframetime = 0;
}

void iModKinectV2::processSkeleton(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
  fps_body_tracking++;

  HRESULT hr;
  //if (m_pCoordinateMapper) // Why? It's not used
  {
    bool map = true;
    for (int i = 0; i < nBodyCount && map; ++i)
    {
      bool run_processing = true;
      IBody* pBody = ppBodies[i];
      if (pBody)
      {
        BOOLEAN bTracked = false;
        hr = pBody->get_IsTracked(&bTracked);

        if (SUCCEEDED(hr) && bTracked)
        {
          HandState leftHandState = HandState_Unknown;
          HandState rightHandState = HandState_Unknown;

          pBody->get_HandLeftState(&leftHandState);
          pBody->get_HandRightState(&rightHandState);

          // The Way to Go:
          Joint joints[JointType_Count];
          JointOrientation orientations[JointType_Count];

          hr = pBody->GetJoints(JointType_Count, joints);
          if (SUCCEEDED(hr))
          {
            hr = pBody->GetJointOrientations(JointType_Count, orientations);
            if (SUCCEEDED(hr))
            {
              KinectJoint* buffer = (KinectJoint*)malloc(sizeof(KinectJoint) * JointType_Count);
              for(int i = 0; i < JointType_Count && run_processing; i++)
              {
                KinectJoint tmp;
                tmp.type = joints[i].JointType;
                tmp.trackingstate = joints[i].TrackingState;
                tmp.position.x = joints[i].Position.X;
                tmp.position.y = joints[i].Position.Y;
                tmp.position.z = joints[i].Position.Z;

                if(tmp.position.z < minrange || tmp.position.z > maxrange) // If user is too close or too far -> cut him off
                {
                  run_processing = false;
                  break;
                }

                // find the matching orientation...
                for(int j = 0; j < JointType_Count; j++)
                {
                  if(joints[i].JointType == orientations[j].JointType)
                  {
                    tmp.orientation.w = orientations[j].Orientation.w;
                    tmp.orientation.x = orientations[j].Orientation.x;
                    tmp.orientation.y = orientations[j].Orientation.y;
                    tmp.orientation.z = orientations[j].Orientation.z;
                    break;
                  }
                }
                buffer[i] = tmp;
              }

              if(run_processing)
              {
                QByteArray kinectbuffer = QByteArray((char*)buffer, JointType_Count * sizeof(KinectJoint));
                UINT64 tid;
                pBody->get_TrackingId(&tid);
                emit this->signalSendKinectData(tid, (quint8)JointType_Count - 1, kinectbuffer, kinectid);
              }
              free(buffer);
            }
          }
        }
      }
    }
  }
}

KinectScreenPoint iModKinectV2::bodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
  // Calculate the body's position on the screen
  DepthSpacePoint depthPoint = {0};
  m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

  qreal screenPointX = static_cast<float>(depthPoint.X * width) / cDepthWidth;
  qreal screenPointY = static_cast<float>(depthPoint.Y * height) / cDepthHeight;

  return KinectScreenPoint(screenPointX, screenPointY);
}

#endif
