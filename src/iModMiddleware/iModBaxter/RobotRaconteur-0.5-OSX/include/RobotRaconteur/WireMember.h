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
#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/signals2.hpp>

#pragma warning (push)
#pragma warning(disable: 4250)

namespace RobotRaconteur
{
	class WireBase;

	class WireConnectionBase : public RR_ENABLE_SHARED_FROM_THIS<WireConnectionBase>
	{

		friend class WireBase;
		friend class WireClientBase;
		friend class WireServerBase;

	public:
		virtual uint32_t GetEndpoint();
		
		virtual TimeSpec GetLastValueReceivedTime();

		virtual TimeSpec GetLastValueSentTime();
		
		virtual void Close();

		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		void AsyncClose(boost::asio::yield_context& yield, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			detail::async_wrap_for_spawn_void(boost::bind((void (WireConnectionBase::*)(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>, int32_t))&WireConnectionBase::AsyncClose,this,_1,timeout),yield);
		}
#endif

		WireConnectionBase(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint=0);

		virtual ~WireConnectionBase() {}

		virtual void WirePacketReceived(TimeSpec timespec, RR_SHARED_PTR<RRObject> packet);

		

		virtual bool GetInValueValid();

		virtual bool GetOutValueValid();

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

	protected:

		virtual void RemoteClose();

		RR_SHARED_PTR<RRObject> inval;
		RR_SHARED_PTR<RRObject> outval;

		bool inval_valid;
		TimeSpec lasttime_send;

		bool  outval_valid;
		TimeSpec lasttime_recv;

		uint32_t endpoint;
		RR_WEAK_PTR<WireBase> parent;

		boost::mutex sendlock;
		boost::mutex recvlock;


		RR_SHARED_PTR<RRObject> GetInValueBase();

		RR_SHARED_PTR<RRObject> GetOutValueBase();

		void SetOutValueBase(RR_SHARED_PTR<RRObject> value);
		
		virtual void fire_WireValueChanged(RR_SHARED_PTR<RRObject> value, TimeSpec time)=0;
	
		virtual void fire_WireClosedCallback()=0;
	

		RR_SHARED_PTR<WireBase> GetParent();

		boost::mutex inval_lock;
		boost::mutex outval_lock;

	};

	template <class T>
	class WireConnection : public virtual WireConnectionBase
	{
	private:
		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> WireConnectionClosedCallback;
		boost::mutex WireConnectionClosedCallback_lock;

	public:
		boost::signals2::signal<void (RR_SHARED_PTR<WireConnection<T> > connection, T value, TimeSpec time)> WireValueChanged;
		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectionClosedCallback()
		{
			boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
			return WireConnectionClosedCallback;
		}

		void SetWireConnectionClosedCallback(boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> callback)
		{
			boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
			WireConnectionClosedCallback=callback;
		}

		virtual ~WireConnection() {}

		virtual T GetInValue()
		{
			return RRPrimUtil<T>::PreUnpack(GetInValueBase());
		}

		virtual T GetOutValue()
		{
			return RRPrimUtil<T>::PreUnpack(GetOutValueBase());
		}

		virtual void SetOutValue(T value)
		{
			SetOutValueBase(RRPrimUtil<T>::PrePack(value));
		}

		WireConnection(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint=0) 
			: WireConnectionBase(parent,endpoint) {}

	protected:
		virtual void fire_WireValueChanged(RR_SHARED_PTR<RRObject> value, TimeSpec time)
		{
			WireValueChanged(rr_cast<WireConnection<T> >(shared_from_this()),RRPrimUtil<T>::PreUnpack(value),time);
		}
	
		virtual void fire_WireClosedCallback()
		{
			boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> c=GetWireConnectionClosedCallback();
			if (!c) return;
			c(rr_cast<WireConnection<T> >(shared_from_this()));
		}

	public:
		virtual void Close()
		{
			WireConnectionBase::Close();
			{
				boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
				WireConnectionClosedCallback.clear();
			}
			WireValueChanged.disconnect_all_slots();
		}

	protected:

		virtual void AsyncClose1(RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler)
		{
			try
			{
				{
					boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
					WireConnectionClosedCallback.clear();
				}
				WireValueChanged.disconnect_all_slots();
			}
			catch (std::exception&) {}

			
			handler(err);
		}

	public:
		virtual void AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=2000)
		{
			WireConnectionBase::AsyncClose(boost::bind(&WireConnection<T>::AsyncClose1,rr_cast<WireConnection<T> >(shared_from_this()),_1,handler),timeout);
			
		}

	protected:
		virtual void RemoteClose()
		{
			WireConnectionBase::RemoteClose();
			{
				boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
				WireConnectionClosedCallback.clear();
			}
			WireValueChanged.disconnect_all_slots();
		}


	};


	class WireBase : public RR_ENABLE_SHARED_FROM_THIS<WireBase>
	{
		

	public:

		friend class WireConnectionBase;
		
		virtual ~WireBase() {}

		virtual std::string GetMemberName()=0;

		virtual void WirePacketReceived(RR_SHARED_PTR<MessageEntry> m, uint32_t e=0)=0;

		virtual void Shutdown()=0;


		

		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)=0;

	protected:

		virtual void SendWirePacket(RR_SHARED_PTR<RRObject> data, TimeSpec time, uint32_t endpoint)=0;

		bool rawelements;

		void DispatchPacket (std::vector<RR_SHARED_PTR<MessageElement> >& me, RR_SHARED_PTR<WireConnectionBase> e);

		std::vector<RR_SHARED_PTR<MessageElement> > PackPacket(RR_SHARED_PTR<RRObject> data, TimeSpec time);

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
	class Wire : public virtual WireBase
	{

		friend class WireConnectionBase;

	public:

		virtual ~Wire() {}
		
		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback()=0;
		
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function)=0;
		
		virtual RR_SHARED_PTR<WireConnection<T> > Connect()=0;

		virtual void AsyncConnect(boost::function<void (RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)=0;

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		RR_SHARED_PTR<WireConnection<T> > AsyncConnect(boost::asio::yield_context& yield, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			return detail::async_wrap_for_spawn(boost::bind((void (Wire<T>::*)(int32_t, boost::function<void(RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>, int32_t))&Wire<T>::AsyncConnect,this,_1,timeout),yield);
		}
#endif

	protected:

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
	
	class WireClientBase : public virtual WireBase
	{
		friend class WireConnectionBase;

	public:

		virtual ~WireClientBase() {}

		virtual std::string GetMemberName();

		virtual void WirePacketReceived(RR_SHARED_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		

		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		RR_SHARED_PTR<ServiceStub> GetStub();

	protected:

		virtual void SendWirePacket(RR_SHARED_PTR<RRObject> packet, TimeSpec time, uint32_t endpoint);

		std::string m_MemberName;
		
		RR_SHARED_PTR<WireConnectionBase> connection;
		boost::mutex connection_lock;

		RR_WEAK_PTR<ServiceStub> stub;

		void AsyncConnect_internal(boost::function<void (RR_SHARED_PTR<WireConnectionBase>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		void AsyncConnect_internal1(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void (RR_SHARED_PTR<WireConnectionBase>,RR_SHARED_PTR<RobotRaconteurException>)> handler);


		WireClientBase(std::string name, RR_SHARED_PTR<ServiceStub> stub);

		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection()=0;

		

	};


	template <class T>
	class WireClient : public virtual Wire<T>, public virtual WireClientBase
	{
	public:

		virtual ~WireClient() {}

		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback()
		{
			throw std::runtime_error("Not valid for client");
		}
		
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function)
		{
			throw std::runtime_error("Not valid for client");
		}
		
		virtual void AsyncConnect(boost::function<void (RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			AsyncConnect_internal(boost::bind(handler,boost::bind(&rr_cast<WireConnection<T>, WireConnectionBase >,_1),_2),timeout); 
		}

		virtual RR_SHARED_PTR<WireConnection<T> > Connect()
		{
			RR_SHARED_PTR<detail::sync_async_handler<WireConnection<T> > > t=RR_MAKE_SHARED<detail::sync_async_handler<WireConnection<T> > >();
			AsyncConnect(boost::bind(&detail::sync_async_handler<WireConnection<T> >::operator(),t,_1,_2),GetNode()->GetTransactionTimeout());
			return t->end(); 
		}

		WireClient(std::string name, RR_SHARED_PTR<ServiceStub> stub) : WireClientBase(name,stub) 
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

		/*using WireClientBase::GetMemberName;
		using WireClientBase::WirePacketReceived;
		using WireClientBase::Shutdown;
		using WireClientBase::Close;
		using WireClientBase::SendWirePacket;*/



	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection()
		{
			return RR_MAKE_SHARED<WireConnection<T> >(rr_cast<WireClient<T> >(shared_from_this()));
		}

	};


	class ServiceSkel;
	class WireServerBase : public virtual WireBase
	{
		friend class WireConnectionBase;

	public:

		virtual ~WireServerBase() {}

		virtual std::string GetMemberName();

		virtual void WirePacketReceived(RR_SHARED_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		virtual RR_SHARED_PTR<MessageEntry> WireCommand(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		RR_SHARED_PTR<ServiceSkel> GetSkel();

	protected:

		virtual void SendWirePacket(RR_SHARED_PTR<RRObject> data, TimeSpec time, uint32_t endpoint);

		std::string m_MemberName;

		RR_UNORDERED_MAP<uint32_t,RR_SHARED_PTR<WireConnectionBase> > connections;
		boost::mutex connections_lock;

		RR_WEAK_PTR<ServiceSkel> skel;

		
		WireServerBase(std::string name, RR_SHARED_PTR<ServiceSkel> skel);

		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t e)=0;
				
		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)=0;

		

		bool init;
		boost::signals2::connection listener_connection;
	public:
		void ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param);


		
	};

	template <class T>
	class WireServer : public virtual WireServerBase, public virtual Wire<T>
	{

	public:

		virtual ~WireServer() {}

		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback()
		{
			return callback;
		}
		
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function)
		{
			callback=function;
		}
		
		virtual void AsyncConnect(boost::function<void (RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			throw std::runtime_error("Not valid for server");
		}

		virtual RR_SHARED_PTR<WireConnection<T> > Connect()
		{
			throw std::runtime_error("Not valid for server");
		}
		

		WireServer(std::string name, RR_SHARED_PTR<ServiceSkel> skel) : WireServerBase(name,skel) 
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
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t e)
		{
			return RR_MAKE_SHARED<WireConnection<T> >(rr_cast<WireServer<T> >(shared_from_this()),e);
		}

		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> callback;

		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)
		{
			if (!callback) return;
			callback(rr_cast<WireConnection<T> >(e));
		}

	public:
		virtual void Shutdown()
		{
			WireServerBase::Shutdown();
			callback.clear();
		}

	};

	template <class T>
	class WireBroadcaster : public RR_ENABLE_SHARED_FROM_THIS<WireBroadcaster<T> >
	{
	protected:

		class connected_connection
		{
		public:

			connected_connection(RR_SHARED_PTR<WireConnection<T> > connection)
			{
				this->connection=connection;
			}

			RR_WEAK_PTR<WireConnection<T> > connection;

		};

		std::list<RR_SHARED_PTR<connected_connection> > connected_wires;
		boost::mutex connected_wires_lock;
		RR_WEAK_PTR<Wire<T> > wire;

		


		void ConnectionClosed(RR_SHARED_PTR<connected_connection> ep)
		{
			boost::mutex::scoped_lock lock(connected_wires_lock);
			try
			{
				connected_wires.remove(ep);
			}
			catch (std::exception&) {}
		}

		void ConnectionConnected(RR_SHARED_PTR<WireConnection<T> > ep)
		{
			boost::mutex::scoped_lock lock(connected_wires_lock);

			RR_SHARED_PTR<connected_connection> c=RR_MAKE_SHARED<connected_connection>(ep);

			ep->SetWireConnectionClosedCallback(boost::bind(&WireBroadcaster<T>::ConnectionClosed,this->shared_from_this(),c));
			
			connected_wires.push_back(c);
		}

	public:

		WireBroadcaster() {}

		void Init(RR_SHARED_PTR<Wire<T> > wire)
		{
			this->wire=wire;

			wire->SetWireConnectCallback(boost::bind(&WireBroadcaster<T>::ConnectionConnected,this->shared_from_this(),_1));
		}

		void SetOutValue(T value)
		{
			boost::mutex::scoped_lock lock(connected_wires_lock);

			for (typename std::list<RR_SHARED_PTR<connected_connection> >::iterator ee=connected_wires.begin(); ee!=connected_wires.end(); )
			{
				try
				{
					RR_SHARED_PTR<WireConnection<T> > c=(*ee)->connection.lock();
					if (!c)
					{
						ee=connected_wires.erase(ee);
						continue;
					}
					else
					{
						ee++;
					}

					c->SetOutValue(value);
				}
				catch (std::exception&) {}
			}

		}


	};




}

#pragma warning(pop)
