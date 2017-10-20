#include "imodcontrol_daemon.h"

#include <QtNetwork/QTcpServer>
#include <QCoreApplication>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include "Robot/imodrobot.h"
#include "imodutile.h"
#include "imodart.h"
#include "imoddatabase.h"
#include "imoddaemon.h"
#include "Robot/imodur5.h"
#include "Kinect/imodkinect.h"
#include "Robot/imodgripper.h"
#include "Robot/imodftsensor.h"

#include <QtCore/QDebug>

using namespace iModNetwork;

iModControlDaemon::iModControlDaemon(int id, QTcpSocket* _controller, iModART* _art, iModRobot* _ur5, iModDatabase* _db, iModKinect* _kn, iModUtile* _utile, iModGripper* _gr, iModFTSensor* _ft, QObject *parent) :
  NetworkBase(parent),
  controller(_controller),
  utile(_utile),
  art(_art),
  robot(_ur5),
  database(_db),
  kinect(_kn),
  gripper(_gr),
  ftsensor(_ft)
{
  initialize(controller);
  setClientID(id);

  // Inform the new one about some current stuff :)
  RobotControllValues* rcv = new RobotControllValues();
  qreal b, s, a;
  bool en;
  char om;
  robot->getUR5Propertys(&b, &s, &a, &en, &om);
  rcv->set_blending(b);
  rcv->set_has_blending(true);
  rcv->set_maxspeed(s);
  rcv->set_has_maxspeed(true);
  rcv->set_maxspeedup(a);
  rcv->set_has_maxspeedup(true);
  rcv->set_enablerobot(en);
  rcv->set_has_enablerobot(true);
  rcv->set_robotoutputmode(om);
  rcv->set_has_robotoutputmode(true);
  sendPacketNotification(rcv);

  InputMethode* im = new InputMethode();
  char methode = art->getInputMethode();
  im->set_methode(methode);
  im->set_has_methode(true);
  sendPacketNotification(im);

  connect(this, &iModControlDaemon::signalSetCurrentRootKinect, kinect, &iModKinect::slotSetCurrentRootKinect);
  connect(this, &iModControlDaemon::signalSetKinectOffsets    , kinect, &iModKinect::slotSetKinectOffsets);
}

iModControlDaemon::~iModControlDaemon()
{
  //if(ur5thread) delete ur5thread;
  //if(ur5)       delete ur5;
  //if(database)  delete database;
  //if(art)       delete art;
  //if(server)    delete server;
}

void iModControlDaemon::init()
{
  qDebug() << "Initializing new slave controller" << clientid;
  connect(controller, &QTcpSocket::disconnected,
          this,       &iModControlDaemon::slotControllerDisconnected);
  connect(this, &iModControlDaemon::signalUpdateKinectData,
          kinect,  &iModKinect::slotUpdateKinectData);
  connect(robot, &iModRobot::signalUR5ConnectionState,
          this,  &iModControlDaemon::slotUR5ConnectionState);
  connect(this, &iModControlDaemon::signalKinectObjectTracking,
          kinect, &iModKinect::slotKinectObjectTracking);
  connect(this, &iModControlDaemon::signalUR5MoveToInit,
          robot, &iModRobot::slotBlenderUR5MoveInit);
  connect(this, &iModControlDaemon::signalUR5Move,
          robot, &iModRobot::slotBlenderUR5Move);
  connect(gripper, &iModGripper::signalGripperState,
          this, &iModControlDaemon::slotGripperState);
  connect(robot, &iModRobot::singalUpdateRobotTCP,
          this, &iModControlDaemon::slotPublishRobotTCP);
  connect(ftsensor, &iModFTSensor::signalFTSensorValues,
          this, &iModControlDaemon::slotFTSensorValues);

  // Normally, this would only be for root, but Blender will never be the root client.... perhaps classify more than "root or not"
  connect(this, &iModControlDaemon::signalTriadicObjectData,
          gripper, &iModGripper::slotAutomaticGripperDriverMatlab);
}

void iModControlDaemon::setClientID(int value)
{
  if(value == 0)
  {
    clientid = 0;
    initroot();
  }
  else
  {
    clientid = value;
    removeroot();
  }

  DaemonControll* dcon = new DaemonControll();
  dcon->set_rootclient(clientid);
  dcon->set_has_rootclient(true);
  sendPacketNotification(dcon);
  delete dcon;
}

void iModControlDaemon::sendNotificationAll(PacketBase* packet)
{
  sendPacketNotification(packet);
}

int iModControlDaemon::getIP()
{
  return QHostAddress(controller->peerAddress().toIPv4Address()).toString().split(".", QString::SkipEmptyParts)[3].toInt();
}

void iModControlDaemon::initroot()
{
  qDebug() << "Initializing new root controller" << clientid;

  connect(this, &iModControlDaemon::signalUR5Connect,
          robot,  &iModRobot::slotEstablishConnection);
  connect(this, &iModControlDaemon::signalUR5FPSChanged,
          robot,  &iModRobot::slotChangeFPS);
  connect(this, &iModControlDaemon::signalStartRecording,
          robot,  &iModRobot::slotStartRecording);
  connect(this, &iModControlDaemon::signalStopRecording,
          robot,  &iModRobot::slotStopRecording);
  connect(this, &iModControlDaemon::signalWriteRecording,
          robot,  &iModRobot::slotWriteRecording);
  connect(this, &iModControlDaemon::signalSendFramesRobot,
          robot,  &iModRobot::slotSendFramesRobot);
  connect(this, &iModControlDaemon::signalGripperControlValues,
          gripper, &iModGripper::slotSetGripperStatus);
  connect(this, &iModControlDaemon::signalConnectGripper,
          gripper, &iModGripper::slotConnectGripper);
  connect(gripper, &iModGripper::signalGripperStatus,
          this,    &iModControlDaemon::slotGripperStatus);
  connect(this, &iModControlDaemon::signalGripperAutomatic,
          gripper, &iModGripper::slotGripperAutomatic);

  iModUR5* ur5ptr = dynamic_cast<iModUR5*>(robot);
  if(ur5ptr)
  {
    connect(this, &iModControlDaemon::signalUR5Output,
            ur5ptr, &iModUR5::slotUR5Output);
  }
}

void iModControlDaemon::removeroot()
{
  qDebug() << "Removing root controller";

  disconnect(this, &iModControlDaemon::signalUR5Connect,
             robot,  &iModRobot::slotEstablishConnection);
  disconnect(this, &iModControlDaemon::signalUR5FPSChanged,
             robot,  &iModRobot::slotChangeFPS);
  disconnect(this, &iModControlDaemon::signalStartRecording,
             robot,  &iModRobot::slotStartRecording);
  disconnect(this, &iModControlDaemon::signalStopRecording,
             robot,  &iModRobot::slotStopRecording);
  disconnect(this, &iModControlDaemon::signalWriteRecording,
             robot,  &iModRobot::slotWriteRecording);
  disconnect(this, &iModControlDaemon::signalSendFramesRobot,
             robot,  &iModRobot::slotSendFramesRobot);
  disconnect(this, &iModControlDaemon::signalGripperControlValues,
             gripper, &iModGripper::slotSetGripperStatus);
  disconnect(this, &iModControlDaemon::signalConnectGripper,
             gripper, &iModGripper::slotConnectGripper);

  iModUR5* ur5ptr = dynamic_cast<iModUR5*>(robot);
  if(ur5ptr)
  {
    disconnect(this, &iModControlDaemon::signalUR5Output,
               ur5ptr, &iModUR5::slotUR5Output);
  }
}

void iModControlDaemon::slotControllerDisconnected()
{
  emit this->signalDisconnect(this);
}

void iModControlDaemon::slotUR5ConnectionState(bool con)
{
  UR5Connection* ur5con = new UR5Connection();
  ur5con->set_connected(con);
  ur5con->set_has_connected(true);
  sendPacketNotification(ur5con);
  delete ur5con;
}

void iModControlDaemon::slotGripperStatus(QByteArray data)
{
  GripperControlValues* gcv = new GripperControlValues();
  gcv->set_data(data);
  gcv->set_has_data(true);
  sendPacketNotification(gcv);
  delete gcv;
}

void iModControlDaemon::slotFTSensorValues(QVector3D f, QVector3D t)
{
  FTSensorValues* ftsv = new FTSensorValues();
  ftsv->set_force(f);
  ftsv->set_has_force(true);
  ftsv->set_torque(t);
  ftsv->set_has_torque(true);
  sendPacketNotification(ftsv);
  delete ftsv;
}

void iModControlDaemon::slotGripperState(char gs)
{
  BlenderCommunication* bc = new BlenderCommunication();
  bc->set_gripperstate(gs);
  bc->set_has_gripperstate(true);
  sendNotificationAll(bc);
  bc->deleteLater();
}

void iModControlDaemon::slotPublishRobotTCP()
{
  BlenderCommunication* bc = new BlenderCommunication();
  bc->set_ur5tcp(robot->getLastTCP());
  bc->set_has_ur5tcp(true);
  sendNotificationAll(bc);
  bc->deleteLater();
}

void iModControlDaemon::recvSyn_DaemonControll(DaemonControll* packet, bool request)
{
  if(request)
  {
    packet->set_rootclient(clientid);
    packet->set_has_rootclient(true);
    packet->set_success(true);
  }
}

void iModControlDaemon::recvAyn_DaemonControll(DaemonControll* packet, bool request)
{
  if(!request)
  {
    if(packet->has_reset() && packet->reset())
    {
      QCoreApplication::exit(1000);
    }

    if(packet->has_forceroot() && packet->forceroot())
    {
      emit this->signalDismissCurrentRoot(this->clientid);
    }

    if(packet->has_setrootkinect())
    {
      emit this->signalSetCurrentRootKinect(packet->setrootkinect());
    }
  }
}

void iModControlDaemon::recvSyn_DatabaseControllValues(DatabaseControllValues* packet, bool request)
{
  if(request)
  {
    packet->set_categorylist(database->getCategorys());
    packet->set_has_categorylist(true);

    if(packet->has_loadfromcategory())
    {
      packet->set_tablelist(database->getTablesFromCategory(packet->loadfromcategory()));
      packet->set_has_tablelist(true);
    }

    packet->set_category(database->getCurrentCategory());
    packet->set_has_category(true);

    packet->set_table(database->getCurrentTable());
    packet->set_has_table(true);

    packet->set_success(true);
  }
}

void iModControlDaemon::recvAyn_DatabaseControllValues(DatabaseControllValues* packet, bool request)
{
  if(!request)
  {
    if(packet->has_load() && packet->load())
    {
      database->loadTable(packet->category(), packet->table());
    }
  }
}

void iModControlDaemon::recvSyn_RobotControllValues(RobotControllValues* packet, bool request)
{
  if(request)
  {
    double b, s, a;
    bool en;
    char om;
    robot->getUR5Propertys(&b, &s, &a, &en, &om);

    packet->set_blending(b);
    packet->set_has_blending(true);
    packet->set_maxspeed(s);
    packet->set_has_maxspeed(true);
    packet->set_maxspeedup(a);
    packet->set_has_maxspeedup(true);
    packet->set_enablerobot(en);
    packet->set_has_enablerobot(true);
    packet->set_robotoutputmode(om);
    packet->set_has_robotoutputmode(true);

    packet->set_success(true);
    //qDebug() << "Responded";
  }
}

void iModControlDaemon::recvAyn_RobotControllValues(RobotControllValues* packet, bool request)
{
  if(!request && clientid == 0)
  {
    robot->setUR5Propertys(packet->blending(), packet->maxspeed(), packet->maxspeedup());
    char om;
    robot->getUR5Propertys(NULL, NULL, NULL, NULL, &om);
    packet->set_robotoutputmode(om);
    packet->set_has_robotoutputmode(om);
    emit this->signalSendFramesRobot(packet->enablerobot()?0x01:0x00);
    emit this->signalSendNotificationAll(packet);
  }
}

void iModControlDaemon::recvSyn_InputMethode(InputMethode* packet, bool request)
{
  if(request)
  {
    packet->set_methode(art->getInputMethode());
    packet->set_has_methode(true);

    packet->set_success(true);
  }
}

void iModControlDaemon::recvAyn_InputMethode(InputMethode* packet, bool request)
{
  if(!request && clientid == 0)
  {
    if(packet->has_methode())
      art->changeInputMethode(packet->methode());
    emit this->signalSendNotificationAll(packet);
  }
}

void iModControlDaemon::recvAyn_DatabaseRecordingControll(DatabaseRecordingControll* packet, bool request)
{
  if(!request && clientid == 0)
  {
    if(packet->has_replayfps())
      art->setReplayFps(packet->replayfps());

    if(packet->has_recorde())
    {
      if(packet->recorde())
        art->startRecording();
      else
        art->stopRecording();
    }

    if(packet->has_writecategory() && packet->has_writetable())
    {
      art->writeRecording(packet->writecategory(), packet->writetable());
    }

    emit this->signalSendNotificationAll(packet);
  }
}

void iModControlDaemon::recvSyn_DatabaseRecordingControll(DatabaseRecordingControll* packet, bool request)
{
  if(request)
  {
    packet->set_recorde(art->isRecording());
    packet->set_has_recorde(true);
    packet->set_replayfps(art->getReplayFps());
    packet->set_has_replayfps(true);

    packet->set_success(true);
  }
}

void iModControlDaemon::recvAyn_KinectControllValues(KinectControllValues* packet, bool request)
{
  if(!request && clientid == 0)
  {
    if(packet->has_positionoffset() && packet->has_rotationoffset())
      emit this->signalSetKinectOffsets(packet->positionoffset(), packet->rotationoffset());

    emit this->signalSendNotificationAll(packet);
  }
}

void iModControlDaemon::recvAyn_ARTControllValues(ARTControllValues* packet, bool request)
{
  if(!request)
  {
    if(packet->has_starttracking())
    {
      if(packet->starttracking())
        art->startARTTracking();
      else
        art->stopARTTracking();
    }
  }
}

void iModControlDaemon::recvAyn_UR5Connection(UR5Connection* packet, bool request)
{
  if(!request)
  {
    emit this->signalUR5Connect(packet->robot_ip(), packet->robot_port());
  }
}

void iModControlDaemon::recvAyn_KinectTrackingData(KinectTrackingData* packet, bool request)
{
  if(!request)
  {
    //qDebug() << packet->jointcount() << packet->trackingdata().size() << packet->kinectid();
    emit this->signalUpdateKinectData(packet->bodyid(), packet->jointcount(), packet->trackingdata(), clientid);
  }
}

void iModControlDaemon::recvAyn_UR5Output(UR5Output* packet, bool request)
{
  if(!request)
  {
    emit this->signalUR5Output(packet->outputtype());

    RobotControllValues* packet_bc = new RobotControllValues();
    double b, s, a;
    bool en;
    char om;
    robot->getUR5Propertys(&b, &s, &a, &en, &om);
    packet_bc->set_blending(b);
    packet_bc->set_has_blending(true);
    packet_bc->set_maxspeed(s);
    packet_bc->set_has_maxspeed(true);
    packet_bc->set_maxspeedup(a);
    packet_bc->set_has_maxspeedup(true);
    packet_bc->set_enablerobot(en);
    packet_bc->set_has_enablerobot(true);
    packet_bc->set_robotoutputmode(packet->outputtype());
    packet_bc->set_has_robotoutputmode(true);
    emit this->signalSendNotificationAll(packet_bc);
    packet_bc->deleteLater();
  }
}

void iModControlDaemon::recvAyn_KinectRange(iModNetwork::KinectRange* packet, bool request)
{
  if(!request)
  {
    QList<int> clients = deserializeQList<int>(packet->clients());
    KinectRange* kr = new KinectRange();
    kr->set_minrange(packet->minrange());
    kr->set_has_minrange(true);
    kr->set_maxrange(packet->maxrange());
    kr->set_has_maxrange(true);
    kr->set_has_clients(false);

    for(int i = 0; i < clients.count(); i++)
      emit this->signalSendNotificationToClient(clients[i], kr);
    kr->deleteLater();
  }
}

void iModControlDaemon::recvAyn_GripperControlValues(GripperControlValues* packet, bool request)
{
  if(!request)
  {
    if(packet->ip() == "")
      emit this->signalGripperControlValues(packet->data());
    else
      emit this->signalConnectGripper(packet->ip());
  }
}

void iModControlDaemon::recvAyn_KinectObjectTracking(KinectObjectTracking* packet, bool request)
{
  if(!request)
  {
    if(packet->has_data())
      emit this->signalKinectObjectTracking(packet->data(), clientid);
    // DEBUGGING
    //QList<QVector3D> tmp = deserializeQList<QVector3D>(packet->data());
    //qDebug() << tmp;
  }
}

void iModControlDaemon::recvAyn_KinectOpenCV(KinectOpenCV* packet, bool request)
{
  if(!request)
  {
    emit this->signalSendNotificationAll(packet);
  }
}

void iModControlDaemon::recvAyn_BlenderCommunication(BlenderCommunication* packet, bool request)
{
  if(!request)
  {
    if(packet->has_type() && packet->has_ur5pose() && packet->type() == 1)
      emit this->signalUR5Move(packet->ur5pose());
    if(packet->has_type() && packet->has_ur5pose() && packet->type() == 2)
      emit this->signalUR5MoveToInit(packet->ur5pose());
  }
}

void iModControlDaemon::recvAyn_TriadicObjectData(TriadicObjectData* packet, bool request)
{
  if(!request)
  {
    emit this->signalTriadicObjectData(packet->ownership(), packet->probability(), packet->closingvalue());
  }
}

void iModControlDaemon::recvAyn_GripperAutomatic(GripperAutomatic* packet, bool request)
{
  if(!request)
  {
    if(packet->has_type())
      emit this->signalGripperAutomatic(packet->type());
  }
}
