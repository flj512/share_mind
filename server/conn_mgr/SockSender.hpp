#ifndef __BOOK_SHARE_MIND_SOCK_SENDER__
#define __BOOK_SHARE_MIND_SOCK_SENDER__

#include"Session.hpp"
#include"SockMonitor.hpp"
#include"CmdReciever.hpp"
#include"KeepAliveDetect.hpp"

class CmdSender:public Pipe{
public:
	CmdSender(){};
	virtual ~CmdSender(){};
	int open(void);
	int getMonitorFD(void);
	virtual int send(session_t id,const u8 *msg_body,int len);
};

class CmdRecieverSender:public CmdReciever{
protected:
	CmdSender *m_sender;
public:
	void setSender(CmdSender *sender);
};
class SockSender{
protected:
	SockMonitor *m_monitor;
public:
	SockSender();
	virtual ~SockSender();
	void setMonitor(SockMonitor *monitor);
	SockMonitor* getMonitor(void);
	virtual void run()=0;
};

#endif
