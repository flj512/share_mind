#ifndef __BOOK_MIND_SHARE_SOCK_MONITOR__
#define __BOOK_MIND_SHARE_SOCK_MONITOR__
#include<set>

class SockMonitor;

class SockMonitorEvnetListner{
protected:
	SockMonitor *m_monitor;
public:
	SockMonitorEvnetListner(void);
	virtual ~SockMonitorEvnetListner(void)=0;
	void setMonitor(SockMonitor *monitor);
	
	virtual int onAccept(int sock)=0;
	virtual int onRead(int sock)=0;
	virtual int onWrite(int sock)=0;
	virtual int onFDRead(int fd)=0;
	virtual int onFDWrite(int fd)=0;

};

class SockMonitor{
protected:
	bool m_read_enable;
	SockMonitorEvnetListner *m_listener;
public:
	SockMonitor();
	virtual ~SockMonitor();
	void setLister(SockMonitorEvnetListner *listener);	
	virtual bool setReadEnable(bool enable);
	virtual bool getReadEnable(void);

	
	virtual bool addRead(int sock)=0;// add sockect to read
	virtual bool delRead(int sock)=0;
	virtual bool addWrite(int sock)=0;//add sock to write
	virtual bool delWrite(int sock)=0;
	virtual bool addListen(int sock)=0;//add sock to listen
	virtual bool delListen(int sock)=0;
	virtual bool addFDRead(int fd)=0;//add file to read
	virtual bool delFDRead(int fd)=0;
	virtual bool addFDWrite(int fd)=0;//add file to write
	virtual bool delFDWrite(int fd)=0;

	virtual void status(void)=0;
	virtual int wait(void)=0;
};

class SockFDMonitor:public SockMonitor{
protected:
	std::set<int> m_fd_read_set;
	std::set<int> m_fd_write_set;
public:
	bool addFDRead(int fd);
	bool delFDRead(int fd);
	bool addFDWrite(int fd);
	bool delFDWrite(int fd);
};
class SockCacheMonitor:public SockFDMonitor{
protected:
	std::set<int> m_read_set;
	std::set<int> m_write_set;
	std::set<int> m_listen_set;

public:

	bool addRead(int sock);
	bool delRead(int sock);
	bool addWrite(int sock);
	bool delWrite(int sock);
	bool addListen(int sock);
	bool delListen(int sock);
	
	void status(void);
	virtual int wait(void)=0;
	
};

class ResourceMonitor{
public:
	ResourceMonitor();
	virtual ~ResourceMonitor();
	virtual bool overflowed(void)=0;
	virtual bool normalLevel(void)=0;
};

class SockMonitorFactory{
public:
	static SockMonitor* createSockMonitor(const char *type);
	static SockMonitor* createDefaultSockMonitor(void);
};
#endif
