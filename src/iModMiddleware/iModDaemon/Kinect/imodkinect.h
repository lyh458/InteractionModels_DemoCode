/**
* @file iModDaemon/Kinect/imodkinect.h
* @brief This file contains the class for managing multiple kinect data
*/

#ifndef IMODKINECT_H
#define IMODKINECT_H

#include <QObject>
#include <QtGui/QMatrix4x4>
#include <QtCore/QTimer>
#include <QtCore/QMutex>

#include "imodkinecttypes.h"

class iModRingBuffer;
typedef struct _otd
{
  qint64 ts;
  QString artstring;
  QList<QVector3D> virtualtargets;
  QList<iModRingBuffer*> buffer;
  float lastvaliddepth;

  _otd();
  ~_otd();

  void update(QList<QVector3D> data);

} ObjectTrackingData;

class QTimer;
class iModUtile;
class KVMainWindow;
class iModMathUtils;
class iModKinect : public QObject
{
    Q_OBJECT
  public:
    explicit iModKinect(iModUtile* _ut, QObject *parent = 0);
    ~iModKinect();

    // For now: Make sure that these fuctions are ONLY(!!) called by external classes -> otherwise deadlock (Perhaps prevention with QMutex::Recursive mode)
    int getCurrentRootKinect()   { QMutexLocker ml(datamutex); return current_root_kinect;           }
    int getUsernumber()          { QMutexLocker ml(datamutex); return getUserIDs().count();          }
    int getMASize()              { QMutexLocker ml(datamutex); return kinect_ma_size;                }
    int getNumberOfTransitions() { QMutexLocker ml(datamutex); return kinecttransitions.count();     }
    int getNumberOfStreams()     { QMutexLocker ml(datamutex); return kinectdata.count();            }
    double getLastFrametime()    { QMutexLocker ml(datamutex); return lastframetime;                 }
    QString getCurrentFrame()    { QMutexLocker ml(datamutex); return currentframe;                  }
    bool isCallibrating()        { QMutexLocker ml(datamutex); return callibrationtimer->isActive(); }
    QString getOTData()          { QMutexLocker ml(otmutex);   return otdata.artstring;              }
    QString getPerformanceData();
    bool otDataAvailable();

  signals:
    void signalKVSetTransitions(QMap<int, QMatrix4x4>);
    void signalKVUpdateDrawing(int, QByteArray, QByteArray);

  public slots:
    // For now: Make sure that these fuctions are ONLY(!!) called by external classes -> otherwise deadlock (Perhaps prevention with QMutex::Recursive mode)
    void slotUpdateKinectData(quint64 bid, quint8 jointcount, QByteArray data, quint8 kinectid);
    void slotGenerateNextFrame();
    void slotInit();
    void slotSetCurrentRootKinect(int value);
    void slotSetKinectOffsets(QVector3D position, QQuaternion rotation);
    void slotCalibrateKinects();
    void slotDetectUsers();
    void slotBuildARTMatching();
    void slotKinectObjectTracking(QByteArray data, quint8 clientid);

  private slots:
    void slotCleanKinects() { cleanKinects(1000); }
    void slotCallibrationTimer();
    void slotSystemTimer();

  private:
    QList<int> getKinects();
    QMatrix4x4 getKinectTransformation(int _src, int _dst);
    void updateBodyMatching(QPair<int, quint64> index);
    void cleanKinects(int msec);
    int getNextUserID();
    quint8 getNextARTID();
    QList<int> getUserIDs();
    KinectJoint combineKinects(int currentjoint, int userid);
    KinectJoint applyTransformation(QMatrix4x4 matrix, KinectJoint joint);
    bool kinectindexExists(QPair<int, quint64> index);
    template<typename type>
    QByteArray serializeQList(QList<type> list)
    {
      QByteArray result;
      result.resize(list.count() * sizeof(type));
      for(int i = 0; i < list.count(); i++)
        memcpy(result.data() + i * sizeof(type), &list[i], sizeof(type));
      return result;
    }

    QList<QMap<int, int> > kinectid_to_artid;
    QMap<int, QMatrix4x4> kinecttransitions;
    QList< QMap<int, QMatrix4x4> > callibrationbuffer;
    int current_root_kinect;
    QList<UserDataKinect> bodymatching;
    QVector3D kinect_position;
    QQuaternion kinect_rotation;
    QMatrix4x4 k_to_c;
    QMap<QPair<int, quint64>, KinectKameraData* > kinectdata;
    QMap<int, QMap<int, iModRingBuffer*>* > kinecthistory;
    int kinect_ma_size;
    QTimer* systemtimer;
    iModUtile* utile;
    QMap<int, int> kinect_rotationmatching;
    double lastframetime;
    QTimer* callibrationtimer;
    qreal currentaccuracy;
    QMap<QPair<int, quint64>, KinectDataQuality> kinectquality;
    QString currentframe;
    QMutex* datamutex; // Currently NonRecursive
    bool initialized;
    quint8 ot_root_kinect;

    KVMainWindow* view;
    iModMathUtils* math_test;
    ObjectTrackingData otdata;
    QMutex* otmutex;
    float lastvaliddepth;
    quint8 fps_object_tracking;
    quint8 fps_skeletal_tracking;
    QString performancestring;

    QList<QVector3D> last_eigenvectors;
    /*
    QFile* dumpfileART;
    QFile* dumpfileSingleKinect;
    QFile* dumpfileDualKinect;
    */
};

#endif // IMODKINECT_H
