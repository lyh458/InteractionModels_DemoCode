#include "InteractionMeshControllerNetwork.h"
#include "../utils/notify.h"

extern "C"
{
extern void MC_InitFunc(void) __attribute__ ((constructor));
extern void MC_TermFunc(void) __attribute__ ((destructor));



//! Library Constructor
void MC_InitFunc(void){
  controller = new iMod::InteractionMeshControllerNetwork();
  controller->initialize();
}

//! Destructor of our Matlab controller
void MC_TermFunc(void)
{
}

//! Predict the next posture
void MC_predict(double* X, double* Y, double* Z, int lenghtVertices, double* jointAngles, int lengthJointAngles)
{
  controller->predict(X, Y, Z, lenghtVertices);
}

//! Get the last predicted posture in world coordinates, index identifies the ID of the vertex
void MC_getPostureByIndex(double& x, double& y, double& z, int index){
  x = controller->getPose(index).x;
  y = controller->getPose(index).y;
  z = controller->getPose(index).z;
}

//! Set a mesh
void MC_setMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate){
  controller->createInteractionMesh(X,Y,Z,jointAngles, size, sizeAngles, index, triangulate);
}

}
