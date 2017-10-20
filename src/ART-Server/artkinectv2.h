#ifndef ARTKINECTV2_H
#define ARTKINECTV2_H

#ifdef WIN32
#include <Windows.h>
#include <Kinect.h>

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "artkinectbase.h"

class ARTKinectV2 : public ARTKinectBase
{
    Q_OBJECT
  public:
    explicit ARTKinectV2(QMap<int, QString> bonemapping, bool fullbody);

  private:
    HRESULT startKinect();
    void update();
    void processSkeleton(INT64 nTime, int nBodyCount, IBody** ppBodies);
    QPoint BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);
    JointType getJointEnum(QString name);

  signals:
    void signalDrawJoint(int, int, int, bool, bool);

  public slots:
    void slotStartKinect();

  private:
    static const int cDepthWidth  = 512;
    static const int cDepthHeight = 424;
    IKinectSensor* m_pKinectSensor;
    ICoordinateMapper* m_pCoordinateMapper;
    IBodyFrameReader* m_pBodyFrameReader;
    INT64 m_nStartTime;
    double m_fFreq;
    INT64 m_nLastCounter;
    DWORD m_nFramesSinceUpdate;
<<<<<<< HEAD
=======
    QMap<int, QString> bonemapping;
>>>>>>> [Feature_ARTServer] improoved windows version with kinect baseclass
};
#endif
#endif // ARTKINECTV2_H
