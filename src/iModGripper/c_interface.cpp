#include "gripper/GripperController.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern void GC_init(void) __attribute__((constructor));
extern void GC_term(void) __attribute__((destructor));

GripperController *gripper = NULL;

//! Library Constructor
void GC_init() {
	if (gripper == NULL) {
		gripper = new GripperController();
	}
}

//! Destructor
void GC_term(void) {
	delete gripper;
}

/**
 * connect lib to gripper
 * @param bool result optional return value
 */
void GC_connect(bool &result) {
	result = gripper->connect();
}

void GC_isConnected(bool &connected) {
	connected = gripper->isConnected();
}

/**
 * read single register as unsigned int
 * @param  addr register-addr
 * @param  unsigned int result return value
 */
void GC_getValue(int addr, unsigned int &result) {
	result = gripper->getValue(addr);
}

/**
 * read single register as unsigned int
 * @param  addr register-addr
 * @param  unsigned int result return value
 */
void GC_getInputValue(int addr, unsigned int &result) {
	result = gripper->getInputValue(addr);
}

/**
 * write single register
 * @param  addr  register-addr
 * @param  value value
 * @param  int result optional return value
 */
void GC_setValue(int reg_addr, int value, int &result) {
	result = gripper->setValue(reg_addr, value);
}

/**
 * open gripper
 * @param  openTo range from 0 to 255
 * @return         bool
 */
void GC_openGripper(int openTo = 255) {
	gripper->open(openTo);
}

/**
 * close gripper
 * @param  closeTo range from 0 to 255
 * @return         bool
 */
void GC_closeGripper(int closeTo = 255) {
	gripper->close(closeTo);
}

/**
 * get current gripper open-value
 * @param result returns unsigned int
 */
void GC_getOpenGripper(unsigned int &result) {
	result = gripper->getOpen();
}

/**
 * get current gripper close-value
 * @param result returns unsigned int
 */
void GC_getCloseGripper(unsigned int &result) {
	result = gripper->getClose();
}

/**
 * init gripper
 * @return bool
 */
void GC_initGripper() {
	gripper->init();
}

/**
 * set and get speed for next operation
 * @param  speed of grippe in range from 0 = 22mm/s to 255 = 110mm/s
 * @return       current speed
 */
void GC_setSpeed(int speed = -1) {
	gripper->speed(speed);
}

/**
 * set and get force for next operation
 * @param  force of grippe in range from 0 = 15N to 255 = 60N
 * @return       current force
 */
void GC_setForce(int force = -1) {
	gripper->force(force);
}

/**
 * blocks future actions by sleeping until current action finished
 * @param initMode see: hasFinished() method
 * @param seconds  minimum time to wait (1 should be fine)
 */
void GC_wait(bool initMode = false) {
	gripper->wait(initMode);
}

/**
 * check if gripper has finished current action
 * @param  initMode check if initialiazation of gripper has finished if set, else check for all stops
 * @return          bool
 */
void GC_hasFinished(bool &finished, bool initMode = false) {
	finished = gripper->hasFinished(initMode);
}

/**
 * returns if gripper has object detected
 * @param  contactOnly returns object-based interruptions if true, else returns if gripper has stopped motion generally
 * @return             bool
 */
void GC_hasObjectDetected(bool &detected , bool contactOnly = true) {
	detected = gripper->hasObjectDetected(contactOnly);
}

#if defined(__cplusplus)
}
#endif
