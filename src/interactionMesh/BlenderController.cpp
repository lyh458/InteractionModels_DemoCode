/*
 * BlenderController.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ddvogt
 */

#include "BlenderController.h"
#include "Timer.h"
#include "../utils/notify.h"
#include "../utils/Configuration.h"

#include <boost/cerrno.hpp>

using namespace iMod;

BlenderController::BlenderController() {
	// TODO Auto-generated constructor stub
	poseContainer = new PoseContainer();
	messageConverter = new MessageConverter();
}

BlenderController::~BlenderController() {
	// TODO Auto-generated destructor stub
	delete(messageConverter);
	delete(poseContainer);
}

void BlenderController::readBlenderPoses(int poseLength) {
		std::vector<double> data;
		data.resize(poseLength,0);
		boost::asio::streambuf buf; // stores the read stream

		for (;;) {
			boost::system::error_code error; // connection error handling
			Timer t; // A timer to measure how long the message sending takes

			//Read until a terminal symbol has been found --> the terminal symbol is the trailing symbol in the stream buffer
			try{
				size_t len = boost::asio::read_until(*socket, buf, "\n", error);
					//Convert the filled buffer to a char pointer
				std::istream str(&buf);
				std::string s; 
				std::getline(str, s);
				
				//Convert the read string to an array of doubles
				messageConverter->convertToDouble(s.c_str(),data);
				poseContainer->addPose(data,0);

				timeElapsed = t.elapsed();

			} catch (std::exception& e) {
				UT_NOTIFY(LV_INFO,"Can't read from network. Reconnecting..."<<e.what());
				this->disconnect();
				this->initialize(this->_port, this->_ip, 1);
				continue;
			}

			if (error){
				UT_NOTIFY(LV_INFO, error.message());
				this->disconnect();
				this->initialize(this->_port, this->_ip, 1);

				//throw boost::system::system_error(error); // Some other error.
			}		
		}
	return;
}

void BlenderController::writeBlenderPoses(int sleep) {
	try {
		for (;;) {
			if (!poseContainer->isContainerFilled()) {
				boost::this_thread::sleep(boost::posix_time::milliseconds(sleep));
				continue;
			}

			Timer t; //! Create a new timer to measure how long the message sending takes

			//Get the last pose as a string that will be send
			std::string ss;
			*poseContainer >> ss;

			//Send the message and do some basic error checking
			boost::system::error_code error;
			socket->write_some(boost::asio::buffer(ss), error);
			if (error == boost::asio::error::fault){
				UT_NOTIFY(LV_ERROR,"Write poses from Blender error. Aborting write loop ");
				break;
				//this->disconnect();
				//this->initialize(this->_port, this->_ip, iMod::Configuration::getInstance().getParameter<int>("iModOSG","reconnects"));
			}
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			//Calculate how long the we need to wait in order to reach approximately 30fps
			boost::this_thread::sleep(boost::posix_time::milliseconds(sleep - t.elapsed()));
		}
	} catch (std::exception& e) {
		UT_NOTIFY(LV_ERROR,"Write poses from Blender error: "<<e.what());
	}
	return;
}

void BlenderController::initialize(int port, std::string ip, int trails) {
	try {
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);

		this->_port = port;
		this->_ip = ip;

		tcp::resolver::query query(ip, boost::lexical_cast<std::string>(port));
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::endpoint endpoint;

		socket = new tcp::socket(io_service);

		boost::system::error_code error = boost::asio::error::host_not_found;
		socket->close(error);
		//socket->non_blocking(true,error);

		socket->open(tcp::v4());
		while (endpoint_iterator != tcp::resolver::iterator()){

			boost::asio::ip::tcp::endpoint endpoint;
			endpoint = *endpoint_iterator++;
			UT_NOTIFY(LV_INFO,"Blender Controller: Endpoint "<<endpoint);
			socket->connect(endpoint, error);
		}

		//Do some basic connection error handling
		if (error != 0){
			UT_NOTIFY(LV_INFO,"Blender Controller connection error. Retrying. ");//If the connection has been refused then wait a bit and try again
			if(trails > 0){
				boost::this_thread::sleep(boost::posix_time::seconds(1));
				initialize(port,ip,--trails);
			}
		}
		else
			UT_NOTIFY(LV_INFO,"Connection to "<<ip<<" created successfully.");

	} 	
	catch (boost::system::system_error& e) {
		UT_NOTIFY(LV_ERROR,"Initialize Blender error: "<<e.what());
	}catch (std::exception& e) {
		UT_NOTIFY(LV_ERROR,"Initialize Blender error: "<<e.what());
	}
	return;
}

void BlenderController::disconnect(){
	if(socket){
		//socket->close();
		//socket.reset();
	}
}

