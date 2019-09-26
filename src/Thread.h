
#ifndef EZ_THREAD_H
#define EZ_THREAD_H
#include <string>
#include <functional>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#pragma warning(push)
#pragma warning(disable : 4251)
#include "atomic.h"
#else
#include <stdarg.h>
#include <pthread.h>
#include <arpa/inet.h>
#endif

#include "NoCopyable.h"
#include "pthread.h"


namespace Ez
{



class Thread:public NoCopyable
{
	public:
		typedef std::function<void ()> ThreadFunc;

		explicit Thread(const ThreadFunc, const std::string& name = std::string());
		~Thread();

		void start();
		int join(); // return pthread_join()

		bool started() const { return started_; }
		 pthread_t pthreadId() const { return pthreadId_; }
		unsigned int tid() const { return tid_; }
		const std::string& name() const { return name_; }

		static int numCreated() { return numCreated_.get(); }
		//static std::string threadName(uint32_t tid){;}
	private:
		// No copying allowed
		Thread(const Thread&);
		void operator=(const Thread&);

	private:
		
		void setDefaultName();

		bool       started_;
		bool       joined_;
		pthread_t pthreadId_;
		pid_t      tid_;
		ThreadFunc func_;
		std::string     name_;

		//CountDownLatch latch_;


		static void * threadProc(void * pParam);


		static AtomicInt32 numCreated_;
		//static std::map<uint32_t,std::string> thread_names_;
};


}
#endif