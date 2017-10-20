#include "imodartclient.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>

iModARTClient::iModARTClient(QTcpSocket *_socket, int port, QObject *parent)
  : QObject(parent),
    socket(_socket),
    udpsendport(port)
{
  connect(socket, &QTcpSocket::readyRead,
          this,   &iModARTClient::slotTcpIncomming);
  qDebug() << "New ARTClient";
  udpsocket = new QUdpSocket();
  senddata = false;
}

iModARTClient::~iModARTClient()
{
  socket->close();
  delete socket;
}

void iModARTClient::slotTcpIncomming()
{
  while(socket->bytesAvailable())
  {
    QString data = QString(socket->readAll());
    qDebug() << data;
    if(data == "dtrack2 tracking start")
    {
      socket->write(QString("dtrack2 ok").toUtf8());
      senddata = true;
    }
    if(data == "dtrack2 tracking stop")
    {
      socket->write(QString("dtrack2 ok").toUtf8());
      senddata = false;
    }
    if(data == "dtrack2 getmsg")
      socket->write(QString("dtrack2 msg word word2 0 0 message").toUtf8());
  }
}

void iModARTClient::sendData(QByteArray *data)
{
  //qDebug() << QString(*data);
  if(senddata)
    udpsocket->writeDatagram(*data, socket->peerAddress(), udpsendport);
}
