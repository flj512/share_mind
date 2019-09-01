#include<poll.h>
#include"PollSockMonitor.hpp"
#include"util_macro.h"

int PollSockMonitor::wait(void)
{
	nfds_t index=0,total=0;
	std::set<int>::iterator iterator,iterator_t;
		
	if(m_read_enable)
	{
		for(iterator=m_read_set.begin();iterator!=m_read_set.end();++iterator)
		{
			m_array[total].fd=*iterator;
			m_array[total].events=POLLIN|POLLHUP;
			m_array[total++].revents=0;
			ALERT_DO(total<sizeof(m_array)/sizeof(m_array[0]),"FD OverFlow",goto Wait);
		}
	}

	for(iterator=m_listen_set.begin();iterator!=m_listen_set.end();++iterator)
	{
		m_array[total].fd=*iterator;
		m_array[total].events=POLLIN|POLLHUP;
		m_array[total++].revents=0;
		ALERT_DO(total<sizeof(m_array)/sizeof(m_array[0]),"FD OverFlow",goto Wait);
	}

	for(iterator=m_fd_read_set.begin();iterator!=m_fd_read_set.end();++iterator)
	{
		m_array[total].fd=*iterator;
		m_array[total].events=POLLIN|POLLHUP;
		m_array[total++].revents=0;
		ALERT_DO(total<sizeof(m_array)/sizeof(m_array[0]),"FD OverFlow",goto Wait);
	}

	for(iterator=m_write_set.begin();iterator!=m_write_set.end();++iterator)
	{
		m_array[total].fd=*iterator;
		m_array[total].events=POLLOUT|POLLHUP;
		m_array[total++].revents=0;
		ALERT_DO(total<sizeof(m_array)/sizeof(m_array[0]),"FD OverFlow",goto Wait);
	}

	for(iterator=m_fd_write_set.begin();iterator!=m_fd_write_set.end();++iterator)
	{
		m_array[total].fd=*iterator;
		m_array[total].events=POLLOUT|POLLHUP;
		m_array[total++].revents=0;
		ALERT_DO(total<sizeof(m_array)/sizeof(m_array[0]),"FD OverFlow",goto Wait);
	}

Wait:
	ALERT_RET_V_E(poll(m_array,total,-1)>=0||errno==EINTR,"Pool Fail",-1);

	
	for(index=0;index<total;index++)
	{
		if(m_array[index].revents&(POLLOUT|POLLHUP))
		{
		
			if(m_fd_write_set.find(m_array[index].fd)!=m_fd_write_set.end())
			{
				m_listener->onFDWrite(m_array[index].fd);
			}
			else
			{
				m_listener->onWrite(m_array[index].fd);
			}
		}
		else if(m_array[index].revents&(POLLIN|POLLHUP))
		{
			if(m_fd_read_set.find(m_array[index].fd)!=m_fd_read_set.end())
			{
				m_listener->onFDRead(m_array[index].fd);
			}
			else if(m_listen_set.find(m_array[index].fd)!=m_listen_set.end())
			{
				m_listener->onAccept(m_array[index].fd);
			}
			else
			{
				m_listener->onRead(m_array[index].fd);
			}
		}
		
	}

	return 0;
}
//file end
