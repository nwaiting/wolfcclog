#include "protable_os.h"

void Protable_os::Init_Mutex(Protable_thread_mutex_t* pMutex, int nType)
{
#ifdef __MY_Win_32__
	::InitializeCriticalSection(pMutex);
#else

	Protable_thread_mutexattr_t  lmutexAtrr = {0};
	int initFlag = 0;
	if(nType == 1) //µÝ¹éËø
	{
		initFlag = 1;
		pthread_mutexattr_init(&lmutexAtrr);
		pthread_mutexattr_settype(&lmutexAtrr, PTHREAD_MUTEX_RECURSIVE);
	}
	else
	{
		::pthread_mutexattr_init(&lmutexAtrr);
		::pthread_mutexattr_settype(&lmutexAtrr,PTHREAD_MUTEX_ADAPTIVE_NP);
	}
	
	::pthread_mutex_init(pMutex, &lmutexAtrr);
	if(initFlag)
		::pthread_mutexattr_destroy(&lmutexAtrr);
#endif
}

void Protable_os::Remove_Mutex(Protable_thread_mutex_t* pMutex)
{
#ifdef __MY_Win_32__
	::DeleteCriticalSection(pMutex);
#else
	::pthread_mutex_destroy(pMutex);
#endif
}

void Protable_os::Lock_Mutex(Protable_thread_mutex_t* pMutex)
{
#ifdef __MY_Win_32__
	::EnterCriticalSection(pMutex);
#else
	::pthread_mutex_lock(pMutex);
#endif
}

void Protable_os::Unlock_Mutex(Protable_thread_mutex_t* pMutex)
{
#ifdef __MY_Win_32__
	::LeaveCriticalSection(pMutex);
#else
	::pthread_mutex_unlock(pMutex);
#endif
}

int Protable_os::Try_Lock_Mutex(Protable_thread_mutex_t* pMutex)
{
#ifdef __MY_Win_32__
	return TryEnterCriticalSection(pMutex);
#else
	return pthread_mutex_trylock(pMutex);
#endif
}

void Protable_os::Init_RW_Mutex(Protable_thread_rwlock_t* pLock)
{
#ifdef __MY_Win_32__
	pLock->Init();
#else
	pthread_rwlock_init(pLock, NULL);
#endif
}

void Protable_os::Remove_RW_Mutex(Protable_thread_rwlock_t* pLock)
{
#ifdef __MY_Win_32__
	//noting to do
#else
	pthread_rwlock_destroy(pLock);
#endif
}

void Protable_os::Lock_Write_Mutex(Protable_thread_rwlock_t* pLock)
{
#ifdef __MY_Win_32__
	pLock->WaitToWrite();
#else
	pthread_rwlock_wrlock(pLock);
#endif
}

void Protable_os::Lock_Read_Mutex(Protable_thread_rwlock_t* pLock)
{
#ifdef __MY_Win_32__
	pLock->WaitToRead();
#else
	pthread_rwlock_rdlock(pLock);
#endif
}

void Protable_os::Unlock_RW_Mutex(Protable_thread_rwlock_t* pLock)
{
#ifdef __MY_Win_32__
	pLock->Done();
#else
	pthread_rwlock_unlock(pLock);
#endif
}

int Protable_os::Try_Lock_Write_Mutex(Protable_thread_rwlock_t* pLock)
{
#ifdef __MY_Win_32__
	return -1; //windows²»Ö§³Ö´Ë½Ó¿Ú
#else
	return pthread_rwlock_trywrlock(pLock);
#endif
}

int Protable_os::MkDir(const char* dirName, mode_t mode /*= 0*/)
{
#ifdef __MY_Win_32__
	return _mkdir(dirName);
#else
	return mkdir(dirName, mode);
#endif
}

int Protable_os::RMDir(const char* dirName)
{
#ifdef __MY_Win_32__
	return _rmdir(dirName);
#else
	return rmdir(dirName);
#endif
}

int Protable_os::Stat(const char *path, Protable_stat_t *buffer)
{
#ifdef __MY_Win_32__
	return _stat(path, buffer);
#else
	return stat(path, buffer);
#endif
}


bool Protable_os::Create_Thread(Protable_THR_FUNC func, void *args, Protable_thread_t *thr_id, Protable_hthread_t * t_handle , unsigned flags /*= 0*/)
{
#ifdef __MY_Win_32__
	if(flags)
	{
		flags = CREATE_SUSPENDED;
	}
	else
	{
		flags = 0;
	}
	*t_handle = (HANDLE)_beginthreadex(NULL, 0, (ACE_WIN32THRFUNC_T)func, args, flags, thr_id);
	if(*t_handle == 0)
		return false;
	return true;
#else
	int ret = pthread_create(thr_id,NULL,func, args);
	if(ret == 0)
		return true;
	return false;
#endif
}

void Protable_os::Resume_Thread(Protable_hthread_t t_handle)
{
	#ifdef __MY_Win_32__
	::ResumeThread(t_handle);
	#else
	#endif
}

void Protable_os::Join_Thread(Protable_thread_t th, Protable_THR_FUNC_RETURN* ret)
{
	#ifdef __MY_Win_32__

	#else
		pthread_join(th, ret);
	#endif
}

int Protable_os::GetLastError()
{
#ifdef __MY_Win_32__
	return (int)::GetLastError();
#else
	return errno;
#endif
}

long Protable_os::GetCpuNum()
{
#ifdef __MY_Win_32__
		SYSTEM_INFO  info;
		::GetSystemInfo(&info);
		return (long)info.dwNumberOfProcessors;
#else
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

int Protable_os::GetSelfThreadId()
{
#ifdef __MY_Win_32__
	return (int)GetCurrentThreadId();
#else
	return (int)pthread_self();
#endif
}

