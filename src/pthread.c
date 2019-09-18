


//https://locklessinc.com/articles/pthreads_on_windows/


#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

#include "pthread.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "targetver.h"
#include <windows.h>
#include <process.h>    /* _beginthreadex(), _endthreadex() */

#include "port.h"

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/timeb.h>
#include "error.h"

int PTW32_CDECL pthread_attr_init(pthread_attr_t * attr)
{
    if (attr != NULL) {
        *attr = NULL;
        return (int)0;
    }
    else
        return (int)-1;
}

int PTW32_CDECL pthread_attr_destroy(pthread_attr_t * attr)
{
    return 0;
}

pthread_t PTW32_CDECL pthread_self(void)
{
    ///
    /// 关于GetCurrentThread()的返回
    ///
    /// See: http://www.cnblogs.com/freebird92/articles/846520.html
    ///

    HANDLE hCurrentThread = NULL;

    DuplicateHandle(
        GetCurrentProcess(),        // Handle of process that thread
                                    // pseudo-handle is relative to
        GetCurrentThread(),         // Current thread's pseudo-handle
        GetCurrentProcess(),        // Handle of process that the new, real,
                                    // thread handle is relative to
        &hCurrentThread,            // Will receive the new, real, handle
                                    // identifying the parent thread
        0,                          // Ignored due to DUPLICATE_SAME_ACCESS
        FALSE,                      // New thread handle is not inheritable
        DUPLICATE_SAME_ACCESS       // New thread handle has same
    );

    return (pthread_t)hCurrentThread;
}

pid_t gettid(void)
{
	return GetCurrentThreadId();
}

/*
 * PThread Functions
 */
int PTW32_CDECL pthread_create(pthread_t * tid,
                               const pthread_attr_t * attr,
                               void *(PTW32_API *start)(void *),
                               void * arg)
{
    pthread_t hThread;
    unsigned int nThreadId;
    DWORD dwErrorCode;
    hThread = (pthread_t)_beginthreadex((void *)NULL, 0, (pthread_proc_t)start,
                                        arg, 0, (unsigned int *)&nThreadId);
    if (hThread == INVALID_HANDLE_VALUE || hThread == NULL) {
        dwErrorCode = GetLastError();
        printf("pthread_create(): dwErrorCode = 0x%08X\n", dwErrorCode);
        return (int)-1;
    }
    if (tid != NULL)
        *tid = hThread;
    return (int)0;
}

int PTW32_CDECL pthread_detach(pthread_t tid)
{
    BOOL bResult;
    bResult = TerminateThread((HANDLE)tid, -1);
    if (bResult && tid != NULL)
        bResult = CloseHandle((HANDLE)tid);
    return (bResult ? 0 : (int)-1);
}

int PTW32_CDECL pthread_join(pthread_t thread, void **value_ptr)
{
    DWORD dwMillisecs;
    DWORD dwResponse;
    if (value_ptr == NULL)
        dwMillisecs = INFINITE;
    else
        dwMillisecs = (DWORD)(*value_ptr);
    dwResponse = WaitForSingleObject(thread, dwMillisecs);
    return ((dwResponse == WAIT_OBJECT_0) ? 0 : (int)-1);
}







// 
// 
// 
// #define PTHREAD_ONCE_INIT 0
// typedef long pthread_once_t;
// 
// typedef struct _pthread_cleanup _pthread_cleanup;
// struct _pthread_cleanup
// {
// 	void (*func)(void *);
// 	void *arg;
// 	_pthread_cleanup *next;
// };
// 
// #define pthread_cleanup_push(F, A)\
// {\
// 	const _pthread_cleanup _pthread_cup = {(F), (A), pthread_self()->clean};\
// 	_ReadWriteBarrier();\
// 	pthread_self()->clean = (_pthread_cleanup *) &_pthread_cup;\
// 	_ReadWriteBarrier()
// 
// /* Note that if async cancelling is used, then there is a race here */
// #define pthread_cleanup_pop(E)\
// 	(pthread_self()->clean = _pthread_cup.next, (E?_pthread_cup.func(_pthread_cup.arg):0));}
// 
// static void _pthread_once_cleanup(pthread_once_t *o)
// {
// 	*o = 0;
// }
// 
// static int pthread_once(pthread_once_t *o, void (*func)(void))
// {
// 	long state = *o;
// 
// 	_ReadWriteBarrier();
// 
// 	while (state != 1)
// 	{
// 		if (!state)
// 		{
// 			if (!_InterlockedCompareExchange(o, 2, 0))
// 			{
// 				/* Success */
// 				pthread_cleanup_push(_pthread_once_cleanup, o);
// 				func();
// 				pthread_cleanup_pop(0);
// 
// 				/* Mark as done */
// 				*o = 1;
// 
// 				return 0;
// 			}
// 		}
// 
// 		YieldProcessor();
// 
// 		_ReadWriteBarrier();
// 
// 		state = *o;
// 	}
// 
// 	/* Done */
// 	return 0;
// }
// 
// 
// #define PTHREAD_CANCEL_DISABLE 0
// #define PTHREAD_CANCEL_ENABLE 0x01
// 
// #define PTHREAD_CANCEL_DEFERRED 0
// #define PTHREAD_CANCEL_ASYNCHRONOUS 0x02
// 
// #define PTHREAD_CANCELED ((void *) 0xDEADBEEF)
// 
// volatile long _pthread_cancelling;
// 
// static void _pthread_invoke_cancel(void)
// {
// 	_pthread_cleanup *pcup;
// 
// 	_InterlockedDecrement(&_pthread_cancelling);
// 
// 	/* Call cancel queue */
// 	for (pcup = pthread_self()->clean; pcup; pcup = pcup->next)
// 	{
// 		pcup->func(pcup->arg);
// 	}
// 
// 	pthread_exit(PTHREAD_CANCELED);
// }
// 
// static void pthread_testcancel(void)
// {
// 	if (_pthread_cancelling)
// 	{
// 		pthread_t t = pthread_self();
// 
// 		if (t->cancelled && (t->p_state & PTHREAD_CANCEL_ENABLE))
// 		{
// 			_pthread_invoke_cancel();
// 		}
// 	}
// }
// 
// static int pthread_cancel(pthread_t t)
// {
// 	if (t->p_state & PTHREAD_CANCEL_ASYNCHRONOUS)
// 	{
// 		/* Dangerous asynchronous cancelling */
// 		CONTEXT ctxt;
// 
// 		/* Already done? */
// 		if (t->cancelled) return ESRCH;
// 
// 		ctxt.ContextFlags = CONTEXT_CONTROL;
// 
// 		SuspendThread(t->h);
// 		GetThreadContext(t->h, &ctxt);
// #ifdef _M_X64
// 		ctxt.Rip = (uintptr_t) _pthread_invoke_cancel;
// #else
// 		ctxt.Eip = (uintptr_t) _pthread_invoke_cancel;
// #endif
// 		SetThreadContext(t->h, &ctxt);
// 
// 		/* Also try deferred Cancelling */
// 		t->cancelled = 1;
// 
// 		/* Notify everyone to look */
// 		_InterlockedIncrement(&_pthread_cancelling);
// 
// 		ResumeThread(t->h);
// 	}
// 	else
// 	{
// 		/* Safe deferred Cancelling */
// 		t->cancelled = 1;
// 
// 		/* Notify everyone to look */
// 		_InterlockedIncrement(&_pthread_cancelling);
// 	}
// 
// 	return 0;
// }






/*
 * Mutex Functions
 */
int PTW32_CDECL pthread_mutex_init(pthread_mutex_t * mutex,
                                   const pthread_mutexattr_t * attr)
{
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)
    static const DWORD dwSpinCount = 2;
    BOOL bResult = FALSE;
    if (mutex != NULL)
        bResult = InitializeCriticalSectionAndSpinCount(mutex, dwSpinCount);
    return (bResult ? 0 : (int)-1);
#else
    if (mutex != NULL)
        InitializeCriticalSection(mutex);
    return ((mutex != NULL) ? 0 : (int)-1);
#endif  /* defined(_M_X64) || defined(_WIN64) */
}

int PTW32_CDECL pthread_mutex_destroy(pthread_mutex_t * mutex)
{
    if (mutex != NULL)
        DeleteCriticalSection(mutex);
    return ((mutex != NULL) ? 0 : (int)-1);
}

int PTW32_CDECL pthread_mutex_lock(pthread_mutex_t * mutex)
{
    EnterCriticalSection(mutex);
    return ((mutex != NULL) ? 0 : (int)-1);
}

int PTW32_CDECL pthread_mutex_timedlock(pthread_mutex_t * mutex,
                                        const struct timespec *abstime)
{
    EnterCriticalSection(mutex);
    return ((mutex != NULL) ? 0 : (int)-1);
}

int PTW32_CDECL pthread_mutex_trylock(pthread_mutex_t * mutex)
{
    BOOL bResult = FALSE;
    if (mutex != NULL)
        bResult = TryEnterCriticalSection(mutex);
    return (bResult ? 0 : (int)-1);
}

int PTW32_CDECL pthread_mutex_unlock(pthread_mutex_t * mutex)
{
    LeaveCriticalSection(mutex);
    return ((mutex != NULL) ? 0 : (int)-1);
}

int PTW32_CDECL pthread_mutex_consistent(pthread_mutex_t * mutex)
{
    return 0;
}

/*
 * Spinlock Functions
 */
int PTW32_CDECL pthread_spin_init(pthread_spinlock_t * lock, int pshared)
{
    return 0;
}

int PTW32_CDECL pthread_spin_destroy(pthread_spinlock_t * lock)
{
    return 0;
}

int PTW32_CDECL pthread_spin_lock(pthread_spinlock_t * lock)
{
    return 0;
}

int PTW32_CDECL pthread_spin_trylock(pthread_spinlock_t * lock)
{
    return 0;
}

int PTW32_CDECL pthread_spin_unlock(pthread_spinlock_t * lock)
{
    return 0;
}




//Condition
int pthread_cond_init(pthread_cond_t *c, pthread_condattr_t *a)
{
	(void) a;

	InitializeConditionVariable(c);
	return 0;
}

int pthread_cond_signal(pthread_cond_t *c)
{
	WakeConditionVariable(c);
	return 0;
}

int pthread_cond_broadcast(pthread_cond_t *c)
{
	WakeAllConditionVariable(c);
	return 0;
}

int pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
	//pthread_testcancel();
	SleepConditionVariableCS(c, m, INFINITE);
	return 0;
}

int pthread_cond_destroy(pthread_cond_t *c)
{
	(void) c;
	return 0;
}




struct timespec
{
	/* long long in windows is the same as long in unix for 64bit */
	long long tv_sec;
	long long tv_nsec;
};

static unsigned long long _pthread_time_in_ms(void)
{
	struct __timeb64 tb;

	_ftime64(&tb);

	return tb.time * 1000 + tb.millitm;
}

static unsigned long long _pthread_time_in_ms_from_timespec(const struct timespec *ts)
{
	unsigned long long t = ts->tv_sec * 1000;
	unsigned long long ns = (ts->tv_nsec / 1000000);
	t += ns;

	return t;
}

static unsigned long long _pthread_rel_time_in_ms(const struct timespec *ts)
{
	unsigned long long t1 = _pthread_time_in_ms_from_timespec(ts);
	unsigned long long t2 = _pthread_time_in_ms();

	/* Prevent underflow */
	if (t1 < t2) return 1;
	return t1 - t2;
}

 int pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, struct timespec *t)
 {
 	unsigned long long tm = _pthread_rel_time_in_ms(t);
 
 	//pthread_testcancel();
 
 	if (!SleepConditionVariableCS(c, m, tm)) return ETIMEDOUT;
 
 	/* We can have a spurious wakeup after the timeout */
 	if (!_pthread_rel_time_in_ms(t)) return ETIMEDOUT;
 
 	return 0;
 }






#endif  // defined(_MSC_VER) || defined(__INTERL_COMPILER)

#if defined(_MSC_VER) || defined(__INTEL_COMPILER) || defined(__MINGW32__) || defined(__CYGWIN__)

#if defined(__MINGW32__) || defined(__CYGWIN__)
#include <pthread.h>
#include "msvc/pthread.h"
#include "port.h"

#include <stdio.h>
#include <assert.h>

typedef struct ptw32_thread_t_       ptw32_thread_t;

struct ptw32_thread_t_
{
  unsigned __int64 seqNumber;   /* Process-unique thread sequence number */
  HANDLE threadH;               /* Win32 thread handle - POSIX thread is invalid if threadH == 0 */
  pthread_t ptHandle;           /* This thread's permanent pthread_t handle */
};
#endif  /* defined(__MINGW32__) || defined(__CYGWIN__) */

HANDLE PTW32_CDECL pthread_process_self(void)
{
    ///
    /// 关于GetCurrentThread()的返回
    ///
    /// See: http://www.cnblogs.com/freebird92/articles/846520.html
    ///
#if defined(__MINGW32__) || defined(__CYGWIN__)
    //pthread_t ptw32_process = { 0, 0 };
#endif
    HANDLE hCurrentProcess = NULL;

    DuplicateHandle(
        GetCurrentProcess(),        // Handle of process that process
                                    // pseudo-handle is relative to
        GetCurrentProcess(),        // Process's pseudo-handle
        GetCurrentProcess(),        // Handle of process that the new, real,
                                    // thread handle is relative to
        &hCurrentProcess,           // Will receive the new, real, handle
                                    // identifying the process
        0,                          // Ignored due to DUPLICATE_SAME_ACCESS
        FALSE,                      // New thread handle is not inheritable
        DUPLICATE_SAME_ACCESS       // New thread handle has same
    );

#if defined(__MINGW32__) || defined(__CYGWIN__)
    /*
    ptw32_process.p = hCurrentProcess;
    ptw32_process.x = 0;
    return ptw32_process;
    //*/
    return (HANDLE)hCurrentProcess;
#else
    return (HANDLE)hCurrentProcess;
#endif
}

/*
 * About the thread affinity
 */
int PTW32_CDECL pthread_getaffinity_np(pthread_t thread, size_t cpuset_size,
                                       cpu_set_t * cpuset)
{
    return 0;
}

int PTW32_CDECL pthread_setaffinity_np(pthread_t thread_in, size_t cpuset_size,
                                       const cpu_set_t * cpuset)
{
    static const int echo = 0;
    HANDLE hCurrentProcess;
    HANDLE hTargetThread, thread;
    DWORD_PTR dwProcessAffinity, dwSystemAffinity;
    DWORD_PTR dwAffinityMask, dwAffinityMaskOld;
#if defined(__MINGW32__) || defined(__CYGWIN__)
    ptw32_thread_t *sp;
    pthread_t threadTmp;
#endif
    BOOL bAffResult;
    int numCore;
    unsigned int loop_cnt;

#if defined(__MINGW32__) || defined(__CYGWIN__) || defined(__MSYS__)
  #if defined(PTW32_VERSION) && defined(PTW32_VERSION_STRING)
    sp = (ptw32_thread_t *)thread_in.p;
    thread = sp->threadH;
  #elif defined(__MSYS__) || defined(__CYGWIN__)
    thread = (HANDLE)thread_in;
    //printf("thread_in = %8p\n", thread_in);
  #else
    thread = (thread_t)NULL;
  #endif
#else
    thread = thread_in;
#endif

    //printf("thread = %8p\n", thread);

    assert(thread != NULL && thread != INVALID_HANDLE_VALUE);
    assert(cpuset != NULL);
    assert(cpuset_size != 0);

    if (thread == NULL || thread == INVALID_HANDLE_VALUE) {
        return -1;
    }

    numCore = get_num_of_processors();
    if (numCore <= 1) {
        return 0;
    }

    if (echo) {
        printf("\n");
    }

    dwAffinityMask = cpuset->mask;
    if (dwAffinityMask == 0) {
        dwAffinityMask = 0xFFFFFFFFU;
    }

#if 1
    // Check and set dwProcessAffinity to dwSystemAffinity
    loop_cnt = 0;
    hCurrentProcess = pthread_process_self();

    dwProcessAffinity = 0; dwSystemAffinity = 0;
    bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinity, &dwSystemAffinity);
    if (bAffResult) {
        while (dwProcessAffinity != dwSystemAffinity) {
            if (echo) {
                printf("This process can not utilize all processors.\n");
            }
            bAffResult = SetProcessAffinityMask(hCurrentProcess, dwSystemAffinity);
            loop_cnt++;
            if (loop_cnt > 10) {
                printf("SetProcessAffinityMask(): bAffResult = %u, loop_cnt = %u.\n", bAffResult, loop_cnt);
                break;
            }
            if (bAffResult) {
                // Wait for the process affinity effected
                Sleep(1);
                // Update dwProcessAffinity
                bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinity, &dwSystemAffinity);
            }
        }
    }
    else {
        return -1;
    }
#else
    loop_cnt = 0;
    hCurrentProcess = pthread_process_self();

    dwProcessAffinity = 0; dwSystemAffinity = 0;
    bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinity, &dwSystemAffinity);
#endif

    if (echo) {
        printf("dwProcessMask = %Iu, dwSystemMask = %Iu\n", dwProcessAffinity, dwSystemAffinity);
    }

    // Adjust dwAffinityMask
    dwAffinityMask = dwAffinityMask & dwProcessAffinity;

    // Set the affinity mask
#if defined(__MINGW32__) || defined(__CYGWIN__) || defined(__MSYS__)
  #if defined(PTW32_VERSION) && defined(PTW32_VERSION_STRING)
    sp = (ptw32_thread_t *)thread_in.p;
    hTargetThread = sp->threadH;
  #elif defined(__MSYS__) || defined(__CYGWIN__)
    hTargetThread = (HANDLE)thread_in;
    //printf("hTargetThread = %8p\n", hTargetThread);
  #else
    hTargetThread = (thread_t)NULL;
  #endif
#else
    hTargetThread = thread;
#endif
    dwAffinityMaskOld = SetThreadAffinityMask(hTargetThread, dwAffinityMask);
    if (echo) {
        printf("dwMask = %Iu, dwOldMask = %Iu\n", dwAffinityMask, dwAffinityMaskOld);
    }

    if (echo) {
        printf("\n");
    }

    if (dwAffinityMaskOld != 0) {
        if (echo)
            printf("pthread_setaffinity_np() ok.\n");
        return 0;
    }
    else {
        if (echo)
            printf("pthread_setaffinity_np() failed.\n");
        return -1;
    }
}

#if 0

int PTW32_CDECL pthread_setaffinity_np2(pthread_t thread_in, size_t cpuset_size,
                                        const cpu_set_t * cpuset)
{
    static const int echo = 1;
    HANDLE hCurrentProcess = pthread_process_self();
    HANDLE hCurrentThread = pthread_self(), thread;
    DWORD dwProcessAffinity = 0, dwSystemAffinity = 0;
    DWORD dwAffinityMask;
    BOOL bAffResult;

#if defined(__MINGW32__) || defined(__CYGWIN__)
    thread = thread_in.p;
#else
    thread = thread_in;
#endif

    assert(thread != NULL);
    assert(cpuset != NULL);
    assert(cpuset_size != 0);

    if (echo) {
        printf("\n");
    }

    dwAffinityMask = cpuset->mask;
    bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinity, &dwSystemAffinity);
    if (bAffResult) {
        if (dwProcessAffinity != dwSystemAffinity) {
            if (echo) {
                printf("This process can not utilize all processors.\n");
            }
        }

        while ((dwAffinityMask != 0) && (dwAffinityMask <= dwProcessAffinity)) {
            // Check to make sure we can utilize this processsor first.
            if ((dwAffinityMask & dwProcessAffinity) != 0) {
                bAffResult = SetProcessAffinityMask(hCurrentProcess, dwAffinityMask);
                if (bAffResult) {
                    // Wait for the process affinity effected
                    Sleep(1);
                    //
                    if (echo) {
                        printf("SetProcessAffinityMask(): dwAffinityMask = 0x%08X\n", dwAffinityMask);
                    }
                    DWORD dwProcessAffinityNew = 0;
                    bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityNew, &dwSystemAffinity);
                    if (dwProcessAffinityNew == dwAffinityMask) {
                        if (echo) {
                            printf("SetProcessAffinityMask(): Success.\n");
                        }
                        bAffResult = SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
                        Sleep(1);
                        break;
                    }
                }
            }
        }
    }

    if (echo) {
        printf("\n");
    }
    return 0;
}

#endif

#endif  // defined(_MSC_VER) || defined(__INTERL_COMPILER) || defined(__MINGW32__) || defined(__CYGWIN__)
