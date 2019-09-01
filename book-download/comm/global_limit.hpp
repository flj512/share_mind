#ifndef __BOOK_SHARE_MIND_LIMIT__
#define __BOOK_SHARE_MIND_LIMIT__

#define MAX_RECIEVE_QUEUE_SIZE			500000
#define MAX_REPONSE_QUEUE_SIZE			500000

#define MAX_READ_FIFO_BUFFER_SIZE		(16*1024)

#define MAX_CONNECT_COUNT				(50000)

#define MAX_MSG_IN_LOG_QUEUE			100
#define MAX_SIZE_PER_MSG				512
#define MAX_LOG_SIZE_PER_FILE			(10*1024*1024)

#define MAX_KEEP_ALIVE_COUNT			(10)
#define KEEP_ALIVE_DECTECT_PERIOD		(20)//unit seconds

#define MAX_TIMESTAMP_EXPIRE			(10*60)//unit seconds

#define MAX_FILE_PATH_LEN				(128)

#define MAX_THREADS_COUNT				(100)
#define MAX_FILE_SIZE					(10*1024*1024)
#endif