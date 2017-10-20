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
#include "RobotRaconteur/NodeID.h"





namespace RobotRaconteur
{

	class ArrayBinaryReader;
	class ArrayBinaryWriter;
	class NodeID;

	class Message;
	class MessageHeader;
	class MessageEntry;
	class MessageElement;

	class Message : public virtual RRObject
	{
	public:
		RR_SHARED_PTR<MessageHeader> header;
		std::vector<RR_SHARED_PTR<MessageEntry> > entries;

		Message();

		uint32_t ComputeSize();

		void Write(ArrayBinaryWriter &w);

		RR_SHARED_PTR<MessageEntry> FindEntry(std::string name);

		RR_SHARED_PTR<MessageEntry> AddEntry(MessageEntryType t, std::string name);

		void Read(ArrayBinaryReader &r);

		virtual std::string RRType()
		{
			return "RobotRaconteur::Message";
		}
	};

	class MessageHeader : public virtual RRObject
	{
	public:

		virtual ~MessageHeader() {}

		uint16_t HeaderSize;

		uint32_t SenderEndpoint;

		uint32_t ReceiverEndpoint;

		std::string SenderNodeName;

		std::string ReceiverNodeName;

		NodeID SenderNodeID;

		NodeID ReceiverNodeID;

		std::string MetaData;

		uint16_t EntryCount;

		uint16_t MessageID;

		uint16_t MessageResID;

		uint32_t MessageSize;

		uint16_t ComputeSize();

		void UpdateHeader(uint32_t message_size, uint16_t entry_count);

		void Write(ArrayBinaryWriter &w);

		void Read(ArrayBinaryReader &r);

		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageHeader";
		}

	private:
		void InitializeInstanceFields();

public:
		MessageHeader()
		{
			InitializeInstanceFields();
		}
	};


	class MessageEntry : public virtual RRObject
	{
	public:
		virtual ~MessageEntry() {}

		uint32_t EntrySize;

		MessageEntryType EntryType;

		std::string ServicePath;

		std::string MemberName;

		uint32_t TransactionID;

		MessageErrorType Error;

		std::string MetaData;

		std::vector<RR_SHARED_PTR<MessageElement> > elements;

		MessageEntry();

		MessageEntry(MessageEntryType t, std::string n);

		uint32_t ComputeSize();

		RR_SHARED_PTR<MessageElement> FindElement(std::string name);

		RR_SHARED_PTR<MessageElement> AddElement(std::string name, RR_SHARED_PTR<MessageElementData> data);

		RR_SHARED_PTR<MessageElement> AddElement(RR_SHARED_PTR<MessageElement> m);

		void Write(ArrayBinaryWriter &w);

		void Read(ArrayBinaryReader &r);

		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageEntry";
		}

	private:
		void InitializeInstanceFields();
	};

	

	class MessageElement : public virtual RRObject
	{
	public:

		virtual ~MessageElement() {}

		uint32_t ElementSize;

		std::string ElementName;

		DataTypes ElementType;

		std::string ElementTypeName;

		std::string MetaData;

		uint32_t DataCount;

	private:
		RR_SHARED_PTR<MessageElementData> dat;

	public:
		MessageElement();
				
		MessageElement(std::string name, RR_SHARED_PTR<MessageElementData> datin);
				
		RR_SHARED_PTR<MessageElementData> GetData();
		void SetData(RR_SHARED_PTR<MessageElementData> value);

		uint32_t ComputeSize();

		void UpdateData();

		void Write(ArrayBinaryWriter &w);

		void Read(ArrayBinaryReader &r);

		static RR_SHARED_PTR<MessageElement> FindElement(std::vector<RR_SHARED_PTR<MessageElement> > &m, std::string name);

		static bool ContainsElement(std::vector<RR_SHARED_PTR<MessageElement> > &m, std::string name);

		template<class T>
		RR_SHARED_PTR<T> CastData()
		{
			return rr_cast<T>(dat);
		}

		std::string CastDataToString();

		template<class T>
		static RR_SHARED_PTR<T> CastData(RR_SHARED_PTR<MessageElementData> Data)
		{
			return rr_cast<T>(Data);
		}

		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElement";
		}

	private:
		void InitializeInstanceFields();
	};

		class  MessageElementStructure : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		MessageElementStructure(std::string type_, const std::vector<RR_SHARED_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_structure_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::CMessageElementStructure";
		}

	};

	

	template<class T>
	class MessageElementMap : public MessageElementData
	{
	public:
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;


		MessageElementMap(const std::vector<RR_SHARED_PTR<MessageElement> >& e)
		{
			Elements=e;
		}

		virtual std::string GetTypeString()
		{
			if (typeid(T) == typeid(int32_t))
			{
				return "varvalue{int32}";
			}

			if (typeid(T) == typeid(std::string))
			{
				return "varvalue{string}";
			}
			throw DataTypeException("Invalid set keytype");
		}
		virtual DataTypes GetTypeID()
		{
			if (typeid(T) == typeid(int32_t))
			{
				return DataTypes_vector_t;
			}

			if (typeid(T) == typeid(std::string))
			{
				return DataTypes_dictionary_t;
			}
			throw DataTypeException("Invalid object keytype");	
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::CMessageElementStructure";
		}



	};

	
	class MessageElementList : public MessageElementData
	{
	public:
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;


		MessageElementList(const std::vector<RR_SHARED_PTR<MessageElement> >& e)
		{
			Elements=e;
		}

		virtual std::string GetTypeString()
		{
			return "varvalue{list}";
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_list_t;	
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::CMessageElementStructure";
		}



	};

	class MessageElementMultiDimArray  : public MessageElementData
	{
	public:


		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		virtual ~MessageElementMultiDimArray() {}

		MessageElementMultiDimArray(const std::vector<RR_SHARED_PTR<MessageElement> > &e);

		virtual std::string GetTypeString()
		{
			DataTypes rrt=GetTypeID();

			
			std::string elementtype=GetRRDataTypeString(rrt);

			return elementtype + "[*]";

		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_multidimarray_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementMultiDimArray";
		}
	};



}
