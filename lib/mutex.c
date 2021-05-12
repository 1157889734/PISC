/*
 * @Descripttion: 
 * @vesion: 
 * @Author: sunzhguy
 * @Date: 2020-05-28 14:54:50
 * @LastEditor: sunzhguy
 * @LastEditTime: 2020-06-09 08:34:42
 */ 
/*
*******************************************************************************
**  Copyright (c) 2012, 深圳市有限公司, All rights reserved.
**  作者：梁桥江
**  当前版本：v1.0
**  创建日期: 2012.11.10
**  文件说明: 封装 socket通信 相关函数
*******************************************************************************
*/

#include "mutex.h"



static void UnClMutexLock(struct  ClMutexLock *mutex_lock)
{
	pthread_mutex_destroy( &mutex_lock->m_mutex );
}

static void Lock(struct  ClMutexLock *mutex_lock)
{

	pthread_mutex_lock(&mutex_lock->m_mutex);
}

static int Trylock(struct  ClMutexLock *mutex_lock)
{
	return pthread_mutex_trylock(&mutex_lock->m_mutex );
}

static void Unlock(struct  ClMutexLock *mutex_lock)
{
	//pthread_mutex_unlock( &mutex_lock->m_mutex );
}


/* ===== 对普通锁的封装 ===== */
static void ClMutexLock(struct  ClMutexLock *mutex_lock)
{
	pthread_mutex_init(&mutex_lock->m_mutex,NULL);
	
}

void ClMutexLock_Init(struct  ClMutexLock * mutex_lock)
{
   ClMutexLock(mutex_lock);
   mutex_lock->UnClMutexLock = UnClMutexLock;
   mutex_lock->Lock = Lock;
   mutex_lock->Trylock = Trylock;
   mutex_lock->Unlock = Unlock;
   printf("---init over......%p\r\n",&mutex_lock->m_mutex);

} 
#if 0

/* ===== end 对普通锁的封装 ===== */

/* ===== 对读写锁的封装 ===== */
CRwLock::CRwLock()
{  
    pthread_rwlock_init( &m_rwl, NULL );         
}  
  
CRwLock::~CRwLock()
{  
    pthread_rwlock_destroy( &m_rwl );  
}  
  
void CRwLock::ReadLock()
{  
    pthread_rwlock_rdlock( &m_rwl );   
}   
  
void CRwLock::WriteLock()
{  
    pthread_rwlock_wrlock( &m_rwl );  
}    
 
void CRwLock::Unlock()
{  
    pthread_rwlock_unlock( &m_rwl );
}

/* ===== end 对读写锁的封装 ===== */

CMutexLockRecursive::CMutexLockRecursive()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_mutex_init( &m_mutex, &attr );
	pthread_mutexattr_destroy( &attr );
}

CMutexLockRecursive::~CMutexLockRecursive()
{
	pthread_mutex_destroy( &m_mutex );
}

void CMutexLockRecursive::Lock()
{
	pthread_mutex_lock( &m_mutex );
}

void CMutexLockRecursive::Unlock()
{
	pthread_mutex_unlock( &m_mutex );
}

pthread_mutex_t *CMutexLockRecursive::GetMutex()
{
	return &m_mutex;
}

#endif