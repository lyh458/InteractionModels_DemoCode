#include "artnetworkclient.h"

#include <QtNetwork/QTcpSocket>

ARTNetworkClient::ARTNetworkClient(qintptr socketDescriptor) :
  QObject(), socketDescriptor(socketDescriptor)
{
  networkdata = "";
}

void ARTNetworkClient::runClient()
{
  tcpsocket = new QTcpSocket();

  connect(tcpsocket, &QIODevice::readyRead,
          this,      &ARTNetworkClient::slotDataAvaible);
  connect(tcpsocket, &QAbstractSocket::disconnected,
          this,      &ARTNetworkClient::slotDisconnect);
  connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)),
          this,      SLOT(slotError(QAbstractSocket::SocketError)));


  if (!tcpsocket->setSocketDescriptor(socketDescriptor)) {
    emit this->signalDisplayLogMessage(tcpsocket->errorString());
    return;
  }
  emit this->signalDisplayLogMessage("New client connected " + QString::number(socketDescriptor));
}

void ARTNetworkClient::slotError(QAbstractSocket::SocketError socketError)
{
  emit this->signalRemoveClient(socketDescriptor);
}

void ARTNetworkClient::slotDisconnect()
{
  emit this->signalRemoveClient(socketDescriptor);
}

void ARTNetworkClient::slotDataAvaible()
{
  if(tcpsocket->bytesAvailable())
  {
    networkdata += QString(tcpsocket->readAll());

    QString parserdata = "";
    //go throu it and find first match...
    for(int i = 0; i <= networkdata.length(); i++)
    {
      parserdata = networkdata;

      if(parserdata == "dtrack2 tracking start")
      {
        tcpsocket->write(QString("dtrack2 ok").toUtf8());
        emit this->signalAddClient(socketDescriptor, tcpsocket->peerAddress().toString());
        emit this->signalDisplayLogMessage("Starting Dtrack2");
        networkdata = "";
      }
      else if (parserdata == "dtrack2 tracking stop")
      {
        tcpsocket->write(QString("dtrack2 ok").toUtf8());
        emit this->signalRemoveClient(socketDescriptor);
        emit this->signalDisplayLogMessage("Stopping Dtrack2");
        networkdata = "";
      }
      else if (parserdata == "dtrack2 getmsg")
      {
        emit this->signalDisplayLogMessage("Dtrack2: getmsg");
        tcpsocket->write(QString("dtrack2 msg word word2 0 0 message").toUtf8());
        networkdata = "";
      }
      else if (parserdata == "dtrack2 set")
      {
        networkdata = "";
      }
      else if (parserdata == "dtrack2 get")
      {
        emit this->signalDisplayLogMessage("Dtack2: get");
        networkdata = "";
      }
    }
  }
}
