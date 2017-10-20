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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include <istream>
#include <boost/signals2.hpp>


namespace RobotRaconteur
{


	

	class StructureStub 
	{
	public:
		virtual RR_SHARED_PTR<MessageElementStructure> PackStructure(RR_SHARED_PTR<RRObject> s) = 0;


		
		virtual RR_SHARED_PTR<RRStructure> UnpackStructure(RR_SHARED_PTR<MessageElementStructure> m)=0;

		StructureStub(RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode() { return GetNode();}

		virtual ~StructureStub() {}
	protected:

		RR_WEAK_PTR<RobotRaconteurNode> node;

	};


	class ServerEndpoint;
	class AuthenticatedUser;
	class ClientContext;
	class ServerContext;
	class ServerContext_ObjectLock;
	class ServerContext_MonitorObjectSkel;
	
	namespace detail
	{
		std::string encode_index(const std::string& index);

		std::string decode_index(const std::string& index);

	}



	class ServiceStub : public virtual RRObject, public RR_ENABLE_SHARED_FROM_THIS<ServiceStub>
	{


	public:
		ServiceStub(const std::string &path, RR_SHARED_PTR<ClientContext> c);

		std::string ServicePath;

		RR_WEAK_PTR<ClientContext> context;

		RR_SHARED_PTR<boost::recursive_mutex> RRMutex;

		RR_SHARED_PTR<ClientContext> GetContext();

		RR_SHARED_PTR<MessageEntry> ProcessTransaction(RR_SHARED_PTR<MessageEntry> m);

		void AsyncProcessTransaction(RR_SHARED_PTR<MessageEntry> m, boost::function<void ( RR_SHARED_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException> )> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		virtual void DispatchEvent(RR_SHARED_PTR<MessageEntry> m) = 0;

		RR_SHARED_PTR<RRObject> FindObjRef(const std::string &n);

		void AsyncFindObjRef(const std::string &n, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		RR_SHARED_PTR<RRObject> FindObjRef(const std::string &n, const std::string &i);

		RR_SHARED_PTR<RRObject> FindObjRefTyped(const std::string &n, const std::string& objecttype);

		RR_SHARED_PTR<RRObject> FindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype);

		void AsyncFindObjRef(const std::string &n, const std::string &i, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjRefTyped(const std::string &n, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjRefTyped(const std::string &n, const std::string &i, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		template <typename T>
		static void EndAsyncFindObjRef(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<T>,RR_SHARED_PTR<RobotRaconteurException>)> handler);		

		std::string FindObjectType(const std::string &n);

		std::string FindObjectType(const std::string &n, const std::string &i);
		
		void AsyncFindObjectType(const std::string &n, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjectType(const std::string &n, const std::string &i, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncSendPipeMessage(RR_SHARED_PTR<MessageEntry> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > handler);

		void SendWireMessage(RR_SHARED_PTR<MessageEntry> m);

		virtual void DispatchPipeMessage(RR_SHARED_PTR<MessageEntry> m);

		virtual void DispatchWireMessage(RR_SHARED_PTR<MessageEntry> m);

		virtual RR_SHARED_PTR<MessageEntry> CallbackCall(RR_SHARED_PTR<MessageEntry> m);

		virtual std::string RRType() {return "RobotRaconteur.ServiceStub";}

		virtual void RRClose();
		
		virtual void RRInitStub()=0;
	
		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode();
	protected:
		RR_WEAK_PTR<RobotRaconteurNode> node;
	};

	

	class IRobotRaconteurMonitorObject;

	class ServiceSkel : public RR_ENABLE_SHARED_FROM_THIS<ServiceSkel>
	{

	public:

		virtual ~ServiceSkel() {}

		virtual void Init(const std::string &s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c);

		virtual void InitCallbackServers(RR_SHARED_PTR<RRObject> o);



		std::string GetServicePath() const;
	protected:
		std::string m_ServicePath;

	public:
		RR_SHARED_PTR<ServerContext> GetContext() ;
	protected:
		RR_WEAK_PTR<ServerContext> m_context;

		RR_SHARED_PTR<RRObject> uncastobj;

	public:
		RR_SHARED_PTR<RRObject> GetUncastObject() const;

		virtual RR_SHARED_PTR<MessageEntry> CallGetProperty(RR_SHARED_PTR<MessageEntry> m) = 0;


		virtual RR_SHARED_PTR<MessageEntry> CallSetProperty(RR_SHARED_PTR<MessageEntry> m) = 0;

		virtual RR_SHARED_PTR<MessageEntry> CallFunction(RR_SHARED_PTR<MessageEntry> m) = 0;

	protected:

		static void EndAsyncCallGetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<MessageElement> value, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

		static void EndAsyncCallSetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

		static void EndAsyncCallFunction(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<MessageElement> ret, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

	public:

		virtual RR_SHARED_PTR<RRObject> GetSubObj(const std::string &name, const std::string &ind) = 0;

		RR_SHARED_PTR<RRObject> GetSubObj(const std::string &name);

		virtual void RegisterEvents(RR_SHARED_PTR<RRObject> obj1);

		virtual void UnregisterEvents(RR_SHARED_PTR<RRObject> obj1);

		virtual void InitPipeServers(RR_SHARED_PTR<RRObject> obj1);

		virtual void InitWireServers(RR_SHARED_PTR<RRObject> obj1);

		void ObjRefChanged(const std::string &name);

		void SendEvent(RR_SHARED_PTR<MessageEntry> m);

		void ReleaseObject();

		virtual void ReleaseCastObject() = 0;

		void AsyncSendPipeMessage(RR_SHARED_PTR<MessageEntry> m, uint32_t e,  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > handler);

		void SendWireMessage(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		virtual void DispatchPipeMessage(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		virtual void DispatchWireMessage(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		
		virtual RR_SHARED_PTR<MessageEntry> CallPipeFunction(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		virtual RR_SHARED_PTR<MessageEntry> CallWireFunction(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		virtual RR_SHARED_PTR<void> GetCallbackFunction(uint32_t endpoint, const std::string& membername);

		virtual RR_SHARED_PTR<MessageEntry> CallMemoryFunction(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

		RR_WEAK_PTR<ServerContext_ObjectLock> objectlock;
		boost::mutex objectlock_lock;

		RR_SHARED_PTR<ServerContext_MonitorObjectSkel> monitorlock;

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> > monitorlocks;
		boost::mutex monitorlocks_lock;

		 bool IsLocked() ;

		 bool IsMonitorLocked() ;

		 virtual std::string GetObjectType() = 0;

		 RR_SHARED_PTR<RobotRaconteurNode> RRGetNode();

	protected:
		RR_WEAK_PTR<RobotRaconteurNode> node;

	private:
		void InitializeInstanceFields();
	};

	


	


	class ClientContext : public Endpoint, public RR_ENABLE_SHARED_FROM_THIS<ClientContext>
	{
	protected:
		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceStub> > stubs;
		boost::mutex stubs_lock;
		std::list<std::string> active_stub_searches;
		std::list<boost::tuple<std::string, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> > > active_stub_searches_handlers;

	   // public delegate void EventDispatchMessageEntry(MessageEntry e);

		RR_SHARED_PTR<ServiceFactory> m_ServiceDef;

	public:
		RR_SHARED_PTR<ServiceFactory> GetServiceDef() const;

		ClientContext(RR_SHARED_PTR<RobotRaconteurNode> node);

		ClientContext(RR_SHARED_PTR<ServiceFactory> service_def, RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RRObject> FindObjRef(const std::string &path, const std::string& objecttype="");

		void AsyncFindObjRef(const std::string &path, const std::string& objecttype2, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


		template <typename T>
		static void EndAsyncFindObjRef(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<T>,RR_SHARED_PTR<RobotRaconteurException>)> handler)
		{
			if (err)
			{
				handler(RR_SHARED_PTR<T>(),err);
				return;
			}
			else
			{
			
				try
				{
					RR_SHARED_PTR<T> castobj=rr_cast<T>(obj);
					try
					{
						handler(castobj,RR_SHARED_PTR<RobotRaconteurException>());
					}
					catch (std::exception& exp)
					{
						try
						{
							rr_cast<ServiceStub>(obj)->RRGetNode()->HandleException(&exp);
						}
						catch (std::exception& e) {}
					}
				}
				catch (RobotRaconteurException& err2)
				{
					handler(RR_SHARED_PTR<T>(),RobotRaconteurExceptionUtil::DownCastException(err2));	
				}
				catch (std::exception& err2)
				{
					handler(RR_SHARED_PTR<T>(),RR_MAKE_SHARED<ServiceException>(err2.what()));
				}
			}

		}

	protected:
		void AsyncFindObjRef1(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, std::string path, std::string objecttype2, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		void AsyncFindObjRef2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > ret, RR_SHARED_PTR<RobotRaconteurException> err, std::string objecttype, std::string objectdef, std::string path, std::string objecttype2, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout);

		void AsyncFindObjRef3(RR_SHARED_PTR<RRObject> ret, RR_SHARED_PTR<RobotRaconteurException> err, std::string path, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler);

	public:

		std::string FindObjectType(const std::string &path);

		void AsyncFindObjectType(const std::string &path, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


		bool VerifyObjectImplements(const std::string& objecttype, const std::string& implementstype);

	protected:
		void AsyncFindObjectType1(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler);

	private:



		boost::mutex pulled_service_defs_lock;
		std::map<std::string,RR_SHARED_PTR<ServiceDefinition> > pulled_service_defs;

		
		bool VerifyObjectImplements2(const std::string& objecttype, const std::string& implementstype);

		struct outstanding_transaction
		{
			RR_SHARED_PTR<MessageEntry> ret;
			RR_SHARED_PTR<AutoResetEvent> evt;
			boost::function<void ( RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException>)> handler;
			RR_SHARED_PTR<Timer> timer;
		};
		
		boost::mutex outstanding_transactions_lock;
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_transaction> > outstanding_transactions;

		
		boost::mutex FindObjRef_lock;

	public:
		RR_SHARED_PTR<MessageEntry> ProcessTransaction(RR_SHARED_PTR<MessageEntry> m);


		//Process a transaction asynchronously.  Note that this will never timeout unless the service is closed.


		void AsyncProcessTransaction(RR_SHARED_PTR<MessageEntry> m, boost::function<void ( RR_SHARED_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>  )> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


		//std::map<std::string, RR_SHARED_PTR<RRObject> > MultipleMemberAction(const std::string &objectpath, std::vector<std::string> &propertyget, std::map<std::string, RR_SHARED_PTR<RRObject> > &propertyset, std::map<std::string, std::map<std::string, RR_SHARED_PTR<RRObject> > > &functioncall);



		void SendMessage(RR_SHARED_PTR<MessageEntry> m);

		void AsyncSendMessage(RR_SHARED_PTR<MessageEntry> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);

	private:
		
		void AsyncProcessTransaction_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t transactionid);

		void AsyncProcessTransaction_timeout(const TimerEvent& error, uint32_t transactionid);

		uint32_t transaction_number;
		//private AutoResetEvent rec_event;

		RR_SHARED_PTR<Timer> connection_test_timer;

		void connection_test(const TimerEvent& ev);

	public:
		virtual void MessageReceived(RR_SHARED_PTR<Message> m);

	protected:
		void MessageEntryReceived(RR_SHARED_PTR<MessageEntry> m);

	/*public:
		RR_SHARED_PTR<MessageElementStructure> PackStructure(RR_SHARED_PTR<RRObject> s);

		template<typename T>
		T UnpackStructure(RR_SHARED_PTR<MessageElementStructure> l);
		*/
	protected:
		std::string m_ServiceName;
	public:
		std::string GetServiceName() const;

		bool GetConnected() const;
	protected:
		bool m_Connected;
		boost::mutex m_Connected_lock;
		boost::mutex close_lock;
		

	private:
		RR_SHARED_PTR<Transport> connecttransport;
		std::string connecturl;


	public:

		std::map<std::string, RR_SHARED_PTR<RRObject> > GetAttributes();
		

	private:
		std::map<std::string, RR_SHARED_PTR<RRObject> > m_Attributes;
		boost::mutex m_Attributes_lock;

	public:
		RR_SHARED_PTR<RRObject> ConnectService(RR_SHARED_PTR<Transport> c, const std::string &url, const std::string &username = "",  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials =( RR_SHARED_PTR<RRMap<std::string,RRObject> >()), const std::string& objecttype="");

		RR_SHARED_PTR<RRObject> ConnectService(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, const std::string &url, const std::string &username = "",  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials =( RR_SHARED_PTR<RRMap<std::string,RRObject> >()), const std::string& objecttype="");

		

	

		void AsyncConnectService(RR_SHARED_PTR<Transport> c, const std::string &url, const std::string &username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler,int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncConnectService(RR_SHARED_PTR<Transport> c, RR_SHARED_PTR<ITransportConnection> tc, const std::string &url, const std::string &username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler,int32_t timeout=RR_TIMEOUT_INFINITE);

	protected:

		//These functions are part of the connection process
		void AsyncConnectService1(RR_SHARED_PTR<Transport> c,RR_SHARED_PTR<ITransportConnection> tc, RR_SHARED_PTR<RobotRaconteurException> e, std::string url, std::string username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, std::string objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler);

		void AsyncConnectService2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, RR_SHARED_PTR<RobotRaconteurException> e, std::string username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, std::string objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler); 

		void AsyncConnectService3(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, std::string username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, std::string objecttype, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler); 

		void AsyncConnectService4(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, RR_SHARED_PTR<RobotRaconteurException> e, std::string username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, std::string objecttype, std::string type, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler); 

		void AsyncConnectService5(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> e, std::string username,  RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, std::string objecttype, std::string type, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler); 

		void AsyncConnectService6(RR_SHARED_PTR<std::string> ret, RR_SHARED_PTR<RobotRaconteurException> e, std::string type, RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > d, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler); 
		
		boost::recursive_mutex connect_lock;

	public:


		void Close();

		void AsyncClose(boost::function<void()> handler);
	protected:

		void AsyncClose1(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void()> handler);

	public:

		void AsyncSendPipeMessage(RR_SHARED_PTR<MessageEntry> m, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>) > handler);


		void SendWireMessage(RR_SHARED_PTR<MessageEntry> m);

		RR_SHARED_PTR<ServiceDefinition> PullServiceDefinition(const std::string &ServiceType = "");

		void AsyncPullServiceDefinition(const std::string &ServiceType, boost::function<void (RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);
	protected:
		
		void AsyncPullServiceDefinition1(RR_SHARED_PTR<MessageEntry> ret3, RR_SHARED_PTR<RobotRaconteurException> err, std::string ServiceType, boost::function<void (RR_SHARED_PTR<ServiceDefinition>, RR_SHARED_PTR<RobotRaconteurException>)>);

	public:

		std::vector<RR_SHARED_PTR<ServiceDefinition> > PullServiceDefinitionAndImports(const std::string &servicetype = "");

		void AsyncPullServiceDefinitionAndImports(const std::string &servicetype, boost::function<void (RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);
	protected:
		void AsyncPullServiceDefinitionAndImports1(RR_SHARED_PTR<ServiceDefinition> root,RR_SHARED_PTR<RobotRaconteurException> err, std::string servicetype,boost::function<void (RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)> handler, boost::posix_time::ptime timeout_time);
		void AsyncPullServiceDefinitionAndImports2(RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > imported,RR_SHARED_PTR<RobotRaconteurException> err, boost::tuple<std::string , RR_SHARED_PTR<ServiceDefinition> , RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > > , std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, boost::function<void (RR_SHARED_PTR<std::vector<RR_SHARED_PTR<ServiceDefinition> > >, RR_SHARED_PTR<RobotRaconteurException>)>, boost::posix_time::ptime> args);
	public:

		std::vector<std::string> GetPulledServiceTypes();

		RR_SHARED_PTR<ServiceFactory> GetPulledServiceType(std::string type);

	private:

		boost::mutex pulled_service_types_lock;
		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> > pulled_service_types;

		bool use_pulled_types;

	private:

		RR_SHARED_PTR<ITransportConnection> TransportConnection;

		boost::mutex m_Authentication_lock;

		bool m_UserAuthenticated;

	public:
		const bool GetUserAuthenticated() ;
	private:
		std::string m_AuthenticatedUsername;
	public:
		const std::string GetAuthenticatedUsername() ;

		std::string AuthenticateUser(const std::string &username,  RR_SHARED_PTR<RRObject> credentials);

		void AsyncAuthenticateUser(const std::string &username,  RR_SHARED_PTR<RRObject> credentials, boost::function<void(RR_SHARED_PTR<std::string> , RR_SHARED_PTR<RobotRaconteurException> )> handler , int32_t timeout=RR_TIMEOUT_INFINITE);

	protected:
		void AsyncAuthenticateUser2(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, std::string username, boost::function<void(RR_SHARED_PTR<std::string> , RR_SHARED_PTR<RobotRaconteurException> )> handler);
	public:

		std::string LogoutUser();

		std::string RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags);

		void AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);
	protected:
		void EndAsyncLockOp(RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler);
	public:
		std::string ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj);

		void AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		std::string MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout);

		void MonitorExit(RR_SHARED_PTR<RRObject> obj);


	

	public:
		virtual void PeriodicCleanupTask();

		uint32_t CheckServiceCapability(const std::string &name);

		

//		public event ClientServiceListenerDelegate ClientServiceListener;

		boost::signals2::signal<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> ClientServiceListener;

	protected:
		void ProcessCallbackCall(RR_SHARED_PTR<MessageEntry> m);

	private:
		void InitializeInstanceFields();
	};

	template <typename T>
	void ServiceStub::EndAsyncFindObjRef(RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<T>,RR_SHARED_PTR<RobotRaconteurException>)> handler)
	{
		ClientContext::EndAsyncFindObjRef(obj,err,handler);
	}


	

	class UserAuthenticator;
	

	class ServerContext_ObjectLock : public RR_ENABLE_SHARED_FROM_THIS<ServerContext_ObjectLock>
		{

		private:
			bool m_Locked;
			std::string m_Username;
			std::vector<RR_WEAK_PTR<ServiceSkel> > skels;
			RR_WEAK_PTR<ServiceSkel> m_RootSkel;
			uint32_t m_Endpoint;


			std::string m_RootServicePath;
			boost::mutex skels_lock;

		public:
			ServerContext_ObjectLock(const std::string &username, const RR_SHARED_PTR<ServiceSkel> &root_skel, uint32_t endpoint = 0);

			 std::string GetUsername() const;
			 bool IsLocked() const;
			 std::string GetRootServicePath() const;
			 uint32_t GetEndpoint() const;

			void AddSkel(RR_SHARED_PTR<ServiceSkel> skel);

			void ReleaseSkel(RR_SHARED_PTR<ServiceSkel> skel);

			void ReleaseLock();



		private:
			void InitializeInstanceFields();
		};

	class ServerContext_MonitorObjectSkel : public RR_ENABLE_SHARED_FROM_THIS<ServerContext_MonitorObjectSkel>
		{
		private:
			//RR_SHARED_PTR<boost::thread> wait_thread;
			RR_SHARED_PTR<AutoResetEvent> wait_event;
			bool wait_started;
			RR_WEAK_PTR<IRobotRaconteurMonitorObject> obj;
			uint32_t local_endpoint;
			int32_t timeout;
			RR_SHARED_PTR<std::exception> monitor_acquire_exception;
			bool monitor_acquired;
			RR_SHARED_PTR<AutoResetEvent> monitor_thread_event;
			bool maintain_lock;
			RR_WEAK_PTR<ServiceSkel> skel;
			//boost::mutex monitorlocks_lock;

		public:
			 uint32_t GetLocalEndpoint() const;

			 bool IsLocked() const;

			ServerContext_MonitorObjectSkel(RR_SHARED_PTR<ServiceSkel> skel);

			std::string MonitorEnter(uint32_t local_endpoint, int32_t timeout);

			std::string MonitorContinueEnter(uint32_t localendpoint);

			void MonitorRefresh(uint32_t localendpoint);

			std::string MonitorExit(uint32_t local_endpoint);

			void Shutdown();

		private:
			void thread_func();
			
			boost::mutex close_lock;

		private:
			void InitializeInstanceFields();
		};


	class ServerContext : public RR_ENABLE_SHARED_FROM_THIS<ServerContext>, boost::noncopyable
	{
	public:
		

	public:
		
	public:

		std::map<std::string, RR_SHARED_PTR<RRObject> > GetAttributes();
		void SetAttributes(const std::map<std::string, RR_SHARED_PTR<RRObject> >& attr);

	private:
		std::map<std::string, RR_SHARED_PTR<RRObject> > m_Attributes;
		boost::mutex m_Attributes_lock;
	public:
		RR_SHARED_PTR<ServiceFactory> GetServiceDef() const;

		virtual ~ServerContext() {}

	protected:
		RR_SHARED_PTR<ServiceFactory> m_ServiceDef;

	public:
		std::string GetServiceName() const;

	protected:
		std::string m_ServiceName;


		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceSkel> > skels;
		boost::mutex skels_lock;

		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> > client_endpoints;
		boost::mutex client_endpoints_lock;

	public:
		std::string GetRootObjectType() const;

	protected:
		std::string m_RootObjectType;


		RR_WEAK_PTR<RobotRaconteurNode> node;


		/*public Message SendRequest(Message m)
		{
		    return null;
		}*/

	public:
		ServerContext(RR_SHARED_PTR<ServiceFactory> f, RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual void SendEvent(RR_SHARED_PTR<MessageEntry> m);
#undef SendMessage
		virtual void SendMessage(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

		virtual void SendMessage(RR_SHARED_PTR<MessageEntry> m, uint32_t e);

		virtual void AsyncSendMessage(RR_SHARED_PTR<MessageEntry> m, uint32_t e, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);

		virtual void AsyncSendMessage(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);



	private:
		//boost::mutex rec_sync;



		bool base_object_set;

	protected:
		void SetSecurityPolicy(RR_SHARED_PTR<ServiceSecurityPolicy> policy);


	public:
		virtual void SetBaseObject(const std::string &name, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServiceSecurityPolicy> policy = RR_SHARED_PTR<ServiceSecurityPolicy>());

		virtual RR_SHARED_PTR<ServiceSkel> GetObjectSkel(const std::string &servicepath);


		virtual void ReplaceObject(const std::string &path);

		virtual std::string GetObjectType(const std::string &servicepath);

		static RR_SHARED_PTR<ServerContext> GetCurrentServerContext();
	private:

		static boost::thread_specific_ptr<RR_SHARED_PTR<ServerContext> > m_CurrentServerContext;

	public:
		static std::string GetCurrentServicePath() ;
	private:

		static boost::thread_specific_ptr<std::string> m_CurrentServicePath;


	public:
		virtual RR_SHARED_PTR<MessageEntry> ProcessMessageEntry(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> c);

		virtual void Close();

		virtual void MessageReceived(RR_SHARED_PTR<Message> m, RR_SHARED_PTR<ServerEndpoint> e);

		virtual void AddClient(RR_SHARED_PTR<ServerEndpoint> cendpoint);

		virtual void RemoveClient(RR_SHARED_PTR<ServerEndpoint> cendpoint);


		virtual void KickUser(const std::string& username);
		


	private:
		RR_SHARED_PTR<UserAuthenticator> user_authenticator;
		std::map<std::string, std::string> security_policies;

		bool RequireValidUser;
		bool AllowObjectLock;

		virtual RR_SHARED_PTR<MessageEntry> ClientSessionOp(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e);

	public:
		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, std::map<std::string, RR_SHARED_PTR<RRObject> > &credentials);



	private:
		boost::mutex ClientLockOp_lockobj;

		virtual void ClientLockOp(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<MessageEntry> ret);

	protected:
		void check_lock(RR_SHARED_PTR<ServiceSkel> skel);

		void check_monitor_lock(RR_SHARED_PTR<ServiceSkel> skel);

		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> > active_object_locks;

	public:
		virtual void PeriodicCleanupTask();

	protected:
		RR_SHARED_PTR<MessageEntry> CheckServiceCapability(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> c);

	public:
		boost::signals2::signal<void (RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>)> ServerServiceListener;

	public:
		void ReleaseServicePath(const std::string &path);

		void AsyncProcessCallbackTransaction(RR_SHARED_PTR<MessageEntry> m, uint32_t endpoint, boost::function<void ( RR_SHARED_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>  )> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

	private:
		
		void AsyncProcessCallbackTransaction_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t transactionid);

		void AsyncProcessCallbackTransaction_timeout(const TimerEvent&, uint32_t transactionid);

		uint32_t transaction_number;

		struct outstanding_transaction
		{
			RR_SHARED_PTR<MessageEntry> ret;
			RR_SHARED_PTR<AutoResetEvent> evt;
			boost::function<void ( RR_SHARED_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)> handler;
			RR_SHARED_PTR<Timer> timer;
		};
		
		boost::mutex outstanding_transactions_lock;
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_transaction> > outstanding_transactions;


	public:
		RR_SHARED_PTR<MessageEntry> ProcessCallbackTransaction(RR_SHARED_PTR<MessageEntry> m, uint32_t endpointid);
	
		int32_t GetMonitorThreadPoolCount();

		void SetMonitorThreadPoolCount(int32_t count);

		RR_SHARED_PTR<ThreadPool> GetMonitorThreadPool();

	protected:

		RR_SHARED_PTR<ThreadPool> monitor_thread_pool;
		boost::mutex monitor_thread_pool_lock;

	private:
		void InitializeInstanceFields();
	};

	


	class ServerEndpoint : public Endpoint, public RR_ENABLE_SHARED_FROM_THIS<ServerEndpoint>
	{
	public:
		RR_SHARED_PTR<ServerContext> service;

	private:

		static boost::thread_specific_ptr<RR_SHARED_PTR<ServerEndpoint> > m_CurrentEndpoint;

	public:
		static RR_SHARED_PTR<ServerEndpoint> GetCurrentEndpoint() ;

	private:

		static boost::thread_specific_ptr<RR_SHARED_PTR<AuthenticatedUser> > m_CurrentAuthenticatedUser;

	public:
		static RR_SHARED_PTR<AuthenticatedUser> GetCurrentAuthenticatedUser();

	private:
		RR_SHARED_PTR<AuthenticatedUser> endpoint_authenticated_user;

	public:
		const std::string GetAuthenticatedUsername() const;


		virtual void MessageReceived(RR_SHARED_PTR<Message> m);

		void AuthenticateUser(const std::string &username, std::map<std::string, RR_SHARED_PTR<RRObject> > &credentials);

		void LogoutUser();


		virtual void PeriodicCleanupTask();

		

	private:
		void InitializeInstanceFields();

public:
		ServerEndpoint(RR_SHARED_PTR<RobotRaconteurNode> node) : Endpoint(node)
		{
			InitializeInstanceFields();
		}
	};

	

	

	
	

	class ServiceSecurityPolicy
	{
	public:
		RR_SHARED_PTR<UserAuthenticator> Authenticator;
		std::map<std::string, std::string> Policies;

		ServiceSecurityPolicy();

		ServiceSecurityPolicy(RR_SHARED_PTR<UserAuthenticator> Authenticator, const std::map<std::string, std::string>& Policies);

	};



	class AuthenticatedUser
	{
	private:
		std::string m_Username;
		std::vector<std::string> m_Privileges;
		std::vector<std::string> m_Properties;
		boost::posix_time::ptime m_LoginTime;
		boost::posix_time::ptime m_LastAccessTime;

		boost::mutex m_LastAccessTime_lock;

		RR_WEAK_PTR<ServerContext> context;

	public:
		virtual std::string GetUsername() ;

		virtual std::vector<std::string> GetPrivileges();

		virtual std::vector<std::string> GetProperties();

		virtual boost::posix_time::ptime GetLoginTime() ;

		virtual boost::posix_time::ptime GetLastAccessTime() ;

		AuthenticatedUser(const std::string &username, const std::vector<std::string>& privileges, const std::vector<std::string>& properties, RR_SHARED_PTR<ServerContext> context);

		virtual void UpdateLastAccess();

	};


	class UserAuthenticator
	{
	public:
		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, const std::map<std::string, RR_SHARED_PTR<RRObject> > &credentials, RR_SHARED_PTR<ServerContext> context) = 0;

		virtual ~UserAuthenticator() {}

	};

	class PasswordFileUserAuthenticator : public UserAuthenticator
	{

	private:
		class User
		{
		public:
			std::string username;
			std::string passwordhash;
			std::vector<std::string> privileges;

		};

	private:
		std::map<std::string, RR_SHARED_PTR<User> > validusers;

	public:
		PasswordFileUserAuthenticator(std::istream &file);

		PasswordFileUserAuthenticator(const std::string &data);

		virtual ~PasswordFileUserAuthenticator() {}

	private:
		void load(const std::string &data);

	public:
		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, const std::map<std::string, RR_SHARED_PTR<RRObject> > &credentials, RR_SHARED_PTR<ServerContext> context);


		static std::string MD5Hash(const std::string &text);


	private:
		void InitializeInstanceFields();
	};

	class IRobotRaconteurMonitorObject
	{
	 public:
		 virtual void RobotRaconteurMonitorEnter() = 0;

		 virtual void RobotRaconteurMonitorEnter(int32_t timeout) = 0;

		 virtual void RobotRaconteurMonitorExit() = 0;

		 virtual ~IRobotRaconteurMonitorObject() {}

	};


}
