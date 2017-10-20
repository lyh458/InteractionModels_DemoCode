#ifndef ARTKINECT_H
#define ARTKINECT_H

#ifdef WIN32
#include <Windows.h>
#include <NuiApi.h>

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QMap>

#include "artkinectbase.h"

class ARTKinectWidget;
class ARTKinectV1 : public ARTKinectBase
{
    Q_OBJECT

  public:
    enum KState { KSfinished, KSerror, KSrunning };

  public:
    ARTKinectV1(QMap<int, QString> bonemapping, bool fullbody);
    ~ARTKinectV1();

    inline bool isKinectRunning() { return runkinect; }
    inline void stopKinect() { runkinect = false; }

  private:
    HRESULT startKinect();
    void update();
    void processSkeleton();
    NUI_SKELETON_POSITION_INDEX getJointEnum(QString name);

  public slots:
    void slotStartKinect();

  signals:
    void signalDrawJoint(int, int, int, bool);

  private:
    // Current Kinect
    INuiSensor* m_pNuiSensor;
    HANDLE m_hNextSkeletonEvent;

};
#endif
#endif // ARTKINECT_H
