#include<new>
#include<stdio.h>
#include"CmdResolver.hpp"
#include"SockMgr.hpp"
#include"SessionMgr.hpp"
#include"util_macro.h"

CmdResolver::CmdResolver()
{
	m_queue = NULL;
}
CmdResolver::CmdResolver(SafeQueue<CmdData> *queue)
{
	m_queue=queue;
}

void CmdResolver::setQueue(SafeQueue<CmdData> *queue)
{
	m_queue = queue;
}

bool CmdResolver::sqlInjectDetect(const char *src,const char *desc)
{
	const char* key[]={"select","insert","update","drop","delete","alter","create"};

	for(unsigned int i=0;i<sizeof(key)/sizeof(key[0]);i++)
	{
		if(strstr(src,key[i])!=NULL)
		{
			DEBUG_INFO("[%s] :%s\n",desc,src);
			return true;
		}
	}

	return false;
}
void CmdResolver::onCmdIn(CmdInfo *info)
{
	CmdData cmd_data;
	
	SockData *sock_data=SockMgr::getSingleton()->getSockData(info->getSock());

	ALERT_RET(sock_data!=NULL,"Sock Not Exist");

	Session *session = sock_data->getSession();

	ALERT_RET(session!=NULL,"Sock Not Bind Any Session");

	if(sqlInjectDetect((char*)info->getCmd(),sock_data->getDesc()))
	{
		return;
	}

	cJSON *body=cJSON_Parse((char*)info->getCmd());
	ALERT_RET(body!=NULL,"Bad Json String");
	
	cmd_data.id=session->getSessionID();
	
	cmd_data.cmd=body;
	
	m_queue->push(cmd_data);
}
void CmdResolver::onConnectIn(SockData *sock_data)//bind a session
{
	char recordMsg[32];
	Session *session= SessionMgr::getSingleton()->newSession();

	snprintf(recordMsg,sizeof(recordMsg),"[%s] Connect In",sock_data->getDesc());
	DEBUG_INFO("%s\n",recordMsg);
	
	if( session!=NULL )
	{
		session->setSock(sock_data);
		sock_data->setSession(session);
	}
}
void CmdResolver::onClose(SockData *sock_data)//unbind session
{
	//char recordMsg[32];
	Session *session=sock_data->getSession();
	
	//snprintf(recordMsg,sizeof(recordMsg),"[%s] Close",sock_data->getDesc());
	//DEBUG_INFO("%s\n",recordMsg);
	
	if(session!=NULL)
	{
		sock_data->setSession(NULL);
		SessionMgr::getSingleton()->removeSession(session->getSessionID());
	}
}
SystemResourceMonitor::SystemResourceMonitor()
{
	m_queue = NULL;
}
void SystemResourceMonitor::setQueue(SafeQueue<CmdData> *queue)
{
	m_queue = queue;
}
bool SystemResourceMonitor::overflowed(void)
{
	bool ret=(m_queue->size()>MAX_RECIEVE_QUEUE_SIZE || Session::getCachedCmdCount()>MAX_REPONSE_QUEUE_SIZE);
	if(ret)
	{
		//DEBUG_INFO("overflowed,queue size=%d,response size=%d",m_queue->size(),(int)Session::getCachedCmdCount());
	}
	return ret;
}

bool SystemResourceMonitor::normalLevel(void)
{
	bool ret=(m_queue->size()<=MAX_RECIEVE_QUEUE_SIZE/2 && Session::getCachedCmdCount()<=MAX_REPONSE_QUEUE_SIZE/2);
	if(ret)
	{
		//DEBUG_INFO("normalLevel,queue size=%d,response size=%d",m_queue->size(),(int)Session::getCachedCmdCount());
	}
	return ret;
}
//file end
