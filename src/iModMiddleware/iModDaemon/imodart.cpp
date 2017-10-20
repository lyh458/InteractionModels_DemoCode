#include "imodart.h"

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtCore/QDebug>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>
#include <qmath.h>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QUuid>

#ifdef WIN64
#include <cstdint>
#endif

#include "imodutile.h"
#include "imoddatabase.h"
#include "imodmathutils.h"
#include "imodringbuffer.h"
#include "imodartclient.h"

iModART::iModART(iModUtile* _utile, iModDatabase* _db, iModKinect* _kn, QObject *parent) :
  QObject(parent),
  artincomming(NULL),
  artserver(NULL),
  realartserver(NULL),
  utile(_utile),
  framecounter(0),
  artasmiddleware(true),
  artaskinect(false),
  artasdatabase(false),
  database(_db),
  recordactive(false),
  replayfps(60),
  kinect(_kn)
{
  artserver = new QTcpServer(this);
  qDebug() << "Bind ART-Simulated TCP Listen"
           << utile->readSetting("iModMiddleware/ARTSimulatedTCPPort").toUInt()
           << artserver->listen(QHostAddress::Any, utile->readSetting("iModMiddleware/ARTSimulatedTCPPort").toInt());
  connect(artserver, &QTcpServer::newConnection,
          this,      &iModART::slotNewClientConnected);

  artincomming = new QUdpSocket(this);
  qDebug() << "Bind ART-Server UDP Listen"
           << utile->readSetting("iModMiddleware/ARTServerUDPPort").toUInt()
           << artincomming->bind(QHostAddress::Any, utile->readSetting("iModMiddleware/ARTServerUDPPort").toUInt());
  connect(artincomming, &QUdpSocket::readyRead,
          this,         &iModART::slotRecvTrackingdata);
  qDebug() << "ART Listening...";

  timerdatabase = new QTimer();
  timerdatabase->setInterval(1000.0/replayfps);
  connect(timerdatabase, &QTimer::timeout, this, &iModART::slotTimerDatabase);

  timerkinect = new QTimer();
  timerkinect->setInterval(1000.0/30); // use it with 30 FPS -> Hardwarekinects do too
  connect(timerkinect, &QTimer::timeout, this, &iModART::slotTimerKinect);

  changeInputMethode(0x00);
  connect(this, &iModART::signalGenerateKinectFrame, kinect, &iModKinect::slotGenerateNextFrame);
  connect(this, &iModART::signalBuildARTMatching,    kinect, &iModKinect::slotBuildARTMatching);

  /*
  dumpfileART = new QFile("DumpfileART.csv");
  if(!dumpfileART->open(QIODevice::ReadWrite))
  {
    delete dumpfileART;
    dumpfileART = NULL;
  }

  dumpfileDualKinect = new QFile("DumpfileDualKinect.csv");
  if(!dumpfileDualKinect->open(QIODevice::ReadWrite))
  {
    delete dumpfileDualKinect;
    dumpfileDualKinect = NULL;
  }

  dumpfileSingleKinect = new QFile("DumpfileSingleKinect.csv");
  if(!dumpfileSingleKinect->open(QIODevice::ReadWrite))
  {
    delete dumpfileSingleKinect;
    dumpfileSingleKinect = NULL;
  }
  */
}

iModART::~iModART()
{
  foreach(iModARTClient* c, clientlist)
    delete c;
  clientlist.clear();

  if(artincomming)
  {
    artincomming->close();
    delete artincomming;
  }
  if(artserver)
  {
    artserver->close();
    delete artserver;
  }
  if(realartserver)
  {
    realartserver->close();
    delete realartserver;
  }
  /*
  if(dumpfileART)
  {
    dumpfileART->flush();
    dumpfileART->close();
    delete dumpfileART;
  }

  if(dumpfileSingleKinect)
  {
    dumpfileSingleKinect->flush();
    dumpfileSingleKinect->close();
    delete dumpfileSingleKinect;
  }

  if(dumpfileDualKinect)
  {
    dumpfileDualKinect->flush();
    dumpfileDualKinect->close();
    delete dumpfileDualKinect;
  }
*/
}

void iModART::slotRecvTrackingdata()
{
  while (artincomming->hasPendingDatagrams())
  {
    QByteArray datagram;
    datagram.resize(artincomming->pendingDatagramSize());
    artincomming->readDatagram(datagram.data(), datagram.size());

    // processing here
    /*
    int32_t targets = 0;
    foreach(QString part, QString(datagram).split("\n", QString::SkipEmptyParts))
      if(part.startsWith("6d "))
      {
        targets = QString(part.mid(3, (part.indexOf(" ",3)==-1?part.count():part.indexOf(" ", 3)) - 3)).toInt();
        emit this->signalSendData(0x16, QByteArray((char*)(&targets), sizeof(int32_t)));
      }
    */
    /*
    QStringList partlist = QString(datagram).split("1.000][", QString::SkipEmptyParts);
    partlist.removeAt(0);
    QString totalstring = QString::number(QDateTime::currentMSecsSinceEpoch()) + ",";
    foreach(QString s, partlist)
    {
      QStringList values = s.split(" ", QString::SkipEmptyParts);
      for(int i = 0; i < 3; i++)
        totalstring += values[i] + ",";
    }
    totalstring = totalstring.left(totalstring.size() - 1) + "\n";
    if(dumpfileART)
      dumpfileART->write(totalstring.toLocal8Bit());
*/
    // 1. Write to database
    if(recordactive && artasmiddleware)
      recordinglist.append(QString(datagram));

    // 2. Act as middleware
    if(artasmiddleware)
      foreach(iModARTClient* client, clientlist)
        client->sendData(&datagram);
  }
}

void iModART::slotArtControllIncomming()
{
  while(realartserver->bytesAvailable())
  {
    qDebug() << QString(realartserver->readAll());
  }
}

void iModART::slotNewClientConnected()
{
  iModARTClient* client = new iModARTClient(artserver->nextPendingConnection(),
                                            utile->readSetting("iModMiddleware/ARTSimulatedUDPPort").toInt());
  clientlist.append(client);
}

void iModART::sendKinectFrameToClients(QString frame)
{
  framecounter++;
  frame = "fr " + QString::number(framecounter) + "\r\n" + frame;
  QByteArray data = frame.toUtf8();

  if(recordactive)
    recordinglist.append(QString(data));

  //qDebug() << frame;
  foreach(iModARTClient* client, clientlist)
    client->sendData(&data);
}

QString iModART::getStatusString()
{
  QString result;
  if(artaskinect)
  {
    result += "<h4>Kinect</h4>";
    result += QString("Current root kinect id: %1<br>").arg(kinect->getCurrentRootKinect());
    result += QString("Number of data streams: %1 streams<br>").arg(kinect->getNumberOfStreams());
    result += "<a href=\"ck\">Calibrate Kinects</a><br>";
    result += QString("Callibration running: %1 <br>").arg(kinect->isCallibrating()?"In Progress...":"No");
    result += QString("Calculated transitions: %1<br>").arg(kinect->getNumberOfTransitions());
    result += QString("Moving average size: %1 frames<br>").arg(kinect->getMASize());
    result += "<a href=\"ud\">Detect users</a><br>";
    result += QString("Number of Users: %1<br>").arg(kinect->getUsernumber());
    result += kinect->getPerformanceData();
    //result += QString("Current Accuracy: %1").arg(kinect->getCurrentAccuracy()==-1?"undefined":QString::number(kinect->getCurrentAccuracy(), 'f', 4));
  }

  result += "<h4>Middleware</h4>";
  result += QString("Input methode: %1<br>").arg(artasdatabase?"Database":artaskinect?"Kinect":"Piping real ART");
  if(artaskinect)
  {
    result += QString("Frametime: <%1 ms<br>").arg(kinect->getLastFrametime()+1);
  }
  return result;
}

void iModART::connectARTServer()
{
  realartserver = new QTcpSocket();
  realartserver->connectToHost(utile->readSetting("iModMiddleware/ARTServerAdress").toString(),
                               utile->readSetting("iModMiddleware/ARTServerTCPPort").toInt());
  connect(realartserver, &QTcpSocket::readyRead, this, &iModART::slotArtControllIncomming);
  qDebug() << "Connected ARTServer" << utile->readSetting("iModMiddleware/ARTServerAdress").toString() << utile->readSetting("iModMiddleware/ARTServerTCPPort").toInt() << realartserver->waitForConnected(5000);
}

void iModART::startARTTracking()
{
  if(!realartserver)
    connectARTServer();
  realartserver->write(QString("dtrack2 tracking start").toUtf8()); // fix this...
}

void iModART::stopARTTracking()
{
  if(realartserver)
  {
    realartserver->write(QString("dtrack2 tracking stop").toUtf8()); // fix this...
    realartserver->disconnectFromHost();
    delete realartserver;
    realartserver = NULL;
  }
}

void iModART::changeInputMethode(char value)
{
  // apply the stuff
  artasdatabase = false;
  artasmiddleware = false;
  artaskinect = false;
  timerdatabase->stop();
  timerkinect->stop();

  if(value == 0x00)
    artasmiddleware = true;
  else if(value == 0x01)
  {
    emit this->signalBuildARTMatching();
    artaskinect = true;
    timerkinect->start();
  }
  else if(value == 0x02)
  {
    artasdatabase = true;
    timerdatabase->start();
  }
  else
    artasmiddleware = true;

  // stop recordings
  recordactive = false;
  recordinglist.clear();
}

char iModART::getInputMethode()
{
  if(artasmiddleware)
    return 0x00;
  if(artaskinect)
    return 0x01;
  if(artasdatabase)
    return 0x02;
  return 0x03;
}

void iModART::slotTimerDatabase()
{
  framecounter++;
  QString frame = database->getNextFrame();
  if(frame != "")
  {
    frame = QString(frame).arg(QString::number(framecounter));

    QByteArray data = frame.toUtf8();

    if(recordactive)
      recordinglist.append(QString(data));

    foreach(iModARTClient* client, clientlist)
      client->sendData(&data);
  }
}

void iModART::slotTimerKinect()
{
  if(artaskinect)
  {
    emit this->signalGenerateKinectFrame();
    QString frame = kinect->getCurrentFrame();
    if(frame != "")
      sendKinectFrameToClients(frame);
  }
}

void iModART::startRecording()
{
  recordactive = true;
  recordinglist.clear();
}

void iModART::writeRecording(QString category, QString name)
{
  recordactive = false;
  database->writeRecording(category, name, recordinglist, true);
}

void iModART::stopRecording()
{
  recordactive = false;
}

void iModART::toggleRecording()
{
  recordactive = recordactive?false:true;
  if(recordactive)
    recordinglist.clear();
  else
  {
    QDateTime timestamp(QDateTime::currentDateTime());
    writeRecording("Kinect_Test", "Recording_" + timestamp.toString(Qt::SystemLocaleLongDate));
  }
}

void iModART::setReplayFps(int _fps)
{
  replayfps = _fps;
  timerdatabase->setInterval(1000.0/replayfps);
}
