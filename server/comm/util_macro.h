#ifndef __BOOK_SHARE_MIND__UTIL_MACRO__
#define __BOOK_SHARE_MIND__UTIL_MACRO__

#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<types_d.h>
#include"LogUtil.hpp"

#define GET_BYTE(s,i)       			((u32)(((unsigned char*)(s))[i]))
#define GET_WORD_BIG(s) 				((GET_BYTE(s,0)<<8)+GET_BYTE(s,1))
#define GET_WORD_END(s) 				((GET_BYTE(s,1)<<8)+GET_BYTE(s,0))
#define GET_DWORD_BIG(s) 				((GET_BYTE(s,0)<<24)+(GET_BYTE(s,1)<<16)+(GET_BYTE(s,2)<<8)+GET_BYTE(s,3))
#define GET_DWORD_END(s) 				((GET_BYTE(s,3)<<24)+(GET_BYTE(s,2)<<16)+(GET_BYTE(s,1)<<8)+GET_BYTE(s,0))
#define GET_DDWORD_BIG(s) 				((((u64)GET_DWORD_BIG(s))<<32)+GET_DWORD_BIG((unsigned char*)(s)+4))
#define GET_DDWORD_END(s) 				(GET_DWORD_END(s)+(((u64)GET_DWORD_END((unsigned char*)(s)+4))<<32))

#define SET_BYTE(s,i,v)					(((unsigned char*)(s))[i] = (unsigned char)(v))
#define SET_WORD_BIG(s,v) 				{SET_BYTE(s,0,(v)>>8);SET_BYTE(s,1,v);}
#define SET_WORD_END(s,v) 				{SET_BYTE(s,1,(v)>>8);SET_BYTE(s,0,v);}
#define SET_DWORD_BIG(s,v) 				{SET_BYTE(s,0,(v)>>24);SET_BYTE(s,1,(v)>>16);SET_BYTE(s,2,(v)>>8);SET_BYTE(s,3,v);}
#define SET_DWORD_END(s,v) 				{SET_BYTE(s,3,(v)>>24);SET_BYTE(s,2,(v)>>16);SET_BYTE(s,1,(v)>>8);SET_BYTE(s,0,v);}
#define SET_DDWORD_BIG(s,v) 			{SET_DWORD_BIG(s,(v>>32)&0xffffffff);SET_DWORD_BIG((unsigned char*)(s)+4,v&0xffffffff);}
#define SET_DDWORD_END(s,v) 			{SET_DWORD_END(s,v&0xffffffff);SET_DWORD_END((unsigned char*)(s)+4,(v>>32)&0xffffffff);}


#define UTIL_MAX(a,b)					((a)>=(b)?(a):(b))
#define UTIL_MIN(a,b)					((a)<=(b)?(a):(b))

#define DEBUG_INFO(format,args...)\
{\
	char buffer[MAX_SIZE_PER_MSG];\
	snprintf(buffer,sizeof(buffer),format,##args);\
	LogUtil::log(buffer);\
}


#define ASSERT_INFO(msg)				DEBUG_INFO("[%s:%d]:%s\n",__FUNCTION__,__LINE__,msg)
#define ALERT_INFO(cond,msg)    		if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d]\n",msg,__FUNCTION__,__LINE__);}
#define ALERT_DO(cond,msg,action)		if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d]\n",msg,__FUNCTION__,__LINE__);action;}
#define ALERT_RET(cond,msg)				if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d]\n",msg,__FUNCTION__,__LINE__);return; }
#define ALERT_RET_V(cond,msg,value)		if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d]\n",msg,__FUNCTION__,__LINE__);return value;}

#define ASSERT_INFO_E(msg)				DEBUG_INFO("[%s:%d]:%s,error[%d]:%s\n",__FUNCTION__,__LINE__,msg,errno,strerror(errno))
#define ALERT_INFO_E(cond,msg)    		if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d] error[%d]:%s\n",msg,__FUNCTION__,__LINE__,errno,strerror(errno));}
#define ALERT_DO_E(cond,msg,action)		if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d] error[%d]:%s\n",msg,__FUNCTION__,__LINE__,errno,strerror(errno));action;}
#define ALERT_RET_E(cond,msg)			if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d] error[%d]:%s\n",msg,__FUNCTION__,__LINE__,errno,strerror(errno));return; }
#define ALERT_RET_V_E(cond,msg,value)	if(!(cond)){DEBUG_INFO("[%s] @ [%s:%d] error[%d]:%s\n",msg,__FUNCTION__,__LINE__,errno,strerror(errno));return value;}

#define RUN_AS_DEAMON	

#endif
