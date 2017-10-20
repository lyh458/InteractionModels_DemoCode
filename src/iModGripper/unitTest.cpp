/*
 * unitTest.cpp
 *
 *  @date 16.08.2015
 *  @author: Richard Weinhold
 */

#include <memory>
#include <unistd.h>

#include "c_interface.cpp"

int main() {
	// some variables for testing only
	bool connected;
	bool finished;
	bool detected;
	int result;
	unsigned int value;

	// do all the stuff \o/
	GC_init();
	GC_connect(connected);
	usleep(1000000);

	GC_isConnected(connected);
	if (!connected) {
		return -1;
	}

	GC_initGripper();
	GC_wait(true);

	GC_openGripper();
	// GC_openGripper(100);

	// GC_getOpenGripper(value);

	// GC_closeGripper();
	// GC_closeGripper(100);

	// GC_getCloseGripper(value);

	// GC_setValue(0, 0x00, result);
	// GC_getValue(0, value);

	// GC_getInputValue(0, value);

	// GC_setSpeed(255);
	// GC_setForce(255);

	GC_wait(false);

	GC_hasFinished(finished);
	GC_hasObjectDetected(detected);

	GC_term();

	return 0;
}
