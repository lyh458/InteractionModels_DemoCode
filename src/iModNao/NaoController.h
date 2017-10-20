/*
 * NaoController.h
 *
 *  Created on: Sep 11, 2012
 *      Author: ddvogt
 */

#ifndef NAOCONTROLLER_H_
#define NAOCONTROLLER_H_

#include <alproxies/almotionproxy.h>
#include <alerror/alerror.h>

#include "../interactionMesh/TimedPoseContainer.h"
#include "../interactionMesh/Timer.h"
#include "../interactionMesh/MatlabController.h"
#include "../utils/Configuration.h"

namespace iMod{

class NaoController {

	AL::ALMotionProxy *motionPrxy; ///< Stores the connection to the Nao robot
	int minPostureCount; ///< Stores the minium amount of posture required to run the extrapolation

public:
	NaoController();
	virtual ~NaoController();

	/*!
	 * Initializes a connection to the robot
	 * @param IP The robots IP address
	 */
	void intialize(std::string IP, int port);

	/*!
	 * Start the sending thread and begin sending postures.
	 * This function creates a new thread and returns after done so.
	 */
	void startSending();


private:

	/*!
	 * Uses the pose container to send joint angles to the robot
	 * This is a blocking call.
	 */
	void sendJointAngles();

	/*!
	 * Extrapolate joint angles to make a future prediction
	 * @param extrapolatedPosture The posture that has been calculated.
	 */
	void extrapolate(Pose_t &extrapolatedPosture);


private:
	MatlabController *matlab;

public:
	//TimedPoseContainer* poseContainer; //!< Public access to the container which stores all read/sent postures
	PoseContainer* poseContainer;
};
}

extern "C"{
	extern void NC_InitFunc(void) __attribute__ ((constructor));
	extern void NC_TermFunc(void) __attribute__ ((destructor));

	static iMod::NaoController* naoController;
	static iMod::Timer* timer;

	//! Library Constructor
	void NC_InitFunc(void);

	//! Destructor of our Matlab controller
	void NC_TermFunc(void);

	//! Initialize the connection to Nao and start the sending thread
	void NC_initialize(char* IP, int port);

	//! Add a posture to the pose container
	void NC_addPosture(double* pose, int size);
}

#endif /* NAOCONTROLLER_H_ */
