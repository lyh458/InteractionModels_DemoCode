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

//#define ROBOTRACONTEUR_USE_ASIO_SPAWN

#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/NodeID.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/Transport.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/AsyncUtils.h"
#include "RobotRaconteur/ThreadPool.h"
#include "RobotRaconteur/Timer.h"
#include <queue>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/bind/protect.hpp>
#include <boost/random.hpp>

#ifdef BOOST_WINDOWS

#undef SendMessage

#endif



namespace RobotRaconteurServiceIndex
{
	class ServiceInfo;

	class NodeInfo;
}

namespace RobotRaconteur
{

	


	class ServiceInfo2
	{
	public:

		ServiceInfo2();

		std::string Name;
		std::string RootObjectType;
		std::vector<std::string> RootObjectImplements;
		std::vector<std::string> ConnectionURL;
		std::map<std::string,RR_SHARED_PTR<RRObject> > Attributes;
		RobotRaconteur::NodeID NodeID;
		std::string NodeName;

		ServiceInfo2(const RobotRaconteurServiceIndex::ServiceInfo &info, const RobotRaconteurServiceIndex::NodeInfo &ninfo);

	};

	class NodeInfo2
	{
	public:		
		RobotRaconteur::NodeID NodeID;
		std::string NodeName;
		std::vector<std::string> ConnectionURL;
	};

	enum RobotRaconteurObjectLockFlags
	{
		RobotRaconteurObjectLockFlags_USER_LOCK = 0,
		RobotRaconteurObjectLockFlags_CLIENT_LOCK
	};


	class ServiceSecurityPolicy;
	



	class NodeDiscoveryInfoURL
	{
	public:
		std::string URL;
		boost::posix_time::ptime LastAnnounceTime;
	};

	
	class NodeDiscoveryInfo
	{
	public:

		RobotRaconteur::NodeID NodeID;
		std::string NodeName;
		std::vector<NodeDiscoveryInfoURL> URLs;
	
	};


	
	
	
	class RobotRaconteurNode : boost::noncopyable, public RR_ENABLE_SHARED_FROM_THIS<RobotRaconteurNode>
	{
	private:
		
		bool is_shutdown;
		boost::mutex shutdown_lock;

		

		static bool is_init;

		bool instance_is_init;
		boost::mutex init_lock;

	public:

		void Init();

		RobotRaconteurNode();

		virtual ~RobotRaconteurNode();
		

		static RobotRaconteurNode* s();

		static RR_SHARED_PTR<RobotRaconteurNode> sp();

		RobotRaconteur::NodeID NodeID();

		std::string NodeName();

		void SetNodeID(const RobotRaconteur::NodeID& id);

		void SetNodeName(const std::string& name);

		RR_SHARED_PTR<ServiceFactory> GetServiceType(const std::string& servicename);
		
		bool IsServiceTypeRegistered(const std::string& servicename);

		void RegisterServiceType(RR_SHARED_PTR<ServiceFactory> factory);


		void UnregisterServiceType(const std::string& type);


		std::vector<std::string> GetRegisteredServiceTypes();

		RR_SHARED_PTR<MessageElementStructure> PackStructure(RR_SHARED_PTR<RRStructure> structure);

		RR_SHARED_PTR<RRStructure> UnpackStructure(RR_SHARED_PTR<MessageElementStructure> structure);
	
		template <class T>
		RR_SHARED_PTR<MessageElementMultiDimArray> PackMultiDimArray(RR_SHARED_PTR<RRMultiDimArray<T> > arr)
		{
			if (!arr) return RR_SHARED_PTR<MessageElementMultiDimArray>();

			std::vector<RR_SHARED_PTR<MessageElement> > ar;
			ar.push_back(RR_SHARED_PTR<MessageElement>(new MessageElement("dimcount",ScalarToRRArray(arr->DimCount))));
			ar.push_back(RR_SHARED_PTR<MessageElement>(new MessageElement("dims",arr->Dims)));
			ar.push_back(RR_SHARED_PTR<MessageElement>(new MessageElement("real",arr->Real)));
			if (arr->Complex)
			{
				ar.push_back(RR_SHARED_PTR<MessageElement>(new MessageElement("imag",arr->Imag)));
			}

			return RR_MAKE_SHARED<MessageElementMultiDimArray>(ar);
		}

		template <class T>
		RR_SHARED_PTR<RRMultiDimArray<T> > UnpackMultiDimArray(RR_SHARED_PTR<MessageElementMultiDimArray> ar)
		{
			if (!ar) return RR_SHARED_PTR<RRMultiDimArray<T> >();

			RR_SHARED_PTR<RRMultiDimArray<T> > arr=RR_MAKE_SHARED<RRMultiDimArray<T> >();
			arr->Complex=false;
			arr->DimCount=RRArrayToScalar(MessageElement::FindElement(ar->Elements,"dimcount")->CastData<RRArray<int32_t> >());
			arr->Dims=MessageElement::FindElement(ar->Elements,"dims")->CastData<RRArray<int32_t> >();
			arr->Real=MessageElement::FindElement(ar->Elements,"real")->CastData<RRArray<T> >();
			if (MessageElement::ContainsElement(ar->Elements,"imag"))
			{
				arr->Complex=true;
				arr->Imag=MessageElement::FindElement(ar->Elements,"imag")->CastData<RRArray<T> >();
			}


			return arr;
		}

	
		RR_SHARED_PTR<MessageElementData> PackVarType(RR_SHARED_PTR<RRObject> vardata);

		RR_SHARED_PTR<RRObject> UnpackVarType(RR_SHARED_PTR<MessageElement> mvardata);
	
		template<class K, class T>
		RR_SHARED_PTR<MessageElementMap<K> > PackMapType(RR_SHARED_PTR<RRObject> set)
		{
			if (!set) return RR_SHARED_PTR<MessageElementMap<K> >();

			int indextype;

			if (typeid(K)==typeid(int32_t))
			{
				indextype=0;
			}
			else if (typeid(K)==typeid(std::string))
			{
				indextype=1;
			}
#ifdef ROBOTRACONTEUR_USEWSTRING
			else if (typeid(K)==typeid(std::wstring))
			{
				indextype=2;
			}
#endif
			else
			{
				//Unknown keytype type
				throw DataTypeException("Invalid object keytype");
			}


			RR_SHARED_PTR<RRMap<K,T> > set2=rr_cast<RRMap<K,T> >(set);
			

			std::vector<RR_SHARED_PTR<MessageElement> > mret;

			typedef typename std::map<K,RR_SHARED_PTR<T> >::iterator iter1;
			iter1 iter=set2->map.begin();
			for (; iter!=set2->map.end(); ++iter)
			{
				std::string key;
				switch (indextype)
				{
				case 0:
					{
					//std::stringstream conv; conv << iter->first; key=conv.str();
					key=boost::lexical_cast<std::string>(iter->first);
					}
					break;
				case 1:
					key=iter->first;
					break;
#ifdef ROBOTRACONTEUR_USEWSTRING
				case 2:
					{
					void* key2=(void*)&(iter->first);
					key=utf8_encode(*((std::wstring*)key2));
					}
					break;
#endif

				}
			

				RR_SHARED_PTR<MessageElementData> dat=PackVarType(rr_cast<RRObject>(iter->second));

				MessageElement* m=new MessageElement(key,dat);
				mret.push_back(RR_SHARED_PTR<MessageElement>(m));
			}
			return RR_MAKE_SHARED<MessageElementMap<K> >(mret);

		
		}

		template<class K, class T>
		RR_SHARED_PTR<RRObject> UnpackMapType(RR_SHARED_PTR<MessageElementMap<K> > mset)
		{
			if (!mset) return RR_SHARED_PTR<RRObject>();

			int indextype;

			if (typeid(K)==typeid(int32_t))
			{
				indextype=0;
			}
			else if (typeid(K)==typeid(std::string))
			{
				indextype=1;
			}
			else if (typeid(K)==typeid(std::wstring))
			{
				indextype=2;
			}
			else
			{
				//Unknown keytype type
				throw DataTypeException("Invalid set keytype");
			}

			RRMap<K,T>* pret=new RRMap<K,T>();
			RR_SHARED_PTR<RRMap<K,T> > ret(pret);

			for (size_t i=0; i<mset->Elements.size(); i++)
			{
				std::string skey=mset->Elements.at(i)->ElementName;

				K key;
				switch(indextype)
				{
				case 0:
					{
						//std::istringstream s(skey);
						//s > > key;
						key=boost::lexical_cast<int32_t>(skey);
					}
					break;
				case 1:
					{
						void* p=(void*)&skey;
						key=*(K*)p;
					}
					break;
#ifdef ROBOTRACONTEUR_USEWSTRING
				case 2:
					{

						std::wstring decode=utf8_decode(skey);

						void* p=(void*)&decode;
						key=*(K*)p;
					}
					break;
#endif
				}


				RR_SHARED_PTR<T> dat=rr_cast<T>(UnpackVarType(mset->Elements.at(i)));
				ret->map.insert(std::make_pair(key,dat));
		

			}


			return rr_cast<RRObject>(ret);
		}


		template<class T>
		RR_SHARED_PTR<MessageElementList > PackListType(RR_SHARED_PTR<RRObject> set)
		{
			if (!set) return RR_SHARED_PTR<MessageElementList >();

			

			RR_SHARED_PTR<RRList<T> > set2=rr_cast<RRList<T> >(set);
			

			std::vector<RR_SHARED_PTR<MessageElement> > mret;

			
			for (uint32_t i=0; i <set2->list.size(); i++)
			{
				std::string key=boost::lexical_cast<std::string>(i);			

				RR_SHARED_PTR<MessageElementData> dat=PackVarType(rr_cast<RRObject>(set2->list.at(i)));

				MessageElement* m=new MessageElement(key,dat);
				mret.push_back(RR_SHARED_PTR<MessageElement>(m));
			}
			return RR_MAKE_SHARED<MessageElementList >(mret);

		
		}

		template<class T>
		RR_SHARED_PTR<RRObject> UnpackListType(RR_SHARED_PTR<MessageElementList > mset)
		{
			if (!mset) return RR_SHARED_PTR<RRObject>();

			
			
			RR_SHARED_PTR<RRList<T> > ret=RR_MAKE_SHARED<RRList<T> >();

			for (int32_t i=0; i<(int32_t)mset->Elements.size(); i++)
			{
				std::string skey=mset->Elements.at(i)->ElementName;

				uint32_t key=boost::lexical_cast<uint32_t>(skey);
				
				if (key!=i) throw DataTypeException("Invalid list format");


				RR_SHARED_PTR<T> dat=rr_cast<T>(UnpackVarType(mset->Elements.at(i)));
				ret->list.push_back(dat);
		

			}


			return rr_cast<RRObject>(ret);
		}


	private:
		template<class T> 
		class PackAnyTypeSupport
		{
		public:
			template<typename NodeType>
			static RR_SHARED_PTR<MessageElementData> PackAnyType(RR_SHARED_PTR<RRObject> data, NodeType node)
			{
				return node->PackVarType(data);
			}

			template<typename NodeType>
			static RR_SHARED_PTR<RRObject> UnpackAnyType(RR_SHARED_PTR<MessageElement> mdata, NodeType node)
			{
				return node->UnpackVarType(mdata);
			}
		};
		
		template<typename K, typename T>
		class PackAnyTypeSupport<RR_SHARED_PTR<RRMap<K,T> > >
		{
		public:
			template<typename NodeType>
			static RR_SHARED_PTR<MessageElementData> PackAnyType(RR_SHARED_PTR<RRObject> data, NodeType node)
			{
				return node->template PackMapType<K,T>(data);
			}

			template<typename NodeType>
			static RR_SHARED_PTR<RRObject> UnpackAnyType(RR_SHARED_PTR<MessageElement> mdata, NodeType node)
			{
				return node->template UnpackMapType<K,T>(mdata->CastData<MessageElementMap<K> >());
			}
		};
		
		template<typename T>
		class PackAnyTypeSupport<RR_SHARED_PTR<RRList<T> > >
		{
		public:
			template<typename NodeType>
			static RR_SHARED_PTR<MessageElementData> PackAnyType(RR_SHARED_PTR<RRObject> data, NodeType node)
			{
				return node->template PackListType<T>(data);
			}

			template<typename NodeType>
			static RR_SHARED_PTR<RRObject> UnpackAnyType(RR_SHARED_PTR<MessageElement> mdata, NodeType node)
			{
				return node->template UnpackListType<T>(mdata->CastData<MessageElementList >());
			}
		};

		
	public:
		
		template<class T> RR_SHARED_PTR<MessageElementData> PackAnyType(RR_SHARED_PTR<RRObject> data)
		{
			return PackAnyTypeSupport<T>::PackAnyType(data,this);
		}

		template<class T> RR_SHARED_PTR<RRObject> UnpackAnyType(RR_SHARED_PTR<MessageElement> mdata)
		{
			return PackAnyTypeSupport<T>::UnpackAnyType(mdata,this);
		}
		

		virtual void Shutdown();
		
		


		uint32_t RegisterTransport(RR_SHARED_PTR<Transport> transport);

		void AsyncSendMessage(RR_SHARED_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )> callback);

		void SendMessage(RR_SHARED_PTR<Message> m);

		void MessageReceived(RR_SHARED_PTR<Message> m);
	private:
		uint32_t TransactionTimeout;
		boost::mutex TransactionTimeout_lock;

		uint32_t TransportInactivityTimeout;
		boost::mutex TransportInactivityTimeout_lock;

		uint32_t EndpointInactivityTimeout;
		boost::mutex EndpointInactivityTimeout_lock;
		
		uint32_t MemoryMaxTransferSize;
		boost::mutex MemoryMaxTransferSize_lock;
		


	public:

		uint32_t GetTransactionTimeout();
		void SetTransactionTimeout(uint32_t timeout);

		uint32_t GetTransportInactivityTimeout();
		void SetTransportInactivityTimeout(uint32_t timeout);

		uint32_t GetEndpointInactivityTimeout();
		void SetEndpointInactivityTimeout(uint32_t timeout);

		uint32_t GetMemoryMaxTransferSize();
		void SetMemoryMaxTransferSize(uint32_t size);

	protected:
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> > endpoints;
		std::map<uint32_t,boost::posix_time::ptime> recent_endpoints;
		
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> > transports;
		boost::mutex transports_lock;
	
		RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> dynamic_factory;
		boost::mutex dynamic_factory_lock;
	public:

		const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> GetDynamicServiceFactory() ;


		void SetDynamicServiceFactory(RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> f);

		RR_SHARED_PTR<Message> GenerateErrorReturnMessage(RR_SHARED_PTR<Message> m, MessageErrorType err, const std::string &errname, const std::string &errdesc);



		RR_SHARED_PTR<ServerContext> RegisterService(const std::string &name, const std::string &servicetype, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<ServiceSecurityPolicy> securitypolicy = RR_SHARED_PTR<ServiceSecurityPolicy>());


		RR_SHARED_PTR<ServerContext> RegisterService(RR_SHARED_PTR<ServerContext> c);

		void CloseService(const std::string &sname);


		std::vector<std::string> GetRegisteredServiceNames();

		RR_SHARED_PTR<ServerContext> GetService(const std::string &name);

		public:
			

		RR_SHARED_PTR<Message> SpecialRequest(RR_SHARED_PTR<Message> m, uint32_t transportid);

		RR_SHARED_PTR<RRObject> ConnectService(const std::string &url, const std::string &username = "", RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials=(RR_SHARED_PTR<RRMap<std::string,RRObject> >()), boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener = 0, const std::string& objecttype="");

		void AsyncConnectService(const std::string &url, const std::string &username, RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		RR_SHARED_PTR<RRObject> AsyncConnectService(const std::string &url, const std::string &username, RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::asio::yield_context& yield, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			return detail::async_wrap_for_spawn<RR_SHARED_PTR<RRObject> >(boost::bind((void (RobotRaconteurNode::*)(const std::string&, const std::string&, RR_SHARED_PTR<RRMap<std::string,RRObject> >, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)>, const std::string&, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)>,int32_t))&RobotRaconteurNode::AsyncConnectService,this,boost::ref(url),boost::ref(username),boost::ref(credentials),boost::ref(listener),boost::ref(objecttype),_1,timeout),yield);
		}
#endif

		RR_SHARED_PTR<RRObject> ConnectService(const std::vector<std::string>& urls, const std::string &username = "", RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials=(RR_SHARED_PTR<RRMap<std::string,RRObject> >()), boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener = 0, const std::string& objecttype="");

		void AsyncConnectService(const std::vector<std::string> &url, const std::string &username, RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		RR_SHARED_PTR<RRObject> AsyncConnectService(const std::vector<std::string> &url, const std::string &username, RR_SHARED_PTR<RRMap<std::string,RRObject> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::asio::yield_context& yield, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			return detail::async_wrap_for_spawn<RR_SHARED_PTR<RRObject> >(boost::bind((void (RobotRaconteurNode::*)(const std::vector<std::string>&, const std::string&, RR_SHARED_PTR<RRMap<std::string,RRObject> >, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)>, const std::string&, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)>,int32_t))&RobotRaconteurNode::AsyncConnectService,this,boost::ref(url),boost::ref(username),boost::ref(credentials),boost::ref(listener),boost::ref(objecttype),_1,timeout),yield);
		}
#endif

		void DisconnectService(RR_SHARED_PTR<RRObject> obj);

		void AsyncDisconnectService(RR_SHARED_PTR<RRObject> obj, boost::function<void()> handler);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		void AsyncDisconnectService(RR_SHARED_PTR<RRObject> obj, boost::asio::yield_context& yield)
		{
			detail::async_wrap_for_spawn_void(boost::bind((void (RobotRaconteurNode::*)(RR_SHARED_PTR<RRObject>,boost::function<void()>))&RobotRaconteurNode::AsyncDisconnectService,this,boost::ref(obj),_1),yield);
		}
#endif

		std::map<std::string, RR_SHARED_PTR<RRObject> > GetServiceAttributes(RR_SHARED_PTR<RRObject> obj);

		uint32_t RegisterEndpoint(RR_SHARED_PTR<Endpoint> e);

		void DeleteEndpoint(RR_SHARED_PTR<Endpoint> e);

		void CheckConnection(uint32_t endpoint);



	private:

		
		uint32_t transport_count;
		

		boost::shared_mutex transport_lock;
		boost::mutex endpoint_lock;

		static RobotRaconteurNode m_s;
		static RR_SHARED_PTR<RobotRaconteurNode> m_sp;

		RobotRaconteur::NodeID m_NodeID;
		std::string m_NodeName;

		bool NodeID_set;
		bool NodeName_set;

		RR_UNORDERED_MAP<std::string,RR_SHARED_PTR<ServiceFactory> > service_factories;
		boost::shared_mutex service_factories_lock;
		boost::mutex id_lock;

		RR_UNORDERED_MAP<std::string,RR_SHARED_PTR<ServerContext> > services;
		boost::shared_mutex services_lock;

	
		std::map<std::string, RR_SHARED_PTR<NodeDiscoveryInfo> > m_DiscoveredNodes;
		boost::mutex m_DiscoveredNodes_lock;

	public:
		std::vector<NodeDiscoveryInfo> GetDetectedNodes();

		void NodeAnnouncePacketReceived(std::string packet);

		void NodeDetected(const NodeDiscoveryInfo& info);

		void UpdateDetectedNodes();

		void AsyncUpdateDetectedNodes(boost::function<void()> handler, int32_t timeout=5000);

	protected:
		void CleanDiscoveredNodes();


	public:
		static std::string SelectRemoteNodeURL(const std::vector<std::string>& urls);
			

		std::vector<ServiceInfo2> FindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes);

		void AsyncFindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) > handler, int32_t timeout=5000);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		/*std::vector<ServiceInfo2> AsyncFindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes, boost::asio::yield_context& yield, int32_t timeout=5000)
		{
			return *detail::async_wrap_for_spawn<RR_SHARED_PTR<std::vector<ServiceInfo2> > >(boost::bind((void (RobotRaconteurNode::*)(const std::string&, const std::vector<std::string>&, boost::function< void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) >, int32_t))&RobotRaconteurNode::FindServiceByType,this,boost::ref(servicetype),boost::ref(transportschemes),_1,timeout),yield);
		}*/
#endif

		std::vector<NodeInfo2> FindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes);

		void AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout=5000);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		/*std::vector<NodeInfo2> AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes, boost::asio::yield_context& yield, int32_t timeout=5000)
		{
			return *detail::async_wrap_for_spawn<RR_SHARED_PTR<std::vector<NodeInfo2> > >(boost::bind((void (RobotRaconteurNode::*)(const RobotRaconteur::NodeID&, const std::vector<std::string>&, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) >, int32_t))&RobotRaconteurNode::FindNodeByID,this,boost::ref(id),boost::ref(transportschemes),_1,timeout),yield);
		}*/
#endif

		std::vector<NodeInfo2> FindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes);

		void AsyncFindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout=5000);

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
		/*std::vector<NodeInfo2> AsyncFindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes, boost::asio::yield_context& yield, int32_t timeout=5000)
		{
			return *detail::async_wrap_for_spawn<RR_SHARED_PTR<std::vector<NodeInfo2> > >(boost::bind((void (RobotRaconteurNode::*)(const std::string&, const std::vector<std::string>&, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) >, int32_t))&RobotRaconteurNode::FindNodeByID,this,boost::ref(name),boost::ref(transportschemes),_1,timeout),yield);
		}*/
#endif

	protected:
		void EndAsyncFindNodeByID(RobotRaconteur::NodeID id, std::vector<std::string> transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler);

		void EndAsyncFindNodeByName(std::string name, std::vector<std::string> transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler);



	public:

		std::string RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags);

		void AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		std::string ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj);

		void AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout = RR_TIMEOUT_INFINITE);

		void MonitorExit(RR_SHARED_PTR<RRObject> obj);

		class ScopedMonitorLock
		{
			bool locked;
			RR_SHARED_PTR<RRObject> obj;
			RR_WEAK_PTR<RobotRaconteurNode> node;
			
		public:

			RR_SHARED_PTR<RobotRaconteurNode> GetNode();
			
			ScopedMonitorLock(RR_SHARED_PTR<RRObject> obj, int32_t timeout=-1);

			void lock(int32_t timeout=-1);

			void unlock();

			void release();

			~ScopedMonitorLock();
			

		};

	private:
		
		

		
		RR_SHARED_PTR<Timer> PeriodicCleanupTask_timer;

		static void StartPeriodicCleanupTask(RR_SHARED_PTR<RobotRaconteurNode> node);

		void PeriodicCleanupTask(const TimerEvent& err);
		bool PeriodicCleanupTask_timerstarted;

		std::vector<RR_SHARED_PTR<IPeriodicCleanupTask> > cleanupobjs;

		boost::mutex cleanupobjs_lock;

		boost::mutex cleanup_thread_lock;

	public:
		void AddPeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task);

		void RemovePeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task);

		

	public:
		//Utility functions to help retrieve objrefs with a specific type

		RR_SHARED_PTR<RRObject> FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& objecttype);

		RR_SHARED_PTR<RRObject> FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& index, const std::string& objecttype);

		void AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& index, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		std::string FindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n);

		std::string FindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, const std::string &i);
		
		void AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, const std::string &i, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


	private:
		RR_SHARED_PTR<ThreadPool> thread_pool;
		boost::mutex thread_pool_lock;
		RR_SHARED_PTR<ThreadPoolFactory> thread_pool_factory;
		boost::mutex thread_pool_factory_lock;

	public:

		RR_SHARED_PTR<ThreadPool> GetThreadPool();

		void SetThreadPool(RR_SHARED_PTR<ThreadPool> pool);

		RR_SHARED_PTR<ThreadPoolFactory> GetThreadPoolFactory();

		void SetThreadPoolFactory(RR_SHARED_PTR<ThreadPoolFactory> factory);

		int32_t GetThreadPoolCount();

		void SetThreadPoolCount(int32_t count);



		//Do not call ReleaseThreadPool unless you really know what you are doing. In most cases it will be destroyed automatically.
		void ReleaseThreadPool();

	public:

		std::vector<std::string> GetPulledServiceTypes(RR_SHARED_PTR<RRObject> obj);

		RR_SHARED_PTR<ServiceFactory> GetPulledServiceType(RR_SHARED_PTR<RRObject> obj, const std::string& type);

		void SetExceptionHandler(boost::function<void (std::exception*)> handler);

		boost::function<void (std::exception*)> GetExceptionHandler();

		void HandleException(std::exception* exp);

	protected:

		boost::function<void (std::exception*) > exception_handler;
		boost::mutex exception_handler_lock;

		boost::mutex random_generator_lock;
		boost::random::mt19937 random_generator;

	public:

		virtual boost::posix_time::ptime NowUTC();

	protected:

		RR_WEAK_PTR<ITransportTimeProvider> time_provider;
		boost::shared_mutex time_provider_lock;

	public:

		virtual RR_SHARED_PTR<Timer> CreateTimer(const boost::posix_time::time_duration& period, boost::function<void (const TimerEvent&)> handler, bool oneshot=false);

		virtual RR_SHARED_PTR<Rate> CreateRate(double frequency);

		virtual void Sleep(const boost::posix_time::time_duration& duration);

		virtual RR_SHARED_PTR<AutoResetEvent> CreateAutoResetEvent();

		void DownCastAndThrowException(RobotRaconteurException& exp);

		RR_SHARED_PTR<RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp);

		virtual std::string GetServicePath(RR_SHARED_PTR<RRObject> obj);
	};


}
