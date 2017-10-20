#ifndef ARTNETWORKCLIENT_H
#define ARTNETWORKCLIENT_H

#include <QtCore/QObject>
#include <QtNetwork/QHostAddress>

class QTcpSocket;
class ARTNetworkClient : public QObject
{
    Q_OBJECT
  public:
    explicit ARTNetworkClient(qintptr socketDescriptor);

  protected:
//    virtual void runClient();

  signals:
    void signalDisplayLogMessage(QString);
    void signalAddClient(int, QString);
    void signalRemoveClient(int);

  public slots:
    void runClient();
    void slotDataAvaible();
    void slotDisconnect();
    void slotError(QAbstractSocket::SocketError socketError);

  private:
    qintptr socketDescriptor;
    QTcpSocket* tcpsocket;
    QString networkdata;

};

#endif // ARTNETWORKCLIENT_H
