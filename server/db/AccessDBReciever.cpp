#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include"AccessDBReciever.hpp"
#include"cJSON.h"
#include"LibMd5.h"
#include"util_macro.h"
#include"com_utils.h"

#define USE_DB_NAME					"BookShareMind"

//tag list
#define TAG_CMD						"CMD"
#define TAG_BOOK_LIST_KEY			"BookListKey"
#define TAG_BOOK_INFO_ID			"ID"
#define TAG_BOOK_INFO_AUTHOR		"Author"
#define TAG_BOOK_INFO_NAME			"Name"
#define TAG_BOOK_INFO_PATH			"Path"
#define TAG_BOOK_OPINION_CHAPTER	"chapter"
#define TAG_BOOK_OPINION_START		"start"
#define TAG_BOOK_OPINION_END		"end"
#define TAG_BOOK_OPINION			"opinion"
#define TAG_BOOK_RESULT				"result"
#define TAG_BOOK_USER				"user"
#define TAG_BOOK_VERIFY_CODE		"verifyCode"
#define TAG_BOOK_SHA				"sha"
#define TAG_BOOK_RESULT_CODE		"code"


//cmd list
#define CMD_GET_BOOK_LIST			"GetBookList"
#define CMD_GET_OPINION_LIST		"GetOpinionList"
#define CMD_SET_OPINION				"SetOpionion"

//response
#define CMD_RESPONSE_FAIL			"Fail"
#define CMD_RESPONSE_SUCCESS		"Success"

#define DEFAULT_OPINION_SIZE		10
#define DEFAULT_BOOK_SIZE			20

typedef enum{
	CODE_SUCCESS=0,
	CODE_DB=1,
	CODE_PARAM=2,
	CODE_NOTEXIST=3,
	CODE_AUTH=4,
	CODE_TIMESTAMP=5,
	CODE_DUPLITE=6
}ServerRespCode;

AccessDBReciever::AccessDBReciever()
{
	m_db = new MyDatabase("localhost",0);
}

AccessDBReciever::~AccessDBReciever()
{
	if(m_db!=NULL)
	{
		delete m_db;
	}
}

bool AccessDBReciever::init(const char *user,const char *passwd,const char *key)
{
	bool ret=false;
	char *usr,*pd;
	char k[]={"Og6u+cB2nHigl0R="};
	const char *pk;

    pk=(key==NULL?k:key);
	usr=aes_decrypt_char(user,pk);
	pd=aes_decrypt_char(passwd,pk);

	if(usr!=NULL&&pd!=NULL)
	{
		ret = m_db->open(usr,pd,USE_DB_NAME);
		if(ret)
		{
			//ret=m_db->selectDB(USE_DB_NAME);
		}
		
		memset(usr,0,strlen(usr));
		memset(pd,0,strlen(pd));
		free(usr);
		free(pd);
	}
	else
	{
		ASSERT_INFO("Verify Fail");
	}

	memset(k,0,sizeof(k));

	return ret;
}
int AccessDBReciever::cmdIn(CmdData *data)
{
	int ret=-1;
	cJSON *cmd;

	ALERT_RET_V(data!=NULL&&data->cmd!=NULL,"Bad Command",ret);

	cmd=cJSON_GetObjectItem(data->cmd,TAG_CMD);
	if(cmd==NULL||cmd->type!=cJSON_String)
	{
		return ret;
	}

	if(strcmp(cmd->valuestring,CMD_GET_BOOK_LIST)==0)
	{
		ret=procGetBookList(data);
	}
	else if(strcmp(cmd->valuestring,CMD_GET_OPINION_LIST)==0)
	{
		ret=procGetOpinionList(data);
	}
	else if(strcmp(cmd->valuestring,CMD_SET_OPINION)==0)
	{
		ret=procSetOpinion(data);
	}
	
	return ret;
}
int static sql_strcpy_safe(char *dest,int dest_size,const char *src)//return used size of buffer
{
	int i=0;
	
	for(;i<dest_size-1&&*src!=0;i++)
	{
		if(*src=='\''||*src=='\\'||*src=='%'||*src=='_')
		{
			if(i==dest_size-2)
			{
				break;
			}
			else
			{
				dest[i++]='\\';
			}
		}
		
		dest[i]=*src++;

	}

	dest[i]=0;
	
	return i;
}
int AccessDBReciever::sendResultString(int code,const char *resp,session_t id)
{
	int ret=-1;
	cJSON *ret_json=cJSON_CreateObject();

	ALERT_RET_V(ret_json!=NULL,"Create JSON Array Fail",ret);

	cJSON_AddNumberToObject(ret_json,TAG_BOOK_RESULT_CODE,code);
	cJSON_AddStringToObject(ret_json,TAG_BOOK_RESULT,resp);
	ret=sendJson(ret_json,id);
	return ret;
}
int AccessDBReciever::sendJson(cJSON *json,session_t id)
{
	int ret=-1;
	char *response=cJSON_PrintUnformatted(json);
	
	if(response!=NULL)
	{
		int resp_len=strlen(response);
		response[resp_len]='\n';//replace 0 to '\n'
		
		ret=m_sender->send(id,(u8*)response,resp_len+1);
		
		cJSON_Free(response);
		
	}
	return ret;
}

int AccessDBReciever::procGetBookList(CmdData *data)
{
	int ret=-1;
	const char *sql="select id,name,author,path from bookinfo";
	char sql_buffer[256];
	const char *sql_stmt=sql;

	cJSON *key=cJSON_GetObjectItem(data->cmd,TAG_BOOK_LIST_KEY);
	
	if(key!=NULL&&key->type==cJSON_String)
	{
		unsigned int n=snprintf(sql_buffer,sizeof(sql_buffer),"%s where name like \'%%",sql);
		n+=sql_strcpy_safe(sql_buffer+n,sizeof(sql_buffer)-n,key->valuestring);
		n+=snprintf(sql_buffer+n,sizeof(sql_buffer)-n,"%%\' order by create_time desc limit %d",DEFAULT_BOOK_SIZE);

		if(n<sizeof(sql_buffer))// (n>=sizeof(sql_buffer)) may cause unsafe query
		{
			sql_stmt=sql_buffer;
		}
	}

	if(sql_stmt==sql)//default
	{
		unsigned int n=snprintf(sql_buffer,sizeof(sql_buffer),"%s order by create_time desc limit %d",sql,DEFAULT_BOOK_SIZE);
		if(n<sizeof(sql_buffer))// (n>=sizeof(sql_buffer)) may cause unsafe query
		{
			sql_stmt=sql_buffer;
		}
		else
		{
			return ret;
		}
	}
	
	//ASSERT_INFO(sql_stmt);

	if(m_db->excute(sql_stmt))
	{
		cJSON *response=cJSON_CreateArray();
		cJSON *item=NULL;
		const char *field;

		ALERT_RET_V(response!=NULL,"Create JSON Array Fail",ret);

		if(!m_db->initRow())
		{
			cJSON_Delete(response);
			return sendResultString(CODE_DB,CMD_RESPONSE_FAIL,data->id);
		}
		

		while(m_db->selectNextRow())
		{
			item=cJSON_CreateObject();
			ALERT_DO(item!=NULL,"Create JSON Object Fail",goto out);

			field=m_db->getField(0);
			if(field)
			{
				cJSON_AddStringToObject(item,TAG_BOOK_INFO_ID,field);
			}

			field=m_db->getField(1);
			if(field)
			{
				cJSON_AddStringToObject(item,TAG_BOOK_INFO_NAME,field);
			}
			
			field=m_db->getField(2);
			if(field)
			{
				cJSON_AddStringToObject(item,TAG_BOOK_INFO_AUTHOR,field);
			}
			
			field=m_db->getField(3);
			if(field)
			{
				cJSON_AddStringToObject(item,TAG_BOOK_INFO_PATH,field);
			}

			cJSON_AddItemToArray(response,item);
			item=NULL;
		
		}

		ret=sendJson(response,data->id);
	out:
		m_db->freeRow();
		if(item!=NULL) 
		{
			cJSON_Delete(item);
		}
		if(response!=NULL)
		{
			cJSON_Delete(response);
		}
		
	}
	else
	{
		ret=sendResultString(CODE_DB,CMD_RESPONSE_FAIL,data->id);
	}

	return ret;
	
}
static inline bool getJsonIntItem(cJSON *root,const char* tag,int& ret)
{
	cJSON *json=NULL;
	
	json=cJSON_GetObjectItem(root,tag);
	if(json!=NULL&&json->type==cJSON_Number)
	{
		ret=json->valueint;
		return true;
	}
	else
	{
		return false;
	}
}
static inline bool getJsonStringItem(cJSON *root,const char* tag,char*& ret)
{
	cJSON *json=NULL;
	
	json=cJSON_GetObjectItem(root,tag);
	if(json!=NULL&&json->type==cJSON_String)
	{
		ret=json->valuestring;
		return true;
	}
	else
	{
		return false;
	}
}

int AccessDBReciever::procGetOpinionList(CmdData *data)
{
	int ret=-1;
	int chapter_id,start,end,book_id;
	unsigned int n;
	char sql_buffer[256];
	int code=CODE_PARAM;
	
	if(!getJsonIntItem(data->cmd,TAG_BOOK_OPINION_CHAPTER,chapter_id))
	{
		goto fail_out;
	}
	if(!getJsonIntItem(data->cmd,TAG_BOOK_OPINION_START,start))
	{
		goto fail_out;
	}
	if(!getJsonIntItem(data->cmd,TAG_BOOK_OPINION_END,end))
	{
		goto fail_out;
	}
	if(!getJsonIntItem(data->cmd,TAG_BOOK_INFO_ID,book_id))
	{
		goto fail_out;
	}

	n=snprintf(sql_buffer,sizeof(sql_buffer),"select opinion from bookopinion where book_id=%d and chapter=%d and position between %d and %d order by position,update_time desc limit %d",book_id,chapter_id,start,end,DEFAULT_OPINION_SIZE);
	if(n>=sizeof(sql_buffer))
	{
		goto fail_out;
	}

	code=CODE_DB;
	if(m_db->excute(sql_buffer))
	{
		cJSON *response=cJSON_CreateArray();
		cJSON *item=NULL;
		const char *field;

		ALERT_RET_V(response!=NULL,"Create JSON Array Fail",ret);
		
		if(!m_db->initRow())
		{
			cJSON_Delete(response);
			goto fail_out;
		}

		while(m_db->selectNextRow())
		{
			item=cJSON_CreateObject();

			ALERT_DO(item!=NULL,"Create JSON Object Fail",goto out;);

			field=m_db->getField(0);
			if(field!=NULL)
			{
				cJSON_AddStringToObject(item,TAG_BOOK_OPINION,field);
			}

			cJSON_AddItemToArray(response,item);
			item=NULL;
		}
		ret=sendJson(response,data->id);
	out:
		m_db->freeRow();
		if(item!=NULL)
		{
			cJSON_Delete(item);
		}
		if(response!=NULL)
		{
			cJSON_Delete(response);
		}
	}
	else
	{
		goto fail_out;
	}

	return ret;

fail_out:
	return sendResultString(code,CMD_RESPONSE_FAIL,data->id);
}
bool AccessDBReciever::getPasswd(const char *user,char *passwd,int buffer_sz)
{
	char sql_buffer[128];
	unsigned int n=0;
	const char *ret=NULL;

	n=snprintf(sql_buffer,sizeof(sql_buffer),"select passwd from userinfo where id=\'");
	n+=sql_strcpy_safe(sql_buffer+n,sizeof(sql_buffer)-n,user);
	n+=snprintf(sql_buffer+n,sizeof(sql_buffer)-n,"\'");
	if(n>=sizeof(sql_buffer))
	{
		return NULL;
	}
	
	if(m_db->excute(sql_buffer))
	{
		if(m_db->initRow())
		{
			if(m_db->selectNextRow())
			{
				ret= m_db->getField(0);
				snprintf(passwd,buffer_sz,"%s",ret);
			}
			m_db->freeRow();
		}
	}

	return (ret!=NULL);
}
bool AccessDBReciever::opinionExist(int book_id,int chapter,int start,u32 hash,const char* opinion)
{
	char sql_buffer[160];
	unsigned int n=0;
	bool ret=false;
	const char *opinion_in_db=NULL;

	n=snprintf(sql_buffer,sizeof(sql_buffer),"select opinion from bookopinion where book_id=%d and chapter=%d and position=%d and opinion_hash=%u",book_id,chapter,start,hash);
	
	if(n>=sizeof(sql_buffer))
	{
		return false;
	}
	
	if(m_db->excute(sql_buffer))
	{
		if(m_db->initRow())
		{
			if(m_db->selectNextRow())
			{
				opinion_in_db=m_db->getField(0);
				if(opinion_in_db!=NULL)
				{
					ret=(strcmp(opinion_in_db,opinion)==0);
				}
			}
			m_db->freeRow();
		}
	}

	return ret;
}

int AccessDBReciever::verifyOK(int book_id,int chapter,int start, const char *user,const char *verifyCode,char *data,const char *sha)
{
	u64 timestamp;
	char buffer[128];
	char passwd[36];
	char hash_str[MD5_HASH_SIZE*2+1];
	u64 current=(u64)time(NULL);
	char* pcharA[]={buffer,data};
	int code=CODE_AUTH;

	timestamp=getLong(verifyCode);
	if(timestamp + MAX_TIMESTAMP_EXPIRE < current)
	{
		//DEBUG_INFO("Timestamp Expire,local=%ld,remote=%ld",current,timestamp);
		printf("Timestamp Expire,local=%ld,remote=%ld\n",current,timestamp);
		code=CODE_TIMESTAMP;
		return code;
	}
	if(!getPasswd(user,passwd,sizeof(passwd)))
	{
		return code;
	}
	snprintf(buffer,sizeof(buffer),"%d%d%d%s%s%s",book_id,chapter,start,user,passwd,verifyCode);
	memset(passwd,0,sizeof(passwd));
	
	calcMultiStringMD5(pcharA,sizeof(pcharA)/sizeof(pcharA[0]),hash_str,sizeof(hash_str));
	
	if(strcasecmp(sha,hash_str)==0)
	{
		code=CODE_SUCCESS;
	}

	return code;
}
int AccessDBReciever::procSetOpinion(CmdData *data)
{
	int chapter_id,start,book_id;
	unsigned int n,crc32;
	char sql_buffer[1024],*opinion,*user,*verifyCode,*sha;
	const char *resp=CMD_RESPONSE_FAIL;
	int code=CODE_PARAM;
	
	if(!getJsonIntItem(data->cmd,TAG_BOOK_OPINION_CHAPTER,chapter_id))
	{
		goto out;
	}
	if(!getJsonIntItem(data->cmd,TAG_BOOK_INFO_ID,book_id))
	{
		goto out;
	}
	if(!getJsonStringItem(data->cmd,TAG_BOOK_OPINION,opinion))
	{
		goto out;
	}
	if(!getJsonStringItem(data->cmd,TAG_BOOK_USER,user))
	{
		goto out;
	}
	if(!getJsonStringItem(data->cmd,TAG_BOOK_VERIFY_CODE,verifyCode))
	{
		goto out;
	}
	if(!getJsonStringItem(data->cmd,TAG_BOOK_SHA,sha))
	{
		goto out;
	}
	if(!getJsonIntItem(data->cmd,TAG_BOOK_OPINION_START,start))
	{
		goto out;
	}
	if(start<0||chapter_id<0)
	{
		goto out;
	}
	if((code=verifyOK(book_id,chapter_id,start,user,verifyCode,opinion,sha))!=CODE_SUCCESS)
	{
		DEBUG_INFO("user=%s,verify fail",user);
		goto out;
	}

	crc32=calcCrc32(opinion);
	if(opinionExist(book_id,chapter_id,start,crc32,opinion))
	{
		code=CODE_DUPLITE;
		goto out;
	}
	//when verify is ok,then user string is safe
	n=snprintf(sql_buffer,sizeof(sql_buffer),"insert into bookopinion(book_id,chapter,position,opinion_hash,user,opinion) values(%d,%d,%d,%u,\'%s\',\'",book_id,chapter_id,start,crc32,user);
	n+=sql_strcpy_safe(sql_buffer+n,sizeof(sql_buffer)-n,opinion);
	n+=snprintf(sql_buffer+n,sizeof(sql_buffer)-n,"\')");
	
	if(n>=sizeof(sql_buffer))
	{
		goto out;
	}

	//printf("sql_cmd=%s\n",sql_buffer);

	if(m_db->excute(sql_buffer))
	{
		code=CODE_SUCCESS;
		resp=CMD_RESPONSE_SUCCESS;
	}
	else
	{
		code=CODE_DB;
	}
out:
	return sendResultString(code,resp,data->id);
}

//file end
