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

#include "RobotRaconteur/NodeID.h"

#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Message.h"
#include <boost/date_time.hpp>

namespace RobotRaconteur
{

	class RobotRaconteurNode;

	class Endpoint : boost::noncopyable
	{
	private:
		boost::mutex m_LocalEndpoint_lock;
		uint32_t m_LocalEndpoint;
	public:
		uint32_t GetLocalEndpoint();
		void SetLocalEndpoint(uint32_t endpoint);
		
	private:
		boost::mutex m_RemoteEndpoint_lock;
		uint32_t m_RemoteEndpoint;
	public:
		uint32_t GetRemoteEndpoint();
		void SetRemoteEndpoint(uint32_t endpoint);
		
	private:
		std::string m_RemoteNodeName;
		boost::mutex m_RemoteNodeName_lock;
	public:
		std::string GetRemoteNodeName();
		void SetRemoteNodeName(std::string name);
		
	private:
		NodeID m_RemoteNodeID;
		boost::mutex m_RemoteNodeID_lock;
	public:
		NodeID GetRemoteNodeID();
		void SetRemoteNodeID(NodeID id);

	private:
		boost::mutex m_transport_lock;
		uint32_t m_transport;
	public:
		uint32_t GetTransport();
		void SetTransport(uint32_t transport);


	private:
	
		boost::posix_time::ptime m_LastMessageReceivedTime;

		boost::mutex m_LastMessageReceivedTime_lock;
	
	public:
		boost::posix_time::ptime GetLastMessageReceivedTime();
		void SetLastMessageReceivedTime(boost::posix_time::ptime time);

	private:
		boost::posix_time::ptime m_LastMessageSentTime;

		boost::mutex m_LastMessageSentTime_lock;
	public:
		boost::posix_time::ptime GetLastMessageSentTime();
		void SetLastMessageSentTime(boost::posix_time::ptime time);

	private:
		
		uint16_t MessageNumber;

		
		boost::mutex MessageNumber_lock;
		
	public:
		virtual void SendMessage(RR_SHARED_PTR<Message> m);

		virtual void AsyncSendMessage(RR_SHARED_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);
				
		virtual void MessageReceived(RR_SHARED_PTR<Message> m) = 0;

		virtual void PeriodicCleanupTask();

	protected:
		virtual void CheckEndpointCapabilityMessage(RR_SHARED_PTR<Message> m);

	public:
		virtual uint32_t EndpointCapability(const std::string &name);

	public:
		void InitializeInstanceFields();

public:
		Endpoint(RR_SHARED_PTR<RobotRaconteurNode> node);
		

		virtual ~Endpoint() {}

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

	protected:
		RR_WEAK_PTR<RobotRaconteurNode> node;

	};


	
	

}