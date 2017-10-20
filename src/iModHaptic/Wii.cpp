#include "Wii.h"

using namespace iModHaptic;


wiimote** wiimotes = wiiuse_init(4); /*! init wiimotes for connection with Wii-controller*/
float radiusvalue;


void Wii::init(){
	int found = wiiuse_find(wiimotes, 4, 5); /*! search for Wii-controller*/
		int connected = wiiuse_connect(wiimotes, 4); /*! create connection to all found Wii-controller*/
		if (connected){ 
			printf("Connected to %i wiimotes (of %i found).\n", connected, found); 	
		} 
		else { 
			printf("Failed to connect to any wiimote.\n");
			return;
		}	
}	

/*! return radiusvalue if key A or B is pressed*/
	
float Wii::is_pressed(){
radiusvalue=0;
	 
	if(wiiuse_poll(wiimotes,2)){
		
		switch(wiimotes[0]->event){
			
			case WIIUSE_EVENT:
				if(IS_PRESSED(wiimotes[0],WIIMOTE_BUTTON_A)) {
				radiusvalue=0.005;
				
				std::cout<<"A is pressed"<<std::endl;
				return radiusvalue;
				}
				if(IS_PRESSED(wiimotes[0],WIIMOTE_BUTTON_B)) {
				radiusvalue=-0.005;
				
				std::cout<<"B is pressed"<<std::endl;
				return radiusvalue;
				}
				break;
			
			default:
				break;
		}
		

	}
	return radiusvalue;
	
}


void Wii::startFeedbackLeft(){
		
	wiiuse_rumble(wiimotes[0], 1);
	
}
void Wii::startFeedbackRight(){
			
	wiiuse_rumble(wiimotes[1], 1);
	
}
void Wii::stoppFeedbackLeft(){
	wiiuse_rumble(wiimotes[0], 0);
}

void Wii::stoppFeedbackRight(){
	wiiuse_rumble(wiimotes[1], 0);
}

void Wii::startFeedbackLeftThumb(){
	

	
}
void Wii::startFeedbackRightThumb(){

	
	
}
void Wii::stoppFeedbackLeftThumb(){
	
}

void Wii::stoppFeedbackRightThumb(){
	
}

void Wii::startFeedbackLeftIndex(){
			

	
}
void Wii::startFeedbackRightIndex(){
			
	
	
}
void Wii::stoppFeedbackLeftIndex(){
	
}

void Wii::stoppFeedbackRightIndex(){
	
}

void Wii::startFeedbackLeftMiddle(){
			
	
	
}
void Wii::startFeedbackRightMiddle(){
			
	
	
}
void Wii::stoppFeedbackLeftMiddle(){
	
}

void Wii::stoppFeedbackRightMiddle(){
	
}
