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


#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/signals2.hpp>
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <list>

#pragma warning (push)
#pragma warning (disable: 4250)

namespace RobotRaconteur
{
	
	class PipeBase;

	class PipeEndpointBase :  public RR_ENABLE_SHARED_FROM_THIS<PipeEndpointBase>
	{
		friend class PipeBase;
		friend class PipeClientBase;
		friend class PipeServerBase;

	public:

		virtual ~PipeEndpointBase() {}

		virtual int32_t GetIndex();

		virtual uint32_t GetEndpoint();

		bool GetRequestPacketAck();

		void SetRequestPacketAck(bool ack);
		
		virtual void Close();

		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		void AsyncClose(boost::asio::yield_context& yield, int32_t timeout=RR_TIMEOUT_INFINITE)
		{			
			detail::async_wrap_for_spawn_void(boost::bind((void (PipeEndpointBase::*)(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>, int32_t))&PipeEndpointBase::AsyncClose,this,_1,timeout),yield);
		}
#endif

		virtual size_t Available();

		bool IsUnreliable();

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

	protected:

		virtual void RemoteClose();

		PipeEndpointBase(RR_SHARED_PTR<PipeBase> parent, int32_t index, uint32_t endpoint=0, bool unreliable=false);

		bool unreliable;

		bool RequestPacketAck;

		void AsyncSendPacketBase(RR_SHARED_PTR<RRObject> packet, boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)> handler);

		RR_SHARED_PTR<RRObject> ReceivePacketBase();

		RR_SHARED_PTR<RRObject> PeekPacketBase();

		boost::mutex sendlock;
		boost::mutex recvlock;

		std::deque<RR_SHARED_PTR<RRObject> > recv_packets;

		uint32_t increment_packet_number(uint32_t packetnum);

	
		void PipePacketReceived(RR_SHARED_PTR<RRObject> packet, uint32_t packetnum);

		void PipePacketAckReceived(uint32_t packetnum);

	protected:
		virtual void fire_PipeEndpointClosedCallback()=0;
	
		virtual void fire_PacketReceivedEvent()=0;
		
		virtual void fire_PacketAckReceivedEvent(uint32_t packetnum)=0;

		RR_SHARED_PTR<PipeBase> GetParent();

	private:
		
		uint32_t send_packet_number;
		uint32_t recv_packet_number;

		RR_WEAK_PTR<PipeBase> parent;
		int32_t index;
		uint32_t endpoint;

		RR_UNORDERED_MAP<uint32_t,RR_SHARED_PTR<RRObject> > out_of_order_packets;

	};

	template <class T>
	class PipeEndpoint : public PipeEndpointBase
	{
	private:
		boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> PipeEndpointClosedCallback;
		boost::mutex PipeEndpointClosedCallback_lock;
	public:
		
		boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeEndpointClosedCallback()
		{
			boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
			return PipeEndpointClosedCallback;
		}

		void SetPipeEndpointClosedCallback(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> callback)
		{
			boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
			PipeEndpointClosedCallback=callback;
		}

		boost::signals2::signal<void (RR_SHARED_PTR<PipeEndpoint<T> >)> PacketReceivedEvent;
		boost::signals2::signal<void (RR_SHARED_PTR<PipeEndpoint<T> >,uint32_t)> PacketAckReceivedEvent;

		virtual uint32_t SendPacket(T packet)
		{
			 RR_SHARED_PTR<detail::sync_async_handler<uint32_t> > t=RR_MAKE_SHARED<detail::sync_async_handler<uint32_t> >();
			 AsyncSendPacket(packet,boost::bind(&PipeEndpoint::send_handler,_1,_2,boost::protect(boost::bind(&detail::sync_async_handler<uint32_t>::operator(),t,_1,_2))));
			 return *t->end();
		}

		virtual void AsyncSendPacket(T packet, boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)> handler)
		{
			 AsyncSendPacketBase(RRPrimUtil<T>::PrePack(packet), handler);
		}

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		uint32_t AsyncSendPacket(T packet, boost::asio::yield_context& yield)
		{
			return detail::async_wrap_for_spawn(boost::bind((void (PipeEndpointBase::*)(T,boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)>))&PipeEndpointBase::AsyncSendPacket,this,boost::ref(packet),_1),yield);
		}
#endif

		virtual T ReceivePacket()
		{
			return RRPrimUtil<T>::PreUnpack(ReceivePacketBase());
		}

		virtual T PeekNextPacket()
		{
			return RRPrimUtil<T>::PreUnpack(PeekPacketBase());
		}

		PipeEndpoint(RR_SHARED_PTR<PipeBase> parent, int32_t index, uint32_t endpoint=0, bool unreliable=false) 
			: PipeEndpointBase(parent,index,endpoint,unreliable) {};

		

	protected:
		virtual void fire_PipeEndpointClosedCallback()
		{
			boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> c=GetPipeEndpointClosedCallback();
			if (!c) return;
			c(rr_cast<PipeEndpoint<T> >(shared_from_this()));
		}
	
	
		virtual void fire_PacketReceivedEvent()
		{
			PacketReceivedEvent(rr_cast<PipeEndpoint<T> >(shared_from_this()));
		}
		
		virtual void fire_PacketAckReceivedEvent(uint32_t packetnum)
		{
			PacketAckReceivedEvent(rr_cast<PipeEndpoint<T> >(shared_from_this()),packetnum);
		}

		static void send_handler(uint32_t packetnumber, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<uint32_t>,RR_SHARED_PTR<RobotRaconteurException>)> handler)
		{
			handler(RR_MAKE_SHARED<uint32_t>(packetnumber),err);
		}

	public:
		virtual void Close()
		{
			PipeEndpointBase::Close();
			{
			
				boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
				PipeEndpointClosedCallback.clear();

			}
			PacketReceivedEvent.disconnect_all_slots();
			PacketAckReceivedEvent.disconnect_all_slots();
		}

	protected:

		virtual void AsyncClose1(RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler)
		{
			try
			{
				{
					boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
					PipeEndpointClosedCallback.clear();
				}
			PacketReceivedEvent.disconnect_all_slots();
			PacketAckReceivedEvent.disconnect_all_slots();
			}
			catch (std::exception&) {}

			
			handler(err);
		}

	public:
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=2000)
		{
			PipeEndpointBase::AsyncClose(boost::bind(&PipeEndpoint<T>::AsyncClose1,rr_cast<PipeEndpoint<T> >(shared_from_this()),_1,handler),timeout);
			
		}

	protected:
		


		virtual void RemoteClose()
		{
			PipeEndpointBase::RemoteClose();
			{
			boost::mutex::scoped_lock lock(PipeEndpointClosedCallback_lock);
			PipeEndpointClosedCallback.clear();
			}
			PacketReceivedEvent.disconnect_all_slots();
			PacketAckReceivedEvent.disconnect_all_slots();
		}

	};

	class PipeBase : public RR_ENABLE_SHARED_FROM_THIS<PipeBase>
	{
		friend class PipeEndpointBase;

	public:

		virtual ~PipeBase() {}

		static const int32_t ANY_INDEX=-1;

		virtual std::string GetMemberName()=0;

		virtual void PipePacketReceived(RR_SHARED_PTR<MessageEntry> m, uint32_t e=0)=0;

		virtual void Shutdown()=0;


		

		virtual void AsyncClose(RR_SHARED_PTR<PipeEndpointBase> endpoint, bool remote, uint32_t ee, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)=0;

	protected:

		bool unreliable;

		virtual void AsyncSendPipePacket(RR_SHARED_PTR<RRObject> data, int32_t index, uint32_t packetnumber, bool requestack, uint32_t endpoint, bool unreliable, boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)> handler)=0;

		bool rawelements;

		void DispatchPacketAck (RR_SHARED_PTR<MessageElement> me, RR_SHARED_PTR<PipeEndpointBase> e);

		bool DispatchPacket(RR_SHARED_PTR<MessageElement> me, RR_SHARED_PTR<PipeEndpointBase> e, uint32_t& packetnumber);

		RR_SHARED_PTR<MessageElement> PackPacket(RR_SHARED_PTR<RRObject> data, int32_t index, uint32_t packetnumber, bool requestack);

		virtual void DeleteEndpoint(RR_SHARED_PTR<PipeEndpointBase> e)=0;

		virtual RR_SHARED_PTR<MessageElementData> PackData(RR_SHARED_PTR<RRObject> data)
		{
			return GetNode()->PackVarType(data);
		}

		virtual RR_SHARED_PTR<RRObject> UnpackData(RR_SHARED_PTR<MessageElement> mdata)
		{
			return GetNode()->UnpackVarType(mdata);
		}

		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

	};


	template <class T>
	class Pipe : public virtual PipeBase
	{
	public:
		
		friend class PipeEndpointBase;

		virtual ~Pipe() {}

		virtual boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeConnectCallback()=0;
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> function)=0;
		
		virtual RR_SHARED_PTR<PipeEndpoint<T> > Connect(int32_t index)=0;

		virtual void AsyncConnect(int32_t index, boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)=0;

		virtual void AsyncConnect(boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			AsyncConnect(-1,handler,timeout);
		}

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		RR_SHARED_PTR<PipeEndpoint<T> > AsyncConnect(int32_t index, const boost::asio::yield_context& yield, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			return detail::async_wrap_for_spawn(boost::bind((void (Pipe<T>::*)(int32_t, boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)>, int32_t))&Pipe<T>::AsyncConnect,this,boost::ref(index),_1,timeout),yield);
		}
#endif

		virtual RR_SHARED_PTR<MessageElementData> PackData(RR_SHARED_PTR<RRObject> data)
		{
			return GetNode()->template PackAnyType<T>(data);
		}

		virtual RR_SHARED_PTR<RRObject> UnpackData(RR_SHARED_PTR<MessageElement> mdata)
		{
			return GetNode()->template UnpackAnyType<T>(mdata);
		}
		
		
	};

	
	class ServiceStub;
	
	class PipeClientBase : public virtual PipeBase
	{
	public:

		virtual ~PipeClientBase() {}

		virtual std::string GetMemberName();

		virtual void PipePacketReceived(RR_SHARED_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		

		virtual void AsyncClose(RR_SHARED_PTR<PipeEndpointBase> endpoint, bool remote, uint32_t ee, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		RR_SHARED_PTR<ServiceStub> GetStub();

	protected:

		virtual void AsyncSendPipePacket(RR_SHARED_PTR<RRObject> data, int32_t index, uint32_t packetnumber, bool requestack, uint32_t endpoint, bool unreliable, boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)> handler);

		
		std::string m_MemberName;

		RR_UNORDERED_MAP<int32_t,RR_SHARED_PTR<PipeEndpointBase> > pipeendpoints;
		boost::mutex pipeendpoints_lock;

		RR_WEAK_PTR<ServiceStub> stub;
		

		void AsyncConnect_internal(int32_t index, boost::function<void (RR_SHARED_PTR<PipeEndpointBase>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		void AsyncConnect_internal1(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, int32_t index, boost::function<void (RR_SHARED_PTR<PipeEndpointBase>,RR_SHARED_PTR<RobotRaconteurException>)> handler);

		PipeClientBase(std::string name, RR_SHARED_PTR<ServiceStub> stub, bool unreliable);

		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable)=0;

		virtual void DeleteEndpoint(RR_SHARED_PTR<PipeEndpointBase> e);

		

	};


	template <class T>
	class PipeClient : public virtual Pipe<T>, public virtual PipeClientBase
	{
	public:

		virtual ~PipeClient() {}
		
		virtual boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeConnectCallback()
		{
			throw std::runtime_error("Not valid for client");
		}
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> function)
		{
			throw std::runtime_error("Not valid for client");
		}
		
		virtual void AsyncConnect(int32_t index, boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			
			AsyncConnect_internal(index,boost::bind(handler,boost::bind(&rr_cast<PipeEndpoint<T>, PipeEndpointBase >,_1),_2),timeout); 
		}

		virtual RR_SHARED_PTR<PipeEndpoint<T> > Connect(int32_t index)
		{
			RR_SHARED_PTR<detail::sync_async_handler<PipeEndpoint<T> > > t=RR_MAKE_SHARED<detail::sync_async_handler<PipeEndpoint<T> > >();
			AsyncConnect(index,boost::bind(&detail::sync_async_handler<PipeEndpoint<T> >::operator(),t,_1,_2),GetNode()->GetTransactionTimeout());
			return t->end();
		}

		PipeClient(std::string name, RR_SHARED_PTR<ServiceStub> stub, bool unreliable=false) : PipeClientBase(name,stub,unreliable) 
		{
			if (typeid(T)==typeid(MessageElement))
			{
				rawelements=true;
			}
			else
			{
				rawelements=false;
			}
		
		}		

		using PipeClientBase::GetMemberName;
		using PipeClientBase::PipePacketReceived;
		using PipeClientBase::Shutdown;
		using PipeClientBase::AsyncSendPipePacket;
		using PipeClientBase::AsyncClose;
		

	protected:
		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, bool unreliable)
		{
			return RR_MAKE_SHARED<PipeEndpoint<T> >(rr_cast<PipeClient<T> >(shared_from_this()),index,0,unreliable);
		}

	};


	class ServiceSkel;
	class PipeServerBase : public virtual PipeBase
	{
	public:

		virtual ~PipeServerBase() {}

		virtual std::string GetMemberName();

		virtual void PipePacketReceived(RR_SHARED_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		virtual void AsyncSendPipePacket(RR_SHARED_PTR<RRObject> data, int32_t index, uint32_t packetnumber, bool requestack, uint32_t endpoint, bool unreliable,boost::function<void(uint32_t,RR_SHARED_PTR<RobotRaconteurException>)> handler);

		virtual void AsyncClose(RR_SHARED_PTR<PipeEndpointBase> endpoint, bool remote, uint32_t ee, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		virtual RR_SHARED_PTR<MessageEntry> PipeCommand(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		RR_SHARED_PTR<ServiceSkel> GetSkel();

	protected:
		
		std::string m_MemberName;
				
		struct pipe_endpoint_server_id
		{
			pipe_endpoint_server_id(uint32_t endpoint, int32_t index)
			{
				this->endpoint=endpoint;
				this->index=index;
			}

			uint32_t endpoint;
			int32_t index;

			bool operator==(const pipe_endpoint_server_id& rhs) const
			{
				return (endpoint==rhs.endpoint && index==rhs.index);
			}
						
		};
		

		struct hash_value
		{
		 std::size_t operator()(pipe_endpoint_server_id const& e) const {
		  std::size_t seed = 0;
		  boost::hash_combine( seed, e.endpoint );
		  boost::hash_combine( seed, e.index );
		  return seed;
		}
		};

		RR_UNORDERED_MAP<pipe_endpoint_server_id,RR_SHARED_PTR<PipeEndpointBase> , hash_value > pipeendpoints;
		boost::mutex pipeendpoints_lock;

		RR_WEAK_PTR<ServiceSkel> skel;

		
		PipeServerBase(std::string name, RR_SHARED_PTR<ServiceSkel> skel, bool unreliable);

		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index,uint32_t endpoint, bool unreliable)=0;

		void DeleteEndpoint(RR_SHARED_PTR<PipeEndpointBase> e);

		virtual void fire_PipeConnectCallback(RR_SHARED_PTR<PipeEndpointBase> e)=0;

		
		bool init;
		boost::signals2::connection listener_connection;
		
	public:
		void ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param);

	};

	template <class T>
	class PipeServer : public virtual PipeServerBase, public virtual Pipe<T>
	{

	public:

		virtual ~PipeServer() {}

		virtual boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> GetPipeConnectCallback()
		{
			return callback;
		}
		
		virtual void SetPipeConnectCallback(boost::function<void(RR_SHARED_PTR<PipeEndpoint<T> >)> function)
		{
			callback=function;
		}
		
		virtual RR_SHARED_PTR<PipeEndpoint<T> > Connect(int32_t index)
		{
			throw std::runtime_error("Not valid for server");
		}

		virtual void AsyncConnect(int32_t index, boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			throw std::runtime_error("Not valid for server");
		}

		PipeServer(std::string name, RR_SHARED_PTR<ServiceSkel> skel, bool unreliable=false) : PipeServerBase(name,skel,unreliable) 
		{
			if (typeid(T)==typeid(MessageElement))
			{
				rawelements=true;
			}
			else
			{
				rawelements=false;
			}
		}


	protected:
		virtual RR_SHARED_PTR<PipeEndpointBase> CreateNewPipeEndpoint(int32_t index, uint32_t endpoint, bool unreliable)
		{
			return RR_MAKE_SHARED<PipeEndpoint<T> >(rr_cast<PipeServer<T> >(shared_from_this()),index,endpoint,unreliable);
		}

		boost::function<void (RR_SHARED_PTR<PipeEndpoint<T> >)> callback;

		virtual void fire_PipeConnectCallback(RR_SHARED_PTR<PipeEndpointBase> e)
		{
			if (!callback) return;
			callback(rr_cast<PipeEndpoint<T> >(e));
		}

	public:

		virtual void Shutdown()
		{
			PipeServerBase::Shutdown();

			callback.clear();
		}


	};

	template <class T>
	class PipeBroadcaster : public RR_ENABLE_SHARED_FROM_THIS<PipeBroadcaster<T> >
	{

	protected:
		class connected_endpoint
		{
		public:
			connected_endpoint(RR_SHARED_PTR<PipeEndpoint<T> > ep)
			{
				//backlog=0;
				endpoint=ep;
				bool sending=false;
			}

			//Endpoints are stored within the pipe so it is safe to use weak_ptr here
			RR_WEAK_PTR<PipeEndpoint<T> > endpoint;
			std::list<uint32_t> backlog;
			std::list<uint32_t> forward_backlog;
			bool sending;
		};

		std::list<RR_SHARED_PTR<connected_endpoint> > endpoints;
		boost::mutex endpoints_lock;

		void EndpointConnected(RR_SHARED_PTR<PipeEndpoint<T> > ep)
		{
			boost::mutex::scoped_lock lock(endpoints_lock);

			if (maximum_backlog!=0)
			{
				ep->SetRequestPacketAck(true);
			}

			RR_SHARED_PTR<connected_endpoint> cep=RR_MAKE_SHARED<connected_endpoint>(ep);
			ep->SetPipeEndpointClosedCallback(boost::bind(&PipeBroadcaster<T>::EndpointClosed,this->shared_from_this(),cep));
			ep->PacketReceivedEvent.connect(boost::bind(&PipeBroadcaster<T>::PacketReceived,this->shared_from_this(),_1));
			ep->PacketAckReceivedEvent.connect(boost::bind(&PipeBroadcaster<T>::PacketAckReceived,this->shared_from_this(),cep,_2));
			endpoints.push_back(cep);
		}


		void EndpointClosed(RR_SHARED_PTR<connected_endpoint> ep)
		{
			boost::mutex::scoped_lock lock(endpoints_lock);
			try
			{
				endpoints.remove(ep);
			}
			catch (std::exception&) {}
		}

		void PacketAckReceived(RR_SHARED_PTR<connected_endpoint> ep, uint32_t id)
		{
			boost::mutex::scoped_lock lock(endpoints_lock);
			try
			{
				if (std::count(ep->backlog.begin(),ep->backlog.end(),id)==0)
				{
					ep->forward_backlog.push_back(id);
				}
				else
				{
					ep->backlog.remove(id);
				}
			}
			catch (std::exception&) {}
		}

		void PacketReceived(RR_SHARED_PTR<PipeEndpoint<T> > ep)
		{

			//Received packets are discarded. This prevents a possible memory leak.
			boost::mutex::scoped_lock lock(endpoints_lock);
			
			try
			{
				while(ep->Available() > 0)
				{
					ep->ReceivePacket();
				}
			}
			catch (std::exception&) {}
		}

		RR_WEAK_PTR<Pipe<T> > pipe;
		int32_t maximum_backlog;
		
		struct async_send_operation
		{
			boost::mutex keys_lock;
			std::list<int32_t> keys;
		};

		void handle_send(int32_t id, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<connected_endpoint> ep, RR_SHARED_PTR<async_send_operation> op, int32_t key, boost::function<void()> handler) 
		{
			if (maximum_backlog!=-1)
			{
				boost::mutex::scoped_lock lock(endpoints_lock);
				if (std::count(ep->forward_backlog.begin(),ep->forward_backlog.end(),id)!=0)
				{
					ep->forward_backlog.remove(id);
				}
				else
				{
					ep->backlog.push_back(id);
				}
			}

			{
				boost::mutex::scoped_lock lock(op->keys_lock);
				op->keys.remove(key);
				if (op->keys.size()!=0) return;
			}

			try
			{
				handler();
			}
			catch (std::exception& e)
			{
				RR_SHARED_PTR<Pipe<T> > p=pipe.lock();
				if (!p) return;
				p->GetNode()->HandleException(&e);
			}
		
		}

	public:
		PipeBroadcaster() {}

		void Init(RR_SHARED_PTR<Pipe<T> > pipe, int32_t maximum_backlog=-1)
		{
			
			this->pipe=pipe;
			this->maximum_backlog=maximum_backlog;
			
			pipe->SetPipeConnectCallback(boost::bind(&PipeBroadcaster<T>::EndpointConnected,this->shared_from_this(),_1));
		}

		void SendPacket(T packet)
		{
			RR_SHARED_PTR<detail::sync_async_handler<void> > t=RR_MAKE_SHARED<detail::sync_async_handler<void> >(RR_MAKE_SHARED<TransactionTimeoutException>("Timeout"));
			AsyncSendPacket(packet,boost::bind(&detail::sync_async_handler<void>::operator(),t));
			t->end_void();
		}

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		void AsyncSendPacket(T packet, const boost::asio::yield_context& yield)
		{
			detail::async_wrap_for_spawn_void(boost::bind((void (PipeBroadcaster<T>::*)(T packet, boost::function<void()>))&PipeBroadcaster<T>::AsyncSendPacket,this,_1),yield);
		}
#endif

		void AsyncSendPacket(T packet, boost::function<void()> handler)
		{

			boost::mutex::scoped_lock lock(endpoints_lock);

			RR_SHARED_PTR<async_send_operation> op=RR_MAKE_SHARED<async_send_operation>();
			boost::mutex::scoped_lock lock2(op->keys_lock);
			int32_t count=0;

			for (typename std::list<RR_SHARED_PTR<connected_endpoint> >::iterator ee=endpoints.begin(); ee!=endpoints.end(); )
			{
				try
				{
					typename std::list<RR_SHARED_PTR<connected_endpoint> >::iterator ee2=ee;

					RR_SHARED_PTR<PipeEndpoint<T> > ep=(*ee)->endpoint.lock();
					if (!ep)
					{
						ee=endpoints.erase(ee);
						continue;
					}
					else
					{
						ee++;
					}

					if (maximum_backlog!=-1 && ((int32_t)(*ee2)->backlog.size()) > maximum_backlog)
					{
						continue;
					}
					
					ep->AsyncSendPacket(packet,boost::bind(&PipeBroadcaster<T>::handle_send,this->shared_from_this(),_1,_2,*ee2,op,count,handler));
					op->keys.push_back(count);
					count++;
				}
				catch (std::exception&) {}

			}
		}



	};

}

#pragma warning(pop)
