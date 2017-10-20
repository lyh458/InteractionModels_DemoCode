#ifndef IMODRIFTNETWORK_H
#define IMODRIFTNETWORK_H

#include <QtCore/QThread>
#include <QtNetwork/QUdpSocket>
#include <QtGui/QMatrix4x4>
#include <QtCore/QVector>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>

class OculusDevice;
class QSettings;
class QFile;
class iModRiftNetwork : public QThread
{
        Q_OBJECT

    public:
        explicit iModRiftNetwork(OculusDevice* _od, QObject *parent = 0);
        ~iModRiftNetwork();

        void writeLog(QString message);
        QMatrix4x4 getARTDataMatrix() { return artdata; }
        QVector3D getARTPosition() { return artposition; }
        QQuaternion getARTRotation() { return artrotation; }

    protected:
        virtual void run();

    signals:
        void signalUpdateHMD(QMatrix4x4);

    public slots:

    private:
        void processDatagram(QByteArray datagram);
        void writeFile(QString message);

        QUdpSocket* socket;
        OculusDevice* od;
        QMatrix4x4 artdata;
        QVector3D artposition;
        QQuaternion artrotation;
        int udpport;
        QSettings* settingsfile;
        QFile* logfile;
};

#endif // IMODRIFTNETWORK_H
