#ifndef __BOOK_SHARE_MIND_ACCESS_DB_RECIEVER__
#define __BOOK_SHARE_MIND_ACCESS_DB_RECIEVER__

#include"CmdReciever.hpp"
#include"DatabaseWraper.hpp"
#include"SockSender.hpp"


class AccessDBReciever:public CmdRecieverSender
{
private:
	DatabaseWraper *m_db;

	inline int sendResultString(int code,const char *resp,session_t id);
	int sendJson(cJSON *json,session_t id);
	int procGetBookList(CmdData *data);
	int procGetOpinionList(CmdData *data);
	bool getPasswd(const char *user,char *passwd,int buffer_sz);
	int verifyOK(int book_id,int chapter,int start, const char *user,const char *verifyCode, char *data,const char *sha);
	bool opinionExist(int book_id,int chapter,int start,u32 hash,const char* opinion);
	int procSetOpinion(CmdData *data);
	
public:
	AccessDBReciever();
	~AccessDBReciever();

	bool init(const char *user,const char *passwd,const char *key);
	bool init(void){return true;};
	int cmdIn(CmdData *data);
};

#endif
