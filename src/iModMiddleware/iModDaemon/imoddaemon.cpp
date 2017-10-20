#include "imoddaemon.h"

#include <QtNetwork/QUdpSocket>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "imodutile.h"
#include "imoddatabase.h"
#include "Robot/imodur5.h"
#include "imodcontrol_daemon.h"
#include "imodart.h"
#include "Robot/imodftsensor.h"
#include "Robot/imodgripper.h"

iModDaemon::iModDaemon(iModUtile* _utile, QObject *parent) :
  QObject(parent),
  utile(_utile)
{
  httpserver = new QTcpServer(this);
  qDebug() << "HTTP Status server listening on Port 8080:" << httpserver->listen(QHostAddress::Any, 8080);
  connect(httpserver, &QTcpServer::newConnection, this, &iModDaemon::slotNewHttpRequest);

  database = new iModDatabase(utile);

  ur5 = new iModUR5(database);
  ur5thread = new QThread();
  ur5->moveToThread(ur5thread);
  connect(ur5thread, &QThread::finished,
          ur5,       &QObject::deleteLater);
  ur5thread->start();
  connect(ur5, &iModUR5::signalRobotBlenderFPS, this, &iModDaemon::slotRobotBlenderFPS);

  kinect = new iModKinect(utile);
  kinectthread = new QThread();
  kinect->moveToThread(kinectthread);
  connect(kinectthread, &QThread::started,  kinect,       &iModKinect::slotInit);
  connect(kinectthread, &QThread::finished, kinect,       &iModKinect::deleteLater);
  connect(kinectthread, &QThread::finished, kinectthread, &QThread::deleteLater);
  kinectthread->start();

  connect(this, &iModDaemon::signalCalibrateKinects,  kinect, &iModKinect::slotCalibrateKinects);
  connect(this, &iModDaemon::signalDetectUsersKinect, kinect, &iModKinect::slotDetectUsers);

  art = new iModART(utile, database, kinect, this);
  //connect(this, &iModDaemon::signalUpdateKinectData, art, &iModART::slotUpdateKinectData);

  brodcastupd = new QUdpSocket();
  qDebug() << "Binding brodcastlistener on port" << utile->readSetting("iModMiddleware/ControllerDaemonBroadcast").toInt() << ":" << brodcastupd->bind(QHostAddress::Any, utile->readSetting("iModMiddleware/ControllerDaemonBroadcast").toInt());
  connect(brodcastupd, &QUdpSocket::readyRead, this, &iModDaemon::slotIncommingBrodcast);

  gripper = new iModGripper(this);

  ftsensor = new iModFTSensor();
  if(ftsensor->connectSensor())
  {
    ftsensorthread = new QThread();
    ftsensor->moveToThread(ftsensorthread);
    connect(ftsensorthread, &QThread::started,  ftsensor,       &iModFTSensor::slotStart);
    connect(ftsensorthread, &QThread::finished, ftsensor,       &iModFTSensor::deleteLater);
    connect(ftsensorthread, &QThread::finished, ftsensorthread, &QThread::deleteLater);
    connect(ftsensor, &iModFTSensor::signalFTSensorValues, gripper, &iModGripper::slotAutomaticGripperDriverFTSensor);
    ftsensorthread->start();
  }
}

iModDaemon::~iModDaemon()
{
  delete brodcastupd;
  delete server;
  delete utile;
  delete art;
  delete ur5;
  delete ur5thread;
  delete database;
  delete gripper;
}

void iModDaemon::startControll()
{
  server = new QTcpServer(this);
  qDebug() << "Controller TCP listening on port" << utile->readSetting("iModMiddleware/DaemonControllerPort").toUInt() << ":" <<
              server->listen(QHostAddress::Any, utile->readSetting("iModMiddleware/DaemonControllerPort").toUInt());
  connect(server, &QTcpServer::newConnection, this,   &iModDaemon::slotNewConnection);
}

void iModDaemon::slotClientDisconnected(iModControlDaemon* client)
{
  clientlist.removeOne(client);

  if(client == rootcontroler)
  {
    if(clientlist.size() > 0)
    {
      qDebug() << "Rootcontroller disconnected. Electing random slave to root.";
      rootcontroler = clientlist.at(0);
      rootcontroler->setClientID(0);
    }
    else
    {
      qDebug() << "Rootcontroller disconnected. Found no new controller";
    }
  }
  else
    qDebug() << "Slavecontroller disconnected";


  delete client;
  updateClientIDList();
}

void iModDaemon::slotSendNotificationToAll(iModNetwork::PacketBase* packet)
{
  foreach (iModControlDaemon* client, clientlist)
    client->sendNotificationAll(packet);
}

void iModDaemon::slotSendNotificationToClient(int cid, iModNetwork::PacketBase* packet)
{
  foreach (iModControlDaemon* client, clientlist)
    if(client->getClientID() == cid)
      client->sendNotificationAll(packet);
}

void iModDaemon::slotNewConnection()
{
  qDebug() << "New connection";
  QTcpSocket* newclient = server->nextPendingConnection();

  int id = QHostAddress(newclient->peerAddress().toIPv4Address()).toString().split(".", QString::SkipEmptyParts)[3].toInt();

  iModControlDaemon* controller = new iModControlDaemon(id, newclient, art, ur5, database, kinect, utile, gripper, ftsensor, this);
  connect(controller, &iModControlDaemon::signalDisconnect, this, &iModDaemon::slotClientDisconnected);
  connect(controller, &iModControlDaemon::signalDismissCurrentRoot, this, &iModDaemon::slotDismissCurrentRoot);
  connect(controller, &iModControlDaemon::signalSendNotificationAll, this, &iModDaemon::slotSendNotificationToAll);
  connect(controller, &iModControlDaemon::signalSendNotificationToClient, this, &iModDaemon::slotSendNotificationToClient);
  controller->init();

  if(clientlist.size() == 0)
  {
    rootcontroler = controller;
    rootcontroler->setClientID(0);
  }

  clientlist.append(controller);
  updateClientIDList();
}

void iModDaemon::slotDismissCurrentRoot(int newroot)
{
  foreach(iModControlDaemon* client, clientlist)
  {
    if(client->getClientID() == 0)
    {
      int id = client->getIP();
      qDebug() << "Former root client changed to it's IP" << id;
      client->setClientID(id);
    }
    if(client->getClientID() == newroot)
    {
      qDebug() << "Client" << newroot << "got root";
      client->setClientID(0);
    }
  }
  updateClientIDList();
}

void iModDaemon::slotIncommingBrodcast()
{
  while(brodcastupd->hasPendingDatagrams())
  {
    QByteArray data;
    data.resize(brodcastupd->pendingDatagramSize());
    QHostAddress sender;
    quint16 port;
    brodcastupd->readDatagram(data.data(), data.size(), &sender, &port);

    if(QString(data) == "REQIP")
    {
      qDebug() << "Brodcast IP-Request from" << sender.toString();
      QByteArray data = QString(QString("RESIP ") + utile->readSetting("iModMiddleware/DaemonControllerPort").toString()).toLocal8Bit();
      brodcastupd->writeDatagram(data.data(), data.size(), sender, port);
    }
  }
}

void iModDaemon::slotRobotBlenderFPS(int rfps, int bfps)
{
  iModNetwork::BlenderRobotFPS* brfps = new iModNetwork::BlenderRobotFPS();
  brfps->set_blender_fps(bfps);
  brfps->set_has_blender_fps(true);
  brfps->set_robot_fps(rfps);
  brfps->set_has_robot_fps(true);
  this->slotSendNotificationToAll(brfps);
  delete brfps;
}

void iModDaemon::slotNewHttpRequest()
{
  if(httpserver->hasPendingConnections())
  {
    QTcpSocket* httpsocket = httpserver->nextPendingConnection();
    httpsocket->waitForReadyRead();
    QString request = QString(httpsocket->readAll());
    if(request.startsWith("GET / HTTP"))
    {
      httpsocket->write(createStatusPage("HTTP/1.x 200 OK").toLatin1());
      httpsocket->waitForBytesWritten(2000);
    }
    else if(request.startsWith("GET /ck HTTP"))
    {
      httpsocket->write(createStatusPage("HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Type: text/html; charset=UTF-8").toLatin1());
      httpsocket->waitForBytesWritten(2000);
      emit this->signalCalibrateKinects();
    }
    else if(request.startsWith("GET /ud HTTP"))
    {
      httpsocket->write(createStatusPage("HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Type: text/html; charset=UTF-8").toLatin1());
      httpsocket->waitForBytesWritten(2000);
      emit this->signalDetectUsersKinect();
    }
    else if(request.startsWith("GET /ssr HTTP"))
    {
      httpsocket->write(createStatusPage("HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Type: text/html; charset=UTF-8").toLatin1());
      httpsocket->waitForBytesWritten(2000);
      art->toggleRecording();
    }
    httpsocket->close();
  }
}

int iModDaemon::readInt32(char* data, int offset)
{
  int32_t value;
  memcpy(&value, data + offset, sizeof(int32_t));
  return value;
}

void iModDaemon::updateClientIDList()
{
  clientidlist.clear();
  foreach(iModControlDaemon* c, clientlist)
    clientidlist.append(QString::number(c->getClientID()));

  iModNetwork::DaemonState* ds = new iModNetwork::DaemonState();
  ds->set_currentclients(clientidlist);
  ds->set_has_currentclients(true);
  slotSendNotificationToAll(ds);
}

QString iModDaemon::getStatusString()
{
  QString result;
  result += "<h4>Daemon</h4>";
  result += QString("Connected clients: %1<br>").arg(clientlist.count());
  return result;
}

QString iModDaemon::createStatusPage(QString httpstate)
{
  return
      QString( httpstate + "\r\nConnection: close\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
                           "<html><head><title>iModMiddleware Status</title></head><body>"
                           "%1"
                           "%2"
                           "%3"
                           "<script>setInterval(function(){window.location.reload();},1500);</script></body></html>")
      .arg(
        this->getStatusString(),
        art->getStatusString(),
        database->getStatusString().arg(art->isRecording()?"True":"False"));
}

