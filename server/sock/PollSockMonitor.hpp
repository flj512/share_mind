#ifndef __BOOK_SHARE_MIND__POLL_SOCK_MONITOR__
#define __BOOK_SHARE_MIND__POLL_SOCK_MONITOR__

#include<poll.h>
#include"SockMonitor.hpp"
#include"global_limit.hpp"

class PollSockMonitor:public SockCacheMonitor{
private:
	struct pollfd m_array[MAX_CONNECT_COUNT*2+64];//include pipe ,stdin,listen_port
public:
	int wait(void);
};

#endif
