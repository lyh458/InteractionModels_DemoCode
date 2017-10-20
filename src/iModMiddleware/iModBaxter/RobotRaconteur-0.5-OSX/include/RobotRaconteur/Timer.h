#include <boost/date_time.hpp>
#include "RobotRaconteur/DataTypes.h"
#include <boost/asio/deadline_timer.hpp>

#pragma once

namespace RobotRaconteur
{

struct TimerEvent
{
	bool stopped;
	boost::posix_time::ptime last_expected;
	boost::posix_time::ptime last_real;
	boost::posix_time::ptime current_expected;
	boost::posix_time::ptime current_real;
};

class Timer
{
public:

	virtual void Start()=0;

	virtual void Stop()=0;

	virtual boost::posix_time::time_duration GetPeriod()=0;

	virtual void SetPeriod(const boost::posix_time::time_duration& period)=0;
	
	virtual bool IsRunning()=0;

	virtual void Clear()=0;
	
	virtual ~Timer() {}

};

class RobotRaconteurNode;

class Rate
{
	
public:
			
	virtual void Sleep()=0;

	virtual ~Rate() {};
};

class WallRate : public Rate
{
protected:
	RR_WEAK_PTR<RobotRaconteurNode> node;
	boost::posix_time::time_duration period;
	boost::posix_time::ptime start_time;
	boost::posix_time::ptime last_time;

	boost::asio::deadline_timer timer;

	
public:
	
	WallRate(double frequency, RR_SHARED_PTR<RobotRaconteurNode> node=RR_SHARED_PTR<RobotRaconteurNode>());
		
	virtual void Sleep();

	virtual ~WallRate() {}
};


class WallTimer : public Timer, public RR_ENABLE_SHARED_FROM_THIS<WallTimer>
{
protected:
	boost::posix_time::time_duration period;
	boost::posix_time::ptime start_time;
	boost::posix_time::ptime actual_last_time;
	boost::posix_time::ptime last_time;
	bool oneshot;

	bool running;
	boost::mutex running_lock;

	boost::function<void (const TimerEvent&)> handler;

	RR_SHARED_PTR<boost::asio::deadline_timer> timer;

	RR_WEAK_PTR<RobotRaconteurNode> node;

	void timer_handler(const boost::system::error_code& ec);
	

public:
	
	WallTimer(const boost::posix_time::time_duration& period, boost::function<void (const TimerEvent&)> handler, bool oneshot, RR_SHARED_PTR<RobotRaconteurNode> node=RR_SHARED_PTR<RobotRaconteurNode>());
	

	virtual void Start();

	virtual void Stop();
	
	virtual boost::posix_time::time_duration GetPeriod();

	virtual void SetPeriod(const boost::posix_time::time_duration& period);

	virtual bool IsRunning();

	virtual void Clear();

	

	

	virtual ~WallTimer() {
	
	}

};


}