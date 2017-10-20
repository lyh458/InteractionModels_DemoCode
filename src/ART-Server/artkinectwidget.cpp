#ifdef WIN32
#include "artkinectwidget.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>

#include <Windows.h>
#include <NuiApi.h>
#include <Kinect.h>

#include "artkinectcalibrationwidget.h"

ARTKinectWidget::ARTKinectWidget(QWidget *parent, bool fullbody) :
  QWidget(parent),
  jointlist(NULL),
  calibrationwidget(NULL),
  fullbodytracking(fullbody)
{
  int width = 490;
  this->resize(width,width*0.75);
  this->move(parent->size().width()-width-25, 3);
  this->show();
}

void ARTKinectWidget::paintEvent(QPaintEvent *event)
{

  QPainter painter(this);
  painter.fillRect(rect(), QColor(Qt::black));

  if(displaytext != "")
  {
    painter.save();
    painter.setPen(QColor(Qt::white));
    painter.setFont(QFont("Arial", 20, -1, false));
    painter.drawText(rect(), Qt::AlignCenter, displaytext);
    painter.restore();
  }

  if(jointlist == NULL)
    return;

  int jointnumber = kinectversion==1?NUI_SKELETON_POSITION_COUNT:JointType_Count;
  for(int i = 0; i < jointnumber; i++)
  {
    if(jointlist->at(i).pos.x() > 0 && jointlist->at(i).pos.y() > 0)
    {
      painter.setPen(QColor(jointlist->at(i).tracked?"orange":jointlist->at(i).calculated?"orangered":"red"));
      painter.drawEllipse(jointlist->at(i).pos,2,2);
    }
  }

  if(kinectversion==1)
  {
    // Render Torso
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_HEAD),            jointlist->at(NUI_SKELETON_POSITION_SHOULDER_CENTER) );
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_SHOULDER_CENTER), jointlist->at(NUI_SKELETON_POSITION_SHOULDER_LEFT)   );
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_SHOULDER_CENTER), jointlist->at(NUI_SKELETON_POSITION_SHOULDER_RIGHT)  );
    // Left Arm
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_SHOULDER_LEFT),   jointlist->at(NUI_SKELETON_POSITION_ELBOW_LEFT)      );
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_ELBOW_LEFT),      jointlist->at(NUI_SKELETON_POSITION_WRIST_LEFT)      );
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_WRIST_LEFT),      jointlist->at(NUI_SKELETON_POSITION_HAND_LEFT)       );
    // Right Arm
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_SHOULDER_RIGHT),  jointlist->at(NUI_SKELETON_POSITION_ELBOW_RIGHT)     );
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_ELBOW_RIGHT),     jointlist->at(NUI_SKELETON_POSITION_WRIST_RIGHT)     );
    drawBone(&painter, jointlist->at(NUI_SKELETON_POSITION_WRIST_RIGHT),     jointlist->at(NUI_SKELETON_POSITION_HAND_RIGHT)      );
  }
  else
  {

    // Torso
    drawBone(&painter, jointlist->at(JointType_Head),          jointlist->at(JointType_Neck)          );
    drawBone(&painter, jointlist->at(JointType_Neck),          jointlist->at(JointType_SpineShoulder) );
    drawBone(&painter, jointlist->at(JointType_SpineShoulder), jointlist->at(JointType_ShoulderRight) );
    drawBone(&painter, jointlist->at(JointType_SpineShoulder), jointlist->at(JointType_ShoulderLeft)  );

    if(fullbodytracking)
    {
      drawBone(&painter, jointlist->at(JointType_SpineShoulder), jointlist->at(JointType_SpineMid)      );
      drawBone(&painter, jointlist->at(JointType_SpineBase),     jointlist->at(JointType_SpineMid)      );
      drawBone(&painter, jointlist->at(JointType_SpineBase),     jointlist->at(JointType_HipLeft)       );
      drawBone(&painter, jointlist->at(JointType_SpineBase),     jointlist->at(JointType_HipRight)      );
    }

    // Right Arm
    drawBone(&painter, jointlist->at(JointType_ShoulderRight), jointlist->at(JointType_ElbowRight)    );
    drawBone(&painter, jointlist->at(JointType_ElbowRight),    jointlist->at(JointType_WristRight)    );
    drawBone(&painter, jointlist->at(JointType_WristRight),    jointlist->at(JointType_HandRight)     );
    drawBone(&painter, jointlist->at(JointType_HandRight),     jointlist->at(JointType_HandTipRight)  );
    drawBone(&painter, jointlist->at(JointType_WristRight),    jointlist->at(JointType_ThumbRight)    );

    // Left Arm
    drawBone(&painter, jointlist->at(JointType_ShoulderLeft),  jointlist->at(JointType_ElbowLeft)     );
    drawBone(&painter, jointlist->at(JointType_ElbowLeft),     jointlist->at(JointType_WristLeft)     );
    drawBone(&painter, jointlist->at(JointType_WristLeft),     jointlist->at(JointType_HandLeft)      );
    drawBone(&painter, jointlist->at(JointType_HandLeft),      jointlist->at(JointType_HandTipLeft)   );
    drawBone(&painter, jointlist->at(JointType_WristLeft),     jointlist->at(JointType_ThumbLeft)     );

    if(fullbodytracking)
    {
      //Right Leg
      drawBone(&painter, jointlist->at(JointType_FootRight),     jointlist->at(JointType_AnkleRight)    );
      drawBone(&painter, jointlist->at(JointType_KneeRight),     jointlist->at(JointType_AnkleRight)    );
      drawBone(&painter, jointlist->at(JointType_KneeRight),     jointlist->at(JointType_HipRight)      );

      //Left Leg
      drawBone(&painter, jointlist->at(JointType_FootLeft),      jointlist->at(JointType_AnkleLeft)     );
      drawBone(&painter, jointlist->at(JointType_KneeLeft),      jointlist->at(JointType_AnkleLeft)     );
      drawBone(&painter, jointlist->at(JointType_KneeLeft),      jointlist->at(JointType_HipLeft)       );
    }
  }
}

void ARTKinectWidget::drawBone(QPainter* painter, joint joint1, joint joint2)
{
  if(!joint1.tracked || !joint2.tracked)
    painter->setPen(QColor("gray"));
  else if(joint1.tracked && joint2.tracked)
    painter->setPen(QColor("green"));
  else if(joint1.calculated || joint2.calculated)
    painter->setPen(QColor("lightgreen"));

  painter->drawLine(joint1.pos, joint2.pos);
}

void ARTKinectWidget::slotDrawJointV1(int i, int x, int y, bool tracked)
{
  // pos is in 320x240 space
  joint tmp;
  tmp.pos = QPoint(x * (size().width() / 320.0),
                            y * (size().height() / 240.0));
  tmp.tracked = tracked;
  tmp.calculated = false;
  jointlist->replace(i,tmp);
  repaint();
}
void ARTKinectWidget::slotDrawJointV2(int i, int x, int y, bool tracked, bool calculated)
{

  if(((i>11 && i <20) || i < 2) && !fullbodytracking)
    return;
  joint tmp;
  tmp.pos = QPoint(x, y);
  tmp.tracked = tracked;
  tmp.calculated = calculated;
  jointlist->replace(i, tmp);
  repaint();
}

void ARTKinectWidget::mousePressEvent(QMouseEvent * event)
{
  emit this->signalStartKinect();
  if(calibrationwidget == NULL)
  {
    calibrationwidget = new ARTKinectCalibrationWidget(this);
    calibrationwidget->show();
    connect(calibrationwidget, &ARTKinectCalibrationWidget::finished,
            this,              &ARTKinectWidget::slotCalibrationFinished);
    connect(calibrationwidget, &ARTKinectCalibrationWidget::signalSetOffsets,
            this,              &ARTKinectWidget::slotSetOffsets);
  }
}

void ARTKinectWidget::setKinectVersion(int i)
{
  kinectversion = i;
  if(kinectversion == 1)
  {
    jointlist = new QVector<joint>(NUI_SKELETON_POSITION_COUNT);
  }
  else
  {
    jointlist = new QVector<joint>(JointType_Count);
  }
}
void ARTKinectWidget::slotCalibrationFinished(int result)
{
  calibrationwidget = NULL;
}

#endif
