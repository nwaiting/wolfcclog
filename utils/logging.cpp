#include <time.h>
#include <sstream>
#include "logging.h"
#include "platform_define.h"

CLogger::CLogger()
{
	m_curLevel = 0;
    m_fileAdapIndex = -1;
	m_bEnd = false;
	m_bLogThread = false;
	memset(m_timeBuff, 0 ,sizeof(m_timeBuff));
}

CLogger::~CLogger()
{
	for(size_t i = 0; i < m_LogAdapters.size(); ++i) {
		delete m_LogAdapters[i];
	}
}

void CLogger::WriteLog(int level, long lLine, const char* fileName, const char* format, ...)
{
    if (level < m_curLevel || m_LogAdapters.empty() || m_bEnd) {
        return;
    }

	char infoBuf[MAX_LOG_LEN] = {0};
	va_list argptr;
	va_start(argptr, format);
	vsnprintf_s(infoBuf, sizeof(infoBuf), _TRUNCATE, format, argptr);
	va_end(argptr);

	std::string errorFlag;
	switch(level)
	{
	case LOG_FATAL:
		errorFlag = " [FATAL] ";
		break;
	case LOG_ERROR:
		errorFlag = " [ERROR] ";
		break;
	case LOG_LUA:
		errorFlag = " [LUA] ";
		break;
	case LOG_INFO:
		errorFlag = " [INFO] ";
		break;
	case LOG_DEBUG:
		errorFlag = " [DEBUG] ";
		break;
	default:
		errorFlag = " [UNKNOWN] ";
		break;
	}
	
	std::string fName = GetFileName(fileName);
	if (m_bLogThread) {
		LogBuff* pLogBuff = new LogBuff();
        if (pLogBuff){
            sprintf_s((char*)pLogBuff->buff, sizeof(pLogBuff->buff), "%s%s [%s:%ld]\n", errorFlag.c_str(), infoBuf, fName.c_str(), lLine);
            m_logQueue.AddMsg(pLogBuff);
        }
	}
	else {
		char buff[2*MAX_LOG_LEN];
		memset(buff, 0, sizeof(buff));
		sprintf_s((char*)buff, sizeof(buff),"%s%s [%s:%ld]\n", errorFlag.c_str(), infoBuf, fName.c_str(), lLine);
		WriteLogDirect(buff);
	}
	
}

void CLogger::StartLogger(int level, bool bLogThread)
{
	m_curLevel = level;
	m_bEnd = false;
	m_bEmpty = false;
	m_bLogThread = bLogThread;
    if (m_bLogThread){
        StartThread(1);
    }
}

void CLogger::CloseLogger()
{
	m_bEnd = true;
	m_bEmpty = false;
	WaitThreadEnd();
}

void CLogger::WriteLogDirect(const char* logChar)
{
	CalcTimeStamp();
	for(size_t i = 0; i < m_LogAdapters.size(); ++i) {
		m_LogAdapters[i]->OutputLogDircect(logChar, m_timeBuff);
	}
}

unsigned int CLogger::OnRun()
{
	while(!m_bEnd || !m_bEmpty) {
		LogBuff* pBuff = m_logQueue.PopMsg();
		if(pBuff) {
			CalcTimeStamp();
			for(size_t i = 0; i < m_LogAdapters.size(); ++i) {
				m_LogAdapters[i]->OutputLog(pBuff, m_timeBuff);
			}
			delete pBuff;
			Sleep(5);
		}
		else {
			if(m_bEnd) {
				m_bEmpty = true;
			}
            else{
                Sleep(100);
            }
		}
	}

	return 0;
}

bool CLogger::CalcTimeStamp()
{
	time_t now;
	time(&now);
	struct tm tm_now;
	localtime_s(&tm_now, &now);
	strftime(m_timeBuff, 64, "[%H:%M:%S]", &tm_now);
	
    if (m_fileAdapIndex != -1) {
        return ((CFileAdpter*)m_LogAdapters[m_fileAdapIndex])->CreateLogFile(tm_now);
	}

	return true;
}

void CConsoleAdpter::OutputLogDircect(const char* pBuff, const char* timeStamp)
{
	printf("%s\n", timeStamp);
	printf("%s\n", pBuff);

	fflush(stdout);
}

void CConsoleAdpter::OutputLog(const LogBuff* pBuff, const char* timeStamp)
{
	printf("%s\n", timeStamp);
	if(pBuff->m_bIsWchar) {
		wprintf(L"%s\n", pBuff->buff);
	}
	else {
		printf("%s\n", (char*)pBuff->buff);
	}
	fflush(stdout);
}

CFileAdpter::CFileAdpter(const char *appName, const char *logPath)
    : CLogAdapter(LOG_ADAPTER_FILE)
    , m_curDay(-1)
    , m_appName(appName)
    , m_file(NULL)
    , m_logPath(logPath)
{
	int er;
	//int i = Protable_os::MkDir("logs", S_IRWXU|S_IRGRP|S_IROTH);
    int i = Protable_os::MkDir(logPath, S_IRWXU | S_IRGRP | S_IROTH);
	if(i == -1) {
		er = errno;
	}
}

CFileAdpter::~CFileAdpter()
{
    if (m_file) {
        fclose(m_file);
    }
}

bool CFileAdpter::CreateLogFile(const tm& curDate)
{
	if(curDate.tm_mday != m_curDay || !m_file) {
		m_curDay = curDate.tm_mday;
		if(m_file) {
			fclose(m_file);
		}
		char bufName[64] = {0};
#ifdef __MY_Win_32__
		_snprintf_s(bufName, 64,  _TRUNCATE, "%s\\%s_%d_%d_%d.log", m_logPath.c_str(), m_appName.c_str(), curDate.tm_year + 1900, curDate.tm_mon + 1, curDate.tm_mday);
#else
		snprintf(bufName, 64, "%s/%s_%d_%d_%d.log", m_logPath.c_str(),m_appName.c_str(), curDate.tm_year + 1900, curDate.tm_mon + 1, curDate.tm_mday);
		std::string of = bufName;
#endif

#ifdef __MY_Win_32__
		errno_t err = fopen_s(&m_file, bufName, "at" );
		if (err != 0) {
            char errbuf[64];
            memset(errbuf, 0, sizeof(errbuf));
            strerror_s(errbuf, 64-1, err);
            printf("open file error %s\n", errbuf);
			if (m_file != NULL) {
				fclose(m_file);
				return false;
			}
			return false;
		}
#else
		m_file = fopen(bufName, "at");
        if (!m_file) {
            char errbuf[64];
            memset(errbuf, 0, sizeof(errbuf));
            strerror_s(errbuf, 64 - 1, err);
            printf("open file error %s\n", errbuf);
        }
#endif

        if (m_file == NULL){
            return false;
        }
#ifndef __MY_Win_32__
		std::string lnTarg = m_appName+"log";
		std::ostringstream oss;
		oss.str("");
		oss<<"ln --force -s "<< of <<" "<<lnTarg;
		system(oss.str().c_str());
#endif
	}
	return true;
}

void CFileAdpter::OutputLogDircect(const char* pBuff, const char* timeStamp)
{
    if (m_file){
        fprintf(m_file, "%s", timeStamp);
        fprintf(m_file, "%s", (char*)pBuff);
        fflush(m_file);
    }
}

void CFileAdpter:: OutputLog(const LogBuff* pBuff, const char* timeStamp)
{
    if (m_file) {
        fprintf(m_file, "%s", timeStamp);
        if (pBuff->m_bIsWchar) {
            fwprintf(m_file, L"%s", pBuff->buff);
        }
        else {
            fprintf(m_file, "%s", (char*)pBuff->buff);
        }
        fflush(m_file);
    }
}
