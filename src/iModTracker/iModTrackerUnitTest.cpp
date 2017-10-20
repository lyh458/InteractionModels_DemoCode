/*
 * unitTest.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include <memory>

#include "ART.h"
#ifdef WIN32
#include "../utils/notify_windows.h"
#else
#include "../utils/notify.h"
#endif

int main(){

	//Testing C++ API
	// using namespace iModTracker;
	// for(int k;k<3;k++){
	// 	std::shared_ptr<ART> art(new ART); 
	// 	art->registerTargets();
	// 	art->start();

	// 	int i=0;
	// 	while(i<50){
	//         for(int j=0;j<16;j++){
	//     		std::shared_ptr<Target> t = art->getTarget(j);
	//     		if(t)
	//             	std::cout<<*t<<std::endl;
	//         }
	// 		++i;
	// 		usleep(100000);
	// 	}
	// }

	//Testing C API
	NC_InitFunc();
	NC_start();

	UT_NOTIFY(LV_INFO,"Start requesting 18 markers every 0.25 seconds.");

	while(true){
        for(int j=0;j<18;j++){
        	double x=0,y=0,z=0,w=0;
    		NC_getTargetPosition(j,x,y,z);
    		UT_NOTIFY(LV_INFO,"Marker "<<j<<" Pos "<<x<<" "<<y<<" "<<z);

    		double x1=0,y1=0,z1=0,x2=0,y2=0,z2=0,x3=0,y3=0,z3=0;
    		NC_getTargetRotation(j,x1,y1,z1,x2,y2,z2,x3,y3,z3);
    		UT_NOTIFY(LV_INFO,"Marker "<<j<<" Rot:"<< x1<<" "<<y1<<" "<<z1<<";"<<x2<<" "<<y2<<" "<<z2<<";"<<x3<<" "<<y3<<" "<<z3);
        }
#ifdef WIN32
        boost::this_thread::sleep(boost::posix_time::microseconds(250000));
#else
		usleep(250000);
#endif
	}
	return 0;
}
