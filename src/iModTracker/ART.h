/*
 * ART.h
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#ifndef ART_H_
#define ART_H_

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "DTrackSDK.hpp"
#include "Target.h"
#include "../utils/Configuration.h"
#include "../utils/Timer.h"


namespace iModTracker {

/*!
 * A connection class to A.R.T motion capture systems
 * */
class ART{

	bool _validSQL;
	std::string tableName;

	typedef std::map<int, TargetPtr> TargetMapT;
	TargetMapT targets;

	DTrackSDK* dt;

	/*!
	 * Flag if a server connection exists
	 * */
	bool _updateTargets;

	/*!
	 * Define if ART data is converted to meter or kept in cm
	 */
	int _convertToMeter;

	/*!
	 * Define if ART data is converted to degree or kept in radiant
	 * */
	int _convertToDeree;

	/*!
	 * Stores the local mutex to provide threadsafe container access
	 */
	boost::mutex mx;

	/*!
	* Stores the working condition variable
	*/
	boost::condition_variable _workCondition;

	/*!
	 * Thread for read from ART servers
	 * */
	boost::thread* readTargetsFromServerThread;

	/*!
	 * Definition of how many frames per second are read from the ART system
	 * */
	int _refreshRate;

public:
	ART();
	virtual ~ART();

	/*!
	* Register new targets
	* This functions reads all target ID from the global configuration file and adds them to the requested ART target list.
	*/
	virtual void registerTargets();
	/*
	* Get a target by its ID
	* @param name The id of the requested ART target. This parameter is in general the ART ID
	*/
	TargetPtr getTarget(const int& name);

	/*!
	 * Starts a new thread which updates targets
	 * */
	virtual void start();

	/*!
	 * Stop updating
	 * */
	virtual void stop();
    
    /*!
    * Start the ART data recording
    */
    void startRecording();

private:
	void messages_to_console();

	/*!
	* Create new SQl database entry for a given target
    * @param body The ART Dtrack body
	* @param _frameCounter The current frame of the behavior
	*/
    std::string createSQlTargetString(DTrack_Body_Type_d &body,  unsigned int &_frameCounter);

	/*!
	* Create an SQL string the represents the creation of a new table. 
	* The table name is created here @see iModTracker::ART::initMySQL
	*/
	std::string createSQlTableString();

	/*!
	 * Update targets.
	 * This is a blocking call
	 * */
	void update();

	/**
 * 	\brief Prints current tracking data to console.
 */
void output_to_console();

};

typedef std::shared_ptr<ART> ARTPtr;

} /* namespace iModTracker */


extern "C"
{
#ifndef WIN32
    extern void NC_InitFunc(void) __attribute__ ((constructor));
    extern void NC_TermFunc(void) __attribute__ ((destructor));
#endif

	//! Library Constructor
#ifdef WIN32
    __declspec(dllexport) void NC_InitFunc(void);

	//! Destructor
    __declspec(dllexport) void NC_TermFunc(void);

    __declspec(dllexport) void NC_getTargetPosition(int id, double& x, double& y, double& z);
    __declspec(dllexport) void NC_getTargetRotation(int id, double& x, double& y, double& z, double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    __declspec(dllexport) void NC_start();
    __declspec(dllexport) void NC_stop();
#else
    extern void NC_InitFunc(void);

    //! Destructor
    extern void NC_TermFunc(void);

    extern void NC_getTargetPosition(int id, double& x, double& y, double& z);
    extern void NC_getTargetRotation(int id, double& x, double& y, double& z, double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    extern void NC_start();
    extern void NC_stop();
#endif
}


#endif /* ART_H_ */
