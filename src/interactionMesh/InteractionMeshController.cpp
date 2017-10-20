/*
 * InteractionMeshController.cpp
 *
 *  Created on: Apr 10, 2013
 *      Author: ddvogt
 */

#include "InteractionMeshController.h"
#include "../utils/Configuration.h"
#include "../utils/notify.h"
#include "../utils/Timer.h"

namespace iMod {

InteractionMeshController::InteractionMeshController(std::string mFilePath) : iMod::MatlabController(mFilePath){
	// TODO Auto-generated constructor stub
	//jointsCount = 18;
}

InteractionMeshController::~InteractionMeshController() {
	// TODO Auto-generated destructor stub
}

void InteractionMeshController::initialize(){
    //Call base class initialization function to get Matlab up and running
    MatlabController::initialize();

    //Call prepareIMPrediction in Matlab to load meshes beforehand
    char* path;
    path = getenv("IMOD_ROOT");
    if (path == NULL)
        UT_NOTIFY(LV_FATAL,"Could not get IMOD_ROOT");

    std::stringstream s,ImodTMP,alg,ImodTMPVideo;
    ImodTMP<<path<<Configuration::getInstance().getParameter<std::string>("iMod","meshFilePath");
    ImodTMPVideo<<path<<Configuration::getInstance().getParameter<std::string>("iMod","meshFilePathVideo");

    alg<<Configuration::getInstance().getParameter<std::string>("iMod","MeshSelectionAlgorithm");
    UT_NOTIFY(LV_INFO,"Using selection algorithm :"<<alg.str());

    //Set parameter list
    std::vector< std::string> parameters = Configuration::getInstance().getParameterList("Matlab");
    MatlabParametersPtr parametersPtr(new MatlabParameters());
    for(auto i:parameters)
        parametersPtr->add(i,Configuration::getInstance().getParameter<std::string>("Matlab", i));
    
    setParameterList("parameterList", parametersPtr);

    s<<"prepareIMPrediction(['"<<ImodTMP.str()<<"'],['"<<ImodTMPVideo.str()<<"'],'"<<alg.str()<<"',parameterList);";
    UT_NOTIFY(LV_INFO,s.str());

    engEvalString(ep, s.str().c_str());
}
    
bool InteractionMeshController::createInteractionMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate){
	try {
		setMatrix("X",X,size,1);
		setMatrix("Y",Y,size,1);
		setMatrix("Z",Z,size,1);
		setMatrix("JointAngles",jointAngles,size,1);

//		engEvalString(ep, createMatlabStringFromDouble("X",X,size).c_str());
//		engEvalString(ep, createMatlabStringFromDouble("Y",Y,size).c_str());
//		engEvalString(ep, createMatlabStringFromDouble("Z",Z,size).c_str());
//		engEvalString(ep, createMatlabStringFromDouble("JointAngles",jointAngles,sizeAngles).c_str());

		char* path;
		path = getenv("IMOD_ROOT");
		if (path == NULL)
			UT_NOTIFY(LV_FATAL,"Could not get IMOD_ROOT");

		std::stringstream s;
		s<<path<<Configuration::getInstance().getParameter<std::string>("iMod","meshFilePath");

		std::stringstream call;
		std::stringstream number;
		number<<std::setw(5)<<std::setfill('0')<<index;

		if(!triangulate)
            call<<"createInteractionMesh(X,Y,Z,JointAngles,"<<triangulate<<","<<"'"<<s.str()<<"mesh"<<number.str()<<".mat')";
        else
            call<<"createInteractionMesh(X,Y,Z,JointAngles,"<<triangulate<<","<<"'"<<s.str()<<"meshReference.mat')";
		engEvalString(ep, call.str().c_str());

		UT_NOTIFY(LV_INFO,"Interaction mesh created in "<<s.str()<<" triangulation="<<triangulate<<" index="<<number.str());
		
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

bool InteractionMeshController::predict(iMod::MatrixType data, int lenghtVertices, double* jointAngles, int lengthJointAngles){

    std::vector< std::string> parameters = Configuration::getInstance().getParameterList("Matlab");
    MatlabParametersPtr parametersPtr(new MatlabParameters());
    
    for(auto i:parameters)
        parametersPtr->add(i,Configuration::getInstance().getParameter<std::string>("Matlab", i));
    
    setParameterList("parameterList", parametersPtr);
    setMatrix("data",data.data(),lenghtVertices,3);

    //iMod::Timer_T<high_resolution_clock> t;
    engEvalString(ep, "[posture] = predictIM(data,parameterList);");
    //UT_NOTIFY(LV_DEBUG,"The execution took "<<t.seconds());
     
    MatrixMap posture = getMatrix("posture");
    
    
	//Update posture database
	//Reserving memory if the amount of vertices in the container changed
	if(positions.size() != lenghtVertices)
        positions.resize(lenghtVertices);
	
    for(int i = 0; i<lenghtVertices; ++i)
        positions[i] = PointPtr(new Point(posture.row(i)[0],posture.row(i)[1],posture.row(i)[2]));
    
	return true;
}

    
MatrixMap InteractionMeshController::getNetTopology(){
	// char* path;
	// path = getenv("IMOD_ROOT");
	// if (path == NULL)
	// 	UT_NOTIFY(LV_FATAL,"Could not get IMOD_ROOT");

	// std::stringstream s;
	// s<<path<<Configuration::getInstance().getParameter<std::string>("iMod","topologyFilePath");

	// std::stringstream call;
	// call<<"[ A ] = getMeshTopology('"<<s.str()<<"meshReference.matmeshTriangulation');";
	
	// UT_NOTIFY(LV_INFO,call.str());
	// engEvalString(ep, call.str().c_str());

	// //std::cout<<"Length "<<getVector("length")<<std::endl;
	// return getMatrix("A");

	return *netTopology;
}

int InteractionMeshController::getNetTriangleCount(){
	MatrixType m = this->getNetTopology();
	return m.rows();
}

// int InteractionMeshController::getReferenceInteractionMeshID(Pose_t jointAngles){
// 	setMatrix("A",jointAngles,jointAngles.size(),1);
	
// 	std::stringstream call;
// 	call<<"[ ID , topology ] = getReferenceInteractionMeshID(A);";

// 	Timer *t = new Timer();
// 	engEvalString(ep, call.str().c_str());
// 	//netTopology = new MatrixMap(getMatrix("topology"));
// 	UT_NOTIFY(LV_INFO,t->elapsed());

// 	int value = getVector("ID")[0];
// 	return value;
// }

//MatrixMap InteractionMeshController::getNetTopologyOrder(int mesh_ref){
//	std::stringstream s;
//	s<<"[ V ] = getMeshOrder('models/HMM.mat',"<<mesh_ref<<");";
//	engEvalString(ep, s.str().c_str());
//
//	return getMatrix("V");
//}

} /* namespace iMod */
