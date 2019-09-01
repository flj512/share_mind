#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"SockListen.hpp"
#include"util_macro.h"
#include"com_utils.h"


SockListen::SockListen(int port,const char *ipv4_addr,const char *ipv6_addr)
{
	m_port = port;
	
	memset(m_ipv4_addr,0,sizeof(m_ipv4_addr));
	if(ipv4_addr!=NULL)
	{
		strncpy(m_ipv4_addr,ipv4_addr,sizeof(m_ipv4_addr)-1);
	}
	
	memset(m_ipv6_addr,0,sizeof(m_ipv6_addr));
	if(ipv6_addr!=NULL)
	{
		strncpy(m_ipv6_addr,ipv6_addr,sizeof(m_ipv6_addr)-1);
	}

	m_ipv4_listen_sock = -1;
	m_ipv6_listen_sock = -1;
	m_monitor = NULL;
}

SockListen::~SockListen(void)
{
	if(m_ipv4_listen_sock >= 0)
	{
		close(m_ipv4_listen_sock);
	}

	if(m_ipv6_listen_sock >=0)
	{
		close(m_ipv6_listen_sock);
	}
}

void SockListen::setMonitor(SockMonitor *monitor)
{
	m_monitor = monitor;
}

SockMonitor* SockListen::getMonitor(void)
{
	return m_monitor;
}
int SockListen::sockIPv4Listen(void)
{
	int sock;
	int reuse=1;
	struct sockaddr_in in;

	sock = socket(AF_INET,SOCK_STREAM,0);
	ALERT_RET_V_E(sock>=0,"Create Socket Fail",-1);
	ALERT_RET_V_E(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))>=0,"Set Socket Reuse Fail",-1);

	memset(&in,0,sizeof(in));
	in.sin_family = AF_INET;
	in.sin_port = htons(m_port);
	
	if(strlen(m_ipv4_addr)!=0)
	{
		inet_aton(m_ipv4_addr,&in.sin_addr);
	}
	else
	{
		in.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	ALERT_DO_E(bind(sock,(struct sockaddr*)&in,sizeof(in))==0,"Bind Fail",goto out);
	ALERT_DO_E(listen(sock,LISTEN_QUEUE_SZ)==0,"Listen Fail",goto out);
	ALERT_DO_E(comm_setfd_noblock(sock)==0,"SET NONBLOCK Fail",goto out);

	return sock;
out:
	close(sock);
	return -1;
		
}

int SockListen::sockIPv6Listen(void)
{
	return -1;
}

int SockListen::sockListen(void)
{
	int ret=-1;
	
	m_ipv4_listen_sock=sockIPv4Listen();
	m_ipv6_listen_sock=sockIPv6Listen();

	if(m_ipv4_listen_sock>=0)
	{
		ret = 0;
	}

	return ret;
}
//file
