#ifndef ARTRINGBUFFER_H
#define ARTRINGBUFFER_H

#include <QtCore/QVector>
#include <QtCore/QString>

class ARTRingBuffer
{
  public:
    ARTRingBuffer();
    void insert(QString frame);
    QString getNextFrame();
    void clearBuffer();
    void resetBuffer();

  private:
    QVector<QString> framelist;
    long bufferposition;
};

#endif // ARTRINGBUFFER_H
