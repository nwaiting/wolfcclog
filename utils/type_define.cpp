#include "type_define.h"
#ifdef __MY_Win_32__
#include <Windows.h>
#include <tchar.h>
#else
#include <iconv.h>
#endif
#include <math.h>
#include <time.h>
#include <vector>
#include <sstream>
#include <assert.h>
#include <iostream>
#include <strstream>
#include <fstream>
#include <stdint.h>
#include "logging.h"
#include "nTime.h"

using namespace std;

#ifndef __MY_Win_32__
static void initGlobal() __attribute__ ((constructor));
void initGlobal()
{
    nTime::setTimeZone();
}
#endif

const std::wstring AToW(const std::string& aString)
{
    wchar_t tmp[8192] = {0};
    #ifdef  __MY_Win_32__
    mbstowcs_s(NULL, tmp, 8192, aString.c_str(), aString.size());
    #else
    mbstowcs(tmp, aString.c_str(), aString.size()*2);
    #endif
    return std::wstring(tmp);
}

const std::string WToA(const wstring& wString)
{
    char tmp[8192] = {0};
    #ifdef  __MY_Win_32__
    wcstombs_s(NULL, tmp, 8192, wString.c_str(), wString.size());
    #else
    wcstombs(tmp,wString.c_str(),8192);
    #endif
    return std::string(tmp);
}



bool WideToNarrow( const wstring&wStr, std::string &sStr)
{
    #ifdef  __MY_Win_32__
    int size = WideCharToMultiByte(CP_UTF8,0,wStr.c_str(),-1,NULL,0,NULL,NULL);
    char *ch = new char[size + 1];
    if (!WideCharToMultiByte(CP_UTF8,0,wStr.c_str(),-1,ch,size,NULL,NULL))
    {
        delete [] ch ;
        return false;
    }
    sStr = ch;
    delete [] ch ;
    #else
    size_t srcSize = wStr.size()*2;
    size_t destSize = srcSize + 1;
    size_t result;
    iconv_t env;
    env = iconv_open("UTF-8", "WCHAR_T");
    if (env == (iconv_t)-1)
    {
        return false;
    }
    char *ch = new char[srcSize + 1];
    //TODO
    const wchar_t * pwStr = wStr.c_str();
    result = iconv(env, (char**)&pwStr,(size_t*)&srcSize, (char**)&ch, (size_t*)&destSize);
    if (result == (size_t)-1)
    {
        delete [] ch ;
        return false;
    }
    iconv_close(env);
    sStr = ch;
    delete [] ch ;
    #endif
    return true;
}

bool NarrowToWide( const std::string &sStr, wstring &wStr)
{
    #ifdef  __MY_Win_32__
    int size = MultiByteToWideChar(CP_UTF8,0,sStr.c_str(),-1,NULL,0);
    wchar_t *ch = new wchar_t[size + 1];
    if (!MultiByteToWideChar(CP_UTF8,0,sStr.c_str(),-1,ch,size))
    {
        delete [] ch ;
        return false;
    }
    wStr = ch;

    delete [] ch ;
    #else
    size_t result;
    iconv_t env;
    int size = sStr.size()+1;
    int destSize = size;
    env = iconv_open("WCHAR_T", "UTF-8");
    if (env == (iconv_t)-1)
    {
        return false;
    }
    wchar_t *ch = new wchar_t[size + 1];
    const char* psStr = sStr.c_str();
    result = iconv(env, (char**)&psStr, (size_t*)&size, (char**)&ch, (size_t*)&destSize);
    if (result == (size_t)-1)
    {
        delete [] ch ;
        return false;
    }
    iconv_close(env);
    wStr = ch;
    delete [] ch ;
    #endif
    return true;
}

// [left , right]
int RandomInArea(int left, int right)
{
    int temp = right - left+1;
    if (temp <= 0)
    {
        return 0;
    }
    //srand((int)time(NULL));
    unsigned int randomRate = (::rand() << 17) | (::rand() <<3) | (::rand());
    return temp == 0 ? left : (left + randomRate % temp); 
}

int MyRound(float number)
{
    int ret = (int)(number + 0.5);
    return ret;
}


int GetNowWday()
{
    time_t curtime = time((time_t*)NULL);
    tm pTime;
    localtime_s(&pTime, &curtime);

    if (pTime.tm_wday == 0)
    {
        return 7;
    }
    return pTime.tm_wday;
}

time_t GetZeroToday()
{
    time_t curtime = time((time_t*)NULL);
    
    tm pTime;
    localtime_s(&pTime, &curtime);

    pTime.tm_hour = 0;
    pTime.tm_min = 0;
    pTime.tm_sec = 0;

    return mktime( &pTime );
}

time_t GetZeroTime(UINT timeParam)
{
    time_t curtime = timeParam;

    tm pTime;
    localtime_s(&pTime, &curtime);

    pTime.tm_hour = 0;
    pTime.tm_min = 0;
    pTime.tm_sec = 0;

    return mktime( &pTime );
}
bool findInt(vector<UINT>::iterator begin, vector<UINT>::iterator end,UINT ival)
{
    while (begin != end)
    {
        if (*begin == ival)
            break;
        else
            ++begin;
    }

    if (begin != end)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*
string GetIP(SOCKET socket)
{
    sockaddr_in sin;
    int len = sizeof(sin);
    getpeername(socket, (struct sockaddr*)&sin, &len);

    string ip =  string(inet_ntoa(sin.sin_addr));
    //int port = ntohs(sin.sin_port);
    return ip;
}

int GetPort(SOCKET socket)
{
    sockaddr_in sin;
    int len = sizeof(sin);
    getpeername(socket, (struct sockaddr*)&sin, &len);

    //string ip =  string(inet_ntoa(sin.sin_addr));
    return ntohs(sin.sin_port);
}
*/
std::string ConvertTimeToString( time_t time ) 
{
    char szbuffer[64]={0} ;
    struct tm tm_now ;
    localtime_s( &tm_now ,&time ) ;
    sprintf_s( szbuffer , _countof(szbuffer) , ("%d-%.2d-%.2d %.2d:%.2d:%.2d") ,
        tm_now.tm_year+1900 ,tm_now.tm_mon+1,tm_now.tm_mday,
        tm_now.tm_hour,tm_now.tm_min ,tm_now.tm_sec ) ;
    return szbuffer ;
}

void PrintAnsiStrLog(const std::string &logStr)
{
    /*
    std::wstring unicodeLog;
    NarrowToWide(logStr,unicodeLog);
    WriteLogW(LOG_ERROR, _T("%s error"), unicodeLog.c_str());
    */
}

std::string ChangeIntToStr(int srcInt)
{
    strstream ss;
    std::string destStr;
    ss << srcInt;
    ss >> destStr;
    return destStr;
}

bool checkVersion(const std::string & version)
{
    if (version.compare("v 1.0.0") > 0)
    {
        return true;
    }
    return false;
}

DWORD getMsTime()
{
    UINT MSTime = 0;
#ifdef __MY_Win_32__
    MSTime = GetTickCount();
#else
    timeval tv;
    gettimeofday(&tv, NULL);
    MSTime = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
    return MSTime;
}

void CloseSocket(Protable_SOCKET sock)
{
#ifdef __MY_Win_32__
    closesocket(sock);
#else
    ::close(sock);
#endif
}

void* mm_alloc_fn(size_t count, size_t size)
{
    size_t sz = count * size;
    void *p = malloc(sz);
    if (p)
        memset(p, 0, sz);
    return p;
}

#ifdef __MY_Win_32__
void* bcopy(const void* src, void* dest, size_t count)
{
    return memcpy(dest, src, count);
}
void* bzero(void* dest, size_t count)
{
    return memset(dest, 0 ,count);
}
#endif

bool IsTextUtf8(const char* str, size_t length)
{
    int nBytes = 0;
    unsigned char chr = 0;
    bool bAllAscii = true;
    for (uint32_t i = 0; i < length; i++) {
        chr = *(str+i);
        if ( (chr&0x80) != 0)
            bAllAscii = false;
        if (nBytes == 0) {
            if ((chr & 0x80) != 0) {
                while((chr & 0x80) != 0) {
                    chr <<= 1;
                    nBytes++;
                }
                if (nBytes < 2 || nBytes > 6)
                    return false;
                nBytes--;
            }
        }
        else {
            if ((chr & 0xc0) != 0x80)
                return false;
            nBytes--;
        }
    }
    if (bAllAscii)
        return false;
    return nBytes == 0; 
}
bool IsUtf8File(const std::string& fileName)
{
    ifstream fin(fileName.c_str(), ios::binary);
    if (!fin) {
        WriteLogA(LOG_ERROR,"can not open file when check utf8");
        return false;
    }
    else
    {
        BYTE bytes[3]={0};
        fin.read((char*)&bytes, sizeof(bytes) );
        if (bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) {
            return true;
        }
        else {
            std::string line;
            std::getline(fin, line);
            if (IsTextUtf8(line.c_str(), line.size()))
                return true;

        }
        fin.close();
    }
    return false;
}

bool ConvertStringTime( const std::string&soucStr ,time_t& ntime )
{
    struct tm tm_time = { 0 } ;
    /*
    int nresult = sscanf( soucStr.c_str(), "%d-%d-%d %d:%d:%d",
        &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday, 
        &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec
        ) ;
    */

    int nresult = sscanf_s(soucStr.c_str(), "%d-%d-%d %d:%d:%d",
        &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
        &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec);

    if ( nresult != 6) {
        assert_fail_rtn_false( "time format error" );
    }
    tm_time.tm_year -= 1900 ;
    tm_time.tm_mon -=1;
    ntime = mktime( &tm_time ) ;
    return true ;
}

bool IsExistWdayVect(BYTE wday, const vector<BYTE> &wdayVect)
{
    for (size_t i = 0; i < wdayVect.size(); i++) {
        if (wday == wdayVect[i]){
            return true;
        }
    }
    return false;
}
