#include <boost/asio.hpp>
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ErrorUtil.h"
#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/AutoResetEvent.h"

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN
#include <boost/asio/spawn.hpp>
#endif

#pragma once

template<typename T>
void MyPost(T f);

namespace RobotRaconteur
{
	namespace detail
	{
		template<typename T>
		class sync_async_handler
		{
		public:
			RR_SHARED_PTR<AutoResetEvent> ev;
			RR_SHARED_PTR<RobotRaconteurException> err;
			RR_SHARED_PTR<T> data;

			sync_async_handler()
			{
				ev=RR_MAKE_SHARED<AutoResetEvent>();
			}

			sync_async_handler(RR_SHARED_PTR<RobotRaconteurException> err)
			{
				ev=RR_MAKE_SHARED<AutoResetEvent>();
				this->err=err;
			}

			void operator() ()
			{
				
				ev->Set();
			}

			void operator() (RR_SHARED_PTR<RobotRaconteurException> err)
			{
				this->err=err;
				ev->Set();
			}

			void operator() (RR_SHARED_PTR<T> data, RR_SHARED_PTR<RobotRaconteurException> err)
			{
				this->err=err;
				this->data=data;
				ev->Set();
			}

			RR_SHARED_PTR<T> end()
			{
				ev->WaitOne();

				if (err)
				{
					RobotRaconteurExceptionUtil::DownCastAndThrowException(err);
				}

				if (!data) throw std::runtime_error("Internal async error");

				return data;
			}

			void end_void()
			{
				ev->WaitOne();

				if (err)
				{
					RobotRaconteurExceptionUtil::DownCastAndThrowException(err);
				}

			}

		};
		

		RR_SHARED_PTR<Timer> async_timeout_wrapper_CreateTimer(RR_SHARED_PTR<RobotRaconteurNode> node, const boost::posix_time::time_duration& period, boost::function<void(const TimerEvent&)> handler, bool oneshot);

		template<typename T>
		void async_timeout_wrapper_closer(RR_SHARED_PTR<T> d) 
		{
			try
			{
				d->Close();
			}
			catch (std::exception&) {}
		}

		template<typename T, typename T2>
		void async_timeout_wrapper_closer(RR_SHARED_PTR<T> d) 
		{
			try
			{
				RR_SHARED_PTR<T2> t2=RR_DYNAMIC_POINTER_CAST<T2>(d);
				if (!t2) return;
				t2->Close();
			}
			catch (std::exception&) {}
		}

		template<typename T>
		class async_timeout_wrapper : public RR_ENABLE_SHARED_FROM_THIS<async_timeout_wrapper<T> >
		{
		private:
			boost::function<void (RR_SHARED_PTR<T>,RR_SHARED_PTR<RobotRaconteurException>)> handler_;
			RR_SHARED_PTR<Timer> timeout_timer_;
			boost::mutex handled_lock;
			bool handled;
			RR_SHARED_PTR<RobotRaconteurException> timeout_exception_;
			boost::function<void(RR_SHARED_PTR<T>)> deleter_;
			RR_WEAK_PTR<RobotRaconteurNode> node;

		public:

			async_timeout_wrapper(RR_SHARED_PTR<RobotRaconteurNode> node, boost::function<void (RR_SHARED_PTR<T>,RR_SHARED_PTR<RobotRaconteurException>)> handler, boost::function<void(RR_SHARED_PTR<T>)> deleter=0)
			{
				handler_=handler;
				
				handled=false;
				deleter_=deleter;
				this->node=node;
				
				

			}

			void start_timer(int32_t timeout,  RR_SHARED_PTR<RobotRaconteurException> timeout_exception=RR_MAKE_SHARED<ConnectionException>("Timeout during operation"));

			void operator() (RR_SHARED_PTR<T> data, RR_SHARED_PTR<RobotRaconteurException> err)
			{
				{
					boost::mutex::scoped_lock lock(handled_lock);
					if (handled)
					{
						if (data && deleter_) deleter_(data);
						return;
					}
					handled=true;
					
					try
					{
					if (timeout_timer_) timeout_timer_->Stop();
					}
					catch (std::exception&) {}
					timeout_timer_.reset();
				}

				handler_(data,err);

			}

			void handle_error(RR_SHARED_PTR<RobotRaconteurException> err)
			{
				{
					boost::mutex::scoped_lock lock(handled_lock);
					if (handled) return;
					handled=true;

					{
						
						try
						{
						if (timeout_timer_) timeout_timer_->Stop();
						}
						catch (std::exception&) {}
						timeout_timer_.reset();
					}
				}

				handler_(RR_SHARED_PTR<T>(),err);
			}

			void handle_error(const boost::system::error_code& err)
			{				
				if (err.value()==boost::system::errc::timed_out) handle_error(timeout_exception_);
				handle_error(RR_MAKE_SHARED<ConnectionException>(err.message()));
			}


			

		private:
			void timeout_handler(const TimerEvent& /*e*/)
			{
				{
					boost::mutex::scoped_lock lock(handled_lock);
					if (handled) return;
					handled=true;
				//	timeout_timer_.reset();
				
					
					timeout_timer_.reset();
				}


				handler_(RR_SHARED_PTR<T>(),timeout_exception_);

			}


			
		};

		template<typename T>
		void async_timeout_wrapper<T>::start_timer(int32_t timeout,  RR_SHARED_PTR<RobotRaconteurException> timeout_exception)
		{
			RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
			if (!n) std::runtime_error("Node has been released");


			boost::mutex::scoped_lock lock(handled_lock);

			if (handled) return;

			if (timeout!=RR_TIMEOUT_INFINITE)
			{
				timeout_timer_=async_timeout_wrapper_CreateTimer(n,boost::posix_time::milliseconds(timeout),boost::bind(&async_timeout_wrapper<T>::timeout_handler,this->shared_from_this(),_1),true);
				timeout_timer_->Start();
				timeout_exception_=timeout_exception;
			}
		}
	

#ifdef ROBOTRACONTEUR_USE_ASIO_SPAWN

		

		template<typename ReturnType, typename Handler>
		class async_spawn_wrapper_handler 
		{
		private:
			async_spawn_wrapper_handler(async_spawn_wrapper_handler& in) {}

		public:
			Handler& h_;

			explicit async_spawn_wrapper_handler(Handler& h) : h_(h) {}

			void operator()(ReturnType d, RR_SHARED_PTR<RobotRaconteurException> err)
			{
				boost::tuple<ReturnType, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> > d2=boost::make_tuple(d,err);
				::MyPost(boost::bind(&async_spawn_wrapper_handler<ReturnType,Handler>::handler_invoke, this, d2));
			}

			void operator()(ReturnType d)
			{
				boost::tuple<ReturnType, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> > d2=boost::make_tuple(d,RR_SHARED_PTR<RobotRaconteurException>());
				boost::asio::asio_handler_invoke(boost::bind<void>(h_,d2), boost::addressof(h_)); 
			}

			void handler_invoke(boost::tuple<ReturnType, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> > d2)
			{
				boost::asio::asio_handler_invoke(boost::bind<void>(h_,d2), boost::addressof(h_));
			}

		};

		template<typename ReturnType, typename Handler>
		async_spawn_wrapper_handler<ReturnType,Handler> make_async_spawn_wrapper_handler(Handler h)
		{
			return async_spawn_wrapper_handler<ReturnType,Handler>(h);
		}

		template<typename ReturnType, typename Function>
		ReturnType async_wrap_for_spawn(Function func, const boost::asio::yield_context& yield)
		{
			boost::asio::detail::async_result_init<boost::asio::yield_context, void(boost::tuple<ReturnType,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> >) > init(BOOST_ASIO_MOVE_CAST(boost::asio::yield_context)(yield));

			typedef boost::asio::handler_type<boost::asio::yield_context,void(boost::tuple<ReturnType,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> >)>::type HandlerType;

			async_spawn_wrapper_handler<ReturnType,HandlerType> h(init.handler);
			func(boost::bind(&async_spawn_wrapper_handler<ReturnType,HandlerType>::operator(),&h,_1,_2));
			boost::tuple<ReturnType, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> > ret=init.result.get();
			if (ret.get<1>()) throw *ret.get<1>();
			return ret.get<0>();
		}

		template<typename Handler>
		class async_spawn_wrapper_handler_void
		{
		public:
			Handler h_;

			explicit async_spawn_wrapper_handler_void(Handler h) : h_(h) {}

			void operator()(RR_SHARED_PTR<RobotRaconteurException> err)
			{
			
				boost::asio::asio_handler_invoke(boost::bind<void>(h_,err), boost::addressof(h_)); 
			}

			void operator()()
			{
			
				boost::asio::asio_handler_invoke(boost::bind<void>(h_,RR_MAKE_SHARED<RobotRaconteurException>()), boost::addressof(h_)); 
			}

		};

		template<typename Handler>
		async_spawn_wrapper_handler_void<Handler> make_async_spawn_wrapper_handler_void(Handler h)
		{
			return async_spawn_wrapper_handler_void<Handler>(h);
		}

		template<typename Function>
		void async_wrap_for_spawn_void(Function func, const boost::asio::yield_context& yield)
		{
			boost::asio::detail::async_result_init<boost::asio::yield_context, void(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > init(BOOST_ASIO_MOVE_CAST(boost::asio::yield_context)(yield));
			func(make_async_spawn_wrapper_handler_void(init.handler));
			RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> ret=init.result.get();
			if(ret) throw *ret;
		
		}
#endif
	}




}
