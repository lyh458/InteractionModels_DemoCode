/**
* @file iModControll/imodnetwork.h
* @brief Contains the networking classes for clients
*/

#ifndef IMODNETWORK_H
#define IMODNETWORK_H

#include "../iModProtocol/iMod.protocol.h"
#include <QtNetwork/QHostAddress>

#ifdef WIN64
#include <cstdint>
#endif

class QTcpSocket;
class QUdpSocket;
class QTimer;
/**
 * @class iModSkynet
 * @brief Subcass of generated iModProtocoll
 * @details This class implements the iModProtocoll and adds the logic behind the different network packages. The main task of this class is to distribute and create network packages from and to the daemon.
 */
class iModSkynet : public iModNetwork::NetworkBase
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModSkynet
     * @param parent The parent of this clas
     */
    explicit iModSkynet(QObject *parent = 0);
    ~iModSkynet();

    /**
     * @brief Connects to the Robot
     * @param ip Robot ip
     * @param port Robot port
     */
    void connectUR5(QString ip, int port);
    /**
     * @brief Creates a daemon control package
     * @details This function can be used for various purposes. 1. Reset daemon, 2. Force root, 3. Set root kinect, 4. Request if this client has root and the current root Kinect. Attention: The different usecases cant be combined and are computed from 1. to 4.. The first match will be considdered as desired purpose. Example: If you want to get the current root kinect, pass (false, false, -1, &boolptr, &quint8ptr). If you want to get root privileges, pass (false, true, <dont care>, <dont care>, <dont care>)
     * @param reset If true, resets the daemon.
     * @param forceroot If true, instructs the daemon to give this client root.
     * @param setrootkinect Sets the rootkinect.
     * @param root Returns, if this client has root privileges.
     * @param rootkinect Returns the current root kinect.
     * @return Returns true, of the package could be sent over the network.
     */
    bool daemonControll(bool reset, bool forceroot, qint16 setrootkinect, bool& root, quint8& rootkinect);
    /**
     * @brief Sets or gets the Kinect offsets.
     * @details This function has two usecases. You can switch between them with the use of the setvalues flag.
     * @warning The offsets are currently not used by the the daemon. If you requet them, you will get (0,0,0) and (1,0,0,0).
     * @param position Returns/Sets the position offset.
     * @param orientation Returns/Sets the position offset.
     * @param setvalues If true, sets the values.
     * @return Returns true, of the package could be sent over the network.
     */
    bool kinectControllValues(QVector3D& position, QQuaternion& orientation, bool setvalues);
    /**
     * @brief Creates a database control package.
     * @details If load is true, the specified table is loaded to the memory of the daemon. If loadfromcat is not empty, a category is loaded and the tables in this category are returned in categorylist and tablelist. In addition the current category and table are returned in the appropriate variables.
     * @warning Loading tables can take some time. The recording will start to play as soon as the loading process is done.
     * @param categorylist Returns the list of categorys.
     * @param tablelist Returns the list of tables.
     * @param cur_categroy Returns the current category.
     * @param cur_table Returns the current table.
     * @param load If true, a table with trackingdata is loaded. (If this is true, loadfromcat is ignored)
     * @param loadfromcat If not empty, a category is loaded.
     * @return Returns true, of the package could be sent over the network.
     */
    bool databaseControllValues(QStringList& categorylist, QStringList& tablelist, QString& cur_categroy, QString& cur_table, bool load, QString loadfromcat);
    /**
     * @brief Creates an A.R.T. control package
     * @details If requesttargets is set to true, the number of identified targets is returned. Be aware, that you cant start the A.R.T.-Tracking and request the number of targets at the same time.
     * @param start If true, the A.R.T.-Tracking is started, otherwise stopped.
     * @param identifiedtargets Returns the number of identified targets.
     * @param requesttargets Set this to true, if you want to request the number of targets.
     * @return Returns true, of the package could be sent over the network.
     */
    bool artControllValues(bool start, quint8& identifiedtargets, bool requesttargets);
    /**
     * @brief Creates a database control package.
     * @details If a table is provided, the last recording will be stored in the database. If setvalues is true, replayfps and recording status is written to the daemon. When no table is provided and no values are written, the function will return the current recording state and the replay fps.
     * @param recording Returns/Sets the recording status.
     * @param replayfps Returns/Sets the replay FPS.
     * @param setvalues When true, recordingstate and replayfps are set, otherwise these values will be requestet from the daemon.
     * @param category If set along with table, the last recording will be written to the database.
     * @param table If set along with category, the last recording will be written to the database.
     * @return Returns true, of the package could be sent over the network.
     */
    bool databaseRecordingControll(bool& recording, quint8& replayfps, bool setvalues, QString category, QString table);
    /**
     * @brief Creates a packet to change the input methode.
     * @details Selects the source of tracking data. Selection between A.R.T., Kinect and Database. 0x00 Real A.R.T., 0x01 Kinect, 0x02 Database and everything else will fall back to A.R.T.
     * @warning This will stop and discard every unsaved (written to database) recording.
     * @param value Returns/Sets the tracking source.
     * @param setvalue If true, the trackingsource will be written, otherwise requested.
     * @return Returns true, of the package could be sent over the network.
     */
    bool inputMethode(char& value, bool setvalue);
    /**
     * @brief Creates a robot control package.
     * @param blending Returns/Sets the blending radius.
     * @param speed Returns/Sets the maximal speed.
     * @param speedup Returns/Sets the maximal speedup.
     * @param enable Returns/Sets if the robot is enabled. (Software enable/disable on daemon side)
     * @param setvalues If true, the values will be written, otherwise read.
     * @return Returns true, of the package could be sent over the network.
     */
    bool robotControllValues(qreal& blending, qreal& speed, qreal& speedup, bool& enable, bool setvalues);
    /**
     * @brief Creates a Kinect trackingdata package.
     * @param tid Bodyid set by the Kinect for the certian skeleton.
     * @param targets Number of targets contained in this package.
     * @param data Serialized trackingdata.
     * @param kinectid ID of this kinect.
     * @return Returns true, of the package could be sent over the network.
     */
    bool kinectTrackingData(quint64 tid, quint8 targets, QByteArray data, quint8 kinectid);
    bool kinectObjectTracking(QByteArray data, quint8 kinectid);
    /**
     * @brief Creates a package to set the robot output mode.
     * @param mode New mode: 0x00 UR5 direct, 0x01 UR5 ros, 0x02 File.
     * @return Returns true, of the package could be sent over the network.
     */
    bool robotOutputMode(char mode);
    /**
     * @brief Creates a package to set the Kinect range.
     * @param _cl List of affectet clients (Representet by ID).
     * @param mi_r Near cutting plane.
     * @param ma_r Far cutting plane.
     * @return Returns true, of the package could be sent over the network.
     */
    bool kinectRange(QList<int> _cl, qreal mi_r, qreal ma_r);
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

  signals:
    /**
     * @brief Signal to publish a recived daemon control package
     */
    void signalDaemonControll(quint8, quint8);
    /**
     * @brief Signal to publish a recived robot control package
     */
    void signalRobotControllValues(qreal, qreal, qreal, bool, char);
    /**
     * @brief Signal to publish a recived input mehtode package
     */
    void signalInputMethode(char);
    /**
     * @brief Signal to publish a recived daemon state package
     */
    void signalDaemonState(QStringList);
    /**
     * @brief Signal to publish a recived database control package
     */
    void signalDatabaseRecordingCrontroll(bool, quint8);
    /**
     * @brief Signal to publish a recived kinect control package
     */
    void signalKinectControllValues(QVector3D, QQuaternion);
    /**
     * @brief Signal to publish a recived ur5 connection package
     */
    void signalUR5Connection(bool);
    /**
     * @brief Signal to publish a recived blender and robot fps package
     */
    void signalBlenderRobotFPS(int, int);
    /**
     * @brief Signal to publish a recived kinect range package
     */
    void signalKinectRange(qreal, qreal);
    /**
     * @brief Signal used to publish gripper status
     */
    void signalGripperControlValue(QByteArray);
    /**
     * @brief Signal used to publish opencv values
     */
    void signalKinectOpenCVValues(QVector3D, QVector3D, quint8, quint8, bool);
    /**
     * @brief Signal used to publish the gripper state for Blender-C-Interface
     */
    void signalBlenderCommunication(char, QVector3D);
    void signalBlenderCommunicationGS(char);
    void signalBlenderCommunicationRO(QVector3D);
    void signalFTSensorValues(QVector3D, QVector3D);

  public slots:
    /**
     * @brief This slot gets triggered, wenn data per UDP arrive.
     * @detaisl The UDP socket is used to handel the connection-udp-broadcast.
     */
    void slotUDPIncomming();
    /**
     * @brief This slot is triggered, when the connection to the daemon is lost.
     * @details This slot starts the timer which trys to reconnect periotically and connects the required slots.
     */
    void slotDaemonDisconnected();
    /**
     * @brief Sends robot angles to the daemon
     * @param type Type 1 = Initpose, 2 = Normal movement
     * @param data Pointer to the data
     * @param length Length of data
     * @return
     */
    bool slotCIsendBlenderCommunication(char type, QByteArray qdata);
    /**
     * @brief Publishes the data from Matlab conserning the objectdata
     * @param os Ownership 1 = none, 2 = A, 3 = B, 4 = Both
     * @param pb Probability for os
     * @param cv Derivided closing value: 0 = open, 1 = closed
     */
    bool slotCIsendTriadicObjectData(quint8 os, qreal pb, qreal cv);
    /**
     * @brief Sets the gripper behaviour: 0 = Manual, 1 = Controlled by Matrlab, 2 = Controlled by FTSensor
     * @param type Type
     */
    bool setGripperAutomatic(char type);

  private slots:
    /**
     * @brief This slot is triggered to send a reconnect attempt to the daemon.
     * @details This slot is used to reconnect and to connect initially.
     */
    void slotDaemonReconnect();

  private: // reimplemented Functions
    virtual void recvAyn_DaemonControll(iModNetwork::DaemonControll* packet, bool request);
    virtual void recvAyn_RobotControllValues(iModNetwork::RobotControllValues* packet, bool request);
    virtual void recvAyn_InputMethode(iModNetwork::InputMethode* packet, bool request);
    virtual void recvAyn_DaemonState(iModNetwork::DaemonState* packet, bool request);
    virtual void recvAyn_DatabaseRecordingControll(iModNetwork::DatabaseRecordingControll* packet, bool request);
    virtual void recvAyn_KinectControllValues(iModNetwork::KinectControllValues* packet, bool request);
    virtual void recvAyn_UR5Connection(iModNetwork::UR5Connection* packet, bool request);
    virtual void recvAyn_BlenderRobotFPS(iModNetwork::BlenderRobotFPS* packet, bool request);
    virtual void recvAyn_KinectRange(iModNetwork::KinectRange* packet, bool request);
    virtual void recvAyn_GripperControlValues(iModNetwork::GripperControlValues* packet, bool request);
    virtual void recvAyn_KinectOpenCV(iModNetwork::KinectOpenCV* packet, bool request);
    virtual void recvAyn_BlenderCommunication(iModNetwork::BlenderCommunication* packet, bool request);
    virtual void recvAyn_FTSensorValues(iModNetwork::FTSensorValues* packet, bool request);

    /* These functions can implemented
    virtual void recvAyn_TriadicObjectData(iModNetwork::TriadicObjectData* packet, bool request);
    virtual void recvAyn_ARTControllValues(iModNetwork::ARTControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_DatabaseControllValues(iModNetwork::DatabaseControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_GripperControllValues(iModNetwork::GripperControllValues* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_KinectTrackingData(iModNetwork::KinectTrackingData* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    virtual void recvAyn_ReadRemoteSetting(iModNetwork::ReadRemoteSetting* packet, bool request) { Q_UNUSED(packet); Q_UNUSED(request); }
    */

  private:
    /**
     * @brief Delets an object savely.
     * @param value Pointer to object.
     */
    void safeDelete(QObject* value);
    /**
     * @brief Reads an int32 from a byte array.
     * @param data Pointer to byte array.
     * @param offset Offset added to the pointer.
     * @return Returns an int32 taken from the byte array.
     */
    int32_t readInt32(char* data, int offset = 0);
    template<typename type>
    /**
     * @brief Serializes a QList with variable type to QByteArray
     * @param list The list to be serialized
     * @return Returns the QByteArray of the list passed to the fuction.
     */
    QByteArray serializeQList(QList<type> list)
    {
      QByteArray result;
      result.resize(list.count() * sizeof(type));
      for(int i = 0; i < list.count(); i++)
        memcpy(result.data() + i * sizeof(type), &list[i], sizeof(type));
      return result;
    }

    /**
     * @brief Holds a pointer to the socket connected to the daemon
     */
    QTcpSocket* connectionsocket;
    /**
     * @brief Holds a pointer to the UDP socket used to connect/reconnect to the daemon.
     */
    QUdpSocket* udpsocket;
    /**
     * @brief Holds a pointer to the timer used to reconnect to the daemon.
     */
    QTimer* reconnecttimer;
    /**
     * @brief Holds the UDP port of the controler.
     */
    int controllerudpport;
    /**
     * @brief Holds the ip of the daemon.
     * @details This variable is set by the UDP-Boradcast-Connection and used to connect with TCP.
     */
    QHostAddress demonaddress;
};

#endif // IMODNETWORK_H
