#ifndef __PROTABLE_OS_H__
#define __PROTABLE_OS_H__

#include "platform_define.h"
#include "type_define.h"
namespace Protable_os
{

	//mutex interface for os
	//mutex nType = 1 recursive lock otherwish normal lock
	extern void Init_Mutex(Protable_thread_mutex_t* pMutex, int nType);

	extern void Remove_Mutex(Protable_thread_mutex_t* pMutex);

	extern void Lock_Mutex(Protable_thread_mutex_t* pMutex);

	extern void Unlock_Mutex(Protable_thread_mutex_t* pMutex);

	extern int Try_Lock_Mutex(Protable_thread_mutex_t* pMutex);

	extern void Init_RW_Mutex(Protable_thread_rwlock_t* pLock);

	extern void Remove_RW_Mutex(Protable_thread_rwlock_t* pLock);

	extern void Lock_Write_Mutex(Protable_thread_rwlock_t* pLock);

	extern void Lock_Read_Mutex(Protable_thread_rwlock_t* pLock);

	extern void Unlock_RW_Mutex(Protable_thread_rwlock_t* pLock);

	extern int Try_Lock_Write_Mutex(Protable_thread_rwlock_t* pLock);
	//end mutex interface

	//dir interface for os
	extern int MkDir(const char* dirName, mode_t mode = 0);	
	extern int RMDir(const char* dirName);
	extern int Stat(const char *path, Protable_stat_t *buffer);

	//thread interface for os
	extern bool Create_Thread(Protable_THR_FUNC func, void *args, Protable_thread_t *thr_id, Protable_hthread_t * t_handle, unsigned flags = 0);

	extern void Resume_Thread(Protable_hthread_t t_handle);
	
	extern void Join_Thread(Protable_thread_t th, Protable_THR_FUNC_RETURN* ret);

	extern int GetLastError();
	
	extern long GetCpuNum();
	
	extern int GetSelfThreadId();

};

#endif
