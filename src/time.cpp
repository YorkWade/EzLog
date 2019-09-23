#include "time.h"


int clock_gettime_monotonic(struct timespec *tv)
 {
 	static LARGE_INTEGER ticksPerSec;
 	LARGE_INTEGER ticks;
 	double seconds;
 
 	if (!ticksPerSec.QuadPart) {
 		QueryPerformanceFrequency(&ticksPerSec);
 		if (!ticksPerSec.QuadPart) {
 			//errno = ENOTSUP;
 			return -1;
 		}
 	}
 
 	QueryPerformanceCounter(&ticks);
 
 	seconds = (double) ticks.QuadPart / (double) ticksPerSec.QuadPart;
 	tv->tv_sec = (time_t)seconds;
 	tv->tv_nsec = (long)((ULONGLONG)(seconds * NS_PER_SEC) % NS_PER_SEC);
 
 	return 0;
 }
 
 
 int clock_gettime_realtime(struct timespec *tv)
 {
 	FILETIME ft;
 	ULARGE_INTEGER hnsTime;
 
 	GetSystemTimeAsFileTime(&ft);
 
 	hnsTime.LowPart = ft.dwLowDateTime;
 	hnsTime.HighPart = ft.dwHighDateTime;
 
 	// To get POSIX Epoch as baseline, subtract the number of hns intervals from Jan 1, 1601 to Jan 1, 1970.
 	hnsTime.QuadPart -= (11644473600ULL * HNS_PER_SEC);
 
 	// modulus by hns intervals per second first, then convert to ns, as not to lose resolution
 	tv->tv_nsec = (long) ((hnsTime.QuadPart % HNS_PER_SEC) * NS_PER_HNS);
 	tv->tv_sec = (long) (hnsTime.QuadPart / HNS_PER_SEC);
 
 	return 0;
 }
 
 int clock_gettime(int type,struct timespec *tv)
 {
 	switch(type)
 	{
 	case CLOCK_REALTIME:
 		clock_gettime_realtime(tv);
 		break;
 	case CLOCK_MONOTONIC:
 		clock_gettime_monotonic(tv);
 		break;
 	default:
 		break;
 	}
 	
 	return 0;
 }



int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    FILETIME    file_time;
    SYSTEMTIME  system_time;
    ULARGE_INTEGER ularge;

    GetLocalTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;

    tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);

    return 0;
}
