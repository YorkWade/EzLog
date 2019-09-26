#ifndef EZ_MUTEX_H
#define EZ_MUTEX_H


#include "NoCopyable.h"
#include "current_thread.h"
#include "pthread.h"
#include "Thread.h"
#include <assert.h>


namespace Ez
{

class MutexLock : NoCopyable
{
public:
	MutexLock()
		: holder_(0)
	{
		pthread_mutex_init(&mutex_, NULL);
	}

	~MutexLock()
	{
		assert(holder_ == 0);
		pthread_mutex_destroy(&mutex_);
		
	}

	// must be called when locked, i.e. for assertion
	bool isLockedByThisThread() const
	{
		//return holder_ == CurrentThread::tid();
	}

	void assertLocked() const
	{
		assert(isLockedByThisThread());
	}

	// internal usage

	void lock()
	{
		pthread_mutex_lock(&mutex_);
		assignHolder();
	}

	void unlock()
	{
		unassignHolder();
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t* getPthreadMutex() /* non-const */
	{
		return &mutex_;
	}

private:
	friend class Condition;

	class UnassignGuard : NoCopyable
	{
	public:
		explicit UnassignGuard(MutexLock& owner)
			: owner_(owner)
		{
			owner_.unassignHolder();
		}

		~UnassignGuard()
		{
			owner_.assignHolder();
		}

	private:
		MutexLock& owner_;
	};

	void unassignHolder()
	{
		holder_ = 0;
	}

	void assignHolder()
	{
		holder_ = CurrentThread::tid();
	}

	pthread_mutex_t mutex_;
	pid_t holder_;
};


class MutexLockGuard : NoCopyable
{
public:
	explicit MutexLockGuard(MutexLock& mutex)
		: mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard()
	{
		mutex_.unlock();
	}

private:

	MutexLock& mutex_;
};


}


#endif