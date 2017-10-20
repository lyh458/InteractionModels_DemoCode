#ifndef ARTKINECTBASE_H
#define ARTKINECTBASE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QMutex>

class ARTKinectBase : public QObject
{
    Q_OBJECT
  public:
    ARTKinectBase(QMap<int, QString> bm, bool fullbody);

  protected:
    template<class Interface>
    inline void safeRelease(Interface *& pInterfaceToRelease)
    {
        if (pInterfaceToRelease != NULL)
        {
            pInterfaceToRelease->Release();
            pInterfaceToRelease = NULL;
        }
    }

    QString calculateRotation(int roll, int pitch, int yaw);

  public slots:
    void slotSendFrame();
    void slotSetMinDistance(double d) { mindistance = d; }
    void slotSetMaxDistance(double d) { maxdistance = d; }
    void slotSetOffsets(double x_loc, double y_loc, double z_loc, int x_rot, int y_rot, int z_rot);

  signals:
    void signalDisplayLogMessage(QString);
    void signalKinectError(QString);
    void signalKinectStopped();
    void signalKinectRunning();
    void signalSendARTData(QString);

  protected:
    QMap<int, QString> bonemapping;
    QMutex kinectMutex;
    bool runkinect;
    bool sendframe;
    double mindistance;
    double maxdistance;
    double x_loc;
    double y_loc;
    double z_loc;
    int x_rot;
    int y_rot;
    int z_rot;
    bool fullbodytracking;
};

#endif // ARTKINECTBASE_H
