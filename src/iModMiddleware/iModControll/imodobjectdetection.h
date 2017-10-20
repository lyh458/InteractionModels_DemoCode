#ifdef WIN64
#ifndef IMODOBJECTDETECTION_H
#define IMODOBJECTDETECTION_H

#include <QObject>
#include <Kinect.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

#include <QtGui/QMatrix4x4>
#include <QtCore/QTimer>

class iModObjectDetection : public QObject
{
    Q_OBJECT
  public:
    explicit iModObjectDetection(QObject *parent = 0);
    ~iModObjectDetection();
    bool isRunning() { return is_running; }

  signals:
    void signalPublishObjectTrackingResult(QByteArray);

  public slots:
    void slotDetectObject(DepthSpacePoint* dsp, CameraSpacePoint* csp, int nDepthWidth, int nDepthHeight, const RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight);
    void slotKinectOCVValues(QVector3D cmin, QVector3D cmax, quint8 e, quint8 d, bool s);

  private slots:
    void slotSystemTimer();

  private:
    bool is_running;
    QMatrix4x4 computeNumerical(QList<QVector3D> _master, QList<QVector3D> _slave);

    // Constant OCV buffers
    IplImage* ocv_color_image;
    IplImage* ocv_8UC4_rgbr;
    IplImage* ocv_8UC4_rgbr_small;
    IplImage* ocv_8UC3_hsv;
    IplImage* ocv_8UC1_obcl;
    QVector3D color_min;
    QVector3D color_max;
    quint8 erode_cycles;
    quint8 delate_cycles;
    bool showwindow;
    quint8 fpscounter;
    int lastframetime;
    QTimer* systemtimer;
};

#endif // IMODOBJECTDETECTION_H
#endif
