#include "platform_define.h"

#ifdef __MY_Win_32__
WinRWLock::WinRWLock()
{
}

void WinRWLock::Init()
{
	m_nActive = m_nWaitingReaders = m_nWaitingWriters = 0;
	m_hsemReaders = CreateSemaphore(NULL, 0, MAXLONG, NULL);
	m_hsemWriters = CreateSemaphore(NULL, 0, MAXLONG, NULL);
	InitializeCriticalSection(&m_cs);
}

WinRWLock::~WinRWLock()
{
	m_nActive = m_nWaitingReaders = m_nWaitingWriters = 0;
	DeleteCriticalSection(&m_cs);
	CloseHandle(m_hsemReaders);
	CloseHandle(m_hsemWriters);
}

void WinRWLock::WaitToRead()
{
	EnterCriticalSection(&m_cs);
	bool fResouceWritePending = (m_nWaitingWriters || m_nActive < 0);
	if(fResouceWritePending)
	{
		m_nWaitingReaders ++;
	}
	else
	{
		m_nActive++;
	}
	LeaveCriticalSection(&m_cs);
	if(fResouceWritePending)
	{
		WaitForSingleObject(m_hsemReaders, INFINITE);
	}
}

void WinRWLock::WaitToWrite()
{
	EnterCriticalSection(&m_cs);
	bool fResouceOwned = (m_nActive != 0);
	if(fResouceOwned)
	{
		m_nWaitingWriters++;
	}
	else
	{
		m_nActive = -1;
	}
	LeaveCriticalSection(&m_cs);

	if(fResouceOwned)
	{
		WaitForSingleObject(m_hsemWriters, INFINITE);
	}
}

void WinRWLock::Done()
{
	EnterCriticalSection(&m_cs);
	if(m_nActive > 0)
	{
		m_nActive --;
	}
	else
	{
		m_nActive++;
	}

	HANDLE hsem = NULL;
	long lcount = 1;
	if(m_nActive == 0)
	{
		if(m_nWaitingWriters > 0)
		{
			m_nActive = -1;
			m_nWaitingWriters --;
			hsem = m_hsemWriters;
		}
		else if(m_nWaitingReaders > 0)
		{
			m_nActive = m_nWaitingReaders;
			m_nWaitingReaders = 0;
			hsem = m_hsemReaders;
			lcount = m_nActive;
		}
		else
		{
			//noting to do
		}
	}
	LeaveCriticalSection(&m_cs);

	if(hsem != NULL)
	{
		ReleaseSemaphore(hsem, lcount, NULL);
	}
}

#endif

