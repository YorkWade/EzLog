#ifndef EZ_LOG_ASYNC_LOGGER_H
#define EZ_LOG_ASYNC_LOGGER_H


#include "condition.h"
#include "count_down_latch.h"
#include "LogStream.h"
#include "Mutex.h"
#include "NoCopyable.h"
#include "Thread.h"
#include <string>
#include <vector>
#include <memory>

namespace Ez
{

class AsyncLogger:public NoCopyable
{

public:

	AsyncLogger(const std::string& basename,
		off_t rollSize,
		int flushInterval = 3);

	~AsyncLogger();
	

	void append(const char* logline, int len);

	void start()
	{
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop()
	{
		running_ = false;
		cond_.notify();
		thread_.join();
	}

private:

	void threadFunc();

	typedef FixedBuffer<kLargeBuffer> Buffer;
	typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
	typedef BufferVector::value_type BufferPtr;

	const int flushInterval_;
	bool running_;
	const std::string basename_;
	const off_t rollSize_;
	Thread thread_;
	CountDownLatch latch_;
	MutexLock mutex_;
	Condition cond_;
 	BufferPtr currentBuffer_;
 	BufferPtr nextBuffer_;
 	BufferVector buffers_;
};

}

#endif