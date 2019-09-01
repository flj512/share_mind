#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/stat.h>
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
	//ALERT_DO_E(comm_setfd_noblock(sock)==0,"SET NONBLOCK Fail",goto out);

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
SockThreadsLoop::SockThreadsLoop(int port,const char *ipv4_addr,const char *ipv6_addr,int count):SockListen(port,ipv4_addr,ipv6_addr),m_queue(WAIT_QUEUE_SZ)
{
	m_thread_count=count;
}

SockThreadsLoop::~SockThreadsLoop()
{
}
void SockThreadsLoop::run()
{
	pthread_t pid;
	int new_sock;

	ALERT_RET(sockListen()>=0,"Listen Fail");
	
	for(int i=0;i<m_thread_count;i++)
	{
		 ALERT_RET(pthread_create(&pid,NULL,SockThreadsLoop::loop,this)>=0,"Create Thread Fail");
	}

	while(1)
	{
		new_sock=accept(m_ipv4_listen_sock,NULL,NULL);
		
		ALERT_DO_E(new_sock >=0 ,"Accept Fail",continue);
		
		m_queue.push(new_sock);
	}
}
void SockThreadsLoop::procOneSock(void)
{
	int sock,n;
	char dummy[512];

	sock=m_queue.pop();
	if(done(sock))
	{
		ALERT_INFO_E(shutdown(sock,SHUT_RDWR)>=0,"ShutDown Error");
		while((n=recv(sock,dummy,sizeof(dummy),0))>0)
		{
		}
		
		ALERT_INFO_E(n==0,"Wait For Close Fail");
	}
	close(sock);
}

void* SockThreadsLoop::loop(void *args)
{
	SockThreadsLoop *sock_threads=(SockThreadsLoop *)args;
	
	while(1)
	{
		sock_threads->procOneSock();
	}

	return NULL;
}

FileServerLoop::FileServerLoop(int port,const char *ipv4_addr,const char *ipv6_addr,int count):SockThreadsLoop(port,ipv4_addr,ipv6_addr,count)
{
}
bool FileServerLoop::checkInvalidFileName(const char* name)
{
	int i=0;
	const char *index=name;

	while(*index!=0&&++i<MAX_FILE_PATH_LEN)
	{
		if(!((*index>='0'&&*index<='9')||(*index>='a'&&*index<='z')||(*index>='A'&&*index<='Z')))
		{
			return false;
		}
		index++;
	}

	return i<MAX_FILE_PATH_LEN;
}
int FileServerLoop::Read(int fd,void *buffer,int n)
{
	int nr;
	
	while(1)
	{
		nr=read(fd,buffer,n);
		if(nr<0 && errno==EINTR)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	return nr;
	
}

bool FileServerLoop::Readn(int fd,void *buffer,int n)
{
	int nr,ns=0;
	
	while(ns<n)
	{
		nr=Read(fd,(u8*)buffer+ns,n-ns);
		if(nr <= 0)
		{
			return false;
		}
		
		ns+=nr;
	}

	return true;
}
int FileServerLoop::Write(int fd,void *buffer,int n)
{
	int nw;
	
	while(1)
	{
		nw=write(fd,buffer,n);
		if(nw<0 && errno==EINTR)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	return nw;
}
bool FileServerLoop::Writen(int fd,void *buffer,int n)
{
	int nw,ns=0;
	
	while(ns<n)
	{
		nw=Write(fd,(u8*)buffer+ns,n-ns);
		if(nw <= 0)
		{
			return false;
		}
		
		ns+=nw;
	}

	return true;
}

bool FileServerLoop::done(int sock)
{
	int n,nr,nt=0,nname;
	char file_path[256];
	struct timeval val;
	struct sockaddr remote;
	socklen_t len;
	int file;
	struct stat file_stat;
	u8 buffer[4096];
	u8 header[6];
	
	val.tv_usec=0;
	
	val.tv_sec=READ_TIMEOUT;
	ALERT_RET_V_E(setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&val,sizeof(val))>=0,"Set Read Timeout Fail",false);
	
	val.tv_sec=WRITE_TIMEOUT;
	ALERT_RET_V_E(setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,&val,sizeof(val))>=0,"Set Write Timeout Fail",false);

	ALERT_RET_V_E(Readn(sock,header,sizeof(header)),"Read Header Fail",false);
	
	nname=GET_WORD_BIG(header+4);
	if(header[0]!=CMD_INFO_HEADER0||header[1]!=CMD_INFO_HEADER1||header[2]!=CMD_INFO_HEADER2||header[3]!=CMD_INFO_HEADER3||nname>=MAX_FILE_PATH_LEN)
	{
		return false;
	}

	n=snprintf(file_path,sizeof(file_path),"%s",BOOKS_ROOT_PATH);
	nr=UTIL_MIN((unsigned int)nname,sizeof(file_path)-1-n);
	ALERT_RET_V(Readn(sock,file_path+n,nr),"Read Name Fail",false);
	file_path[n+nr]=0;
	//endOfStringEnter(file_path+n);


	len=sizeof(remote);
	if(getpeername(sock,&remote,&len)>=0)
	{
		char desc[128];

		if(inet_ntop(AF_INET,&((sockaddr_in*)&remote)->sin_addr,desc,sizeof(desc))==NULL)
		{
			snprintf(desc,sizeof(desc),"unkown");
		}
		
		DEBUG_INFO("[%s] Get [%s]\n",desc,file_path+n);
	}
	else
	{
		ASSERT_INFO_E("Get Peer Fail");
	}
	
	if(!checkInvalidFileName(file_path+n))
	{
		return false;
	}

	file=open(file_path,O_RDONLY);
	ALERT_RET_V_E(file>=0,"Open File Fail",false);
	ALERT_DO_E(fstat(file,&file_stat)>=0,"Stat File Fail",goto out;);
	ALERT_DO(file_stat.st_size<=MAX_FILE_SIZE,"File Too Big",goto out;);
	SET_DWORD_BIG(buffer,file_stat.st_size);
	ALERT_DO_E(Writen(sock,buffer,4),"Send Fail",goto out;);
	
	while(1)
	{
		ALERT_DO_E((nr=Read(file,buffer,sizeof(buffer)))>=0,"Read Fail",break;);
		if(nr==0)
		{
			break;
		}
		
		ALERT_DO_E(Writen(sock,buffer,nr),"Write Fail",break;);
		nt+=nr;
	}
	
out:
	close(file);
	return ((long)nt==file_stat.st_size);
}
//file
