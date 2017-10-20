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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/shared_array.hpp>

#pragma once

namespace RobotRaconteur
{
	enum IPNodeDiscoveryFlags
	{
		IPNodeDiscoveryFlags_NODE_LOCAL = 0x1,
		IPNodeDiscoveryFlags_LINK_LOCAL = 0x2,
		IPNodeDiscoveryFlags_SITE_LOCAL = 0x4

	};
		

	class TcpTransportConnection;
	namespace detail
	{
		class TcpConnector;
	}

	class TcpTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<TcpTransport>
	{
		friend class TcpTransportConnection;
		friend class detail::TcpConnector;

	private:
		int32_t m_Port;

		bool transportopen;

		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
		boost::mutex TransportConnections_lock;
		

		TcpTransport(RR_SHARED_PTR<RobotRaconteurNode> node=RobotRaconteurNode::sp());

		virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual ~TcpTransport();


	public:
		virtual bool IsServer() const;
		virtual bool IsClient() const;

		
		virtual int32_t GetDefaultReceiveTimeout();
		virtual void SetDefaultReceiveTimeout(int32_t milliseconds);
		virtual int32_t GetDefaultConnectTimeout();
		virtual void SetDefaultConnectTimeout(int32_t milliseconds);

		virtual std::string GetUrlSchemeString() const;

		virtual int32_t GetListenPort();

		virtual void SendMessage(RR_SHARED_PTR<Message> m);

		virtual void AsyncSendMessage(RR_SHARED_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);

		virtual void AsyncCreateTransportConnection(std::string url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) > callback);


		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(std::string url, RR_SHARED_PTR<Endpoint> e);

		

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

	protected:

		virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<void> timer);
		
	public:

		

		virtual void StartServer(int32_t porte);

		
		virtual bool CanConnectService(std::string url);

		

		
		virtual void Close();

		virtual void CheckConnection(uint32_t endpoint);

		
		void EnableNodeDiscoveryListening(uint32_t flags=(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL));

		void DisableNodeDiscoveryListening();

		
		void EnableNodeAnnounce(uint32_t flags=(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL));

		void DisableNodeAnnounce();


		virtual void PeriodicCleanupTask();

		

		

		uint32_t TransportCapability(std::string name);


		static void GetLocalAdapterIPAddresses(std::vector<boost::asio::ip::address>& addresses);

		virtual void MessageReceived(RR_SHARED_PTR<Message> m);

		virtual int32_t GetDefaultHeartbeatPeriod();
		virtual void SetDefaultHeartbeatPeriod(int32_t milliseconds);

	protected:

		boost::mutex node_discovery_lock;
		RR_SHARED_PTR<void> node_discovery;

		RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> ipv4_acceptor;
		RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> ipv6_acceptor;
		boost::mutex acceptor_lock;

		static void handle_v4_accept(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error);

		static void handle_v6_accept(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error);

		virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
		virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);

		int32_t heartbeat_period;
		boost::mutex heartbeat_period_lock;
	
		int32_t default_connect_timeout;
		boost::mutex default_connect_timeout_lock;

		int32_t default_receive_timeout;
		boost::mutex default_receive_timeout_lock;
		


	};



	
}
