#ifndef __BOOK_SHARE_MIND_SOCK_CONNECTOR__
#define __BOOK_SHARE_MIND_SOCK_CONNECTOR__

#include"SockMonitor.hpp"
#include"SockListen.hpp"
#include"CmdInfo.hpp"
#include"KeepAliveDetect.hpp"

class SockConnectorEventListner:public SockMonitorEvnetListner{
private:
	CmdInfo *m_respose_cmd;
	KeepAliveDetect *m_keepAliveDetect;
	
	int networkStatus(void);
	int broadcastKeepAlive(void);
public:
	SockConnectorEventListner(void);
	~SockConnectorEventListner(void);
 	int onAccept(int sock);
 	int onRead(int sock);
 	int onWrite(int sock);
	int onFDRead(int fd);
	int onFDWrite(int fd);
	void setKeepAliveDetect(KeepAliveDetect *keepAliveDetect);
};

class SockConnectorListen:public SockListen{
private:
	SockConnectorEventListner *m_listener;
	ResourceMonitor *m_resource_monitor;
	

public:
	SockConnectorListen(int port,const char *ipv4_addr,const char *ipv6_addr);
	~SockConnectorListen();
	void setResourceMonitor(ResourceMonitor *resource_monitor);
	void setKeepAliveDetect(KeepAliveDetect *keepAliveDetect);
	void run();
};

#endif
