#include<sys/epoll.h>
#include<unistd.h>
#include"SockMonitor.hpp"
#include"EpollSockMonitor.hpp"
#include"util_macro.h"


#define epoll_del_fd(efd,fd) (epoll_ctl(efd,EPOLL_CTL_DEL,fd,NULL))

static inline int epoll_add_fd(int efd,int fd,u32 event)
{
	epoll_event evt;
	
	evt.events = event;
	evt.data.fd = fd;
	
	return epoll_ctl(efd,EPOLL_CTL_ADD,fd,&evt);
}

static inline int epoll_mod_fd(int efd,int fd,u32 event)
{
	epoll_event evt;
	
	evt.events = event;
	evt.data.fd = fd;
	
	return epoll_ctl(efd,EPOLL_CTL_MOD,fd,&evt);
}

#define epoll_del_fd_error_return(efd,fd) ALERT_RET_V_E(epoll_ctl(efd,EPOLL_CTL_DEL,fd,NULL)>=0,"Delete From Epoll Fail",false)

#define epoll_add_fd_error_return( efd, fd, event) ALERT_RET_V_E(epoll_add_fd(efd,fd,event)>=0,"Add To Epoll Fail",false)

#define epoll_mod_fd_error_return(efd, fd, event) ALERT_RET_V_E(epoll_mod_fd(efd, fd, event)>=0,"Modify Epoll Fail",false)

EpollSockMonitor::EpollSockMonitor()
{
	ALERT_INFO_E((m_efd=epoll_create(sizeof(m_array)/sizeof(m_array[0])))>=0,"Create Epoll File Fail");
}
EpollSockMonitor::~EpollSockMonitor()
{
	if(m_efd>=0)
	{
		close(m_efd);
	}
}
/*bool EpollSockMonitor::setReadEnable(bool enable)
{
	std::set<int>::iterator iterator;

	if(enable!=m_read_enable)
	{	
		ALERT_RET_V_E(close(m_efd)>=0,"Close Epoll File Fail",false);
		ALERT_RET_V_E((m_efd=epoll_create(sizeof(m_array)/sizeof(m_array[0])))>=0,"Create Epoll File Fail",false);

		
		if(enable)
		{
			for(iterator=m_read_set.begin();iterator!=m_read_set.end();++iterator)
			{
				epoll_add_fd_error_return(m_efd,*iterator,EPOLLIN);
			}
		}
		
		for(iterator=m_write_set.begin();iterator!=m_write_set.end();++iterator)
		{
			if(enable)
			{
				epoll_mod_fd_error_return(m_efd,*iterator,EPOLLIN|EPOLLOUT);
			}
			else
			{
				epoll_add_fd_error_return(m_efd,*iterator,EPOLLOUT);
			}
		}

		for(iterator=m_listen_set.begin();iterator!=m_listen_set.end();++iterator)
		{
			epoll_add_fd_error_return(m_efd,*iterator,EPOLLIN);
		}
	
		for(iterator=m_fd_read_set.begin();iterator!=m_fd_read_set.end();++iterator)
		{
			epoll_add_fd_error_return(m_efd,*iterator,EPOLLIN);
		}
	
		for(iterator=m_fd_write_set.begin();iterator!=m_fd_write_set.end();++iterator)
		{
			if(m_fd_read_set.find(*iterator)!=m_fd_read_set.end())
			{
				epoll_mod_fd_error_return(m_efd,*iterator,EPOLLIN|EPOLLOUT);
			}
			else
			{
				epoll_add_fd_error_return(m_efd,*iterator,EPOLLOUT);
			}
		}
	
	}

	SockCacheMonitor::setReadEnable(enable);
}*/
bool EpollSockMonitor::setReadEnable(bool enable)
{
	std::set<int>::iterator iterator;

	if(enable!=m_read_enable)
	{	
	
		if(enable)
		{
			for(iterator=m_read_set.begin();iterator!=m_read_set.end();++iterator)
			{
				if(epoll_add_fd(m_efd,*iterator,EPOLLIN|EPOLLHUP)<0)
				{
					ALERT_RET_V_E(errno==EEXIST,"Add Read Fail",false);
					epoll_mod_fd_error_return(m_efd,*iterator,EPOLLIN|EPOLLOUT|EPOLLHUP);
				}
			}
		}
		else
		{
			for(iterator=m_read_set.begin();iterator!=m_read_set.end();++iterator)
			{
				if(epoll_del_fd(m_efd,*iterator)<0)
				{
					ALERT_RET_V_E(errno==ENOENT,"Del Read Fail",false);
				}
				//epoll_del_fd_error_return(m_efd,*iterator)
			}

			for(iterator=m_write_set.begin();iterator!=m_write_set.end();++iterator)
			{
				epoll_add_fd_error_return(m_efd,*iterator,EPOLLOUT|EPOLLHUP);
			}
		}
		
	}

	return SockCacheMonitor::setReadEnable(enable);
}

bool EpollSockMonitor::addRead(int sock)
{
	bool ret;

	if(m_read_set.find(sock)!=m_read_set.end())
	{
		return true;
	}
	
	ret=SockCacheMonitor::addRead(sock);

	if(ret)
	{
		if(m_read_enable)
		{
			if(epoll_add_fd(m_efd,sock,EPOLLIN|EPOLLHUP)<0)
			{
				ALERT_RET_V_E(errno==EEXIST,"Add Read Fail",false);
				
				if(m_write_set.find(sock)!=m_write_set.end())
				{
					epoll_mod_fd_error_return(m_efd,sock,EPOLLIN|EPOLLOUT|EPOLLHUP);
				}
				else
				{
					epoll_mod_fd_error_return(m_efd,sock,EPOLLIN|EPOLLHUP);
				}
			}
		}
	}
	
	return ret;
}
bool EpollSockMonitor::delRead(int sock)
{
	bool ret=SockCacheMonitor::delRead(sock);

	if(ret)
	{
		if(m_write_set.find(sock)!=m_write_set.end())
		{
			epoll_mod_fd_error_return(m_efd,sock,EPOLLOUT|EPOLLHUP);
		}
		else
		{
			if(epoll_del_fd(m_efd,sock)<0)
			{
				ALERT_RET_V_E(errno==ENOENT,"Del Read Fail",false);
			}
		}
	}

	return ret;
}
bool EpollSockMonitor::addWrite(int sock)
{
	bool ret;

	if(m_write_set.find(sock)!=m_write_set.end())
	{
		return true;
	}

	ret=SockCacheMonitor::addWrite(sock);

	if(ret)
	{
		if(m_read_enable)
		{
			if(epoll_mod_fd(m_efd,sock,EPOLLIN|EPOLLOUT|EPOLLHUP)<0)
			{
				ALERT_RET_V_E(errno==ENOENT,"Modify Epoll Fail",false);

				epoll_add_fd_error_return(m_efd,sock,EPOLLOUT|EPOLLHUP);
			}
		}
		else
		{
			epoll_add_fd_error_return(m_efd,sock,EPOLLOUT|EPOLLHUP);
		}
	}

	return ret;
}
bool EpollSockMonitor::delWrite(int sock)
{
	bool ret=SockCacheMonitor::delWrite(sock);

	if(ret)
	{
		if(m_read_enable&&m_read_set.find(sock)!=m_read_set.end())
		{
			epoll_mod_fd_error_return(m_efd,sock,EPOLLIN|EPOLLHUP);
		}
		else
		{
			epoll_del_fd_error_return(m_efd,sock);
		}
	}

	return ret;
}
bool EpollSockMonitor::addListen(int sock)
{
	bool ret;

	if(m_listen_set.find(sock)!=m_listen_set.end())
	{
		return true;
	}

	ret=SockCacheMonitor::addListen(sock);

	if(ret)
	{
		epoll_add_fd_error_return(m_efd,sock,EPOLLIN|EPOLLHUP);
	}

	return ret;
}
bool EpollSockMonitor::delListen(int sock)
{
	bool ret=SockCacheMonitor::delListen(sock);

	if(ret)
	{
		epoll_del_fd_error_return(m_efd,sock);
	}

	return ret;
}
bool EpollSockMonitor::addFDRead(int fd)
{
	bool ret;

	if(m_fd_read_set.find(fd)!=m_fd_read_set.end())
	{
		return true;
	}
	
	ret=SockCacheMonitor::addFDRead(fd);

	if(ret)
	{
		if(epoll_add_fd(m_efd,fd,EPOLLIN|EPOLLHUP)<0)
		{
			ALERT_RET_V_E(errno==EEXIST,"Add Read Fail",false);

			if(m_fd_write_set.find(fd)!=m_fd_write_set.end())
			{
				epoll_mod_fd_error_return(m_efd,fd,EPOLLIN|EPOLLOUT|EPOLLHUP);
			}
			else
			{
				epoll_mod_fd_error_return(m_efd,fd,EPOLLIN|EPOLLHUP)
			}
		}
	}
	
	return ret;
}
bool EpollSockMonitor::delFDRead(int fd)
{
	bool ret=SockCacheMonitor::delFDRead(fd);
	
	
	if(m_fd_write_set.find(fd)!=m_fd_write_set.end())
	{
		epoll_mod_fd_error_return(m_efd,fd,EPOLLOUT|EPOLLHUP);
	}
	else
	{
		epoll_del_fd_error_return(m_efd,fd);
	}

	return ret;
}
bool EpollSockMonitor::addFDWrite(int fd)
{
	bool ret;

	if(m_fd_write_set.find(fd)!=m_fd_write_set.end())
	{
		return true;
	}
	
	ret=SockCacheMonitor::addFDWrite(fd);

	if(epoll_add_fd(m_efd,fd,EPOLLOUT|EPOLLHUP)<0)
	{
		ALERT_RET_V_E(errno==EEXIST,"Add Write Fail",false);

		if(m_fd_read_set.find(fd)!=m_fd_read_set.end())
		{
			epoll_mod_fd_error_return(m_efd,fd,EPOLLIN|EPOLLOUT|EPOLLHUP);
		}
		else
		{
			epoll_mod_fd_error_return(m_efd,fd,EPOLLOUT|EPOLLHUP);
		}
	}

	return ret;
}
bool EpollSockMonitor::delFDWrite(int fd)
{
	bool ret=SockCacheMonitor::delFDWrite(fd);

	if(ret)
	{
		if(m_fd_read_set.find(fd)!=m_fd_read_set.end())
		{
			epoll_mod_fd_error_return(m_efd,fd,EPOLLIN|EPOLLHUP);
		}
		else
		{
			epoll_del_fd_error_return(m_efd,fd);
		}
	}

	return ret;
}
int EpollSockMonitor::wait(void)
{
	int count;
	
	ALERT_RET_V_E((count=epoll_wait(m_efd,m_array,sizeof(m_array)/sizeof(m_array[0]),-1))>=0||errno==EINTR,"Epoll Wait Fail",-1);

	for(int index=0;index<count;index++)
	{
		if(m_array[index].events&(EPOLLIN|EPOLLHUP))
		{
			if(m_listen_set.find( m_array[index].data.fd)!=m_listen_set.end())
			{
				m_listener->onAccept(m_array[index].data.fd);
			}
			else if(m_fd_read_set.find( m_array[index].data.fd)!=m_fd_read_set.end())
			{
				m_listener->onFDRead( m_array[index].data.fd);
			}
			else
			{
				m_listener->onRead( m_array[index].data.fd);
			}
		}
		
		if(m_array[index].events&(EPOLLOUT|EPOLLHUP))
		{
			if(m_fd_write_set.find( m_array[index].data.fd)!=m_fd_write_set.end())
			{
				m_listener->onFDWrite( m_array[index].data.fd);
			}
			else
			{
				m_listener->onWrite(m_array[index].data.fd);
			}
		}
	}

	return 0;
}
//file end
