/*
 * MatlabController.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: ddvogt
 */

#include "MatlabController.h"
#include "matrix.h"

using namespace iMod;

MatlabController::MatlabController(std::string mFilePath) : MatlabInstance(mFilePath) {
}

MatlabController::~MatlabController() {
}


MatrixMap MatlabController::getMatrix(std::string variable){
	const mxArray *tmp = engGetVariable(ep,variable.c_str());
	int rows = mxGetM(tmp);
	int cols = mxGetN(tmp);

	//std::cout<<variable<<" -> Rows:"<<rows<<std::endl;
	//std::cout<<variable<<" -> Cols:"<<cols<<std::endl;

	double *dataPtr = mxGetPr(tmp);//TODO: This variable is never freed --> Memory Leak!
	//std::cout<<"Got a pointer"<<std::endl;
    
	return MatrixMap(&(dataPtr[0]),rows,cols);
}

void MatlabController::setMatrix(std::string paramName, double* values, int rows, int cols){
    mxArray *T = NULL;

	T = mxCreateDoubleMatrix(rows, cols, mxREAL);
    memcpy((void *)mxGetPr(T), (void *)values, sizeof(double)*rows*cols);

    engPutVariable(ep, paramName.c_str(), T);
}

VectorMap MatlabController::getVector(std::string variable){
	const mxArray *tmp = engGetVariable(ep,variable.c_str());
    if(tmp == NULL)
        UT_NOTIFY(LV_ERROR,"Matlab variable not found.");
        
    int rows = mxGetM(tmp);
	//int cols = mxGetN(tmp);

	double *dataPtr = mxGetPr(tmp);//TODO: This variable is never freed --> Memory Leak!
	//std::cout<<variable<<" -> Rows:"<<rows<<std::endl;
	//std::cout<<variable<<" -> Cols:"<<cols<<std::endl;

	return VectorMap(&(dataPtr[0]),rows);
}

// void MatlabController::setVector(std::string variable, std::vector<double> &data){
// 	std::stringstream newValues;
// 	newValues<<variable<<" = [";
// 	for(unsigned int i=0;i<data.size();++i)
// 		newValues<<data[i]<<" ";

// 	newValues<<"]';";

// 	evaluate(newValues.str());
// }

void MatlabController::evaluate(std::string s){
	//std::cout<<"Matlab::Evaluating: "<<s<<std::endl;
	engEvalString(ep, s.c_str());
}

// std::string MatlabController::createMatlabStringFromDouble(std::string matlabVariable, double* array, int length){
// 	std::stringstream newValues;

// 	newValues<<matlabVariable<<" = [";
// 	for(int i=0;i<length;++i){
// 		newValues<<array[i]<<" ";
// 	}
// 	newValues<<"]';";

// 	return newValues.str();
// }

// std::string MatlabController::createMatlabDataStringFromPoseContainer(bool latestOnly){
// 	Poses_t::iterator it = poseContainer.begin();
// 	std::stringstream newValues;
// 	newValues<<"A = [";
// 	int runLength = poseContainer.size();
// 	if(latestOnly)runLength=1;

// 	for(int i=0;i<runLength;++i){
// 		it++;
// 		for(int j=0;j<dimPose;j++)
// 			newValues<<(*it)[j]<<" ";
// 	}
// 	newValues<<"];";

// 	return newValues.str();
// }

void MatlabController::setParameterList(std::string paramName, MatlabParametersPtr parameters){
	mxArray *T = NULL;

	const char *field_names[] = {"ParameterName","Value"};
    mwSize dims[2] = {1, (unsigned long)parameters->getLength()};

	// 1-by-n array of struct
	T = mxCreateStructArray(2, dims, 2, field_names);

	// filling the matlab struct-array
    int j=0;
    for(const auto i:parameters->get()){
        mxSetFieldByNumber(T, j, 0, mxCreateString(i.first.c_str()));
        mxSetFieldByNumber(T, j, 1, mxCreateString(i.second.c_str()));
        j+=1;
    }
    
    engPutVariable(ep, paramName.c_str(), T);
}
