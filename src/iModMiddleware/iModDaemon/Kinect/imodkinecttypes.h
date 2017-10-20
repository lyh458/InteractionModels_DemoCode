/**
* @file iModDaemon/Kinect/imodkinecttypes.h
* @brief This file contains multiple structs that are used to make compute merged skeletons
*/

#ifndef IMODKINECTTYPES_H
#define IMODKINECTTYPES_H

#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>

#ifdef WIN64
typedef struct __declspec( align( 16 ) ) _kjo
#else
typedef struct __attribute__ ((aligned (16))) _kjo
#endif
{
  _kjo()
  {
    x = 0; y = 0; z = 0; w = 1;
  }

  double x;
  double y;
  double z;
  double w;

  QString print()
  {
    return QString::number(x) + " " + QString::number(y) + " " + QString::number(z) + " " + QString::number(w);
  }

  bool operator==(const _kjo &p)
  {
    if(x == p.x && y == p.y && z == p.z && w == p.w)
      return true;
    return false;
  }

} KinectJointOrientation;

#ifdef WIN64
typedef struct __declspec( align( 16 ) ) _kjp
#else
typedef struct __attribute__ ((aligned (16))) _kjp
#endif
{
  _kjp ()
  {
    x = 0; y = 0; z = 0;
  }

  double x;
  double y;
  double z;

  bool operator==(const _kjp &p)
  {
    if(x == p.x && y == p.y && z == p.z)
      return true;
    return false;
  }
} KinectJointPosition;

#ifdef WIN64
typedef struct __declspec( align( 16 ) )
#else
typedef struct __attribute__ ((aligned (16)))
#endif
{
  int type; // see joint types from kinect....
  KinectJointPosition position;
  KinectJointOrientation orientation;
  int trackingstate; // 0 not tracked, 1 interferred, 2 tracked

  void setPosition(QVector3D pos)
  {
    position.x = pos.x();
    position.y = pos.y();
    position.z = pos.z();
  }

  QVector3D getPosition()
  {
    return QVector3D(position.x, position.y, position.z);
  }

  void setRotation(QQuaternion quat)
  {
    orientation.x = quat.x();
    orientation.y = quat.y();
    orientation.z = quat.z();
    orientation.w = quat.scalar();
  }

  QString toString()
  {
    return "Pos: " + QString::number(position.x) + " " + QString::number(position.y) + " " + QString::number(position.z) + " Rot: " +
        QString::number(orientation.w) + " " + QString::number(orientation.x) + " " + QString::number(orientation.y) + " " + QString::number(orientation.z);
  }
} KinectJoint;

typedef struct _kinectkameradata
{
    QList<KinectJoint>* jointdata;
    qint64 timestamp;

    _kinectkameradata()
    {
      jointdata = new QList<KinectJoint>();
    }

    ~_kinectkameradata()
    {
      jointdata->clear();
      delete jointdata;
    }
} KinectKameraData;

typedef struct _userdata
{
    quint64 bodyid;
    qint32 kinectid;
    quint8 userid;
    qint64 timestamp;
    QVector3D center;
    quint8 artid;

    _userdata()
    {
        artid = 0;
        userid = 0;
    }

    bool operator==(_userdata ud)
    {
      if(bodyid == ud.bodyid && kinectid == ud.kinectid && userid == ud.userid && timestamp == ud.timestamp)
        return true;
      return false;
    }
} UserDataKinect;

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

typedef struct _kdq
{
    double quality;
    double diff;
    qint64 timestamp;

    _kdq()
    {
      quality = 0; diff = 0; timestamp = 0;
    }
} KinectDataQuality;

#endif // IMODKINECTTYPES_H
