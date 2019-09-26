#ifndef EZ_TIME_H
#define EZ_TIME_H

#include <Windows.h>
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

static const unsigned __int64 epoch = ((unsigned __int64) 116444736000000000ULL);

/*
 * timezone information is stored outside the kernel so tzp isn't used anymore.
 *
 * Note: this function is not for Win32 high precision timing purpose. See
 * elapsed_time().
 */
int gettimeofday(struct timeval * tp, struct timezone * tzp);

BOOLEAN nanosleep(LONGLONG ns);

#endif