/**
* @file iModDaemon/imodringbuffer.h
* @brief This file contains a class for a custom ringbuffer
*/

#ifndef IMODRINGBUFFER_H
#define IMODRINGBUFFER_H

#include <QtCore/QVector>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>

/**
 * @class iModRingBuffer
 * @brief Class implementation for a custom ringbuffer
 */
class iModRingBuffer
{
  public:
    /**
     * @brief Constructor of iModRingBuffer
     * @param size Desired size of the buffer
     */
    iModRingBuffer(int size);
    /**
     * @brief Inserts a value to the ringbuffer at the write index
     * @param value Position value
     * @param quat Rotation value
     */
    void insert(QVector3D value, QQuaternion quat = QQuaternion());
    /**
     * @brief Reads the next position from the buffer at the given position
     * @param index Read position
     * @return Returns the position
     */
    QVector3D readPos(int index);
    /**
     * @brief Reads the next rotation from the buffer at the given position
     * @param index Read position
     * @return Returns the rotation
     */
    QQuaternion readRot(int index);
    /**
     * @brief Returns the size of the ringbuffer
     * @return Size
     */
    int size() { return buffer_size; }

  private:
    /**
     * @brief Buffer for position vectors
     */
    QVector<QVector3D> pos_buffer;
    /**
     * @brief Buffer for rotations
     */
    QVector<QQuaternion> rot_buffer;
    /**
     * @brief Next index to write in the bufer
     */
    int index_write;
    /**
     * @brief Total size of the buffer
     */
    int buffer_size;
};

#endif // IMODRINGBUFFER_H
