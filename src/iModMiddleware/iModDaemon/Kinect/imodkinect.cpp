#include "imodkinect.h"

#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <qmath.h>

#include "../imodringbuffer.h"
#include "../imodmathutils.h"
#include "../imodutile.h"
#include "kvmainwindow.h"
#include "qmath.h"

iModKinect::iModKinect(iModUtile* _ut, QObject *parent) :
  QObject(parent),
  current_root_kinect(0),
  kinect_ma_size(8),
  utile(_ut),
  lastframetime(0),
  view(NULL),
  initialized(false),
  ot_root_kinect(0),
  lastvaliddepth(0),
  fps_object_tracking(0),
  fps_skeletal_tracking(0)
{
  otdata.ts = 0;
  // dont create stuff with "new" here! -> use init()

  // Except this thing... its a GUI -> cant be pushed to another Thread (GUIs always in main thread)
  //view = new KVMainWindow();
  //connect(this, &iModKinect::signalKVSetTransitions, view, &KVMainWindow::slotSetTransitions);
  //connect(this, &iModKinect::signalKVUpdateDrawing,  view, &KVMainWindow::slotUpdateDrawing );
}

// Why is it this way: iModKinect is supposed to run in a Thread (moveToThread)
// If something is created on the Heap within the constructor:
//  -> ownership in Thread, that created iModKinect -> leads to segfault
// Result: This is a slot so it can be triggered with QThread::started signal and stuff belongs to new thread
void iModKinect::slotInit()
{
  datamutex = new QMutex(QMutex::NonRecursive);
  otmutex = new QMutex(QMutex::NonRecursive);
  QMutexLocker ml(datamutex);
  initialized = true;
  kinect_position = QVector3D(0, 0, 0);
  kinect_rotation = QQuaternion();

  systemtimer = new QTimer();
  connect(systemtimer, &QTimer::timeout, this, &iModKinect::slotCleanKinects);
  connect(systemtimer, &QTimer::timeout, this, &iModKinect::slotSystemTimer);
  systemtimer->start(1000);

  callibrationtimer = new QTimer();
  connect(callibrationtimer, &QTimer::timeout, this, &iModKinect::slotCallibrationTimer);

  math_test = new iModMathUtils();
  math_test->resetAveraging();
}

iModKinect::~iModKinect()
{
  qDebug() << "Deleting Kinect";
  delete systemtimer;
  delete callibrationtimer;
  delete datamutex;
  delete otmutex;
}

QString iModKinect::getPerformanceData()
{
  QMutexLocker ml(datamutex);
  return performancestring;
}

bool iModKinect::otDataAvailable()
{
  // Just make sure the data are not older than 1 second
  QMutexLocker ml(otmutex);
  if(otdata.ts + 1000 > QDateTime::currentMSecsSinceEpoch())
    return true;
  return false;
}

void iModKinect::slotSetCurrentRootKinect(int value)
{
  QMutexLocker ml(datamutex);
  current_root_kinect = value;
  ot_root_kinect = value; // For debugging, Root kinect = Object tracking kinect
  qDebug() << "Root Kinect" <<current_root_kinect;
  kinecttransitions.clear();
  bodymatching.clear();
}

void iModKinect::slotSetKinectOffsets(QVector3D position, QQuaternion rotation)
{
  QMutexLocker ml(datamutex);
  kinect_position = position;
  kinect_rotation = rotation;

  k_to_c = QMatrix4x4(kinect_rotation.toRotationMatrix());
  k_to_c.setColumn(3, QVector4D(kinect_position.x(), kinect_position.y(), kinect_position.z(), 1));
  qDebug() << "Kinect to ART transformation:" << k_to_c;
}

void iModKinect::slotCalibrateKinects()
{
  QMutexLocker ml(datamutex);
  qDebug() << "[Info]Resetting Kinect calibration";
  qDebug() << "[Info] Kinect calibration: Make shure only one person is in the view volume";
  kinecttransitions.clear();
  bodymatching.clear();

  callibrationtimer->start(1000);
}

void iModKinect::slotDetectUsers()
{
  QMutexLocker ml(datamutex);
  qDebug() << "[Info] Resetting user detection";
  bodymatching.clear();
}

void iModKinect::slotGenerateNextFrame()
{
  QMutexLocker ml(datamutex);
  QTime timer;
  timer.start();

  bool useobjecttracking = otDataAvailable();
  if(kinectdata.count() == 0 || kinecttransitions.count() == 0)
  {
    // There is no skeletal tracking... but do we have an object?
    if(useobjecttracking)
    {
      currentframe = "6d 7 " + getOTData();
      currentframe = currentframe.left(currentframe.size() - 1) + "\r\n";
      lastframetime = timer.elapsed();
      //qDebug() << currentframe;
    }

    return;
  }

  QList<int> currentuser = getUserIDs();
  QList<double> view_merged;
  QList<char> tstate;
  QString art_6d = "";
  if(useobjecttracking)
    art_6d = "6d " + QString::number(qMin(kinectid_to_artid.count(), currentuser.count())  * 8 + 7) + " " ;
  else
    art_6d = "6d " + QString::number(qMin(kinectid_to_artid.count(), currentuser.count())  * 8) + " " ;

  for(int i = 0; i < qMin(kinectid_to_artid.count(), currentuser.count()); i++)
  {
    for(int jid = 0; jid < 24; jid++)
    {
      if(kinectid_to_artid[i].contains(jid))
      {
        KinectJoint joint = combineKinects(jid, currentuser[i]);

        // This crashes, when currenuser[i] is not present as key -> Default return value will be unusable
        if(!kinecthistory.keys().contains(currentuser[i]))
          return;
        if(!kinecthistory.value(currentuser[i])->contains(joint.type))
          kinecthistory.value(currentuser[i])->insert(joint.type, new iModRingBuffer(kinect_ma_size));

        if(joint.position == KinectJointPosition()) // Try to fix it with last pose -> Not a definit fix if history is empty
        {
          iModRingBuffer* buff = kinecthistory.value(currentuser[i])->value(joint.type);
          if(buff)
          {
            joint.setPosition(buff->readPos(kinect_ma_size - 1));
            //joint.setRotation(buff->readRot(kinect_ma_size - 1));
            //joint.type = jid;
          }
          else
          {
            // Failed to build joints -> Skip frame
            lastframetime = timer.elapsed();
            return;
          }
        }

        if(joint.orientation == KinectJointOrientation())
        {
          iModRingBuffer* buff = kinecthistory.value(currentuser[i])->value(joint.type);
          if(buff)
          {
            //joint.setPosition(buff->readPos(kinect_ma_size - 1));
            joint.setRotation(buff->readRot(kinect_ma_size - 1));
            //joint.type = jid;
          }
        }

        KinectScreenPoint ksp;
        kinecthistory.value(currentuser[i])->
            value(joint.type)->insert(QVector3D(joint.position.x, joint.position.y, joint.position.z),
                                      QQuaternion(joint.orientation.w, joint.orientation.x, joint.orientation.y, joint.orientation.z));

        joint.setPosition(iModMathUtils::movingAveragePosition(kinecthistory.value(currentuser[i])->value(joint.type)));
        joint.setRotation(iModMathUtils::movingAverageRotation(kinecthistory.value(currentuser[i])->value(joint.type)));

        view_merged.append(joint.position.x);
        view_merged.append(joint.position.y);
        view_merged.append(joint.position.z);
        tstate.append(joint.trackingstate);
        // Build a transition matrix
        QMatrix4x4 matrix;
        QQuaternion quat(joint.orientation.w, joint.orientation.x, joint.orientation.y, joint.orientation.z);
        quat.normalize();
        matrix.rotate(quat);
        matrix.setColumn(3, QVector4D(joint.position.x, joint.position.y, joint.position.z, 1));

        //matrix = k_to_c * matrix;

        /*
        if(joint.type == 0 || joint.type == 11)
          totalstring += "," + QString::number(matrix.column(3).x(), 'f', 3) + "," + QString::number(matrix.column(3).y(), 'f', 3) + "," + QString::number(matrix.column(3).z(), 'f', 3);
        */
        // id quality
        QString targetinfo = "[" + QString::number(kinectid_to_artid[i].value(joint.type)) + " 1.000]";
        // position
        targetinfo += "[" + QString::number(matrix.column(3).x(), 'f', 4) + " " +
                      QString::number(matrix.column(3).y(), 'f', 4) + " " +
                      QString::number(matrix.column(3).z(), 'f', 4);
        targetinfo += " 0.000000 0.000000 0.000000]";
        // rotation
        targetinfo += "[" + QString::number(matrix(0,0), 'f', 4) + " " + QString::number(matrix(0,1), 'f', 4) + " " + QString::number(matrix(0,2), 'f', 4) + " "
                      + QString::number(matrix(1,0), 'f', 4) + " " + QString::number(matrix(1,1), 'f', 4) + " " + QString::number(matrix(1,2), 'f', 4) + " "
                      + QString::number(matrix(2,0), 'f', 4) + " " + QString::number(matrix(2,1), 'f', 4) + " " + QString::number(matrix(2,2), 'f', 4) + "]";
        art_6d += targetinfo + " ";
      }
    }
  }

  if(view)
    emit this->signalKVUpdateDrawing(1, serializeQList<double>(view_merged), serializeQList<char>(tstate));

  /*
  totalstring += "\n";
  if(dumpfileDualKinect && totalstring.contains(","))
    dumpfileDualKinect->write(totalstring.toLocal8Bit());
  */

  if(useobjecttracking)
    art_6d += getOTData();
  art_6d = art_6d.left(art_6d.size() - 1) + "\r\n";

  lastframetime = timer.elapsed();
  currentframe = art_6d;
  //qDebug() << currentframe;
}

void iModKinect::slotBuildARTMatching()
{
  QMutexLocker ml(datamutex);
  kinectid_to_artid.clear();
  for(int i = 0; i < 2; i++)
  {
    QMap<int, int> map;
    foreach(QString ktvalue, utile->getAllKeys("Kinect_V2_Targets"))
    {
      int key = utile->readSetting("Kinect_V2_Targets/" + ktvalue).toInt();
      QVariant matchingart = utile->readSetting("Kinect_to_ART/" + ktvalue);
      if(matchingart.isValid())
      {
        int value = utile->readSetting("ART_Targets/"+matchingart.toString().split(" ", QString::SkipEmptyParts)[i]).toInt();
        map.insert(key, value);
      }
    }
    kinectid_to_artid.append(map);
  }

  kinect_rotationmatching.clear();
  foreach(QString joint1, utile->getAllKeys("Kinect_V2_Rotationmatching"))
  {
    int key        = utile->readSetting("Kinect_V2_Targets/" + joint1).toInt();
    QString joint2 = utile->readSetting("Kinect_V2_Rotationmatching/" + joint1).toString();
    int value      = utile->readSetting("Kinect_V2_Targets/" + joint2).toInt();
    kinect_rotationmatching.insert(value, key);
  }
}

void iModKinect::slotKinectObjectTracking(QByteArray data, quint8 clientid)
{
  // Return if not the right kinect
  if(clientid != ot_root_kinect)
    return;

  quint8 vectors = data.count() / sizeof(QVector3D);
  if(vectors != 5)
  {
    qDebug() << "[slotKinectObjectTracking@iModKinect] Wrong OT data size";
    return;
  }

  // Copy data
  QVector3D center;
  memcpy(&center, data.data(), sizeof(QVector3D));
  QVector3D dimensions;
  memcpy(&dimensions, data.data() + sizeof(QVector3D), sizeof(QVector3D));

  QList<QVector3D> eigenvectors;
  for(int i = 2; i < data.count() / sizeof(QVector3D); i++)
  {
    QVector3D tmp;
    memcpy(&tmp, data.data() + i * sizeof(QVector3D), sizeof(QVector3D));
    eigenvectors.prepend(tmp); // Eivenvectors become less important with higher index
  }

  if(eigenvectors[0].x() < 0)
  {
    eigenvectors[0] = eigenvectors[0] * -1;
  }
  eigenvectors[0] = eigenvectors[0].normalized();
  eigenvectors[1] = QVector3D::crossProduct(QVector3D(1, 0, 0), eigenvectors[0]).normalized();
  eigenvectors[2] = QVector3D::crossProduct(eigenvectors[0], eigenvectors[1]).normalized();

  // Create virtual A.R.T. Targets
  QList<QVector3D> buffer;

  buffer.append(center);
  buffer.append(center + dimensions[0] * eigenvectors[0]);
  buffer.append(center - dimensions[0] * eigenvectors[0]);
  buffer.append(center + dimensions[1] * eigenvectors[1]);
  buffer.append(center - dimensions[1] * eigenvectors[1]);
  buffer.append(center + dimensions[2] * eigenvectors[2]);
  buffer.append(center - dimensions[2] * eigenvectors[2]);

  QMutexLocker ml(otmutex);
  otdata.update(buffer);

  fps_object_tracking++;
}

void iModKinect::slotUpdateKinectData(quint64 bid, quint8 jointcount, QByteArray data, quint8 kinectid)
{
  fps_skeletal_tracking++;
  QMutexLocker ml(datamutex);
  QPair<int, quint64> index = QPair<int, quint64>(kinectid, bid);
  if(!kinectdata.contains(index))
    kinectdata.insert(index, new KinectKameraData());
  else
    kinectdata.value(index)->jointdata->clear();

  kinectdata.value(index)->timestamp = QDateTime::currentMSecsSinceEpoch();

  //QString totalstring = QString::number(QDateTime::currentMSecsSinceEpoch());
  QList<QVector3D> master, slave;
  QList<double> trackingdata;
  QList<char> tstate;
  for(int i = 0; i < jointcount+1; i++)
  {
    KinectJoint tmp;
    memcpy(&tmp, data.data() + i * sizeof(KinectJoint), sizeof(KinectJoint));
    //slave.append(tmp.getPosition());
    trackingdata.append(tmp.position.x);
    trackingdata.append(tmp.position.y);
    trackingdata.append(tmp.position.z);
    tmp.position.x *= 1000.0;
    tmp.position.y *= 1000.0;
    tmp.position.z *= 1000.0;
    kinectdata.value(index)->jointdata->append(tmp);
    tstate.append(tmp.trackingstate);

    //KinectScreenPoint tmp2;
    //memcpy(&tmp2, screendata.data() + i * sizeof(KinectScreenPoint), sizeof(KinectScreenPoint));
    //master.append(QVector3D(tmp2.px, tmp2.py, 0));
    /*
    if((tmp.type == 0 || tmp.type == 11) && kinectid == current_root_kinect)
    {
      totalstring += "," + QString::number(tmp.position.x,'f',3) + "," + QString::number(tmp.position.y,'f',3) + "," + QString::number(tmp.position.z,'f',3);
    }
*/
  }
  /*
  totalstring += "\n";
  if(dumpfileSingleKinect && totalstring.contains(","))
    dumpfileSingleKinect->write(totalstring.toLocal8Bit());
*/
  if(view)
    emit this->signalKVUpdateDrawing(kinectid, serializeQList<double>(trackingdata), serializeQList<char>(tstate));
  updateBodyMatching(index);
  cleanKinects(1000/5);

  //QMatrix4x4 matrixtest = iModMathUtils::computeNumerical(master, slave);
  //qDebug() << matrixtest * slave[0] <<  master[0];
  //qDebug() << matrixtest;
}

void iModKinect::slotCallibrationTimer()
{
  QList<int> kinectlist = getKinects();
  if(callibrationbuffer.count() == 7)
  {
    callibrationtimer->stop();

    iModMathUtils* math = new iModMathUtils();
    QMatrix4x4 result;
    foreach(int kinectid, kinectlist)
    {
      math->resetAveraging();

      for(int i = 0; i < callibrationbuffer.count(); i++)
        result = math->averageTransitionMatrix(callibrationbuffer[i].value(kinectid));

      kinecttransitions.insert(kinectid, result);
    }
    delete math;
    callibrationbuffer.clear();
    if(view)
      emit this->signalKVSetTransitions(kinecttransitions);
  }
  else
  {
    QMap<int, QMatrix4x4> tmpcallibration;

    foreach(int kinectid, kinectlist)
    {
      QMatrix4x4 result = getKinectTransformation(kinectid, current_root_kinect);
      if(!result.isIdentity())
        tmpcallibration.insert(kinectid, result);
      else
      {
        //Considder an uncallibrated Kinect, which can see the user in a position, where the rootkinect is incapable to see the user
        //Try to callibrate with the use of another Kinect, that is already known by root: New->Known->Root
        qDebug() << "Using backup callibration";
        foreach(int secondary_kinect, tmpcallibration.keys())
        {
          QMatrix4x4 result = getKinectTransformation(kinectid, secondary_kinect);
          if(!result.isIdentity())
          {
            tmpcallibration.insert(kinectid, result * tmpcallibration.value(secondary_kinect));
            break;
          }
        }
      }
    }
    //qDebug() << tmpcallibration.value(241).determinant();
    callibrationbuffer.append(tmpcallibration);
  }
}

void iModKinect::slotSystemTimer()
{
  //qDebug() << "Object tracking FPS:" << fps_object_tracking;
  performancestring = "FPS OT: " + QString::number(fps_object_tracking) + " | FPS ST: " + QString::number(fps_skeletal_tracking);
  fps_object_tracking = 0;
  fps_skeletal_tracking = 0;
}

QList<int> iModKinect::getKinects()
{
  QList<int> result;
  result.append(current_root_kinect);

  for(int i = 0; i < kinectdata.count(); i++)
    if(!result.contains(kinectdata.keys()[i].first))
      result.append(kinectdata.keys()[i].first);

  return result;
}

QMatrix4x4 iModKinect::getKinectTransformation(int _src, int _dst)
{
  quint64 srcbody = 0, dstbody = 0;
  for(int i = 0; (i < kinectdata.size()) && (srcbody == 0 || dstbody == 0); i++)
  {
    if(kinectdata.keys()[i].first == _src)
      srcbody = kinectdata.keys()[i].second;
    if(kinectdata.keys()[i].first == _dst)
      dstbody = kinectdata.keys()[i].second;
  }
  if(srcbody == 0 || dstbody == 0)
  {
    qDebug() << "[Error] Kinect calibration: Cant find transition because not all required kinects can see the user";
    return QMatrix4x4();
  }

  QPair<int, quint64> src = QPair<int, quint64>(_src, srcbody);
  QPair<int, quint64> dst = QPair<int, quint64>(_dst, dstbody);

  QList<int> matchingjoints;

  // Still crashes here.... Build explicit calibration!!
  if(kinectdata.contains(src) && kinectdata.contains(dst))
  {
    foreach(KinectJoint joint, *(kinectdata.value(src)->jointdata))
      foreach(KinectJoint joint2, *(kinectdata.value(dst)->jointdata))
        if(joint.type == joint2.type && joint.trackingstate == 2 && joint2.trackingstate == 2)
          matchingjoints.append(joint.type);
  }
  else
  {
    qDebug() << "Transitions cant be calculated... did you set the root kinect?";
    return QMatrix4x4();
  }

  if(matchingjoints.count() < 4) // We need at least 4 joints
    return QMatrix4x4();

  QList<QVector3D> master;
  QList<QVector3D> slave;

  for(int i = 0; i < matchingjoints.count(); i++)
  {
    QVector3D tmp_master = QVector3D(kinectdata.value(dst)->jointdata->at(matchingjoints.at(i)).position.x,
                                     kinectdata.value(dst)->jointdata->at(matchingjoints.at(i)).position.y,
                                     kinectdata.value(dst)->jointdata->at(matchingjoints.at(i)).position.z);
    QVector3D tmp_slave  = QVector3D(kinectdata.value(src)->jointdata->at(matchingjoints.at(i)).position.x,
                                     kinectdata.value(src)->jointdata->at(matchingjoints.at(i)).position.y,
                                     kinectdata.value(src)->jointdata->at(matchingjoints.at(i)).position.z);

    master.append(tmp_master);
    slave.append(tmp_slave);
  }

  //qDebug() << "[Info] Calibrating with" << master.count() << "joints";
  QMatrix4x4 result = iModMathUtils::computeNumerical(master, slave);
  //qDebug() << "Transformation" << src << "to" << dst << ":\n" << result;
  return result;
}

void iModKinect::updateBodyMatching(QPair<int, quint64> index)
{
  if(kinecttransitions.count() == 0)
    return;

  bool existing = false;
  for(int i = 0; i < bodymatching.count(); i++)
  {
    if(bodymatching[i].kinectid == index.first && bodymatching[i].bodyid == index.second)
    {
      existing = true;
      bodymatching[i].timestamp = QDateTime::currentMSecsSinceEpoch();
      break;
    }
  }

  if(!existing)
  {
    UserDataKinect newuser;
    newuser.bodyid = index.second;
    newuser.kinectid = index.first;
    newuser.timestamp = QDateTime::currentMSecsSinceEpoch();

    if(bodymatching.isEmpty())
    {
      newuser.userid = getNextUserID();
      newuser.artid = getNextARTID();
      bodymatching.append(newuser);
      qDebug() << "Created new user with id" << newuser.userid;
      kinecthistory.insert(newuser.userid, new QMap<int, iModRingBuffer*>());
      return;
    }

    //Find x Joints that are tracked
    QList<KinectJoint> new_merginglist;
    foreach(KinectJoint joint, *kinectdata.value(index)->jointdata)
      if(joint.trackingstate == 2)
        new_merginglist.append(joint);

    if(new_merginglist.count() < 10) // Merge with at least 10 Joints
      return;

    // find matches from other users
    QList<KinectJoint> known_merginglist;
    foreach(UserDataKinect udata, bodymatching)
    {
      // Build a list of known joints for a user
      QPair<int, quint64> newindex = QPair<int, quint64>(udata.kinectid, udata.bodyid);
      if(!kinectindexExists(newindex))
        return;

      foreach(KinectJoint known_joint, *kinectdata.value(newindex)->jointdata)
        foreach(KinectJoint new_joint, new_merginglist)
          if(known_joint.type == new_joint.type && known_joint.trackingstate == 2)
            known_merginglist.append(known_joint);

      // Create summations
      if(known_merginglist.count() >= 10) // Merge with at least 10 Joints
      {
        QVector3D known_summation, new_summation;
        foreach(KinectJoint joint, known_merginglist)
          known_summation += joint.getPosition();
        known_summation /= known_merginglist.count();
        known_summation = QVector4D(kinecttransitions.value(udata.kinectid) * QVector4D(known_summation, 1)).toVector3D();

        foreach(KinectJoint new_joint, new_merginglist)
          foreach(KinectJoint known_joint, known_merginglist) // only for joints in known ons
            if(new_joint.type == known_joint.type)
              new_summation += new_joint.getPosition();
        new_summation /= known_merginglist.count();
        new_summation = QVector4D(kinecttransitions.value(index.first) * QVector4D(new_summation, 1)).toVector3D();

        qreal distance = iModMathUtils::distance(new_summation, known_summation);
        qDebug() << "User distance:" << distance << "Resulting action:";
        if(distance < 500) // Values between 300-600 normal; sometimes much better, sometimes worse
        {
          newuser.userid = udata.userid;
          newuser.artid = udata.artid;
          qDebug() << "User" << newuser.userid << "found by additional Kinect. Distance" << distance;
          break;
        }
      }
      else
      {
        known_merginglist.clear();
      }
    }

    if(newuser.userid == 0)
    {
      newuser.userid = getNextUserID();
      newuser.artid = getNextARTID();
      qDebug() << "Created new user with id" << newuser.userid;
      kinecthistory.insert(newuser.userid, new QMap<int, iModRingBuffer*>());
    }
    bodymatching.append(newuser);
  }
}

void iModKinect::cleanKinects(int msec)
{
  QList<QPair<int, quint64> > test = kinectdata.keys();
  qint64 currenttime = QDateTime::currentMSecsSinceEpoch();
  for(int i = 0; i < test.size(); i++)
    if(currenttime > (kinectdata.value(test[i])->timestamp + msec))
      kinectdata.remove(test[i]);

  for(int i = 0; i < bodymatching.size(); i++)
    if(currenttime > bodymatching[i].timestamp + msec)
    {
      int uid = bodymatching[i].userid;
      int kid = bodymatching[i].kinectid;
      qDebug() << "Removed user with id" << uid << "from Kinect" << kid;
      bodymatching.removeOne(bodymatching[i]);

      QList<int> users = getUserIDs(); // check, if the history has to be deleted too
      if(!users.contains(uid))
        kinecthistory.remove(uid);
    }
}

int iModKinect::getNextUserID()
{
  QList<int> uids = getUserIDs();
  for(int i = 1;;i++)
    if(!uids.contains(i))
      return i;
}

quint8 iModKinect::getNextARTID()
{
  quint8 value = 1;
  bool changed = true;
  while(changed)
  {
    changed = false;
    foreach(UserDataKinect udata, bodymatching)
    {
      if(udata.artid == value)
      {
        changed = true;
        value++;
      }
    }
  }
  return value;
}

QList<int> iModKinect::getUserIDs()
{
  QList<int> list;

  quint8 highest_art_id = 0;

  foreach(UserDataKinect udata, bodymatching)
    highest_art_id = qMax(highest_art_id, udata.artid);

  if(highest_art_id == 0)
    return list;

  for(int i = 0; i <= highest_art_id; i++)
  {
    foreach(UserDataKinect udata, bodymatching)
      if(!list.contains(udata.userid) && i == udata.artid)
        list.append(udata.userid);
  }

  return list;
}

KinectJoint iModKinect::combineKinects(int currentjoint, int userid)
{
  QMap<int, KinectJoint> merginglist;
  int desired_trackingstate = 2;

  // Build a list with all possible joints
  do
  {
    foreach(UserDataKinect udata, bodymatching)
    {
      if(udata.userid != userid)
        continue;

      if(kinectdata.keys().contains(QPair<int, quint64>(udata.kinectid, udata.bodyid)))
        foreach(KinectJoint joint, *kinectdata.value(QPair<int, quint64>(udata.kinectid, udata.bodyid))->jointdata)
          if(joint.type == currentjoint && joint.trackingstate == desired_trackingstate)
            merginglist.insert(udata.kinectid, joint);
    }
    desired_trackingstate--;
  } while(merginglist.isEmpty() && desired_trackingstate > 0);

  // This should never happen
  if(merginglist.count() == 0)
    return KinectJoint();

  // Transform all joints to root kinect system
  foreach(int key, merginglist.keys()) // keys = Kinectids
  {
    if(key != current_root_kinect)
    {
      if(!kinecttransitions.keys().contains(key))
      {
        return KinectJoint();
      }
      merginglist.insert(key, applyTransformation(kinecttransitions.value(key), merginglist.value(key)));
    }
  }

  // Merge all joints in the merginglist
  KinectJoint result;
  result.trackingstate = merginglist.values().last().trackingstate;
  result.type = merginglist.values().last().type;

  QMatrix4x4 resultmatrix;
  iModMathUtils* math = new iModMathUtils();
  math->resetAveraging();

  foreach(KinectJoint joint, merginglist.values())
  {
    //IF w is zero, its no rotation.... but .toRotationMatrix() can handle it
    QQuaternion jointrotation(joint.orientation.w, joint.orientation.x, joint.orientation.y, joint.orientation.z);
    QMatrix4x4 jointmatrix(jointrotation.toRotationMatrix());
    jointmatrix.setColumn(3, QVector4D(joint.position.x, joint.position.y, joint.position.z, 1));

    resultmatrix = math->averageTransitionMatrix(jointmatrix);
  }

  QQuaternion resultorientation = iModMathUtils::matrixToQuaternion(resultmatrix);

  result.position.x = resultmatrix.column(3).x();
  result.position.y = resultmatrix.column(3).y();
  result.position.z = resultmatrix.column(3).z();
  result.orientation.x = resultorientation.x();
  result.orientation.y = resultorientation.y();
  result.orientation.z = resultorientation.z();
  result.orientation.w = resultorientation.scalar();

  delete math;
  return result;
}

KinectJoint iModKinect::applyTransformation(QMatrix4x4 matrix, KinectJoint joint)
{
  QQuaternion jointrotation(joint.orientation.w, joint.orientation.x, joint.orientation.y, joint.orientation.z);
  QMatrix4x4 jointmatrix(jointrotation.toRotationMatrix());
  jointmatrix.setColumn(3, QVector4D(joint.position.x, joint.position.y, joint.position.z, 1));
  jointmatrix =  matrix * jointmatrix;
  jointrotation = iModMathUtils::matrixToQuaternion(jointmatrix);
  joint.position.x = jointmatrix.column(3).x();
  joint.position.y = jointmatrix.column(3).y();
  joint.position.z = jointmatrix.column(3).z();
  joint.orientation.y = jointrotation.y();
  joint.orientation.x = jointrotation.x();
  joint.orientation.z = jointrotation.z();
  joint.orientation.w = jointrotation.scalar();

  return joint;
}

bool iModKinect::kinectindexExists(QPair<int, quint64> index)
{
  return kinectdata.keys().contains(index);
}

_otd::_otd()
{
  for(int i = 0; i < 7; i++)
  {
    buffer.append(new iModRingBuffer(12));
    virtualtargets.append(QVector3D());
  }
}

_otd::~_otd()
{
  for(int i = 0; i < buffer.size(); i++)
    delete buffer[i];
}

void _otd::update(QList<QVector3D> data)
{
  if(data.size() != buffer.size())
    return;
  for(int i = 0; i < buffer.size(); i++)
  {
    buffer[i]->insert(data[i]);
    virtualtargets[i] = iModMathUtils::movingAveragePosition(buffer[i]);
  }

  artstring = "";
  // Create A.R.T. string
  for(int i = 0; i < virtualtargets.count(); i++)
  {
    virtualtargets[i] = virtualtargets[i] * 1000.0; // Set m to mm

    if(virtualtargets[i].z() > 10000) // This thing is invalid....
      virtualtargets[i] = QVector3D(virtualtargets[i].x(), virtualtargets[i].y(), lastvaliddepth);
    else
      lastvaliddepth = virtualtargets[i].z();

    artstring += QString("[%1 1.000]["
                         + QString::number(virtualtargets[i].x(), 'f', 4)
                         + " " + QString::number(virtualtargets[i].y(), 'f', 4)
                         + " " + QString::number(virtualtargets[i].z(), 'f', 4)
                         + " 0.000 0.000 0.000][1.0000 0.0000 0.0000 "
                         + "0.0000 1.0000 0.0000 "
                         + "0.0000 0.0000 1.0000] ")
                 .arg(i + 20);
  }

  ts = QDateTime::currentMSecsSinceEpoch();
}
