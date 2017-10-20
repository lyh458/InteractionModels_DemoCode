/*
 * Timer.h
 *
 *  Created on: Feb 22, 2013
 *      Author: ddvogt
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "boost/date_time/posix_time/posix_time.hpp"

namespace iMod {
class Timer {

	boost::posix_time::ptime startTime; ///< Stores the starting time
public:
	/*!
	 * Creates a new timer and automatically starts it
	 * */
	Timer();

	virtual ~Timer();

	/*!
	 * Calculates the time elapsed since the creation of the timer object
	 * @return The time elapsed in milliseconds
	 * */
	long elapsed();

	/*!
	 * Resets the timer
	 * */
	void reset();
};
}  // namespace iMod
#endif /* TIMER_H_ */
