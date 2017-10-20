/*
 * Controller.cpp
 *
 *  Created on: Nov 9, 2012
 *      Author: ddvogt
 */

#include "Controller.h"

using namespace iMod;

Controller::Controller() {
	// TODO Auto-generated constructor stub

}

Controller::~Controller() {
	// TODO Auto-generated destructor stub
}

void Controller::pushPose(Pose_t poseA){
	if((signed int)poseContainer.size() <= lag)
		poseContainer.push_front(poseA);

	if((signed int)poseContainer.size() > lag)
		poseContainer.pop_back();
}
