#ifndef __MUTEXW_H__
#define __MUTEXW_H__

#include <pthread.h>


#if 0
// 普通锁
class ClMutexLock
{
public:
	ClMutexLock();
	~ClMutexLock();
	void Lock();
	int Trylock();
	void Unlock();
  	pthread_mutex_t *GetMutex();
	
private:
	pthread_mutex_t m_mutex;
};

#define CMutexLock ClMutexLock
// 读写锁
class CRwLock  
{  
public:  
    CRwLock();  
    ~CRwLock();  
    void ReadLock();  
    void WriteLock();  
    void Unlock();  

private:  
    pthread_rwlock_t m_rwl;  
};

// 递归锁
class CMutexLockRecursive
{
public:
	CMutexLockRecursive();
	~CMutexLockRecursive();
	void Lock();
	void Unlock();
	pthread_mutex_t *GetMutex();	
private:
	pthread_mutex_t m_mutex;
};
#endif

struct  ClMutexLock
{
	void (*UnClMutexLock)( struct  ClMutexLock *mutex_lock);
	void (*Lock)(struct  ClMutexLock *mutex_lock);
	int (*Trylock)(struct  ClMutexLock *mutex_lock);
	void (*Unlock)(struct  ClMutexLock *mutex_lock);
	pthread_mutex_t m_mutex;
};

void ClMutexLock_Init(struct  ClMutexLock * mutex_lock);


#ifdef _OscarXdvs_  // 仅仅用来方便调试而已
enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
#ifdef __USE_UNIX98
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#endif
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
}
#endif // _OscarXdvs_

#endif // __MUTEXW_H__

