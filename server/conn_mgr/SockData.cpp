#include<new>
#include<string.h>
#include"util_macro.h"
#include"SockData.hpp"


SockData::SockData(int sock,const char* desc)
{
	m_sock = sock;
	m_session = NULL;
	m_cmd_cache = NULL;
	if(desc!=NULL)
	{
		strncpy(m_desc,desc,sizeof(m_desc)-1);
		m_desc[sizeof(m_desc)-1]=0;
	}
	else
	{
		m_desc[0]=0;
	}

	m_keep_live_counter_r=0;
	m_keep_live_counter_w=0;
}
SockData::~SockData()
{
	ALERT_DO(m_session==NULL,"Do Not Detach From Session",m_session->setSock(NULL););
	
	if(m_cmd_cache!=NULL)
	{
		delete m_cmd_cache;
		m_cmd_cache = NULL;
	}
}

void SockData::setSession(Session *session)
{
	ALERT_INFO( m_session == NULL || session == NULL,"Replace Session Bind In Sock");

	m_session = session;
}
Session* SockData::getSession(void)
{
	return m_session;
}

int SockData::getSock(void)
{
	return m_sock;
}

int SockData::pushOneCmd(u8 *data,int len)
{
	int ret = -1;
			
	ALERT_RET_V(data!=NULL&&len>0,"Empty Data",ret);
	
	if(m_cmd_cache == NULL)
	{
		m_cmd_cache=CmdInfoFactory::createDefaultCmdInfo(m_sock);
		ALERT_RET_V(m_cmd_cache!=NULL,"Create CmdInfo Fail",ret);
	}

	ret = m_cmd_cache->addContent(data,len);

	return ret;
}
CmdInfo* SockData::getCmd(void)
{
	if(m_cmd_cache!=NULL && m_cmd_cache->cmdComplete())
	{
		return m_cmd_cache;
	}
	
	return NULL;
}
void SockData::releaseCmd(void)
{
	if(m_cmd_cache!=NULL&&m_cmd_cache->empty())
	{
		delete m_cmd_cache;
		m_cmd_cache = NULL;
	}
}
const char* SockData::getDesc(void)
{
	return m_desc;
}
void SockData::increaseKeepAliveCounterR(void)
{
	m_keep_live_counter_r++;
}
bool SockData::keepAliveTimeoutR(void)
{
	return m_keep_live_counter_r>MAX_KEEP_ALIVE_COUNT;
}
void SockData::clearKeepAliveCounterR(void)
{
	m_keep_live_counter_r=0;
}
void SockData::increaseKeepAliveCounterW(void)
{
	m_keep_live_counter_w++;
}
bool SockData::keepAliveTimeoutW(void)
{
	return m_keep_live_counter_w>MAX_KEEP_ALIVE_COUNT;
}
void SockData::clearKeepAliveCounterW(void)
{
	m_keep_live_counter_w=0;
}

//file end
