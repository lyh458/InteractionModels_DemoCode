/**
* @file iModDaemon/imoddaemon.h
* @brief This file contains the main class of the daemon.
*/

#ifndef IMODDAEMON_H
#define IMODDAEMON_H

#include <QObject>

#include "../iModProtocol/iMod.protocol.h"

class QTcpSocket;
class QTcpServer;
class QUdpSocket;
class iModUtile;
class iModART;
class iModUR5;
class iModDatabase;
class QThread;
class iModControlDaemon;
class iModKinect;
class iModFTSensor;
class iModGripper;

/**
 * @calss iModDaemon
 * @brief This class is the main class of the daemon.
 * @details This class handles all clients, distributes root privileges, creates a status webpage and does general management.
 */
class iModDaemon : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModDaemon
     * @param _utile Pointer to the utility class.
     * @param parent The parent of this instance.
     */
    explicit iModDaemon(iModUtile* _utile, QObject *parent = 0);
    ~iModDaemon();
    /**
     * @brief Starts the Daemon
     * @details This function creates the TCPServer for clients to connect.
     */
    void startControll();

  signals:
    /**
     * @brief This signal is emitted, whenever the user presses the 'callibrate kinects' link on the webpage.
     */
    void signalCalibrateKinects();
    /**
     * @brief This signal is emitted, whenever the user presses the 'detect users' link on the webpage.
     */
    void signalDetectUsersKinect();

  public slots:
    /**
     * @brief Slot called, whenever a client disconnects.
     * @details This slot is connected to every client and hadles disconnections. In addition it elects a new root client, if the disconnected client had root privileges.
     * @param client Disconnected client pointer.
     */
    void slotClientDisconnected(iModControlDaemon* client);
    /**
     * @brief Slot to call, when all clients should be informed about a package.
     * @param packet The packet.
     */
    void slotSendNotificationToAll(iModNetwork::PacketBase* packet);
    /**
     * @brief This slot is used to send a notification to a single client.
     * @param cid Desired clientid
     * @param packet Packet
     */
    void slotSendNotificationToClient(int cid, iModNetwork::PacketBase* packet);

  private slots:
    /**
     * @brief Slot triggered when a new client connects to the daemon.
     * @details This function creates a new controler and gives the required privileges
     */
    void slotNewConnection();
    /**
     * @brief Slot used when a client forces root privileges.
     * @details This function removes the root privileges from the current root client and initializes the new root client.
     * @param newroot ID of new root client.
     */
    void slotDismissCurrentRoot(int newroot);
    /**
     * @brief Slot for UDP-Broadcas-Connections
     * @details When a client searches for the daemon with UDP-Broadcast the daemon will answer the client and provide its controler port.
     */
    void slotIncommingBrodcast();
    /**
     * @brief Slot used to update the FPS values on every client.
     * @param rfps Robot FPS.
     * @param bfps Blender FPS.
     */
    void slotRobotBlenderFPS(int rfps, int bfps);
    /**
     * @brief Slot used for website requests.
     * @details This slot creates the website and handels the links like calibrate Kinect and detect users.
     */
    void slotNewHttpRequest();

  private:
    /**
     * @brief Reads an in from a bytearray.
     * @param data Pointer to bytearray.
     * @param offset Offset to aply to the pointer.
     * @return Returns an int parsed from the bytearry
     */
    int readInt32(char* data, int offset);
    /**
     * @brief Updates the list of client IDs
     * @details Do not relay on these values. Use the clientlist instead of the clientidlist. This list is only vor visualisation in the View.
     */
    void updateClientIDList();
    /**
     * @brief Returns the statusstring of the daemon to be shown on the website.
     * @return Returns the statusstring.
     */
    QString getStatusString();
    /**
     * @brief Creates the website.
     * @details This function creates valid HTML and returns the page as singel string.
     * @param httpstate The HTTP status. Mostly it's 'OK'
     * @return Returns the webpage as string
     */
    QString createStatusPage(QString httpstate);

    /**
     * @brief Holds a list of all clients.
     */
    QList<iModControlDaemon*> clientlist;
    /**
     * @brief Holds a list of client IDs
     * @warning Dont relay on this list, it may not be consistant
     */
    QStringList clientidlist;
    /**
     * @brief UDP socket used to establish connections with the clients
     */
    QUdpSocket* brodcastupd;
    /**
     * @brief The TCP server accepting connections form the clients
     */
    QTcpServer* server;
    /**
     * @brief The 'webserver' which responds to status website requests.
     */
    QTcpServer* httpserver;
    /**
     * @brief The rootc controler/client
     */
    iModControlDaemon* rootcontroler;
    /**
     * @brief Holds a pointer to the utility class.
     */
    iModUtile* utile;
    /**
     * @brief Holds a pointer to the A.R.T. class
     */
    iModART* art;
    /**
     * @brief Holds a pointer to the UR5 class
     */
    iModUR5* ur5;
    /**
     * @brief Represents the thread used by the UR5 robot
     */
    QThread* ur5thread;
    /**
     * @brief Holds a pointer to the database.
     */
    iModDatabase* database;
    /**
     * @brief Holds a pointer to the Kinect
     */
    iModKinect* kinect;
    /**
     * @brief Holds a pointer to the thread where the kinect merging is running in.
     */
    QThread* kinectthread;
    /**
     * @brief Holds a pointer to the FT sensor
     */
    iModFTSensor* ftsensor;
    /**
     * @brief Holds a pointer to the thread used for the FT sensro.
     */
    QThread* ftsensorthread;
    /**
     * @brief Holds a pointer to the gripper class
     */
    iModGripper* gripper;
};

#endif // IMODDAEMON_H
