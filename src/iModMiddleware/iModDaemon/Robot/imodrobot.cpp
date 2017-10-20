#include "imodrobot.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QTimer>

#include "imodur5.h"

iModRobot::iModRobot(QObject *parent) :
  QObject(parent),
  robot_ip(""),
  robot_port(0),
  robot_blending(0.0),
  robot_speed(0.4),
  robot_speedup(0.02),
  robot_sending_allowed(true),
  blender_fpscount(0),
  outputmode(0x00)
{
  timer_fps = new QTimer(this);
  connect(timer_fps, &QTimer::timeout, this, &iModRobot::slotFPSTimerUpdate);
  timer_fps->start(1000);
}

iModRobot::~iModRobot()
{
  delete timer_fps;
}

void iModRobot::getUR5Propertys(double* blending, double* speed, double* speedup, bool* enabled, char* om)
{
  if(blending) *blending = robot_blending;
  if(speed)    *speed = robot_speed;
  if(speedup)  *speedup = robot_speedup;
  if(enabled)  *enabled = robot_sending_allowed;
  if(om)       *om = outputmode;
}

void iModRobot::slotEstablishConnection(QString ip, int port)
{
  this->robot_ip = ip;
  this->robot_port = port;

  connectRobot();
}

void iModRobot::slotStateRequest(bool connected)
{
  emit this->signalUR5ConnectionState(connected);
}

void iModRobot::slotChangeFPS(int fps)
{
  this->robot_supply_fps = fps;
  fpsChanged();
}

void iModRobot::setUR5Propertys(double blending, double speed, double speedup)
{
  robot_blending = blending;
  robot_speed = speed;
  robot_speedup = speedup;
  robotPropertysChanged();
  emit this->signalUR5PropertysChanged(robot_blending, robot_speed, robot_speedup, outputmode);
}

void iModRobot::slotSendFramesRobot(char value)
{
  robot_sending_allowed = value == 0x01;
  robotSendPersmissionChanged(robot_sending_allowed);
}

void iModRobot::slotBlenderUR5MoveInit(QByteArray data)
{
  if((data.size()) % 4 == 0)
  {
    QList<float> values;
    for(int i = 0; i < data.size(); i += 4)
      values.append(readBlenderFloat(data.data(), i));

    parseBlenderData(0x02, values);
  }
}

void iModRobot::slotBlenderUR5Move(QByteArray data)
{
  if((data.size()) % 4 == 0)
  {
    QList<float> values;
    for(int i = 0; i < data.size(); i += 4)
      values.append(readBlenderFloat(data.data(), i));

    parseBlenderData(0x01, values);
  }
}

void iModRobot::slotFPSTimerUpdate()
{
  emit this->signalRobotBlenderFPS(robot_fpscount, blender_fpscount);
  robot_fpscount = 0;
  blender_fpscount = 0;
}

void iModRobot::appendInt(qint32 value, QByteArray *data)
{
  data->append((char*)&value,sizeof(qint32));
}

float iModRobot::readBlenderFloat(char* data, int offset)
{
  float value;
  memcpy(&value, data + offset, 4);
  return value;
}

QString iModRobot::vectorToString(QList<float>* list, int startindex, int size)
{
  QString value = "";

  for(int i = startindex; i < startindex + size && list->size() >= startindex + size; i++)
    value += QString::number(list->at(i), 'f', 8) + ",";

  return value.left(value.size() - 1);
}
