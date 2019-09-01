#ifndef __BOOK_MIND_SHARE__SELECT_SOCK_MONITOR__
#define __BOOK_MIND_SHARE__SELECT_SOCK_MONITOR__

#include<sys/select.h>
#include"SockMonitor.hpp"

class SelectSockMonitor:public SockCacheMonitor{
private:
	fd_set m_read;
	fd_set m_write;
public:
	int wait(void);
};
#endif
