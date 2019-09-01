#ifndef __BOOK_SHARE_MIND_SESSION__
#define __BOOK_SHARE_MIND_SESSION__

#include<list>
#include"types_d.h"
#include"SockData.hpp"
#include"CmdInfo.hpp"

typedef s64 session_t;

class SockData;

class Session{
private:
	session_t m_session_id;
	SockData *m_sock;
	std::list<CmdInfo*> m_info_list;
	
	static u64 CACHED_CMD_COUNT;
public:
	Session(session_t id);
	virtual ~Session();
	session_t getSessionID();

	void setSock(SockData *sock);
	SockData * getSock(void);
	void push(CmdInfo *info);
	CmdInfo* front();
	void pop();
	bool empty();
	static u64 getCachedCmdCount(void);

};


#endif
