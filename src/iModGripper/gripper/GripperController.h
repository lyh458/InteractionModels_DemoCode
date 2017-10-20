#include <iostream>
#include <modbus/modbus.h>
#include <boost/thread.hpp>

class GripperController {
  protected:
	modbus_t *bus;
	int connection_state;
	boost::thread *connectThread;

	bool _paused;
	unsigned int _speed : 8;
	unsigned int _force : 8;

	struct byteRegStruct {
		unsigned int setA : 8; // 0
		unsigned int setB : 8; // 1

		unsigned int rRS2 : 8; // 2
		unsigned int rPRA : 8; // 3

		unsigned int rSPA : 8; // 4
		unsigned int rFRA : 8; // 5

		unsigned int rPRB : 8; // 6
		unsigned int rSPB : 8; // 7

		unsigned int rFRB : 8; // 8
		unsigned int rPRC : 8; // 9

		unsigned int rSPC : 8; // 10
		unsigned int rFRC : 8; // 11

		unsigned int rPRS : 8; // 12
		unsigned int rSPS : 8; // 13

		unsigned int rFRS : 8; // 14
		unsigned int empt : 8; // 15 reserved
	} byteReg;


	struct commandStruct {
		// Byte 0
		unsigned int rACT : 1; // action
		unsigned int rMOD : 2; // grasping mode
		/* 0x0 Basic
		 * 0x1 Pinch
		 * 0x2 Wide
		 * 0x3 Scissor
		 */
		unsigned int rGTO : 1; // Go-To mode | 0x0 - Stop | 0x1 - go to position
		unsigned int rATR : 1; // automatic release | 0x0 - normal | 0x0 - emergency auto-release
		unsigned int rRS0 : 3; // reserved

		//Byte 1
		unsigned int rGLV : 1; // reserved
		unsigned int rAAC : 1; // reserved
		unsigned int rICF : 1; // individual finger control mode | 0x0 - normal | 0x1 - enabled individual control
		unsigned int rICS : 1; // control of scissor
		unsigned int rRS1 : 4; // reserved

		unsigned int rRS2 : 8; // reserved

		unsigned int rPRA : 8; // position request | 0x00 (0) - open | 0xFF (255) - close
		unsigned int rSPA : 8; // speed | 0x00 (0) - minimum 22mm/s | 0xFF (255) - maximum 110mm/s
		unsigned int rFRA : 8; // force | 0x00 (0) - minimum 15N | 0xFF (255) - maximum 60N

		// individual fingers:
		unsigned int rPRB : 8; // position request | 0x00 (0) - open | 0xFF (255) - close
		unsigned int rSPB : 8; // speed | 0x00 (0) - minimum 22mm/s | 0xFF (255) - maximum 110mm/s
		unsigned int rFRB : 8; // force | 0x00 (0) - minimum 15N | 0xFF (255) - maximum 60N

		unsigned int rPRC : 8; // position request | 0x00 (0) - open | 0xFF (255) - close
		unsigned int rSPC : 8; // speed | 0x00 (0) - minimum 22mm/s | 0xFF (255) - maximum 110mm/s
		unsigned int rFRC : 8; // force | 0x00 (0) - minimum 15N | 0xFF (255) - maximum 60N

		// scissor:
		unsigned int rPRS : 8; // position request | 0x00 (0) - open | 0xFF (255) - close
		unsigned int rSPS : 8; // speed | 0x00 (0) - minimum 22mm/s | 0xFF (255) - maximum 110mm/s
		unsigned int rFRS : 8; // force | 0x00 (0) - minimum 15N | 0xFF (255) - maximum 60N
	} command, local;

	uint16_t _receiveValue(int addr);
	uint16_t _receiveInputValue(int addr);

	GripperController &_verifyCommand();
	GripperController &_resetCommand();
	GripperController &_build();

	uint16_t _buildByte16(unsigned int registerNr);

	int _setReg(int addr, int nb, const uint16_t *data);
	bool _set(int byte = -1);

	void _connectNonBlock(std::string ip_address, int port = 502);
	bool _connectNonBlockThread(std::string ip_address, int port = 502);

  public:
	GripperController();
	~GripperController();

	bool connect();

	bool isConnected();

	unsigned int getValue(int addr);
	unsigned int getInputValue(int addr);
	int setValue(int reg_addr, int value);

	unsigned int speed(int speed = -1);
	unsigned int force(int force = -1);

	bool move(int moveTo);
	bool open(int openTo = 255);
	bool close(int closeTo = 255);


	unsigned int getMove();
	unsigned int getOpen();
	unsigned int getClose();

	bool init();
	bool deinit();
	bool pause();

	bool hasFinished(bool initMode = false);
	bool hasObjectDetected(bool contactOnly = true);

	void wait(bool initMode = false);

};
