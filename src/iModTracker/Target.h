/*
 * Target.h
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#ifndef TARGET_H_
#define TARGET_H_

#include <cstring>
#include <iostream>
#include <ostream>
#include <memory>		

namespace iModTracker {

/*!
 * Base class for a general motion capture target
 * */
class Target {

	/*!
	 * A target position
	 * This is a 3D vector
	 * */
	double *pos;

	/*!
	 * A targets rotation
	 * This is a 9D vector aka 3x3 matrix
	 * */
	double *rot;

	/*!
	 * A target's
	 * */
	std::string name;

public:
	Target();
	virtual ~Target();

	Target(const Target& cp);

	friend std::ostream& operator<< (std::ostream &out, const Target& t);

	/*!
	 * Get the rotation of the target
	 * */
	double* getPosition(){
		return pos;
	}

	/*!
	 * Get the rotation of a target
	 * */
	double* getRotation(){
		return rot;
	}

    std::string& getName(){
		return name;
	}

	/*!
	 * Set a targets rotation in radient
	 * */
	void setRotation(double* rot){
		/*this->rot[0] = rot[0];
		this->rot[1] = rot[1];
		this->rot[2] = rot[2];
		this->rot[2] = rot[2];	
		*/
		memcpy(this->rot,rot, 9 * sizeof(double));
	}

	/*!
	 * Set a targets rotation
	 * */
	void setPosition(double* pos){
		/*this->pos[0] = pos[0];
		this->pos[1] = pos[1];
		this->pos[2] = pos[2];
		*/
		memcpy(this->pos, pos, 3 * sizeof(double));
	}

	/*!
	 * Set the name of a target
	 * */
	void setName(const std::string& name){
		this->name = name;
	}
};

typedef std::shared_ptr<Target> TargetPtr;

} /* namespace iModTracker */
#endif /* TARGET_H_ */
