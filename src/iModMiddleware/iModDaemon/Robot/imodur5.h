/**
* @file iModDaemon/Robot/imodur5.h
* @brief This file contains the implementation of the UR5 robot based on the iModRobot class
*/

#ifndef IMODUR5_H
#define IMODUR5_H

#include <QObject>
#include <QtCore/QQueue>
#include <QtCore/QTime>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtGui/QVector3D>
#include <QtNetwork>
#include <QtCore>
#include "imodrobot.h"

#ifdef WIN64
#include <cstdint>
#endif

typedef struct
{
    double safety_mode;
} UR5State;

class UR5Angle;
class QTcpSocket;
class QTimer;
class QUdpSocket;
class iModDatabase;
/**
 * @class iModUR5
 * @brief This class handles the connection to the UR5 and sends the control commands.
 */
class iModUR5 : public iModRobot
{
    Q_OBJECT
  public:
    explicit iModUR5(iModDatabase* db, QObject *parent = 0);
    ~iModUR5();
    /**
     * @brief Returns the current output mode
     * @return Outputmode: 0x00 UR5, 0x01 UR5 over ROS, 0x02 File
     */
    char getOutputMode() { return outputmode; }
    /**
     * @brief Returns the last ToolCenterPoint
     * @return TCP
     */
    QVector3D getLastTCP();

  signals:

  private slots:
    /**
     * @brief Slot used by the TCP connection to the robot. This recives the robotstates
     */
    void slotRobotIncomming();

  public slots:
    /**
     * @brief Used to change the output mode and makes all required initialisations
     * @param type Outputmode: 0x00 UR5, 0x01 UR5 over ROS, 0x02 File
     */
    void slotUR5Output(char type);

  private:
    /**
     * @brief Used to parse the data from Blender (inverse kinematics)
     * @details This function creates a movep command based on the IK-values from blender and sends it to the robot, if the robot is enabled
     * @param opcode The opcode form Blender: b drive to init pose, a normal drive commands
     * @param data Serialized float values from Blender containin the join angels for the Robot
     */
    virtual void parseBlenderData(char opcode, QList<float> data);
    virtual void connectRobot();
    QByteArray reverseArray(QByteArray& seq);
    /**
     * @brief Reads the current joint angels from the robot state -> offset 252
     * @param data Bytearray to read from
     * @param offset Offset for the bytearray pointer
     */
    void readCurrentAngle(char* data, int offset);
    /**
     * @brief Reads the position of the curren TCP
     * @param data Buffer to read from ...
     * @param offset ... at this offset
     */
    void readCurrentTCP(char* data, int offset);

  private:
    QTcpSocket* robot_socket;
    bool drivetoinit;
    /**
     * @brief Holds a list of the current/last robot angels.
     */
    QList<double> lastangle;
    /**
     * @brief Holds the current TCP
     */
    QList<double> lasttcp;
    /**
     * @brief Caps the movement to a maximum of 10° per joint between frames and fixes things like -360° = 0° = +360°
     * @param blender Joint angels from blender
     * @return Caped joints
     */
    QList<float> capMovement(QList<float> blender);
    template <typename type>
    void swap_endian(type& d)
    {
      char& raw = reinterpret_cast<char&>(d);
      std::reverse(&raw, &raw + sizeof(type));
    }

    template <typename type>
    void appendValue(QByteArray* data, type& d)
    {
      swap_endian(d);
      QByteArray tmp;
      tmp.resize(sizeof(type));
      memcpy(tmp.data(), &d, sizeof(type));

      data->append(tmp);
    }

    // grehl
    QUdpSocket* ros_socket;
    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes; //We need to store the size to verify if a block has received completely

    QFile* ur5dumpfile;
    QMutex datamutex;
};


#endif // IMODUR5_H
