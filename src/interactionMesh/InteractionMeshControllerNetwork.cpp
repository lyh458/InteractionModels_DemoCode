#include "InteractionMeshControllerNetwork.h"

#include "../utils/notify.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

iMod::InteractionMeshControllerNetwork::InteractionMeshControllerNetwork() :
  lastframecount(0),
  pose(NULL),
  inbuffersize(2048),
  error(false),
  notify(true)
{

}

iMod::InteractionMeshControllerNetwork::~InteractionMeshControllerNetwork()
{
  runthread.interrupt();
  runthread.join();
  free(pose);
  free(inbuffer);
}

void iMod::InteractionMeshControllerNetwork::initialize()
{
  int rc;
  struct hostent *h;

  h = gethostbyname ("127.0.0.1");
  if (h == NULL)
  {
    UT_NOTIFY(LV_ERROR, "Unknown host");
    error = true;
    return;
  }

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy ( (char *) &remoteServAddr.sin_addr.s_addr,
           h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(12000);

  s = socket (AF_INET, SOCK_DGRAM, 0);
  if (s < 0)
  {
    UT_NOTIFY(LV_ERROR, "Can't open socket");
    error = true;
    return;
  }

  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  cliAddr.sin_port = htons(12001);
  rc = bind ( s, (struct sockaddr *) &cliAddr,
              sizeof (cliAddr) );
  if (rc < 0)
  {
    UT_NOTIFY(LV_ERROR, "Can't bind port");
    error = true;
    return;
  }

  inbuffer = (char*) malloc(sizeof(char) * inbuffersize);
  if(!inbuffer)
  {
    UT_NOTIFY(LV_ERROR, "Malloc failed for inbuffer");
    error = true;
    return;
  }

  pose = (MLPoint*) malloc(sizeof(MLPoint) * 30); // Not more than 30 joints possible
  if(!pose)
  {
    UT_NOTIFY(LV_ERROR, "Malloc failed for pose");
    error = true;
    return;
  }

  binarydata = (double*) malloc(sizeof(double) * 30 * 3); // Not more than 30 joints possible
  if(!pose)
  {
    UT_NOTIFY(LV_ERROR, "Malloc failed for binarydata");
    error = true;
    return;
  }

  runthread = boost::thread(&iMod::InteractionMeshControllerNetwork::receiveloop, this);
  runthread.detach();
}

bool iMod::InteractionMeshControllerNetwork::createInteractionMesh(double* X, double* Y, double* Z, double* jointAngles, int size, int sizeAngles, int index, int triangulate)
{
  UT_NOTIFY(LV_WARN, "Not implemented");
  return false;
}

bool iMod::InteractionMeshControllerNetwork::predict(double* X, double* Y, double* Z, int lengthVertices)
{
  if(error)
  {
    if(notify)
      UT_NOTIFY(LV_ERROR, "An error occured -> see previous log");
    notify = false;
    return false;
  }

  memset(binarydata, 0, sizeof(double) * 30 * 3);
  int counter = 0;
  for(int i = 0; i < lengthVertices; i++)
  {
    binarydata[i * 3 + 0] = X[i];
    binarydata[i * 3 + 1] = Y[i];
    binarydata[i * 3 + 2] = Z[i];
  }

  int rc = sendto(s, binarydata, lengthVertices * 3 * sizeof(double), 0,(struct sockaddr *) &remoteServAddr,sizeof (remoteServAddr));

  if (rc < 0)
  {
    UT_NOTIFY(LV_ERROR, "Can't send data");
    close(s);
    return false;
  }

  return true;
}

iMod::MLPoint iMod::InteractionMeshControllerNetwork::getPose(int index)
{
  datamutex.lock();
  MLPoint value = pose[index];
  datamutex.unlock();
  return value;
}

void iMod::InteractionMeshControllerNetwork::split(const std::string &s, char delim)
{
  datamutex.lock();
  std::stringstream ss(s);
  std::string item;
  int i = 0;
  double* point_ptr = (double*) pose;
  while (std::getline(ss, item, delim))
  {
    try
    {
      point_ptr[i] = atof(item.c_str());
    }
    catch (std::out_of_range &e)
    {
      UT_NOTIFY(LV_ERROR,e.what());
    }
    i++;
  }
  datamutex.unlock();
}

void *iMod::InteractionMeshControllerNetwork::receiveloop()
{
  try
  {
    int len = sizeof(cliAddr);

    while(true)
    {
      memset(inbuffer, 0, inbuffersize);
      int total = recvfrom(s, inbuffer, inbuffersize, 0, (struct sockaddr *) &cliAddr,(socklen_t*) &len);

      parseBinaryData(total);
    }
  }
  catch (boost::thread_interrupted& interruption)
  {
    UT_NOTIFY(LV_INFO, "Finishing thread");
  }
  catch (std::exception& e)
  {
    UT_NOTIFY(LV_ERROR, "Unexpected error while executing thread");
  }
}

void iMod::InteractionMeshControllerNetwork::parseBinaryData(int amount)
{
  if(amount % 8 != 0)
  {
    UT_NOTIFY(LV_ERROR, "Received data cant be used as doubles");
    return;
  }

  datamutex.lock();
  double* point_ptr = (double*) pose;
  double* data_ptr = (double*) inbuffer;
  for(int i = 0; i < amount/8; i++)
    point_ptr[i] = data_ptr[i];
  datamutex.unlock();
  //std::cout << pose->x << " " << pose->y << " " << pose->z << std::endl;
}
