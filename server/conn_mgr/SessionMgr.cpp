#include<new>
#include"SessionMgr.hpp"
#include"util_macro.h"

pthread_mutex_t SessionMgr::m_mutex = PTHREAD_MUTEX_INITIALIZER;
SessionMgr* SessionMgr::m_mgr = NULL;
session_t SessionMgr::m_current_id = -1;

SessionMgr::SessionMgr(void){}
SessionMgr::SessionMgr(const SessionMgr& m){}
SessionMgr& SessionMgr::operator=(const SessionMgr& m){ return *this;}

SessionMgr* SessionMgr::getSingleton(void)
{
	if(m_mgr == NULL)
	{
		pthread_mutex_lock(&m_mutex);
		if(m_mgr == NULL)
		{
			m_mgr = new SessionMgr();//do not catch exception
		}
		pthread_mutex_unlock(&m_mutex);
	}

	return m_mgr;
}

Session* SessionMgr::newSession(void)
{
	session_t id=m_current_id+1;
	Session *session = NULL;
	std::pair<SessionMapIt,bool> ret;
	
	ret=m_session_map.insert(SessionMapValue(id,Session(id)));

	if(ret.second)
	{
		m_current_id++;
		session = &ret.first->second;
	}
	else
	{
		ASSERT_INFO("New Session Fail");
	}

	return session;
}

void SessionMgr::removeSession(session_t id)
{
	if(id >= 0 )
	{
		m_session_map.erase(id);
	}
}

Session* SessionMgr::getSession(session_t id)
{
	SessionMapIt it=m_session_map.find(id);

	if(it!=m_session_map.end())
	{
		return &it->second;
	}

	return NULL;
}
//file end
