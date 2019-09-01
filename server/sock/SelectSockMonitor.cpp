#include"SelectSockMonitor.hpp"
#include"util_macro.h"

int SelectSockMonitor::wait(void)
{
	int max_sock=0;
	std::set<int>::iterator iterator,iterator_t;
	
	FD_ZERO(&m_read);
	FD_ZERO(&m_write);

	if(m_read_enable)
	{
		for(iterator=m_read_set.begin();iterator!=m_read_set.end();++iterator)
		{
			FD_SET(*iterator,&m_read);
			max_sock = UTIL_MAX(max_sock,*iterator);
		}
	}

	for(iterator=m_listen_set.begin();iterator!=m_listen_set.end();++iterator)
	{
		FD_SET(*iterator,&m_read);
		max_sock = UTIL_MAX(max_sock,*iterator);
	}

	for(iterator=m_fd_read_set.begin();iterator!=m_fd_read_set.end();++iterator)
	{
		FD_SET(*iterator,&m_read);
		max_sock = UTIL_MAX(max_sock,*iterator);
	}

	for(iterator=m_write_set.begin();iterator!=m_write_set.end();++iterator)
	{
		FD_SET(*iterator,&m_write);
		max_sock = UTIL_MAX(max_sock,*iterator);
	}

	for(iterator=m_fd_write_set.begin();iterator!=m_fd_write_set.end();++iterator)
	{
		FD_SET(*iterator,&m_write);
		max_sock = UTIL_MAX(max_sock,*iterator);
	}
	
	ALERT_RET_V_E(select(max_sock+1,&m_read,&m_write,NULL,NULL)>=0||errno==EINTR,"Select Fail",-1);

	for(iterator=m_read_set.begin();iterator!=m_read_set.end();/*++iterator*/) //onRead/onAccept/onWrite.... May Modified set
	{
		iterator_t=iterator++;
		if(FD_ISSET(*iterator_t,&m_read))
		{
			if(m_listener!=NULL) 
			{
				m_listener->onRead(*iterator_t);
			}
		}
	}

	for(iterator=m_fd_read_set.begin();iterator!=m_fd_read_set.end();/*++iterator*/) 
	{
		iterator_t=iterator++;
		if(FD_ISSET(*iterator_t,&m_read))
		{
			if(m_listener!=NULL) 
			{
				m_listener->onFDRead(*iterator_t);
			}
		}
	}

	for(iterator=m_write_set.begin();iterator!=m_write_set.end();/*++iterator*/)
	{
		iterator_t=iterator++;
		if(FD_ISSET(*iterator_t,&m_write))
		{
		
			if(m_listener!=NULL) 
			{
				m_listener->onWrite(*iterator_t);
			}
		}
	}

	for(iterator=m_fd_write_set.begin();iterator!=m_fd_write_set.end();/*++iterator*/)
	{
		iterator_t=iterator++;
		if(FD_ISSET(*iterator_t,&m_write))
		{
		
			if(m_listener!=NULL) 
			{
				m_listener->onFDWrite(*iterator_t);
			}
		}
	}


	for(iterator=m_listen_set.begin();iterator!=m_listen_set.end();/*++iterator*/)
	{
		iterator_t=iterator++;
		if(FD_ISSET(*iterator_t,&m_read))
		{
			if(m_listener!=NULL) 
			{
				m_listener->onAccept(*iterator_t);
			}
		}
	}

	return 0;
	
}

//file end
