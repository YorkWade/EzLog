#include "LogStream.h"
#include <iostream>
#include "Mutex.h"
#include "Thread.h"
#include <assert.h>
#include "Logger.h"
#include "async_logger.h"

using namespace Ez;

void fun()
{
	printf("fun started!");
	for (int i = 1;i<5;i++)
	{
		Sleep(1000);
	}
};


AsyncLogger* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
	g_asyncLog->append(msg, len);
}


void main()
{
	LogStream stream;
	int x = 100000;
	double y = 5.3;
	char byte_str[]={1,2,3,4,5,6,7};
	stream<<"a"<<" "<<1<<" "<<Fmt("%x",x)<<Fmt("%9.3f",5.3)<<" "<<Hex(byte_str,sizeof byte_str)<<Hex(x);
	std::cout<<stream.buffer().data()<<"\n";

	{
		LOG_WARN << "dtor[" << "]";
	}
	{
		LOG_INFO<<"aaaa";
	}

	
//	Thread th(std::bind(&fun), "Logging");

// 	MutexLock mutex;
// 	MutexLockGuard lock(mutex);
// 	th.start();
// 	th.join();

	//Logger logger("main",500);
	//logger.start();
	off_t kRollSize = 500*1000*1000;
	AsyncLogger log("testLog", kRollSize);

	g_asyncLog = &log;
	log.start();


	Logger::setOutput(asyncOutput);


	for (int i = 0;i<3000000;i++)
	{
		LOG_INFO<<i<<Fmt("%x",i)<<Fmt("%9.3f",i)<<" "<<Hex(byte_str,sizeof byte_str)<<Hex(i);
	}

	system("pause");
}