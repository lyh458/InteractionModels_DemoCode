/*
 * CMD_Interface.cpp
 * A simple command line interface to open or close the Robotique Gripper
 *
 *  @date 10.09.2015
 *  @author: David Vogt
 */

#include <iostream>
#include <boost/program_options.hpp>
#include <iterator>

#include "c_interface.cpp"
#include "../utils/notify.h"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char* argv[]) {
	// some variables for testing only
	bool connected;
	bool finished;
	unsigned int value = 255;
	unsigned int force = 1;
	unsigned int speed = 1;


	try{
		// Declare the supported options.
		po::options_description desc("Allowed options");
		desc.add_options()
		    ("help", "produce help message")
		    ("open", po::value<int>(), "set the grippers open state [0...255]")
		    ("force", po::value<int>(), "set the grippers force state [0...255]")
		    ("speed", po::value<int>(), "set the grippers speed state [0...255]");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
		    cout << desc << "\n";
		    return 1;
		}

		if (vm.count("open")) {
			value = vm["open"].as<int>();
			UT_NOTIFY(LV_INFO, "Openness of the gripper set to " << value);
		} else {
			UT_NOTIFY(LV_INFO, "Openness of the gripper set to " << value);
		}

		if (vm.count("force")) {
			force = vm["force"].as<int>();
			UT_NOTIFY(LV_INFO, "Force of the gripper set to " << force);
		} else {
			UT_NOTIFY(LV_INFO, "Force of the gripper set to " << force);
		}

		if (vm.count("speed")) {
			speed = vm["speed"].as<int>();
			UT_NOTIFY(LV_INFO, "Speed of the gripper set to " << speed);
		} else {
			UT_NOTIFY(LV_INFO, "Speed of the gripper set to " << speed);
		}
    }
    catch(exception& e) {
    	UT_NOTIFY(LV_ERROR, e.what());
        return 1;
    }
    catch(...) {
        UT_NOTIFY(LV_ERROR, "Exception of unknown type!");
    }

	GC_init();
	GC_connect(connected);

	GC_isConnected(connected);
	if (!connected) {
		UT_NOTIFY(LV_ERROR, "Could not connect to gripper");
		return -1;
	}

	GC_initGripper();
	//sleep(5);
	GC_wait(true);

	GC_setSpeed(speed);
	GC_setForce(force);
	GC_openGripper(value);

	GC_wait(false);
	GC_hasFinished(finished);
	GC_term();

	return 0;
}
