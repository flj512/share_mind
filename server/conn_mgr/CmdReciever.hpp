#ifndef __BOOK_SHARE_MIND_CMD_RECIEVER__
#define __BOOK_SHARE_MIND_CMD_RECIEVER__

#include"CmdResolver.hpp"
class CmdReciever{
private:
	SafeQueue<CmdData> *m_cmd_queue;
public:
	CmdReciever();
	virtual ~CmdReciever();
	void run(void);
	void setQueue(SafeQueue<CmdData> *queue);
	virtual bool init(void);
	virtual int cmdIn(CmdData *data)=0;//return non zero,exit run loop
	
};
#endif
