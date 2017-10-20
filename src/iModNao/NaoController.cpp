/*
 * NaoController.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: ddvogt
 */

#include <iostream>

#include "NaoController.h"

using namespace iMod;

NaoController::NaoController() {
	// TODO Auto-generated constructor stub

	Configuration::getInstance().startCyclicRead();

	//poseContainer = new TimedPoseContainer();
	poseContainer = new PoseContainer();
	minPostureCount = 6;//Configuration::getInstance().getParameter("Nao","ExtrapolationMinPostureCount");


}

NaoController::~NaoController() {
	// TODO Auto-generated destructor stub
}

void NaoController::intialize(std::string IP, int port) {
	std::cout<<"Connecting to Nao "<<IP<<" with port "<<port<<std::endl;
	motionPrxy = new AL::ALMotionProxy(IP, port);

	motionPrxy->wbEnable(true);

	motionPrxy->wbFootState("Plane","Legs");

	motionPrxy->wbEnableBalanceConstraint(true, "Legs");

	//motionPrxy->wbEnableEffectorOptimization("LArm", false);
    //motionPrxy->wbEnableEffectorOptimization("RArm", false);

    motionPrxy->setSmartStiffnessEnabled(true);


	//TODO: Fix the hard coded path
	matlab = new MatlabController("/Users/ddvogt/Desktop/eclipse_workspace/InteractionModels/matlab/utils/");
	matlab->initialize();
}

void NaoController::startSending(){
	boost::thread naoSendPostureThread(boost::bind(&NaoController::sendJointAngles,this));
}

void NaoController::sendJointAngles() {

	std::cout<<"Starting write robot thread"<<std::endl;

	//double frameDuration = 33; //Approximately 30FPS
	Pose_t postureCurrent(26,0);

	boost::posix_time::time_duration timeDuration;

	std::vector<std::string> names = motionPrxy->getJointNames("Body");
	AL::ALValue timeLists;
	AL::ALValue angleLists;
	double interpolationDuration = 0.250; //4 FPS

	for (;;) {
//		if (!poseContainer->isContainerFilled()) {
//			//boost::this_thread::sleep(boost::posix_time::seconds(1));
//			sleep(1);
//			continue;
//		}


		if (poseContainer->getPostureContainerSize() < minPostureCount) {
			//boost::this_thread::sleep(boost::posix_time::seconds(1));
			sleep(1);
			continue;
		}

		interpolationDuration = 1000/Configuration::getInstance().getParameter<double>("Nao","angleInterpolationTime");

		//Get the posture that will be send to the robot
		//postureCurrent = poseContainer->getLastPose();
		//Pose_t lastPose = postureCurrent;
		//extrapolate(postureCurrent);

		postureCurrent = poseContainer->getLastPose();

		unsigned int j = 0;
		for (j = 0; j < 26; j++){
			//angleLists.arrayPush(postureCurrent[j]-lastPose[j]);

			//Disable the legs
			timeLists.arrayPush(interpolationDuration);
			if (j<8 || j>19)
				angleLists.arrayPush(postureCurrent[j]);
			else
				angleLists.arrayPush(0);
		}

		//Send the posture to the robot
		try {
			//motionPrxy->changeAngles(names,angleLists,0.92f);
			//motionPrxy->setAngles(names, angleLists, Configuration::getInstance().getParameter("Nao","setAnglesTimeValue"));
			motionPrxy->angleInterpolation(names, angleLists, timeLists, true);

		} catch (const AL::ALError& e) {
			std::cerr << "Caught Aldebaran exception: " << e.what() << std::endl;
		}

		//Clean up
		timeLists.clear();
		angleLists.clear();
	}

	return;
}

void NaoController::extrapolate(Pose_t &extrapolatedPosture){

	//Get a pointer to the actual data
	Poses_t p = poseContainer->getPoses();
	std::list<double> times = poseContainer->getTimes();

	//Set the variable in Matlab
	matlab->setMatrix(&p,"A");

	std::vector<double> t;
	for(std::list<double>::const_iterator i=times.begin();i != times.end();i++)
		t.push_back(*i);

	matlab->setVector("t",t);
	std::stringstream ss;
	ss<<"[ p ] = extrapolateMotion( A , t,"
			<<Configuration::getInstance().getParameter<double>("Nao","extraPolationSteps")
			<<");";
	matlab->evaluate(ss.str().c_str());
	VectorMap extra = matlab->getVector("p");

	for(int i=0;i<extra.rows();i++)
		extrapolatedPosture[i] = (double)extra[i];
}
