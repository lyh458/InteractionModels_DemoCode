/**
* @file iModDaemon/imodcontrol_daemon.h
* @brief This file contains the network communication subclass.
*/

#ifndef IMODCONTROLL_H
#define IMODCONTROLL_H

#include <QObject>
#ifdef WIN64
#include <cstdint>
#endif

#include "../iModProtocol/iMod.protocol.h"

class QTcpServer;
class QTcpSocket;
class iModUtile;
class iModART;
class iModRobot;
class iModDatabase;
class QUdpSocket;
class iModGripper;
class iModKinect;
class iModFTSensor;
/**
 * @class iModControlDaemon
 * @brief Subclass of generated networking class.
 * @details This class is responsible for the network communication, builds packages and publishes events from the clients to the different parts of the daemon. Every daemon-client has its own instance of this class.
 */
class iModControlDaemon : public iModNetwork::NetworkBase
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor for iModControlDaemon
     * @param id Client id
     * @param _controller TCP-socket to communicate with the client.
     * @param _art Holds a pointer to the A.R.T. class used to publish events to this class.
     * @param _ur5 Holds a pointer to the UR5 class used to publish events to this class.
     * @param _db Holds a pointer to the database class used to publish events to this class.
     * @param _kn Holds a pointer to the kinect class used to publish events to this class.
     * @param _utile Holds a pointer to the utility class to get information from it.
     * @param parent The parent of this instance.
     */
    explicit iModControlDaemon(int id, QTcpSocket* _controller, iModART* _art, iModRobot* _ur5, iModDatabase* _db, iModKinect* _kn, iModUtile* _utile, iModGripper* _gr, iModFTSensor* _ft, QObject *parent);
    ~iModControlDaemon();

    /**
     * @brief Removes root privileges from this clientid
     * @details If the core process in iModDaemon decides to remove the root permission, various slots are disconnected so all messages with control attempts cant be executet.
     */
    void removeroot();
    /**
     * @brief Initializes the client and connects the minimal required signals.
     */
    void init();
    /**
     * @brief Gets the current client ID
     * @return Client ID
     */
    int getClientID() { return clientid; }
    /**
     * @brief Sets the client ID
     * @details Based on the new value, the initialization for root is done, or the root privileges are removed.
     * @param value New ID
     */
    void setClientID(int value);
    /**
     * @brief Sends a notification to this client.
     * @details The name is a bit missleading, but when the daemon decides to send a message to all clients, this function is called on every client.
     * @param packet The package to send.
     */
    void sendNotificationAll(iModNetwork::PacketBase* packet);
    /**
     * @brief Returns the default client ID.
     * @details The client ID is the last byte of the IP. This id will stay the same all the time. If this client looses root privileges, it will return to its IP-ID. The real client ID can be 0 to show this client is root.
     * @return Client ID
     */
    int getIP();

  signals:
    /**
     * @brief This signal is emitted to publish a UR5Control package if this client is root.
     */
    void signalUR5Connect(QString, int);
    /**
     * @brief This signal is emitted to publish a UR5FPSChanged package if this client is root.
     * @warning Currently not used.
     */
    void signalUR5FPSChanged(int);
    /**
     * @brief This signal is emitted to publish Kinect tracking data.
     */
    void signalUpdateKinectData(quint64, quint8, QByteArray, quint8);
    /**
     * @brief This signal is emitted to publish a start recording package if this client is root.
     * @warning Currently not used. Realized in another way.
     */
    void signalStartRecording();
    /**
     * @brief This signal is emitted to publish a write recording package if this client is root.
     * @warning Currently not used. Realized in another way.
     */
    void signalWriteRecording(QString, QString);
    /**
     * @brief This signal is emitted to publish a stop recording package if this client is root.
     * @warning Currently not used. Realized in another way.
     */
    void signalStopRecording();
    /**
     * @brief This signal is emitted to publish an enable robot package if this client is root.
     */
    void signalSendFramesRobot(char);
    /**
     * @brief This signal is emitted when a client disconnected.
     */
    void signalDisconnect(iModControlDaemon*);
    /**
     * @brief This signal is emitted to force to daemon main process to give this client root.
     */
    void signalDismissCurrentRoot(int);
    /**
     * @brief This signal is emitted to inform the daemon main process to send a notification to all clients.
     */
    void signalSendNotificationAll(iModNetwork::PacketBase* );
    /**
     * @brief This signal is emitted to inform the daemon main process to send a notification to a specific client.
     */
    void signalSendNotificationToClient(int, iModNetwork::PacketBase* );
    /**
     * @brief This signal is emitted to publish a package to set the UR5 output if this client is root.
     */
    void signalUR5Output(char value);
    /**
     * @brief This signal is emitted to publish a new root Kinect if this client is root.
     */
    void signalSetCurrentRootKinect(quint8);
    /**
     * @brief This signal is emitted to set new Kinect offsets if this client is root.
     * @warning Not used anymore
     */
    void signalSetKinectOffsets(QVector3D, QQuaternion);
    /**
     * @brief Signal emitted to transfere gripper values;
     */
    void signalGripperControlValues(QByteArray);
    /**
     * @brief Connect with the gripper
     */
    void signalConnectGripper(QString);
    /**
     * @brief Provide data from object tracker
     */
    void signalKinectObjectTracking(QByteArray, quint8);
    /**
     * @brief Signal used to instruct the robot to move to the given init pose
     */
    void signalUR5MoveToInit(QByteArray);
    /**
     * @brief Normal movement command for UR5 robot
     */
    void signalUR5Move(QByteArray);
    /**
     * @brief Used for triadic object information
     */
    void signalTriadicObjectData(quint8, qreal, qreal);
    /**
     * @brief Sets the gripper automation type
     */
    void signalGripperAutomatic(quint8);

  public slots:
    /**
     * @brief This slot informs the daemon that this client has disconnectet and can be destroyed.
     */
    void slotControllerDisconnected();

  private slots:
    /**
     * @brief This slot sends the connection state of the UR5 to the client.
     * @param con Connection state.
     */
    void slotUR5ConnectionState(bool con);
    /**
     * @brief Publis gripper status
     * @param data Status
     */
    void slotGripperStatus(QByteArray data);
    /**
     * @brief Publish FTSensor status
     * @param f Force
     * @param t Torque
     */
    void slotFTSensorValues(QVector3D f, QVector3D t);
    /**
     * @brief Used to publish the opening/closing state to all clients. In addition, the current Robot-TCP is published
     * @param gs Gripper state
     */
    void slotGripperState(char gs);
    /**
     * @brief Used to publish the current robot TCP to all clients
     */
    void slotPublishRobotTCP();

  private:
    /**
     * @brief This function connects all signals required for a root client.
     */
    void initroot();

  private: // Reimplemented Functions
    virtual void recvSyn_DaemonControll(iModNetwork::DaemonControll* packet, bool request);
    virtual void recvAyn_DaemonControll(iModNetwork::DaemonControll* packet, bool request);
    virtual void recvSyn_DatabaseControllValues(iModNetwork::DatabaseControllValues* packet, bool request);
    virtual void recvAyn_DatabaseControllValues(iModNetwork::DatabaseControllValues* packet, bool request);
    virtual void recvSyn_RobotControllValues(iModNetwork::RobotControllValues* packet, bool request);
    virtual void recvAyn_RobotControllValues(iModNetwork::RobotControllValues* packet, bool request);
    virtual void recvSyn_InputMethode(iModNetwork::InputMethode* packet, bool request);
    virtual void recvAyn_InputMethode(iModNetwork::InputMethode* packet, bool request);
    virtual void recvAyn_DatabaseRecordingControll(iModNetwork::DatabaseRecordingControll* packet, bool request);
    virtual void recvSyn_DatabaseRecordingControll(iModNetwork::DatabaseRecordingControll* packet, bool request);
    virtual void recvAyn_KinectControllValues(iModNetwork::KinectControllValues* packet, bool request);
    virtual void recvAyn_ARTControllValues(iModNetwork::ARTControllValues* packet, bool request);
    virtual void recvAyn_UR5Connection(iModNetwork::UR5Connection* packet, bool request);
    virtual void recvAyn_KinectTrackingData(iModNetwork::KinectTrackingData* packet, bool request);
    virtual void recvAyn_UR5Output(iModNetwork::UR5Output* packet, bool request);
    virtual void recvAyn_KinectRange(iModNetwork::KinectRange* packet, bool request);
    virtual void recvAyn_GripperControlValues(iModNetwork::GripperControlValues* packet, bool request);
    virtual void recvAyn_KinectObjectTracking(iModNetwork::KinectObjectTracking* packet, bool request);
    virtual void recvAyn_KinectOpenCV(iModNetwork::KinectOpenCV* packet, bool request);
    virtual void recvAyn_BlenderCommunication(iModNetwork::BlenderCommunication* packet, bool request);
    virtual void recvAyn_TriadicObjectData(iModNetwork::TriadicObjectData* packet, bool request);
    virtual void recvAyn_GripperAutomatic(iModNetwork::GripperAutomatic* packet, bool request);

    /* These functions needs to be implemented
    virtual void recvSyn_ARTControllValues(iModNetwork::ARTControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_BlenderRobotFPS(iModNetwork::BlenderRobotFPS* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_BlenderRobotFPS(iModNetwork::BlenderRobotFPS* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_GripperControllValues(iModNetwork::GripperControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_GripperControllValues(iModNetwork::GripperControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_KinectControllValues(iModNetwork::KinectControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_KinectTrackingData(iModNetwork::KinectTrackingData* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_ReadRemoteSetting(iModNetwork::ReadRemoteSetting* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_ReadRemoteSetting(iModNetwork::ReadRemoteSetting* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_UR5Connection(iModNetwork::UR5Connection* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvSyn_UR5Output(UR5Output* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    */

  private:
    template<typename type>
    /**
     * @brief Converts a bytearray to a list of given type.
     * @param data Serialized list of type 'type'.
     * @return Returns the list.
     */
    QList<type> deserializeQList(QByteArray data)
    {
      QList<type> result;
      type dummy;
      int datacount = data.count() / sizeof(type);
      for(int i = 0; i < datacount; i++)
      {
        memcpy(&dummy, data.data() + i * sizeof(type), sizeof(type));
        result.append(dummy);
      }
      return result;
    }

    /**
     * @brief Holds the TCPSocket for communication with the client.
     */
    QTcpSocket* controller;
    /**
     * @brief Holds the pointer to the utility class.
     */
    iModUtile* utile;
    /**
     * @brief Holds a pointer to the A.R.T. class
     */
    iModART* art;
    /**
     * @brief Holds a pointer to the UR5 class
     */
    iModRobot* robot;
    /**
     * @brief Holds a pointer to the database class
     */
    iModDatabase* database;
    /**
     * @brief Holds the client id
     */
    quint8 clientid;
    /**
     * @brief Holds a pointer to the Kinect class.
     */
    iModKinect* kinect;
    iModGripper* gripper;
    iModFTSensor* ftsensor;
};

#endif // IMODCONTROLL_H
