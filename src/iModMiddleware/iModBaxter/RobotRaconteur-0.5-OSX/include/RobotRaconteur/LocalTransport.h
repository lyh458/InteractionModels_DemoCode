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
#include <boost/asio/windows/stream_handle.hpp>

#pragma once



namespace RobotRaconteur
{
	class LocalTransportConnection;


	namespace detail
	{
#ifdef BOOST_WINDOWS
		namespace windows
		{
			// Boost ASIO doesn't have a named pipe acceptor :/
			class named_pipe_acceptor;
		}
#endif					
		class LocalTransportFD;			
	}
		
	class LocalTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<LocalTransport>
	{
		friend class LocalTransportConnection;
		
	private:
		std::string socket_file_name;

		bool transportopen;

		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

#ifdef BOOST_WINDOWS
		typedef boost::asio::windows::stream_handle socket_type;
		typedef detail::windows::named_pipe_acceptor socket_acceptor_type;
#else
		typedef boost::asio::local::stream_protocol::socket socket_type;
		typedef boost::asio::local::stream_protocol::acceptor socket_acceptor_type;
#endif

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
		boost::mutex TransportConnections_lock;
		

		LocalTransport(RR_SHARED_PTR<RobotRaconteurNode> node=RobotRaconteurNode::sp());

		virtual RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual ~LocalTransport();
		
		
	public:
		virtual bool IsServer() const;
		
		virtual bool IsClient() const;

								
		virtual std::string GetUrlSchemeString() const;

		virtual void SendMessage(RR_SHARED_PTR<Message> m);

		virtual void AsyncSendMessage(RR_SHARED_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);

		virtual void AsyncCreateTransportConnection(std::string url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) > callback);

	 
		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(std::string url, RR_SHARED_PTR<Endpoint> e);

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

	protected:

		virtual void AsyncCreateTransportConnection2(RR_SHARED_PTR<LocalTransport::socket_type> socket , RR_SHARED_PTR<ITransportConnection> transport, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) > callback);

		virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<void> timer);
		
	public:

		virtual void StartServerAsNodeName(const std::string& name);

		virtual void StartServerAsNodeID(const NodeID& nodeid);
		
		virtual bool CanConnectService(std::string url);
		
		virtual void Close();
		
		virtual void CheckConnection(uint32_t endpoint);

		virtual void PeriodicCleanupTask();
						
		uint32_t TransportCapability(std::string name);
		
		virtual void MessageReceived(RR_SHARED_PTR<Message> m);

		virtual void AsyncGetDetectedNodes(boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

	protected:

		
		RR_SHARED_PTR<socket_acceptor_type> acceptor;
		
		boost::mutex acceptor_lock;

		static void handle_accept(RR_SHARED_PTR<LocalTransport> parent,RR_SHARED_PTR<socket_acceptor_type> acceptor, RR_SHARED_PTR<socket_type> socket, const boost::system::error_code& error);
		
		virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
		virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);
		
		boost::mutex fds_lock;
		RR_SHARED_PTR<detail::LocalTransportFD> fds;
	};

	class NodeNameAlreadyInUse : public std::runtime_error 
	{
	public:
		NodeNameAlreadyInUse() : runtime_error("Could not start server: NodeName already in use") {};
	};

	class NodeIDAlreadyInUse : public std::runtime_error 
	{
	public:
		NodeIDAlreadyInUse() : runtime_error("Could not start server: NodeID already in use") {};
	};
}
