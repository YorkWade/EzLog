#include "async_logger.h"

#include "log_file.h"

AsyncLogger::AsyncLogger(const std::string& basename,
						 off_t rollSize,
						 int flushInterval /*= 3*/)
	:	flushInterval_(flushInterval),
		running_(false),
		basename_(basename),
		rollSize_(rollSize),
		thread_(std::bind(&AsyncLogger::threadFunc, this), "Logging"),
		latch_(1),
		mutex_(),
		cond_(mutex_),
 		currentBuffer_(new Buffer),
 		nextBuffer_(new Buffer),
		buffers_()
{
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}


AsyncLogger::~AsyncLogger(void)
{
	if (running_)
	{
		stop();
	}
}



void AsyncLogger::append(const char* logline, int len)
{
	MutexLockGuard lock(mutex_);
	if (currentBuffer_->avail() > len)
	{
		currentBuffer_->append(logline, len);
	}
	else
	{
		buffers_.push_back(std::move(currentBuffer_));

		if (nextBuffer_)
		{
			currentBuffer_ = std::move(nextBuffer_);
		}
		else
		{
			currentBuffer_.reset(new Buffer); // Rarely happens
		}
		currentBuffer_->append(logline, len);
		cond_.notify();
	}
}


void AsyncLogger::threadFunc()
{
	printf("AsyncLogger::threadFunc ");
	assert(running_ == true);
	latch_.countDown();
	LogFile output(basename_, rollSize_, false);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	while (running_)
	{
		assert(newBuffer1 && newBuffer1->length() == 0);
		assert(newBuffer2 && newBuffer2->length() == 0);
		assert(buffersToWrite.empty());

		{
			MutexLockGuard lock(mutex_);
			if (buffers_.empty())  // unusual usage!
			{
				cond_.waitForSeconds(flushInterval_);
			}
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_ = std::move(newBuffer1);
			buffersToWrite.swap(buffers_);
			if (!nextBuffer_)
			{
				nextBuffer_ = std::move(newBuffer2);
			}
		}

		assert(!buffersToWrite.empty());

		if (buffersToWrite.size() > 25)
		{
			char buf[256];
			_snprintf/*snprintf*/(buf, sizeof buf, "Dropped log messages at , %zd larger buffers\n",
				/*Timestamp::now().toFormattedString().c_str(),*/
				buffersToWrite.size()-2);
			fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
		}

		for (BufferVector::iterator it = buffersToWrite.begin();it != buffersToWrite.end();it++)
		{
			// FIXME: use unbuffered stdio FILE ? or use ::writev ?
			output.append((*it)->data(), (*it)->length());
		}

		if (buffersToWrite.size() > 2)
		{
			// drop non-bzero-ed buffers, avoid trashing
			buffersToWrite.resize(2);
		}

		if (!newBuffer1)
		{
			assert(!buffersToWrite.empty());
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if (!newBuffer2)
		{
			assert(!buffersToWrite.empty());
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}
	output.flush();
}