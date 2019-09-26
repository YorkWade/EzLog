#include "time_stamp.h"

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

//#include <time.h>
#include "time.h"
#ifdef WIN32
#include <windows.h>
#include "inttypes.h"
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <stdio.h>



//#include <inttypes.h>


#ifdef _MSC_VER
#define	snprintf	sprintf_s
#endif

namespace Ez
{


static_assert(sizeof(Timestamp) == sizeof(int64_t),
	"Timestamp is same size as int64_t");

std::string Timestamp::toString() const
{
	char buf[32] = {0};
	int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
	int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
	char buf[64] = {0};
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	struct tm tm_time;
	//gmtime_r(&seconds, &tm_time);
	gmtime_s(&tm_time, &seconds);

	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
			microseconds);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	return buf;
}

// Timestamp Timestamp::now()
// {
// 	struct timeval tv;
// 	gettimeofday(&tv, NULL);
// 	int64_t seconds = tv.tv_sec;
// 	return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
// }
Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;
	return Timestamp(seconds*Timestamp::kMicroSecondsPerSecond + tv.tv_usec);
}

}