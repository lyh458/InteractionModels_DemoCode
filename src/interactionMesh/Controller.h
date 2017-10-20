/*
 * Controller.h
 *
 *  Created on: Nov 9, 2012
 *      Author: ddvogt
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <list>
#include <string>
#include <vector>

#include "Pose.h"
#include "../utils/notify.h"

namespace iMod{
//!  A abstract controller class
/*!
 This class is the base for various controllers
 */

class Controller {

public:
	const static int dimPose = 57; //!< Target dimenions


protected:
	Poses_t poseContainer; //!< Previous postures A

	/*!
	 * TIME LAG --> PREVIOUS POSTURES INCLUDED IN THE PREDICTION
	 * @param @LAG=1 ONLY ONE POSTURE IS USED TO CALCULATE THE RECONSTRUCTION VECTOR
	 */
	const static int lag = 1;

public:
	Controller();
	virtual ~Controller();

	/*!
	 * Initializes the controller
	 */
	virtual void initialize() = 0;

	/*!
	 * Updates the reconstruction vector for B depending on the content of the pose container
	 * @return true if the update was successful otherwise false
	 * The prediction is made in joint space
	 */
	//virtual bool predict() = 0;

	/*!
	 * Adds pose to the container
	 * @param poseA The pose which is added to the container
	 */
	void pushPose(Pose_t pose);

	/*!
	 * Returns the dimension of the controllers architecture, e.g. size of poses
	 * @return The pose dimension, e.g. 58 DOF
	 */
	int getTargetDimension(){
		return dimPose;
	}

public:
	/*!
	 * Calculated posture of agent B
	 */
	Pose_t jointAnglesB;

};
}
#endif /* CONTROLLER_H_ */
