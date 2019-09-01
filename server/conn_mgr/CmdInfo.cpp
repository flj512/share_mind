#include<new>
#include<string.h>
#include<stdlib.h>
#include"CmdInfo.hpp"
#include"util_macro.h"

CmdInfo::CmdInfo(int sock)
{
	m_sock = sock;
}

CmdInfo:: ~CmdInfo()
{
}

int CmdInfo::getSock(void)
{
	return m_sock;
}

u8 CmdInfoHeaderBody::HEADER_TAG[4]={CMD_INFO_HEADER0,CMD_INFO_HEADER1,CMD_INFO_HEADER2,CMD_INFO_HEADER3};
int CmdInfoHeaderBody::HEADER_LENGTH=6;


CmdInfoHeaderBody::CmdInfoHeaderBody(int sock):CmdInfo(sock)
{
	clearData();
}

int CmdInfoHeaderBody::getCmdLen(void)
{
	return m_cmd_len-m_reader_index;
}

u8* CmdInfoHeaderBody::getCmd(void)//cmd is json string ,comfirm last positon is zero
{
	m_cmd[m_cmd_cur_len]=0;
	return m_cmd+m_reader_index;
}

int CmdInfoHeaderBody::matchHeader(u8 *content,int len)
{
	int match_len,total_match_len=0,last_cache_header_len;
	
	
	while(len>0)
	{
		last_cache_header_len=m_header_cache_len;
		match_len= UTIL_MIN(len,CmdInfoHeaderBody::HEADER_LENGTH-m_header_cache_len);
		if(match_len > 0)
		{
			memcpy(m_cmd+m_header_cache_len,content,match_len);
			m_header_cache_len+=match_len;
		}


		if(m_header_cache_len == CmdInfoHeaderBody::HEADER_LENGTH)
		{
			unsigned int header_len = GET_WORD_BIG(m_cmd+sizeof(CmdInfoHeaderBody::HEADER_TAG));
			if(memcmp(CmdInfoHeaderBody::HEADER_TAG,m_cmd,sizeof(CmdInfoHeaderBody::HEADER_TAG))!=0 || header_len >=sizeof(m_cmd)-1 || header_len==0)//max len=sizeof(m_cmd)-2
			{	
				//shift last header cache
				if(last_cache_header_len > 0)
				{
					for(int i=0;i<last_cache_header_len-1;i++)//do memmove funtion
					{
						m_cmd[i]=m_cmd[i+1];
					}
					m_header_cache_len=--last_cache_header_len;
				}
				else
				{
					content++;//forward one byte
					len--;
					total_match_len++;
					m_header_cache_len=0;
				}
				
				ASSERT_INFO("Detect Bad Header");
				//exit(0);
			}
			else//match header ok
			{
				total_match_len+=match_len;
				m_cmd_len=header_len;
				m_cmd_cur_len=0;
				m_header_cache_len=0;
				break;
			}
		}
		else
		{
			len-=match_len;
			content+=match_len;
			total_match_len+=match_len;
		}
		
	}


	return total_match_len;
}


int CmdInfoHeaderBody::addContent(u8 *content,int len)
{
	int add_len,match_len=0;

	if(cmdComplete())
	{
		return 0;
	}
	
	if(m_cmd_len==0)
	{
		match_len=matchHeader(content,len);

		len-=match_len;
		content+=match_len;
	
		if(m_cmd_len == 0)//can not match header
		{
			ALERT_INFO(len==0,"Find Bug,Header Match Not Complete");
			return match_len;
		}
	}

	add_len = UTIL_MIN(m_cmd_len-m_cmd_cur_len,len);
	memcpy(m_cmd+m_cmd_cur_len,content,add_len);
	m_cmd_cur_len+=add_len;

	return add_len+match_len;
}
		
bool CmdInfoHeaderBody::cmdComplete(void)
{
	return (m_cmd_len!=0&&m_cmd_cur_len==m_cmd_len);
}

void CmdInfoHeaderBody::clearData(void)
{
	m_cmd_cur_len = 0;
	m_cmd_len = 0;
	m_keyindex = 0;
	m_header_cache_len=0;
	m_reader_index=0;
	m_cmd[sizeof(m_cmd)-1]=0;
}

bool CmdInfoHeaderBody::empty(void)
{
	return (m_cmd_len==0&&m_header_cache_len==0);
}

void CmdInfoHeaderBody::deleteBytes(int nbytes)
{
	m_reader_index+=UTIL_MIN(nbytes,m_cmd_len-m_reader_index);
}

void CmdInfoHeaderBody::setHeader(u8 *buffer,int body_len)
{
	buffer[0] = CMD_INFO_HEADER0;
	buffer[1] = CMD_INFO_HEADER1;
	buffer[2] = CMD_INFO_HEADER2;
	buffer[3] = CMD_INFO_HEADER3;

	ALERT_INFO((unsigned int)body_len<sizeof(m_cmd)-1,"Body Data OverFlow");
	
	SET_WORD_BIG(buffer+4,body_len);
}


CmdInfo *CmdInfoFactory::createCmdInfo(const char *name,int sock)
{
	return createDefaultCmdInfo(sock);
}

CmdInfo *CmdInfoFactory::createDefaultCmdInfo(int sock)
{
	CmdInfoHeaderBody *body;

	try
	{
		body = new CmdInfoHeaderBody(sock);
		
	}catch(std::bad_alloc &e){
		return NULL;
	}

	return body;
}

//file end
