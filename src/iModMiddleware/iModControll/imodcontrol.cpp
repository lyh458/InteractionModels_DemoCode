#include "imodcontrol.h"

#include "imodnetwork.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtCore/QThread>

#ifdef WIN64
#include "imodkinectv2.h"
#include <QtCore/QThread>
#endif

iModControl::iModControl(QObject* parent) :
  QObject(parent),
  gstate(0)
{
  network = new iModSkynet(this);
  starting_from_blender = false;

#ifdef WIN64
  kinectthread = NULL;
  kinect = NULL;
#endif

  initializeConnections();
}

iModControl::iModControl(bool dummy) : gstate(4)
{
  starting_from_blender = dummy;
}

iModControl::~iModControl()
{
  delete network;
#ifdef WIN64
  if(kinect) delete kinect;
  if(kinectthread) delete kinectthread;
#endif
}

void iModControl::connectDaemon()
{
  emit this->signalTriggerDaemonConnect();
}

void iModControl::connectUR5(QString ip, int port)
{
  network->connectUR5(ip, port);
}

bool iModControl::resetDaemon()
{
  bool tmp;
  quint8 tmp2;
  return network->daemonControll(true, false, -1, tmp, tmp2);
}

bool iModControl::forceRoot()
{
  bool tmp;
  quint8 tmp2;
  return network->daemonControll(false, true, -1, tmp, tmp2);
}

bool iModControl::setRootKinect(quint8 kinectid)
{
  bool tmp;
  quint8 tmp2;
  return network->daemonControll(false, false, kinectid, tmp, tmp2);
}

bool iModControl::daemonControll(bool& root, quint8& rootkinect)
{
  return network->daemonControll(false, false, -1, root, rootkinect);
}

bool iModControl::setKinectOffsets(QVector3D position, QQuaternion orientation)
{
  return network->kinectControllValues(position, orientation, true);
}

bool iModControl::getKinectOffsets(QVector3D& position, QQuaternion& orientation)
{
  return network->kinectControllValues(position, orientation, false);
}

bool iModControl::getDatabaseCategorylist(QStringList& list)
{
  QStringList tmp;
  QString tmp2, tmp3;
  return network->databaseControllValues(list, tmp, tmp2, tmp3, false, "");
}

bool iModControl::getDatabaseTableList(QStringList& list, QString category)
{
  QStringList tmp;
  QString tmp2, tmp3;
  return network->databaseControllValues(tmp, list, tmp2, tmp3, false, category);
}

bool iModControl::startARTTracking(bool start)
{
  quint8 tmp;
  return network->artControllValues(start, tmp, false);
}

bool iModControl::startDatabaseRecording(bool start, quint8 rfps)
{
  return network->databaseRecordingControll(start, rfps, true, "", "");
}

bool iModControl::getDatabaseState(bool& recording, quint8& replayfps)
{
  return network->databaseRecordingControll(recording, replayfps, false, "", "");
}

bool iModControl::writeRecordingToDatabase(QString category, QString table)
{
  bool tmp;
  quint8 tmp2;
  return network->databaseRecordingControll(tmp, tmp2, false, category, table);
}

bool iModControl::setInputMethode(char value)
{
  return network->inputMethode(value, true);
}

bool iModControl::inputMethode(char& value)
{
  return network->inputMethode(value, false);
}

bool iModControl::setDatabaseReplayFPS(quint8 value)
{
  quint8 tmp;
  bool recording;
  getDatabaseState(recording, tmp);
  return network->databaseRecordingControll(recording, value, true, "", "");
}

bool iModControl::loadCurrentTable(QString cat, QString tab)
{
  QStringList tmp, tmp2;
  return network->databaseControllValues(tmp, tmp2, cat, tab, true, "");
}

bool iModControl::setRobotControllValues(qreal blending, qreal speed, qreal speedup, bool enable)
{
  return network->robotControllValues(blending, speed, speedup, enable, true);
}

bool iModControl::getRobotControllValues(qreal& blending, qreal& speed, qreal& speedup, bool& enable)
{
  return network->robotControllValues(blending, speed, speedup, enable, false);
}

bool iModControl::getIdentifiedTargets(quint8& targets)
{
  bool tmp = false;
  return network->artControllValues(tmp, targets, true);
}

bool iModControl::setUR5OutputMode(char mode)
{
  return network->robotOutputMode(mode);
}

bool iModControl::setKinectMinMaxRange(QList<int> _cl, qreal mi_r, qreal ma_r)
{
  return network->kinectRange(_cl, mi_r, ma_r);
}

void iModControl::startKinect(int id)
{
#ifdef WIN64
  qDebug() << "Starting KinectV2";
  kinect = new iModKinectV2(this, network);
  kinect->startKinect();
  kinect->slotSetKinectID(id, 0); // set it once...
  kinectthread = new QThread(this);
  kinect->moveToThread(kinectthread);
  connect(network, &iModSkynet::signalDaemonControll, kinect, &iModKinectV2::slotSetKinectID);
  connect(kinect, &iModKinectV2::signalSendKinectData, this, &iModControl::slotSendKinectData);
  connect(network, &iModSkynet::signalKinectRange, this, &iModControl::slotDEBUG_KinectRange);
  connect(kinectthread, &QThread::started, kinect, &iModKinectV2::update);
  kinectthread->start();
#else
  Q_UNUSED(id);
#endif
}

bool iModControl::connectGripper(QString ip)
{
  return network->connectGripper(ip);
}

bool iModControl::setGripperStatus(QByteArray data)
{
  return network->setGripperStatus(data);
}

bool iModControl::setOpenCVValues(QVector3D cmin, QVector3D cmax, quint8 e, quint8 d, bool s)
{
  return network->setOpenCVValues(cmin, cmax, e, d, s);
}

void iModControl::sendBlenderCommunication(char type, QByteArray qdata)
{
  if(starting_from_blender)
    emit this->signalCIsendBlenderCommunication(type, qdata);
  else
    qDebug() << "You are using this library wrong -> Read the documentation";
}

bool iModControl::sendTriadicObjectData(quint8 os, qreal pb, qreal cv)
{
  if(starting_from_blender)
    emit this->signalCIsendTriadicObjectData(os, pb, cv);
  else
    qDebug() << "You are using this library wrong -> Read the documentation";
  return true;
}

void iModControl::setGripperAutomatic(char type)
{
  network->setGripperAutomatic(type);
}

void iModControl::slotDEBUG_KinectRange(qreal min, qreal max)
{
#ifdef WIN64
  // Normally I would connect the signal to the slot in iModKinectV2, but it is not tirggered
  // NOT THREAD SAVE -> Hardwire
  kinect->slotKinectRange(min, max);
#else
  Q_UNUSED(min);
  Q_UNUSED(max);
#endif
}

void iModControl::slotSendKinectData(quint64 tid, quint8 jointcount, QByteArray data, quint8 kinectid)
{
  network->kinectTrackingData(tid, jointcount, data, kinectid);
}

void iModControl::slotFTSensorValues(QVector3D f, QVector3D t)
{
  emit this->signalFTSensorValues(f,t);
  this->ftsensor_f = f;
  this->ftsensor_t = t;
}

void iModControl::initializeConnections()
{
  connect(network, &iModSkynet::signalDaemonControll,             this,    &iModControl::slotDaemonControll            );
  connect(network, &iModSkynet::signalRobotControllValues,        this,    &iModControl::slotRobotControllValues       );
  connect(network, &iModSkynet::signalInputMethode,               this,    &iModControl::slotInputMethode              );
  connect(network, &iModSkynet::signalDaemonState,                this,    &iModControl::slotDaemonState               );
  connect(network, &iModSkynet::signalDatabaseRecordingCrontroll, this,    &iModControl::slotDatabaseRecordingCrontroll);
  connect(network, &iModSkynet::signalKinectControllValues,       this,    &iModControl::slotKinectControllValues      );
  connect(network, &iModSkynet::signalUR5Connection,              this,    &iModControl::slotUR5Connection             );
  connect(network, &iModSkynet::signalBlenderRobotFPS,            this,    &iModControl::slotBlenderRobotFPS           );
  connect(this,    &iModControl::signalTriggerDaemonConnect,      network, &iModSkynet::slotDaemonDisconnected         );
  connect(network, &iModSkynet::signalGripperControlValue,        this,    &iModControl::slotGripperControlValues      );
  connect(network, &iModSkynet::signalBlenderCommunication,       this,    &iModControl::slotBlenderCommunication      );
  connect(network, &iModSkynet::signalBlenderCommunicationGS,     this,    &iModControl::slotBlenderCommunicationGS    );
  connect(network, &iModSkynet::signalBlenderCommunicationRO,     this,    &iModControl::slotBlenderCommunicationRO    );
  connect(network, &iModSkynet::signalFTSensorValues,             this,    &iModControl::slotFTSensorValues            );
}

void iModControl::slotSendObjectTrackingData(QByteArray data)
{
#ifdef WIN64
  network->kinectObjectTracking(data, kinect->getKinectID());
#endif
}

void iModControl::slotCInterfaceStartup()
{
#ifndef WIN64
  if(starting_from_blender)
  {
    qDebug() << "Starting Qt internals for CInterface in thread" << QThread::currentThreadId();

    int argc = 1;
    char* dummy = "/home";

    network = new iModSkynet();

    connect(this, &iModControl::signalCIsendBlenderCommunication, network, &iModSkynet::slotCIsendBlenderCommunication);
    connect(this, &iModControl::signalCIsendTriadicObjectData,    network, &iModSkynet::slotCIsendTriadicObjectData   );

    this->initializeConnections();
    this->connectDaemon();

    QCoreApplication* app = new QCoreApplication(argc, &dummy);
    app->exec();
  }
  else
    qDebug() << "You are using this library wrong -> Read the documentation";
#endif
}
