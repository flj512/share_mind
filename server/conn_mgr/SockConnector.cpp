#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"SockConnector.hpp"
#include"SockMgr.hpp"
#include"CmdInfo.hpp"
#include"SessionMgr.hpp"
#include"Session.hpp"
#include"util_macro.h"
#include"com_utils.h"
#include"global_limit.hpp"

SockConnectorListen::SockConnectorListen(int port,const char *ipv4_addr,const char *ipv6_addr):SockListen(port,ipv4_addr,ipv6_addr)
{
	m_monitor=SockMonitorFactory::createDefaultSockMonitor();
	m_listener=new SockConnectorEventListner();
	m_monitor->setLister(m_listener);
	m_resource_monitor = NULL;
}

SockConnectorListen::~SockConnectorListen()
{
	m_monitor->setLister(NULL);
	delete m_listener;
	delete m_monitor;
}

void SockConnectorListen::run()
{
	if(sockListen()<0)
	{
		return ;
	}

	ALERT_RET_E(m_monitor->addListen(m_ipv4_listen_sock),"Listen Fail");
	
	while(1)
	{
		if(m_resource_monitor!=NULL)
		{
			if(m_monitor->getReadEnable()&&m_resource_monitor->overflowed())
			{
				if(!m_monitor->setReadEnable(false))
				{
					break;
				}
			}
			else
			{
				if(!m_monitor->getReadEnable()&&m_resource_monitor->normalLevel())
				{
					if(!m_monitor->setReadEnable(true))
					{
						break;
					}
					
				}
			}
		}
		
		if(m_monitor->wait()<0)
		{
			break;
		}
	}
}

void SockConnectorListen::setResourceMonitor(ResourceMonitor *resource_monitor)
{
	m_resource_monitor = resource_monitor;
}
void SockConnectorListen::setKeepAliveDetect(KeepAliveDetect *keepAliveDetect)
{
	m_listener->setKeepAliveDetect( keepAliveDetect);
}

SockConnectorEventListner::SockConnectorEventListner(void)
{
	m_respose_cmd = NULL;
}
SockConnectorEventListner::~SockConnectorEventListner(void)
{
	if(m_respose_cmd!=NULL)
	{
		delete m_respose_cmd;
	}
}

int SockConnectorEventListner::onAccept(int sock)
{
	struct sockaddr_in in;
	socklen_t len=sizeof(in);
	char desc[SOCK_DESC_LEN];
	
	int new_sock=accept(sock,(struct sockaddr*)&in,&len);
	SockMgr *mgr=SockMgr::getSingleton();
	
	
	ALERT_RET_V_E(new_sock >=0 ,"Accept Fail",-1);
	
	ALERT_DO_E(comm_setfd_noblock(new_sock)==0,"SET NONBLOCK Fail",goto out0;);

	ALERT_DO(m_monitor!=NULL&&m_monitor->addRead(new_sock),"Add to Read Fail",goto out0;)

	snprintf(desc,sizeof(desc),"%s",inet_ntoa(in.sin_addr));

	ALERT_DO(mgr->insertSock(new_sock,desc),"Insert Sock Fail",goto out1;);


	return new_sock;
out1:
	m_monitor->delRead(new_sock);
out0:
	close(new_sock);
	return -1;
}
void SockConnectorEventListner::setKeepAliveDetect(KeepAliveDetect *keepAliveDetect)
{
	m_keepAliveDetect=keepAliveDetect;
}

int SockConnectorEventListner::onRead(int sock)//read only once
{
	u8 buffer[CMD_MAX_SIZE];
	int rd_len;
	SockMgr *mgr=SockMgr::getSingleton();


	rd_len=recv(sock,buffer,sizeof(buffer),0);
		
	ALERT_RET_V_E(rd_len>=0,"Read Fail",-1);

	if(rd_len==0)
	{
		if(m_monitor!=NULL) 
		{
			ALERT_INFO(m_monitor->delRead(sock),"Remove Read Monitor Fail");
			//ALERT_INFO(m_monitor->delWrite(sock),"Remove Write Monitor Fail");
			m_monitor->delWrite(sock);
		}

		mgr->removeSock(sock);
		
		ALERT_INFO_E(close(sock)>=0,"Close Sock Fail");

		return 0;
	}

	//ALERT_INFO(mgr->pushData(sock,buffer,rd_len)==0,"Bad Data,Drop");
	mgr->pushData(sock,buffer,rd_len);
		
	return rd_len;		
}

int SockConnectorEventListner::onWrite(int sock)
{
	int wr_len=0;
	SockData *sockData;

	sockData=SockMgr::getSingleton()->getSockData(sock);
	if(sockData!=NULL)
	{
		Session *session=sockData->getSession();
		if(session!=NULL)
		{
			CmdInfo *info=session->front();
			if(info!=NULL)
			{
				wr_len=send(sock,info->getCmd(),info->getCmdLen(),0);

				if(wr_len<0)
				{
					ASSERT_INFO_E("Write Fail");
					//if(errno==EPIPE)
					{
						ALERT_INFO(m_monitor->delWrite(sock),"Remove Write Monitor Fail");
						ALERT_INFO(m_monitor->delRead(sock),"Remove Read Monitor Fail");
						SockMgr::getSingleton()->removeSock(sock);
						ALERT_INFO_E(close(sock)>=0,"Close Sock Fail");
					}

					return -1;
				}
				else if(wr_len>0)
				{
					sockData->clearKeepAliveCounterW();
				}
				
				info->deleteBytes(wr_len);

				if(info->getCmdLen()==0)
				{
					delete info;
					session->pop();
				}
			}

			if(session->empty())
			{
				//ALERT_INFO(m_monitor->delWrite(sock),"Remove Write Monitor Fail");
				m_monitor->delWrite(sock);
			}
		}
		else
		{
			//ASSERT_INFO("Sock Not Bind Any Session");
		}
	}
	else
	{
		//ALERT_INFO(m_monitor->delWrite(sock),"Remove Write Monitor Fail");
		m_monitor->delWrite(sock);
		//ASSERT_INFO("Sock Not Found");
	}
	return wr_len;
}

int SockConnectorEventListner::networkStatus()
{
	char cmd[64];

	if(fgets(cmd,sizeof(cmd)-1,stdin)==NULL)
	{
		return -1;
	}

	cmd[sizeof(cmd)-1]=0;
	for(unsigned int i=0;i<sizeof(cmd)&&cmd[i]!=0;i++)
	{
		if(cmd[i]=='\n')
		{
			cmd[i]=0;
			break;
		}
	}

	if(strcmp("status",cmd)==0)
	{
		m_monitor->status();
	}
	
	return 0;
}
int SockConnectorEventListner::broadcastKeepAlive(void)
{
	char msg[16];
	int len;
	
	if((len=read(m_keepAliveDetect->getReadFD(),msg,sizeof(msg)-1))<=0)
	{
		return -1;
	}

	while(len-->0)
	{
		SockMgr::getSingleton()->increaseKeepAliveCounter();
	}
	
	return 0;
}

int SockConnectorEventListner::onFDRead(int fd)
{
	static u8 buffer[MAX_READ_FIFO_BUFFER_SIZE];
	u8 *writer_point;
	int rd_len,remain_len,add_len;

	if(fd==STDIN_FILENO)
	{
		return networkStatus();
	}
	else if(fd==m_keepAliveDetect->getReadFD())
	{
		return broadcastKeepAlive();
	}
	
	remain_len=rd_len=read(fd,buffer,sizeof(buffer));

	ALERT_RET_V_E(rd_len>=0,"Read FD Fail",-1);
	writer_point = buffer;
	
	while(remain_len>0)
	{
		if(m_respose_cmd==NULL)
		{
			m_respose_cmd=CmdInfoFactory::createDefaultCmdInfo(fd);
			ALERT_RET_V(m_respose_cmd!=NULL,"Create CmdInfo Fail",-1);
		}
		
		add_len=m_respose_cmd->addContent(writer_point,remain_len);
		writer_point+=add_len;
		remain_len-=add_len;

		if(m_respose_cmd->cmdComplete())
		{
			SockData *sockData;
			Session *session;

			session=SessionMgr::getSingleton()->getSession(GET_DDWORD_BIG(m_respose_cmd->getCmd()));
			if(session!=NULL)
			{
				sockData=session->getSock();
				if(sockData!=NULL)
				{
					if(m_monitor->addWrite(sockData->getSock()))
					{
						m_respose_cmd->deleteBytes(sizeof(session_t));
						session->push(m_respose_cmd);
						m_respose_cmd=NULL;
					}
				}
				else
				{
					//ASSERT_INFO("Session Bind No Sockect");
				}
			}
			else
			{
				//ASSERT_INFO("Session Offline");
			}

			if(m_respose_cmd!=NULL)
			{
				delete m_respose_cmd;//drop directly,not cache to session
				m_respose_cmd = NULL;
			}
		}
	}
	
	return rd_len;
}
int SockConnectorEventListner::onFDWrite(int fd)
{
	return 0;
}
//file end
