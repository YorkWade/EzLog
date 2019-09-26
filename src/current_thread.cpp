#include "current_thread.h"
#include <stdio.h>


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


}