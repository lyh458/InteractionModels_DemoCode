#include "artkinectbase.h"

#include <QtMath>

ARTKinectBase::ARTKinectBase(QMap<int, QString> bm, bool fullbody) :
  QObject(),
  bonemapping(bm),
  runkinect(false),
  sendframe(false),
  mindistance(0.5),
  maxdistance(8.0),
  x_loc(0),
  y_loc(0),
  z_loc(0),
  x_rot(0),
  y_rot(0),
  z_rot(0),
  fullbodytracking(fullbody)
{
}

void ARTKinectBase::slotSetOffsets(double x_loc, double y_loc, double z_loc, int x_rot, int y_rot, int z_rot)
{
  this->x_loc = x_loc;
  this->y_loc = y_loc;
  this->z_loc = z_loc;
  this->x_rot = x_rot;
  this->y_rot = y_rot;
  this->z_rot = z_rot;
}

void ARTKinectBase::slotSendFrame()
{
  sendframe = true;
}

QString ARTKinectBase::calculateRotation(int roll, int pitch, int yaw)
{
  QString rotation = " [";

  int places = 4;
  qreal rollR = (M_PI/180.0) * roll;
  qreal pitchR = (M_PI/180.0) * pitch;
  qreal yawR = (M_PI/180.0) * yaw;

  rotation += QString("%1").arg((qCos(pitchR) * qCos(yawR)),0,'f',places) + " ";
  rotation += QString("%1").arg((qCos(pitchR) * qSin(yawR)),0,'f',places) + " ";
  rotation += QString("%1").arg((-1.0 * qSin(pitchR)),0,'f',places) + " ";

  rotation += QString("%1").arg((qSin(rollR) * qSin(pitchR) * qCos(yawR) - qCos(rollR) * qSin(yawR)),0,'f',places) + " ";
  rotation += QString("%1").arg((qSin(rollR) * qSin(pitchR) * qSin(yawR) + qCos(rollR) * qCos(yawR)),0,'f',places) + " ";
  rotation += QString("%1").arg((qSin(rollR) * qCos(pitchR)),0,'f',places) + " ";

  rotation += QString("%1").arg((qCos(rollR) * qSin(pitchR) * qCos(yawR) + qSin(rollR) * qSin(yawR)),0,'f',places) + " ";
  rotation += QString("%1").arg((qCos(rollR) * qSin(pitchR) * qSin(yawR) - qSin(rollR) * qCos(yawR)),0,'f',places) + " ";
  rotation += QString("%1").arg((qCos(rollR) * qCos(pitchR)),0,'f',places) + "]";

  return rotation;
}
