#ifndef __TEST_SOCK_CONNECT__
#define __TEST_SOCK_CONNECT__
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"SelectSockMonitor.hpp"
#include"SockListen.hpp"
#include"util_macro.h"
#include"com_utils.h"


class TestSockMonitorEvnetListner:public SockMonitorEvnetListner{

public:
	int onAccept(int sock)
	{
		struct sockaddr_in in;
		socklen_t len=sizeof(in);
		
		int new_sock=accept(sock,(struct sockaddr*)&in,&len);

		ALERT_RET_V_E(new_sock >=0 ,"Accept Fail",-1);

		printf("Connect Form [%s:%d]\n",inet_ntoa(in.sin_addr),in.sin_port);
		ALERT_DO(comm_setfd_noblock(new_sock)==0,"SET NONBLOCK",close(new_sock);return -1;);
		if(m_monitor!=NULL) m_monitor->addRead(new_sock);
		
		return new_sock;
	}
	int onRead(int sock)
	{
		u8 buffer[1024];
		int rd_len=recv(sock,buffer,sizeof(buffer),0);

		ALERT_RET_V_E(rd_len>=0,"Read Fail",-1);
		ALERT_DO(rd_len!=0,"Remote Disconnect",if(m_monitor!=NULL) m_monitor->delRead(sock);close(sock);return 0;);

		buffer[rd_len]=0;

		//printf("reciev from [%d]:%s\n",sock,(char*)buffer);
		
		return rd_len;		
	}
	int onWrite(int sock)
	{
		return -1;
	}
	int onFDRead(int fd)
	{
		return -1;
	}
	int onFDWrite(int fd)
	{
		return -1;
	}
};

class TestSockListen:public SockListen{
public:
	TestSockListen(int port,char *ipv4_addr,char *ipv6_addr):SockListen(port,ipv4_addr,ipv6_addr)
	{
	}
	void run()
	{
		ALERT_RET_E(sockListen()>=0,"Listen Fail");
		ALERT_RET(m_monitor!=NULL,"No Monitor");
		m_monitor->addListen(m_ipv4_listen_sock);

		while(1)
		{
			ALERT_RET(m_monitor->wait()>=0,"Wait Fail");
		}
	}
};
#endif
