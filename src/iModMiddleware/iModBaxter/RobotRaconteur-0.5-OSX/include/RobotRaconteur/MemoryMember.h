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
#include "RobotRaconteur/Service.h"
#include <stdexcept>

namespace RobotRaconteur
{

	namespace detail
	{
		template <class Y,class U>
		static std::vector<Y> ConvertVectorType(std::vector<U> in)
		{
			std::vector<Y> out(in.size());
			for (size_t i=0; i<in.size(); i++) out[i]=(Y)in[i];
			return out;
		}

		

		template <class Y,class U>
		static RR_SHARED_PTR<RRArray<Y> > ConvertRRArrayType(RR_SHARED_PTR<RRArray<U> > in)
		{
			RR_SHARED_PTR<RRArray<Y> > out=AllocateRRArray<Y>(in->size());
			for (size_t i=0; i<in->size(); i++) (*out)[i]=(Y)(*in)[i];
			return out;
		}

		void CalculateMatrixBlocks(uint32_t element_size, std::vector<uint64_t> count, uint64_t max_elems, int32_t &split_dim, uint64_t &split_dim_block, uint64_t &split_elem_count, int32_t &splits_count, int32_t &split_remainder, std::vector<uint64_t>& block_count, std::vector<uint64_t>& block_count_edge);
	}



	class ArrayMemoryBase
	{
	public:
		virtual uint64_t Length()=0;
		virtual DataTypes ElementTypeID()=0;
		virtual ~ArrayMemoryBase() {}

	};

	template <class T>
	class ArrayMemory : public virtual ArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRArray<T> > memory;
		boost::mutex memory_lock;

	public:

		ArrayMemory() {};

		ArrayMemory(RR_SHARED_PTR<RRArray<T> > memory)
		{
			this->memory=memory;
		}

		virtual void Attach(RR_SHARED_PTR<RRArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory=memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->Length();
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos+count > memory->Length()) throw std::out_of_range("Index out of range");
			if (bufferpos+count > buffer->Length()) throw std::out_of_range("Index out of range");

			memcpy(buffer->ptr()+bufferpos,memory->ptr()+memorypos,(size_t)count*sizeof(T));
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos+count > memory->Length()) throw std::out_of_range("Index out of range");
			if (bufferpos+count > buffer->Length()) throw std::out_of_range("Index out of range");
			memcpy(memory->ptr()+memorypos,buffer->ptr()+bufferpos,(size_t)count*sizeof(T));
		}
		
		virtual DataTypes ElementTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();

		}

	};


	class MultiDimArrayMemoryBase
	{
	public:
		virtual std::vector<uint64_t> Dimensions()=0;
		virtual uint64_t DimCount()=0;
		virtual bool Complex()=0;
		virtual DataTypes ElementTypeID()=0;
		virtual ~MultiDimArrayMemoryBase() {}
	};


	template <class T>
	class MultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		MultiDimArrayMemory() {};

		MultiDimArrayMemory(RR_SHARED_PTR<RRMultiDimArray<T> > multimemory)
		{
			this->multimemory =multimemory;
		}

		virtual ~MultiDimArrayMemory() {}

		virtual void Attach(RR_SHARED_PTR<RRMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory =multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			RR_SHARED_PTR<RRArray<int32_t> > dims=multimemory->Dims;
			std::vector<uint64_t> s(dims->Length());
			for (size_t i=0; i<s.size(); i++)
			{
				s[i]=(*dims)[i];
			}

			return s;

		}

		virtual uint64_t DimCount()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->DimCount;
		}

		virtual bool Complex()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->Complex;
		}

		virtual void Read(std::vector<uint64_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<int32_t>(memorypos),buffer,detail::ConvertVectorType<int32_t>(bufferpos),detail::ConvertVectorType<int32_t>(count));
		}

		virtual void Write(std::vector<uint64_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<int32_t>(memorypos),buffer,detail::ConvertVectorType<int32_t>(bufferpos),detail::ConvertVectorType<int32_t>(count));
		}

		virtual DataTypes ElementTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();

		}
	};

	
	template<typename T>
	class ArrayMemoryServiceSkel
	{

	private:
		std::string m_MemberName;
		RR_WEAK_PTR<ServiceSkel> skel;
		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode()
		{
			RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
			if (!n) throw std::runtime_error("Node has been released");
			return n;
		}

		std::string GetMemberName() const
		{
			return m_MemberName;
		}

		ArrayMemoryServiceSkel(std::string membername, RR_SHARED_PTR<ServiceSkel> skel)
		{
			this->m_MemberName = membername;
			this->skel = skel;
			this->node=skel->RRGetNode();
		}

		virtual ~ArrayMemoryServiceSkel() {}

		virtual RR_SHARED_PTR<MessageEntry> CallMemoryFunction(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<ArrayMemory<T> > mem)
		{

			switch (m->EntryType)
			{
				case MessageEntryType_MemoryRead:
				{

						uint64_t memorypos = RRArrayToScalar(m->FindElement("memorypos")->CastData<RRArray<uint64_t> >());
						uint64_t count = RRArrayToScalar(m->FindElement("count")->CastData<RRArray<uint64_t> >());
						RR_SHARED_PTR<RRArray<T> > data = AllocateRRArray<T>((size_t)count);
						mem->Read(memorypos, data, 0, (size_t)count);
						RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryReadRet, GetMemberName());
						ret->AddElement("memorypos", ScalarToRRArray(memorypos));
						ret->AddElement("count", ScalarToRRArray(count));
						ret->AddElement("data", data);
						return ret;

				}
				case MessageEntryType_MemoryWrite:
				{

						uint64_t memorypos = RRArrayToScalar(m->FindElement("memorypos")->CastData<RRArray<uint64_t> >());
						uint64_t count = RRArrayToScalar(m->FindElement("count")->CastData<RRArray<uint64_t> >());
						RR_SHARED_PTR<RRArray<T> > data = m->FindElement("data")->CastData<RRArray<T> >();
						mem->Write(memorypos, data, 0, (size_t)count);
						RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryReadRet, GetMemberName());
						ret->AddElement("memorypos", ScalarToRRArray(memorypos));
						ret->AddElement("count", ScalarToRRArray(count));
						return ret;

				}
				case MessageEntryType_MemoryGetParam:
				{
						std::string param = m->FindElement("parameter")->CastDataToString();
						if (param == "Length")
						{
									RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParamRet, GetMemberName());
									ret->AddElement("return", ScalarToRRArray(mem->Length()));
									return ret;

						}
						else if (param == "MaxTransferSize")
						{
									RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParamRet, GetMemberName());
									uint32_t MaxTransferSize = GetNode()->GetMemoryMaxTransferSize();
									ret->AddElement("return", ScalarToRRArray(MaxTransferSize));
									return ret;

						}
						else
						{
								throw std::runtime_error("Unknown parameter");
						}
				}
				default:
					throw ProtocolException("Invalid command");



			}
		}



	};


	template<typename T>
	class MultiDimArrayMemoryServiceSkel
	{

	private:
		std::string m_MemberName;
		RR_WEAK_PTR<ServiceSkel> skel;
		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode()
		{
			RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
			if (!n) throw std::runtime_error("Node has been released");
			return n;
		}

		std::string GetMemberName() const
		{
			return m_MemberName;
		}

		MultiDimArrayMemoryServiceSkel(std::string membername, RR_SHARED_PTR<ServiceSkel> skel)
		{
			this->m_MemberName = membername;
			this->skel = skel;
			this->node=skel->RRGetNode();
		}

		virtual ~MultiDimArrayMemoryServiceSkel() {}

		virtual RR_SHARED_PTR<MessageEntry> CallMemoryFunction(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<MultiDimArrayMemory<T> > mem)
		{

			switch (m->EntryType)
			{
				case MessageEntryType_MemoryRead:
				{

						RR_SHARED_PTR<RRArray<uint64_t> > memorypos = m->FindElement("memorypos")->CastData<RRArray<uint64_t> >();
						RR_SHARED_PTR<RRArray<uint64_t> > count = m->FindElement("count")->CastData<RRArray<uint64_t> >();
						int32_t elemcount = 1;
						for (size_t i = 0; i < count->size(); i++)
							elemcount *= static_cast<int32_t>((*count)[i]);
						RR_SHARED_PTR<RRArray<T> > real = AllocateRRArray<T>((size_t)elemcount);

						RR_SHARED_PTR<RRMultiDimArray<T> > data;
						if (!mem->Complex())
						{
							data = RR_MAKE_SHARED<RRMultiDimArray<T> >(detail::ConvertRRArrayType<int32_t>(count), real);
						}
						else
						{
							RR_SHARED_PTR<RRArray<T> > imag = AllocateRRArray<T>((size_t)elemcount);
							data = RR_MAKE_SHARED<RRMultiDimArray<T> >(detail::ConvertRRArrayType<int32_t>(count), real, imag);
						}
						mem->Read(RRArrayToVector<uint64_t>(memorypos), data, std::vector<uint64_t>(count->size()), RRArrayToVector<uint64_t>(count));
						RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryReadRet, GetMemberName());
						ret->AddElement("memorypos", memorypos);
						ret->AddElement("count", memorypos);
						ret->AddElement("data", GetNode()->PackMultiDimArray(data));
						return ret;

				}
				case MessageEntryType_MemoryWrite:
				{

						RR_SHARED_PTR<RRArray<uint64_t> > memorypos = m->FindElement("memorypos")->CastData<RRArray<uint64_t> >();
						RR_SHARED_PTR<RRArray<uint64_t> > count = m->FindElement("count")->CastData<RRArray<uint64_t> >();
						int32_t elemcount = 1;
						for (size_t i = 0; i < count->size(); i++)
							elemcount *= static_cast<int32_t>((*count)[i]);
						
						RR_SHARED_PTR<RRMultiDimArray<T> > data =GetNode()->template UnpackMultiDimArray<T>(m->FindElement("data")->CastData<MessageElementMultiDimArray>());
						

						mem->Write(RRArrayToVector<uint64_t>(memorypos), data, std::vector<uint64_t>(count->size()), RRArrayToVector<uint64_t>(count));
						RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryReadRet, GetMemberName());
						ret->AddElement("memorypos", memorypos);
						ret->AddElement("count", count);

						return ret;

				}
				case MessageEntryType_MemoryGetParam:
				{
						std::string param = m->FindElement("parameter")->CastDataToString();
						


						if (param == "Dimensions")
						{
									RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParamRet, GetMemberName());
									ret->AddElement("return", VectorToRRArray<uint64_t>(mem->Dimensions()));
									return ret;
						}

						else if (param == "DimCount")
						{
									RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParamRet, GetMemberName());
									ret->AddElement("return", ScalarToRRArray(mem->DimCount()));
									return ret;
						}

						else if (param == "Complex")
						{
									RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParamRet, GetMemberName());
									int32_t complex = static_cast<int32_t>(mem->Complex() ? 1 : 0);
									ret->AddElement("return", ScalarToRRArray((int32_t)(complex? 1 : 0)));
									return ret;
						}

						else if (param == "MaxTransferSize")
						{
									RR_SHARED_PTR<MessageEntry> ret = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParamRet, GetMemberName());
									uint32_t MaxTransferSize = GetNode()->GetMemoryMaxTransferSize();
									ret->AddElement("return", ScalarToRRArray(MaxTransferSize));
									return ret;

						}
						else
						{
								throw std::runtime_error("Unknown parameter");
						}
				}
				default:
					throw ProtocolException("Invalid command");



			}
		}



	};

	
	template<typename T>
	class ArrayMemoryClient : public virtual ArrayMemory<T>
	{


	private:
		std::string m_MemberName;
		RR_WEAK_PTR<ServiceStub> stub;
		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:
		const std::string GetMemberName() const
		{
			return m_MemberName;
		}

		RR_SHARED_PTR<RobotRaconteurNode> GetNode()
		{
			RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
			if (!n) throw std::runtime_error("Node has been released");
			return n;
		}

		ArrayMemoryClient(std::string membername, RR_SHARED_PTR<ServiceStub> stub)
		{
			InitializeInstanceFields();
			this->stub = stub;
			this->node=stub->RRGetNode();
			m_MemberName = membername;
			
		}

		virtual ~ArrayMemoryClient() {}

		RR_SHARED_PTR<ServiceStub> GetStub()
		{
			RR_SHARED_PTR<ServiceStub> out=stub.lock();
			if (!out) throw std::runtime_error("Memory client has been closed");
			return out;
		}


		virtual void Attach(RR_SHARED_PTR<RRArray<T> > memory)
		{
			throw std::runtime_error("Invalid for client");
		}

		virtual uint64_t Length()
		{
			
			RR_SHARED_PTR<MessageEntry> m = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParam, GetMemberName());
			m->AddElement("parameter", stringToRRArray("Length"));
			RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(m);
			return RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint64_t> >());
		}



	private:
		bool max_size_read;
		uint32_t remote_max_size;
		boost::mutex max_size_lock;

		uint32_t GetMaxTransferSize()
		{
			
			{
				
				
				boost::mutex::scoped_lock lock (max_size_lock);
				if (!max_size_read)
				{
					RR_SHARED_PTR<MessageEntry> m = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParam, GetMemberName());
					m->AddElement("parameter", stringToRRArray("MaxTransferSize"));
					RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(m);
					remote_max_size = RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint32_t> >());
				}
				uint32_t my_max_size = GetNode()->GetMemoryMaxTransferSize();
				if (remote_max_size > my_max_size)
					return my_max_size;
				else
					return remote_max_size;
			}

		}

	public:
		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			uint32_t max_transfer_size = GetMaxTransferSize();

			
			DataTypes t = RRPrimUtil<T>::GetTypeID();

			uint32_t size = (size_t)RRArrayElementSize(t);
			uint32_t max_elems = static_cast<uint32_t>(max_transfer_size) / size;



			if (count <= max_elems)
			{
				
				//Transfer all data in one block
				RR_SHARED_PTR<MessageEntry> e = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryRead, GetMemberName());
				e->AddElement("memorypos", ScalarToRRArray(memorypos));
				e->AddElement("count", ScalarToRRArray(count));
				RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(e);
				RR_SHARED_PTR<RRArray<T> > data = ret->FindElement("data")->CastData<RRArray<T> >();
				//Array::Copy(static_cast<Array*>(data), static_cast<int64_t int64_t>(0), static_cast<Array*>(buffer), static_cast<int64_t int64_t>(bufferpos), static_cast<int64_t int64_t>(count));
				memcpy(buffer->ptr()+bufferpos,data->ptr(),(size_t)count*sizeof(T));


			}
			else
			{
				uint64_t blocks = count / static_cast<uint64_t>(max_elems);
				uint64_t blockrem = count % static_cast<uint64_t>(max_elems);

				for (uint64_t i = 0; i < blocks; i++)
				{
					uint64_t bufferpos_i = bufferpos + max_elems * i;
					uint64_t memorypos_i = memorypos + max_elems * i;

					Read(memorypos_i, buffer, bufferpos_i, max_elems);

				}

				if (blockrem > 0)
				{
					uint64_t bufferpos_i = bufferpos + max_elems * blocks;
					uint64_t memorypos_i = memorypos + max_elems * blocks;

					Read(memorypos_i, buffer, bufferpos_i, blockrem);
				}

			}


		}



		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{

			uint32_t max_transfer_size = GetMaxTransferSize();


			DataTypes t = RRPrimUtil<T>::GetTypeID();

			uint32_t size = (size_t)RRArrayElementSize(t);
			uint32_t max_elems = static_cast<uint32_t>(max_transfer_size) / size;



			if (count <= max_elems)
			{
				
				//Transfer all data in one block
				RR_SHARED_PTR<MessageEntry> e = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryWrite, GetMemberName());
				e->AddElement("memorypos", ScalarToRRArray(memorypos));
				e->AddElement("count", ScalarToRRArray(count));

				if (bufferpos == 0 && buffer->size() == static_cast<size_t>(count))
				{
					e->AddElement("data", buffer);
				}
				else if ((buffer->size() - static_cast<size_t>(bufferpos)) >= static_cast<size_t>(count))
				{
					RR_SHARED_PTR<RRArray<T> > data = AllocateRRArray<T>((size_t)count);
					//Array::Copy(buffer, static_cast<int64_t int64_t>(bufferpos), data, static_cast<int64_t int64_t>(0), static_cast<int64_t int64_t>(count));
					memcpy(data->ptr(),buffer->ptr()+bufferpos,(size_t)count*sizeof(T));
					e->AddElement("data", data);

				}
				else
					throw std::out_of_range("");

				RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(e);


			}
			else
			{
				if ((buffer->size() - static_cast<int64_t>(bufferpos)) < static_cast<int64_t>(count))
					throw std::out_of_range("");

				uint64_t blocks = count / static_cast<uint64_t>(max_elems);
				uint64_t blockrem = count % static_cast<uint64_t>(max_elems);

				for (uint64_t i = 0; i < blocks; i++)
				{
					uint64_t bufferpos_i = bufferpos + max_elems * i;
					uint64_t memorypos_i = memorypos + max_elems * i;

					Write(memorypos_i, buffer, bufferpos_i, max_elems);

				}

				if (blockrem > 0)
				{
					uint64_t  bufferpos_i = bufferpos + max_elems * blocks;
					uint64_t  memorypos_i = memorypos + max_elems * blocks;

					Write(memorypos_i, buffer, bufferpos_i, blockrem);
				}
			}
		}




		void Shutdown()
		{
			
			
			//stub.reset();
		}

		
	private:
		void InitializeInstanceFields()
		{
			max_size_read = false;
			remote_max_size = 0;
			
		}
	};

	
	template <class T>
	class MultiDimArrayMemoryClient : public virtual MultiDimArrayMemory<T>
	{
	private:
		std::string m_MemberName;
		RR_WEAK_PTR<ServiceStub> stub;
		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode()
		{
			RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
			if (!n) throw std::runtime_error("Node has been released");
			return n;
		}

		const std::string GetMemberName() const
		{
			return m_MemberName;
		}

		MultiDimArrayMemoryClient(const std::string &membername, RR_SHARED_PTR<ServiceStub> stub)
		{

			InitializeInstanceFields();
			this->stub = stub;
			this->node=stub->RRGetNode();
			m_MemberName = membername;
		}

		virtual ~MultiDimArrayMemoryClient() {}

		RR_SHARED_PTR<ServiceStub> GetStub()
		{
			RR_SHARED_PTR<ServiceStub> out=stub.lock();
			if (!out) throw std::runtime_error("Memory client has been closed");
			return out;
		}

		virtual void Attach(RR_SHARED_PTR<RRMultiDimArray<T> > memory)
		{
			throw std::runtime_error("Not valid for client");
		}



		virtual std::vector<uint64_t> Dimensions() 
		{
			
			RR_SHARED_PTR<MessageEntry> m = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParam, GetMemberName());
			m->AddElement("parameter", stringToRRArray("Dimensions"));
			RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(m);
			return RRArrayToVector<uint64_t>(ret->FindElement("return")->CastData<RRArray<uint64_t> >());
		}

		virtual uint64_t DimCount() 
		{
			
			RR_SHARED_PTR<MessageEntry> m = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParam, GetMemberName());
			m->AddElement("parameter", stringToRRArray("DimCount"));
			RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(m);
			return RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint64_t> >());
		}

		virtual bool Complex() 
		{
			
			RR_SHARED_PTR<MessageEntry> m = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParam, GetMemberName());
			m->AddElement("parameter", stringToRRArray("Complex"));
			RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(m);
			return RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<int32_t> >()) != 0;
		}

	private:
		bool max_size_read;
		uint32_t remote_max_size;
		boost::mutex max_size_lock;

		uint32_t GetMaxTransferSize()
		{

			
			{
			
				boost::mutex::scoped_lock lock (max_size_lock);
				if (!max_size_read)
				{
					RR_SHARED_PTR<MessageEntry> m = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryGetParam, GetMemberName());
					m->AddElement("parameter", stringToRRArray("MaxTransferSize"));
					RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(m);
					remote_max_size = RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint32_t> >());
				}
				uint32_t my_max_size = GetNode()->GetMemoryMaxTransferSize();
				if (remote_max_size > my_max_size)
					return my_max_size;
				else
					return remote_max_size;
			}

		}



	public:
		virtual void Read(std::vector<uint64_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count)
		{
			uint32_t max_transfer_size = GetMaxTransferSize();

			DataTypes t = RRPrimUtil<T>::GetTypeID();

			uint32_t size = (size_t)RRArrayElementSize(t);
			uint64_t elemcount = 1;
			for (size_t i = 0; i < count.size(); i++)
				elemcount *= count[i];
			uint32_t max_elems = static_cast<uint32_t>(max_transfer_size) / size;



			if (elemcount <= max_elems)
			{
				
				//Transfer all data in one block
				RR_SHARED_PTR<MessageEntry> e = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryRead, GetMemberName());
				e->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
				e->AddElement("count", VectorToRRArray<uint64_t>(count));
				RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(e);

				RR_SHARED_PTR<RRMultiDimArray<T> > data = GetNode()->template UnpackMultiDimArray<T>(ret->FindElement("data")->CastData<MessageElementMultiDimArray>());

				RR_SHARED_PTR<MultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<MultiDimArrayMemory<T> >(data);
				data2->Read(std::vector<uint64_t>(count.size()), buffer, bufferpos, count);


			}
			else
			{
				//We need to read the array in chunks.  This is a little complicated...

				int32_t split_dim;
				uint64_t split_dim_block;
				uint64_t split_elem_count;
				int32_t splits_count;
				int32_t split_remainder;
				std::vector<uint64_t> block_count;
				std::vector<uint64_t> block_count_edge;

				detail::CalculateMatrixBlocks(size, count, max_elems, split_dim, split_dim_block, split_elem_count, splits_count, split_remainder, block_count, block_count_edge);

				bool done = false;
				std::vector<uint64_t> current_pos = std::vector<uint64_t>(count.size());

				while (!done)
				{





					for (int32_t i = 0; i < splits_count; i++)
					{
						current_pos[split_dim] = split_dim_block * static_cast<uint64_t>(i);

						std::vector<uint64_t> current_buf_pos = std::vector<uint64_t>(bufferpos.size());
						std::vector<uint64_t> current_mem_pos = std::vector<uint64_t>(bufferpos.size());

						for (size_t j = 0; j < current_buf_pos.size(); j++)
						{
							current_buf_pos[j] = current_pos[j] + bufferpos[j];
							current_mem_pos[j] = current_pos[j] + memorypos[j];
						}

						Read(current_mem_pos, buffer, current_buf_pos, block_count);

					}

					if (split_remainder != 0)
					{
						current_pos[split_dim] = split_dim_block * static_cast<uint64_t>(splits_count);
						std::vector<uint64_t> current_buf_pos = std::vector<uint64_t>(bufferpos.size());
						std::vector<uint64_t> current_mem_pos = std::vector<uint64_t>(bufferpos.size());

						for (size_t j = 0; j < current_buf_pos.size(); j++)
						{
							current_buf_pos[j] = current_pos[j] + bufferpos[j];
							current_mem_pos[j] = current_pos[j] + memorypos[j];
						}

						Read(current_mem_pos, buffer, current_buf_pos, block_count_edge);
					}

					if (split_dim == (int32_t)(count.size() - 1))
					{
						done = true;
					}
					else
					{
						current_pos[split_dim + 1]++;
						if (current_pos[split_dim + 1] >= count[split_dim + 1])
						{
							if (split_dim + 1 == (int32_t)(count.size() - 1))
							{
								done = true;
							}
							else
							{
								current_pos[split_dim + 1] = 0;
								for (size_t j = split_dim + 2; j < count.size(); j++)
								{
									if (current_pos[j - 1] >= count[j - 1])
									{
										current_pos[j]++;
									}
								}
								if (current_pos[count.size() - 1] >= count[count.size() - 1])
									done = true;

							}

						}
					}


				}
			}
		}

	

	public:
		virtual void Write(std::vector<uint64_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<uint64_t> bufferpos, std::vector<uint64_t> count)
		{
			uint32_t max_transfer_size = GetMaxTransferSize();

			DataTypes t = RRPrimUtil<T>::GetTypeID();

			uint32_t size = (size_t)RRArrayElementSize(t);
			uint64_t elemcount = 1;
			for (size_t i = 0; i < count.size(); i++)
				elemcount *= count[i];
			uint32_t max_elems = static_cast<uint32_t>(max_transfer_size) / size;



			if (elemcount <= max_elems)
			{
				
				//Transfer all data in one block
				RR_SHARED_PTR<MessageEntry> e = RR_MAKE_SHARED<MessageEntry>(MessageEntryType_MemoryWrite, GetMemberName());
				e->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
				e->AddElement("count", VectorToRRArray<uint64_t>(count));

				bool equ = true;
				for (size_t i = 0; i < count.size(); i++)
				{
					if (bufferpos[i] != 0 || static_cast<uint64_t>((*buffer->Dims)[i]) != count[i])
					{
						equ = false;
						break;
					}
				}

				if (equ)
				{
					e->AddElement("data", GetNode()->PackMultiDimArray(buffer));
				}
				else
				{
					RR_SHARED_PTR<RRMultiDimArray<T> > data;
					if (!buffer->Complex)
					{
						data = RR_MAKE_SHARED<RRMultiDimArray<T> >(VectorToRRArray<int32_t>(count), AllocateRRArray<T>((size_t)elemcount));
					}
					else
					{
						data = RR_MAKE_SHARED<RRMultiDimArray<T> >(VectorToRRArray<int32_t>(count), AllocateRRArray<T>((size_t)elemcount), AllocateRRArray<T>((size_t)elemcount));
					}


					buffer->RetrieveSubArray(detail::ConvertVectorType<int32_t>(bufferpos), data, std::vector<int32_t>(count.size()), detail::ConvertVectorType<int32_t>(count));
					e->AddElement("data", GetNode()->PackMultiDimArray(data));

				}


				RR_SHARED_PTR<MessageEntry> ret = GetStub()->ProcessTransaction(e);

			}
			else
			{
				int32_t split_dim;
				uint64_t split_dim_block;
				uint64_t split_elem_count;
				int32_t splits_count;
				int32_t split_remainder;
				std::vector<uint64_t> block_count;
				std::vector<uint64_t> block_count_edge;

				detail::CalculateMatrixBlocks(size, count, max_elems, split_dim, split_dim_block, split_elem_count, splits_count, split_remainder, block_count, block_count_edge);

				bool done = false;
				std::vector<uint64_t> current_pos = std::vector<uint64_t>(count.size());

				while (!done)
				{





					for (int32_t i = 0; i < splits_count; i++)
					{
						current_pos[split_dim] = split_dim_block * static_cast<uint64_t>(i);

						std::vector<uint64_t> current_buf_pos = std::vector<uint64_t>(bufferpos.size());
						std::vector<uint64_t> current_mem_pos = std::vector<uint64_t>(bufferpos.size());

						for (size_t j = 0; j < current_buf_pos.size(); j++)
						{
							current_buf_pos[j] = current_pos[j] + bufferpos[j];
							current_mem_pos[j] = current_pos[j] + memorypos[j];
						}

						Write(current_mem_pos, buffer, current_buf_pos, block_count);

					}

					if (split_remainder != 0)
					{
						current_pos[split_dim] = split_dim_block * static_cast<uint64_t>(splits_count);
						std::vector<uint64_t> current_buf_pos(bufferpos.size());
						std::vector<uint64_t> current_mem_pos(bufferpos.size());

						for (size_t j = 0; j < current_buf_pos.size(); j++)
						{
							current_buf_pos[j] = current_pos[j] + bufferpos[j];
							current_mem_pos[j] = current_pos[j] + memorypos[j];
						}

						Write(current_mem_pos, buffer, current_buf_pos, block_count_edge);
					}

					if (split_dim == (int32_t)(count.size() - 1))
					{
						done = true;
					}
					else
					{
						current_pos[split_dim + 1]++;
						if (current_pos[split_dim + 1] >= count[split_dim + 1])
						{
							if (split_dim + 1 == (int32_t)(count.size() - 1))
							{
								done = true;
							}
							else
							{
								current_pos[split_dim + 1] = 0;
								for (size_t j = split_dim + 2; j < count.size(); j++)
								{
									if (current_pos[j - 1] >= count[j - 1])
									{
										current_pos[j]++;
									}
								}
								if (current_pos[count.size() - 1] >= count[count.size() - 1])
									done = true;

							}

						}
					}


				}
			}
		}



		void Shutdown()
		{
			//stub.reset();
		}

	private:
		void InitializeInstanceFields()
		{
			max_size_read = false;
			remote_max_size = 0;
			
		}
	};
	



}
