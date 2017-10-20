/**
 * modbus-docu:     http://libmodbus.org/docs/v3.1.2/
 * robotiq-3Finger: http://support.robotiq.com/download/attachments/5963880/3-FINGER-140613.pdf
 * @author: Richard Weinhold <privat@ricwein.com>
 */

#include "GripperController.h"
#include "../../utils/Configuration.h"
#include "../../utils/notify.h"

#include <algorithm>
#include <cerrno>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <modbus/modbus.h>
#include <boost/thread.hpp>

/**
 * constructor
 */
GripperController::GripperController() {}

/**
* Initializes a modbus connection to the gripper
*/
bool GripperController::connect() {
	this->_paused = false;
	this->connection_state = -1;
	this->bus = NULL;
	this->_speed = 0;
	this->_force = 0;

	this->_resetCommand();
	this->speed(iMod::Configuration::getInstance().getParameter<unsigned int>("Gripper", "Speed"));
	this->force(iMod::Configuration::getInstance().getParameter<unsigned int>("Gripper", "Force"));

	if (!this->isConnected()) {
		this->_connectNonBlock(
			iMod::Configuration::getInstance().getParameter<std::string>("Gripper", "IP"),
			iMod::Configuration::getInstance().getParameter<int>("Gripper", "Port")
		);
	}

	return true;
}

void GripperController::_connectNonBlock(std::string ip_address, int port) {
	this->connectThread = new boost::thread(boost::bind(&GripperController::_connectNonBlockThread, this, ip_address, port));
}

bool GripperController::_connectNonBlockThread(std::string ip_address, int port) {
	UT_NOTIFY(LV_INFO, "connect to gripper: " << ip_address << ":" << port);
	this->bus = modbus_new_tcp(ip_address.c_str(), port);

	if (this->bus == NULL) {
		UT_NOTIFY(LV_ERROR, "->unable to allocate libmodbus context");
		return false;
	}

	// connect
	this->connection_state = modbus_connect(this->bus);

	if (this->connection_state < 0) {
		UT_NOTIFY(LV_ERROR, "->connection failed");
		modbus_free(this->bus);
		return false;
	}

	modbus_set_error_recovery(this->bus, MODBUS_ERROR_RECOVERY_LINK);
	return true;
}

/**
 * destructor
 */
GripperController::~GripperController() {
	if (this->isConnected()) {
		try {
			this->connectThread->interrupt();

			modbus_set_error_recovery(this->bus, MODBUS_ERROR_RECOVERY_NONE);
			modbus_close(this->bus);
			modbus_free(this->bus);
		} catch (int exception) {
			UT_NOTIFY(LV_ERROR, "->closing bus failed with status: " << exception);
		}
	}
}

/**
 * returns connection-state as summary-bool
 * @return bool
 */
bool GripperController::isConnected() {
	return (this->bus != NULL && this->connection_state >= 0);
}

/**
 * write single register
 * @param  addr  register-addr
 * @param  value value
 * @return int   returnstate
 */
int GripperController::setValue(int addr, int value) {
	if (this->isConnected()) {
		return modbus_write_register(this->bus, addr, value);
	} else {
		return -1;
	}
}

/**
 * read single register
 * @param  addr register-addr
 * @return      value
 */
uint16_t GripperController::_receiveValue(int addr) {
	static uint16_t dest[1];
	dest[0] = 0;

	if (this->isConnected()) {
		modbus_read_registers(this->bus, addr, 1, dest);
	}

	return dest[0];
}

/**
 * read single register
 * @param  addr register-addr
 * @return      value
 */
uint16_t GripperController::_receiveInputValue(int addr) {
	static uint16_t dest[1];
	dest[0] = 0;

	if (this->isConnected()) {
		modbus_read_input_registers(this->bus, addr, 1, dest);
	}

	return dest[0];
}

/**
 * read single register as unsigned int
 * @param  addr register-addr
 * @return      value
 */
unsigned int GripperController::getValue(int addr) {
	return (unsigned int) this->_receiveValue(addr);
}

/**
 * read single input (info) register as unsigned int
 * @param  addr register-addr
 * @return      value
 */
unsigned int GripperController::getInputValue(int addr) {
	return (unsigned int) this->_receiveInputValue(addr);
}

/**
 * returns if gripper has object detected
 * @param  contactOnly returns object-based interruptions if true, else returns if gripper has stopped motion generally
 * @return             bool
 */
bool GripperController::hasObjectDetected(bool contactOnly) {
	unsigned int current = this->getInputValue(0);
	bool result;

	if (contactOnly) {
		// check for contacts only
		result = false;
		result = result && (((current & 0x01) && !(current & 0x02)) || ((current & 0x02) && !(current & 0x01))); // Finger A
		result = result && (((current & 0x04) && !(current & 0x08)) || ((current & 0x08) && !(current & 0x04))); // Finger B
		result = result && (((current & 0x10) && !(current & 0x20)) || ((current & 0x20) && !(current & 0x10))); // Finger C
		result = result && (((current & 0x40) && !(current & 0x80)) || ((current & 0x80) && !(current & 0x40))); // Scissor
	} else {
		// check for all stops
		result = ((current & 0x01) && (current & 0x02) && (current & 0x04) && (current & 0x08) && (current & 0x10) && (current & 0x20) && (current & 0x40) && (current & 0x80));
	}

	return result;
}

/**
 * check if gripper has finished current action
 * @param  initMode check if initialiazation of gripper has finished if set, else check for all stops
 * @return          bool
 */
bool GripperController::hasFinished(bool initMode) {
	if (initMode) {
		unsigned int current = this->getInputValue(0);
		return ((current & 0x2000) && (current & 0x1000)); // gSTA
	} else {
		return this->hasObjectDetected(false); // object detection
	}
}

/**
 * blocks future actions by sleeping until current action finished
 * @param initMode see: hasFinished() method
 */
void GripperController::wait(bool initMode) {
	while (!this->hasFinished(initMode)) {
		usleep(100000);
	}

	UT_NOTIFY(LV_DEBUG, "->action finished");
}

/**
 * set and get speed for next operation
 * @param  speed of grippe in range from 0 = 22mm/s to 255 = 110mm/s
 * @return       current speed
 */
unsigned int GripperController::speed(int speed) {
	speed = std::max(0x00, (int) speed);
	speed = std::min(0xFF, (int) speed);
	this->_speed = speed;
	return this->_speed;
}

/**
 * set and get force for next operation
 * @param  force of grippe in range from 0 = 15N to 255 = 60N
 * @return       current force
 */
unsigned int GripperController::force(int force) {
	force = std::max(0x00, (int) force);
	force = std::min(0xFF, (int) force);
	this->_force = force;
	return this->_force;
}

/**
 * move gripper to position
 * @param  openTo range from 0 to 255
 * @return         bool
 */
bool GripperController::move(int moveTo) {
	moveTo = std::max(0x00, (int) moveTo);
	moveTo = std::min(0xFF, (int) moveTo);

	this->_resetCommand();
	this->command.rACT = 0x01; // reg 0
	this->command.rMOD = 0x00; // reg 0

	if (this->_paused) {
		this->command.rGTO = 0x00; // reg 0
	} else {
		this->command.rGTO = 0x01; // reg 0
	}

	this->command.rPRA = moveTo; // reg 1
	this->command.rSPA = this->_speed; // reg 2
	this->command.rFRA = this->_force; // reg 2
	this->_build();
	bool result = this->_set(0) && this->_set(1) && this->_set(2);
	// UT_NOTIFY(LV_DEBUG, "->move gripper to: " << moveTo);
	return result;
}

/**
 * open gripper
 * @param  openTo range from 0 to 255
 * @return         bool
 */
bool GripperController::open(int openTo) {
	return this->move(0xFF - openTo);
}

/**
 * close gripper
 * @param  closeTo range from 0 to 255
 * @return         bool
 */
bool GripperController::close(int closeTo) {
	return this->move(closeTo);
}

/**
 * get current gripper position
 * @return unsigned int
 */
unsigned int GripperController::getMove() {
	return (this->getValue(1) & 0x00FF);
}

/**
 * get current gripper open-position
 * @return unsigned int
 */
unsigned int GripperController::getOpen() {
	return (0xFF - this->getMove());
}

/**
 * get current gripper close-position
 * @return unsigned int
 */
unsigned int GripperController::getClose() {
	return this->getMove();
}

/**
 * init gripper
 * @return bool
 */
bool GripperController::init() {
	if (this->setValue(0, 0x0100) >= 0) {
		UT_NOTIFY(LV_DEBUG, "->initialized gripper");
		return true;
	} else {
		return false;
	}
}

/**
 * deinit gripper
 * @return bool
 */
bool GripperController::deinit() {
	return (this->setValue(0, 0x0000) >= 0);
}

bool GripperController::pause() {
	this->_resetCommand();

	if (this->_paused == true) {
		this->command.rGTO = 0x00; // reg 0
		this->_paused = false;
	} else {
		this->command.rGTO = 0x01; // reg 0
		this->_paused = true;
	}

	this->_build();
	return this->_set(0);
}

/**
 * send selected register from this->command to gripper
 * @param  byte number of byte to send
 * @return      bool
 */
bool GripperController::_set(int registerNr) {

	if (registerNr >= 0 && registerNr <= 7) {
		return (this->setValue(registerNr, this->_buildByte16(registerNr)) > 0);
	} else if (registerNr == -1) {

		uint16_t data[8];
		for (int count = 1; count <= 8; ++count) {
			data[count] = this->_buildByte16(count);
		}

		return this->_setReg(0, 8, data);

		// send by calling each register recursive
		// return this->_set(0) && this->_set(1) && this->_set(2) && this->_set(3) && this->_set(4) && this->_set(5) && this->_set(6) && this->_set(7);
	} else {
		UT_NOTIFY(LV_ERROR, "unknown register");
		return false;
	}
}

/**
 * write multiple registers
 * @param  addr first register-addr
 * @param  nb   number of registers
 * @param  data array of values
 * @return int returnstate
 */
int GripperController::_setReg(int addr, int nb, const uint16_t *data) {
	if (this->isConnected()) {
		return modbus_write_registers(this->bus, addr, nb, data);
	} else {
		return -1;
	}
}

/**
 * build 16-bit registers (2bytes) for modbus out of single bytes
 * @param  registerNr number of destination register
 * @return uint16_t   16-bit register
 */
uint16_t GripperController::_buildByte16(unsigned int registerNr) {
	switch (registerNr) {
		case 0: return ((this->byteReg.setA << 8) + this->byteReg.setB);
		case 1: return ((this->byteReg.rRS2 << 8) + this->byteReg.rPRA);
		case 2: return ((this->byteReg.rSPA << 8) + this->byteReg.rFRA);
		case 3: return ((this->byteReg.rPRB << 8) + this->byteReg.rSPB);
		case 4: return ((this->byteReg.rFRB << 8) + this->byteReg.rPRC);
		case 5: return ((this->byteReg.rSPC << 8) + this->byteReg.rFRC);
		case 6: return ((this->byteReg.rPRS << 8) + this->byteReg.rSPS);
		case 7: return ((this->byteReg.rFRS << 8) + this->byteReg.empt);
		default: UT_NOTIFY(LV_ERROR, "unknow byte"); return 0x00;
	}
}

/**
 * set this->command bits to default-values
 * @return this
 */
GripperController &GripperController::_resetCommand() {
	this->command.rACT = 0x01;
	this->command.rMOD = 0x00;
	this->command.rGTO = 0x00;
	this->command.rATR = 0x00;
	this->command.rRS0 = 0x00;
	this->command.rGLV = 0x00;
	this->command.rAAC = 0x00;
	this->command.rICF = 0x00;
	this->command.rICS = 0x00;
	this->command.rRS1 = 0x00;
	this->command.rRS2 = 0x00;
	this->command.rPRA = 0x00;
	this->command.rSPA = this->_speed;
	this->command.rFRA = this->_force;
	this->command.rPRB = 0x00;
	this->command.rSPB = this->_speed;
	this->command.rFRB = this->_force;
	this->command.rPRC = 0x00;
	this->command.rSPC = this->_speed;
	this->command.rFRC = this->_force;
	this->command.rPRS = 0x00;
	this->command.rSPS = this->_speed;
	this->command.rFRS = this->_force;

	this->_verifyCommand();
	return *this;
}

/**
 * ensures single bit-values matching the allowed ranges
 * @return this
 */
GripperController &GripperController::_verifyCommand() {
	this->command.rACT = std::max(0x00, (int) this->command.rACT);
	this->command.rACT = std::min(0x01, (int) this->command.rACT);

	this->command.rMOD = std::max(0x00, (int) this->command.rMOD);
	this->command.rMOD = std::min(0x03, (int) this->command.rMOD);

	this->command.rGTO = std::max(0x00, (int) this->command.rGTO);
	this->command.rGTO = std::min(0x01, (int) this->command.rGTO);

	this->command.rATR = std::max(0x00, (int) this->command.rATR);
	this->command.rATR = std::min(0x01, (int) this->command.rATR);

	this->command.rRS0 = std::max(0x00, (int) this->command.rRS0);
	this->command.rRS0 = std::min(0x07, (int) this->command.rRS0);

	this->command.rGLV = std::max(0x00, (int) this->command.rGLV);
	this->command.rGLV = std::min(0x01, (int) this->command.rGLV);

	this->command.rAAC = std::max(0x00, (int) this->command.rAAC);
	this->command.rAAC = std::min(0x01, (int) this->command.rAAC);

	this->command.rICF = std::max(0x00, (int) this->command.rICF);
	this->command.rICF = std::min(0x01, (int) this->command.rICF);

	this->command.rICS = std::max(0x00, (int) this->command.rICS);
	this->command.rICS = std::min(0x01, (int) this->command.rICS);

	this->command.rRS1 = std::max(0x00, (int) this->command.rRS1);
	this->command.rRS1 = std::min(0x0F, (int) this->command.rRS1);

	this->command.rRS2 = std::max(0x00, (int) this->command.rRS2);
	this->command.rRS2 = std::min(0xFF, (int) this->command.rRS2);

	this->command.rPRA = std::max(0x00, (int) this->command.rPRA);
	this->command.rPRA = std::min(0xFF, (int) this->command.rPRA);

	this->command.rSPA = std::max(0x00, (int) this->command.rSPA);
	this->command.rSPA = std::min(0xFF, (int) this->command.rSPA);

	this->command.rFRA = std::max(0x00, (int) this->command.rFRA);
	this->command.rFRA = std::min(0xFF, (int) this->command.rFRA);

	this->command.rPRB = std::max(0x00, (int) this->command.rPRB);
	this->command.rPRB = std::min(0xFF, (int) this->command.rPRB);

	this->command.rSPB = std::max(0x00, (int) this->command.rSPB);
	this->command.rSPB = std::min(0xFF, (int) this->command.rSPB);

	this->command.rFRB = std::max(0x00, (int) this->command.rFRB);
	this->command.rFRB = std::min(0xFF, (int) this->command.rFRB);

	this->command.rPRC = std::max(0x00, (int) this->command.rPRC);
	this->command.rPRC = std::min(0xFF, (int) this->command.rPRC);

	this->command.rSPC = std::max(0x00, (int) this->command.rSPC);
	this->command.rSPC = std::min(0xFF, (int) this->command.rSPC);

	this->command.rFRC = std::max(0x00, (int) this->command.rFRC);
	this->command.rFRC = std::min(0xFF, (int) this->command.rFRC);

	this->command.rPRS = std::max(0x00, (int) this->command.rPRS);
	this->command.rPRS = std::min(0xFF, (int) this->command.rPRS);

	this->command.rSPS = std::max(0x00, (int) this->command.rSPS);
	this->command.rSPS = std::min(0xFF, (int) this->command.rSPS);

	this->command.rFRS = std::max(0x00, (int) this->command.rFRS);
	this->command.rFRS = std::min(0xFF, (int) this->command.rFRS);

	return *this;
}

/**
 * build bytes out of given bits
 * @return this
 */
GripperController &GripperController::_build() {
	this->_verifyCommand();

	this->byteReg.setA = 0x00;
	this->byteReg.setA |= this->command.rACT;
	this->byteReg.setA |= (this->command.rMOD << 1);
	this->byteReg.setA |= (this->command.rGTO << 3);
	this->byteReg.setA |= (this->command.rATR << 4);
	this->byteReg.setA |= (this->command.rRS0 << 5);

	this->byteReg.setB = 0x00;
	this->byteReg.setB |= this->command.rGLV;
	this->byteReg.setB |= (this->command.rAAC << 1);
	this->byteReg.setB |= (this->command.rICF << 2);
	this->byteReg.setB |= (this->command.rICS << 3);
	this->byteReg.setB |= (this->command.rRS1 << 4);

	this->byteReg.rRS2 = this->command.rRS2;

	this->byteReg.rPRA = this->command.rPRA;
	this->byteReg.rSPA = this->command.rSPA;
	this->byteReg.rFRA = this->command.rFRA;

	this->byteReg.rPRB = this->command.rPRB;
	this->byteReg.rSPB = this->command.rSPB;
	this->byteReg.rFRB = this->command.rFRB;

	this->byteReg.rPRC = this->command.rPRC;
	this->byteReg.rSPC = this->command.rSPC;
	this->byteReg.rFRC = this->command.rFRC;

	this->byteReg.rPRS = this->command.rPRS;
	this->byteReg.rSPS = this->command.rSPS;
	this->byteReg.rFRS = this->command.rFRS;

	this->byteReg.empt = 0x00;

	return *this;
}
