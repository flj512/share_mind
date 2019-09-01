#ifndef __BOOK_SHARE_MIND_EPOLL_SOCK_MONITOR_
#define __BOOK_SHARE_MIND_EPOLL_SOCK_MONITOR_

#include<sys/epoll.h>
#include"SockMonitor.hpp"
#include"global_limit.hpp"

class EpollSockMonitor:public SockCacheMonitor
{
private:
	struct epoll_event m_array[MAX_CONNECT_COUNT+1];
	int m_efd;

public:
	EpollSockMonitor();
	~EpollSockMonitor();
	bool setReadEnable(bool enable);

	
	bool addRead(int sock);
	bool delRead(int sock);
	bool addWrite(int sock);
	bool delWrite(int sock);
	bool addListen(int sock);
	bool delListen(int sock);
	bool addFDRead(int fd);
	bool delFDRead(int fd);
	bool addFDWrite(int fd);
	bool delFDWrite(int fd);
	int wait(void);
	
};
#endif
