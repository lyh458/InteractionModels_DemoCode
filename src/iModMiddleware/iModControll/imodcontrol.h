/**
* @file iModControll/imodcontrol.h
*
* @brief Interface for the iModControl library
*
*/

#ifndef IMODCONTROLL_H
#define IMODCONTROLL_H

#include "imodcontroll_global.h"

#include <string>
#include <QObject>

#include <QStringList>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>

class iModSkynet;
#ifdef WIN64
class iModKinectV2;
#endif
/**
 * @class iModControl
 * @brief Control class for clients
 * @details This class is the interface for the user. If you want to build a custom client, use this interface to communicate with the daemon.
 */
class IMODCONTROLLSHARED_EXPORT iModControl : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor for iModControll.
     * @details Creating the iModSkynet and.
     * @param parent The parent of this class.
     */
    iModControl(QObject *parent = 0);
    /**
     * @brief Dummy constructor to seperate blender... creating network class in different thread -> External event loop
     * @param dummy Just to seperate the constructors
     */
    iModControl(bool dummy);
    /**
      * @brief Destructor
      */
    ~iModControl();

    /**
     * @brief Returns the gripper state
     * @return Gripper state
     */
    char getGripperState() { return this->gstate; }
    /**
     * @brief Returns the last robot tcp
     * @return TCP
     */
    QVector3D getLastRobotTCP() { return this->ur5lasttcp; }
    /**
     * @brief getLastFTForce
     * @return Force
     */
    QVector3D getLastFTForce() { return this->ftsensor_f; }
    /**
     * @brief getLastFTTorque
     * @return Torque
     */
    QVector3D getLastFTTorque() { return this->ftsensor_t; }
    /**
     * @brief Connect to Daemon.
     * @details This function tiggers the signal signalTriggerDaemonConnect().
     */
    void connectDaemon();
    /**
     * @brief Connecto the Daemon to the UR5 Robot.
     * @param ip Robot ip.
     * @param port Robot port.
     */
    void connectUR5(QString ip, int port);
    /**
     * @brief This resets the Daemon
     * @details This forces the Daemon to shut down. Over the special exit code, the main-function of the Daemon will restart the program. Attention: This will temporally disconnect all Clients -> You can loose root permission.
     * @return Returns true, if the command could be sent over network.
     */
    bool resetDaemon();
    /**
     * @brief Get root previlegs from Daemon.
     * @return Returns true, if the command could be sent over network.
     */
    bool forceRoot();
    /**
     * @brief Sets the root Kinect.
     * @param kinectid ID of new root Kinect.
     * @return Returns true, if the command could be sent over network.
     */
    bool setRootKinect(quint8 kinectid);
    /**
     * @brief Request the current root Kinect and if this client is root.
     * @param root Writes the privileges here.
     * @param rootkinect Writes the current rootkinect here.
     * @return Returns true, if the command could be sent over network.
     */
    bool daemonControll(bool& root, quint8& rootkinect);
    /**
     * @brief Set Kinect offsets.
     * @details This function is not used anymore by the Daemon and will be ignored.
     * @param position Positionoffset.
     * @param orientation Rotationoffset.
     * @return Returns true, if the command could be sent over network.
     */
    bool setKinectOffsets(QVector3D position, QQuaternion orientation);
    /**
     * @brief Gets the curren Kinect offsets.
     * @details The offsets are not used by the Daemon. This function will return 0,0,0 for the positionoffset and an 1,0,0,0 for the quaternion.
     * @param position Writes the positionoffset to this vector.
     * @param orientation Writes the orientationoffset to this quaternion.
     * @return Returns true, if the command could be sent over network.
     */
    bool getKinectOffsets(QVector3D& position, QQuaternion& orientation);
    /**
     * @brief Gets the categorys from the database.
     * @param list Writes the result to this list.
     * @return Returns true, if the command could be sent over network.
     */
    bool getDatabaseCategorylist(QStringList& list);
    /**
     * @brief Gets the tables from a certain category.
     * @param list Writes the result to this list.
     * @param category Desired category
     * @return Returns true, if the command could be sent over network.
     */
    bool getDatabaseTableList(QStringList& list, QString category);
    /**
     * @brief Starts the A.R.T.-Tracking.
     * @param start If true, starts the tracking, otherwise stops it.
     * @return Returns true, if the command could be sent over network.
     */
    bool startARTTracking(bool start);
    /**
     * @brief Starts recording to database
     * @param start If true, starts recording, otherwise stops it.
     * @param rfps Just for compatibility -> not used.
     * @return Returns true, if the command could be sent over network.
     */
    bool startDatabaseRecording(bool start, quint8 rfps);
    /**
     * @brief Gets the state of the database.
     * @param recording Returns, if it is currently recording a session.
     * @param replayfps Returns, how fast the playback runs.
     * @warning This will only record trackingdata. Recording the robot state is not testet yet.
     * @return Returns true, if the command could be sent over network.
     */
    bool getDatabaseState(bool& recording, quint8& replayfps);
    /**
     * @brief Writes the last recording to the database.
     * @param category Target category (Has to exist)
     * @param table Target table (Is createt if necessary)
     * @return Returns true, if the command could be sent over network.
     */
    bool writeRecordingToDatabase(QString category, QString table);
    /**
     * @brief Sets the input methode.
     * @param value Meaning: 0x00 = A.R.T., 0x01 = Kinect, 0x02 = Database, Every unknown value leads to A.R.T..
     * @warning Calling this function will stop and discard every recording.
     * @return Returns true, if the command could be sent over network.
     */
    bool setInputMethode(char value);
    /**
     * @brief Request current input Methode
     * @param value Returns the current methide with this value.
     * @return Returns true, if the command could be sent over network.
     */
    bool inputMethode(char& value);
    /**
     * @brief Sets the replay FPS of the database.
     * @param value FPS value.
     * @return Returns true, if the command could be sent over network.
     */
    bool setDatabaseReplayFPS(quint8 value);
    /**
     * @brief Loading a certain table to Daemon memory.
     * @details Loading the table to the memory increases the performance of the replay.
     * @param cat Desiret category (Has to exist)
     * @param tab Desired table (Has to exist)
     * @warning Loading big recordings can take some time. The replay will start as soon as it is loaded.
     * @return Returns true, if the command could be sent over network.
     */
    bool loadCurrentTable(QString cat, QString tab);
    /**
     * @brief Set the limits/controlvalues for the Robot
     * @param blending Robot movement blend radius
     * @param speed Robot movement maximal speed
     * @param speedup Robot movement maximal speedup
     * @param enable Enable robot
     * @return Returns true, if the command could be sent over network.
     */
    bool setRobotControllValues(qreal blending, qreal speed, qreal speedup, bool enable);
    /**
     * @brief Get the limits/controlvalues for the Robot
     * @param blending Returns blend radius
     * @param speed Returns maximal speed
     * @param speedup Returns maximal speedup
     * @param enable Returns, if robot is enabled
     * @return Returns true, if the command could be sent over network.
     */
    bool getRobotControllValues(qreal& blending, qreal& speed, qreal& speedup, bool& enable);
    /**
     * @brief Get the number of identified A.R.T.-Targets
     * @param targets Returns the number of identified targets
     * @return Returns true, if the command could be sent over network.
     */
    bool getIdentifiedTargets(quint8& targets);
    /**
     * @brief Sets the output mode for the Robot.
     * @param mode Output mode: 0x00 UR5 direct, 0x01 UR5 over ROS, 0x02 File.
     * @return Returns true, if the command could be sent over network.
     */
    bool setUR5OutputMode(char mode);
    /**
     * @brief Sets the clipping ranges for the kinects
     * @param _cl List of affected client IDs
     * @param mi_r Near clipping plane
     * @param ma_r Far clipping plane
     * @return Returns true, if the command could be sent over network.
     */
    bool setKinectMinMaxRange(QList<int> _cl, qreal mi_r, qreal ma_r);
    /**
     * @brief Starts the Kinect
     * @param id Client ID
     */
    void startKinect(int id);
    /**
     * @brief Sends the trackingdata from the Kinect to the Daemon
     */
    void sendKinectDataToDaemon();
    /**
     * @brief Connects to the gripper
     * @param ip IP of the gripper
     * @return Returns true, if the command could be sent over network.
     */
    bool connectGripper(QString ip);
    /**
     * @brief Sets and applys the gripper status.
     * @param data Gripper status
     * @return Returns true, if the command could be sent over network.
     */
    bool setGripperStatus(QByteArray data);
    /**
     * @brief Sets the limits for OpenCV object detection
     * @param cmin HSV minimal values
     * @param cmax HSV maximal values
     * @param e Number of erode cycles
     * @param d Number of delate cycles
     * @return
     */
    bool setOpenCVValues(QVector3D cmin, QVector3D cmax, quint8 e, quint8 d, bool s);
    /**
     * @brief Sends robot angles to the daemon: WRAPPER for CInterface ONLY
     * @param type Type 1 = Initpose, 2 = Normal movement
     * @param data Pointer to the data
     * @param length Length of data
     * @return
     */
    void sendBlenderCommunication(char type, QByteArray qdata);
    /**
     * @brief Publishes the data from Matlab conserning the objectdata
     * @param os Ownership 1 = none, 2 = A, 3 = B, 4 = Both
     * @param pb Probability for os
     * @param cv Derivided closing value: 0 = open, 1 = closed
     */
    bool sendTriadicObjectData(quint8 os, qreal pb, qreal cv);
    /**
     * @brief Sets the gripper behaviour: 0 = Manual, 1 = Controlled by Matrlab, 2 = Controlled by FTSensor
     * @param type Type
     */
    void setGripperAutomatic(char type);

  signals:
    /**
     * @brief Signal providing deamon controll values
     */
    void signalDaemonControll(quint8, quint8);
    /**
     * @brief Signal providing robot limits/control values
     */
    void signalRobotControllValues(qreal, qreal, qreal, bool, char);
    /**
     * @brief Signal providing the tracking input methode
     */
    void signalInputMethode(char);
    /**
     * @brief Signal providing a list of connected clients
     */
    void signalDaemonState(QStringList);
    /**
     * @brief Signal providing if a recording is active and the current replay FPS
     */
    void signalDatabaseRecordingCrontroll(bool, quint8);
    /**
     * @brief Signal providing the current kinect offsets
     * @warning This is not supportet anymore
     */
    void signalKinectControllValues(QVector3D, QQuaternion);
    /**
     * @brief Signal providing, if the daemon is connectet with the robot
     */
    void signalUR5Connection(bool);
    /**
     * @brief Signal providing the FPS bewtwen Blender->Daemon and Robot->Daemon
     */
    void signalBlenderRobotFPS(int, int);
    /**
     * @brief Signal triggered, when the client needs to connect/reconnect to the daemon
     */
    void signalTriggerDaemonConnect();
    /**
     * @brief Signal used to publish gripper status
     */
    void signalGripperControlValue(QByteArray);
    /**
     * @brief For CInterface only -> Wrapper to move sendBlenderCommunication to another thread
     */
    void signalCIsendBlenderCommunication(char, QByteArray);
    /**
     * @brief For CInterface only -> Wrapper to move sendBlenderCommunication to another thread
     */
    void signalCIsendTriadicObjectData(quint8, qreal, qreal);
    void signalFTSensorValues(QVector3D, QVector3D);

  public slots:
    /**
     * @brief This thing ist for debugging ONLY
     * @param min Kinect min range
     * @param max Kinect max range
     * @warning Normally I would connect the signal to the slot in iModKinectV2, but somehow it is not tirggered. CURRENTLY NOT THREAD SAVE!
     * @bug This thing should not be used, but currently there seems to be a bug with the slot in iModKinectV2. It wont connect.
     */
    void slotDEBUG_KinectRange(qreal min, qreal max);
    /**
     * @brief Sends the results from object tracking to the daemon
     * @param data Data
     * @param kinectid ID of this kinect
     */
    void slotSendObjectTrackingData(QByteArray data);
    /**
     * @brief Use this slot only, when you use the CInterface
     */
    void slotCInterfaceStartup();

  private slots:
    /**
     * @brief Sending kinect tracking data to the demon
     * @param tid Kinect tracking body ID
     * @param jointcount Joints inside the data
     * @param data Serialized trackingdata
     * @param kinectid ID of current Kinect/client id
     */
    void slotSendKinectData(quint64 tid, quint8 jointcount, QByteArray data, quint8 kinectid);
    /**
     * @brief Slot to republish changes within daemon control values
     * @param rc ID of current client
     * @param rk ID of current root kinect
     */
    void slotDaemonControll(quint8 rc, quint8 rk) { emit this->signalDaemonControll(rc, rk); }
    /**
     * @brief Slot to republish changes within robot control values
     * @param b Robot blending
     * @param s Robot maximal speed
     * @param a Robot maximal speedup
     * @param en Robto enabled
     * @param ty Robot output methode
     */
    void slotRobotControllValues(qreal b, qreal s, qreal a, bool en, char ty) { emit this->signalRobotControllValues(b, s, a, en, ty); }
    /**
     * @brief Slot to republish a changed input methode
     * @param value New input methode
     */
    void slotInputMethode(char value) { emit this->signalInputMethode(value); }
    /**
     * @brief Slot to republish changes in the client list
     * @param list List of connected client IDs
     */
    void slotDaemonState(QStringList list) { emit this->signalDaemonState(list); }
    /**
     * @brief Slot to republish changes in database control values
     * @param rc Is true, when an recording is active
     * @param rfps The current replay FPS
     */
    void slotDatabaseRecordingCrontroll(bool rc, quint8 rfps) { emit this->signalDatabaseRecordingCrontroll(rc, rfps); }
    /**
     * @brief Slot to republish changes in Kinect offsets
     * @param pos Position offset
     * @param rot Rotation offseto
     * @warning This is not supportet by the daemon anymore
     */
    void slotKinectControllValues(QVector3D pos, QQuaternion rot) { emit this->signalKinectControllValues(pos, rot); }
    /**
     * @brief Slot to republish a change regarding the robot connection
     * @param con Connectionstate
     */
    void slotUR5Connection(bool con) { emit this->signalUR5Connection(con); }
    /**
     * @brief Slot to republish the FPS between Blender->Daemon and Robot->Daemon
     * @param bfps Blender FPS
     * @param rfps Robot FPS
     */
    void slotBlenderRobotFPS(int bfps, int rfps) { emit this->signalBlenderRobotFPS(bfps, rfps); }
    /**
     * @brief Republishes the gripper status
     * @param data Status
     */
    void slotGripperControlValues(QByteArray data) { emit this->signalGripperControlValue(data); }
    /**
     * @brief This slot stores the gripper state for the Blender-C-Interface
     * @param gs
     */
    void slotBlenderCommunication(char gs, QVector3D tcp) { this->gstate = gs; this->ur5lasttcp = tcp; }
    void slotBlenderCommunicationGS(char gs) { this->gstate = gs; }
    void slotBlenderCommunicationRO(QVector3D tcp) { this->ur5lasttcp = tcp; }
    void slotFTSensorValues(QVector3D f, QVector3D t);

  private:
    /**
     * @brief Connects the signals and slots
     */
    void initializeConnections();
    /**
     * @brief Holding the underlying network
     */
    iModSkynet* network;
    /**
     * @brief Gripper state
     */
    char gstate;
    /**
     * @brief Holds the last TCP
     */
    QVector3D ur5lasttcp;
    /**
     * @brief This variable is true, if the controller is started from blender
     */
    bool starting_from_blender;
    QVector3D ftsensor_f;
    QVector3D ftsensor_t;
#ifdef WIN64
    /**
     * @brief Holding the Kinect instance
     */
    iModKinectV2* kinect;
    /**
     * @brief Instance of the kinectthread
     */
    QThread* kinectthread;
#endif
};

#endif // IMODCONTROLL_H
