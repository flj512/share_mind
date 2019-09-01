#ifndef __BOOK_SHARE_MIND_LOG_UTIL_H__
#define __BOOK_SHARE_MIND_LOG_UTIL_H__

#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include"SafeQueue.hpp"

class LogUtil{
private:
	static const char * m_log_path;
	static const char * m_log_file;
	static char m_path[256];
	static FILE *m_file;
	static long m_write_size;
	static  pthread_mutex_t m_mutex;
	static SafeQueueLimitM<char*> *m_queue;

	static void changeLogFile(int len);
	static void writeMsg(const char *msg);
	
public:
	static bool init(void);
	static void log(const char *msg);
	static void run(void);
	static void flush(void);
};

//int DEBUG_INFO(const char *format,...);
#endif
