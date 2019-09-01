#ifndef __BOOK_SHARE_MIND_DATABASE__
#define __BOOK_SHARE_MIND_DATABASE__

#include<mysql/mysql.h>

#define DB_ADDR_MAX_LEN			64


class DatabaseWraper{
protected:
	char m_addr[DB_ADDR_MAX_LEN];
	int m_port;
public:
	DatabaseWraper(const char* addr,int port);
	virtual ~DatabaseWraper();

	virtual bool open(const char *user,const char *passwd,const char *db)=0;
	virtual void close(void)=0;
	virtual bool selectDB(const char *db)=0;
	virtual bool excute(const char *cmd)=0;
	virtual int getColumNum(void)=0;
	virtual bool initRow(void)=0;
	virtual bool selectNextRow(void)=0;
	virtual void freeRow(void)=0;
	virtual const char* getField(int column)=0;
};

class MyDatabase:public DatabaseWraper{
private:
	MYSQL m_sql;
	MYSQL_RES *m_res;
	MYSQL_ROW m_row;
	MYSQL_FIELD *m_field;

	MYSQL *m_psql;
public:

	MyDatabase(const char* addr,int port);
	~MyDatabase();
	bool open(const char *user,const char *passwd,const char *db);
	void close(void);
	bool selectDB(const char *db);
	bool excute(const char *cmd);
	int getColumNum(void);
	bool initRow(void);
	bool selectNextRow(void);
	void freeRow(void);
	const char* getField(int column);
};

#endif
