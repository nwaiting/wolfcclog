#ifndef __THREAD_SVC_H__
#define __THREAD_SVC_H__
#include "platform_define.h"
#include "type_define.h"

class CThreadSvc
{
public:
	CThreadSvc(void);
	virtual ~CThreadSvc(void);
	

	bool StartThread(USHORT thrNum, UINT flag = 0);

	void ResumeAllThread();

	/*
	 *@description: 
	 *@pramater: 
	 *@return: 
	 *@time: by lihl written
	 */
	void WaitThreadEnd();

	const std::vector<Protable_thread_t>& GetThreadIDs();

protected:

	virtual unsigned int OnRun() = 0;

private:

	static Protable_THR_FUNC_RETURN start_address( void * arg);

private:
	std::vector<Protable_hthread_t> m_thrHandles; 
	std::vector<Protable_thread_t> m_thrIds;	   
};

#endif
