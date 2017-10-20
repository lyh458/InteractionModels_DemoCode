/*
 * NaoControllerCInterface.cpp
 *
 *  Created on: May 21st, 2012
 *      Author: ddvogt
 */

#include <iostream>

#include "NaoController.h"

extern "C"{
	extern void NC_InitFunc(void) __attribute__ ((constructor));
	extern void NC_TermFunc(void) __attribute__ ((destructor));

	//! Library Constructor
	void NC_InitFunc(void){
		naoController = new iMod::NaoController();
	}

	//! Destructor of our Nao controller
	void NC_TermFunc(void){
		delete naoController;
	}

	//! Initialize the connection to Nao and start the sending thread
	void NC_initialize(char* IP, int port){
		std::cout<<"connecting to: "<<std::string(IP)<<std::endl;
		naoController->intialize(std::string(IP),port);
		naoController->startSending();
	}

	//! Add a posture to the pose container
	void NC_sendJointAngles(double* pose, int size){

		//If the timer has not been initialized, then do it now
		if(timer == NULL)
			timer = new iMod::Timer();

		iMod::Pose_t p;
		p.resize(size);

		for(int i=0;i<size;++i)
			p[i] = pose[i];

		naoController->poseContainer->addPose(p,false,timer->elapsed());
		timer->reset();
	}
}
