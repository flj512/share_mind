#include<fcntl.h>
#include<string.h>
#include<malloc.h>
#include"LibMd5.h"
#include"com_utils.h"
#include"util_macro.h"
#include"aes.h"

int comm_setfd_flag(int fd,int flag)
{
	int old_flag;
	
	old_flag=fcntl(fd,F_GETFL,0);

	if(old_flag < 0)
	{
		return -1;
	}

	flag=old_flag|flag;

	if( fcntl(fd,F_SETFL,flag) <0)
	{
		return -1;
	}

	return 0;
}

int comm_setfd_noblock(int fd)
{
	return comm_setfd_flag(fd,O_NONBLOCK);
}

bool convertByteToHexString(u8 *bytes,int size,char *out,int buffer_size)
{
	static char HEX_CHAR_MAP[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	if(size<0||buffer_size<=2*size)
	{
		return false;
	}
	
	for(int i=0;i<size;i++)
	{
		out[2*i]=HEX_CHAR_MAP[(bytes[i]>>4)&0x0f];
		out[2*i+1]=HEX_CHAR_MAP[bytes[i]&0x0f];
	}
	out[2*size]=0;
	return true;
}
static inline int getValue(char in)
{
	int v=-1;
	if(in>='0'&&in<='9')
	{
		v=in-'0';
	}
	else if(in>='a'&&in<='f')
	{
		v=in-'a'+10;
	}
	else if(in>='A'&&in<='F')
	{
		v=in-'A'+10;
	}
	return v;
}

bool convertHexStringToByte(const char *in,u8 *out,int buffer_size)
{
	int in_len=strlen(in)/2;
	int high,low,i;
	
	if(buffer_size<in_len)
	{
		return false;
	}

	for(i=0;i<in_len;i++)
	{
		high=getValue(in[i*2]);
		if(high==-1)
		{
			return false;
		}
		low=getValue(in[i*2+1]);
		if(low==-1)
		{
			return false;
		}
		out[i]=(u8)(((high<<4)&0xf0)+(low&0xf));
	}
	return true;
}	
inline static bool isDigital(char in)
{
	return (in>='0'&&in<='9');
}
bool isNumber(const char *in)
{
	while(*in!=0)
	{
		if(!isDigital(*in))
		{
			return false;
		}
		in++;
	}
	return true;
}
u64 getLong(const char *in)
{
	int bytes=0;
	u64 ret=0;
	
	if(!isNumber(in))
	{
		return ret;
	}
	while(*in!=0)
	{
		ret=ret*10+(*in-'0');
		if(++bytes>18)//max surport 18 bytes
		{
			return 0;
		}
		in++;
	}
	return ret;
}
void calcStringMD5(char *in,char *out,int size)
{
	Md5Context md5;
	MD5_HASH hash;

	Md5Initialise(&md5);
	Md5Update(&md5,in,strlen(in));
	Md5Finalise(&md5,&hash);
	convertByteToHexString(hash.bytes,MD5_HASH_SIZE,out,size);
}
void calcMultiStringMD5(char **in,int in_size,char *out,int size)
{
	Md5Context md5;
	MD5_HASH hash;

	Md5Initialise(&md5);
	for(int i=0;i<in_size;i++)
	{
		Md5Update(&md5,in[i],strlen(in[i]));
	}
	Md5Finalise(&md5,&hash);
	convertByteToHexString(hash.bytes,MD5_HASH_SIZE,out,size);
}
static const u32 crc32tab[] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
 0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
 0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
 0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL 
};

u32 calcCrc32(const char *in)
{
 	u32  crc;
	int len=strlen(in);
	
 	crc = 0xFFFFFFFF;
 	for (int i = 0; i < len; i++)
  		crc = crc32tab[(crc ^ ((unsigned char)in[i])) & 0xff] ^ (crc >> 8);
  		
 	return crc^0xFFFFFFFF;
}

static void gen128bitKey(const char *key,BYTE bytes[])
{
	Md5Context md5;
	MD5_HASH hash;
	

	Md5Initialise(&md5);
	Md5Update(&md5,key,strlen(key));
	Md5Finalise(&md5,&hash);

	memcpy(bytes,hash.bytes,16);
}
char* aes_encrypt_char(const char* input,const char* key)
{
	if(input==NULL||key==NULL)
	{
		return NULL;
	}
	
	BYTE key_bit[16];
	WORD key_schedule[60];
	char *out=NULL;
	BYTE *buffer=NULL,*mbuffer;
	BYTE tail[AES_BLOCK_SIZE];
	int block_size,i,out_size,buffer_size;
	int input_len=strlen(input)+1;

	block_size=((input_len-1)/AES_BLOCK_SIZE)+1;
	buffer_size=block_size*AES_BLOCK_SIZE;
	ALERT_RET_V((buffer=(BYTE*)malloc(buffer_size))!=NULL,"Memory Error",NULL);
	out_size=(buffer_size*2)+1;
	ALERT_DO((out=(char*)malloc(out_size))!=NULL,"Memory Error",goto out;);

	gen128bitKey(key,key_bit);
	aes_key_setup(key_bit,key_schedule,sizeof(key_bit)*8);
	mbuffer=buffer;
	
	for(i=0;i<block_size;i++,input+=AES_BLOCK_SIZE,input_len-=AES_BLOCK_SIZE,mbuffer+=AES_BLOCK_SIZE)
	{
		if(input_len<AES_BLOCK_SIZE)
		{
			memcpy(tail,input,input_len);
			aes_encrypt(tail,mbuffer,key_schedule,sizeof(key_bit)*8);
		}
		else
		{
			aes_encrypt((const BYTE*)input,mbuffer,key_schedule,sizeof(key_bit)*8);
		}
	}
	convertByteToHexString(buffer,buffer_size,out,out_size);
	memset(key_bit,0,sizeof(key_bit));
	memset(key_schedule,0,sizeof(key_schedule));
	
out:
	free(buffer);
	return out;
}
char* aes_decrypt_char(const char* input,const char* key)
{
	if(input==NULL||key==NULL)
	{
		return NULL;
	}

	BYTE key_bit[16];
	WORD key_schedule[60];
	char *out=NULL,*pout;
	BYTE *buffer=NULL,*mBuffer;
	int block_size,i,out_size,buffer_size;
	int input_len=strlen(input);

	if((input_len%(AES_BLOCK_SIZE*2))!=0||input_len==0)//bad data
	{
		return out;
	}
	block_size=input_len/(AES_BLOCK_SIZE*2);
	buffer_size=block_size*AES_BLOCK_SIZE;
	ALERT_RET_V((buffer=(BYTE*)malloc(buffer_size))!=NULL,"Memory Error",NULL);
	ALERT_DO(convertHexStringToByte(input,buffer,buffer_size),"Decode Hex String Fail",goto out;);
	out_size=buffer_size+1;
	ALERT_DO((out=(char*)malloc(out_size))!=NULL,"Memory Error",goto out;);

	gen128bitKey(key,key_bit);
	aes_key_setup(key_bit,key_schedule,sizeof(key_bit)*8);

	mBuffer=buffer;
	pout=out;
	for(i=0;i<block_size;i++,mBuffer+=AES_BLOCK_SIZE,pout+=AES_BLOCK_SIZE)
	{
		aes_decrypt(mBuffer,(BYTE*)pout,key_schedule,sizeof(key_bit)*8);
	}
	out[out_size]=0;
	memset(key_bit,0,sizeof(key_bit));
	memset(key_schedule,0,sizeof(key_schedule));
	
out:
	free(buffer);
	return out;
}
//file end
