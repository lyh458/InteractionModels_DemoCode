/*
 * Wii.h
 *
 *  Created on: Jan 27, 2015
 *      Author: ddvogt
 */

#ifndef WII_H_
#define WII_H_

#include "wiiuse.h"                     /* for wiimote_t, classic_ctrl_t, etc */

#include "EventContainer.h"
#include "../utils/Configuration.h"
#include "../utils/Timer.h"

#ifdef WIN32
#include "../utils/notify_windows.h"
#else
#include "../utils/notify.h"
#endif

#define MAX_WIIMOTES 1

namespace iModWii {

/*!
 * A connection class to the Nintendo Wii
 * */
class Wii{

	private:
		wiimote** wiimotes;
		int found, connected;
		EventContainer *events;

	public:
		Wii();
		virtual ~Wii(){};

		void connect();

		char getEvent();

	private:

		void findWii();
		void eventLoop();
		char handleEvent();
		char handle_event(struct wiimote_t* wm);
		void handle_ctrl_status(struct wiimote_t* wm);
		void handle_read(struct wiimote_t* wm, byte* data, unsigned short len);
		void handle_disconnect(wiimote* wm);
		short any_wiimote_connected(wiimote** wm, int wiimotes);
};
}

extern "C"{
#ifndef WIN32
	extern void MC_InitFunc(void) __attribute__ ((constructor));
	extern void MC_TermFunc(void) __attribute__ ((destructor));
    //! Library Constructor
    void MC_InitFunc(void);

    //! Library destructor
    void MC_TermFunc(void);

    //! Connecting to Wiis
    void MC_connect(void);

    //! Handle incoming Wii events
    void MC_getEvent(char& c);
#else
	//! Library Constructor
    __declspec(dllexport) void MC_InitFunc(void);

	//! Library destructor
    __declspec(dllexport) void MC_TermFunc(void);

	//! Connecting to Wiis
    __declspec(dllexport) void MC_connect(void);

	//! Handle incoming Wii events
    __declspec(dllexport) void MC_getEvent(char& c);
#endif
}

#endif /* WII_H_ */
