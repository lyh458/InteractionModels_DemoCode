
#include <boost/asio.hpp>
#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{
	class RobotRaconteurNode;

	class ThreadPool : public RR_ENABLE_SHARED_FROM_THIS<ThreadPool>
	{

	protected:
		std::vector<RR_SHARED_PTR<boost::thread> > threads;

		boost::mutex queue_mutex;

		boost::asio::io_service _io_service;

		size_t thread_count;

		bool keepgoing;
		boost::mutex keepgoing_lock;

		RR_SHARED_PTR<boost::asio::io_service::work> _work;

		RR_WEAK_PTR<RobotRaconteurNode> node;

	public:

		ThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		virtual size_t GetThreadPoolCount();

		virtual void SetThreadPoolCount(size_t count);

		virtual void Post(boost::function<void()> function);

		virtual void Shutdown();

		virtual boost::asio::io_service& get_io_service();


	protected:
		virtual void start_new_thread();

		virtual void thread_function();



	};

	class ThreadPoolFactory
	{
	public: 
		virtual RR_SHARED_PTR<ThreadPool> NewThreadPool(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			return RR_MAKE_SHARED<ThreadPool>(node);
		}
	};
}