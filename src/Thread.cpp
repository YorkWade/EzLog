#include "Thread.h"
#include <assert.h>
#include <memory>
#include <stdint.h>
#include <stdio.h>


#ifdef WIN32
#include <time.h>
#include <process.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <windows.h>			// 
#include <intrin.h>

#define snprintf _snprintf

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma warning(disable : 4996)

#else
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <net/if.h>
#endif


#include "pthread.h"
AtomicInt32 Thread::numCreated_;

Thread::Thread(const Thread::ThreadFunc func, const std::string& name)
	: started_(false),
	joined_(false),
	pthreadId_(0),
	tid_(0),
	func_(std::move(func)),
	name_(name)
	//latch_(1)
{
	setDefaultName();
}


Thread::~Thread()
{
	if (started_ && !joined_)
	{
		pthread_detach(pthreadId_);
	}
}

void Thread::setDefaultName()
{
	int num = numCreated_.incrementAndGet();
	if (name_.empty())
	{
		char buf[32];
		_snprintf(buf, sizeof buf, "Thread%d", num);
		name_ = buf;
	}
}

void Thread::start()
{
	assert(!started_);
	started_ = true;
	int ret = pthread_create(&pthreadId_, NULL, threadProc, (void*)this);
	if (ret != 0)
	{
		printf("log4z: create log4z thread error! \r\n");
		started_ = false;
	}



	// 	assert(!started_);
	// 	started_ = true;
	// 	// FIXME: move(func_)
	// 	detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
	// 	if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
	// 	{
	// 		started_ = false;
	// 		delete data; // or no delete?
	// 		LOG_SYSFATAL << "Failed in pthread_create";
	// 	}
	// 	else
	// 	{
	// 		latch_.wait();
	// 		assert(tid_ > 0);
	// 	}
}

int Thread::join()
{
	assert(started_);
	assert(!joined_);
	joined_ = true;

	if (pthread_join(pthreadId_, NULL) != 0)
	{
		return false;
	}
	return true;
}
#ifdef WIN32

void setThreadName( DWORD tid, const char* threadName) {
	// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
#pragma pack(push, 8)
	struct THREADNAME_INFO {
		DWORD dwType; // Must be 0x1000
		LPCSTR szName; // Pointer to name (in user address space)
		DWORD dwThreadID; // Thread ID (-1 for caller thread)
		DWORD dwFlags; // Reserved for future use; must be zero
	};
	union TNIUnion {
		THREADNAME_INFO tni;
		ULONG_PTR upArray[4];
	};
#pragma pack(pop)

	static DWORD kMSVCException = 0x406D1388;

	// std::thread::id is a thin wrapper around an integral thread id,
	// so just extract the ID.
	unsigned int id;
	std::memcpy(&id, &tid, sizeof(id));

	TNIUnion tniUnion = {0x1000, threadName, id, 0};
	// This has to be in a separate stack frame from trimmedName, which requires
	// C++ object destruction semantics.
	
	__try {
		RaiseException(kMSVCException, 0, 4, tniUnion.upArray);
	} __except (
		GetExceptionCode() == kMSVCException ? EXCEPTION_CONTINUE_EXECUTION
		: EXCEPTION_EXECUTE_HANDLER) {
			// Swallow the exception when a debugger isn't attached.
	}
}




int64_t __sync_fetch_and_add(volatile int64_t* ptr, int64_t value) {
	return InterlockedExchangeAdd64(ptr, value);
}
#else

#endif

void * Thread::threadProc(void * pParam)
{
	Thread * p = static_cast<Thread*>(pParam);
	p->tid_ = gettid();
	setThreadName(p->tid_, p->name().c_str());
	p->func_();
	return 0;
}