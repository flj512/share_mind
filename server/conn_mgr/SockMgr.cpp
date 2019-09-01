#include<sys/socket.h>
#include"util_macro.h"
#include"SockMgr.hpp"
#include"CmdInfo.hpp"
#include"global_limit.hpp"


using namespace std;

pthread_mutex_t SockMgr::m_mutex=PTHREAD_MUTEX_INITIALIZER;
SockMgr* SockMgr::m_sockmgr=NULL;

SockMgr::SockMgr(){}
SockMgr::SockMgr(const SockMgr& m){}
SockMgr& SockMgr::operator=(const SockMgr& m){return *this;}

SockMgr* SockMgr::getSingleton(void)
{
	if(m_sockmgr == NULL)
	{
		pthread_mutex_lock(&m_mutex);
		if(m_sockmgr == NULL)
		{
			m_sockmgr = new SockMgr();//do not catch exception
		}
		pthread_mutex_unlock(&m_mutex);
	}

	return m_sockmgr;
}


int SockMgr::pushData(int sock,u8 *data,int len)
{
	int push_len;
	SockMapIt it=_insertSock(sock,NULL);
	SockData *sockdata;
	CmdInfo *info;
	
	ALERT_RET_V(it != m_sock_map.end(),"Socket Not Found!!",-1);

	sockdata=&it->second;
	if(len>0)
	{
		sockdata->clearKeepAliveCounterR();
	}

	while( len > 0)
	{
		push_len=sockdata->pushOneCmd(data,len);
		if(push_len < 0)
		{
			break;
		}
	
		info=sockdata->getCmd();

		if(info!=NULL)
		{
			notifyCmdIn(info);
			info->clearData();
		}

		len-=push_len;
		data+=push_len;
	}

	sockdata->releaseCmd();

	return len;
}

SockMgr::SockMapIt SockMgr::_insertSock(int sock,const char *desc)
{
	SockMapIt it = m_sock_map.find(sock);
	std::pair<SockMapIt,bool> ret;

	if( it == m_sock_map.end())
	{
		ALERT_RET_V(m_sock_map.size()<MAX_CONNECT_COUNT,"Sock Full",it);
		
		ret = m_sock_map.insert(SockMapValue(sock,SockData(sock,desc)));
		if(ret.second)
		{
			it = ret.first;
		}
	}

	return it;
}
bool SockMgr::insertSock(int sock,const char *desc)
{
	bool ret=false;
	
	SockMapIt it = _insertSock(sock,desc);

	ret=(it!=m_sock_map.end());

	if(ret)
	{
		notifyConnect(true,&it->second);
	}
	
	return ret;
}

void SockMgr::removeSock(int sock)
{
	SockMapIt it = m_sock_map.find(sock);

	if(it!=m_sock_map.end())
	{
		notifyConnect(false,&it->second);
		m_sock_map.erase(sock);
	}
}

SockData* SockMgr::getSockData(int sock)
{
	SockMapIt it = m_sock_map.find(sock);

	if(it!=m_sock_map.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}

void SockMgr::addCmdInfoListener(CmdInfoListener *listener)
{
	m_listener_list.push_back(listener);
}
void SockMgr::removeCmdInfoListener(CmdInfoListener *listener)
{
	m_listener_list.remove(listener);
}

void SockMgr::notifyCmdIn(CmdInfo * info)
{
	ListenerListIt it;

	for(it = m_listener_list.begin();it!=m_listener_list.end();++it)
	{
		(*it)->onCmdIn(info);
	}
}

void SockMgr::notifyConnect(bool in,SockData *sock_data)
{
	ListenerListIt it;

	for(it = m_listener_list.begin();it!=m_listener_list.end();++it)
	{
		if(in)
		{
			(*it)->onConnectIn(sock_data);
		}
		else
		{
			(*it)->onClose(sock_data);
		}
	}
}
void SockMgr::increaseKeepAliveCounter(void)
{
	SockMapIt it;
	SockData *sockdata;

	for(it=m_sock_map.begin();it!=m_sock_map.end();++it)
	{
		sockdata=&it->second;
		sockdata->increaseKeepAliveCounterW();
		sockdata->increaseKeepAliveCounterR();

		if(sockdata->keepAliveTimeoutR()||sockdata->keepAliveTimeoutW())
		{
			DEBUG_INFO("Sock %d timeout\n",sockdata->getSock());
			shutdown(sockdata->getSock(),SHUT_RDWR);
		}
	}
}

//file end

