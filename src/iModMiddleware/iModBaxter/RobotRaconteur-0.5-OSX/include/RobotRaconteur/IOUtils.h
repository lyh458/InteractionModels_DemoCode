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
#include <boost/detail/endian.hpp>
#include <stack>


namespace RobotRaconteur

{
	class ArrayBinaryReader
	{
	public:
		ArrayBinaryReader(uint8_t* buffer, size_t start_position, size_t length, bool nativeorder=false);

		size_t Length();

		virtual size_t Position();
		virtual void Seek(size_t position);

		virtual size_t Read(void* buffer, size_t index, size_t length);
		
		template <typename T> 
		T ReadNumber()
		{
			T out;
			Read((uint8_t*)((void*)&out),0,sizeof(T));
#ifdef BOOST_BIG_ENDIAN
			if (!nativeorder) std::reverse((uint8_t*)((void*)&out),((uint8_t*)((void*)&out))+sizeof(T));
#endif
			return out;

		}
		
		
		void ReadArray(RR_SHARED_PTR<RRBaseArray>& arr);

		std::string ReadString8(size_t length);

		//A stack to set local limits on the
		//length of reads.  This will detect errors
		//in binary messages.

		virtual size_t CurrentLimit();

		virtual void PushRelativeLimit(size_t limit);
		
		virtual void PushAbsoluteLimit(size_t limit);

		virtual void PopLimit();

		virtual int32_t DistanceFromLimit();

	private:
		uint8_t* buffer;
		size_t position;
		size_t length;
		bool nativeorder;

		std::stack<size_t> limits;


	};

	class ArrayBinaryWriter
	{
	public:
		ArrayBinaryWriter(uint8_t* buffer, size_t start_position, size_t length, bool nativeorder=false);

		virtual size_t Length();

		virtual size_t Position();
		virtual void Seek(size_t position);

		virtual size_t Write(const void* buffer, size_t index, size_t length);
		
		template <typename T> 
		void WriteNumber(T number)
		{
			void* n1=(void*)&number;
#ifdef BOOST_BIG_ENDIAN
			if (!nativeorder) std::reverse((uint8_t*)n1,((uint8_t*)n1)+sizeof(T));
#endif
			Write((uint8_t*)n1,0,sizeof(T));
			

		}

		
		void WriteArray(RR_SHARED_PTR<RRBaseArray>& arr);

		void WriteString8(std::string str);

		static size_t GetStringByteCount8(const std::string& str);

		//A stack to set local limits on the
		//length of reads.  This will detect errors
		//in binary messages.

		virtual size_t CurrentLimit();

		virtual void PushRelativeLimit(size_t limit);
		
		virtual void PushAbsoluteLimit(size_t limit);

		virtual void PopLimit();

		virtual int32_t DistanceFromLimit();

	private:
		uint8_t* buffer;
		size_t position;
		size_t length;
		bool nativeorder;

		std::stack<size_t> limits;


	};

}
