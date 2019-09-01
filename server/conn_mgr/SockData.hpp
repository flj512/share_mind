#ifndef __BOOK_SHARE_MIND_SOCKDATA__
#define __BOOK_SHARE_MIND_SOCKDATA__

#include"Session.hpp"
#include"CmdInfo.hpp"

#define SOCK_DESC_LEN		24

class Session;

class SockData{
private:

	int m_sock;
	Session *m_session;
	CmdInfo *m_cmd_cache;
	char m_desc[SOCK_DESC_LEN];
	int m_keep_live_counter_r;
	int m_keep_live_counter_w;

public:
	SockData(int sock,const char* desc);
	virtual ~SockData();
	void setSession(Session *session);
	Session* getSession(void); 

	int getSock(void);
	int pushOneCmd(u8 *data,int len);//return: <0,indicate error,>=0 indicate puch bytes count
	CmdInfo* getCmd(void);
	void releaseCmd(void);
	const char *getDesc(void);
	void increaseKeepAliveCounterR(void);
	bool keepAliveTimeoutR(void);
	void clearKeepAliveCounterR(void);
	void increaseKeepAliveCounterW(void);
	bool keepAliveTimeoutW(void);
	void clearKeepAliveCounterW(void);
	
};
#endif
