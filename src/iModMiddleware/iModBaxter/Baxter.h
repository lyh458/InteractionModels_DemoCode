/*
 * Baxter.h
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#ifndef BAXTER_H_
#define BAXTER_H_

#include <iostream>
#include <sstream>      // std::stringstream
#include <string>
#include <vector>

#include "../../utils/Configuration.h"
#include "../../utils/notify.h"

#include "robot__service__camera.h"
#include "robot__service__camera_stubskel.h"
#include "robot__service__joint.h"
#include "robot__service__joint_stubskel.h"
#include "robot__service__peripheral.h"
#include "robot__service__peripheral_stubskel.h"

#include"RobotRaconteur.h"

namespace iModBaxter {

enum ControlMode{
    positionControl = 1,
    velocityControl = 2,
    torqueControl = 3
};

/*!
 * A connection class to Baxter
 * */
class Baxter{

    /*!
     * @brief A pointer to the Baxter joint network interface via Robot Raconteur
     * */
    boost::shared_ptr<robot::service::joint::Baxter> _baxterJointService;

    /*!
     * @brief A pointer to the Baxter camera network interface via Robot Raconteur
     * */
    boost::shared_ptr<robot::service::camera::BaxterCamera> _baxterCameraService;

    /*!
     * @brief A pointer to the Baxter camera network interface via Robot Raconteur
     * */
    boost::shared_ptr<robot::service::peripheral::BaxterPeripherals> _baxterBaxterPeripheralsService;

    /**
     * @brief _serviceAdress Stores the Baxter service URI
     */
    std::string _serviceAdress;

    /**
     * @brief _servicePort The Baxter service port
     */
    int _jointServicePort;

    /**
     * @brief _peripheralsServicePort The port of the peripherals service
     */
    int _peripheralsServicePort;

public:
    /**
     * @brief Constructor
     * @param jointServerPort The network service adress of the Baxter sevice
     */
    Baxter(std::string serviceAdress = "localhost", int jointServerPort=33000, int peripheralsServicePort=330001);

    /**
     * @brief Destructor
     */
	virtual ~Baxter();

    /**
     * @brief Initializes a connection to the Baxter service
     */
    void init();

    /**
     * @brief close an exsiting Baxter connection
     */
    void close();

    /**
     * @brief getJointAngles Read joint angles from Baxter
     * @param data A reference to the joint angle data
     */
    void getJointAngles(std::vector<double> &data);

    /**
     * @brief setJointAngles Send joint angles to Baxter. This is a asynchronous call
     * @param jointAnglesRightLimb A vector containing all joint angles of Baxters right limb
     * @param jointAnglesLeftLimb A vector containing all joint angles of Baxters left limb
     */
    void setJointAngles(std::vector<double> &jointAnglesRightLimb, std::vector<double> &jointAnglesLeftLimb);

    /**
     * @brief setControlMode Change the robots control mode @see ControlMode
     * @param mode Set the mode to given setting
     */
    void setControlMode(ControlMode mode=ControlMode::torqueControl);

    /**
     * @brief There is an internal trajectory generator in position mode, and this command sets the velocity at which it sends the arms to the set-point position.
     * @param speed The input speed must be in [0, 1].
     */
    void setPositionModeSpeed(double speed = 0.3);

    /**
     * @brief Set the grippers state
     * @param gripper e.g. "left", "l" or "right","r"
     * @param position The actual pose [0,100]
     */
    void setGripperPosition(std::string gripper, double position);

    /**
     * @brief Calibrate the gripper
     * @param gripper e.g. "left", "l" or "right","r"
     */
    void calibrateGripper(std::string gripper);

    /**
     * @brief Set the grippers velocity
     * @param gripper e.g. "left", "l" or "right","r"
     * @param velocity The actual velocity [0,100]
     */
    void setGripperVelocity(std::string gripper, double velocity=50);

    /**
     * @brief Set the grippers hold force
     * @param gripper e.g. "left", "l" or "right","r"
     * @param force The actual force [0,100]
     */
    void setGripperHoldForce(std::string gripper, double force=30.0);

    /**
     * @brief Sets the gripper deadband for successful position moves. Deadband must be in [0, 100]. At startup, is set to 5.0
     * @param gripper e.g. "left", "l" or "right","r"
     * @param deadband The actual deadband [0,100]
     */
    void setGripperDeadband(std::string gripper, double deadband=5.0);

    /**
     * @brief Gets the gripper state
     * @param gripper e.g. "left", "l" or "right","r"
     * @param position The actual position [0,100] 0=closed, 100=fully open
     */
    void getGripperState(std::string gripper, float &position);

    /**
     * @brief Pan Baxters head
     * @param angle The actual angle in radian [-pi/2 +pi/2]
     */
    void panHead(double angle);

    /**
     * @brief Get head pan angle
     * @param angle The actual angle in radian [-pi/2 +pi/2]
     */
    void getHeadPanAngle(float &angle);
};

typedef std::shared_ptr<Baxter> BaxterPtr;

} /* namespace iModBaxter */



#endif /* Baxter_H_ */
