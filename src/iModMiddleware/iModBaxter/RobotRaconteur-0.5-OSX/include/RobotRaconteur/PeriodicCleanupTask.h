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

namespace RobotRaconteur
{
/// <summary>
	/// Interface for objects that can be registered with the node for periodic cleanup
	/// </summary>
	class IPeriodicCleanupTask
	{
		/// <summary>
		/// Called by the node every few seconds for general cleanup
		/// </summary>
	public:
		virtual void PeriodicCleanupTask() = 0;
};
}