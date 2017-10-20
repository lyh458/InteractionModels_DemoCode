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

#include "RobotRaconteurServiceIndex_stubskel.h"

#pragma once


namespace RobotRaconteur
{
	class ServiceIndexer : public virtual RobotRaconteurServiceIndex::ServiceIndex
	{
		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:
		
		ServiceIndexer(RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual RR_SHARED_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteurServiceIndex::ServiceInfo> > GetLocalNodeServices();

		virtual RR_SHARED_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteurServiceIndex::NodeInfo> > GetRoutedNodes();

		virtual RR_SHARED_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteurServiceIndex::NodeInfo> > GetDetectedNodes();

		virtual boost::signals2::signal<void ()>& get_LocalNodeServicesChanged();
	private:
		 boost::signals2::signal<void ()> ev;

	};
}