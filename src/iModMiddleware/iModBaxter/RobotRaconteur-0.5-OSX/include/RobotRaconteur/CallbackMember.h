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
#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
	template<typename T>
	class Callback
	{

	protected:
		std::string m_MemberName;

	public:
		Callback(std::string name)
		{
			m_MemberName = name;
		}

		virtual ~Callback() {}

		virtual T GetFunction() = 0;
		virtual void SetFunction(T value) = 0;

		virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e) = 0;

		virtual T GetClientFunction(uint32_t e) = 0;

		virtual std::string GetMemberName()
		{
			return m_MemberName;
		}

		virtual void Shutdown()
		{
			
		}


	};

	template<typename T>
	class CallbackClient : public Callback<T>
	{
	public:
		CallbackClient(std::string name) : Callback<T>(name)
		{
			InitializeInstanceFields();
		}

		virtual ~CallbackClient() {}


	private:
		T function;
		boost::mutex function_lock;
	public:
		virtual T GetFunction() 
		{
			boost::mutex::scoped_lock lock(function_lock);
			if (!function) throw std::runtime_error("Callback function not set");
			return function;
		}
		virtual void SetFunction(T value)
		{
			boost::mutex::scoped_lock lock(function_lock);
			function = value;
		}

		virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e)
		{
			throw std::runtime_error("Invalid for client side of callback");
		}

		virtual T GetClientFunction(uint32_t e)
		{
			throw std::runtime_error("Invalid for client side of callback");
		}

		virtual void Shutdown()
		{
			boost::mutex::scoped_lock lock(function_lock);
			function.clear();
		}



	private:
		void InitializeInstanceFields()
		{
			function = T();
		}
	};

	class ServiceSkel;


	class CallbackServerBase
	{

	public:
		virtual ~CallbackServerBase() {}

	protected:
		RR_WEAK_PTR<ServiceSkel> skel;
		

		virtual RR_SHARED_PTR<void> GetClientFunction_internal(uint32_t e);

		virtual std::string GetMemberName()=0;
	};

	template<typename T>
	class CallbackServer : public Callback<T>, public CallbackServerBase
	{
	private:
		

	public:
		CallbackServer(std::string name, RR_SHARED_PTR<ServiceSkel> skel) : Callback<T>(name)
		{
			this->skel = skel;
		}

		virtual ~CallbackServer() {}

		virtual T GetFunction()
		{
			throw std::runtime_error("Invalid for server side of callback");
		}
		virtual void SetFunction(T value)
		{
			throw std::runtime_error("Invalid for server side of callback");
		}

		virtual T GetClientFunction(RR_SHARED_PTR<Endpoint> e)
		{
			return GetClientFunction(e->GetLocalEndpoint());
		}

		virtual T GetClientFunction(uint32_t e)
		{
			RR_SHARED_PTR<ServiceSkel> s=skel.lock();
			if (!s) throw std::runtime_error("Callback server has been closed");
			return *RR_STATIC_POINTER_CAST<T>(s->GetCallbackFunction(e,GetMemberName()));
		}

	
		virtual std::string GetMemberName()
		{
			return Callback<T>::GetMemberName();
		}

		virtual void Shutdown()
		{
			
		}

		
		

	};
}
