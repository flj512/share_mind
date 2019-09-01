#ifndef __BOOK_SHARE_MIND_SOCKCONN__
#define __BOOK_SHARE_MIND_SOCKCONN__

#include"SafeQueue.hpp"
#include"global_limit.hpp"

#define BOOKS_ROOT_PATH			"/var/books/"

#define IP_ADDR_MAX				32
#define LISTEN_QUEUE_SZ			10
#define WAIT_QUEUE_SZ			(MAX_THREADS_COUNT/2)



#define READ_TIMEOUT			10
#define WRITE_TIMEOUT			10

#define  CMD_INFO_HEADER0				0x5a
#define  CMD_INFO_HEADER1				0xb7
#define  CMD_INFO_HEADER2				0xd0
#define  CMD_INFO_HEADER3				0x2e

 
class SockListen{
private:
	int  m_port;
	char m_ipv4_addr[IP_ADDR_MAX];
	char m_ipv6_addr[IP_ADDR_MAX];
	
	int sockIPv4Listen(void);
	int sockIPv6Listen(void);
	
protected:
	
	int m_ipv4_listen_sock;
	int m_ipv6_listen_sock;
	
	int sockListen(void);

public:

	SockListen(int port,const char *ipv4_addr,const char *ipv6_addr);
	virtual ~SockListen(void);

	virtual void run()=0;	
};
class SockThreadsLoop:public SockListen{
private:
	SafeQueueLimitM<int> m_queue;
	int m_thread_count;
	
public:
	SockThreadsLoop(int port,const char *ipv4_addr,const char *ipv6_addr,int count);
	virtual ~SockThreadsLoop();
	void run();
	static void* loop(void *args);
	virtual bool done(int sock)=0;
	void procOneSock(void);
};
class FileServerLoop:public SockThreadsLoop{
private:
	bool checkInvalidFileName(const char* name);
	int Read(int fd,void *buffer,int n);
	int Write(int fd,void *buffer,int n);
	bool Readn(int fd,void *buffer,int n);
	bool Writen(int fd,void *buffer,int n);
public:
	FileServerLoop(int port,const char *ipv4_addr,const char *ipv6_addr,int count);
	bool done(int sock);
};
#endif
