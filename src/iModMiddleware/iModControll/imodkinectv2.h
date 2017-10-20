/**
* @file iModControll/imodkinectv2.h
* @brief Windows only kinect implementation
*/

#ifdef WIN64
#ifndef IMODKINECTV2_H
#define IMODKINECTV2_H
#include <QObject>
#include <Windows.h>
#include <QPointF>
#include <Kinect.h>
#include "imodnetwork.h"

#ifdef WIN64
typedef struct __declspec(align(16))
#else
typedef struct __attribute__ ((aligned (16)))
#endif
{
  double x;
  double y;
  double z;
  double w;
} KinectJointOrientation;

#ifdef WIN64
typedef struct __declspec(align(16))
#else
typedef struct __attribute__ ((aligned (16)))
#endif
{
  double x;
  double y;
  double z;
} KinectJointPosition;

#ifdef WIN64
typedef struct __declspec(align(16))
#else
typedef struct __attribute__ ((aligned (16)))
#endif
{
  /**
   * @brief Joint type: See joint types in kinect.h
   */
  int type;
  KinectJointPosition position;
  KinectJointOrientation orientation;
  /**
   * @brief Trackingstate: 0 not tracked, 1 interferred, 2 tracked
   */
  int trackingstate;
} KinectJoint;

typedef struct _ksp
{
    qreal px;
    qreal py;

    _ksp()
    {
      px = 0; py = 0;
    }
    _ksp(qreal x, qreal y)
    {
      px = x; py = y;
    }
} KinectScreenPoint;

typedef struct _krgbx
{
    quint8 r;
    quint8 g;
    quint8 b;
    quint16 x;
} KinectRGBX;

class iModControl;
class iModObjectDetection;
class QTime;
class QTimer;
/**
 * @class iModKinectV2
 * @brief Control class for Microsoft Kinect V2. (Windows only)
 * @details This class implements a basic skeletal tracking with the Kinect V2. After parsing the sekelton it will be passed to the daemon over network.
 */
class iModKinectV2 : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModKinectV2
     */
    explicit iModKinectV2(iModControl* c, iModSkynet* n);
    ~iModKinectV2();

    /**
     * @brief Start the Kinect
     * @details Finds the connected Kinect sensore and trys to initialize it. When successfull, the coordinateMapper and bodyReader is initialized too
     * @return Returns true, if the Kinect is initialized successfully
     */
    HRESULT startKinect();
    /**
     * @brief Request the current kinect id;
     * @return Returns Kinectid
     */
    quint8 getKinectID() { return kinectid; }

  signals:
    /**
     * @brief Signal used for transmitting skeletal tracking data
     */
    void signalSendKinectData(quint64, quint8, QByteArray, quint8);
    /**
     * @brief Signal used to trigger the object tracker
     * @param pDepthBuffer
     * @param nDepthWidth
     * @param nDepthHeight
     * @param pColorBuffer
     * @param nColorWidth
     * @param nColorHeight
     */
    void signalProcessObjectTracking(DepthSpacePoint*, CameraSpacePoint*, int, int, const RGBQUAD*, int, int);

  public slots:
    /**
     * @brief Pulling new trackingdata from the Kinect sensore
     * @details This function contains a while(true) loop, that cant end. Connect this slot to a QThread::started signal to run the Kinect in a different thread.
     */
    void update();
    /**
     * @brief Updating the Kinect ID
     * @param cid Clientid
     * @param id Kinectid
     */
    void slotSetKinectID(quint8 cid, quint8 id);
    /**
     * @brief Sets the clipping for the Kinect
     * @param mi_r Near cutting plane
     * @param ma_r Far cutting plane
     * @bug Conserning the other bug. This slot is not triggered when connected to the coresponding in iModSkynet.
     */
    void slotKinectRange(qreal mi_r, qreal ma_r);

  private slots:
    void slotSystemTimer();

  private:
    template<class Interface>
    /**
     * @brief Releses a pointer savely
     * @param pInterfaceToRelease Pointer
     */
    inline void safeRelease(Interface *& pInterfaceToRelease)
    {
      if (pInterfaceToRelease != NULL)
      {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
      }
    }
    /**
     * @brief Processed the seketon
     * @details This function takes the Kinect-Body and converts its jointpositions and orientations to custom structs. These are serialized and sent to the daemon with signalSendKinectData.
     * @param nTime Current Kinect timestamp
     * @param nBodyCount Number of identified bodys
     * @param ppBodies Pointer to the list of bodys
     */
    void processSkeleton(INT64 nTime, int nBodyCount, IBody** ppBodies);
    /**
     * @brief Converts a 3D jointposition to a 2D screen point.
     * @details With the width and height parameter the image is scaled if necessary. Default depth image size 512x424.
     * @param bodyPoint Joint
     * @param width Screenwith
     * @param height Screenheight
     * @return Returns the joint in 2D in a custom struct.
     */
    KinectScreenPoint bodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);
    /**
     * @brief Default depht image width.
     */
    static const int cDepthWidth  = 512;
    static const int cDepthHeight = 424;
    static const int cColorWidth = 1920;
    static const int cColorHeight = 1080;
    /**
     * @brief Holds the pointer to the Kinect sensor.
     */
    IKinectSensor* m_pKinectSensor;
    /**
     * @brief Holds the pinter to the coordinate mapper.
     */
    ICoordinateMapper* m_pCoordinateMapper;
    /**
     * @brief Holds the pointer tp the body frame reader.
     */
    IBodyFrameReader* m_pBodyFrameReader;
    /**
     * @brief Holds a pointer to the multi source frame reader
     */
    IMultiSourceFrameReader* m_pMultiSourceFrameReader;
    /**
     * @brief Color stream buffer;
     */
    RGBQUAD* m_pColorRGBX;
    /**
     * @brief create heap storage for the coorinate mapping from color to depth
     */
    QByteArray qtdepthcoordinates;
    /**
     * @brief Holds a pointer to the depth frame reader
     */
    IDepthFrameReader* m_pDepthFrameReader;
    /**
     * @brief ID of the Kinect
     */
    quint8 kinectid;
    /**
     * @brief Distance of near clipping plane.
     */
    qreal minrange;
    /**
     * @brief Distance of far clipping plane.
     */
    qreal maxrange;
    /**
     * @brief Class for object detection
     */
    iModObjectDetection* odworker;
    /**
     * @brief Object detection thread
     */
    QThread* odthread;
    /**
     * @brief m_pDepthCoordinates
     */
    DepthSpacePoint* m_pDepthCoordinates;
    /**
     * @brief Color buffer
     */
    RGBQUAD *pColorBuffer;
    /**
     * @brief Buffer for camera space points
     */
    CameraSpacePoint* csp;
    /**
     * @brief Controller class for connections
     */
    iModControl* control;
    /**
     * @brief Network class for connections
     */
    iModSkynet* network;
    quint8 fps_body_tracking;
    QTimer* systemtimer;
    int lastframetime;

};
#endif // IMODKINECTV2_H
#endif
