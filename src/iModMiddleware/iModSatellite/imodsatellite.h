/**
* @file iModSatellite/imodsatellite.h
*
* @brief Implements the iModControll Library
*/

#ifndef IMODSATELLITE_H
#define IMODSATELLITE_H

#include <QObject>
class iModControl;

/**
* @class iModSatellite
*
* @brief Main class of the Satellite
*
* This class initializes an instance of the Controller, connects to the Daemon and publishes Kinect-tracking-data
*/
class iModSatellite : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModSatellite
     * Creates the controller and connects the required signals
     * @param parent The parent of this class
     */
    explicit iModSatellite(QObject *parent = 0);
    /**
     * @brief Destructior if iModSatellite
     */
    ~iModSatellite();
    /** @brief Connets to the Daemon */
    void connectDemon();
    /** @brief Starts the Kinect within the Control library */
    void startKinect();

  signals:

  private slots:
    /**
     * @brief Recives the permissions from the Daemon
     * @param rc Id of this client
     * @param rk Id of the root Kinect
     */
    void slotDaemonControll(quint8 rc, quint8 rk);

  private:
    /**
     * @brief clientid The ID of the client
     */
    qint16 clientid;
    /**
     * @brief rootkinect ID of the current root Kinect
     */
    quint8 rootkinect;
    /**
     * @brief controller Instance of the initialized controller
     */
    iModControl* controller;
};

#endif // IMODSATELLITE_H
