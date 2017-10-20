/**
* @file iModDaemon/Robot/imodrobot.h
* @brief This file contains the base class for robots
*/

#ifndef IMODROBOT_H
#define IMODROBOT_H

#include <QObject>
#include <QtGui/QVector3D>

class QTcpSocket;
class QUdpSocket;
class QTimer;
/**
 * class iModRobot
 * @brief The iModRobot class
 * @details Base class for robot controller. Things you have to do, when you subclass this:\n- In parseBlenderData() you have to accept blender datagrams and parse them\n- connectRobot() needs to set up the entire connection. E.g. creating an QTcpSocket or whatever your robot needs\n- You need to care for sending commands to the robot on your own. That means:\n\t- build a timer based on robot_supply_fps\n\t- or think of an own way to send commands
 */
class iModRobot : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModRobot
     * @param parent Parent of this instance
     */
    explicit iModRobot(QObject *parent = 0);
    ~iModRobot();

    /**
     * @brief Sets the propertys for the robot controlling
     * @param blending Blending
     * @param speed Maximal speed
     * @param speedup Maximal Speedup
     */
    void setUR5Propertys(double blending, double speed, double speedup);
    /**
     * @brief Gets the propertys for the robot controlling and writes them to the given variables.
     * @param blending Blending
     * @param speed Maximal speed
     * @param speedup Maximal speedup
     * @param enabled True, if robot is enabled
     * @param om Specifys the output mode: 0x00 UR5 direct, 0x01 UR5 over ROS, 0x02 File
     */
    void getUR5Propertys(double* blending, double* speed, double* speedup, bool* enabled, char* om);
    /**
     * @brief Returns the last ToolCenterPoint
     * @return TCP
     */
    virtual QVector3D getLastTCP() { return QVector3D(); }

  signals:
    /**
     * @brief This signal is emitted, whenever the FPS of Blender or the Robot change
     */
    void signalRobotBlenderFPS(int, int);
    /**
     * @brief Is emitted, whenever the connection state of the robot changes
     */
    void signalUR5ConnectionState(bool);
    /**
     * @brief Is emittet, wenn the propertys for the robot change
     */
    void signalUR5PropertysChanged(double, double, double, char);
    void singalUpdateRobotTCP();

  public slots:
    /**
     * @brief Establishes a connection to the robot.
     * @param ip Destination IP
     * @param port Destination Port
     */
    void slotEstablishConnection(QString ip = "", int port = -1);
    /**
     * @brief This slot is called to republish the connection state
     * @param connected True, if the robot is connected to the daemon
     */
    void slotStateRequest(bool connected);
    /**
     * @brief Changes the supply fps for the robot
     * @param fps New FPS
     */
    void slotChangeFPS(int fps);
    /**
     * @brief Sets, if sending frames to the robot is allowed.
     * @param value True, if value = 0x01, otherwise false
     */
    void slotSendFramesRobot(char value);
    /**
     * @brief Do whatever it needs to store recordable stuff. This slot is called, when a recording starts
     */
    virtual void slotStartRecording() {}
    /**
     * @brief Do whatever it needs to stop the recording. This slot is called, when a recording stops
     */
    virtual void slotStopRecording() {}
    /**
     * This is called, when you are asked to write your recording to the database.
     * category: The category in the database
     * name: The name of the recording
     */
    virtual void slotWriteRecording(QString category, QString name) {Q_UNUSED(category); Q_UNUSED(name);}
    /**
     * @brief Called, when data from blender (Robot IK) are avaible.
     */
    void slotBlenderUR5MoveInit(QByteArray data);
    /**
     * @brief Called, when data from blender (Robot IK) are avaible.
     */
    void slotBlenderUR5Move(QByteArray data);

  private slots:
    /**
     * @brief Slot for the FPS timer
     */
    void slotFPSTimerUpdate();

  protected:
    QString vectorToString(QList<float>* list, int startindex, int size);
    // The following stuff is virtual and can be overwritten
    /** Required.
     * Connect to the robot with whatever it needs.
     * Remember to use robot_ip and/or robot_port, if needed.
     */
    virtual void connectRobot() = 0;
    /** Required
     * This function is called every time, a new datagram is avaible.
     */
    virtual void parseBlenderData(char opcode, QList<float> datagram) = 0;
    /** Optional
     * Gets called every time an FPS-Change is triggered.
     * The new value is stored in robot_supply_fps
     */
    virtual void fpsChanged() {}
    /** Optional
     * This gets called, whenever robot_-blending, -speedup or -speed is changed
     */
    virtual void robotPropertysChanged() {}
    /** Optional
     * Is called every time the permission to send data changes
     */
    virtual void robotSendPersmissionChanged(bool value) { Q_UNUSED(value); }

    /** IP the deamon trys to connect the robot. If you dont need it, set ip != "" anyways */
    QString robot_ip;
    /** Port the deamon trys to connect the robot. If you dont need it, set port > 0 anyways */
    int robot_port;
    /** The robot will be supplied with this framerate */
    int robot_supply_fps;
    /** Blending radius used for controlling movements */
    double robot_blending;
    /** Max speed of the TCP */
    double robot_speed;
    /** Max speedup of TCP */
    double robot_speedup;
    /** Holds a value whether the robot is allowed to send data*/
    bool robot_sending_allowed;
    /** This is a couter for blender frames */
    int blender_fpscount;
    /** This is a couter for robot frames */
    int robot_fpscount;
    /** Mode where the control comands go */
    char outputmode;

  private:
    void appendInt(qint32 value, QByteArray* data);
    float readBlenderFloat(char* data, int offset);

    QTimer* timer_fps;
};

#endif // IMODROBOT_H
