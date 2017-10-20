#include "imodftsensor.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QTimer>

iModFTSensor::iModFTSensor(QObject *parent) : QObject(parent)
{


}

bool iModFTSensor::connectSensor()
{
  qDebug() << "[Robotiq FT300] Trying to connect";

  INT_8 ret;
  bool success = true;
  for(int i = 0; i < 3 && success; i++)
  {
    ret = rq_sensor_state();
    if(ret == -1)
      success = waitForOtherConnection();
  }

  if(success)
    qDebug() << "[Robotiq FT300] Connection established";
  else
    qDebug() << "[Robotiq FT300] Can't connect";
  return success;
}

void iModFTSensor::slotStart()
{
  dbg_counter = 0;
  //dbg_timer = new QTimer(this);
  //connect(dbg_timer, &QTimer::timeout, this, &iModFTSensor::slotDBG_Timer);
  //dbg_timer->start(1000);

  updatetimer = new QTimer(this);
  connect(updatetimer, &QTimer::timeout, this, &iModFTSensor::slotUpdate);
  updatetimer->start(1000/30);
  //while(1)
  //  slotUpdate();
}

void iModFTSensor::slotDBG_Timer()
{
  //qDebug() << dbg_counter << "FPS";
  //dbg_counter = 0;
}

void iModFTSensor::slotUpdate()
{
  INT_8 ret;
  ret = rq_sensor_state();
  if(ret == -1)
    waitForOtherConnection();

  if(rq_sensor_get_current_state() == RQ_STATE_RUN)
  {
    dbg_counter++;
    QVector3D force, torque;
    getData(force, torque);

    if(force_offset == QVector3D())
      force_offset = force;
    if(torque_offset == QVector3D())
      torque_offset = torque;

    force = force - force_offset;
    torque = torque - torque_offset;

    //qDebug() << "Force" << force << "Torque" << torque;
    emit this->signalFTSensorValues(force, torque);
  }
}

bool iModFTSensor::waitForOtherConnection()
{
  INT_8 ret;
  //struct timespec tim;
  //tim.tv_sec = 1;
  //tim.tv_nsec = 0L;
  quint8 con_counter = 0;

  while(con_counter < 3)
  {
    QThread::sleep(1);
    //nanosleep(&tim, (struct timespec *)NULL);
    ret = rq_sensor_state();
    if(ret == 0)
      break;
    con_counter++;
  }
  if(con_counter >= 3)
    return false;
  return true;
}

void iModFTSensor::getData(INT_8* chr_return)
{
  INT_8 i;
  INT_8 floatData[50];

  for(i = 0; i < 6; i++)
  {
    sprintf(floatData, "%f", rq_state_get_received_data(i));
    if(i == 0)
    {
      strcpy(chr_return, "( ");
      strcat(chr_return, floatData);
    }
    else
    {
      strcat(chr_return," , ");
      strcat(chr_return,floatData);
    }
    if(i == 5)
      strcat(chr_return, " )");
  }
}

void iModFTSensor::getData(QVector3D& force, QVector3D& torque)
{
  force.setX(rq_state_get_received_data(0));
  force.setY(rq_state_get_received_data(1));
  force.setZ(rq_state_get_received_data(2));

  torque.setX(rq_state_get_received_data(3));
  torque.setY(rq_state_get_received_data(4));
  torque.setZ(rq_state_get_received_data(5));
}
