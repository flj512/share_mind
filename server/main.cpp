#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include<syslog.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/resource.h>
#include<pwd.h>
#include"CmdInfo.hpp"
#include"util_macro.h"
#include"SockMgr.hpp"
#include"SockListen.hpp"
#include"SockMonitor.hpp"
#include"SockConnector.hpp"
#include"CmdResolver.hpp"
#include"SockSender.hpp"
#include"CmdReciever.hpp"
#include"SockSendResponse.hpp"
#include"AccessDBReciever.hpp"
#include"LogUtil.hpp"
#include"KeepAliveDetect.hpp"


#define APPLICATION_PORT		60000

class Application{
private:
	SockConnectorListen *m_listen;
	CmdResolver *m_cmd_listener;
	SafeQueue<CmdData> *m_cmd_queue;
	CmdSender *m_cmd_sender;
	AccessDBReciever *m_reciever_and_sender;
	SystemResourceMonitor *m_resource_monitor;
	KeepAliveDetect *m_detect;
	pthread_t m_log_thread;
	pthread_t m_log_flush_thread;
	
	static void* connection_loop(void* args)
	{
		SockListen *listen=(SockListen *)args;

		listen->run();

		return NULL;
	}	

	static void*	cmd_hander_loop(void *args)
	{
		CmdReciever *reciever=(CmdReciever *)args;
	
		reciever->run();
		
		return NULL;
	}

	static void* log_system_loop(void *args)
	{
		LogUtil::run();
		return NULL;
	}
	static void* log_system_flush_loop(void *args)
	{
		LogUtil::flush();
		return NULL;
	}

	static void* keep_alive_detect_loop(void *args)
	{
		KeepAliveDetect *detect=(KeepAliveDetect*)args;
		
		detect->run();
		return NULL;
	}
public:
	Application(int port,const char *ipv4_addr,const char *ipv6_addr)
	{
		m_listen=new SockConnectorListen(port,ipv4_addr,ipv6_addr);
		m_cmd_listener = new CmdResolver();
		m_cmd_queue = new SafeQueueUnlimit<CmdData>();
		m_cmd_sender = new CmdSender();
		m_reciever_and_sender=new AccessDBReciever();
		m_resource_monitor=new SystemResourceMonitor();
		m_detect=new KeepAliveDetect(KEEP_ALIVE_DECTECT_PERIOD);
	}

	bool init(const char *user,const char *passwd,const char *key)
	{
		if(!LogUtil::init())
		{
			syslog(LOG_ERR,"Init Log Fail");
			return false;
		}
		if(pthread_create(&m_log_thread,NULL,log_system_loop,NULL)!=0)
		{
			syslog(LOG_ERR,"Init Log Fail");
			return false;
		}
		if(pthread_create(&m_log_flush_thread,NULL,log_system_flush_loop,NULL)!=0)
		{
			syslog(LOG_ERR,"Init Log Flush Fail");
			return false;
		}
		
		ALERT_RET_V_E(m_cmd_sender->open()>=0,"Open Sender Fail",false);
		
		ALERT_RET_V_E(m_reciever_and_sender->init(user,passwd,key),"Init Fail",false);

		ALERT_RET_V(m_detect->init()>=0,"Keep Alive Detect Init Fail",false);
		m_reciever_and_sender->setSender(m_cmd_sender);
		m_reciever_and_sender->setQueue(m_cmd_queue);

		m_resource_monitor->setQueue(m_cmd_queue);
		m_listen->setResourceMonitor(m_resource_monitor);
		m_listen->setKeepAliveDetect(m_detect);
		
		ALERT_RET_V_E(m_listen->getMonitor()->addFDRead(m_cmd_sender->getMonitorFD()),"Add Fifo Fail",false);
		ALERT_RET_V_E(m_listen->getMonitor()->addFDRead(m_detect->getReadFD()),"Add Keep Alive Detect Fifo Fail",false);
	#ifndef RUN_AS_DEAMON
		ALERT_RET_V_E(m_listen->getMonitor()->addFDRead(STDIN_FILENO),"Add STDIN Fail",false);
	#endif
		
		m_cmd_listener->setQueue(m_cmd_queue);
		SockMgr::getSingleton()->addCmdInfoListener(m_cmd_listener);

		return true;
	}
	
	~Application()
	{
		SockMgr::getSingleton()->removeCmdInfoListener(m_cmd_listener);

		delete m_detect;
		delete m_resource_monitor;
		delete m_reciever_and_sender;
		delete m_cmd_sender;
		delete m_cmd_queue;
		delete m_cmd_listener;
		delete m_listen;
	}
	
	void run()
	{
		pthread_t t1,t2,t3;

		ALERT_RET(pthread_create(&t1,NULL,connection_loop,m_listen)==0,"Create Connect Thread Fail");
		ALERT_RET(pthread_create(&t2,NULL,cmd_hander_loop,m_reciever_and_sender)==0,"Create ReciverSender Thread Fail");
		ALERT_RET(pthread_create(&t3,NULL,keep_alive_detect_loop,m_detect)==0,"Create KeepAlive Detect Thread Fail");

		pthread_join(t1,NULL);
		//pthread_join(t2,NULL);
		//pthread_join(t3,NULL);
		//pthread_join(m_log_thread,NULL);
		//pthread_join(m_log_flush_thread,NULL);
	}
};

static void app_sigpipe_handler(int signo)
{
	if(signo == SIGPIPE)
	{
		//printf("catch sigpipe signal\n");
	}
}
#ifdef RUN_AS_DEAMON
static void daemon_init(const char *cmd)
{
	unsigned int i;
	int fd0,fd1,fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	//clear file creation mask
	umask(0);

	//get maximum number of file descriptor
	if(getrlimit(RLIMIT_NOFILE,&rl)<0)
	{
		exit(1);
	}

	//become a seesion leader to lose controlling tty
	if((pid=fork())<0)
	{
		exit(1);
	}
	else if(pid!=0)//parent
	{

		exit(0);
	}
	setsid();

	//ensure future opens woni't allocate controlling ttys
	sa.sa_handler=SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;
	if(sigaction(SIGHUP,&sa,NULL)<0)
	{
		exit(1);
	}
	if((pid=fork())<0)
	{
		exit(1);
	}
	else if(pid!=0)
	{
		exit(0);
	}

	//change the current working directory to the root so we won't prevent file systems from being unmounted
	if(chdir("/")<0)
	{
		exit(1);
	}

	//close all file decriptors
	if(rl.rlim_max==RLIM_INFINITY)
		rl.rlim_max=1024;
	for(i=0;i<rl.rlim_max;i++)
		close((int)i);
		
	//attach file descriptor 0,1,2 to /dev/null
	fd0=open("/dev/null",O_RDWR);
	fd1=dup(0);
	fd2=dup(0);

	//init log file
	openlog(cmd,LOG_CONS,LOG_DAEMON);
	if(fd0!=0||fd1!=1||fd2!=2)
	{
		syslog(LOG_ERR,"unexpected file descroptor %d,%d,%d",fd0,fd1,fd2);
		exit(1);
	}
}
#endif
static inline void delEnter(char *str)
{
	int len=strlen(str);
	if(len>0&&str[len-1]=='\n')
	{
		str[len-1]=0;
	}
	
	if(len>1&&str[len-2]=='\r')
	{
		str[len-2]=0;
	}
}
static bool loadConfigFiles(char u[],int u_size,char p[],int p_size,char du[],int du_size)
{
	bool ret=false;
	FILE *file=fopen("/opt/share_mind/config/cfg","r");
	if(file==NULL)
	{
		goto out;
	}
	if(fgets(u,u_size,file)==NULL)
	{
		goto out;
	}
	if(fgets(p,p_size,file)==NULL)
	{
		goto out;
	}
	if(fgets(du,du_size,file)==NULL)
	{
		goto out;
	}
	ret=true;
	
out:
	u[u_size-1]=0;
	p[p_size-1]=0;
	du[du_size-1]=0;
	delEnter(u);
	delEnter(p);
	delEnter(du);
	if(file!=NULL)
	{
		fclose(file);
	
}
	return ret;
}
static bool getUidGid(const char *name,int out[2])
{
	struct passwd *pd=getpwnam(name);
	if(pd!=NULL)
	{
		out[0]=pd->pw_uid;
		out[1]=pd->pw_gid;
		return true;
	}
	return false;
}
static void quit_error(const char *cmd)
{
	printf("%s,error=%s\n",cmd,strerror(errno));
	exit(1);
}
static void printf_ids(const char *cmd)
{
	printf("%s,uid=%d,euid=%d,gid=%d,egid=%d\n",cmd,getuid(),geteuid(),getgid(),getegid());
}
void change_user(const char *name)
{
	int id[2];
	if(!getUidGid(name,id))
	{
		quit_error("User Not Exist");
	}
	if(setgid(id[1])<0)
	{
		quit_error("set gid fail");
	}
	if(setuid(id[0])<0)
	{
		quit_error("set uid Fail");
	}
	
	printf_ids("current");
}

int main(int argn,char *args[])
{
	static char u[36];
	static char p[72];
	static char du[32];
	if(!loadConfigFiles(u,sizeof(u),p,sizeof(p),du,sizeof(du)))
	{
		quit_error("Read Config");
	}

	change_user(du);
#ifdef RUN_AS_DEAMON
	daemon_init("BookShareMind");
#endif
	
    ALERT_RET_V_E(signal(SIGPIPE,app_sigpipe_handler)!=SIG_ERR,"Handle SIGPIPE Fail",0);

	Application app(APPLICATION_PORT,NULL,NULL);

	if(app.init(u,p,NULL))
	{
		memset(u,0,sizeof(u));
		memset(p,0,sizeof(p));
		memset(du,0,sizeof(du));
		app.run();
	}
	else
	{
		sleep(10);//wait for log error
	}
	return 0;
}
