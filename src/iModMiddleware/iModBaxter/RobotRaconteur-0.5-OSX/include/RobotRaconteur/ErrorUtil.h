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

#include "RobotRaconteur/Message.h"

namespace RobotRaconteur
{
	class RobotRaconteurExceptionUtil
	{
	public:
		static void ExceptionToMessageEntry(std::exception &exception, RR_SHARED_PTR<MessageEntry> entry);

		static RR_SHARED_PTR<RobotRaconteurException> MessageEntryToException(RR_SHARED_PTR<MessageEntry> entry);

		static void ThrowMessageEntryException(RR_SHARED_PTR<MessageEntry> entry);

		static RR_SHARED_PTR<RobotRaconteurException> DownCastException(RobotRaconteurException& err);

		static void DownCastAndThrowException(RR_SHARED_PTR<RobotRaconteurException> err);
	};

}