/*
 * unitTest.cpp
 *
 *  Created on: Feb 10, 2013
 *      Author: ddvogt
 */


#include <iostream>
#include <stdio.h>
#include <dlfcn.h>


int main()
{    
    //###########
    //Create testing variables
    int runs = 100;
    int lenghtJointAngles = 103;
    int lenghtVertices = 16;
    double* x = new double[lenghtVertices];
    double* y = new double[lenghtVertices];
    double* z = new double[lenghtVertices];
    double* jointAngles = new double[lenghtJointAngles];
    
    double LO = -2;
    double HI = 2;
    for(auto i=0;i<lenghtVertices;i++)
    {
        x[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
        y[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
        z[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    }
    
    for(auto i=0;i<lenghtJointAngles;i++)
    {
        jointAngles[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    }

    //###########
    //Tests using my API

    void* dll = dlopen("/home/hunabku/Programs/InteractionModels/CPP/lib/libiModMesh.so", RTLD_NOW);
    printf("%X \n", dll);

    //iMod::Configuration::getInstance();
    //MC_predict(x,y,z,lenghtVertices,jointAngles,lenghtJointAngles);


    
    return 0;
}
