/*
 * Timer.cpp
 *
 *  Created on: Feb 22, 2013
 *      Author: ddvogt
 */

#include "Timer.h"
using namespace iMod;

Timer::Timer() {
	// TODO Auto-generated constructor stub
	startTime = boost::posix_time::microsec_clock::local_time();
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

long Timer::elapsed(){
	boost::posix_time::ptime timerEnd = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration TimeDuration;
	TimeDuration = timerEnd - startTime;

	return TimeDuration.total_milliseconds();
}

void Timer::reset(){
	startTime = boost::posix_time::microsec_clock::local_time();
}
