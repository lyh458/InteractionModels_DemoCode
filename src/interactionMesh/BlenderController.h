/*
 * BlenderController.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ddvogt
 */

#ifndef BLENDERCONTROLLER_H_
#define BLENDERCONTROLLER_H_

#include <iostream>
#include <string>
#include <boost/timer.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "PoseContainer.h"
#include "MessageConverter.h"

using boost::asio::ip::tcp;

//typedef std::shared_ptr<tcp::socket> SocketPtr;

namespace iMod{
class BlenderController {

	int _port;
	std::string _ip;

	tcp::socket* socket; //!< Stores the active TCP connection to blender
	MessageConverter* messageConverter; //!< Converter for translating each TCP packet to a readable string or float vector

public:
	BlenderController();
	virtual ~BlenderController();

	/*!
	 * Initialize the connection to the server
	 * @param port the TCP port in use
	 * @param ip the IP where Blender is running
	 * @param trial How often should the client try to establish a connection
	 */
	void initialize(const int port, const std::string ip = "127.0.0.1", const int trials = 30);

	/*!
	 * Disconnect the BlenderController
	 * This function closes the TCP/IP connection
	 */
	void disconnect();

	/*!
	 * Reads constantly postures from blender
	 * This is a blocking call.
	 * @param sleep sleep duration in microseconds after a single posture acquisition
	 * @param poseLength Amount of joints for each posture.
	 */
    void readBlenderPoses(int poseLength = 57);

	/*!
	 * Sends constantly postures from the pose container to blender.
	 * This is a blocking call.
	 * @param sleep sleep duration in microseconds after a single posture acquisition
	 */
	void writeBlenderPoses(int sleep = 33);

public:

	PoseContainer* poseContainer; //!< Public access to the container which stores all read/sent postures

	long timeElapsed; //!< Time it takes for the readBlenderPoses function to execute
};
}

#endif /* BLENDERCONTROLLER_H_ */
