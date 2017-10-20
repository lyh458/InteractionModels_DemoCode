/*
 * EventContainer.cpp
 *
 *  Created on: Jan 27, 2016
 *      Author: ddvogt
 */
#include "EventContainer.h"

namespace iModWii{
	
	EventContainer::EventContainer(){}

	EventContainer::~EventContainer(){}

    void EventContainer::addEvent(char event){
		boost::mutex::scoped_lock(mx);
        
        if (event=='_') {
            return false;
        }
        
		events.push_back(event);
		if (events.size() > 10){
			events.pop_front();
		}
		return true;
	}

	char EventContainer::getLastEvent(){
		boost::mutex::scoped_lock(mx);

		char v = events.back();
        
        //Only remove an item if the container is filled
        if(events.size()>0)
            events.pop_back();
		
        return v;
	}
}
