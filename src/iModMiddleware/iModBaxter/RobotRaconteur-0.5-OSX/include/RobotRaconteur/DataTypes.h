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



#include <string>
#include <vector>

#include <stdint.h>
#include <map>
#include <deque>
#include <list>


#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/Error.h"

#include <stdexcept>

#include "RobotRaconteur/RobotRaconteurConfig.h"

#include <boost/array.hpp>

#pragma once



namespace RobotRaconteur
{
	template<class T, class U> static RR_SHARED_PTR<T> rr_cast(RR_SHARED_PTR<U> objin)
	{
		if (!objin) return RR_SHARED_PTR<T>();

		RR_SHARED_PTR<T> c=RR_DYNAMIC_POINTER_CAST<T>(objin);
		if (!c)
		{			 
			throw DataTypeMismatchException("Data type cast error");
		}

		return c;
	}

	class RRObject : boost::noncopyable
	{
	public:
		RRObject();

		virtual ~RRObject() {}

		virtual std::string RRType()=0;
		

	};

	class  MessageElementData : public RRObject
	{
	public:
		virtual std::string GetTypeString()=0;
		virtual DataTypes GetTypeID()=0;
	};


	template<class T>
	class RRArray;

	template<class T>
	static RR_SHARED_PTR<RRArray<T> > ScalarToRRArray(T value);

	template<class T>
	static T RRArrayToScalar(RR_SHARED_PTR<RRArray<T> > value);


	RR_SHARED_PTR<RRArray<char> > stringToRRArray(const std::string& str);

	std::string RRArrayToString(RR_SHARED_PTR<RRArray<char> > arr);


	

#ifdef ROBOTRACONTEUR_USE_WSTRING
	RR_SHARED_PTR<RRArray<char> > wstringToRRArray(const std::wstring& str);

	std::wstring RRArrayToWString(RR_SHARED_PTR<RRArray<char> > arr);

	// Convert a wide Unicode string to an UTF8 string
	std::string utf8_encode(const std::wstring &wstr);

	// Convert an UTF8 string to a wide Unicode String
	std::wstring utf8_decode(const std::string &str);
#endif


	template <class T> class RRPrimUtil  
	{ 
	public:
		static DataTypes GetTypeID() {return DataTypes_void_t;} 
		
		
		static RR_SHARED_PTR<RRObject> PrePack(T val) {return rr_cast<RRObject>(val);} 
		
		static T PreUnpack(RR_SHARED_PTR<RRObject> val) {return rr_cast<T>(val);} 

		virtual ~RRPrimUtil() {};
	};
	
	template <typename T> class RRPrimUtil<RR_SHARED_PTR<T> >
	{ 
	public:
		static DataTypes GetTypeID() {return DataTypes_void_t;}
		
		
		static RR_SHARED_PTR<RRObject> PrePack(RR_SHARED_PTR<T> val) {return rr_cast<RRObject>(val);} 
		
		static RR_SHARED_PTR<T> PreUnpack(RR_SHARED_PTR<RRObject> val) {return rr_cast<T>(val);} 
		
		virtual ~RRPrimUtil() {};
	};

	template<> class RRPrimUtil<std::string>
	{
	public:
		static DataTypes GetTypeID() {return DataTypes_string_t;} 
		static RR_SHARED_PTR<RRObject> PrePack(std::string val) {return rr_cast<RRObject>(stringToRRArray(val));} 
		static std::string PreUnpack(RR_SHARED_PTR<RRObject> val) {return RRArrayToString(rr_cast<RRArray<char> >(val));} 

		virtual ~RRPrimUtil() {};

	};

#ifdef ROBOTRACONTEUR_USE_WSTRING

	template<> class RRPrimUtil<std::wstring>
	{
	public:
		static DataTypes GetTypeID() {return DataTypes_string_t;} 
		static RR_SHARED_PTR<RRObject> PrePack(std::wstring val) {return rr_cast<RRObject>(wstringToRRArray(val));} 
		static std::wstring PreUnpack(RR_SHARED_PTR<RRObject> val) {return RRArrayToWString(rr_cast<RRArray<char> >(val));} 

	};
#endif

#define RRPrimUtilNumeric(x,code) template<> class RRPrimUtil<x> {public: static DataTypes GetTypeID() {return code;}  static RR_SHARED_PTR<RRObject> PrePack(x val) {return rr_cast<RRObject>(ScalarToRRArray(val));}  static x PreUnpack(RR_SHARED_PTR<RRObject> val) {return RRArrayToScalar(rr_cast<RRArray<x> >(val));} }; 

	RRPrimUtilNumeric(double,DataTypes_double_t);
	RRPrimUtilNumeric(float,DataTypes_single_t);

	RRPrimUtilNumeric(int8_t,DataTypes_int8_t);
	RRPrimUtilNumeric(uint8_t,DataTypes_uint8_t);
	RRPrimUtilNumeric(int16_t,DataTypes_int16_t);
	RRPrimUtilNumeric(uint16_t,DataTypes_uint16_t);
	RRPrimUtilNumeric(int32_t,DataTypes_int32_t);
	RRPrimUtilNumeric(uint32_t,DataTypes_uint32_t);
	RRPrimUtilNumeric(int64_t,DataTypes_int64_t);
	RRPrimUtilNumeric(uint64_t,DataTypes_uint64_t);
	RRPrimUtilNumeric(char,DataTypes_string_t);

	std::string GetRRDataTypeString(DataTypes type);

	class RRBaseArray : public MessageElementData
	{
	public:
		

		virtual std::string GetTypeString()
		{
			std::string type=GetRRDataTypeString(GetTypeID());
	//string type="hello";
			return type+"[]";
		}

		virtual ~RRBaseArray() {}

		virtual size_t Length()=0;
		
		virtual std::string RRType();

		virtual void* void_ptr()=0;

		virtual size_t ElementSize()=0;
		
	};

	template <class T>
	class  RRArray : public RRBaseArray
	{
	public:

		
		

		virtual DataTypes GetTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();

		}
		
		
		RRArray(T* data, size_t length, bool owned)
		{
			this->data=data;
			this->owned=owned;
			this->element_count=length;
		}

		virtual ~RRArray()
		{
			if (owned)
				delete[] data;
		}

		virtual T* ptr()
		{
			return data;
		}

		virtual void* void_ptr()
		{
			return ptr();
		}

		T *operator->() const {
            return data;
        }

		inline T& operator[](size_t pos) const
		{
			if (pos >= element_count)
			{
				throw std::out_of_range("Index out of range");
			}

			return data[pos];
		}

		
		virtual size_t Length()
		{
			return element_count;
		}

		virtual size_t size()
		{
			return Length();
		}

		virtual size_t ElementSize()
		{
			return sizeof(T);
		}


	private:
		T* data;
		size_t element_count;
		bool owned;

	};



	template<class K, class T>
	class RRMap : public RRObject
	{
	public:
		std::map<K,RR_SHARED_PTR<T> > map;

		RRMap() {}

		RRMap(std::map<K,RR_SHARED_PTR<T> > mapin)
		{
			map=mapin;
		}

		virtual ~RRMap() {}

		virtual std::string RRType()
		{
			std::string keytype;
			if (typeid(K) == typeid(std::string) )
			{
				keytype="string";
			}
#ifdef ROBOTRACONTEUR_USEWSTRING
			if (typeid(K) ==typeid(std::wstring))
			{
				keytype="wstring";
			}
#endif
			else if(typeid(K)==typeid(int32_t))
			{
				keytype="int32_t";
			}
			else
			{
				throw DataTypeException("Unknown data type");

				
			}


			return "RobotRaconteur.RRMap<" +keytype +">";
		}
	};


	template<class T>
	class RRList : public RRObject
	{
	public:
		std::vector<RR_SHARED_PTR<T> > list;

		RRList() {}

		RRList(std::list<RR_SHARED_PTR<T> > listin)
		{
			list=listin;
		}

		virtual ~RRList() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRList";
		}
	};


	class RRStructure : public RRObject
	{
	public:
		virtual ~RRStructure() {}

	};

	class RRMultiDimBaseArray : public RRObject
	{
	public:
		virtual ~RRMultiDimBaseArray() {}
		

	};

	namespace detail
	{
		void MultiDimArray_CalculateCopyIndices(int32_t mema_dimcount, std::vector<int32_t> mema_dims, std::vector<int32_t> memorypos, int32_t memb_dimcount, std::vector<int32_t> memb_dims, std::vector<int32_t> startbuffer, std::vector<int32_t> count, int32_t &dimcount, std::vector<int32_t>& indexa, std::vector<int32_t>& indexb);
	}

	template<class T>
	class RRMultiDimArray : public RRMultiDimBaseArray
	{
	public:
		int32_t DimCount;
		RR_SHARED_PTR<RRArray<int32_t> > Dims;

		bool Complex;
		RR_SHARED_PTR<RRArray<T> > Real;
		RR_SHARED_PTR<RRArray<T> > Imag;

		
		RRMultiDimArray() {};

		RRMultiDimArray(RR_SHARED_PTR<RRArray<int32_t> > Dims, RR_SHARED_PTR<RRArray<T> > Real, RR_SHARED_PTR<RRArray<T> > Imag=RR_SHARED_PTR<RRArray<T> >())
		{
			this->Complex=false;
			this->DimCount=Dims->Length();
			this->Dims=Dims;
			this->Real=Real;
			if (Imag)
			{
				this->Complex=true;
				this->Imag=Imag;
			}

		}

		virtual ~RRMultiDimArray() {}

		virtual std::string RRType()
		{
			std::string stype;
			DataTypes type=RRPrimUtil<T>::GetTypeID();
			stype=GetRRDataTypeString(type);
			return "RobotRaconteur.RRMultiDimArray<" + stype + ">";
		}

		virtual void RetrieveSubArray(std::vector<int32_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<int32_t> bufferpos, std::vector<int32_t> count)
		{

			//TODO: put in bounds checks
			if (Complex != buffer->Complex) throw std::invalid_argument("Complex mismatch");

			int dimcount;
			std::vector<int32_t> index;
			std::vector<int32_t> indexb;

			std::vector<int32_t> mema_dims=std::vector<int32_t>(Dims->ptr(),Dims->ptr()+Dims->size());
			std::vector<int32_t> memb_dims=std::vector<int32_t>(buffer->Dims->ptr(),buffer->Dims->ptr()+buffer->Dims->size());
			detail::MultiDimArray_CalculateCopyIndices(DimCount,mema_dims,memorypos,buffer->DimCount,memb_dims,bufferpos,count,dimcount,index,indexb);

			for (size_t i=0; i<index.size(); i++)
			{
				memcpy((buffer->Real->ptr()+indexb[i]),(Real->ptr())+index[i],count[0]*sizeof(T));
				if (Complex)
				{
					memcpy((buffer->Imag->ptr()+indexb[i]),(Imag->ptr())+index[i],count[0]*sizeof(T));
				}
			}

		}
		
		virtual void AssignSubArray(std::vector<int32_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<int32_t> bufferpos, std::vector<int32_t> count)
		{
			//TODO: put in bounds checks
			if (Complex != buffer->Complex) throw std::invalid_argument("Complex mismatch");

			int dimcount;
			std::vector<int32_t> index;
			std::vector<int32_t> indexb;

			std::vector<int32_t> mema_dims=std::vector<int32_t>(Dims->ptr(),Dims->ptr()+Dims->size());
			std::vector<int32_t> memb_dims=std::vector<int32_t>(buffer->Dims->ptr(),buffer->Dims->ptr()+buffer->Dims->size());
			detail::MultiDimArray_CalculateCopyIndices(DimCount,mema_dims,memorypos,buffer->DimCount,memb_dims,bufferpos,count,dimcount,index,indexb);

			for (size_t i=0; i<index.size(); i++)
			{
				memcpy((Real->ptr()+index[i]),(buffer->Real->ptr()+indexb[i]),count[0]*sizeof(T));
				
				if (Complex)
				{
					memcpy(((Imag->ptr())+index[i]),((buffer->Imag->ptr())+indexb[i]),count[0]*sizeof(T));
				}
			}

		}

	};

	

	template<class T>
	static RR_SHARED_PTR<RRArray<T> > AllocateRRArray(size_t length)
	{
		T* data=new T[length];
		RRArray<T> * ar=new RRArray<T>(data,length,true);
		return RR_SHARED_PTR<RRArray<T> >(ar);
	}

	template<class T>
	static RR_SHARED_PTR<RRArray<T> > AttachRRArray(T* data,size_t length,bool owned)
	{
	
		RRArray<T> * ar=new RRArray<T>(data,length,owned);
		return RR_SHARED_PTR<RRArray<T> >(ar);
	}

	template<class T>
	static RR_SHARED_PTR<RRArray<T> > AttachRRArrayCopy(const T* data,const size_t length)
	{
		T* data_copy=new T[length];
		memcpy(data_copy,data,length*sizeof(T));
		RRArray<T> * ar=new RRArray<T>(data_copy,length,true);
		return RR_SHARED_PTR<RRArray<T> >(ar);
	}

	RR_SHARED_PTR<RRBaseArray> AllocateRRArrayByType(DataTypes type, size_t length);


    size_t RRArrayElementSize(DataTypes type);
    
    template<class T>
	static RR_SHARED_PTR<RRArray<T> > ScalarToRRArray(T value)
	{
		T* data=new T[1];
		data[0]=value;
		return AttachRRArray(data,1,true);
	}
    
	template<class T>
	static T RRArrayToScalar(RR_SHARED_PTR<RRArray<T> > value)
	{
		if (!value)
		{
			throw std::invalid_argument("Null pointer");
		}
        
		if (value->Length()==0) throw std::out_of_range("Index out of range");
        
		return (*value)[0];
        
	}

	template <class Y,class U>
	static std::vector<Y> RRArrayToVector(RR_SHARED_PTR<RRArray<U> > in)
	{
		std::vector<Y> out(in->size());
		for (size_t i=0; i<in->size(); i++) out[i]=(Y)(*in)[i];
		return out;
	}

	template <class Y,class U>
	static RR_SHARED_PTR<RRArray<Y> > VectorToRRArray(std::vector<U> in)
	{
		RR_SHARED_PTR<RRArray<Y> > out=AllocateRRArray<Y>(in.size());
		for (size_t i=0; i<in.size(); i++) (*out)[i]=(Y)in[i];
		return out;
	}

	template <class Y,std::size_t N,class U>
	static boost::array<Y,N> RRArrayToArray(RR_SHARED_PTR<RRArray<U> > in)
	{
		if (in->size()!=N) throw std::out_of_range("Array is incorrect size");
		boost::array<Y,N> out;
		for (size_t i=0; i<N; i++) out[i]=(Y)(*in)[i];
		return out;
	}

	template <class Y,class U,std::size_t N>
	static RR_SHARED_PTR<RRArray<Y> > ArrayToRRArray(boost::array<U,N> in)
	{
		RR_SHARED_PTR<RRArray<Y> > out=AllocateRRArray<Y>(N);
		for (size_t i=0; i<N; i++) (*out)[i]=(Y)in[i];
		return out;
	}

	class RobotRaconteurNode;

	class TimeSpec
	{
	public:
		int64_t seconds;
		int32_t nanoseconds;

		TimeSpec();

		TimeSpec(int64_t seconds, int32_t nanoseconds);

		static TimeSpec Now();

		static TimeSpec Now(RR_SHARED_PTR<RobotRaconteurNode> node);

	public:
		bool operator == (const TimeSpec &t2);

		bool operator != (const TimeSpec &t2);

		TimeSpec operator - (const TimeSpec &t2);

		TimeSpec operator + (const TimeSpec &t2);

		bool operator > (const TimeSpec &t2);

		bool operator >= (const TimeSpec &t2);

		bool operator < (const TimeSpec &t2);

		bool operator <= (const TimeSpec &t2);

	public:

		void cleanup_nanosecs();

	};

	
}
