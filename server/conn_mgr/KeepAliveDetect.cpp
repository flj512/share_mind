#include<unistd.h>
#include<string.h>
#include"KeepAliveDetect.hpp"
#include"com_utils.h"
#include"util_macro.h"

Pipe::Pipe(void)
{
	m_pipe[0]=-1;
	m_pipe[1]=-1;
}
Pipe:: ~Pipe(void)
{
	if(m_pipe[0]>=0)
	{
		close(m_pipe[0]);
	}

	if(m_pipe[1]>=0)
	{
		close(m_pipe[1]);
	}
}
int Pipe::getReadFD(void)
{
	return m_pipe[0];
}
int Pipe::init(void)
{
	int ret= pipe(m_pipe);
	if(ret>=0)
	{
		ret = comm_setfd_noblock(m_pipe[0]);
	}
	
	return ret;
}

KeepAliveDetect::KeepAliveDetect(int period)
{
	m_period=period;
	
}
KeepAliveDetect::~KeepAliveDetect()
{
}

void KeepAliveDetect::run(void)
{
	int len=0;
	while(1)
	{
		sleep(m_period);
		len=strlen(KEEP_ALIVE_DETECT_TEXT);
		ALERT_INFO_E(write(m_pipe[1],KEEP_ALIVE_DETECT_TEXT,len)==len,"Send Keep Alive Msg Fail");
	}
}

//
