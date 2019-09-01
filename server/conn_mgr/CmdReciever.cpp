#include"CmdReciever.hpp"
#include"cJSON.h"
#include"util_macro.h"

CmdReciever::CmdReciever()
{
	m_cmd_queue = NULL;
}
CmdReciever::~CmdReciever()
{
}

void CmdReciever::setQueue(SafeQueue<CmdData> *queue)
{
	m_cmd_queue = queue;
}
bool CmdReciever::init(void)
{
	return true;
}
void CmdReciever::run(void)
{
	CmdData *cmd;
	int ret;
	
	ALERT_RET(m_cmd_queue!=NULL,"No Cmd Queue");
	
	while(1)
	{
		cmd=m_cmd_queue->front();
		if(cmd!=NULL)
		{
			ret = cmdIn(cmd);
		}else
		{
			continue;
		}
		
		cJSON_Delete(cmd->cmd);
		m_cmd_queue->pop();

		if(ret!=0)
		{
			ASSERT_INFO("Handle Cmd Fail");
			//break;
		}
	}
}
//file end
