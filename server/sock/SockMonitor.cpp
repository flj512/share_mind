#include<stdlib.h>
#include<string.h>
#include"SockMonitor.hpp"
#include"SelectSockMonitor.hpp"
#include"PollSockMonitor.hpp"
#include"EpollSockMonitor.hpp"
#include"util_macro.h"

//
SockMonitorEvnetListner::SockMonitorEvnetListner(void)
{
	m_monitor = NULL;
}
SockMonitorEvnetListner::~SockMonitorEvnetListner(){};


void SockMonitorEvnetListner::setMonitor(SockMonitor *monitor)
{
	m_monitor=monitor;
}

SockMonitor::SockMonitor()
{
	m_listener = NULL;
	m_read_enable = true;
}
SockMonitor::~SockMonitor(){}

void SockMonitor::setLister(SockMonitorEvnetListner *listener)
{
	if(listener!=NULL)
	{
		m_listener = listener;
		listener->setMonitor(this);
	}
}
bool SockMonitor::setReadEnable(bool enable)
{
	if(!enable)
	{
		ASSERT_INFO("Enter Busy Mode");
	}
	else
	{
		ASSERT_INFO("Exit Busy Mode");
	}
	m_read_enable = enable;
	return true;
}
bool SockMonitor::getReadEnable(void)
{
	return m_read_enable;
}

bool SockCacheMonitor::addRead(int sock)
{
	std::pair<std::set<int>::iterator,bool> ret= m_read_set.insert(sock);

	ALERT_RET_V(ret.second || ret.first!=m_read_set.end(),"Add Read Fail",false);
	
	return true;
}

bool SockCacheMonitor::delRead(int sock)
{
	bool ret=(m_read_set.erase(sock)>0);
	
	//ALERT_INFO(ret,"Del Read Fail");
	
	return ret;
}

bool SockCacheMonitor::addWrite(int sock)
{
	std::pair<std::set<int>::iterator,bool> ret= m_write_set.insert(sock);

	ALERT_RET_V(ret.second || ret.first!=m_write_set.end(),"Add Write Fail",false);

	return true;
}

bool SockCacheMonitor::delWrite(int sock)
{
	bool ret=  (m_write_set.erase(sock)>0);
	
	//ALERT_INFO(ret,"Del Write Fail");

	return ret;
}

bool SockCacheMonitor::addListen(int sock)
{
	std::pair<std::set<int>::iterator,bool> ret=  m_listen_set.insert(sock);
	
	ALERT_RET_V(ret.second||ret.first!=m_listen_set.end(),"Add Listen Fail",false);

	return true;
}

bool SockCacheMonitor::delListen(int sock)
{
	bool ret=  (m_listen_set.erase(sock)>0);
	
	//ALERT_INFO(ret,"Del Listen Fail");

	return ret;
}

void SockCacheMonitor::status(void)
{
	DEBUG_INFO("Wait Read Count=%ld\n",m_read_set.size());
	DEBUG_INFO("Wait Write Count=%ld\n",m_write_set.size());
}

bool SockFDMonitor::addFDRead(int fd)
{
	std::pair<std::set<int>::iterator,bool>  ret=  m_fd_read_set.insert(fd);
	
	ALERT_RET_V(ret.second || ret.first!=m_fd_read_set.end(),"Add FDRead Fail",false);

	return true;
}
bool SockFDMonitor::delFDRead(int fd)
{
	bool ret=  (m_fd_read_set.erase(fd)>0);
	
	//ALERT_INFO(ret,"Del FDRead Fail");

	return ret;
}
bool SockFDMonitor::addFDWrite(int fd)
{
	std::pair<std::set<int>::iterator,bool> ret=  m_fd_write_set.insert(fd);
	
	ALERT_RET_V(ret.second || ret.first!=m_fd_write_set.end(),"Add FDWrite Fail",false);

	return true;
}
bool SockFDMonitor::delFDWrite(int fd)
{
	bool ret= ( m_fd_write_set.erase(fd)>0);
	//ALERT_INFO(ret,"Del FDWrite Fail");

	return ret;
}
ResourceMonitor::ResourceMonitor(){}
ResourceMonitor::~ResourceMonitor(){};

SockMonitor* SockMonitorFactory::createDefaultSockMonitor(void)
{
	return new EpollSockMonitor();
}
SockMonitor* SockMonitorFactory::createSockMonitor(const char *type)
{
	SockMonitor* monitor=NULL;
	
	if(type!=NULL)
	{
		if(strcmp(type,"select")==0)
		{
			monitor = new SelectSockMonitor();
		}
		else if(strcmp(type,"poll")==0)
		{
			monitor = new PollSockMonitor();
		}
		else if(strcmp(type,"epoll")==0)
		{
			monitor = new EpollSockMonitor();
		}
	}

	if(monitor==NULL)
	{
		monitor = createDefaultSockMonitor();
	}

	return monitor;

}

//file end
