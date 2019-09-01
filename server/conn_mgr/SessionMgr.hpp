#ifndef __BOOK_SHARE_MIND_SESSION_MANAGE__
#define __BOOK_SHARE_MIND_SESSION_MANAGE__

#include<map>
#include<pthread.h>
#include"Session.hpp"

class SessionMgr{//Thread Safe ?

private:

	typedef std::map<session_t,Session> SessionMap;
	typedef std::map<session_t,Session>::value_type SessionMapValue;
	typedef std::map<session_t,Session>::iterator SessionMapIt;

	SessionMap m_session_map;

	static pthread_mutex_t m_mutex;
	static SessionMgr* m_mgr;
	static session_t m_current_id; 
	
	SessionMgr(void);
	SessionMgr(const SessionMgr& m);
	SessionMgr& operator=(const SessionMgr& m);
	

public:
	static SessionMgr* getSingleton(void);
	Session* newSession(void);
	void removeSession(session_t id);
	Session *getSession(session_t id);
};
#endif
