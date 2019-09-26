#include "current_thread.h"
#include <stdio.h>
#include "time.h"

namespace Ez
{



//≥ı ºªØ
namespace CurrentThread
{
	__thread int t_cachedTid = 0;
	__thread char t_tidString[32];
	__thread int t_tidStringLength = 6;
	__thread const char* t_threadName = "unknown";
}

void CurrentThread::cacheTid()
{
	if (t_cachedTid == 0)
	{
		t_cachedTid = gettid();
		t_tidStringLength = _snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
	}
}


void CurrentThread::sleepUsec(int64_t usec)
{
	
#ifdef WIN32
	nanosleep(usec);
#else
	struct timespec ts = { 0, 0 };
	ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
	::nanosleep(&ts, NULL);
#endif
}

}