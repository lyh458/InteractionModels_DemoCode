/*
 * TimedPoseContainer.h
 *
 *  Created on: Feb 28, 2013
 *      Author: ddvogt
 */

#ifndef TIMEDPOSECONTAINER_H_
#define TIMEDPOSECONTAINER_H_

#include "Timer.h"
#include "PoseContainer.h"

namespace iMod {

///< Define a standard time type
typedef boost::posix_time::ptime Time_t;

///< Define a standard list of time stamps
typedef std::list<Time_t> Times_t;

/*!
 * A simple timed posture structure
 * */
struct TimedPosture{
	Pose_t pose;
	Time_t timeAdded; ///< Time when the pose has been added to the container

	friend std::ostream& operator<< (std::ostream &out, TimedPosture &p){
		out<<boost::posix_time::to_simple_string(p.timeAdded)<<" ";
		for(unsigned int i=0;i<p.pose.size();i++)
			out<<p.pose[i]<<" ";
		return out;
	}
};

class TimedPoseContainer: public iMod::PoseContainer {

	Times_t timesAdded;
public:
	TimedPoseContainer();
	virtual ~TimedPoseContainer();

	/*!
	 * Adds a new pose to the container
	 * @param newPose The pose that will be added to the container
	 */
	void addPose(Pose_t newPose);

	/*!
	 * returns the last added pose in the container
	 * @return the actual posture that is returned
	 */
	TimedPosture getLastPose();

	/*!
	 * Get a specific posture
	 * @param index The index of the posture which is to be returned
	 * @param pose The timed pose that will be filled with the posture at index @param index
	 * @return True if the posture has been found, e.g. the index exists
	 */
	bool getPose(int index, TimedPosture &pose);

	/*!
	 * Get the first pose in the container
	 * @return The first pose in the container
	 */
	TimedPosture getFirstPose();
};

} /* namespace iMod */
#endif /* TIMEDPOSECONTAINER_H_ */
