#include "artconverter.h"

#include "qmath.h"
#include <QtGui/QMatrix4x4>

ARTConverter::ARTConverter(QObject *parent) : QObject(parent)
{

}

ARTConverter::~ARTConverter()
{

}

QString ARTConverter::matrixToString(QMatrix3x3 matrix)
{
  QString string = "";
  for(int row = 0; row < 3; row++)
    for(int col = 0; col < 3; col++)
    {
      if(string != "")
        string += " ";
      string += QString::number(matrix(row, col), 'f', 5);
    }

  return string;
}

QMatrix3x3 ARTConverter::stringToMatrix(QString string)
{
  QMatrix3x3 matrix;

  for(int row = 0; row < 3; row++)
    for(int col = 0; col < 3; col++)
      matrix(row, col) = string.split(" ", QString::SkipEmptyParts)[row * 3 + col].toDouble();

  return matrix;
}

QVector3D ARTConverter::matrixToEuler(QMatrix4x4 matrix)
{
  QVector3D euler;

  euler.setY( M_PI - qAsin(matrix(0,2)));

  if(qFabs(matrix(0,2)) == 1)
  {
    euler.setZ(0);
    euler.setX(qAtan2(matrix(1,0), matrix(1,1)));
  }
  else
  {
    euler.setX(qAtan2(- matrix(1,2) / qCos(euler.y()), matrix(2,2) / qCos(euler.y())));
    euler.setZ(qAtan2(- matrix(0,1) / qCos(euler.y()), matrix(0,0) / qCos(euler.y())));
  }

  return euler;
}

QVector3D ARTConverter::quaternionToEuler(QQuaternion quat)
{
  QVector3D euler;

  double test = quat.x() * quat.y() + quat.z() * quat.scalar();
  if (test > 0.4999999) {
    euler.setX( 0 );
    euler.setY( 2 * qAtan2(quat.x(), quat.scalar()) );
    euler.setZ( M_PI / 2.0 );
    return euler;
  }
  if (test < -0.4999999) {
    euler.setX( 0 );
    euler.setY( -2 * qAtan2(quat.x(), quat.scalar()) );
    euler.setZ( -M_PI / 2.0 );
    return euler;
  }

  euler.setX( qAtan2(2 * quat.x() * quat.scalar() - 2 * quat.y() * quat.z(), 1 - 2 * qPow(quat.x(), 2) - 2 * qPow(quat.z(), 2)) );
  euler.setY( qAtan2(2 * quat.y() * quat.scalar() - 2 * quat.x() * quat.z(), 1 - 2 * qPow(quat.y(), 2) - 2 * qPow(quat.z(), 2)) );
  euler.setZ( qAsin(2 * quat.x() * quat.y() + 2 * quat.z() * quat.scalar()) );
  return euler;
}

QQuaternion ARTConverter::eulerToQuaternion(QVector3D euler)
{
  double c1 = qCos( euler.y() / 2.0 );
  double c2 = qCos( euler.z() / 2.0 );
  double c3 = qCos( euler.x() / 2.0 );
  double s1 = qSin( euler.y() / 2.0 );
  double s2 = qSin( euler.z() / 2.0 );
  double s3 = qSin( euler.x() / 2.0 );

  QQuaternion quat;
  quat.setScalar( c1 * c2 * c3 - s1 * s2 * s3 );
  quat.setX( s1 * s2 * c3 + c1 * c2 * s3 );
  quat.setY( s1 * c2 * c3 + c1 * s2 * s3 );
  quat.setZ( c1 * s2 * c3 - s1 * c2 * s3 );

  return quat;
}

QMatrix4x4 ARTConverter::eulerToMatrix(QVector3D euler)
{
  QMatrix4x4 matrix;
  double a = euler.x();
  double b = euler.y();
  double c = euler.z();

  matrix(0,0) = qCos(b) * qCos(c);
  matrix(0,1) = - qCos(b) * qSin(c);
  matrix(0,2) = qSin(b);

  matrix(1,0) = qCos(a) * qSin(c) + qSin(a) * qSin(b) * qCos(c);
  matrix(1,1) = qCos(a) * qCos(c) - qSin(a) * qSin(b) * qSin(c);
  matrix(1,2) = - qSin(a) * qCos(b);

  matrix(2,0) = qSin(a) * qSin(c) - qCos(a) * qSin(b) * qCos(c);
  matrix(2,1) = qSin(a) * qCos(c) + qCos(a) * qSin(b) * qSin(c);
  matrix(2,2) = qCos(a) * qCos(b);

  return matrix;
}

QQuaternion ARTConverter::matrixToQuaternion(QMatrix4x4 matrix)
{
  QQuaternion q;
  float trace = matrix(0,0) + matrix(1,1) + matrix(2,2);
  if( trace > 0 ) {
      float s = 0.5f / sqrtf(trace+ 1.0f);
      q.setScalar(0.25f / s);
      q.setX(( matrix(2,1) - matrix(1,2) ) * s);
      q.setY(( matrix(0,2) - matrix(2,0) ) * s);
      q.setZ(( matrix(1,0) - matrix(0,1) ) * s);
  } else {
      if ( matrix(0,0) > matrix(1,1) && matrix(0,0) > matrix(2,2) ) {
          float s = 2.0f * sqrtf( 1.0f + matrix(0,0) - matrix(1,1) - matrix(2,2));
          q.setScalar((matrix(2,1) - matrix(1,2) ) / s);
          q.setX(0.25f * s);
          q.setY((matrix(0,1) + matrix(1,0) ) / s);
          q.setZ((matrix(0,2) + matrix(2,0) ) / s);
      } else if (matrix(1,1) > matrix(2,2)) {
          float s = 2.0f * sqrtf( 1.0f + matrix(1,1) - matrix(0,0) - matrix(2,2));
          q.setScalar((matrix(0,2) - matrix(2,0) ) / s);
          q.setX((matrix(0,1) + matrix(1,0) ) / s);
          q.setY(0.25f * s);
          q.setZ((matrix(1,2) + matrix(2,1) ) / s);
      } else {
          float s = 2.0f * sqrtf( 1.0f + matrix(2,2) - matrix(0,0) - matrix(1,1) );
          q.setScalar((matrix(1,0) - matrix(0,1) ) / s);
          q.setX((matrix(0,2) + matrix(2,0) ) / s);
          q.setY((matrix(1,2) + matrix(2,1) ) / s);
          q.setZ(0.25f * s);
      }
  }
  return q;
}

QMatrix4x4 ARTConverter::quaternionToMatrix(QQuaternion quaternion)
{
  QMatrix4x4 tmp;
  tmp.rotate(quaternion);
  return tmp;
}

QMatrix4x4 ARTConverter::invertAxis(QVector4D normals, QMatrix4x4 matrix)
{
  QMatrix4x4 switchrotation;
  switchrotation.setRow(0, QVector4D( normals.x(),           0,           0,            0));
  switchrotation.setRow(1, QVector4D(           0, normals.y(),           0,            0));
  switchrotation.setRow(2, QVector4D(           0,           0, normals.z(),            0));
  switchrotation.setRow(3, QVector4D(           0,           0,           0,  normals.w()));

  switchrotation = switchrotation * matrix;
  return switchrotation;
}

QMatrix3x3 ARTConverter::leftToRightHanded(QMatrix3x3 left)
{
  QMatrix3x3 buffer;
  // row, col
  buffer(0, 0) = left(0, 0);
  buffer(1, 0) = left(2, 0);
  buffer(2, 0) = left(1, 0);

  buffer(0, 1) = left(0, 2);
  buffer(1, 1) = left(2, 2);
  buffer(2, 1) = left(1, 2);

  buffer(0, 2) = left(0, 1);
  buffer(1, 2) = left(2, 1);
  buffer(2, 2) = left(1, 1);

  return buffer;
}

QMatrix4x4 ARTConverter::switchAxis(QQuaternion axis, QMatrix4x4 matrix)
{
  QMatrix4x4 frontmatrix;
  frontmatrix.rotate(axis);

  axis.setScalar(axis.scalar() * -1);

  QMatrix4x4 backmatrix;
  backmatrix.rotate(axis);

  return frontmatrix * matrix * backmatrix;
}
