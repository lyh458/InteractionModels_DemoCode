/**
* @file iModDaemon/Kinect/kvkinect.h
* @brief This file contains the class for a single kinect widget.
*/

#ifndef KVKINECT_H
#define KVKINECT_H

#include <QWidget>
#include <QtCore/QList>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector2D>

#include "imodkinecttypes.h"

/**
  * @brief Native depthimage width
  */
#define windowsx 512
/**
  * @brief Native depthimage height
  */
#define windowsy 424

/**
  * @brief This struct represents a 3D point on a screen
  */
typedef struct _kspws
{
    /**
     * @brief X-Positioin
     */
    qreal px;
    /**
     * @brief Y-Position
     */
    qreal py;
    /**
     * @brief Trackingstate of the joint
     */
    char state;

    /**
     * @brief Constructor of KinectScreenPointWS.
     * @param p Point to initialize with.
     * @param s State to initialize with.
     */
    _kspws(QVector2D p, char s)
    {
      px = p.x(), py = p.y(), state = s;
    }
    /**
     * @brief Constructor of KinectScreenPointWS.
     * @param x X-Coordinate of point to initialize with.
     * @param y Y-Coordinate of point to initialize with.
     * @param s State to initialize with.
     */
    _kspws(qreal x, qreal y, char s)
    {
      px = x; py = y; state = s;
    }
    /**
     * @brief Returns the screenpoint as QPointF
     * @return QPointF of struct.
     */
    QPointF toPointF()
    {
      return QPointF(px, py);
    }

} KinectScreenPointWS;

/**
 * @class KVKinect
 * @brief The KVKinect class
 * @details This class is a widget and draws a skeleton from a Kinect on its surface. This class can hold multiple sekeltons and draw them at the same time, but for the normal vies its recommendet to pass only one skeleton. The merged view however holds all skeletons and transforms them. In an ideal world they should exacly be the same, but you can visualize the differance within the merged view. In additionm the merged view shows the merged skeleton, not only the transformed source skeletons.
 */
class KVKinect : public QWidget
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of KVKinect
     * @details The constructor initializes the required projection matrix vom 3D Kinect space to 2D screen space.
     * @param parent Parent of this widget (important for window management)
     */
    explicit KVKinect(QWidget *parent = 0);
    /**
     * @brief Update skeletal drawing
     * @details This functions stores the 2D imagedata in a list. If necessary the 3D point is transformed before it is projected to 2D screen space.
     * @param data The list of 3D joints to draw.
     * @param tstate Trackingstate of each 3D joint.
     * @param _kid ID of the Kinect that is drawn.
     * @param transition If this is no identety, the 3D points are transformed.
     */
    void updateDrawing(QList<QVector3D> data, QList<char> tstate, quint8 _kid, QMatrix4x4 transition);

  signals:

  public slots:

  private:
    /**
     * @brief Overriting the paintEvent from QWidget
     * @details This functions realizes custom painting on the widget. It has a build in differentiation between a merged skeleton or a full skeleton.
     * @param event
     */
    virtual void paintEvent(QPaintEvent* event);

    /**
     * @brief Holds the size of this widget
     */
    QSize size;
    /**
     * @brief positions Holds the list of joints for a certain skeleton indexed by KinectID.
     */
    QMap<quint8,QList<KinectScreenPointWS> > positions;
    /**
     * @brief ID of the Kinect the skeleton came from.
     */
    quint8 kid;
    /**
     * @brief The projection matrix vom 3D space to 2D space
     */
    QMatrix4x4 kinect_view_projection;
};

#endif // KVKINECT_H
