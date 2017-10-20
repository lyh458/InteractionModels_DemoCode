#include "artnetwork.h"

#include <QtCore/QThread>
#include <QtCore/QDateTime>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtCore/qmath.h>

#include "artnetworkclient.h"
#include "artconverter.h"
#include <QtCore/QDebug>

ARTNetwork::ARTNetwork(int tcpport, int udpdstport, bool kinectsused, QObject *parent) :
  QTcpServer(parent),
  tcpport(tcpport),
  udpport(udpdstport),
//  udpsocket(new QUdpSocket()),
  framecounter(0),
  oculuscliente(""),
  oculusfilter(""),
  calibrateoculus(false),
  artframecounter(0),
  oculusdstport(33888),
  defaultheadtarget(-1),
  mergekinecttracking(false),
  headtargetforkinect(""),
  udpoculusclient(NULL),
  kinectinuse(kinectsused)
{
  slotResetOculusCalibration();
  oculusfilter.clear();
}

void ARTNetwork::initNetwork()
{  
  if(!this->listen(QHostAddress::Any, tcpport))
  {
    emit this->signalDisplayLogMessage("Cant start TCP server");
    return;
  }
  else
    emit this->signalDisplayLogMessage("TCP running");
}

void ARTNetwork::incomingConnection(qintptr socketDescriptor)
{
  ARTNetworkClient* newclient = new ARTNetworkClient(socketDescriptor);
  QThread* clientthread = new QThread();
  newclient->moveToThread(clientthread);
  connect(newclient, &ARTNetworkClient::signalDisplayLogMessage,
          this,      &ARTNetwork::slotDisplayLogMessage);
  connect(clientthread, &QThread::finished,
          newclient,    &QObject::deleteLater);
  connect(clientthread, &QThread::started,
          newclient,    &ARTNetworkClient::runClient);
  connect(newclient, &ARTNetworkClient::signalAddClient,
          this,      &ARTNetwork::slotAddClient);
  connect(newclient, &ARTNetworkClient::signalRemoveClient,
          this,      &ARTNetwork::slotRemoveClient);
  clientthread->start();
}

void ARTNetwork::runARTNetwork()
{
  udpsocket = new QUdpSocket();
  initNetwork();
}

void ARTNetwork::slotAddClient(int key, QString host)
{
  if(key < 0)
    oculuscliente = host;
  else
  {
    QMutexLocker locker(&listlock);
    clientlist.insert(key, host);
    emit this->signalSetConnectetClientsNumber(clientlist.count());
  }
}

void ARTNetwork::slotRemoveClient(int key)
{
  QMutexLocker locker(&listlock);
  clientlist.remove(key);
  emit this->signalSetConnectetClientsNumber(clientlist.count());
}

void ARTNetwork::slotSendARTData(QString data)
{
  if(data == "")
    return;

  data.replace("][","] [");
  while(data.right(1) != "]")
    data = data.left(data.length() - 1);

  QMutexLocker locker(&listlock);
  if(!kinectinuse)
  {
    foreach(QString host, clientlist)
    {
      QString frame = "fr " + QString::number(framecounter) + "\n";
      udpsocket->writeDatagram((frame+data).toUtf8(), QHostAddress(host), udpport);
    }
  }
  else
  {
    int targetpos = data.indexOf("[" + QString::number(defaultheadtarget) + " 1]");
    if(targetpos > 0)
    {
      int rotationpos = data.indexOf("[", targetpos + 10);
      if(headtargetforkinect != "")
        data.replace(rotationpos + 1, data.indexOf("]", rotationpos + 1) - rotationpos + 1, headtargetforkinect.split("] [")[2] + " ");

      foreach(QString host, clientlist)
      {
       QString frame = "fr " + QString::number(framecounter) + "\n";
       udpsocket->writeDatagram((frame+data).toUtf8(), QHostAddress(host), udpport);
      }
    }
    else
      emit this->signalDisplayLogMessage("Headtarget missmatch... cant compute headtarget!");
  }
  if(oculuscliente != "")
  {
    // Split for targets:
    data = data.right(data.length() - data.indexOf("[") - 1);
    data = data.left(data.length() - 1);
    QStringList rawtargets = data.split("] [", QString::SkipEmptyParts);

    Target headtarget;
    for(int i = 0; i < rawtargets.length(); i++)
    {
      if(i%3 == 0)
      {
        int id = rawtargets[i].split(" ", QString::SkipEmptyParts)[0].toInt();
        if(id != defaultheadtarget)
        {
          i += 2;
          continue;
        }
        else
          headtarget.id = id;
      }
      if(i%3 == 1)
      {
        QVector4D position;
        position.setX(rawtargets[i].split(" ", QString::SkipEmptyParts)[0].toDouble());
        position.setY(rawtargets[i].split(" ", QString::SkipEmptyParts)[1].toDouble());
        position.setZ(rawtargets[i].split(" ", QString::SkipEmptyParts)[2].toDouble());
        position.setW(1.0);

        headtarget.data.setColumn(3, position);
      }
      if(i%3 == 2)
      {
        QVector4D column;
        column.setW(0);
        column.setX(rawtargets[i].split(" ", QString::SkipEmptyParts)[0].toDouble());
        column.setY(rawtargets[i].split(" ", QString::SkipEmptyParts)[1].toDouble());
        column.setZ(rawtargets[i].split(" ", QString::SkipEmptyParts)[2].toDouble());
        headtarget.data.setColumn(0, column);

        column.setX(rawtargets[i].split(" ", QString::SkipEmptyParts)[3].toDouble());
        column.setY(rawtargets[i].split(" ", QString::SkipEmptyParts)[4].toDouble());
        column.setZ(rawtargets[i].split(" ", QString::SkipEmptyParts)[5].toDouble());
        headtarget.data.setColumn(1, column);

        column.setX(rawtargets[i].split(" ", QString::SkipEmptyParts)[6].toDouble());
        column.setY(rawtargets[i].split(" ", QString::SkipEmptyParts)[7].toDouble());
        column.setZ(rawtargets[i].split(" ", QString::SkipEmptyParts)[8].toDouble());
        headtarget.data.setColumn(2, column);
      }
    }

    // dooing my stuff here....
    if(calibrateoculus)
    {
      calibrateoculus = false;

      slotResetOculusCalibration();
      calibratedframe = headtarget.data;

      if(kinectinuse)
        emit this->signalDisplayLogMessage("Calibration for Kinect will be ignored!");
      calibrationmatrix.rotate(QQuaternion(0.99584, QVector3D(-0.0091, 0.0675, -0.00522)));
      calibrationmatrix.setColumn(3, QVector4D(697.866, -403.649, -862.189 , 1.0));
      oculusdynamicoffset = calibrationmatrix * calibratedframe.inverted();

      emit this->signalDisplayLogMessage("Calibration successful");
    }

    // use calibration:
    if(!kinectinuse)
    {

      // Position
      QVector4D position;
      position.setX(calibrationmatrix(0,3) + (headtarget.data(1,3) - calibratedframe(1,3)));
      position.setY(calibrationmatrix(1,3) + (headtarget.data(2,3) - calibratedframe(2,3))*-1);
      position.setZ(calibrationmatrix(2,3) + (headtarget.data(0,3) - calibratedframe(0,3)));
      position.setW(1.0);

      // Rotation
      headtarget.data = calibratedframe.inverted() * headtarget.data;

      // Reset Position
      headtarget.data.setColumn(3, position);
    }

    // prepare data
    if(kinectinuse) // y, z , x
      data = QString::number(headtarget.data(1, 3) - 40 , 'f', 5) + " " + QString::number(-headtarget.data(2, 3) + 1270, 'f', 5) + " " + QString::number(headtarget.data(0, 3) + 100, 'f', 5);
    else
      data = QString::number(headtarget.data(0, 3), 'f', 5) + " " + QString::number(headtarget.data(1, 3), 'f', 5) + " " + QString::number(headtarget.data(2, 3), 'f', 5);

    QQuaternion rotation = ARTConverter::matrixToQuaternion(headtarget.data);

    rotation.setScalar(-rotation.scalar());
    data += " " + QString::number(rotation.scalar(), 'f', 5) +
            " " + QString::number(rotation.x(), 'f', 5) +
            " " + QString::number(rotation.y(), 'f', 5) +
            " " + QString::number(rotation.z(), 'f', 5);

    //qDebug() << data;
    if(oculuscliente.contains(":"))
      udpsocket->writeDatagram((data).toUtf8(), QHostAddress(oculuscliente.split(":")[0]), oculuscliente.split(":")[1].toInt());
    else
      udpsocket->writeDatagram((data).toUtf8(), QHostAddress(oculuscliente), oculusdstport);
  }
  framecounter++;
}

void ARTNetwork::slotResetOculusCalibration()
{
  oculusdynamicoffset = QMatrix4x4();
  calibrationmatrix = QMatrix4x4();
}

void ARTNetwork::slotConnectARTServer(int udpmwport, int tcpmwport, QString tcphost)
{
  udpartclient = new QUdpSocket();
  if(!udpartclient->bind(QHostAddress::Any, udpmwport))
    qDebug() << "Cant bind UDP Socket for ARTData on port" << udpmwport;

  connect(udpartclient, &QUdpSocket::readyRead,
          this,         &ARTNetwork::slotReadARTDatagram);

  tcpartclient = new QTcpSocket();
  connect(tcpartclient, &QTcpSocket::readyRead,
          this,         &ARTNetwork::slotReadARTControllMessage);
  tcpartclient->connectToHost(tcphost, tcpmwport);
  bool success = tcpartclient->waitForConnected();
  if(!success)
  {
    emit this->signalDisplayLogMessage("Error connecting to ARTServer");
    return;
  }

  emit this->signalDisplayLogMessage("Connected to ARTServer");
  tcpartclient->write(QString("dtrack2 tracking start").toLocal8Bit());

  success = tcpartclient->waitForBytesWritten();
  emit this->signalDisplayLogMessage(success?"Started requesting data":"Cant start tracking!");
}

void ARTNetwork::slotReadARTDatagram()
{
  while (udpartclient->hasPendingDatagrams()) {

    artframecounter++;
    if(QDateTime::currentDateTimeUtc().toTime_t() >= lasttimestemp + 1)
    {
      emit this->signalArtFpsUpdate(artframecounter);
      artframecounter = 0;
      lasttimestemp = QDateTime::currentDateTimeUtc().toTime_t();
    }

    QByteArray datagram;
    datagram.resize(udpartclient->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;

    udpartclient->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

    // Daten wieder verteilen, wenn nicht null
    if(!QString(datagram).startsWith("6d 0"))
    {
      QString data = QString(datagram).split("\n")[1];
      while(data.indexOf(" 1.000]") != -1)
        data.replace(data.indexOf(" 1.000]"), 6, " 1");
      slotSendARTData(data);
    }
  }
}

void ARTNetwork::slotReadOculusTracking(int port, bool merge)
{
  if(udpoculusclient == NULL)
  {
    udpoculusclient = new QUdpSocket();
    udpoculusclient->bind(QHostAddress::Any, port);

    connect(udpoculusclient, &QUdpSocket::readyRead,
            this,            &ARTNetwork::slotReadOculusDatagram);
    emit this->signalDisplayLogMessage("Started oculus data socket");
  }
  mergekinecttracking = merge;
}

void ARTNetwork::slotReadOculusDatagram()
{
  while (udpoculusclient->hasPendingDatagrams()) {
    QByteArray datagram;
    datagram.resize(udpoculusclient->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;

    udpoculusclient->readDatagram(datagram.data(), datagram.size(),
                                  &sender, &senderPort);

    // update headtarget
    QString datagramstring = QString(datagram);

    QVector3D position;
    position.setX(datagramstring.split(" ", QString::SkipEmptyParts)[0].toDouble());
    position.setY(datagramstring.split(" ", QString::SkipEmptyParts)[1].toDouble());
    position.setZ(datagramstring.split(" ", QString::SkipEmptyParts)[2].toDouble());

    QQuaternion rotation;
    rotation.setX(datagramstring.split(" ", QString::SkipEmptyParts)[3].toDouble());
    rotation.setY(datagramstring.split(" ", QString::SkipEmptyParts)[4].toDouble());
    rotation.setZ(datagramstring.split(" ", QString::SkipEmptyParts)[5].toDouble());
    rotation.setScalar(datagramstring.split(" ", QString::SkipEmptyParts)[6].toDouble());

    QMatrix4x4 tmp;
    tmp.rotate(rotation);
    QMatrix3x3 rotationmatrix = tmp.normalMatrix();

    headtargetforkinect = "[" + QString::number(defaultheadtarget) + " 1] " +
                          "[" + QString::number(position.x()) + " " + QString::number(position.y()) + " " + QString::number(position.z()) + "] " +
                          "[" + QString::number(rotationmatrix(0,0)) + " " + QString::number(rotationmatrix(0,1)) + " " + QString::number(rotationmatrix(0,2)) + " " +
                                QString::number(rotationmatrix(1,0)) + " " + QString::number(rotationmatrix(1,1)) + " " + QString::number(rotationmatrix(1,2)) + " " +
                                QString::number(rotationmatrix(2,0)) + " " + QString::number(rotationmatrix(2,1)) + " " + QString::number(rotationmatrix(2,2)) + "]";
  }
}

void ARTNetwork::slotUpdateOculusFilter(QString filter)
{
  if(filter != "")
    oculusfilter = filter.split(";",QString::SkipEmptyParts);
  else
  {
    oculusfilter.clear();
    oculusfilter.append(QString::number(defaultheadtarget));
  }
}

void ARTNetwork::slotReadARTControllMessage()
{
    int size = 0;
    while((size = tcpartclient->bytesAvailable()) > 0)
    {
        QByteArray data;
        data.resize(size);
        data = tcpartclient->read(size);
        emit this->signalDisplayLogMessage(QString(data));
    }
}
