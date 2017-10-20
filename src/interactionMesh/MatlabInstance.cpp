/*
 * MatlabInstance.cpp
 *
 *  Created on: May 29, 2013
 *      Author: ddvogt
 */

#include "MatlabInstance.h"
#include "../utils/notify.h"
#include "../utils/Configuration.h"
#include <sstream>

namespace iMod {

MatlabInstance::MatlabInstance(std::string mFilePath) {
	// TODO Auto-generated constructor stub
	_mFilePath = mFilePath;

}

MatlabInstance::~MatlabInstance() {
	//engEvalString(ep, "close;");
}

void MatlabInstance::initialize(){
	// print out matlab stuff
	UT_NOTIFY(LV_INFO,"Initializing Matlab Engine");

	// Call engOpen with a NULL string. This starts a MATLAB process on the current host using the command "matlab".
	char* pPath;
	pPath = getenv ("IMOD_ROOT");
	if (pPath == NULL)
		UT_NOTIFY(LV_FATAL,"Could not get IMOD_ROOT");

	std::string ip = Configuration::getInstance().getParameter<std::string>("iMod","MatlabInstanceIP");
	UT_NOTIFY(LV_INFO,"Trying to start matlab on the local machine");
	if (!(ep = engOpen(NULL))) {
		UT_NOTIFY(LV_ERROR,"Can't start MATLAB engine under IP"<<ip);
	}
	else {

		engOutputBuffer(ep,buffer,MATLABOUTPUTBUFFERSIZE);

		mxArray *Btmp = NULL;
		engEvalString(ep, "B = version;");
		Btmp = engGetVariable(ep,"B");
		UT_NOTIFY(LV_DEBUG,"Matlab Version: "<<mxArrayToString(Btmp));

		//! Add required folders to the Matlab environment
		//TODO read paths from configuration parser
		//!Add everything that is in the path variable
		addPath("Matlab/");


		//addPath("Matlab/drtoolbox/");
		//addPath("Matlab/InteractionMesh/");

		//TODO!!!!
		//addPath("Matlab/dev/");

		/*
		engEvalString(ep, "addpath(P,genpath('/Users/ddvogt/Desktop/eclipse_workspace/InteractionModels/Matlab/INTERACTION_LEARNING_USING_HMM_FOR_NAO_ROBOT'))");
		engEvalString(ep, "addpath(P,genpath('/Users/ddvogt/Desktop/eclipse_workspace/InteractionModels/Matlab/drtoolbox'))");
		engEvalString(ep, "addpath(P,genpath('/Users/ddvogt/Desktop/eclipse_workspace/InteractionModels/Matlab/MatlabMoCapTools'))");
		engEvalString(ep, "addpath(P,genpath('/Users/ddvogt/Desktop/eclipse_workspace/InteractionModels/Matlab/InteractionMesh'))");
		 */
		mxArray *p = NULL;
		engEvalString(ep, "P = path;");
		p = engGetVariable(ep,"P");
		UT_NOTIFY(LV_DEBUG,"Matlab paths: "<<mxArrayToString(p));
	}
}

void MatlabInstance::addPath(std::string path){

	//!Get IMOD_ROOT path
	char* pPath;
	pPath = getenv ("IMOD_ROOT");
	if (pPath == NULL)
		UT_NOTIFY(LV_FATAL,"Could not get IMOD_ROOT");

	std::stringstream ss;
//	ss<<"addpath(genpath('/vol/vrrender/demos/OSGApplications_new/InteractionModels/Matlab/'));";
	ss<<"addpath(genpath('"<<pPath<<path<<"'));";
	engEvalString(ep,ss.str().c_str());
	UT_NOTIFY(LV_DEBUG,ss.str());
}

} /* namespace iMod */
