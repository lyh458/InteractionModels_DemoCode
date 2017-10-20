//  Robot Raconteur(R) - A communication library for robotics and automation systems
//  Copyright (C) 2014 John Wason <wason@wasontech.com>
//                     Wason Technology, LLC
//
//  This program is released under the terms of the Robot Raconteur(R)
//  license.  Full text can be found at  http://robotraconteur.com/license2 .
//  Attribute to this library as "Robot Raconteur(R)" in documentation or 
//  packaging.  This software is royalty free for commercial use under
//  the conditions of the license.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

#pragma once

#include "RobotRaconteur/DataTypes.h"

namespace RobotRaconteur
{
	class AutoResetEvent
	{
	public:

		AutoResetEvent();
		virtual ~AutoResetEvent();

		virtual void Set();

		virtual void Reset();

		virtual void WaitOne();

		virtual bool WaitOne(int32_t timeout);
#ifdef BOOST_WINDOWS
	private:
		void* ev;
#else
	private:
		volatile bool m_bSet;
		boost::condition_variable m_setCondition;
		boost::mutex m_mutex;
#endif

	};

	


}