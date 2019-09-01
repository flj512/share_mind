#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include"SockSendResponse.hpp"
#include"SockMgr.hpp"
#include"CmdInfo.hpp"
#include"SessionMgr.hpp"
#include"Session.hpp"
#include"util_macro.h"
#include"com_utils.h"
#include"SelectSockMonitor.hpp"
#include"global_limit.hpp"


SockResponseSender::SockResponseSender():m_cmd_fifo_fd(-1)
{
	m_monitor=new SelectSockMonitor();
	m_listener=new ResponseMonitorEvnetListener();
	m_monitor->setLister(m_listener);
}

SockResponseSender::~SockResponseSender()
{
	m_monitor->setLister(NULL);
	delete m_listener;
	delete m_monitor;
}
void SockResponseSender::setCmdFifo(int fd)
{
	m_cmd_fifo_fd=fd;
}

void SockResponseSender::run()
{	
	bool fifo_monited=false;
	
	ALERT_RET(m_cmd_fifo_fd>=0,"No Cmd Fifo");
	
	while(1)
	{
		
		if(!fifo_monited&&Session::getCachedCmdCount() <= MAX_REPONSE_QUEUE_SIZE/2)
		{
			m_monitor->addFDRead(m_cmd_fifo_fd);
			fifo_monited = true;
		}
		else
		{
			if(fifo_monited&&Session::getCachedCmdCount() > MAX_REPONSE_QUEUE_SIZE)
			{
				ALERT_RET(m_monitor->delFDRead(m_cmd_fifo_fd),"Del Fifo Fail");
				fifo_monited=false;
			}
		}
		
		if(m_monitor->wait()<0)
		{
			break;
		}
	}
}

ResponseMonitorEvnetListener::ResponseMonitorEvnetListener(void)
{
	m_respose_cmd = NULL;
}
ResponseMonitorEvnetListener::~ResponseMonitorEvnetListener(void)
{
	if(m_respose_cmd!=NULL)
	{
		delete m_respose_cmd;
	}
}

int ResponseMonitorEvnetListener::onAccept(int sock)
{
	return 0;
}
int ResponseMonitorEvnetListener::onRead(int sock)//read only once
{
	return 0;
}

int ResponseMonitorEvnetListener::onWrite(int sock)
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
				ALERT_RET_V_E(wr_len>=0,"Write Fail",-1);
				info->deleteBytes(wr_len);

				if(info->getCmdLen()==0)
				{
					delete info;
					session->pop();
				}
			}

			if(session->empty())
			{
				ALERT_INFO(m_monitor->delWrite(sock),"Remove Write Monitor Fail");
			}
		}
		else
		{
			ASSERT_INFO("Sock Not Bind Any Session");
		}
	}
	else
	{
		ALERT_INFO(m_monitor->delWrite(sock),"Remove Write Monitor Fail");
		ASSERT_INFO("Sock Not Found");
	}
	return wr_len;
}

int ResponseMonitorEvnetListener::onFDRead(int fd)
{
	static u8 buffer[MAX_READ_FIFO_BUFFER_SIZE];
	u8 *writer_point;
	int rd_len,remain_len,add_len;


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
					ASSERT_INFO("Session Bind No Sockect");
				}
			}
			else
			{
				ASSERT_INFO("Session Offline");
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
int ResponseMonitorEvnetListener::onFDWrite(int fd)
{
	return 0;
}

