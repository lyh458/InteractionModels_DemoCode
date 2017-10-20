#include "imodmathutils.h"

#include <qmath.h>
#include <eigen3/Eigen/Dense>

#include "imodringbuffer.h"

iModMathUtils::iModMathUtils() :
  add_amount_rotation(0),
  add_amount_position(0),
  cumulative_rotation(QVector4D()),
  cumulative_position(QVector3D()),
  cumulative_quaternion(QQuaternion()),
  firstrun(true)
{

}

//Get an average (mean) from more than two quaternions.
//Note: this only works if all the quaternions are relatively close together.
//Usage:
//-Cumulative is an external Vector4 which holds all the added x y z and w components.
//-newRotation is the next rotation to be added to the average pool
//-firstRotation is the first quaternion of the array to be averaged
//-addAmount holds the total amount of quaternions which are currently added
//This function returns the current average quaternion
QQuaternion iModMathUtils::averageRotation(QQuaternion new_rotation)
{
  add_amount_rotation++;

  cumulative_quaternion = QQuaternion::slerp(cumulative_quaternion, new_rotation, 1.0/add_amount_rotation);
  return cumulative_quaternion;

  // OLD OLD OLD
  /*
  // Unity3D code
  if(firstrun)
  {
    first_rotation = new_rotation;
    firstrun = false;
  }

  float w = 0.0f;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  //Before we add the new rotation to the average (mean), we have to check whether the quaternion has to be inverted. Because
  //q and -q are the same rotation, but cannot be averaged, we have to make sure they are all the same.
  if(!areQuaternionsClose(new_rotation, first_rotation))
    new_rotation = inverseSignQuaternion(new_rotation);

  //Average the values
  float addDet = 1.0/add_amount_rotation;
  cumulative_rotation.setW(cumulative_rotation.w() + new_rotation.scalar());
  w = cumulative_rotation.w() * addDet;
  cumulative_rotation.setX(cumulative_rotation.x() + new_rotation.x());
  x = cumulative_rotation.x() * addDet;
  cumulative_rotation.setY(cumulative_rotation.y() + new_rotation.y());
  y = cumulative_rotation.y() * addDet;
  cumulative_rotation.setZ(cumulative_rotation.z() + new_rotation.z());
  z = cumulative_rotation.z() * addDet;

  //note: if speed is an issue, you can skip the normalization step
  last_result = normalizeQuaternion(x, y, z, w);
  return last_result;
  */
}

QMatrix4x4 iModMathUtils::averageRotation(QMatrix4x4 new_rotation)
{
  QVector4D save1 = new_rotation.column(3);
  QVector4D save2 = new_rotation.row(3);

  QMatrix4x4 result;
  result.rotate(averageRotation(matrixToQuaternion(new_rotation)));
  result.setColumn(3, save1);
  result.setRow(3, save2);
  return result;
}

QMatrix4x4 iModMathUtils::averageTransitionMatrix(QMatrix4x4 new_matrix)
{
  QMatrix4x4 test = new_matrix;
  test.setColumn(3, QVector4D(0,0,0,1));
  if(!test.isIdentity())
    new_matrix = averageRotation(new_matrix);
  else
    new_matrix.rotate(last_result);
  cumulative_position += new_matrix.column(3).toVector3D();
  add_amount_position++;
  new_matrix.setColumn(3, QVector4D(cumulative_position/add_amount_position, 1));
  return new_matrix;
}

void iModMathUtils::resetAveraging()
{
  add_amount_rotation = 0;
  cumulative_rotation = QVector4D();
  cumulative_quaternion = QQuaternion();
  add_amount_position = 0;
  cumulative_position = QVector3D();
  firstrun = true;
}

QMatrix4x4 iModMathUtils::computeNumerical(QList<QVector3D> _master, QList<QVector3D> _slave)
{
  if(_master.size() != _slave.size())
  {
    qDebug() << "Failed to compute transition (Eigen3)";
    return QMatrix4x4();
  }

  QList<Eigen::Vector3d> master;
  foreach(QVector3D v, _master)
    master.append(Eigen::Vector3d(v.x(), v.y(), v.z()));

  QList<Eigen::Vector3d> slave;
  foreach(QVector3D v, _slave)
    slave.append(Eigen::Vector3d(v.x(), v.y(), v.z()));


  Eigen::MatrixXd solution = Eigen::MatrixXd::Zero(4, 4);
  solution(3, 3) = 1;

  Eigen::MatrixXd t(slave.size(), 4);

  for (int i = 0; i < slave.size(); i++)
  {
    for (int j = 0; j < 3; j++)
      t(i, j) = slave[i](j);

    t(i, 3) = 1;
  }

  for (int i = 0; i < 3; i++)
  {
    Eigen::VectorXd r(master.size());

    for (int j = 0; j < master.size(); j++)
      r(j) = master[j](i);

    Eigen::MatrixXd result = t.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(r);

    for (int j = 0; j < 4; j++)
      solution(i, j) = result(j);
  }

  QMatrix4x4 result = QMatrix4x4(solution(0,0), solution(0,1), solution(0,2), solution(0,3),
                                 solution(1,0), solution(1,1), solution(1,2), solution(1,3),
                                 solution(2,0), solution(2,1), solution(2,2), solution(2,3),
                                 solution(3,0), solution(3,1), solution(3,2), solution(3,3));
  return result;
}

QQuaternion iModMathUtils::matrixToQuaternion(QMatrix4x4 matrix)
{
  float qw, qx, qy, qz;
  float tr = matrix(0,0) + matrix(1,1) + matrix(2,2);

  if (tr > 0)
  {
    float S = qSqrt(tr+1.0) * 2; // S=4*qw
    qw = 0.25 * S;
    qx = (matrix(2,1) - matrix(1,2)) / S;
    qy = (matrix(0,2) - matrix(2,0)) / S;
    qz = (matrix(1,0) - matrix(0,1)) / S;
  }
  else if ((matrix(0,0) > matrix(1,1))&(matrix(0,0) > matrix(2,2)))
  {
    float S = qSqrt(1.0 + matrix(0,0) - matrix(1,1) - matrix(2,2)) * 2; // S=4*qx
    qw = (matrix(2,1) -  matrix(1,2)) / S;
    qx = 0.25 * S;
    qy = (matrix(0,1) +  matrix(1,0)) / S;
    qz = (matrix(0,2) +  matrix(2,0)) / S;
  }
  else if ( matrix(1,1) >  matrix(2,2))
  {
    float S = qSqrt(1.0 + matrix(1,1) - matrix(0,0) - matrix(2,2)) * 2; // S=4*qy
    qw = (matrix(0,2) -  matrix(2,0)) / S;
    qx = (matrix(0,1) +  matrix(1,0)) / S;
    qy = 0.25 * S;
    qz = (matrix(1,2) + matrix(2,1)) / S;
  }
  else
  {
    float S = qSqrt(1.0 + matrix(2,2) - matrix(0,0) - matrix(1,1)) * 2; // S=4*qz
    qw = (matrix(1,0) -  matrix(0,1)) / S;
    qx = (matrix(0,2) + matrix(2,0)) / S;
    qy = (matrix(1,2) + matrix(2,1)) / S;
    qz =
        0.25 * S;
  }
  return QQuaternion(qw, qx, qy, qz);
}

QQuaternion iModMathUtils::normalizeQuaternion(float x, float y, float z, float w)
{

  float lengthD = 1.0f / (w*w + x*x + y*y + z*z);
  w *= lengthD;
  x *= lengthD;
  y *= lengthD;
  z *= lengthD;

  return QQuaternion(w, x, y, z);
}

//Changes the sign of the quaternion components. This is not the same as the inverse.
QQuaternion iModMathUtils::inverseSignQuaternion(QQuaternion q)
{
  return QQuaternion(-q.scalar(), -q.x(), -q.y(), -q.z());
}

//Returns true if the two input quaternions are close to each other. This can
//be used to check whether or not one of two quaternions which are supposed to
//be very similar but has its component signs reversed (q has the same rotation as
//-q)
bool iModMathUtils::areQuaternionsClose(QQuaternion q1, QQuaternion q2)
{
  float dot = QQuaternion::dotProduct(q1, q2);

  if(dot < 0.0f)
    return false;
  else
    return true;
}

QVector3D iModMathUtils::matrixToEuler(QMatrix4x4 matrix)
{
  float x, y, z;
  matrixToQuaternion(matrix).getEulerAngles(&z, &y, &x);
  return QVector3D(x, y, z);
}

QQuaternion iModMathUtils::eulerToQuaternion(double heading, double attitude, double bank)
{
  QQuaternion quat;
  // Assuming the angles are in radians.
  double c1 = qCos(heading);
  double s1 = qSin(heading);
  double c2 = qCos(attitude);
  double s2 = qSin(attitude);
  double c3 = qCos(bank);
  double s3 = qSin(bank);
  quat.setScalar(qSqrt(1.0 + c1 * c2 + c1*c3 - s1 * s2 * s3 + c2*c3) / 2.0);
  double w4 = (4.0 * quat.scalar());
  quat.setX((c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4);
  quat.setY((s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4);
  quat.setZ((-s1 * s3 + c1 * s2 * c3 +s2) / w4);
  return quat;
}

QVector4D iModMathUtils::rotatePoint(QVector4D point, QVector3D rotation)
{
  QVector4D newpoint = point;
  double sin_t, cos_t;

  // rotate X:
  sin_t = qSin(rotation.x());
  cos_t = qCos(rotation.x());
  newpoint.setY(point.y() * cos_t - point.z() * sin_t);
  newpoint.setZ(point.z() * cos_t + point.y() * sin_t);
  point = newpoint;

  // rotate Y:
  sin_t = qSin(rotation.y());
  cos_t = qCos(rotation.y());
  newpoint.setX(point.x() * cos_t - point.z() * sin_t);
  newpoint.setZ(point.z() * cos_t + point.x() * sin_t);
  point = newpoint;

  // rotate Z:
  sin_t = qSin(rotation.z());
  cos_t = qCos(rotation.z());
  newpoint.setX(point.x() * cos_t - point.y() * sin_t);
  newpoint.setY(point.y() * cos_t + point.x() * sin_t);
  point = newpoint;

  return newpoint;
}

qreal iModMathUtils::distance(QVector3D p1, QVector3D p2)
{
  return qSqrt( qPow(p1.x() - p2.x(), 2) +  qPow(p1.y() - p2.y(), 2) + qPow(p1.z() - p2.z(), 2));
}

QVector3D iModMathUtils::movingAveragePosition(iModRingBuffer* buffer)
{
  // Linear gewichteter gleichverteilter gleitender mittelwert
  qreal ma_x = 2.0 / (buffer->size() * (buffer->size() + 1 ));
  qreal ma_y = ma_x;
  qreal ma_z = ma_x;
  qreal sum_x = 0, sum_y = 0, sum_z = 0;
  for(int i = 1; i <= buffer->size(); i++)
  {
    sum_x += i * buffer->readPos(i-1).x();
    sum_y += i * buffer->readPos(i-1).y();
    sum_z += i * buffer->readPos(i-1).z();
  }
  ma_x *= sum_x;
  ma_y *= sum_y;
  ma_z *= sum_z;

  return QVector3D(ma_x, ma_y, ma_z);
}

QQuaternion iModMathUtils::movingAverageRotation(iModRingBuffer* buffer)
{
  iModMathUtils* math = new iModMathUtils();
  math->resetAveraging();
  QQuaternion result;

  for(int i = 0; i < buffer->size(); i++)
    result = math->averageRotation(buffer->readRot(i));

  delete math;
  return result;
}

