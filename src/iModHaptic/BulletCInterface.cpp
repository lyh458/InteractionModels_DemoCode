#include "HapticFeedbackController.h"
extern "C"{
	
	extern void MC_InitFunc(void) __attribute__ ((constructor));
	extern void MC_TermFunc(void) __attribute__ ((destructor));

	//! Library Constructor
	void MC_InitFunc(void){
		bullet = new iModHaptic::HapticFeedbackController;
		std::cout<<"iModHaptic::MC_InitFunc"<<std::endl;
	}

	//! Destructor
	void MC_TermFunc(void){
	}
	

   

	
	void MC_initPhysics(){
		bullet->initPhysics();
		std::cout<<"iModHaptic::MC_initPhysics done!"<<std::endl;
	}
	void MC_initBlenderController(){
		bullet->initBlenderController();
		std::cout<<"iModHaptic::MC_initBlenderController done!"<<std::endl;
	}
	void MC_initVisual(){

		bullet->initVisual();
		std::cout<<"iModHaptic::MC_Visual done!"<<std::endl;
	}
	void MC_initCollisionResponse(){
		bullet->initCollisionResponse();
		std::cout<<"iModHaptic::MC_initCollisionResponse done!"<<std::endl;
	}

}
