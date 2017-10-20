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
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"

#include <boost/tuple/tuple.hpp>


namespace RobotRaconteur
{
	
	class ServiceEntryDefinition;
	class MemberDefinition;
	class FunctionDefinition;
	class PropertyDefinition;
	class EventDefinition;
	class ObjRefDefinition;
	class PipeDefinition;
	class CallbackDefinition;
	class WireDefinition;
	class MemoryDefinition;
	class TypeDefinition;


	class ServiceDefinition : public RR_ENABLE_SHARED_FROM_THIS<ServiceDefinition>
	{
	public:
		std::string Name;

		std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Structures;
		std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > Objects;

		std::vector<std::string> Options;

		std::vector<std::string> Imports;

		std::vector<std::string> Exceptions;

		std::string Version;

		virtual ~ServiceDefinition() {}

		virtual std::string ToString();

		void FromString(const std::string &s);

		void CheckVersion(std::string ver="");






	private:
		void InitializeInstanceFields();

public:
		ServiceDefinition()
		{
			InitializeInstanceFields();
		}
	};

	class ServiceEntryDefinition : public RR_ENABLE_SHARED_FROM_THIS<ServiceEntryDefinition>
	{
	public:
		std::string Name;

		std::vector<RR_SHARED_PTR<MemberDefinition> > Members;

		bool IsStructure;


		std::vector<std::string> Implements;
		std::vector<std::string> Options;

		

		ServiceEntryDefinition(RR_SHARED_PTR<ServiceDefinition> def);

		virtual ~ServiceEntryDefinition() {}

		virtual std::string ToString();

		void FromString(const std::string &s);

		void FromString(const std::string &s, int32_t startline);

		RR_WEAK_PTR<ServiceDefinition> ServiceDefinition_;

	private:
		void InitializeInstanceFields();
	};


	class MemberDefinition : public RR_ENABLE_SHARED_FROM_THIS<MemberDefinition>
	{
	public:
		std::string Name;
		RR_WEAK_PTR<ServiceEntryDefinition> ServiceEntry;

		MemberDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual ~MemberDefinition() {}

		virtual std::string ToString() { return ""; }
	};

	class PropertyDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		PropertyDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual ~PropertyDefinition() {}

		virtual std::string ToString();

		std::string ToString(bool isstruct);

		void FromString(const std::string &s);

	};


	class FunctionDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> ReturnType;
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		FunctionDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

	private:
		void InitializeInstanceFields();
	};

	class EventDefinition : public MemberDefinition
	{
	public:
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		EventDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

	private:
		void InitializeInstanceFields();
	};

	class ObjRefDefinition : public MemberDefinition
	{
	public:
		std::string ObjectType;
		bool IsArray;

		bool IsMap;
		DataTypes KeyType;

		ObjRefDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();


		void FromString(const std::string &s);
	};

	class PipeDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		PipeDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

	};

	class CallbackDefinition : public MemberDefinition
	{
	public:

		virtual ~CallbackDefinition() {}

		RR_SHARED_PTR<TypeDefinition> ReturnType;
		std::vector<RR_SHARED_PTR<TypeDefinition> > Parameters;

		CallbackDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

	private:
		void InitializeInstanceFields();
	};

	class WireDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		WireDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

	};

	class MemoryDefinition : public MemberDefinition
	{
	public:
		RR_SHARED_PTR<TypeDefinition> Type;

		MemoryDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry);

		virtual std::string ToString();

		void FromString(const std::string &s);

	};

	class TypeDefinition
	{
	public:
		virtual ~TypeDefinition() {}

		std::string Name;

		DataTypes Type;
		std::string TypeString;

		bool IsArray;

		bool IsMultidimArray;

		bool IsList;


		bool IsMap;

		DataTypes KeyType;

		uint32_t Length;


		std::vector<int32_t> MultiDimLength;

		bool VarLength;

		bool ImportedType;

		RR_WEAK_PTR<MemberDefinition> member;


		TypeDefinition();

		virtual std::string ToString();

		void FromString(const std::string &s);

		static DataTypes DataTypeFromString(const std::string &d);

		static std::string StringFromDataType(DataTypes d);





	private:
		void InitializeInstanceFields();
	};

	class RobotRaconteurParseException : public std::runtime_error
	{

	public:
		int32_t LineNumber;

		std::string Message;

		RobotRaconteurParseException(const std::string &e);
		RobotRaconteurParseException(const std::string &e, int32_t line);

		virtual std::string ToString();

		virtual const char* what();

		~RobotRaconteurParseException() throw () {}

	private:
		std::string what_store;

	};

	void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def);

	boost::tuple<std::string,std::string> SplitQualifiedName(const std::string& name);

}
