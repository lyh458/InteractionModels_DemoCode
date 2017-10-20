/**
* @file iModDaemon/imodart.h
* @brief This file contains the main class for publishing an A.R.T.-Tracking-Stream from various sources
*/

#ifndef IMODART_H
#define IMODART_H

#include <QObject>
#include <QtCore/QStringList>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>
#include <QtCore/QDebug>

// DEBUG
#include "Kinect/imodkinect.h"

class QUdpSocket;
class QTcpSocket;
class QTcpServer;
class iModUtile;
class iModARTClient;
class iModDatabase;
class QTimer;
class iModRingBuffer;
class QFile;
class iModKinect;
/**
 * @class iModART
 * @brief The iModART class
 */
class iModART : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModART
     * @param _utile Pointer to iModUtile
     * @param _db Pointer to iModDatabase
     * @param _kn Pointer to iModKinect
     * @param parent Parent of this class
     */
    explicit iModART(iModUtile* _utile, iModDatabase* _db, iModKinect* _kn, QObject *parent = 0);
    ~iModART();

    /**
     * @brief Changes the input methode
     * @param value 0x00 A.R.T., 0x01 Kinect, 0x02 Database
     * @warning Calling this function will discard unsaved recordings.
     */
    void changeInputMethode(char value);
    /**
     * @brief Gets the current input methode
     * @return Input methode
     */
    char getInputMethode();
    /**
     * @brief Starts the recording and discards previous recordings.
     */
    void startRecording();
    /**
     * @brief Stops the current recording and writes it to the database.
     * @param category Destination category.
     * @param name Destination table.
     */
    void writeRecording(QString category, QString name);
    /**
     * @brief Stops the current recording
     */
    void stopRecording();
    /**
     * @brief When recording, stops it, when not, starts it
     */
    void toggleRecording();
    /**
     * @brief Sets the replay FPS for the database.
     * @param _fps FPS value
     */
    void setReplayFps(int _fps);
    /**
     * @brief Returns the database replay FPS
     * @return FPS value
     */
    int getReplayFps() { return replayfps; }

    bool isRecording() { return recordactive; }
    /**
     * @brief Gets the status string diyplayed on the webinterface
     * @return Statusstring
     */
    QString getStatusString();

  signals:
    /**
     * @brief This signal is emitted, when a new kinectframe is required (Kinect in other thread -> signal required)
     */
    void signalGenerateKinectFrame();
    /**
     * @brief Instructs the Kinect thread to generate the matching between ART-IDs and Kinect-IDs
     */
    void signalBuildARTMatching();

  public slots:
    /**
     * @brief This slot recives trackingdata form the A.R.T.-System
     */
    void slotRecvTrackingdata();
    /**
     * @brief This slot is used for the control connection to the real A.R.T.-Server
     */
    void slotArtControllIncomming();
    /**
     * @brief This slot is triggered, when a client connects with his A.R.T.-Control connection to the simulated A.R.T.-Server
     */
    void slotNewClientConnected();
    /**
     * @brief This slot is used to connect the daemon to the real A.R.T.-Server (Used for start/stop tracking)
     * @warning Only one control connection is allowed by the remote A.R.T.-Server (check this with dTrack-GUI)
     */
    void connectARTServer();
    /**
     * @brief Sending DTrack2 tracking start to real A.R.T.-Server
     */
    void startARTTracking();
    /**
     * @brief Sending DTrack2 tracking stop to real A.R.T.-Server
     */
    void stopARTTracking();

  private slots:
    /**
     * @brief Slot triggered by the databasetimer. Everytime it is triggered, the database is requested to offer the next frame.
     */
    void slotTimerDatabase();
    /**
     * @brief Slot triggered by kinecttimer. Everytime this slot is triggered, a new frame is requested from the kinect interface.
     */
    void slotTimerKinect();

  private:
    /**
     * @brief Sending Kinect tracking data to all clients that are connected to the simulated A.R.T.-Server
     * @param frame Frame to send
     */
    void sendKinectFrameToClients(QString frame);

    /**
     * @brief UDP socket for real A.R.T.-trackingdata
     */
    QUdpSocket* artincomming;
    /**
     * @brief TCP server for simulated A.R.T.-Server-Control
     */
    QTcpServer* artserver;
    /**
     * @brief TCP socket for real A.R.T.-Server-Control
     */
    QTcpSocket* realartserver;
    /**
     * @brief Holds a pointer to the utility class
     */
    iModUtile* utile;
    /**
     * @brief Global framecounter. This is prependet as 'fr /number/' to every package sent to the clients
     */
    quint64 framecounter;
    /**
     * @brief List of clients. Is used to trigger data publishing.
     */
    QList<iModARTClient*> clientlist;
    /**
     * @brief Is true, if the daemon only pipes the real A.R.T.-Server.
     */
    bool artasmiddleware;
    /**
     * @brief If true, the daemon computes an A.R.T.-Tracking-Stream based on the Kinect data.
     */
    bool artaskinect;
    /**
     * @brief If true, a recording from the databaseserver will be played.
     */
    bool artasdatabase;
    /**
     * @brief Holds a pointer to the database class.
     */
    iModDatabase* database;
    /**
     * @brief This timer is used to generate the replay FPS for the database.
     */
    QTimer* timerdatabase;
    /**
     * @brief This list stores the recordings in local memory and is written to the database later on.
     */
    QStringList recordinglist;
    /**
     * @brief Holds, if the recording is running.
     */
    bool recordactive;
    /**
     * @brief FPS for database recordings.
     */
    int replayfps;
    /**
     * @brief This timer runs with 30 FPS (same as the Kinects do) and triggers the generation of an A.R.T.-Stream based on the Kinects.
     */
    QTimer* timerkinect;
    /**
     * @brief Holds a pointer the the Kinect management class.
     */
    iModKinect* kinect;

    /*
    QFile* dumpfileART;
    */
};

#endif // IMODART_H
