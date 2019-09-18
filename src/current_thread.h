#ifndef EZ_CURRENT_THREAD_H
#define EZ_CURRENT_THREAD_H
#include "pthread.h"

namespace CurrentThread
{
	// internal
	__thread extern int t_cachedTid;
	__thread extern char t_tidString[32];
	__thread extern int t_tidStringLength;
	__thread extern const char* t_threadName;

	void cacheTid();

	inline int tid()
	{
		if (/*__builtin_expect(*/t_cachedTid == 0/*, 0)*/)
		{
			cacheTid();
		}
		return t_cachedTid;
	}

	inline const char* tidString() // for logging
	{
		return t_tidString;
	}

	inline int tidStringLength() // for logging
	{
		return t_tidStringLength;
	}

	inline const char* name()
	{
		return t_threadName;
	}

	//bool isMainThread();

	//void sleepUsec(int64_t usec);  // for testing

	//std::string stackTrace(bool demangle);
}  // namespace CurrentThread 





#endif