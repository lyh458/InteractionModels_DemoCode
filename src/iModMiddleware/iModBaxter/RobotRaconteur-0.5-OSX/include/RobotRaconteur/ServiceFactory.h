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
#include "RobotRaconteur/ServiceDefinition.h"

namespace RobotRaconteur
{

	class StructureStub;
	class ServiceStub;
	class ServiceSkel;

	class ClientContext;
	class ServerContext;

	class RobotRaconteurNode;
	
	class ServiceFactory
	{

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		void SetNode(RR_SHARED_PTR<RobotRaconteurNode> node);

		virtual ~ServiceFactory() {}

		virtual std::string GetServiceName()=0;

		virtual std::string DefString()=0;

		virtual RR_SHARED_PTR<StructureStub> FindStructureStub(std::string s)=0;
		
		virtual RR_SHARED_PTR<MessageElementStructure> PackStructure(RR_SHARED_PTR<RRStructure> structin)=0;
		
		
		virtual RR_SHARED_PTR<RRObject> UnpackStructure(RR_SHARED_PTR<MessageElementStructure> mstructin)=0; 

		virtual RR_SHARED_PTR<ServiceStub> CreateStub(std::string objecttype, std::string path, RR_SHARED_PTR<ClientContext> context)=0;

		virtual RR_SHARED_PTR<ServiceSkel> CreateSkel(std::string objecttype, std::string path, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<ServerContext> context)=0;

		virtual RR_SHARED_PTR<ServiceDefinition> ServiceDef();
		
		virtual std::string RemovePath(const std::string &path);

		virtual void DownCastAndThrowException(RobotRaconteurException& exp)=0;

		virtual RR_SHARED_PTR<RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp)=0;

	private:
		RR_SHARED_PTR<ServiceDefinition> sdef;
		RR_WEAK_PTR<RobotRaconteurNode> node;
	};

	class DynamicServiceFactory
	{
	public:

		virtual ~DynamicServiceFactory() {}

		virtual RR_SHARED_PTR<ServiceFactory> CreateServiceFactory(std::string def) = 0;

		virtual std::vector<RR_SHARED_PTR<ServiceFactory> > CreateServiceFactories(std::vector<std::string> def) = 0;
	};

}
