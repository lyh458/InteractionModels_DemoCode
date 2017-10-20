#include "imodnetwork.h"

#include <QtNetwork/QTcpSocket>
#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QTimer>

using namespace iModNetwork;

iModSkynet::iModSkynet(QObject *parent) :
  NetworkBase(parent),
  connectionsocket(NULL),
  reconnecttimer(NULL),
  controllerudpport(0)
{
  connectionsocket = new QTcpSocket(this);
  udpsocket = new QUdpSocket();
  qDebug() << "Listening for Demon:" << udpsocket->bind();
  
  connect(udpsocket, &QUdpSocket::readyRead, this, &iModSkynet::slotUDPIncomming);
}

iModSkynet::~iModSkynet()
{
  safeDelete(connectionsocket);
}

void iModSkynet::connectUR5(QString ip, int port)
{
  UR5Connection* ur5con = new UR5Connection();
  ur5con->set_robot_ip(ip);
  ur5con->set_has_robot_ip(true);
  ur5con->set_robot_port(port);
  ur5con->set_has_robot_port(true);
  sendPacketNotification(ur5con);
  delete ur5con;
}

bool iModSkynet::daemonControll(bool reset, bool forceroot, qint16 setrootkinect, bool& root, quint8& rootkinect)
{
  DaemonControll* dcon = new DaemonControll();

  if(reset)
  {
    dcon->set_reset(true);
    dcon->set_has_reset(true);
    sendPacketNotification(dcon);
    dcon->set_success(true);
  }
  else if(forceroot)
  {
    dcon->set_forceroot(true);
    dcon->set_has_forceroot(true);
    sendPacketNotification(dcon);
    dcon->set_success(true);
  }
  else if(setrootkinect >=0 )
  {
    dcon->set_setrootkinect(setrootkinect);
    dcon->set_has_setrootkinect(true);
    sendPacketNotification(dcon);
    dcon->set_success(true);
  }
  else
  {
    sendPacketSynchron(dcon);

    if(dcon->success())
    {
      if(dcon->has_rootclient()) root       = dcon->rootclient();
      if(dcon->has_rootkinect()) rootkinect = dcon->rootkinect();
    }
  }

  bool returnvalue = dcon->success();
  delete dcon;
  return returnvalue;
}

bool iModSkynet::kinectControllValues(QVector3D& position, QQuaternion& orientation, bool setvalues)
{
  KinectControllValues* kcv = new KinectControllValues();

  if(setvalues)
  {
    kcv->set_positionoffset(position);
    kcv->set_has_positionoffset(true);
    kcv->set_rotationoffset(orientation);
    kcv->set_has_rotationoffset(true);
    sendPacketNotification(kcv);
    kcv->set_success(true);
  }
  else
  {
    sendPacketSynchron(kcv);
    if(kcv->success())
    {
      if(kcv->has_positionoffset()) position    = kcv->positionoffset();
      if(kcv->has_rotationoffset()) orientation = kcv->rotationoffset();
    }
  }

  bool returnvalue = kcv->success();
  delete kcv;
  return returnvalue;
}

bool iModSkynet::databaseControllValues(QStringList& categorylist, QStringList& tablelist, QString& cur_categroy, QString& cur_table, bool load, QString loadfromcat)
{
  DatabaseControllValues* dcv = new DatabaseControllValues();
  if(load)
  {
    dcv->set_category(cur_categroy);
    dcv->set_has_category(true);
    dcv->set_table(cur_table);
    dcv->set_has_table(true);
    dcv->set_load(true);
    dcv->set_has_load(true);
    sendPacketNotification(dcv);
    delete dcv;
    return true;
  }

  if(loadfromcat != "")
  {
    dcv->set_loadfromcategory(loadfromcat);
    dcv->set_has_loadfromcategory(true);
  }

  sendPacketSynchron(dcv);

  if(dcv->success())
  {
    if(dcv->has_categorylist()) categorylist = dcv->categorylist();
    if(dcv->has_tablelist())    tablelist    = dcv->tablelist();
    if(dcv->has_category())     cur_categroy = dcv->category();
    if(dcv->has_table())        cur_table    = dcv->table();
  }

  bool returnvalue = dcv->success();
  delete dcv;
  return returnvalue;
}

bool iModSkynet::artControllValues(bool start, quint8& identifiedtargets, bool requesttargets)
{
  ARTControllValues* artcv = new ARTControllValues();
  if(requesttargets)
  {
    sendPacketSynchron(artcv);
    if(artcv->success())
    {
      if(artcv->has_currenttargets()) identifiedtargets = artcv->currenttargets();
    }
  }
  else
  {
    artcv->set_starttracking(start);
    artcv->set_has_starttracking(true);
    sendPacketNotification(artcv);
  }

  bool returnvalue = artcv->success();
  delete artcv;
  return returnvalue;
}

bool iModSkynet::databaseRecordingControll(bool& recording, quint8& replayfps, bool setvalues, QString category, QString table)
{
  DatabaseRecordingControll* drc = new DatabaseRecordingControll();
  if(table != "")
  {
    drc->set_recorde(false);
    drc->set_has_recorde(true);
    drc->set_writecategory(category);
    drc->set_has_writecategory(true);
    drc->set_writetable(table);
    drc->set_has_writetable(true);
    sendPacketNotification(drc);
    delete drc;
    return true;
  }
  else if(setvalues)
  {
    drc->set_recorde(recording);
    drc->set_has_recorde(true);
    drc->set_replayfps(replayfps);
    drc->set_has_replayfps(true);
    sendPacketNotification(drc);
    delete drc;
    return true;
  }
  else
  {
    sendPacketSynchron(drc);
    if(drc->success())
    {
      if(drc->has_recorde())   recording = drc->recorde();
      if(drc->has_replayfps()) replayfps = drc->replayfps();
    }
    bool returnvalue = drc->success();
    delete drc;
    return returnvalue;
  }
}

bool iModSkynet::inputMethode(char& value, bool setvalue)
{
  InputMethode* im = new InputMethode();
  if(setvalue)
  {
    im->set_methode(value);
    im->set_has_methode(true);
    sendPacketNotification(im);
    delete im;
    return true;
  }
  else
  {
    sendPacketSynchron(im);
    if(im->success())
      if(im->has_methode()) value = im->methode();

    bool returnvalue = im->success();
    delete im;
    return returnvalue;
  }
}

bool iModSkynet::robotControllValues(qreal& blending, qreal& speed, qreal& speedup, bool& enable, bool setvalues)
{
  RobotControllValues* rcv = new RobotControllValues();

  if(setvalues)
  {
    rcv->set_blending(blending);
    rcv->set_has_blending(true);
    rcv->set_maxspeed(speed);
    rcv->set_has_maxspeed(true);
    rcv->set_maxspeedup(speedup);
    rcv->set_has_maxspeedup(true);
    rcv->set_enablerobot(enable);
    rcv->set_has_enablerobot(true);
    sendPacketNotification(rcv);
    delete rcv;
    return true;
  }
  else
  {
    sendPacketSynchron(rcv);

    if(rcv->success())
    {
      if(rcv->has_blending()) blending = rcv->blending();
      if(rcv->has_maxspeed()) speed = rcv->maxspeed();
      if(rcv->has_maxspeedup()) speedup = rcv->maxspeedup();
      if(rcv->has_enablerobot()) enable = rcv->enablerobot();
    }

    bool returnvalue = rcv->success();
    delete rcv;
    return returnvalue;
  }
}

bool iModSkynet::kinectTrackingData(quint64 tid, quint8 targets, QByteArray data, quint8 kinectid)
{
  KinectTrackingData* ktd = new KinectTrackingData();
  ktd->set_bodyid(tid);
  ktd->set_has_bodyid(true);
  ktd->set_jointcount(targets);
  ktd->set_has_jointcount(true);
  ktd->set_trackingdata(data);
  ktd->set_has_trackingdata(true);
  ktd->set_kinectid(kinectid);
  ktd->set_has_kinectid(true);
  ktd->set_success(true);
  bool retv = sendPacketNotification(ktd);
  delete ktd;
  return retv;
}

bool iModSkynet::kinectObjectTracking(QByteArray data, quint8 kinectid)
{
  KinectObjectTracking* kot = new KinectObjectTracking();
  kot->set_data(data);
  kot->set_has_data(true);
  kot->set_kinectid(kinectid);
  kot->set_has_kinectid(true);
  bool retv = sendPacketNotification(kot);
  delete kot;
  return retv;
}

bool iModSkynet::robotOutputMode(char mode)
{
  UR5Output* rom = new UR5Output();
  rom->set_outputtype(mode);
  rom->set_has_outputtype(true);
  bool retv = sendPacketNotification(rom);
  delete rom;
  return retv;
}

bool iModSkynet::kinectRange(QList<int> _cl, qreal mi_r, qreal ma_r)
{
  QByteArray clients = serializeQList<int>(_cl);

  KinectRange* kr = new KinectRange();
  kr->set_clients(clients);
  kr->set_has_clients(true);
  kr->set_minrange(mi_r);
  kr->set_has_minrange(true);
  kr->set_maxrange(ma_r);
  kr->set_has_maxrange(true);
  bool retv = sendPacketNotification(kr);
  delete kr;
  return retv;
}

bool iModSkynet::connectGripper(QString ip)
{
  GripperControlValues* gcv = new GripperControlValues();
  gcv->set_ip(ip);
  gcv->set_has_ip(true);
  bool retv = sendPacketNotification(gcv);
  delete gcv;
  return retv;
}

bool iModSkynet::setGripperStatus(QByteArray data)
{
  GripperControlValues* gcv = new GripperControlValues();
  gcv->set_ip("");
  gcv->set_has_ip(false);
  gcv->set_data(data);
  gcv->set_has_data(true);
  gcv->set_setvalues(false);
  gcv->set_has_setvalues(true);
  bool retv = sendPacketNotification(gcv);
  delete gcv;
  return retv;
}

bool iModSkynet::setOpenCVValues(QVector3D cmin, QVector3D cmax, quint8 e, quint8 d, bool s)
{
  KinectOpenCV* kocv = new KinectOpenCV();
  kocv->set_colormax(cmax);
  kocv->set_has_colormax(true);
  kocv->set_colormin(cmin);
  kocv->set_has_colormin(true);
  kocv->set_delate(d);
  kocv->set_has_delate(true);
  kocv->set_erode(e);
  kocv->set_has_erode(true);
  kocv->set_showwindow(s);
  kocv->set_has_showwindow(true);
  bool retv = sendPacketNotification(kocv);
  delete kocv;
  return retv;
}

bool iModSkynet::slotCIsendTriadicObjectData(quint8 os, qreal pb, qreal cv)
{
  TriadicObjectData* tod = new TriadicObjectData();
  tod->set_closingvalue(cv);
  tod->set_has_closingvalue(cv);
  tod->set_ownership(os);
  tod->set_has_ownership(true);
  tod->set_probability(pb);
  tod->set_has_probability(true);
  bool retv = sendPacketNotification(tod);
  delete tod;
  return retv;
}

bool iModSkynet::setGripperAutomatic(char type)
{
  GripperAutomatic* ga = new GripperAutomatic();
  ga->set_type(type);
  ga->set_has_type(true);
  bool retv = sendPacketNotification(ga);
  delete ga;
  return retv;
}

bool iModSkynet::slotCIsendBlenderCommunication(char type, QByteArray qdata)
{
  BlenderCommunication* bc = new BlenderCommunication();
  bc->set_type(type);
  bc->set_has_type(true);
  bc->set_ur5pose(qdata);
  bc->set_has_ur5pose(true);
  bool retv = sendPacketNotification(bc);
  delete bc;
  return retv;
}

void iModSkynet::safeDelete(QObject *value)
{
  if(value)
    delete value;
  value = NULL;
}

void iModSkynet::slotDaemonDisconnected()
{
  if(reconnecttimer)
    return;

  reconnecttimer = new QTimer(this);
  connect(reconnecttimer, &QTimer::timeout, this, &iModSkynet::slotDaemonReconnect);
  reconnecttimer->setInterval(3000);
  reconnecttimer->start();
  slotDaemonReconnect(); // Fire now...
}

void iModSkynet::slotDaemonReconnect()
{
  QByteArray data = "REQIP";
  udpsocket->writeDatagram(data.data(), data.size(), QHostAddress::Broadcast, 3002);
}

void iModSkynet::recvAyn_DaemonControll(DaemonControll* packet, bool request)
{
  if(!request)
  {
    quint8 rootc = 99;
    quint8 rootk = 0;
    if(packet->has_rootclient()) rootc = packet->rootclient();
    if(packet->has_rootkinect()) rootk = packet->rootkinect();
    emit this->signalDaemonControll(rootc, rootk);
  }
}

void iModSkynet::recvAyn_BlenderRobotFPS(BlenderRobotFPS* packet, bool request)
{
  if(!request)
  {
    emit this->signalBlenderRobotFPS(packet->blender_fps(), packet->robot_fps());
  }
}

void iModSkynet::recvAyn_KinectRange(KinectRange* packet, bool request)
{
  if(!request)
  {
    emit this->signalKinectRange(packet->minrange(), packet->maxrange());
  }
}

void iModSkynet::recvAyn_GripperControlValues(GripperControlValues* packet, bool request)
{
  if(!request && packet->has_data())
    emit this->signalGripperControlValue(packet->data());
}

void iModSkynet::recvAyn_KinectOpenCV(KinectOpenCV* packet, bool request)
{
  if(!request)
  {
    emit this->signalKinectOpenCVValues(packet->colormin(), packet->colormax(), packet->erode(), packet->delate(), packet->showwindow());
  }
}

void iModSkynet::recvAyn_BlenderCommunication(BlenderCommunication* packet, bool request)
{
  if(!request)
  {
    if(packet->has_gripperstate())
      emit this->signalBlenderCommunicationGS(packet->gripperstate());
    else if(packet->has_ur5tcp())
      emit this->signalBlenderCommunicationRO(packet->ur5tcp());
  }
}

void iModSkynet::recvAyn_FTSensorValues(FTSensorValues* packet, bool request)
{
  if(!request)
  {
    if(packet->has_force() && packet->has_torque())
      emit this->signalFTSensorValues(packet->force(), packet->torque());
  }
}

void iModSkynet::recvAyn_RobotControllValues(RobotControllValues* packet, bool request)
{
  if(!request)
  {
    emit this->signalRobotControllValues(packet->blending(), packet->maxspeed(), packet->maxspeedup(), packet->enablerobot(), packet->robotoutputmode());
  }
}

void iModSkynet::recvAyn_InputMethode(InputMethode* packet, bool request)
{
  if(!request)
  {
    char value;
    if(packet->has_methode()) value = packet->methode();
    emit this->signalInputMethode(value);
  }
}

void iModSkynet::recvAyn_DaemonState(DaemonState* packet, bool request)
{
  if(!request)
  {
    emit this->signalDaemonState(packet->currentclients());
  }
}

void iModSkynet::recvAyn_DatabaseRecordingControll(DatabaseRecordingControll* packet, bool request)
{
  if(!request)
  {
    emit this->signalDatabaseRecordingCrontroll(packet->recorde(), packet->replayfps());
  }
}

void iModSkynet::recvAyn_KinectControllValues(KinectControllValues* packet, bool request)
{
  if(!request)
  {
    emit this->signalKinectControllValues(packet->positionoffset(), packet->rotationoffset());
  }
}

void iModSkynet::recvAyn_UR5Connection(UR5Connection* packet, bool request)
{
  if(!request)
  {
    emit this->signalUR5Connection(packet->connected());
  }
}

void iModSkynet::slotUDPIncomming()
{
  while(udpsocket->hasPendingDatagrams())
  {
    QByteArray data;
    data.resize(udpsocket->pendingDatagramSize());
    QHostAddress sender;
    quint16 port;
    udpsocket->readDatagram(data.data(), data.size(), &sender, &port);

    if(QString(data).startsWith("RESIP "))
    {
      int daemonport = QString(data).split(" ", QString::SkipEmptyParts)[1].toInt();
      if(connectionsocket->state() == QTcpSocket::ConnectedState)
      {
        qDebug() << "Multiple Daemons in network. Discarding second attempt.";
        qDebug() << "  Currently connected to:" << demonaddress;
        return;
      }
      connectionsocket->connectToHost(sender, daemonport);
      bool success = connectionsocket->waitForConnected(2500);
      if(success)
      {
        connect(connectionsocket, &QTcpSocket::disconnected, this, &iModSkynet::slotDaemonDisconnected);
        reconnecttimer->stop();
        delete reconnecttimer;
        reconnecttimer = NULL;
        demonaddress = sender;

        qDebug() << "Initializing iModSkynet:" << initialize(connectionsocket);
      }
      else
        qDebug() << "Cant connect daemon" << sender << port << " TIMEOUT";
    }
  }
}
