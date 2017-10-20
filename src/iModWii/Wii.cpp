#include "Wii.h"

namespace iModWii{

	Wii::Wii(){
		events = new EventContainer();
	}

	void Wii::connect(){
		boost::thread eventLoop(boost::bind(&iModWii::Wii::eventLoop, this));
	}

	void Wii::findWii(){
		UT_NOTIFY(LV_INFO,"Connecting Wii");
			/*
		 *	Initialize an array of wiimote objects.
		 *
		 *	The parameter is the number of wiimotes I want to create.
		 */
		wiimotes =  wiiuse_init(MAX_WIIMOTES);

		/*
		 *	Find wiimote devices
		 *
		 *	Now we need to find some wiimotes.
		 *	Give the function the wiimote array we created, and tell it there
		 *	are MAX_WIIMOTES wiimotes we are interested in.
		 *
		 *	Set the timeout to be 5 seconds.
		 *
		 *	This will return the number of actual wiimotes that are in discovery mode.
		 */
		found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
		if (!found) {
			printf("No wiimotes found.\n");
			return;
		}

		/*
		 *	Connect to the wiimotes
		 *
		 *	Now that we found some wiimotes, connect to them.
		 *	Give the function the wiimote array and the number
		 *	of wiimote devices we found.
		 *
		 *	This will return the number of established connections to the found wiimotes.
		 */
		connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
		if (connected) {
			printf("Connected to %i wiimotes (of %i found).\n", connected, found);
		} else {
			printf("Failed to connect to any wiimote.\n");
			return;
		}

		/*
		 *	Now set the LEDs and rumble for a second so it's easy
		 *	to tell which wiimotes are connected (just like the wii does).
		 */
		wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
		//wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
		//wiiuse_set_leds(wiimotes[2], WIIMOTE_LED_3);
		//wiiuse_set_leds(wiimotes[3], WIIMOTE_LED_4);
		//wiiuse_rumble(wiimotes[0], 1);
		//wiiuse_rumble(wiimotes[1], 1);	

		#ifndef WIIUSE_WIN32
			usleep(200000);
		#else
			Sleep(200);
		#endif


		wiiuse_rumble(wiimotes[0], 0);
		//wiiuse_rumble(wiimotes[1], 0);
 		
        
	}

	char Wii::getEvent(){
        return events->getLastEvent();
	}


	void Wii::eventLoop(){
        this->findWii();
        while(true)
		{
			char c = this->handleEvent();
			events->addEvent(c);
#ifndef WIIUSE_WIN32
            usleep(100000);
#else
            Sleep(100);
#endif
		}

	}
	char Wii::handleEvent(){
		char value = '_';
		if(any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {
			if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
				/*
				 *	This happens if something happened on any wiimote.
				 *	So go through each one and check if anything happened.
				 */
				int i = 0;
				for (; i < MAX_WIIMOTES; ++i) {
					switch (wiimotes[i]->event) {
						case WIIUSE_EVENT:
							/* a generic event occurred */
							value = this->handle_event(wiimotes[i]);
							//strcpy(c, value);
							break;

						case WIIUSE_STATUS:
							/* a status event occurred */
							this->handle_ctrl_status(wiimotes[i]);
							break;

						case WIIUSE_DISCONNECT:
						case WIIUSE_UNEXPECTED_DISCONNECT:
							/* the wiimote disconnected */
							this->handle_disconnect(wiimotes[i]);
							break;

						case WIIUSE_READ_DATA:
							/*
							 *	Data we requested to read was returned.
							 *	Take a look at wiimotes[i]->read_req
							 *	for the data.
							 */
							break;

						case WIIUSE_NUNCHUK_INSERTED:
							/*
							 *	a nunchuk was inserted
							 *	This is a good place to set any nunchuk specific
							 *	threshold values.  By default they are the same
							 *	as the wiimote.
							 */
							/* wiiuse_set_nunchuk_orient_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 90.0f); */
							/* wiiuse_set_nunchuk_accel_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 100); */
							printf("Nunchuk inserted.\n");
							break;

						case WIIUSE_CLASSIC_CTRL_INSERTED:
							printf("Classic controller inserted.\n");
							break;

						case WIIUSE_WII_BOARD_CTRL_INSERTED:
							printf("Balance board controller inserted.\n");
							break;

						case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
							/* some expansion was inserted */
							this->handle_ctrl_status(wiimotes[i]);
							printf("Guitar Hero 3 controller inserted.\n");
							break;

						case WIIUSE_MOTION_PLUS_ACTIVATED:
							printf("Motion+ was activated\n");
							break;

						case WIIUSE_NUNCHUK_REMOVED:
						case WIIUSE_CLASSIC_CTRL_REMOVED:
						case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
						case WIIUSE_WII_BOARD_CTRL_REMOVED:
						case WIIUSE_MOTION_PLUS_REMOVED:
							/* some expansion was removed */
							this->handle_ctrl_status(wiimotes[i]);
							printf("An expansion was removed.\n");
							break;

						default:
							break;
					}
				}
				
			}
		}
		return value;
	}


	char Wii::handle_event(struct wiimote_t* wm){

		//printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

		/* if a button is pressed, report it */
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)) {
			return 'A';
			printf("A pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) {
			return 'B';
			printf("B pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP)) {
			return 'U';
			printf("UP pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN)){
			return 'D';
			printf("DOWN pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT)){
			return 'L';
			printf("LEFT pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT)){
			return 'R';
			printf("RIGHT pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS)){
			return '-';
			printf("MINUS pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS)){
			return '+';
			printf("PLUS pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE)) {
			return '1';
			printf("ONE pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO)) {
			return '2';
			printf("TWO pressed\n");
		}
		if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME)){
			return 'H';
			printf("HOME pressed\n");
		}

		/*
		 *	Pressing minus will tell the wiimote we are no longer interested in movement.
		 *	This is useful because it saves battery power.
		 */
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) {
			wiiuse_motion_sensing(wm, 0);
		}

		/*
		 *	Pressing plus will tell the wiimote we are interested in movement.
		 */
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) {
			wiiuse_motion_sensing(wm, 1);
		}

		/*
		 *	Pressing B will toggle the rumble
		 *
		 *	if B is pressed but is not held, toggle the rumble
		 */
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B)) {
			wiiuse_toggle_rumble(wm);
		}

		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_UP)) {
			wiiuse_set_ir(wm, 1);
		}
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) {
			wiiuse_set_ir(wm, 0);
		}

		/*
		 * Motion+ support
		 */
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_ONE)) {
			if (WIIUSE_USING_EXP(wm)) {
				wiiuse_set_motion_plus(wm, 2);    // nunchuck pass-through
			} else {
				wiiuse_set_motion_plus(wm, 1);    // standalone
			}
		}

		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_TWO)) {
			wiiuse_set_motion_plus(wm, 0); // off
		}

		/* if the accelerometer is turned on then print angles */
		if (WIIUSE_USING_ACC(wm)) {
			printf("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
			printf("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
			printf("wiimote yaw   = %f\n", wm->orient.yaw);
		}

		/*
		 *	If IR tracking is enabled then print the coordinates
		 *	on the virtual screen that the wiimote is pointing to.
		 *
		 *	Also make sure that we see at least 1 dot.
		 */
		if (WIIUSE_USING_IR(wm)) {
			int i = 0;

			/* go through each of the 4 possible IR sources */
			for (; i < 4; ++i) {
				/* check if the source is visible */
				if (wm->ir.dot[i].visible) {
					printf("IR source %i: (%u, %u)\n", i, wm->ir.dot[i].x, wm->ir.dot[i].y);
				}
			}

			printf("IR cursor: (%u, %u)\n", wm->ir.x, wm->ir.y);
			printf("IR z distance: %f\n", wm->ir.z);
		}

		/* show events specific to supported expansions */
		if (wm->exp.type == EXP_NUNCHUK || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) {
			/* nunchuk */
			struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

			if (IS_PRESSED(nc, NUNCHUK_BUTTON_C)) {
				printf("Nunchuk: C pressed\n");
			}
			if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z)) {
				printf("Nunchuk: Z pressed\n");
			}

			printf("nunchuk roll  = %f\n", nc->orient.roll);
			printf("nunchuk pitch = %f\n", nc->orient.pitch);
			printf("nunchuk yaw   = %f\n", nc->orient.yaw);

			printf("nunchuk joystick angle:     %f\n", nc->js.ang);
			printf("nunchuk joystick magnitude: %f\n", nc->js.mag);

			printf("nunchuk joystick vals:      %f, %f\n", nc->js.x, nc->js.y);
			printf("nunchuk joystick calibration (min, center, max): x: %i, %i, %i  y: %i, %i, %i\n",
			    nc->js.min.x,
			    nc->js.center.x,
			    nc->js.max.x,
			    nc->js.min.y,
			    nc->js.center.y,
			    nc->js.max.y);
		} else if (wm->exp.type == EXP_CLASSIC) {
			/* classic controller */
			struct classic_ctrl_t* cc = (classic_ctrl_t*)&wm->exp.classic;

			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZL)) {
				printf("Classic: ZL pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_B)) {
				printf("Classic: B pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_Y)) {
				printf("Classic: Y pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_A)) {
				printf("Classic: A pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_X)) {
				printf("Classic: X pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZR)) {
				printf("Classic: ZR pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_LEFT)) {
				printf("Classic: LEFT pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_UP)) {
				printf("Classic: UP pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_RIGHT)) {
				printf("Classic: RIGHT pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_DOWN)) {
				printf("Classic: DOWN pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_L)) {
				printf("Classic: FULL L pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_MINUS)) {
				printf("Classic: MINUS pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_HOME)) {
				printf("Classic: HOME pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_PLUS)) {
				printf("Classic: PLUS pressed\n");
			}
			if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_R)) {
				printf("Classic: FULL R pressed\n");
			}

			printf("classic L button pressed:         %f\n", cc->l_shoulder);
			printf("classic R button pressed:         %f\n", cc->r_shoulder);
			printf("classic left joystick angle:      %f\n", cc->ljs.ang);
			printf("classic left joystick magnitude:  %f\n", cc->ljs.mag);
			printf("classic right joystick angle:     %f\n", cc->rjs.ang);
			printf("classic right joystick magnitude: %f\n", cc->rjs.mag);
		} else if (wm->exp.type == EXP_GUITAR_HERO_3) {
			/* guitar hero 3 guitar */
			struct guitar_hero_3_t* gh3 = (guitar_hero_3_t*)&wm->exp.gh3;

			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_UP)) {
				printf("Guitar: Strum Up pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_DOWN))	{
				printf("Guitar: Strum Down pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_YELLOW)) {
				printf("Guitar: Yellow pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_GREEN)) {
				printf("Guitar: Green pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_BLUE)) {
				printf("Guitar: Blue pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_RED)) {
				printf("Guitar: Red pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_ORANGE)) {
				printf("Guitar: Orange pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_PLUS)) {
				printf("Guitar: Plus pressed\n");
			}
			if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_MINUS)) {
				printf("Guitar: Minus pressed\n");
			}

			printf("Guitar whammy bar:          %f\n", gh3->whammy_bar);
			printf("Guitar joystick angle:      %f\n", gh3->js.ang);
			printf("Guitar joystick magnitude:  %f\n", gh3->js.mag);
		} else if (wm->exp.type == EXP_WII_BOARD) {
			/* wii balance board */
			struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
			float total = wb->tl + wb->tr + wb->bl + wb->br;
			float x = ((wb->tr + wb->br) / total) * 2 - 1;
			float y = ((wb->tl + wb->tr) / total) * 2 - 1;
			printf("Weight: %f kg @ (%f, %f)\n", total, x, y);
			/* printf("Interpolated weight: TL:%f  TR:%f  BL:%f  BR:%f\n", wb->tl, wb->tr, wb->bl, wb->br); */
			/* printf("Raw: TL:%d  TR:%d  BL:%d  BR:%d\n", wb->rtl, wb->rtr, wb->rbl, wb->rbr); */
		}

		if (wm->exp.type == EXP_MOTION_PLUS ||
		        wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) {
			printf("Motion+ angular rates (deg/sec): pitch:%03.2f roll:%03.2f yaw:%03.2f\n",
			       wm->exp.mp.angle_rate_gyro.pitch,
			       wm->exp.mp.angle_rate_gyro.roll,
			       wm->exp.mp.angle_rate_gyro.yaw);
		}
		return '_';
	}
	/**
	 *	@brief Callback that handles a read event.
	 *
	 *	@param wm		Pointer to a wiimote_t structure.
	 *	@param data		Pointer to the filled data block.
	 *	@param len		Length in bytes of the data block.
	 *
	 *	This function is called automatically by the wiiuse library when
	 *	the wiimote has returned the full data requested by a previous
	 *	call to wiiuse_read_data().
	 *
	 *	You can read data on the wiimote, such as Mii data, if
	 *	you know the offset address and the length.
	 *
	 *	The \a data pointer was specified on the call to wiiuse_read_data().
	 *	At the time of this function being called, it is not safe to deallocate
	 *	this buffer.
	 */
	void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {
		int i = 0;

		printf("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
		printf("finished read of size %i\n", len);
		for (; i < len; ++i) {
			if (!(i % 16)) {
				printf("\n");
			}
			printf("%x ", data[i]);
		}
		printf("\n\n");
	}


	/**
	 *	@brief Callback that handles a controller status event.
	 *
	 *	@param wm				Pointer to a wiimote_t structure.
	 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
	 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
	 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
	 *	@param led				What LEDs are lit.
	 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
	 *
	 *	This occurs when either the controller status changed
	 *	or the controller status was requested explicitly by
	 *	wiiuse_status().
	 *
	 *	One reason the status can change is if the nunchuk was
	 *	inserted or removed from the expansion port.
	 */
	void Wii::handle_ctrl_status(struct wiimote_t* wm) {
		printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

		printf("attachment:      %i\n", wm->exp.type);
		printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
		printf("ir:              %i\n", WIIUSE_USING_IR(wm));
		printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
		printf("battery:         %f %%\n", wm->battery_level);
	}


	/**
	 *	@brief Callback that handles a disconnection event.
	 *
	 *	@param wm				Pointer to a wiimote_t structure.
	 *
	 *	This can happen if the POWER button is pressed, or
	 *	if the connection is interrupted.
	 */
	void Wii::handle_disconnect(wiimote* wm) {
		printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
	}


	short Wii::any_wiimote_connected(wiimote** wm, int wiimotes) {
		int i;
		if (!wm) {
			return 0;
		}

		for (i = 0; i < wiimotes; i++) {
			if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
				return 1;
			}
		}

		return 0;
	}
}