#ifndef EZ_ATOMIC_H
#define EZ_ATOMIC_H

#include "NoCopyable.h"
#include <stdint.h>
#include <winnt.h>

#ifdef WIN32
class AtomicInt32 : NoCopyable
{
public:
	AtomicInt32()
		: value_(0)
	{
	}

	// uncomment if you need copying and assignment
	//
	// AtomicIntegerT(const AtomicIntegerT& that)
	//   : value_(that.get())
	// {}
	//
	// AtomicIntegerT& operator=(const AtomicIntegerT& that)
	// {
	//   getAndSet(that.get());
	//   return *this;
	// }

	int32_t get()
	{
		// in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
		return InterlockedCompareExchange((LONG*)&value_, 0, 0);
	}

	int32_t getAndAdd(int32_t x)
	{
		// in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
		return InterlockedIncrement((LONG*)&x);
	}

	int32_t addAndGet(int32_t x)
	{
		return getAndAdd(x) + x;
	}

	int32_t incrementAndGet()
	{
		return addAndGet(1);
	}

	int32_t decrementAndGet()
	{
		return addAndGet(-1);
	}

	void add(int32_t x)
	{
		getAndAdd(x);
	}

	void increment()
	{
		incrementAndGet();
	}

	void decrement()
	{
		decrementAndGet();
	}

	int32_t getAndSet(int32_t newValue)
	{
		// in gcc >= 4.7: __atomic_exchange_n(&value, newValue, __ATOMIC_SEQ_CST)
		return InterlockedExchange((LONG*)&value_, newValue);
	}

private:
	volatile int32_t value_;
};

class AtomicInt64 : NoCopyable
{
public:
	AtomicInt64()
		: value_(0)
	{
	}

	// uncomment if you need copying and assignment
	//
	// AtomicIntegerT(const AtomicIntegerT& that)
	//   : value_(that.get())
	// {}
	//
	// AtomicIntegerT& operator=(const AtomicIntegerT& that)
	// {
	//   getAndSet(that.get());
	//   return *this;
	// }

	int64_t get()
	{
		// in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
		return InterlockedCompareExchange64(&value_, 0, 0);
	}

	int64_t getAndAdd( int64_t x)
	{
		// in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
		return InterlockedIncrement64(&x);
	}

	int64_t addAndGet(int64_t x)
	{
		return getAndAdd(x) + x;
	}

	int64_t incrementAndGet()
	{
		return addAndGet(1);
	}

	int64_t decrementAndGet()
	{
		return addAndGet(-1);
	}

	void add(int64_t x)
	{
		getAndAdd(x);
	}

	void increment()
	{
		incrementAndGet();
	}

	void decrement()
	{
		decrementAndGet();
	}

	int64_t getAndSet(int64_t newValue)
	{
		// in gcc >= 4.7: __atomic_exchange_n(&value, newValue, __ATOMIC_SEQ_CST)
		return InterlockedExchange64(&value_, newValue);
	}

private:
	volatile int64_t value_;
};
#else
template<typename T>
class AtomicIntegerT : NoCopyable
{
public:
	AtomicIntegerT()
		: value_(0)
	{
	}

	// uncomment if you need copying and assignment
	//
	// AtomicIntegerT(const AtomicIntegerT& that)
	//   : value_(that.get())
	// {}
	//
	// AtomicIntegerT& operator=(const AtomicIntegerT& that)
	// {
	//   getAndSet(that.get());
	//   return *this;
	// }

	T get()
	{
		// in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	T getAndAdd(T x)
	{
		// in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
		return __sync_fetch_and_add(&value_, x);
	}
	
	T addAndGet(T x)
	{
		return getAndAdd(x) + x;
	}

	T incrementAndGet()
	{
		return addAndGet(1);
	}

	T decrementAndGet()
	{
		return addAndGet(-1);
	}

	void add(T x)
	{
		getAndAdd(x);
	}

	void increment()
	{
		incrementAndGet();
	}

	void decrement()
	{
		decrementAndGet();
	}

	T getAndSet(T newValue)
	{
		// in gcc >= 4.7: __atomic_exchange_n(&value, newValue, __ATOMIC_SEQ_CST)
		return __sync_lock_test_and_set(&value_, newValue);
	}

private:
	volatile T value_;
};


typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;
#endif


#endif
