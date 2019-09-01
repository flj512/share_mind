#ifndef __BOOK_SHARE_MIND_SAFE_QUEUE_H__
#define __BOOK_SHARE_MIND_SAFE_QUEUE_H__

#include<queue>
#include<pthread.h>
#include"global_limit.hpp"


template<class T>
class SafeQueue{
public:
	SafeQueue()
	{
	}
	virtual ~SafeQueue()
	{
	}
	virtual void pop(void)=0;
	virtual void push(T& cmd)=0;
	virtual T* front(void)=0;
	virtual T* back(void)=0;
	virtual int size(void)=0;
	
};

template<class T>
class SafeQueueLimit:public SafeQueue<T>{
private:
	std::queue<T> m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond_t;
	pthread_cond_t m_cond_r;
	int m_limit;

	void init(void)
	{
		pthread_mutex_init(&m_mutex,NULL);
		pthread_cond_init(&m_cond_t,NULL);
		pthread_cond_init(&m_cond_r,NULL);
	}
public:
	SafeQueueLimit()
	{
		init();
		m_limit = MAX_RECIEVE_QUEUE_SIZE;
	}
	SafeQueueLimit(int limit)
	{
		init();
		m_limit=(limit > 0)?limit:MAX_RECIEVE_QUEUE_SIZE;
	}
	~SafeQueueLimit()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond_t);
		pthread_cond_destroy(&m_cond_r);
	}
	void pop(void)
	{
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond_r,&m_mutex);
		}
		m_queue.pop();
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_signal(&m_cond_t);
	}
	void push(T& cmd)
	{
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()>(unsigned int)m_limit)
		{
			pthread_cond_wait(&m_cond_t,&m_mutex);
		}
		m_queue.push(cmd);
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_signal(&m_cond_r);
	}

	T* front(void)
	{
		T *ret;
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond_r,&m_mutex);
		}
		ret = &m_queue.front();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}
	T* back(void)
	{
		T *ret;
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond_r,&m_mutex);
		}
		ret = &m_queue.back();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}

	int size()
	{
		int ret;
		pthread_mutex_lock(&m_mutex);
		ret = m_queue.size();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}

	bool full()
	{
		bool ret;
		
		pthread_mutex_lock(&m_mutex);
		ret = (m_queue.size()>=(unsigned int)m_limit);
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}
	
};

template<class T>
class SafeQueueUnlimit{
private:
	std::queue<T> m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;

	
public:
	SafeQueueUnlimit()
	{
		pthread_mutex_init(&m_mutex,NULL);
		pthread_cond_init(&m_cond,NULL);
	
	}
	~SafeQueueUnlimit()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}
	void pop(void)
	{
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond,&m_mutex);
		}
		m_queue.pop();
		pthread_mutex_unlock(&m_mutex);
	}
	void push(T& cmd)
	{
		pthread_mutex_lock(&m_mutex);
		m_queue.push(cmd);
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_signal(&m_cond);
	}

	T* front(void)
	{
		T *ret;
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond,&m_mutex);
		}
		ret = &m_queue.front();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}
	T* back(void)
	{
		T *ret;
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond,&m_mutex);
		}
		ret = &m_queue.back();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}

	int size()
	{
		int ret;
		pthread_mutex_lock(&m_mutex);
		ret = m_queue.size();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}
	
};


template<class T>
class SafeQueueLimitM{
private:
	std::queue<T> m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond_t;
	pthread_cond_t m_cond_r;
	int m_limit;

	void init(void)
	{
		pthread_mutex_init(&m_mutex,NULL);
		pthread_cond_init(&m_cond_t,NULL);
		pthread_cond_init(&m_cond_r,NULL);
	}
public:
	SafeQueueLimitM()
	{
		init();
		m_limit = MAX_RECIEVE_QUEUE_SIZE;
	}
	SafeQueueLimitM(int limit)
	{
		init();
		m_limit=(limit > 0)?limit:MAX_RECIEVE_QUEUE_SIZE;
	}
	~SafeQueueLimitM()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond_t);
		pthread_cond_destroy(&m_cond_r);
	}
	T pop(void)
	{
		T ret;
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()<=0)
		{
			pthread_cond_wait(&m_cond_r,&m_mutex);
		}
		ret=m_queue.front();
		m_queue.pop();
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_signal(&m_cond_t);
		
		return ret;
	}
	void push(T& cmd)
	{
		pthread_mutex_lock(&m_mutex);
		while(m_queue.size()>(unsigned int)m_limit)
		{
			pthread_cond_wait(&m_cond_t,&m_mutex);
		}
		m_queue.push(cmd);
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_signal(&m_cond_r);
	}

	int size()
	{
		int ret;
		pthread_mutex_lock(&m_mutex);
		ret = m_queue.size();
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}

	bool full()
	{
		bool ret;
		
		pthread_mutex_lock(&m_mutex);
		ret = (m_queue.size()>=(unsigned int)m_limit);
		pthread_mutex_unlock(&m_mutex);
		
		return ret;
	}
	
};

#endif
