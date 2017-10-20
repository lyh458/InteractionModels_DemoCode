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

#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/array.hpp>
#include <boost/uuid/uuid.hpp>

namespace RobotRaconteur
{
	
	class NodeID : virtual public RRObject
	{
	private:
		boost::uuids::uuid id;

	public:

		NodeID();

		NodeID(boost::array<uint8_t,16> id);

		NodeID(boost::uuids::uuid id);

		NodeID(const NodeID& id);

		NodeID(const std::string &id);

		const boost::array<uint8_t,16> ToByteArray() const;

		const boost::uuids::uuid ToUuid() const;

		virtual std::string ToString() const;

		
		static NodeID NewUniqueID();

		bool operator == (const NodeID &id2) const;

		bool operator != (const NodeID &id2) const;

		bool operator <(const NodeID& id2) const;

		NodeID& operator =(const NodeID& id);

		bool IsAnyNode();
		
		static NodeID GetAny(); 

		virtual int32_t GetHashCode() const;

		virtual std::string RRType() { return "RobotRaconteur::NodeID";}

	
	};



}