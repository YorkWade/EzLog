#ifndef EZ_TIME_H
#define EZ_TIME_H

#include <Windows.h>
//#include <winbase.h>
#include <time.h>

struct timespec {
	time_t   tv_sec;        /* seconds */
	long     tv_nsec;       /* nanoseconds */
};

 
 #define MS_PER_SEC      1000ULL     // MS = milliseconds
 #define US_PER_MS       1000ULL     // US = microseconds
 #define HNS_PER_US      10ULL       // HNS = hundred-nanoseconds (e.g., 1 hns = 100 ns)
 #define NS_PER_US       1000ULL
 
 #define HNS_PER_SEC     (MS_PER_SEC * US_PER_MS * HNS_PER_US)
 #define NS_PER_HNS      (100ULL)    // NS = nanoseconds
 #define NS_PER_SEC      (MS_PER_SEC * US_PER_MS * NS_PER_US)
 
 
 #define CLOCK_REALTIME	0
 #define CLOCK_MONOTONIC 1
 
 int clock_gettime_monotonic(struct timespec *tv);
 
 
 int clock_gettime_realtime(struct timespec *tv);
 
 int clock_gettime(int type,struct timespec *tv);




// #define CLOCK_REALTIME	0
// #define CLOCK_MONOTONIC 1
// 
// 
// static int clock_gettime(int, struct timespec *spec)      //C-file part
// {
// 	__int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
// 	wintime -= 116444736000000000i64;  //1jan1601 to 1jan1970
// 	spec->tv_sec = wintime / 10000000i64;           //seconds
// 	spec->tv_nsec = wintime % 10000000i64 * 100;      //nano-seconds
// 	return 0;
// }



// #define WIN32_LEAN_AND_MEAN
// #include <Windows.h>
// #include <stdint.h> // portable: uint64_t   MSVC: __int64 



 /* FILETIME of Jan 1 1970 00:00:00. */
static const unsigned __int64 epoch = ((unsigned __int64) 116444736000000000ULL);

/*
 * timezone information is stored outside the kernel so tzp isn't used anymore.
 *
 * Note: this function is not for Win32 high precision timing purpose. See
 * elapsed_time().
 */
int gettimeofday(struct timeval * tp, struct timezone * tzp);



#endif