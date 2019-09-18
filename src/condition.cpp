
#include "condition.h"
#include "error.h"
#include "time.h"

// returns true if time out, false otherwise.
bool Condition::waitForSeconds(double seconds)
{
#ifdef WIN32
	MutexLock::UnassignGuard ug(mutex_);
	struct timespec abstime;
	abstime.tv_sec = seconds;
	abstime.tv_nsec = 0;//1000000;
	return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);

#else
	struct timespec abstime;
	// FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
	clock_gettime(CLOCK_REALTIME, &abstime);
	
	abstime.tv_sec = seconds;
	abstime.tv_nsec = 0;

	const int64_t kNanoSecondsPerSecond = 1000000000;
	int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

	abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
	abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

	MutexLock::UnassignGuard ug(mutex_);
	return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
#endif
}