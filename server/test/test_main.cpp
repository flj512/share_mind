#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include"CmdInfo.hpp"
#include"util_macro.h"
#include"SockMgr.hpp"
#include"TestSock.hpp"
#include"SockListen.hpp"
#include"SockMonitor.hpp"
#include"SockConnector.hpp"
#include"CmdResolver.hpp"
#include"SockSender.hpp"
#include"CmdReciever.hpp"
#include"SockSendResponse.hpp"
#include"AccessDBReciever.hpp"

//#define CONNECT_TEST_2

#define TEST_ALERT(desc,cond) {const char *res;if(!(cond)) res="Fail"; else res="Pass"; printf("[%s] Test %s @ {%s:%d}\n",desc,res,__FUNCTION__,__LINE__);if(!(cond)) exit(0);}

static void test_seq_set(void)
{
	char seq[32];
	u16 t2 = 0x1234,t2b,t2e;
	u32 t3 = 0x12345678,t3b,t3e;
	u64 t4 = 0x1234568790abcdef,t4b,t4e;
	//unsigned long t4h,t4l;

	//t4h=((0x12<<24)+(0x34<<16)+(0x56<<8)+0x78);

	SET_WORD_BIG(seq,t2);
	t2b=GET_WORD_BIG(seq);
	SET_WORD_END(seq,t2);
	t2e=GET_WORD_END(seq);
	SET_DWORD_BIG(seq,t3);
	t3b=GET_DWORD_BIG(seq);
	SET_DWORD_END(seq,t3);
	t3e=GET_DWORD_END(seq);
	SET_DDWORD_BIG(seq,t4);
	t4b=GET_DDWORD_BIG(seq);
	SET_DDWORD_END(seq,t4);
	t4e=GET_DDWORD_END(seq);
	//t4l=GET_DWORD_END(seq);
	//t4h=GET_DWORD_BIG(seq+4);

	TEST_ALERT("seq module",t2b==t2&&t2e==t2&&t3b==t3&&t3e==t3&&t4b==t4&&t4e==t4);
}

static void test_CmdInfo(void)
{
	int add_count,cmd_len=503;
	CmdInfoHeaderBody info(1);
	const char *test="hello,this is a test cmd!";
	int test_len = strlen(test);
	
	u8 cmd[1024],*pt;

	cmd[0] = CMD_INFO_HEADER0;
	cmd[1] = CMD_INFO_HEADER1;
	cmd[2] = CMD_INFO_HEADER2;
	cmd[3] = CMD_INFO_HEADER3;
	SET_WORD_BIG(cmd+4,cmd_len);
	sprintf((char*)cmd+6,"%s",test);
	memset(cmd+6+test_len,'t',sizeof(cmd)-6-test_len );

	pt = cmd;

	pt+=info.addContent(pt,1);
	pt+=info.addContent(pt,1);
	pt+=info.addContent(pt,2);
	pt+=info.addContent(pt,1);

	while(pt+89<cmd+1024)
	{
		add_count = info.addContent(pt,89);

		if(info.cmdComplete()) break;

		pt+=add_count;
	}

	TEST_ALERT("CmdInfo Module",info.cmdComplete()&&info.getCmdLen()==cmd_len&&memcmp(cmd+6,info.getCmd(),cmd_len)==0);

}

class TestCmdInfoListener:public CmdInfoListener{
private:
	u8 *m_src;
public:
	TestCmdInfoListener(u8 *src)
	{
		m_src=src;
	}

	void onCmdIn(CmdInfo *info)
	{
		TEST_ALERT("SockMgr",info->getCmdLen()!=0&&memcmp(m_src,info->getCmd(),info->getCmdLen())==0);
	}

	void onClose(SockData *sock_data)
	{
	
	}
	void onConnectIn(SockData *sock_data)
	{
		
	}
};

static void test_SockMgr(void)
{
	int cmd_len=503;
	SockMgr *mgr=SockMgr::getSingleton();
	const char *test="hello,this is a test cmd!";
	int test_len = strlen(test);
	u8 cmd[1024];

	TEST_ALERT("MgrSingleton",mgr!=NULL);
	
	cmd[0] = CMD_INFO_HEADER0;
	cmd[1] = CMD_INFO_HEADER1;
	cmd[2] = CMD_INFO_HEADER2;
	cmd[3] = CMD_INFO_HEADER3;
	SET_WORD_BIG(cmd+4,cmd_len);
	sprintf((char*)cmd+6,"%s",test);
	memset(cmd+6+test_len,'t',sizeof(cmd)-6-test_len );

	TestCmdInfoListener *listener = new TestCmdInfoListener(cmd+6);

	mgr->addCmdInfoListener(listener);
	mgr->pushData(5,cmd,cmd_len+6);
	mgr->removeCmdInfoListener(listener);
	
	delete listener;

}
#if defined(CONNECT_TEST_0)
static void test_sock_connect0(void)
{
	SockListen *listen = new TestSockListen(6000,NULL,NULL);
	SockMonitor *monitor = new SelectSockMonitor();
	TestSockMonitorEvnetListner *monitor_listener = new TestSockMonitorEvnetListner();

	monitor->setLister(monitor_listener);
	listen->setMonitor(monitor);

	listen->run();
}
#endif

#if defined(CONNECT_TEST_1)
class TestConnectorCmdInfoListener:public CmdInfoListener{
private:
	int m_cmd_count;
public:
	TestConnectorCmdInfoListener()
	{
		m_cmd_count=0;
	}
	void onCmdIn(CmdInfo *info)
	{
		if(m_cmd_count++%10000==0)
			printf("CmdIn:sock=%d,len=%d,content=%s\n",info->getSock(),info->getCmdLen(),info->getCmd());
	}

	void onClose(SockData *sock_data)
	{
		printf("Close,sock=%d\n",sock_data->getSock());
	}
	void onConnectIn(SockData *sock_data)
	{
		printf("Connect In,sock=%d\n",sock_data->getSock());
	}
};

static void test_sock_connect1(void)
{
	SockConnectorListen *conn=new SockConnectorListen(6000,NULL,NULL);
	CmdInfoListener *cmd_listener = new TestConnectorCmdInfoListener();

	SockMgr::getSingleton()->addCmdInfoListener(cmd_listener);
	conn->run();
	SockMgr::getSingleton()->removeCmdInfoListener(cmd_listener);
}
#endif
#if defined(CONNECT_TEST_2)||defined(CONNECT_TEST_3)
static void* cmd_reciever(void* args)
{
	SockListen *listen=(SockListen *)args;

	listen->run();

	return NULL;
}
#if(0)
static void print_json(cJSON *json,int level)
{
	int item_size = cJSON_GetArraySize(json);

	for(int i=0;i<item_size;i++)
	{
		cJSON *item=cJSON_GetArrayItem(json,i);
		if(item->type==cJSON_Object||item->type==cJSON_Array)
		{
			for(int j=0;j<level;j++) printf("\t");
			printf("%s:->\n",item->string);
			print_json(item,level+1);
		}
		else
		{
			for(int j=0;j<level;j++) printf("\t");
			if(item->type==cJSON_String)
				printf("%s:%s\n",item->string,item->valuestring);
			else
				printf("%s:%d\n",item->string,item->valueint);
		}	
	}
}
static void* cmd_handler(void *args)
{
	SafeQueue<CmdData> *cmd_queue=(SafeQueue<CmdData> *)args;
	CmdData *cmd;

	int count = 0;

	while(1)
	{
		cmd=cmd_queue->front();
		if(count++%50000==0)
		{
			printf("SessionID:%ld\n",cmd->id);
			print_json(cmd->cmd,0);
		}
		cJSON_Delete(cmd->cmd);
		cmd_queue->pop();
	}

	return NULL;
}
#endif
static void* cmd_handler2(void *args)
{
	CmdReciever *reciever=(CmdReciever *)args;

	reciever->run();
	
	return NULL;
}

class TestCmdReciever:public CmdRecieverSender
{
private:
	u64 cmd_count;

public:
	TestCmdReciever()
	{

		cmd_count=0;
	}

	int cmdIn(CmdData * data)
	{
		const char *reponse="Recieved!";
		ALERT_DO(m_sender->send(data->id,(u8*)reponse,strlen(reponse))>=0,"Send Msg Fail",return -1;);//reponse "Recieved!"

		/*if(cmd_count++%7001==0)
			printf("recieve from [%ld],index=%ld \n",data->id,cmd_count);*/

		return 0;
	}
};
#endif
#if defined(CONNECT_TEST_2)
static void test_sock_connect2(void)
{
	SockConnectorListen *conn=new SockConnectorListen(6000,NULL,NULL);
	CmdResolver *cmd_listener = new CmdResolver();
	SafeQueue<CmdData> *cmd_queue = new SafeQueueUnlimit<CmdData>();
	CmdSender *sender = new CmdSender();
	TestCmdReciever *reciever=new TestCmdReciever();
	//AccessDBReciever *reciever=new AccessDBReciever();
	SystemResourceMonitor *resource_monitor=new SystemResourceMonitor();
	

	ALERT_RET_E(sender->open()>=0,"Open Sender Fail");

	ALERT_DO(reciever->init(),"Init Fail",goto out;);
	
	reciever->setSender(sender);
	reciever->setQueue(cmd_queue);

	resource_monitor->setQueue(cmd_queue);
	conn->setResourceMonitor(resource_monitor);
	
	ALERT_RET(conn->getMonitor()->addFDRead(sender->getMonitorFD()),"Add Fifo Fail");
	ALERT_RET(conn->getMonitor()->addFDRead(STDIN_FILENO),"Add Fifo Fail");
	
	cmd_listener->setQueue(cmd_queue);
	SockMgr::getSingleton()->addCmdInfoListener(cmd_listener);


	pthread_t t1,t2;

	pthread_create(&t1,NULL,cmd_reciever,conn);
	pthread_create(&t2,NULL,cmd_handler2,reciever);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	SockMgr::getSingleton()->removeCmdInfoListener(cmd_listener);

out:
	delete resource_monitor;
	delete reciever;
	delete sender;
	delete cmd_queue;
	delete cmd_listener;
	delete conn;
}
#endif

#if defined(CONNECT_TEST_3)
static void* reponseSender(void *args)
{
	SockSender *sender=(SockSender *)args;

	sender->run();
	
	return NULL;
}

static void test_sock_connect3(void)
{
	SockConnectorListen *conn=new SockConnectorListen(6000,NULL,NULL);
	CmdResolver *cmd_listener = new CmdResolver();
	SafeQueue<CmdData> *cmd_queue = new SafeQueueLimit<CmdData>();
	CmdSender *sender = new CmdSender();
	TestCmdReciever *reciever=new TestCmdReciever();
	SockResponseSender *reponse=new SockResponseSender();
	

	ALERT_RET_E(sender->open()>=0,"Open Sender Fail");

	reciever->setSender(sender);
	reciever->setQueue(cmd_queue);
	reponse->setCmdFifo(sender->getMonitorFD());

	
	cmd_listener->setQueue(cmd_queue);
	SockMgr::getSingleton()->addCmdInfoListener(cmd_listener);


	pthread_t t1,t2,t3;

	pthread_create(&t1,NULL,cmd_reciever,conn);
	pthread_create(&t2,NULL,cmd_handler2,reciever);
	pthread_create(&t3,NULL,reponseSender,reponse);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
	
	SockMgr::getSingleton()->removeCmdInfoListener(cmd_listener);

	delete reponse;
	delete reciever;
	delete sender;
	delete cmd_queue;
	delete cmd_listener;
	delete conn;
	
}
#endif
void test_hexstring_byte()
{
	u8 src[]={0x1a,0x1b,0x2c,0x5d,0x67,0x98,0x35};
	char hexString[16];
	u8 dest[10];

	if(convertByteToHexString(src,sizeof(src),hexString,sizeof(hexString)))
	{
		//printf("%s\n",hexString);
		convertHexStringToByte(hexString,dest,sizeof(dest));
	}

	TEST_ALERT("HexByte",memcmp(src,dest,sizeof(src))==0);
}
static char* getUserPasswd(char *user,int ubuff_sz)
{
	printf("***Enter User Name and Passwd***\n");
	printf("Name:");
	
	if(fgets(user,ubuff_sz,stdin)==NULL)
	{
		return NULL;
	}

	while(*user!=0)
	{
		if(*user=='\n')
		{
			*user=0;
			break;
		}
		user++;
	}
	
	return getpass("Passwd:");
}
void gen_key()
{
	char key[]="Og6u+cB2nHigl0R=";
	char u[32];
	char *p=getUserPasswd(u,sizeof(u));

	if(p==NULL)
	{
		return;
	}
	
	char *eu=aes_encrypt_char(u,key);
	char *ep=aes_encrypt_char(p,key);
	printf("%s,%s\n",eu,ep);
	free(eu);
	free(ep);
	memset(key,0,sizeof(key));
	memset(p,0,strlen(p));
}
void test_aes(void)
{
	const char *key="Og6u+cB2nHigl0R=";
	char str[128];
	bool ret=true;
	for(unsigned int i=1;i<=sizeof(str);i++)
	{
		memset(str,'t',i);
		str[i-1]=0;

		char *en=aes_encrypt_char(str,key);
		char *de=aes_decrypt_char(en,key);

		//printf("%s\n",de);
		
		ret=(de!=NULL&&strcmp(str,de)==0);

		free(de);
		free(en);
		if(!ret)
		{
			break;
		}
	}
	
	TEST_ALERT("AES",ret);
}
static void sigpipe_handler(int signo)
{
	if(signo == SIGPIPE)
	{
		printf("catch sigpipe signal\n");
	}
}

int main(void)
{
    ALERT_RET_V_E(signal(SIGPIPE,sigpipe_handler)!=SIG_ERR,"Handle SIGPIPE Fail",0);
    
	test_seq_set();
	test_CmdInfo();
	test_SockMgr();
	test_hexstring_byte();
 	test_aes();
 	gen_key();
#if defined(CONNECT_TEST_0)
	test_sock_connect0();
#endif

#if defined(CONNECT_TEST_1)
	test_sock_connect1();
#endif

#if defined(CONNECT_TEST_2)
	test_sock_connect2();
#endif

#if defined(CONNECT_TEST_3)
	test_sock_connect3();
#endif
	
	return 0;
}
