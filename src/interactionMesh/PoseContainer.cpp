/*
 * PoseContainer.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: ddvogt
 */

#include "PoseContainer.h"

using namespace iMod;

PoseContainer::PoseContainer() {
	// TODO Auto-generated constructor stub
	maxContainerItems = 1;
	poses.clear();
}

PoseContainer::~PoseContainer() {
	// TODO Auto-generated destructor stub
	poses.clear();
}

void PoseContainer::operator>>(std::string& s) {
	boost::mutex::scoped_lock(mx);

	std::stringstream ss;
	ss.precision(8);
	Pose_t jointAngles = this->getLastPose();
	for (unsigned int i = 0; i < jointAngles.size(); i++)
		ss << jointAngles[i] << " ";
	ss << "\n";
	s = ss.str();
}

bool PoseContainer::addPose(Pose_t pose, bool checkDoublicate, double _time) {
	boost::mutex::scoped_lock(mx);

	if (checkDoublicate) {
		if (hasDuplicate(pose,false))
			return false;
	}

	poses.push_back(pose);
	times.push_back(_time);

	if (poses.size() > maxContainerItems){
		poses.pop_front();
		times.pop_front();
	}

	return true;
} //!< mutex unlocked here

bool PoseContainer::isContainerFilled() {
	boost::mutex::scoped_lock(mx);

	Poses_t::iterator i=poses.begin();
	if (i != poses.end())
		return true;
	else
		return false;
}

Pose_t PoseContainer::getLastPose() {
	boost::mutex::scoped_lock(mx);

	Pose_t v = poses.back();
	poses.pop_back();
	return v;
}

Pose_t PoseContainer::getFirstPose() {
	boost::mutex::scoped_lock(mx);
	return poses.front();
}

bool PoseContainer::getPose(int index, Pose_t &pose){
	//boost::lock_guard<boost::mutex> lock(mx);

	// Check if the pose index is existent
	if(poses.size()<index+1)
		return false;

	// fill pose with the posture at position index
	Poses_t::iterator i =  poses.begin();
	for(int j=0;j==index;j++)
		i++;

	std::copy(i->begin(), i->end(), pose.begin() );

	return true;
}

void PoseContainer::setMaxContainerSize(int size) {
	maxContainerItems = size;
}

int PoseContainer::getPostureDimension(){
	boost::mutex::scoped_lock(mx);
	return poses.begin()->size();
}

Poses_t PoseContainer::getPoses(){
	boost::mutex::scoped_lock(mx);
	return poses;
}

void PoseContainer::setPoses(std::list< std::vector<double> > _poses){
	poses = _poses;
	for(unsigned int i=0;i<_poses.size();++i)
		times.push_back(0);
}

std::list<double> PoseContainer::getTimes(){
	boost::mutex::scoped_lock(mx);
	return times;
}

bool equal (std::vector<double> &i, std::vector<double> &j) {
  return std::equal(i.begin(),i.end(),j.begin());
}

void PoseContainer::removeDuplicates(){
	std::list< std::vector<double> >::iterator it;
	it = std::unique(poses.begin(), poses.end());
	std::unique(poses.begin(), poses.end(), equal);
}

bool PoseContainer::hasDuplicate(Pose_t pose, bool checkAll) {

	if (maxContainerItems < 2)
		return false;

	if(checkAll){
		// Check for every item in the pose container if it matches the given pose
		for (Poses_t::iterator i = poses.begin(); i != poses.end(); ++i)
			if(std::equal(i->begin(),i->end(),pose.begin()))
				return true;
	}
	else{
		// Check only for the last item in the pose container if it matches the given pose
		Pose_t p = getLastPose();
		return std::equal(p.begin(),p.end(),pose.begin());
	}
}

int PoseContainer::getPostureContainerSize(){
	return poses.size();
}

int PoseContainer::getTimesContainerSize(){
	return times.size();
}
