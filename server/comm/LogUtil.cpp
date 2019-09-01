#include<pthread.h>
#include<malloc.h>
//#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<errno.h>
#include<stdlib.h>
#include<time.h>
#include<syslog.h>
#include<pthread.h>
#include"LogUtil.hpp"
#include"util_macro.h"

#define LOG_FLUSH_TIMEOUT		(60)


const char* LogUtil::m_log_path="/var/log/book-share-mind";
const char* LogUtil::m_log_file="log";
char LogUtil::m_path[256]={0};
long LogUtil::m_write_size=0;


SafeQueueLimitM<char*>* LogUtil::m_queue=NULL;
pthread_mutex_t LogUtil::m_mutex=PTHREAD_MUTEX_INITIALIZER;

FILE* LogUtil::m_file=NULL;
bool LogUtil::init(void)
{
	snprintf(LogUtil::m_path,sizeof(LogUtil::m_path),"%s/%s",m_log_path,m_log_file);
	if(access(LogUtil::m_log_path,F_OK) < 0)
	{
		syslog(LOG_ERR,"%s not exist\n",LogUtil::m_log_path);
		return false;
	}
	
	
	LogUtil::m_queue=new SafeQueueLimitM<char *>(MAX_MSG_IN_LOG_QUEUE);
	return true;

}
static void getCurrentTimeString(char *dest,int dest_size)
{
	time_t current=time(NULL);
	struct tm *ptm=localtime(&current);

	if(ptm!=NULL)
	{
		snprintf(dest,dest_size,"%04d-%02d-%02d %02d:%02d:%02d",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
	}
	else
	{
		*dest=0;
	}
}
void LogUtil::log(const char *msg)
{
	if(msg!=NULL)
	{
		char *msg_in_buff=NULL;
		int msg_len;
		char time_buffer[64];
			
		getCurrentTimeString(time_buffer,sizeof(time_buffer));

		msg_len=strlen(msg)+1+strlen(time_buffer);
		msg_in_buff=(char*)malloc(msg_len);
		if(msg_in_buff!=NULL)
		{
			snprintf(msg_in_buff,msg_len,"%s-%s",time_buffer,msg);
			if(LogUtil::m_queue->full())
			{
				//syslog(LOG_ERR,"%s","Msg Buffer Full\n");
				char *msg=LogUtil::m_queue->pop();
				if(msg!=NULL)
				{
					free(msg);
				}
			}
			LogUtil::m_queue->push(msg_in_buff);
		}
		
	}
}

void LogUtil::changeLogFile(int len)
{
	LogUtil::m_write_size+=len;
	if((LogUtil::m_write_size>MAX_LOG_SIZE_PER_FILE)&&LogUtil::m_file!=NULL)
	{
		struct stat file_stat;

		if(stat(LogUtil::m_path,&file_stat)<0)
		{
			syslog(LOG_ERR,"%s","Stat Fail\n");
			return;
		}

		if(file_stat.st_size>MAX_LOG_SIZE_PER_FILE)
		{
			char time_buffer[64];
			char new_file[128];
			//char cmd_line[256];
			fflush(LogUtil::m_file);
			if(fclose(LogUtil::m_file)<0)
			{
				syslog(LOG_ERR,"Close %s Fail\n",LogUtil::m_path);
			}
			LogUtil::m_file=NULL;
			
			getCurrentTimeString(time_buffer,sizeof(time_buffer));
			
			/*snprintf(cmd_line,sizeof(cmd_line),"tar -czf \"%s-%s.tar.gz\" \"%s\"",LogUtil::m_path,time_buffer,LogUtil::m_path);
			
			if(system(cmd_line)==0)
			{
				if(unlink(LogUtil::m_path)<0)
				{
					syslog(LOG_ERR,"Remove %s Fail\n",LogUtil::m_path);
				}
				LogUtil::m_write_size=0;
			}
			else
			{
				syslog(LOG_ERR,"System call [%s] Fail",cmd_line);
			}*/

			snprintf(new_file,sizeof(new_file),"%s-%s.txt",LogUtil::m_path,time_buffer);
			if(rename(LogUtil::m_path,new_file)<0)
			{
				syslog(LOG_ERR,"change log file Fail,dest={%s}\n",new_file);
			}
			else
			{
				LogUtil::m_write_size=0;
			}
			
		}
		
	}
}

void LogUtil::writeMsg(const char *msg)
{
	const char *enter="\n";
	if(LogUtil::m_file==NULL)
	{
		LogUtil::m_file=fopen(LogUtil::m_path,"a");
		if(LogUtil::m_file!=NULL)
		{
			struct stat file_stat;

			if(stat(LogUtil::m_path,&file_stat)>=0)
			{
				LogUtil::m_write_size=(long)file_stat.st_size;
			}
		}
	}

	if(LogUtil::m_file==NULL)
	{
		syslog(LOG_ERR,"Open Log File Fail,error=%s\n",strerror(errno));
		return;
	}

	fputs(msg,LogUtil::m_file);
	fwrite(enter,strlen(enter),1,LogUtil::m_file);
	
#ifndef RUN_AS_DEAMON	
	printf("%s\n",msg);
#endif

	changeLogFile(strlen(msg));
}

void LogUtil::run(void)
{
	char *msg=NULL;
	
	while(1)
	{
		msg=LogUtil::m_queue->pop();

		if(msg!=NULL)
		{
			pthread_mutex_lock(&LogUtil::m_mutex);
			writeMsg(msg);
			pthread_mutex_unlock(&LogUtil::m_mutex);
			free(msg);
		}
	}
}

void LogUtil::flush(void)
{
	while(1)
	{
		sleep(LOG_FLUSH_TIMEOUT);
		pthread_mutex_lock(&LogUtil::m_mutex);
		if(LogUtil::m_file!=NULL)
		{
			fflush(LogUtil::m_file);
		}
		pthread_mutex_unlock(&LogUtil::m_mutex);
	}
}

/*int DEBUG_INFO(const char *format,...)
{
	char buffer[MAX_SIZE_PER_MSG];
	int retn=0;
	va_list agrs;
	
	va_start(agrs,format);
	retn=vsnprintf(buffer,sizeof(buffer),format,agrs);
	va_end(agrs);

	LogUtil::log(buffer);
	return retn;
}*/

