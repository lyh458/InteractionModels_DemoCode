#ifndef INTERACTIONMESHCONTROLLERNETWORK_H
#define INTERACTIONMESHCONTROLLERNETWORK_H

#include <memory>
#include <vector>
#include <Eigen/Core>
#include <netinet/in.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace iMod
{
  typedef struct __attribute__ ((aligned(8))) _mlp
  {
      double x,y,z;
      _mlp() : x(0), y(0), z(0) {}
      _mlp(double X, double Y, double Z) : x(X), y(Y), z(Z){}
      _mlp(const _mlp& cp) : x(cp.x), y(cp.y), z(cp.z){}
  } MLPoint ;

  typedef std::shared_ptr<MLPoint> MLPointPtr;
  typedef std::vector<MLPointPtr> MLJointPositions;

  typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> EGMatrixType;
  typedef Eigen::Map<EGMatrixType> EGMatrixMap;

  class InteractionMeshControllerNetwork
  {
    public:
      InteractionMeshControllerNetwork();
      ~InteractionMeshControllerNetwork();

      MLJointPositions positions;

      void initialize();
      bool createInteractionMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate);
      bool predict(double* X, double* Y, double* Z, int lengthVertices);
      MLPoint getPose(int index);

    private:
      template <typename type>
      void swap_endian(type& d)
      {
        char& raw = reinterpret_cast<char&>(d);
        std::reverse(&raw, &raw + sizeof(type));
      }

      void split(const std::string& s, char delim);
      void* receiveloop();
      void parseBinaryData(int amount);

      int s;
      struct sockaddr_in remoteServAddr;
      MLPoint* pose;
      struct sockaddr_in cliAddr;
      int lastframecount;
      char* inbuffer;
      int inbuffersize;
      bool error;
      bool notify;
      boost::thread runthread;
      boost::mutex datamutex;
      double* binarydata;
  };
}

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

//! Predict the next posture using joint angles and vertices
void MC_predict(double* X, double* Y, double* Z, int lenghtVertices, double* jointAngles, int lengthJointAngles);

//! Get single values of a posture
void MC_getPostureByIndex(double& x, double& y, double& z, int index);

//! Set a mesh
void MC_setMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate);
}


#endif // INTERACTIONMESHCONTROLLERNETWORK_H
