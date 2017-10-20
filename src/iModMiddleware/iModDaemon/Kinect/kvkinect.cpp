#include "kvkinect.h"

#include <QtGui/QPainter>
#include <QtGui/QVector2D>

KVKinect::KVKinect(QWidget *parent) : QWidget(parent)
{
  size = QSize(windowsx,windowsy);
  this->resize(size);
  this->show();
  kid = 255;

  kinect_view_projection = QMatrix4x4(132.103 , -0.358592, 0.760091, 253.594,
                                      -7.26896, -130.059 , -24.6259, 279.877,
                                      0       , 0        , 0       , 0      ,
                                      0       , 0        , 0       , 1       );
}

void KVKinect::updateDrawing(QList<QVector3D> data, QList<char> tstate, quint8 _kid, QMatrix4x4 transition)
{
  kid = _kid;
  QList<KinectScreenPointWS> bufferlist;
  for(int i = 0; i < data.count(); i++)
  {
    QVector3D point3d = data[i];
    if(!transition.isIdentity())
    {
      point3d *= 1000.0;
      point3d = transition * point3d;
      point3d /= 1000.0;
    }

    QVector2D point2d = QVector3D(kinect_view_projection * point3d).toVector2D();
    bufferlist.append(KinectScreenPointWS(point2d, tstate[i]));
  }
  positions.insert(kid, bufferlist);
  this->update();
}

void KVKinect::paintEvent(QPaintEvent* event)
{
  QWidget::paintEvent(event);
  QPainter p(this);
  p.fillRect(this->rect(), QColor(Qt::black));

  foreach(quint8 kinect, positions.keys())
  {
    QList<KinectScreenPointWS> tmpbuffer = positions.value(kinect);
    QPen pen;
    pen.setColor(Qt::white);
    p.setPen(pen);
    p.drawText(5, 15, QString("Kinect id: %1").arg(kid));
    for(int i = 0; i < tmpbuffer.count() && (positions.keys().count() == 1 || tmpbuffer.count() == 8); i++)
    {
      if(tmpbuffer[i].state == 2)
        pen.setColor(Qt::green);
      else if(tmpbuffer[i].state == 1)
        pen.setColor(Qt::yellow);
      else
        pen.setColor(Qt::red);

      if(tmpbuffer.count() == 8)
        pen.setWidth(5);
      p.setPen(pen);
      p.drawEllipse(tmpbuffer[i].px, tmpbuffer[i].py, 2, 2);
    }

    if(tmpbuffer.count() < 25)
      continue;
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    p.drawLine(tmpbuffer[3].toPointF(), tmpbuffer[2].toPointF());
    p.drawLine(tmpbuffer[20].toPointF(), tmpbuffer[4].toPointF());
    p.drawLine(tmpbuffer[20].toPointF(), tmpbuffer[8].toPointF());
    p.drawLine(tmpbuffer[20].toPointF(), tmpbuffer[1].toPointF());
    p.drawLine(tmpbuffer[0].toPointF(), tmpbuffer[1].toPointF());
    p.drawLine(tmpbuffer[0].toPointF(), tmpbuffer[16].toPointF());
    p.drawLine(tmpbuffer[0].toPointF(), tmpbuffer[12].toPointF());
    p.drawLine(tmpbuffer[8].toPointF(), tmpbuffer[9].toPointF());
    p.drawLine(tmpbuffer[9].toPointF(), tmpbuffer[10].toPointF());
    p.drawLine(tmpbuffer[10].toPointF(), tmpbuffer[11].toPointF());
    p.drawLine(tmpbuffer[11].toPointF(), tmpbuffer[23].toPointF());
    p.drawLine(tmpbuffer[10].toPointF(), tmpbuffer[24].toPointF());
    p.drawLine(tmpbuffer[4].toPointF(), tmpbuffer[5].toPointF());
    p.drawLine(tmpbuffer[5].toPointF(), tmpbuffer[6].toPointF());
    p.drawLine(tmpbuffer[6].toPointF(), tmpbuffer[7].toPointF());
    p.drawLine(tmpbuffer[7].toPointF(), tmpbuffer[21].toPointF());
    p.drawLine(tmpbuffer[6].toPointF(), tmpbuffer[22].toPointF());
    p.drawLine(tmpbuffer[19].toPointF(), tmpbuffer[18].toPointF());
    p.drawLine(tmpbuffer[17].toPointF(), tmpbuffer[18].toPointF());
    p.drawLine(tmpbuffer[17].toPointF(), tmpbuffer[16].toPointF());
    p.drawLine(tmpbuffer[15].toPointF(), tmpbuffer[14].toPointF());
    p.drawLine(tmpbuffer[13].toPointF(), tmpbuffer[14].toPointF());
    p.drawLine(tmpbuffer[13].toPointF(), tmpbuffer[12].toPointF());
  }
}
