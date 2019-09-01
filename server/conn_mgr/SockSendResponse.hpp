#ifndef __BOOK_SHARE_MIND_RESPONSE_SENDER__
#define __BOOK_SHARE_MIND_RESPONSE_SENDER__

#include"SockSender.hpp"
#include"SockMonitor.hpp"
#include"CmdInfo.hpp"

class ResponseMonitorEvnetListener:public SockMonitorEvnetListner{
private:
	CmdInfo *m_respose_cmd;
	
public:
	ResponseMonitorEvnetListener(void);
	~ResponseMonitorEvnetListener(void);
	int onAccept(int sock);
	int onRead(int sock);
	int onWrite(int sock);
	int onFDRead(int fd);
	int onFDWrite(int fd);
};

class SockResponseSender:public SockSender{
private:
	SockMonitorEvnetListner *m_listener;
	int m_cmd_fifo_fd;

public:
	SockResponseSender();
	~SockResponseSender();
	void setCmdFifo(int fd);
	void run(void);
};
#endif
