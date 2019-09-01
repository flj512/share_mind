#ifndef __BOOK_SHARE_MIND__COMM_UTILS__
#define __BOOK_SHARE_MIND__COMM_UTILS__

#include"types_d.h"

#ifdef __cplusplus
extern "C"{
#endif

int comm_setfd_noblock(int fd);
int comm_setfd_flag(int fd,int flag);
bool convertByteToHexString(u8 *bytes,int size,char *out,int buffer_size);
bool convertHexStringToByte(const char *in,u8 *out,int buffer_size);
bool isNumber(const char *in);
u64 getLong(const char *in);
void calcStringMD5(char *in,char *out,int size);
void calcMultiStringMD5(char **in,int in_size,char *out,int size);
u32 calcCrc32(const char *buf);
char* aes_encrypt_char(const char* input,const char* key);
char* aes_decrypt_char(const char* input,const char* key);
#ifdef __cplusplus
}
#endif
#endif