/*
 * unitTest.cpp
 *
 *  Created on: Feb 10, 2013
 *      Author: ddvogt
 */

//#include "engine.h"

#include <iostream>

//#include "BlenderController.h"
#include "InteractionMeshControllerNetwork.h"
#include "../utils/Configuration.h"
#include "../utils/Timer.h"
#include "../utils/notify.h"

int main(int argc, char *argv[])
{
  //###########
  //Create testing variables
  int runs = 1000;
  int lenghtJointAngles = 0;
  int lenghtVertices = 25;
  double* x = new double[lenghtVertices];
  double* y = new double[lenghtVertices];
  double* z = new double[lenghtVertices];
  double* jointAngles = new double[lenghtJointAngles];

  double LO = -2;
  double HI = 2;
  for(auto i=0;i<lenghtVertices;i++){
    x[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    y[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    z[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
  }

  for(auto i=0;i<lenghtJointAngles;i++){
    jointAngles[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
  }

  //###########
  //Tests using my API
  iMod::Configuration::getInstance();
  iMod::Timer_T<high_resolution_clock> t;

  UT_NOTIFY(LV_DEBUG,"Start measuring prediction/execution time");

  int predictions = runs;
  for(int i=0; i< predictions;i++)
  {
    MC_predict(x,y,z,lenghtVertices,jointAngles,lenghtJointAngles);
    usleep(50000);
  }
  UT_NOTIFY(LV_DEBUG,"The execution took "<<t.seconds()<<"s for "<<predictions<<" predictions.");
  UT_NOTIFY(LV_DEBUG,"FPS: "<<(double)predictions/t.seconds());

  //MC_InitFunc(); //Called automatically when library is loaded

  return 0;
}
