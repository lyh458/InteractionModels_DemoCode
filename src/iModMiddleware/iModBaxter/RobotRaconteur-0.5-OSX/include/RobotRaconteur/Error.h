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


//#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"
#include <stdexcept>
#include <string>
#pragma once


namespace RobotRaconteur
{
	
	
	class RobotRaconteurException : public std::runtime_error
	{
	
	public:
		RobotRaconteurException();

		RobotRaconteurException(MessageErrorType ErrorCode, const std::string &error, const std::string &message);

		RobotRaconteurException(const std::string &message, std::exception &innerexception);

		MessageErrorType ErrorCode;

		std::string Error;

		std::string Message;

		virtual std::string ToString();

		virtual const char* what();

		~RobotRaconteurException() throw () {}

	private:
		std::string what_string;
		void InitializeInstanceFields();
	};
	
	class ConnectionException : public RobotRaconteurException
	{
		
	public:
		ConnectionException(const std::string &message);

	};
	
	class ProtocolException : public RobotRaconteurException
	{
	public:
		ProtocolException(const std::string &message);

	};
	
	class ServiceNotFoundException : public RobotRaconteurException
	{
	public:
		ServiceNotFoundException(const std::string &message);

	};
	
	class ObjectNotFoundException : public RobotRaconteurException
	{
	public:
		ObjectNotFoundException(const std::string &message);

	};

	class InvalidEndpointException : public RobotRaconteurException
	{

	public:
		InvalidEndpointException(const std::string &message);

	};

	class EndpointCommunicationFatalException : public RobotRaconteurException
	{
	public:
		EndpointCommunicationFatalException(const std::string &message);

	};
	
	class NodeNotFoundException : public RobotRaconteurException
	{
	public:
		NodeNotFoundException(const std::string &message);

	};

	class ServiceException : public RobotRaconteurException
	{
	public:
		ServiceException(const std::string &message);

	};

	class MemberNotFoundException : public RobotRaconteurException
	{
	public:
		MemberNotFoundException(const std::string &message);

	};

	
	class MemberFormatMismatchException : public RobotRaconteurException
	{
	
	public:
		MemberFormatMismatchException(const std::string &message);

	};

	class DataTypeMismatchException : public RobotRaconteurException
	{
	public:
		DataTypeMismatchException(const std::string &message);

	};
	
	class DataTypeException : public RobotRaconteurException
	{
	public:
		DataTypeException(const std::string &message);

	};
	
	class DataSerializationException : public RobotRaconteurException
	{
	public:
		DataSerializationException(const std::string &message);

	};
	
	class MessageEntryNotFoundException : public RobotRaconteurException
	{
	public:
		MessageEntryNotFoundException(const std::string &message);

	};

	class MessageElementNotFoundException : public RobotRaconteurException
	{
	public:
		MessageElementNotFoundException(const std::string &message);

	};

	class UnknownException : public RobotRaconteurException
	{
	public:
		UnknownException(const std::string &error, const std::string &message);

	};

	class RobotRaconteurRemoteException : public RobotRaconteurException
	{
	public:
		RobotRaconteurRemoteException(const std::string &error, const std::string &message);

		RobotRaconteurRemoteException(std::exception &innerexception);

	};

	class TransactionTimeoutException : public RobotRaconteurException
	{
	public:
		TransactionTimeoutException(const std::string &message);

	};

	class AuthenticationException : public RobotRaconteurException
	{
	public:
		AuthenticationException(const std::string &message);

	};

	class ObjectLockedException : public RobotRaconteurException
	{
	public:
		ObjectLockedException(const std::string &message);

	};
	
	


}
