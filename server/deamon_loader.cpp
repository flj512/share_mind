#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<pwd.h>

#define BOOK_DEAMON_NAME  "book-share-mind"
/* for test
static char* getCmd(char *cmd,int buff_sz)
{
	char *ret;

	printf(">");
	
	ret=fgets(cmd,buff_sz,stdin);

	while(*cmd!=0)
	{
		if(*cmd=='\n')
		{
			*cmd=0;
			break;
		}
		cmd++;
	}

	return ret;
}*/
bool getUidGid(const char *name,int out[2])
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
void quit_error(const char *cmd)
{
	printf("%s,error=%s\n",cmd,strerror(errno));
	exit(1);
}
void printf_ids(const char *cmd)
{
	printf("%s,uid=%d,euid=%d,gid=%d,egid=%d\n",cmd,getuid(),geteuid(),getgid(),getegid());
}
void change_user(const char *name)
{
	int id[2];
	if(!getUidGid("sharedushu",id))
	{
		quit_error("User Not Exist");
	}
	printf("deamon,uid=%d,gid=%d\n",id[0],id[1]);

	printf_ids("Before");
	if(setgid(id[1])<0)
	{
		quit_error("set gid fail");
	}
	if(setuid(id[0])<0)
	{
		quit_error("set uid Fail");
	}
	printf_ids("After");
}
int main()
{
	int pid;

	change_user("sharedushu");

	
	if((pid=fork())<0)
	{
		quit_error("Fork Fail");
	}
	else if(pid!=0)
	{
		exit(0);
	}
	
	printf_ids("child");

	exit(0);

}

