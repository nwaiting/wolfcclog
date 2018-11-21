#ifndef __LOCK_H__
#define __LOCK_H__

#include "protable_os.h"
#include "platform_define.h"

// lock interface
class InterfaceLock
{
public:
	InterfaceLock(){}

	virtual ~InterfaceLock(){}

	//destroy a lock
	virtual void Remove() = 0;

	// add lock to a thread
	virtual void Lock() = 0;

	//release lock from the thread
	virtual void Unlock() = 0;

	// test add lock to a thread
	// 0 unsuccess get lock, otherwish success
	virtual int TryLock() = 0;

	//add read lock to a thread
	virtual void ReadLock() = 0;

	//add write lock to a thread
	virtual void WriteLock() = 0;
};

class CThreadLock : public  InterfaceLock
{
public:
	CThreadLock(void)
	{
#ifdef __MY_Win_32__
		Protable_os::Init_Mutex(&m_lock, 0);
#else
		Protable_os::Init_Mutex(&m_lock, 1);
#endif
	}

	virtual ~CThreadLock(void)
	{
		this->Remove();
	}

	virtual void Remove()
	{
		Protable_os::Remove_Mutex(&m_lock);
	}

	virtual void Lock()
	{
		Protable_os::Lock_Mutex(&m_lock);
	}

	virtual void Unlock()
	{
		Protable_os::Unlock_Mutex(&m_lock);
	}

	// test add lock to a thread
	virtual int TryLock()
	{
		return Protable_os::Try_Lock_Mutex(&m_lock);
	}

	//add read lock to a thread
	virtual void ReadLock()
	{
		Protable_os::Lock_Mutex(&m_lock);
	}

	//add write lock to a thread
	virtual void WriteLock()
	{
		Protable_os::Lock_Mutex(&m_lock);
	}

private:
	Protable_thread_mutex_t  m_lock;
};

class CThreadRecursiveLock : public  InterfaceLock
{
public:
	CThreadRecursiveLock(void)
	{
		Protable_os::Init_Mutex(&m_lock, 1);
	}

	virtual ~CThreadRecursiveLock(void)
	{
		this->Remove();
	}

	virtual void Remove()
	{
		Protable_os::Remove_Mutex(&m_lock);
	}

	virtual void Lock()
	{
		Protable_os::Lock_Mutex(&m_lock);
	}

	virtual void Unlock()
	{
		Protable_os::Unlock_Mutex(&m_lock);
	}

	// test add lock to a thread
	virtual int TryLock()
	{
		return Protable_os::Try_Lock_Mutex(&m_lock);
	}

	//add read lock to a thread
	virtual void ReadLock()
	{
		Protable_os::Lock_Mutex(&m_lock);
	}

	//add write lock to a thread
	virtual void WriteLock()
	{
		Protable_os::Lock_Mutex(&m_lock);
	}

private:
	Protable_thread_mutex_t  m_lock;

};

class CThreadRWLock : public  InterfaceLock
{
public:
	CThreadRWLock(void)
	{
		Protable_os::Init_RW_Mutex(&m_lock);
	}

	virtual ~CThreadRWLock(void)
	{
		this->Remove();
	}

	virtual void Remove()
	{
		Protable_os::Remove_RW_Mutex(&m_lock);
	}

	virtual void Lock()
	{
		Protable_os::Lock_Write_Mutex(&m_lock);
	}

	virtual void Unlock()
	{
		Protable_os::Unlock_RW_Mutex(&m_lock);
	}

	// test add lock to a thread
	virtual int TryLock()
	{
		return Protable_os::Try_Lock_Write_Mutex(&m_lock);
	}

	//add read lock to a thread
	virtual void ReadLock()
	{
		Protable_os::Lock_Read_Mutex(&m_lock);
	}

	//add write lock to a thread
	virtual void WriteLock()
	{
		Protable_os::Lock_Write_Mutex(&m_lock);
	}

private:
	Protable_thread_rwlock_t  m_lock;

};

class CAutoLock
{
public:
	CAutoLock(InterfaceLock& refLock)
		:m_refLock(refLock)
	{
		m_refLock.Lock();
	}

	~CAutoLock()
	{
		m_refLock.Unlock();
	}

private:
	InterfaceLock & m_refLock;
};

class CAutoRWLock
{
public:
	// isReadLock = true 
	CAutoRWLock(InterfaceLock& refLock, bool isReadLock = true)
		:m_refLock(refLock)
	{
		if(isReadLock)
			m_refLock.ReadLock();
		else
			m_refLock.WriteLock();
	}

	~CAutoRWLock()
	{
		m_refLock.Unlock();
	}

private:
	InterfaceLock & m_refLock;
};


#endif
