/*
 * MatlabInstance.h
 *
 *  Created on: May 29, 2013
 *      Author: ddvogt
 */

#ifndef MATLABINSTANCE_H_
#define MATLABINSTANCE_H_

#include "engine.h"

#include "Controller.h"

#define MATLABOUTPUTBUFFERSIZE 25000

namespace iMod {

class MatlabInstance : public Controller {

	/*!
	 * Path to the work directory
	 */
	std::string _mFilePath;

	/*!
	* Matlabs output buffer
	*/
	char buffer[MATLABOUTPUTBUFFERSIZE]; 


public:
	/*!
	 * Matlab Engine instance
	 */
	Engine *ep;

public:
	MatlabInstance(std::string mFilePath);
	virtual ~MatlabInstance();

	/*!
	 * Initializes the Matlab instance
	 */
	void initialize();


	/*!
	 * Add a workspace folder
	 */
	void addPath(std::string);
};

} /* namespace iMod */
#endif /* MATLABINSTANCE_H_ */
