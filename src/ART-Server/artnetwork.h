#ifndef ARTNETWORK_H
#define ARTNETWORK_H

#include <QtNetwork/QTcpServer>
#include <QtCore/QMutex>
#include <QtGui/QGenericMatrix>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

typedef struct {
    int id;
    QMatrix4x4 data;
} Target;

class QUdpSocket;
class QTcpSocket;
class ARTNetwork : public QTcpServer
{
    Q_OBJECT
  public:
    explicit ARTNetwork(int tcpport, int udpdstport, bool kinectsused, QObject *parent = 0);
    void setOculusDstPort(int port) { oculusdstport = port; }
    void setDefaultHeadtarget(int id) { defaultheadtarget = id; }

  protected:
    virtual void incomingConnection(qintptr socketDescriptor);

  private:
    void initNetwork();

  signals:
    void signalDisplayLogMessage(QString);
    void signalSetConnectetClientsNumber(int);
    void signalArtFpsUpdate(int fps);

  public slots:
    void runARTNetwork();
    void slotDisplayLogMessage(QString s) { emit this->signalDisplayLogMessage(s); }
    void slotAddClient(int key, QString host);
    void slotRemoveClient(int key);
    void slotSendARTData(QString data);
    void slotUpdateOculusFilter(QString filter);
    void slotUpdateOculusCalibration() { calibrateoculus = true; }
    void slotResetOculusCalibration();
    void slotConnectARTServer(int udpmwport, int tcpmwport, QString tcphost);
    void slotReadARTDatagram();
    void slotReadOculusTracking(int port, bool merge);
    void slotReadOculusDatagram();
    void slotReadARTControllMessage();

  private:
    int tcpport;
    int udpport;
    QMap<int, QString> clientlist;
    QUdpSocket* udpsocket;
    QUdpSocket* udpartclient;
    QUdpSocket* udpoculusclient;
    QTcpSocket* tcpartclient;
    QMutex listlock;
    qulonglong framecounter;
    QString oculuscliente;
    QStringList oculusfilter;
    QMatrix4x4 oculusdynamicoffset;
    QMatrix4x4 calibratedframe;
    QMatrix4x4 calibratedfirstframe;
    QMatrix4x4 calibrationmatrix;
    bool calibrateoculus;
    int artframecounter;
    uint lasttimestemp;
    int oculusdstport;
    int defaultheadtarget;
    bool mergekinecttracking;
    QString headtargetforkinect;
    bool kinectinuse;
};

#endif // ARTNETWORK_H
