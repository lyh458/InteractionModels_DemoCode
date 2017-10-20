/**
* @file iModDaemon/imodartclient.h
* @brief This file contains the implementation of an A.R.T.-Client
*/

#ifndef IMODARTCLIENT_H
#define IMODARTCLIENT_H

#include <QObject>

class QTcpSocket;
class QUdpSocket;
/**
 * @class iModARTClient
 * @brief This class handles A.R.T.-Clients
 * @details Every client that connects to the simulated A.R.T.-Server needs to use a TCP-Connection the register on the server. Normal clients do this and the UDP-address for the trackingdata is derivated from the TCP-source address.
 */
class iModARTClient : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModARTClient
     * @param _socket The initialized TCPSocket to communicate with the client.
     * @param port The port to send UDP-trackingdata to.
     * @param parent The parent of this class.
     */
    explicit iModARTClient(QTcpSocket* _socket, int port, QObject* parent =0);
    ~iModARTClient();

    /**
     * @brief Sends trackingdata to the client via UDP, if this client is active.
     * @param data Trackingdata
     */
    void sendData(QByteArray* data);

  private slots:
    /**
     * @brief Incomming messages from the client.
     * @details This can be 'DTrack2 tracking start/stop'. Based on start/stop this client is enabled. Blender currentely isn't reconnecting, just disabeling itselv with start/stop. In addition this slot sends appropriate messages to make the client think it's a real A.R.T.-server.
     */
    void slotTcpIncomming();

  private:
    /**
     * @brief TCP control socket
     */
    QTcpSocket* socket;
    /**
     * @brief UDP trackingdata socket
     */
    QUdpSocket* udpsocket;
    /**
     * @brief If true, the client is active
     */
    bool senddata;
    /**
     * @brief Destination port for trackingdata
     */
    int udpsendport;
};

#endif // IMODARTCLIENT_H
