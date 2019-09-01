#ifndef __BOOK_SHARE_MIND_SOCKCONN__
#define __BOOK_SHARE_MIND_SOCKCONN__

#include"SockMonitor.hpp"

#define IP_ADDR_MAX				32
#define LISTEN_QUEUE_SZ			10
 
class SockListen{
private:
	int  m_port;
	char m_ipv4_addr[IP_ADDR_MAX];
	char m_ipv6_addr[IP_ADDR_MAX];
	
	int sockIPv4Listen(void);
	int sockIPv6Listen(void);
	
protected:
	SockMonitor *m_monitor;
	int m_ipv4_listen_sock;
	int m_ipv6_listen_sock;
	
	int sockListen(void);

public:

	SockListen(int port,const char *ipv4_addr,const char *ipv6_addr);
	virtual ~SockListen(void);

	void setMonitor(SockMonitor *monitor);

	SockMonitor *getMonitor(void);
	
	virtual void run()=0;	
};
#endif
