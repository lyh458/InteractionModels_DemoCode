#include "artringbuffer.h"


ARTRingBuffer::ARTRingBuffer() :
  bufferposition(0)
{
}

void ARTRingBuffer::insert(QString frame)
{
  framelist.append(frame);
}

QString ARTRingBuffer::getNextFrame()
{
  QString frame = "";
  if(framelist.count() > 0)
  {
    if(bufferposition < framelist.count())
      frame = framelist.at(bufferposition);
    else
    {
      bufferposition = 0;
      frame = framelist.at(bufferposition);
    }
    bufferposition++;
  }
  return frame;
}

void ARTRingBuffer::clearBuffer()
{
  framelist.clear();
  bufferposition = 0;
}

void ARTRingBuffer::resetBuffer()
{
  bufferposition = 0;
}
