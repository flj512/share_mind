#include<stdio.h>
#include"Session.hpp"

u64 Session::CACHED_CMD_COUNT=0;

Session::Session(session_t id)
{
	m_session_id = id;
	m_sock = NULL;
}

Session::~Session()
{
	std::list<CmdInfo*>::iterator it;

	for(it=m_info_list.begin();it!=m_info_list.end();++it)
	{
		
		if(CACHED_CMD_COUNT > 0)
		{
			CACHED_CMD_COUNT--;
		}
		delete *it;
	}

}

session_t Session::getSessionID()
{
	return m_session_id;
}

void Session::setSock(SockData * sock)
{
	if(sock!=m_sock)
	{
		m_sock = sock;
	}
}
SockData * Session::getSock(void)
{
	return m_sock;
}
void Session::push(CmdInfo *info)
{
	CACHED_CMD_COUNT++;
	m_info_list.push_back(info);
}

CmdInfo* Session::front()
{
	return m_info_list.front();
}
void Session::pop()
{
	if(CACHED_CMD_COUNT > 0)
	{
		CACHED_CMD_COUNT--;
	}
	
	m_info_list.pop_front();
}
bool Session::empty()
{
	return m_info_list.size()==0;
}
u64 Session::getCachedCmdCount(void)
{
	return CACHED_CMD_COUNT;
}

//file end
