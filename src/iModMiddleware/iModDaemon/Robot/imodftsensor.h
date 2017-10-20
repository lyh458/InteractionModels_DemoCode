/**
* @file iModDaemon/Robot/imodftsensor.h
* @brief This file contains the wraper class for Robotiq FT150
*/

#ifndef IMODFTSENSOR_H
#define IMODFTSENSOR_H

#include <QObject>
#include <QtGui/QVector3D>
extern "C"
{
  #include "../RobotiqFT150/rq_int.h"
  #include "../RobotiqFT150/rq_sensor_state.h"
  #include "../RobotiqFT150/rq_sensor_com.h"
}

class QTimer;
/**
 * @class iModFTSensor
 * @brief The iModFTSensor class
 */
class iModFTSensor : public QObject
{
    Q_OBJECT
  public:
    explicit iModFTSensor(QObject *parent = 0);
    bool connectSensor();

  signals:
    void signalFTSensorValues(QVector3D, QVector3D);

  public slots:
    void slotStart();

  private slots:
    void slotDBG_Timer();
    void slotUpdate();

  private:
    bool waitForOtherConnection();
    void getData(INT_8* chr_return);
    void getData(QVector3D& force, QVector3D& torque);

    QTimer* updatetimer;

    quint32 dbg_counter;
    QTimer* dbg_timer;
    QVector3D force_offset;
    QVector3D torque_offset;

};

#endif // IMODFTSENSOR_H
