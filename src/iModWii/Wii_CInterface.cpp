#include "Wii.h"

extern "C"{
#ifndef WIN32
	extern void MC_InitFunc(void) __attribute__ ((constructor));
	extern void MC_TermFunc(void) __attribute__ ((destructor));
#endif

	//! Static instance of our Matlab controller
	static iModWii::Wii *wii;

	//! Library Constructor
	void MC_InitFunc(void){

		//Get IMOD_ROOT path
		char* pPath;
		pPath = getenv ("IMOD_ROOT");
		if (pPath == NULL)
			UT_NOTIFY(LV_FATAL,"Could not get IMOD_ROOT");

		wii = new iModWii::Wii();
	     
	}

	//! Destructor of our Matlab controller
	void MC_TermFunc(void){
		delete wii;
	}

	void MC_connect(void){
        if(!wii)
            MC_InitFunc();
		wii->connect();
	}

	void MC_getEvent(char& c){
        if(!wii)
            MC_InitFunc();
		char value=wii->getEvent();
		c = value;
	}
}
