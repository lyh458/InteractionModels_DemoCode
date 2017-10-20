#include "BlenderController.h"


using namespace iModHaptic;

BlenderController::BlenderController(){


	_blendercontroller=new iMod::BlenderController();

}

BlenderController::~BlenderController(){

}

void BlenderController::init(){




				std::string blenderHost = iMod::Configuration::getInstance().getParameter<std::string>("iModOSG","BlenderHost");
				std::string numberSphere = iMod::Configuration::getInstance().getParameter<std::string>("Haptic","numberSphere");

				_blendercontroller->initialize(59918,blenderHost);
				int sphere_number = std::stoi(numberSphere);
				boost::thread readBlender(boost::bind(&iMod::BlenderController::readBlenderPoses, _blendercontroller, (sphere_number)*3)); //read String sent from TCPServer




}
iMod::Pose_t BlenderController::getPosition( ){






		if (_blendercontroller->poseContainer->isContainerFilled()==true){

				iMod::Pose_t actualpose=_blendercontroller->poseContainer->getLastPose();

			 return actualpose;


	}

}
