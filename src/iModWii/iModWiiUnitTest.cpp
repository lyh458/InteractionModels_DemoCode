/*
 * unitTest.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include <memory>
#include <iostream>

#include "Wii.h"
#include "../utils/notify.h"

int main(){

	//Testing C API
	MC_InitFunc();
	MC_connect();

	UT_NOTIFY(LV_INFO,"Start polling Wii's");

	while(true){
   		char c;
   		MC_getEvent(c);
   		if (c) {
        	std::cout<<"Got an event: "<<c<<std::endl;    
        }
        usleep(10000);
	}
	return 0;
}
