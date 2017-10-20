/*
 * InteractionMeshController.h
 *
 *  Created on: Apr 10, 2013
 *      Author: ddvogt
 */

#ifndef INTERACTIONMESHCONTROLLER_H_
#define INTERACTIONMESHCONTROLLER_H_

#include "MatlabController.h"
#include "BlenderController.h"
#include "InteractionMeshControllerNetwork.h"

namespace iMod {

  /*!
 * A definition of a joint position
 * */
  class Point{
    public:
      double x,y,z;
      Point(double X, double Y, double Z) : x(X), y(Y), z(Z){}
      Point(const Point& cp) : x(cp.x), y(cp.y), z(cp.z){}
      virtual ~Point(){}
  };

  /*!
 * A vector of joint positions defines a posture
 * */
  typedef std::shared_ptr<Point> PointPtr;
  typedef std::vector<PointPtr> JointPositions;

  class InteractionMeshController: public MatlabController {

    public:
      JointPositions positions;
      //int jointsCount;

    public:
      InteractionMeshController(std::string mFilePath);
      virtual ~InteractionMeshController();

      /*!
     * Overrides the initialization function of MatlabController
     * @see MatlabController::initialize()
     */
      void initialize();

      /*!
   * Create a new interaction mesh within Matlab
   * @param X @param Y @param Z these are the XYZ coordinates of vertices
   * @param size The amount of points stores in the mesh
   * @param index The index of the mesh to be created
   * @param triangulate If 1 then a triangulation is calculated,
   * @param jointAngles The joint angles the current user posture
   * @param sizeAngles The lenght of the @param jointAngles vector
   * @return Returns true if the data could be set otherwise false
   * */
      bool createInteractionMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate);

      /*!
   * Updates the reconstruction vector for B
   * @param X @param Y @param Z these are the XYZ coordinates of joint vertices
   * The prediction is made in task/position space
   * The reference mesh, e.g. the one from which the triangulation is calculted has to be in tmp/meshReference.mat
   */
      //virtual bool predict(double* X, double* Y, double* Z, int length, int index);

      /*!
   * Updates the reconstruction vector for B
   * @param jointAngles these are the joint angles
   * @param data these are the XYZ coordinates of joint vertices
   * The prediction is made in joint angle space
   */
      virtual bool predict(iMod::MatrixType data, int lenghtVertices, double* jointAngles, int lengthJointAngles);

      /*!
   * Return the connection matrix after the triangulation
   * The reference mesh is assumed to be in the file path tmp/meshReference.mat
     * Matlab will check for this file
   * */
      MatrixMap getNetTopology();

      /*!
   * Get the amount of lines in the triangulation matrix
   * The reference mesh is assumed to be in the file path tmp/meshReference.mat
   * */
      int getNetTriangleCount();

      /*!
   * Get the order which has been used for triangulation
   * @param mesh_ref The reference mesh from which the topology is used
   */
      //MatrixMap getNetTopologyOrder(int mesh_ref);

      /*!
  * Get the interaction mesh that corresponds to the current user posture
  * @param jointAngles The current user posture in angles
  */
      //virtual int getReferenceInteractionMeshID(Pose_t jointAngles);

    private:

      /*!
  * The interaction mesh toplogy, which will updated everytime @function getReferenceInteractionMeshID is called
  * returned by @function getNetTopology
  */
      MatrixMap* netTopology;


  };

} /* namespace iMod */

extern "C"{
extern void MC_InitFunc(void) __attribute__ ((constructor));
extern void MC_TermFunc(void) __attribute__ ((destructor));

//! Static instance of our Matlab controller
//static iMod::InteractionMeshController* controller;
static iMod::InteractionMeshControllerNetwork* controller;

//! Library Constructor
void MC_InitFunc(void);

//! Destructor of our Matlab controller
void MC_TermFunc(void);

//! Predict the next posture using vertices
//void MC_predict(double *X, double *Y, double *Z, int length, int index);

//! Predict the next posture using joint angles and vertices
void MC_predict(double* X, double* Y, double* Z, int lenghtVertices, double* jointAngles, int lengthJointAngles);

////! Get the dimension of the posture = amount of joints in use
//int MC_getPostureSize();

//! Get single values of a posture
void MC_getPostureByIndex(double& x, double& y, double& z, int index);

//! Set a mesh
void MC_setMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate);

//! Gets a meshs tringle amount
void MC_getMeshTriangleCount(int& count);

//! Get a meshs topology. The required reference mesh is found by a file name : meshReference.mat
void MC_getMeshTopology(int *topo);

//! Get the interaction mesh ID that corresponds to the current user posture
//void MC_getReferenceInteractionMeshID(int &referenceID, double* jointAngles, int size);

//! Get the order used for triangulation
//void MC_getMeshTopologyOrder(int* order, int mesh_ref);
}

#endif /* INTERACTIONMESHCONTROLLER_H_ */
