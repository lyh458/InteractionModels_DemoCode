/*
 * EventContainer.h
 *
 *  Created on: Sep 13, 2012
 *      Author: ddvogt
 */

#ifndef EVENTCONTAINER_H_
#define EVENTCONTAINER_H_

#include <list>
#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/thread.hpp>


namespace iModWii {

typedef std::list<char > Events_t;

class EventContainer {

private:
	/*!
	 * Stores the local mutex to provide threadsafe container access
	 */
	boost::mutex mx;

	// A list of all received events
	Events_t events;

public:
	public:
	EventContainer();
	virtual ~EventContainer();

	/*!
	 * Adds a new event to the container
	 * @param event The event that will be added to the container
	 * @return Returns true if an addition has been made otherwise false
	 */
    void addEvent(char event);

	/*!
	* returns the last received event
	*/
	char getLastEvent();
};
}

#endif /* EVENTCONTAINER_H_ */
