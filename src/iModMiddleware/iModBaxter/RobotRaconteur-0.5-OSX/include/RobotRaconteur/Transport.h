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
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/PeriodicCleanupTask.h"
#include <boost/date_time.hpp>
#include <boost/signals2.hpp>
#include "RobotRaconteur/Timer.h"

#ifdef BOOST_WINDOWS
#undef SendMessage
#endif

namespace RobotRaconteur
{

	enum TransportListenerEventType
	{
		TransportListenerEventType_TransportClosed = 1,
		TransportListenerEventType_TransportConnectionClosed
	};

	class RobotRaconteurNode;
	class Timer;
	struct TimerEvent;
	class AutoResetEvent;

	class ITransportTimeProvider
	{
	public:
		virtual boost::posix_time::ptime NowUTC()=0;

		virtual RR_SHARED_PTR<Timer> CreateTimer(const boost::posix_time::time_duration& duration, boost::function<void(const TimerEvent&)> handler, bool oneshot=false)=0;

		virtual RR_SHARED_PTR<Rate> CreateRate(double frequency)=0;

		virtual void Sleep(const boost::posix_time::time_duration& duration)=0;

		virtual RR_SHARED_PTR<AutoResetEvent> CreateAutoResetEvent()=0;

		virtual ~ITransportTimeProvider() {}
	};


	class ITransportConnection : boost::noncopyable
	{
	 public:

		virtual ~ITransportConnection() {}

		 virtual void SendMessage(RR_SHARED_PTR<Message> m) = 0;

		 virtual void AsyncSendMessage(RR_SHARED_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> handler) = 0;

		 virtual void Close() = 0;

		 virtual void CheckConnection(uint32_t endpoint) = 0;

		  virtual  uint32_t GetLocalEndpoint() const = 0;

		  virtual  uint32_t GetRemoteEndpoint() const = 0;

		  virtual  NodeID GetRemoteNodeID() const = 0;

		  virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode() =0;

		  
	};


	class NodeDiscoveryInfo;

	class Transport : public IPeriodicCleanupTask, boost::noncopyable
	{
	public:
		friend class ITransport;
		virtual ~Transport() {}
	public:
		static boost::thread_specific_ptr<std::string> m_CurrentThreadTransportConnectionURL;
	public:
		static std::string GetCurrentThreadTransportConnectionURL();

	public:

		static boost::thread_specific_ptr<RR_SHARED_PTR<ITransportConnection> > m_CurrentThreadTransport;

	public:
	   static RR_SHARED_PTR<ITransportConnection> GetCurrentThreadTransport();

//		public event MessageHandler MessageReceivedEvent;

	public:
		uint32_t TransportID;

	public:
		virtual void CheckConnection(uint32_t endpoint)=0;

		virtual bool IsClient() const = 0;

		virtual bool IsServer() const = 0;

		virtual std::string GetUrlSchemeString() const = 0;

		//public abstract string Scheme { get; }

		virtual bool CanConnectService(std::string url) = 0;

		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(std::string url, RR_SHARED_PTR<Endpoint> e) = 0;

		virtual void AsyncCreateTransportConnection(std::string url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) > handler)=0;

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e) = 0;

	
		virtual void SendMessage(RR_SHARED_PTR<Message> m)=0;

		virtual void AsyncSendMessage(RR_SHARED_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> handler) = 0;

	public:
		virtual void MessageReceived(RR_SHARED_PTR<Message> m)=0;

		RR_SHARED_PTR<Message> SpecialRequest(RR_SHARED_PTR<Message> m);

	public:
		virtual void Close();


		virtual void PeriodicCleanupTask();


		virtual uint32_t TransportCapability(std::string name);

		//typedef void (*TransportListenerDelegate)(const RR_SHARED_PTR<Transport> &transport, TransportListenerEventType ev, const RR_SHARED_PTR<void> &parameter);

		boost::signals2::signal<void (RR_SHARED_PTR<Transport> transport, TransportListenerEventType ev, RR_SHARED_PTR<void> parameter   )> TransportListeners;

		virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode() =0;

		virtual std::vector<NodeDiscoveryInfo> GetDetectedNodes();

		virtual void AsyncGetDetectedNodes(boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

	protected:
		void FireTransportEventListener(RR_SHARED_PTR<Transport> shared_this, TransportListenerEventType ev, RR_SHARED_PTR<void> parameter);
	};

}
