//============================================================================
// Name        : mainNoGui.cpp
// Author      : David Vogt
// Version     :
// Copyright   : 2012
// Description : Main Interaction Model C++ Server
//============================================================================

#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <boost/program_options.hpp>

//#include "NaoController.h"
#include "MatlabController.h"
#include "BlenderController.h"
#include "PoseContainer.h"
#include "Timer.h"
#include "Configuration.h"

#define PI 3.14159265359
using namespace iMod;

/*!
 * The three main controllers
 * */
BlenderController* blenderControllerAgentA;
BlenderController* blenderControllerAgentB;
BlenderController* blenderControllerNao;
//NaoController* naoController;

/*!
 * The blender thread for reading the posture of agent A
 * */
void readBlenderPoseAgentA(){
	blenderControllerAgentA = new BlenderController();
	blenderControllerAgentA->initialize(50061);
	blenderControllerAgentA->readBlenderPoses();
}

/*!
 * The blender thread for reading the robots posture
 * */
void readBlenderPoseNao(){
	blenderControllerNao = new BlenderController();
	blenderControllerNao->initialize(50062);
	blenderControllerNao->readBlenderPoses();
}

/*!
 * The blender thread for sending the predicted posture of agent A
 * */
void writeBlenderPoseAgentB(){
	blenderControllerAgentB = new BlenderController();
	blenderControllerAgentB->initialize(50060);
	blenderControllerAgentB->writeBlenderPoses();
}

/*!
 * The thread for sending the posture to the robot
 * */
/*void sendPoseRobot(){
	naoController = new NaoController();
	naoController->intialize("127.0.0.1",9559);
//	naoController->intialize("192.168.2.3",9559);
	naoController->sendJointAngles();
}*/

void parseCommandlineInput(int argc, char **argv){

	namespace po = boost::program_options;
	//Setting up program options
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("NaoConfigFile", po::value<std::string>(), "set file path to a Nao config file")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    std::cout << desc << "\n";
	    return;
	}

	if (vm.count("NaoConfigFile")) {
	    std::cout << "Nao config file set to "<< vm["NaoConfigFile"].as<std::string>() << "\n";
	    Configuration::getInstance().setPath(vm["NaoConfigFile"].as<std::string>());
	} else {
	    std::cout << "Nao config file has not been set.\n";
	}
}

int main(int argc, char **argv) {

	//parseCommandlineInput(argc,argv);

	MatlabController* matlabController;	///< An abstract Matlab controller
	matlabController = new MatlabController("/Users/ddvogt/Desktop/eclipse_workspace/InteractionModels/matlab/");
	matlabController->initialize();

	std::cout<<"Creating Blender threads"<<std::endl;
	boost::thread blenderAngleReadThread(readBlenderPoseAgentA);
	boost::thread blenderAngleWriteThread(writeBlenderPoseAgentB);
	boost::thread blenderAngleReadNaoThread(readBlenderPoseNao);
	//boost::thread naoSendPostureThread(sendPoseRobot);

	Timer *timer = new Timer();
	double frameDuration = 33; //Approximately 30FPS
	while(true){
		//Check if A and B are created
		if(!blenderControllerAgentA || !blenderControllerAgentB || !blenderControllerNao)
			continue;

		//Check if we have received a posture from A
		if(!blenderControllerAgentA->poseContainer->isContainerFilled()){
			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			continue;
		}

		std::vector<double> newBPose = blenderControllerAgentA->poseContainer->getLastPose();

		//Push pose from A to matlab
		//double* matlabPose = new double[newBPose.size()];
		//for(unsigned int i=0;i<newBPose.size();i++)
		//	matlabPose[i] = newBPose[i];

		matlabController->pushPose(newBPose);
		if(!matlabController->predict())
			std::cout<<"No enough postures in Matlabs posture container"<<std::endl;

		//std::cout<<"\rMatlab computation time: "<<timer->elapsed()<<"ms --> "<<1000/timer->elapsed()<<"FPS "<<std::flush;

		//Push predicted pose to Agent B
		blenderControllerAgentB->poseContainer->addPose(matlabController->jointAnglesB);

		//Check if we have received a posture from the robot's armature
		if(!blenderControllerNao->poseContainer->isContainerFilled())
			continue;

		//Get robot inverse kinematics solution from the read-thread and push to Aldebaran API
		//std::vector<double> newNaoPose = blenderControllerNao->poseContainer->getLastPose();
		//naoController->poseContainer->addPose(newNaoPose,true);

		//Calculate how long the we need to wait in order to reach approximately 30fps
		if(timer->elapsed() < frameDuration)
			boost::this_thread::sleep(boost::posix_time::milliseconds(static_cast<unsigned int>(frameDuration - timer->elapsed())));

		timer->reset();
	}

	//delete(timer);
	return 1;
}
