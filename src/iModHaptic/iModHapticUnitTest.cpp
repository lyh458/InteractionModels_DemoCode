/*
 * unitTest.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include <memory>
#include "../interactionMesh/BlenderController.h"

#include "HapticFeedbackController.h"

int main(){
    
	MC_InitFunc();
	MC_initBlenderController();
	MC_initCollisionResponse();
	MC_initPhysics();


	return 0;
}
