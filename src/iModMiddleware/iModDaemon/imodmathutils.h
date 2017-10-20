/**
* @file iModDaemon/imodmathutils.h
* @brief This file contains a class for mathematical functions that are commonly used
*/

#ifndef QAGVUTILITY_H
#define QAGVUTILITY_H

#include <QtGui/QQuaternion>
#include <QtGui/QVector4D>
#include <QtGui/QMatrix4x4>

class iModRingBuffer;
/**
 * @class iModMathUtils
 * @brief This class provides various math functions and manages the history for the kinect frames.
 */
class iModMathUtils
{
  public:
    /**
     * @brief Constructor of iModMathUtils
     */
    iModMathUtils();
    /**
     * @brief Adds the new_rotation to the current average rotation using the slerp algorithm
     * @param new_rotation The new rotation
     * @return New average
     */
    QQuaternion averageRotation(QQuaternion new_rotation);
    /**
     * @brief Adds the new_rotation to the current average rotation using the slerp algorithm
     * @param new_rotation The new rotation
     * @return New average
     */
    QMatrix4x4 averageRotation(QMatrix4x4 new_rotation);
    /**
     * @brief Averages a transition matrix separately for rotations and positions
     * @param new_matrix The new transition matrix to be added.
     * @return New average
     */
    QMatrix4x4 averageTransitionMatrix(QMatrix4x4 new_matrix);
    /**
     * @brief Resets the current averaging and resets the counters for weight calculation.
     */
    void resetAveraging();
    /**
     * @brief Calculates a transition matrix with jacobi svd algorithm to transform all slave points into the master points as good as possible.
     * @param _master Points in master 3D space.
     * @param _slave Points in slave 3D space.
     * @return Calculated transition matrix
     */
    static QMatrix4x4 computeNumerical(QList<QVector3D> _master, QList<QVector3D> _slave);
    /**
     * @brief Converts a matrix to an quaternion (Only 3x3 rotaion part)
     * @param matrix Matrix
     * @return Resulting Quaternion
     */
    static QQuaternion matrixToQuaternion(QMatrix4x4 matrix);
    /**
     * @brief Converts a matrix to euler angels (Only 3x3 rotaion part)
     * @param matrix Matrix
     * @return Resulting angels (x, y, z)
     */
    static QVector3D matrixToEuler(QMatrix4x4 matrix);
    /**
     * @brief Converting euler angels to an quaternion
     * @param heading Heading
     * @param attitude Attitude
     * @param bank Banke
     * @return Resulting quaternion
     */
    static QQuaternion eulerToQuaternion(double heading, double attitude, double bank);
    /**
     * @brief Rotates a point around the origin by the angels given in rotation.
     * @param point Point to rotate.
     * @param rotation Vector containing the rotation angels (x, y, z)
     * @return Returns the rotated point.
     */
    static QVector4D rotatePoint(QVector4D point, QVector3D rotation);
    /**
     * @brief Calculates the distance between two points in 3D space
     * @param p1 Point 1
     * @param p2 Point 2
     * @return Distance between point 1 and point 2
     */
    static qreal distance(QVector3D p1, QVector3D p2);
    /**
     * @brief Calculates the average (exponential) for the given buffer.
     * @param buffer Buffer to use when calculating the position average
     * @return Averaged position
     */
    static QVector3D movingAveragePosition(iModRingBuffer* buffer);
    /**
     * @brief Calculates the average (exponential) for the given buffer.
     * @param buffer Buffer to use when calculating the rotaion average
     * @return Averaged rotation
     */
    static QQuaternion movingAverageRotation(iModRingBuffer* buffer);

  private:
    /**
     * @brief Normalizes an quaternion based on four float values
     * @param x X
     * @param y Y
     * @param z Z
     * @param w W
     * @return Returns a normalized quaternion
     */
    QQuaternion normalizeQuaternion(float x, float y, float z, float w);
    /**
     * @brief Inverts the quaternion
     * @param q Quaternion
     * @return Inverted Quaternion
     */
    QQuaternion inverseSignQuaternion(QQuaternion q);
    /**
     * @brief Returns, if two quaternions are close
     * @param q1 Quaternion 1
     * @param q2 Quaternion 2
     * @return Returns true, if the quaternion are close
     */
    bool areQuaternionsClose(QQuaternion q1, QQuaternion q2);

    /**
     * @brief Number of rotations in the curren average
     */
    int add_amount_rotation;
    /**
     * @brief Number of positions in the curren average
     */
    int add_amount_position;
    /**
     * @brief Current average rotaion
     */
    QVector4D cumulative_rotation;
    /**
     * @brief Current average position
     */
    QVector3D cumulative_position;
    /**
     * @brief Current average quaternion
     */
    QQuaternion cumulative_quaternion;
    /**
     * @brief First added rotation
     */
    QQuaternion first_rotation;
    /**
     * @brief Last added rotation
     */
    QQuaternion last_result;
    /**
     * @brief True, if this is the first run
     */
    bool firstrun;
};

#endif // QAGVUTILITY_H
