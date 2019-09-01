#ifndef __BOOK_SHARE_MIND_CMD_INFO__
#define __BOOK_SHARE_MIND_CMD_INFO__

#include"types_d.h"

#define	 CMD_MAX_SIZE					1024

#define  CMD_INFO_HEADER0				0x5a
#define  CMD_INFO_HEADER1				0xb7
#define  CMD_INFO_HEADER2				0xd0
#define  CMD_INFO_HEADER3				0x2e

class CmdInfo{
private:
	int m_sock;
public:
	CmdInfo(int sock);
	virtual ~CmdInfo();
	int getSock(void);
	virtual int getCmdLen(void)=0;
	virtual u8* getCmd(void)=0;

	virtual int addContent(u8 *content,int len)=0;//return scaned bytes
	
	virtual bool cmdComplete(void)=0;

	virtual void clearData(void)=0;

	virtual bool empty(void)=0;

	virtual void deleteBytes(int nbytes)=0;
};

class CmdInfoHeaderBody:public CmdInfo{
private:
	static u8 HEADER_TAG[4];
	int m_cmd_len;
	int m_cmd_cur_len;
	int m_keyindex;
	int m_header_cache_len;
	int m_reader_index;
	u8 m_cmd[CMD_MAX_SIZE];

	int matchHeader(u8 *content,int len);
public:
	static int HEADER_LENGTH;
	
	CmdInfoHeaderBody(int sock);
	
	int getCmdLen(void);
	u8* getCmd(void);

	int addContent(u8 *content,int len);
	
	bool cmdComplete(void);

	void clearData(void);

	bool empty(void);

	void deleteBytes(int nbytes);

	static void setHeader(u8 *buffer,int body_len);
	
};

class CmdInfoFactory{
public:
	static CmdInfo *createCmdInfo(const char *name,int sock);
	static CmdInfo *createDefaultCmdInfo(int sock);
};
#endif
