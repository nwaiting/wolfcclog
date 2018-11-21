#ifndef __LOG_H___
#define __LOG_H___

#include "Singleton.h"
#include "fix_mempool.h"
#include "message_queue.h"
#include "thread_svc.h"


enum LogLevel
{
	LOG_DEBUG,
	LOG_INFO,
	LOG_LUA,
	LOG_ASSERT,
	LOG_ERROR,
	LOG_FATAL,
};

enum LogAdapter
{
	LOG_ADAPTER_CONSOLE,
	LOG_ADAPTER_FILE,
	MAX_LOG_ADAPTER,
};

#define MAX_LOG_LEN   1024

#define WriteLogA(level, format, ...)  CLogger::Instance().WriteLog(level, __LINE__, __FILE__, format, ##__VA_ARGS__ )

#ifdef __MY_Win_32__
#define WriteLogW(level, format, ...)  CLogger::Instance().WriteLog(level, __LINE__, __FILEW__, format, ##__VA_ARGS__ )
#else
#define WriteLogW(level, format, ...)  CLogger::Instance().WriteLog(level, __LINE__, __FILE__, format, ##__VA_ARGS__ )
#endif

struct LogBuff : public FixObject<LogBuff>
{
	int m_bIsWchar;
	char buff[2*MAX_LOG_LEN];
	LogBuff(int isWchar = 0)
	{
		m_bIsWchar = isWchar;
		memset(buff, 0, sizeof(buff));
	}
};


class CLogAdapter
{
public:
	CLogAdapter(int nAdapter) :m_eAdapter(nAdapter)
	{
	}
	virtual ~CLogAdapter()
	{
	}

	virtual void OutputLog(const LogBuff* pBuff, const char* timeStamp) = 0;
	virtual void OutputLogDircect(const char* pBuff, const char* timeStamp) = 0;

public:
	int m_eAdapter;
};


class CConsoleAdpter : public CLogAdapter
{
public:
	CConsoleAdpter() :CLogAdapter(LOG_ADAPTER_CONSOLE)
	{

	}
	virtual void OutputLog(const LogBuff* pBuff, const char* timeStamp);
	virtual void OutputLogDircect(const char* pBuff, const char* timeStamp);
};

class CFileAdpter : public CLogAdapter
{
public:
	CFileAdpter(const char *appName, const char *logPath);
	~CFileAdpter();
	bool CreateLogFile(const tm& curDate);
	virtual void OutputLog(const LogBuff* pBuff, const char* timeStamp);
	virtual void OutputLogDircect(const char* pBuff, const char* timeStamp);

	std::string GetAppName()
	{
		return m_appName;
	}

	USHORT m_curDay;
private:
	std::string m_appName;
    std::string m_logPath;
	FILE* m_file;
};

class CLogger : public CThreadSvc
{
	Declare_Singleton(CLogger);
public:

	void WriteLog(int level, long lLine, const char* fileName, const char* format, ...);

	//void WriteLog(int level, long lLine, const wchar_t* fileName, const wchar_t* format, ...);

	void StartLogger(int level, bool bLogThread);

	void CloseLogger();

	virtual unsigned int OnRun();

	void WriteLogDirect(const char* logChar);

	bool CalcTimeStamp();

	void AddAdapter(CLogAdapter* adapter)
	{
		m_LogAdapters.push_back(adapter);
		if(adapter->m_eAdapter == LOG_ADAPTER_FILE) {
            m_fileAdapIndex = (int)m_LogAdapters.size() - 1;
		}
	}

	std::string GetFileName( const std::string& path)
	{
#ifdef __MY_Win_32__		
		size_t ind = path.find_last_of("\\");
#else
		size_t ind = path.find_last_of("/");
#endif
		return path.substr(ind + 1);
	}

	std::wstring GetFileName( const std::wstring& path)
	{
		size_t ind = path.find_last_of(L"\\");
		return path.substr(ind + 1);
	}
private:
	int m_fileAdapIndex;
	int m_curLevel;
	char m_timeBuff[64];
	std::vector<CLogAdapter*> m_LogAdapters;
	CMessageQueue<LogBuff> m_logQueue;
	volatile bool m_bEnd;
	volatile bool m_bEmpty;
	volatile bool m_bLogThread;
};


//×Ô¼º¶¨ÒåµÄASSERTºê
//¼ì²é£¬Ê§°ÜÔò¼ÇÂ¼ÈÕÖ¾(µ«ÊÇ²»·µ»Ø,º¯Êý¼ÌÐøÔËÐÐ)
#define CHECK(x) {if(!(x)) {WriteLogA(LOG_ASSERT, "ASSERT FAILED: (%s)", #x);}}
//¶ÏÑÔ£¬Ê§°ÜÔò¼ÇÂ¼ÈÕÖ¾²¢·µ»Ø
#define MYASSERT(x) {if(!(x)) {WriteLogA(LOG_ASSERT, "ASSERT FAILED: (%s)",#x ); return;}}
//¶ÏÑÔ£¬Ê§°ÜÔò¼ÇÂ¼ÈÕÖ¾²¢·µ»Øfalse
#define MYASSERTF(x) {if(!(x)) {WriteLogA(LOG_ASSERT, "ASSERT FAILED: (%s)", #x); return false;}}
//¶ÏÑÔ£¬Ê§°ÜÔò¼ÇÂ¼ÈÕÖ¾²¢·µ»ØNULL
#define MYASSERTN(x) {if(!(x)) {WriteLogA(LOG_ASSERT, "ASSERT FAILED: (%s)", #x); return NULL;}}
#endif

