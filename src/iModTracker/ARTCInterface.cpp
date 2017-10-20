#include "ART.h"

extern "C"{

static iModTracker::ART* art;
#ifndef WIN32
extern void MC_InitFunc(void) __attribute__ ((constructor));
extern void MC_TermFunc(void) __attribute__ ((destructor));
#endif
//! Library Constructor
void NC_InitFunc(void){
    art = new iModTracker::ART;
    art->registerTargets();
}

//! Destructor
void NC_TermFunc(void){
}

void NC_getTargetPosition(int id, double& x, double& y, double& z){
    auto t = art->getTarget(id);
    if(!t){
        x = 0;//p[0];
        y = 0;//p[1];
        z = 0;//p[2];
    }
    else{
        x = t->getPosition()[0];
        y = t->getPosition()[1];
        z = t->getPosition()[2];
        //printf("ID: %i %f %f %f \n",id,x,y,z);
    }
}

void NC_getTargetRotation(int id, double& x, double& y, double& z, double& x1, double& y1, double& z1, double& x2, double& y2, double& z2){
    auto t = art->getTarget(id);
    if(!t){
        x=y=z=x1=y1=z1=x2=y2=z2=0;
        std::cout<<"The requested target with ID="<<id<<" does not exsit."<<std::endl;
    }
    else{
        x = t->getRotation()[0];
        y = t->getRotation()[1];
        z = t->getRotation()[2];

        x1 = t->getRotation()[3];
        y1 = t->getRotation()[4];
        z1 = t->getRotation()[5];

        x2 = t->getRotation()[6];
        y2 = t->getRotation()[7];
        z2 = t->getRotation()[8];
    }
}

void NC_start()
{
    if(!art)
        NC_InitFunc();
    art->start();
}

void NC_stop()
{
    if(art)
        art->stop();
}
}


