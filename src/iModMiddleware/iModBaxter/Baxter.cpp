/*
 * ART.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include "Baxter.h"
#include <cstdlib>

namespace iModBaxter {

Baxter::Baxter(std::string serviceAdress, int jointServerPort, int peripheralsServicePort)
{
    this->_serviceAdress = serviceAdress;
    this->_jointServicePort = jointServerPort;
    this->_peripheralsServicePort = peripheralsServicePort;
}

Baxter::~Baxter() {
//    this->close();
}

void Baxter::init(){

    //Create URI as in "tcp://localhost:2355/{0}/BaxterJointServer/Baxter"
    std::stringstream jointServiceURI,peripheralsURI;
    jointServiceURI << "tcp://"<<_serviceAdress<<":"<<std::to_string(_jointServicePort)<<"/";
    peripheralsURI << "tcp://"<<_serviceAdress<<":"<<std::to_string(_peripheralsServicePort)<<"/";


    std::stringstream jointURI;
    jointURI<<jointServiceURI.str()<<"BaxterJointServer/Baxter";

    std::stringstream cameraURI;
    cameraURI<<jointURI.str()<<"BaxterCameraServer/Baxter";

    std::stringstream peripheralURI;
    peripheralURI<<peripheralsURI.str()<<"BaxterPeripheralServer/BaxterPeripherals";

    try{
        //Register a TCP transport
        boost::shared_ptr<RobotRaconteur::TcpTransport> tcp=boost::make_shared<RobotRaconteur::TcpTransport>();
        RobotRaconteur::RobotRaconteurNode::s()->RegisterTransport(tcp);

         //Register the service type
        RobotRaconteur::RobotRaconteurNode::s()->RegisterServiceType(boost::make_shared<robot::service::joint::robot__service__jointFactory>());
        //RobotRaconteur::RobotRaconteurNode::s()->RegisterServiceType(boost::make_shared<robot::service::camera::robot__service__cameraFactory>());
        RobotRaconteur::RobotRaconteurNode::s()->RegisterServiceType(boost::make_shared<robot::service::peripheral::robot__service__peripheralFactory>());

        UT_NOTIFY(LV_INFO, "Trying to connect to "<<jointURI.str());

        //Connect to the joint service
        _baxterJointService = RobotRaconteur::rr_cast<robot::service::joint::Baxter>(RobotRaconteur::RobotRaconteurNode::s()->ConnectService(jointURI.str(),"",boost::shared_ptr<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRObject> >(),NULL,"robot.service.joint.Baxter"));

        //Connect to the camera service
        //_baxterCameraService = RobotRaconteur::rr_cast<robot::service::camera::BaxterCamera>(RobotRaconteur::RobotRaconteurNode::s()->ConnectService(cameraURI.str(),"",boost::shared_ptr<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRObject> >(),NULL,"robot.service.camera.BaxterCamera"));

        //Connect to the peripherals service
        _baxterBaxterPeripheralsService = RobotRaconteur::rr_cast<robot::service::peripheral::BaxterPeripherals>(RobotRaconteur::RobotRaconteurNode::s()->ConnectService(peripheralURI.str(),"",boost::shared_ptr<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRObject> >(),NULL,"robot.service.peripheral.BaxterPeripherals"));
    }
    catch (std::exception& e)
    {
        UT_NOTIFY(LV_ERROR, "Error while creating a robot service connection. "<<e.what());
        return;
    }
}

void Baxter::close()
{
    try{
        //Shutdown Robot Raconteur
        RobotRaconteur::RobotRaconteurNode::s()->Shutdown();
    }
    catch (std::exception& e)
    {
        UT_NOTIFY(LV_ERROR, "Error while closing the robot service connection. "<<e.what());
        return;
    }
}

void Baxter::getJointAngles(std::vector<double> &data)
{
    if(this->_baxterJointService){
        try{
            boost::shared_ptr< RobotRaconteur::RRArray<double> > jointAngles = this->_baxterJointService->get_joint_positions();
            for(auto i=0;i<jointAngles->Length();i++)
                data.push_back(jointAngles->operator [](i));
        }
        catch (std::exception& e)
        {
            UT_NOTIFY(LV_ERROR, "Error while reading joint angles from the robot service connection. "<<e.what());
            return;
        }
    }
    else {
        UT_NOTIFY(LV_ERROR, "Error while reading joint angles from the robot service connection. No Baxter service connection created. Call init(...) first");
    }
}

void Baxter::setJointAngles(std::vector<double> &jointAnglesRightLimb, std::vector<double> &jointAnglesLeftLimb)
{
    if(this->_baxterJointService){
        try{
            boost::shared_ptr< RobotRaconteur::RRArray<double> > data_r = RobotRaconteur::AllocateRRArray<double>(jointAnglesRightLimb.size());
            boost::shared_ptr< RobotRaconteur::RRArray<double> > data_l = RobotRaconteur::AllocateRRArray<double>(jointAnglesRightLimb.size());
            for (auto i=0;i<jointAnglesRightLimb.size();i++){
                data_r->operator [](i) = jointAnglesRightLimb[i];
                data_l->operator [](i) = jointAnglesLeftLimb[i];
            }
            this->_baxterJointService->setJointCommand("right",data_r);
            this->_baxterJointService->setJointCommand("left",data_l);
        }
        catch (std::exception& e)
        {
            UT_NOTIFY(LV_ERROR, "Error while sending joint angles to the robot service. "<<e.what());
            return;
        }
    }
    else {
        UT_NOTIFY(LV_ERROR, "Error while setting joint angles from the robot service connection. No Baxter service connection present. Did you call init(...) first");
    }
}

void Baxter::setControlMode(ControlMode mode)
{
     if(this->_baxterJointService){
        switch (mode) {
        case ControlMode::positionControl:
            _baxterJointService->setControlMode(0);
            break;
        case ControlMode::velocityControl:
             _baxterJointService->setControlMode(1);
            break;
        case ControlMode::torqueControl:
             _baxterJointService->setControlMode(3);
            break;
        default:
            UT_NOTIFY(LV_WARN, "Unknown control type. Using position control instead");
            _baxterJointService->setControlMode(0);
            break;
        }
     }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting control mode. No Baxter joint service connection present. Did you call init(...) first");
    }
}

void iModBaxter::Baxter::setPositionModeSpeed(double speed)
{
    if(this->_baxterJointService){
        this->_baxterJointService->setPositionModeSpeed(speed);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting position speed mode. No Baxter joint service connection present. Did you call init(...) first");
    }
}

void Baxter::setGripperPosition(std::string gripper, double position)
{
    if(this->_baxterBaxterPeripheralsService){
        this->_baxterBaxterPeripheralsService->setGripperPosition(gripper,position);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting gripper position. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::calibrateGripper(std::string gripper)
{
    if(this->_baxterBaxterPeripheralsService){
        this->_baxterBaxterPeripheralsService->calibrateGripper(gripper);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while calibrating the gripper. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::setGripperVelocity(std::string gripper, double velocity)
{
    if(this->_baxterBaxterPeripheralsService){
        this->_baxterBaxterPeripheralsService->setGripperVelocity(gripper,velocity);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting gripper velocity. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::setGripperHoldForce(std::string gripper, double force)
{
    if(this->_baxterBaxterPeripheralsService){
        this->_baxterBaxterPeripheralsService->setGripperHoldForce(gripper,force);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting gripper force. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::setGripperDeadband(std::string gripper, double deadband)
{
    if(this->_baxterBaxterPeripheralsService){
        this->_baxterBaxterPeripheralsService->setGripperDeadband(gripper, deadband);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting gripper deadband. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::getGripperState(std::string gripper, float &position)
{
    if(this->_baxterBaxterPeripheralsService){
        position = this->_baxterBaxterPeripheralsService->getGripperPosition(gripper);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while getting gripper state. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::panHead(double angle)
{
    if(this->_baxterBaxterPeripheralsService){
        this->_baxterBaxterPeripheralsService->panHead(angle);
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while setting head angle position. No perpherals service connection present. Did you call init(...) first");
    }
}

void Baxter::getHeadPanAngle(float &angle)
{
    if(this->_baxterBaxterPeripheralsService){
        angle = this->_baxterBaxterPeripheralsService->getHeadPanAngle();
    }
    else
    {
        UT_NOTIFY(LV_WARN, "Error while getting the head angle. No perpherals service connection present. Did you call init(...) first");
    }
}

}/* namespace iModBaxter */

