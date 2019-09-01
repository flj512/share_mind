#include<unistd.h>
#include<string.h>
#include<malloc.h>
#include"CmdInfo.hpp"
#include"SockSender.hpp"
#include"util_macro.h"
#include"com_utils.h"


int CmdSender::open(void)
{
	return init();
}

int CmdSender::getMonitorFD(void)
{
	return getReadFD();
}

int CmdSender::send(session_t id,const u8 *msg_body,int len)
{
	int wr_len,msg_len,body_len;
	u8 buffer[CMD_MAX_SIZE];

	while(len>0)
	{
		msg_len=UTIL_MIN(len+CmdInfoHeaderBody::HEADER_LENGTH+sizeof(id),CMD_MAX_SIZE);
		body_len=msg_len-CmdInfoHeaderBody::HEADER_LENGTH-sizeof(id);
		
		CmdInfoHeaderBody::setHeader(buffer,msg_len-CmdInfoHeaderBody::HEADER_LENGTH);
		SET_DDWORD_BIG(buffer+CmdInfoHeaderBody::HEADER_LENGTH,id);
		
		memcpy(buffer+sizeof(id)+CmdInfoHeaderBody::HEADER_LENGTH,msg_body,body_len);
		ALERT_DO_E((wr_len=write(m_pipe[1],buffer,msg_len))==msg_len,"Particial Write",DEBUG_INFO("msg_len=%d,write_byte=%d\n",msg_len,wr_len);return -1;);
		
		msg_body+=body_len;
		len-=body_len;
	}

	return 0;
}

SockSender::SockSender()
{
	m_monitor = NULL;
}
SockSender::~SockSender()
{
	
}
void SockSender::setMonitor(SockMonitor *monitor)
{
	m_monitor = monitor;
}
SockMonitor* SockSender::getMonitor(void)
{
	return m_monitor;
}

void CmdRecieverSender::setSender(CmdSender *sender)
{
	m_sender = sender;
}

//file end
