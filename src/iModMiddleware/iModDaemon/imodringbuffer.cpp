#include "imodringbuffer.h"

iModRingBuffer::iModRingBuffer(int size) :
  index_write(0),
  buffer_size(size)
{
  pos_buffer.reserve(buffer_size);
  rot_buffer.reserve(buffer_size);
  for(int i = 0; i < buffer_size; i++)
  {
    pos_buffer.append(QVector3D());
    rot_buffer.append(QQuaternion());
  }
}

void iModRingBuffer::insert(QVector3D value, QQuaternion quat)
{
  pos_buffer.replace(index_write, value);
  rot_buffer.replace(index_write, quat);
  index_write++;
  if(index_write >= buffer_size)
    index_write = 0;
}

QVector3D iModRingBuffer::readPos(int index)
{
  // read at 0 = oldest element so... current index_write
  int index_read = index_write + index;
  while(index_read >= buffer_size)
    index_read -= buffer_size;
  return pos_buffer[index_read];
}

QQuaternion iModRingBuffer::readRot(int index)
{
  int index_read = index_write + index;
  while(index_read >= buffer_size)
    index_read -= buffer_size;
  return rot_buffer[index_read];
}

