#ifndef _PLATFORM_DEFINE_H_
#define _PLATFORM_DEFINE_H_

#include <stdio.h>

#ifdef __MY_Win_32__ //windows 32	

#ifndef  _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif

#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <WinSock2.h>
#include <direct.h>
#include <tchar.h>
#include <sys\stat.h>
#include <process.h>


class WinRWLock
{
public:
	WinRWLock();
	void Init();
	~WinRWLock();
	void WaitToRead();
	void WaitToWrite();
	void Done();

private:
	CRITICAL_SECTION m_cs;
	HANDLE m_hsemReaders;
	HANDLE m_hsemWriters;
	int m_nWaitingReaders;
	int m_nWaitingWriters;
	int m_nActive;
};

//type definds
typedef CRITICAL_SECTION						Protable_thread_mutex_t;
typedef void 												Protable_thread_mutexattr_t; 
typedef	 WinRWLock									Protable_thread_rwlock_t; //read and write lock
typedef struct _stat									Protable_stat_t;

typedef unsigned int									Protable_THR_FUNC_RETURN;
typedef UINT												Protable_thread_t;
typedef HANDLE										Protable_hthread_t;
typedef unsigned (__stdcall *ACE_WIN32THRFUNC_T)(void*);


#define	S_IRUSR	 0400	/* Read by owner.  */
#define	S_IWUSR  0200	/* Write by owner.  */
#define	S_IXUSR	 0100	/* Execute by owner.  */
/* Read, write, and execute by owner.  */
#define	S_IRWXU	(S_IRUSR|S_IRUSR|S_IRUSR)

#define	S_IRGRP	(S_IRUSR >> 3)	/* Read by group.  */
#define	S_IWGRP	(S_IWUSR >> 3)	/* Write by group.  */
#define	S_IXGRP	(S_IXUSR >> 3)	/* Execute by group.  */
/* Read, write, and execute by group.  */
#define	S_IRWXG	(S_IRWXU >> 3)

#define	S_IROTH	(S_IRGRP >> 3)	/* Read by others.  */
#define	S_IWOTH	(S_IWGRP >> 3)	/* Write by others.  */
#define	S_IXOTH	(S_IXGRP >> 3)	/* Execute by others.  */
/* Read, write, and execute by others.  */
#define	S_IRWXO	(S_IRWXG >> 3)


#else  //linux

#include <pthread.h>
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>


#define __declspec(dllexport)
#define Sleep(millsec) usleep(millsec* 1000)

#define __FILEW__          L(__FILE__)

//type definds
typedef pthread_mutex_t  		Protable_thread_mutex_t;
typedef pthread_mutexattr_t 	Protable_thread_mutexattr_t;
typedef	 pthread_rwlock_t			Protable_thread_rwlock_t; //read and write lock
typedef struct stat						Protable_stat_t;

typedef pthread_t					Protable_thread_t;
typedef Protable_thread_t				Protable_hthread_t;
typedef void*						Protable_THR_FUNC_RETURN;

#endif

typedef Protable_THR_FUNC_RETURN	(*Protable_THR_FUNC)(void *);



#endif


