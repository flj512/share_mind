#ifndef __BOOK_SHARE_MIND_SOCK_MGR__
#define __BOOK_SHARE_MIND_SOCK_MGR__

#include<map>
#include<list>
#include<pthread.h>
#include"SockData.hpp"
#include"CmdInfo.hpp"

class CmdInfoListener
{
public:
	virtual ~CmdInfoListener(){};
	virtual void onCmdIn(CmdInfo *)=0;
	virtual void onConnectIn(SockData *)=0;
	virtual void onClose(SockData *)=0;
};

class SockMgr{//Thread Safe ?

private:

	typedef std::map<int,SockData> SockMap;
	typedef std::map<int,SockData>::value_type SockMapValue;
	typedef std::map<int,SockData>::iterator SockMapIt;

	typedef std::list<CmdInfoListener*> ListenerList;
	typedef std::list<CmdInfoListener*>::iterator ListenerListIt;
	typedef std::list<CmdInfoListener*>::value_type LitenerListValue;

	SockMap m_sock_map;
	ListenerList m_listener_list;

	//
	static pthread_mutex_t m_mutex;
	static SockMgr* m_sockmgr;

	SockMgr();
	SockMgr(const SockMgr& m);
	SockMgr& operator=(const SockMgr& m);
	
	SockMapIt _insertSock(int sock,const char *desc);

	void notifyCmdIn(CmdInfo * info);
	void notifyConnect(bool in,SockData *sock_data);
	
public:

	static SockMgr* getSingleton(void);
	bool insertSock(int sock,const char *desc);
	int pushData(int sock,u8 *data,int len);
	void removeSock(int sock);
	SockData *getSockData(int sock);

	void addCmdInfoListener(CmdInfoListener *listener);
	void removeCmdInfoListener(CmdInfoListener *listener);
	void increaseKeepAliveCounter(void);
};

#endif
