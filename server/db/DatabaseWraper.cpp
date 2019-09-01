#include<stdio.h>
#include"DatabaseWraper.hpp"
#include"util_macro.h"

static const char *ERROR_STR_DB_CLOSE =	"DB Closed";
static const char *ERROR_STR_ROW_UNINIT =	"Row Not Initialized";

DatabaseWraper::DatabaseWraper(const char* addr,int port)
{
	snprintf(m_addr,sizeof(m_addr),"%s",addr);
	m_port = port;
}

DatabaseWraper::~DatabaseWraper()
{
}

MyDatabase::MyDatabase(const char* addr,int port):DatabaseWraper(addr,port)
{
	m_psql = NULL;
	m_res = NULL;
	m_field = NULL;
}
MyDatabase::~MyDatabase()
{
	close();
}
bool MyDatabase::open(const char *user,const char * passwd,const char *db)
{
    mysql_init(&m_sql);
    
	m_psql = mysql_real_connect(&m_sql,m_addr,user,passwd,db,m_port,NULL,0);

	ALERT_INFO(m_psql!=NULL,mysql_error(&m_sql));

	excute("set character_set_server=utf8");
	excute("set names utf8");
	excute("set wait_timeout=2880000");

	return (m_psql!=NULL);
}
void MyDatabase::close(void)
{
	if(m_res!=NULL)
	{
		mysql_free_result(m_res);
		m_res=NULL;
	}
	if(m_psql!=NULL)
	{
		mysql_close(&m_sql);
		m_psql=NULL;
	}

	m_row=NULL;
	m_field=NULL;
}
bool MyDatabase::selectDB(const char *db)
{
	ALERT_RET_V(m_psql!=NULL,ERROR_STR_DB_CLOSE,false);
	
	int ret=mysql_select_db(&m_sql,db);

	ALERT_INFO(ret==0,mysql_error(&m_sql));

	return ret==0;
}
bool MyDatabase::excute(const char *cmd)
{
	ALERT_RET_V(m_psql!=NULL,ERROR_STR_DB_CLOSE,false);
	
	int ret=mysql_query(&m_sql,cmd);

	ALERT_INFO(ret==0,mysql_error(&m_sql));

	return ret==0;
}
int MyDatabase::getColumNum(void)
{
	ALERT_RET_V(m_psql!=NULL,ERROR_STR_DB_CLOSE,false);

	ALERT_RET_V(m_res!=NULL,ERROR_STR_ROW_UNINIT,false);

	return (int)mysql_num_fields(m_res);
}
bool MyDatabase::initRow(void)
{
	ALERT_RET_V(m_psql!=NULL,ERROR_STR_DB_CLOSE,false);
	
	m_res = mysql_use_result(&m_sql);
	ALERT_RET_V(m_res!=NULL,mysql_error(&m_sql),false);
 
	return m_res!=NULL;
}
bool MyDatabase::selectNextRow(void)
{
	ALERT_RET_V(m_res!=NULL,ERROR_STR_ROW_UNINIT,false);

	m_row=mysql_fetch_row(m_res);
	
	/*if(m_row==NULL)//last row
	{
		mysql_free_result(m_res);
		m_res=NULL;
	}*/

	return m_row!=NULL;
}
void MyDatabase::freeRow(void)
{
	ALERT_RET(m_res!=NULL,ERROR_STR_ROW_UNINIT);
	
	mysql_free_result(m_res);
	m_res=NULL;	
}

const char* MyDatabase::getField(int column)
{
	ALERT_RET_V(m_row!=NULL,"Empty Row",NULL);

	ALERT_RET_V(column<getColumNum(),"Column Overflow",NULL);
	
	return m_row[column];
}

//file end
