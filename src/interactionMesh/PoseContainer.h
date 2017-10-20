/*
 * PoseContainer.h
 *
 *  Created on: Sep 13, 2012
 *      Author: ddvogt
 */

#ifndef POSECONTAINER_H_
#define POSECONTAINER_H_

#include <list>
#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/thread.hpp>

#include "Pose.h"

namespace iMod {

class PoseContainer {

	/*!
	 * Stores the local mutex to provide threadsafe container access
	 */
	boost::mutex mx;

protected:
	/*!
	 * How many items will be stored in the container at a max
	 */
	uint8_t maxContainerItems;

public:
	PoseContainer();
	virtual ~PoseContainer();

	/*!
	 * Returns the latest pose in the container as a string
	 * @return the string containing the latest pose
	 */
	void operator>>(std::string& s);

	/*!
	 * Adds a new pose to the container
	 * @param newPose The pose that will be added to the container
	 * @param checkDoublicate Checks if the pose to be added has already been added. Only the last pose is compared.
	 * @param _time A time stamp when the posture has been added
	 * @return Returns true if an addition has been made otherwise false
	 */
	bool addPose(Pose_t newPose, bool checkDuplicate = true, double _time = 0);

	/*!
	 * Are there any poses in the container
	 */
	bool isContainerFilled();

	/*!
	 * returns a copy of the last added pose in the container and the deletes the on in the container (pop_back)
	 * @return the actual posture that is returned
	 */
	Pose_t getLastPose();

	/*!
	 * returns the pose that is in the container longest
	 * @return the actual posture that is returned
	 */
	Pose_t getFirstPose();

	/*!
	 * Get a specific posture in the container
	 * @param index Index of the posture to retrieve
	 * @param value Reference to where the posture will be saved
	 * @return True if the posture has been found, e.g. if the index exists
	 */
	bool getPose(int index, Pose_t &pose);

	/*!
	 * Sets maximal pose container size
	 */
	void setMaxContainerSize(int size);

	/*!
	 * Get the size of a posture in the container
	 */
	int getPostureDimension();


	/*!
	 * Get all poses in the container
	 * */
	Poses_t getPoses();

	/*!
	 * Sets all postures of the container
	 * @param poses The list of poses which will be set
	 * */
	void setPoses(std::list< std::vector<double> > poses);

	/*!
	 * Get all times in the container
	 * */
	std::list<double> getTimes();


	/*!
	 * Get the container size, e.g. how many elements are in the posture container right now
	 * */
	int getPostureContainerSize();

	/*!
	 * Get the container size, e.g. how many elements are in the times container right now
	 * */
	int getTimesContainerSize();

private:
	/*!
	 * Check for duplicate entries
	 * @param pose Pose that will be search for in the container
	 * @param checkAll whether all or only the last pose will be checked
	 * @return Returns True if a duplicate has been found
	 */
	bool hasDuplicate(Pose_t pose, bool checkAll = false);

	void removeDuplicates();

private:
	/*!
	 * Stores all poses.
	 * This container should not be manipulated externally!
	 */
	 Poses_t poses;

	 /*!
	  * Stores all times when postures have been added
	  * This container should not be manipulated externally!
	  * */
	 std::list<double> times;

};
}

#endif /* POSECONTAINER_H_ */
