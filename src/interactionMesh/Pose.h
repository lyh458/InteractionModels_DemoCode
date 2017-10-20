/*
 * Pose.h
 *
 *  Created on: Mar 8, 2013
 *      Author: ddvogt
 */

#ifndef POSE_H_
#define POSE_H_

#include <vector>
#include <list>
#include <iostream>
#include <memory>

namespace iMod {

///< Defining a standard posture as a vector of doubles
typedef std::vector<double> Pose_t;
typedef std::shared_ptr<Pose_t> Pose_ptr;

std::ostream& operator <<(std::ostream& os, Pose_t &p);

///< Defining a list of individual postures as a container of poses
typedef std::list<Pose_t > Poses_t;
typedef std::shared_ptr<Poses_t> Poses_ptr;


//template<class T>
//
//class Pose {
//	std::vector<T> pose;
//
//public:
//	Pose(){}
//	virtual ~Pose(){}
//
//	inline void push_back(const T value){
//		pose.push_back(value);
//	}
//
//	inline T front(){
//		return pose.front();
//	}
//
//	inline T back(){
//		return pose.back();
//	}
//
//	inline T begin(){
//		return pose.begin();
//	}
//
//	inline T end(){
//		return pose.end();
//	}
//
//	inline int size(){
//		return pose.size();
//	}
//};

} /* namespace iMod */
#endif /* POSE_H_ */
