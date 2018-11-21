#include "thread_svc.h"
#include "protable_os.h"


CThreadSvc::CThreadSvc(void)
{
}

CThreadSvc::~CThreadSvc(void)
{
}

bool CThreadSvc::StartThread(USHORT thrNum, UINT flag)
{
    if (thrNum == 0){
        return false;
    }
		
	for(USHORT i = 0; i <thrNum; ++i) {
		Protable_thread_t threadId = 0;
		Protable_hthread_t thrHandle;
		bool ret = Protable_os::Create_Thread((Protable_THR_FUNC)start_address,this, &threadId, &thrHandle, flag);
		if(!ret) {
			return false;
		}
		m_thrHandles.push_back(thrHandle);
		m_thrIds.push_back(threadId);
	}
	return true;
}

void CThreadSvc::ResumeAllThread()
{
	for(size_t i = 0 ; i < m_thrHandles.size(); ++i) {
		Protable_os::Resume_Thread(m_thrHandles[i]);
	}
}

void CThreadSvc::WaitThreadEnd()
{
	#ifdef __MY_Win_32__
		WaitForMultipleObjects((DWORD)m_thrHandles.size(), &m_thrHandles[0], true, INFINITE);
		for(size_t i = 0; i < m_thrHandles.size(); ++i) {
			CloseHandle(m_thrHandles[i]);
		}
	#else
		for(size_t i = 0; i < this->m_thrIds.size(); ++i) {
			Protable_THR_FUNC_RETURN retState;
			Protable_os::Join_Thread(m_thrIds[i], &retState);
		}
	#endif
}

Protable_THR_FUNC_RETURN CThreadSvc::start_address( void * arg)
{
	CThreadSvc* pThis = (CThreadSvc*)arg;
	return (Protable_THR_FUNC_RETURN)pThis->OnRun();
} 

const std::vector<Protable_thread_t>& CThreadSvc::GetThreadIDs()
{
	return m_thrIds;
}
