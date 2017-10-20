/*
 * TimedPoseContainer.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: ddvogt
 */

#include "TimedPoseContainer.h"

namespace iMod {

TimedPoseContainer::TimedPoseContainer(){
	// TODO Auto-generated constructor stub
	setMaxContainerSize(2);
}

TimedPoseContainer::~TimedPoseContainer() {
	// TODO Auto-generated destructor stub
}

void TimedPoseContainer::addPose(Pose_t newPose){

	//Always check for duplicates
	if(PoseContainer::addPose(newPose,true)){
		Time_t t1 = timesAdded.back();
		timesAdded.push_back(boost::posix_time::microsec_clock::local_time());

		std::cout<<"Posture added ";
		for(int i=0;i<newPose.size();i++)
			std::cout<<newPose[i]<<" ";
		std::cout<<std::endl;

		if(timesAdded.size() > 1){
			boost::posix_time::time_duration t = timesAdded.back() - t1;
			//std::cout<<"Time since last pose: "<<boost::posix_time::to_iso_string(t) <<std::endl;
		}
	}

	if(timesAdded.size() > maxContainerItems)
		timesAdded.pop_front();
}

TimedPosture TimedPoseContainer::getLastPose(){
	TimedPosture t = {PoseContainer::getLastPose(),timesAdded.back()};
	return t;
}

bool TimedPoseContainer::getPose(int index, TimedPosture &pose){
	if(!isContainerFilled() || timesAdded.size() < index + 1)
		return false;

	// Get the time when posture @param index has been added
	Times_t::iterator i = timesAdded.begin();
	for(int j=0;j==index;j++)
		i++;

	// fill pose with the posture at position index
	Pose_t posture;
	if(PoseContainer::getPose(index,posture)){
		std::copy(posture.begin(),posture.end(),pose.pose.begin());
		std::cout<<boost::posix_time::to_simple_string(*i)<<" <-- "<<std::endl;
		pose.timeAdded = *i;
		return true;
	}
	else
		return false;

}

TimedPosture TimedPoseContainer::getFirstPose(){
	TimedPosture t = {PoseContainer::getFirstPose(),timesAdded.front()};
	return t;
}
} /* namespace iMod */
