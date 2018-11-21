#ifndef __TYPE_DEFINE_H__
#define __TYPE_DEFINE_H__

#include <string>
#include <vector>
#include <math.h>
#include <map>
#include "platform_socket.h"
#ifdef __MY_Win_32__
#include <WinSock2.h>
#endif

using namespace std;

 
#define CLIENTRINFOBIT  16

#define LOG_OPEN

typedef long long OBJ_ID; 

#ifdef UNICODE
typedef std::wstring string;
typedef std::wstringstream _tstringstream;
#ifndef __MY_Win_32__
typedef wchar_t TCHAR;
#endif
#else
typedef std::stringstream _tstringstream;
#ifndef __MY_Win_32__
typedef	char TCHAR;
#endif
#endif


#ifdef __MY_Win_32__
	typedef  unsigned short  mode_t;
	typedef int socklen_t;

#else
#define __int64 long long
#define VOID void
	typedef unsigned short	USHORT;
	typedef unsigned int	UINT;
	typedef unsigned long	DWORD;
	typedef unsigned char 	BYTE;
	typedef char			CHAR;
	typedef int				INT;
	typedef	INT				BOOL;
	typedef short			SHORT;
	
	#define _TRUNCATE -1
	#define vsnprintf_s(destStr,destSize, maxsize,format,ap)  vsnprintf(destStr,maxsize,format,ap)
	#define sprintf_s(destStr, destSize, format, ...)	sprintf(destStr, format, __VA_ARGS__)
	#define _vsnwprintf_s(wcs, wcsSize, maxlen, format, args)	vswprintf(wcs, maxlen, format, args)
	#define swprintf_s(destStr, destSize, format, ...)	swprintf(destStr, destSize, format, __VA_ARGS__)
	#define _snprintf_s snprintf 
	#define vsprintf_s(destStr,destSize,format, ap)			vsprintf(destStr,format, ap)
	#define strcpy_s(destStr, maxsize, srcStr) strncpy(destStr, srcStr, maxsize) 
	#define strcat_s(destStr, maxsize, srcStr)	strncat(destStr, srcStr, maxsize)
	#define localtime_s(ptm, timeep)	localtime_r(timeep, ptm)
	#define sscanf_s sscanf
	#define _ttoi atoi	
#define _strtoui64  strtoull
#define _atoi64 atoll

#ifndef SAFE_FREE
#define SAFE_FREE(x) if ( (x)!=NULL) {free(x); (x)=NULL;}
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define _T
#define _countof(_Array) (sizeof(_Array)/sizeof(_Array[0]))

#define _tmain main
#define _TCHAR char

#endif


#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if ( (x)!=NULL) {delete(x); (x)=NULL;}
#endif

#ifndef SAFE_DELETE_VEC
#define SAFE_DELETE_VEC(x) if ( (x)!=NULL) {delete [] (x); (x)=NULL;}
#endif

#if defined _HAS_ASSERT_CORE_DOWN
#define assert_debug(expr) assert(expr)
#define assert_debug_rtn_false(expr)   assert(expr)
#elif defined _HAS_ASSERT_LOG
#define assert_debug(expr) \
	do {\
		if ((expr) == false )\
		{\
			WriteLogA(LOG_ERROR, "assert fail");\
		}\
	}while(0) 

#define assert_debug_rtn_false(expr) \
	do {\
	if ((expr) == false )\
	{\
	WriteLogA(LOG_ERROR, "assert fail");\
	}\
	}while(0); \
	return false;
#else
#define assert_debug(expr) ((void)0)
#define assert_debug_rtn_false(expr)  ((void)0)
#endif

#define assert_fail(expr) assert_debug(false && expr)
#define assert_fail_rtn_false(expr) assert_debug_rtn_false(false && expr)



/*
extern const std::wstring AToW(const std::string& aString);
extern const std::string WToA(const std::wstring& wString);

extern bool WideToNarrow( const std::wstring &wStr, std::string &sStr);
extern bool NarrowToWide( const std::string &wStr, std::wstring &sStr);
*/

extern const std::wstring AToW(const std::string& aString);
extern const std::string WToA(const wstring& wString);
extern bool WideToNarrow( const wstring &wStr, std::string &sStr);
extern bool NarrowToWide( const std::string &wStr, wstring &sStr);

extern void PrintAnsiStrLog(const std::string &logStr);
extern std::string ChangeIntToStr(int srcInt);
extern int RandomInArea(int minNum, int MaxNum);
extern UINT encodeGold(UINT gold);
extern bool checkVersion(const std::string & version);

extern int MyRound(float number);


extern int GetNowWday();

extern std::string ConvertTimeToString(time_t time);
extern DWORD getMsTime();
// 得到今天零点时间
extern time_t GetZeroToday();
extern time_t GetZeroTime(UINT timeParam);
extern bool findInt(vector<unsigned int>::iterator begin, vector<unsigned int>::iterator end,unsigned int ival);

extern void CloseSocket(Protable_SOCKET sock);
extern void* mm_alloc_fn(size_t count, size_t sz);
/*
extern string GetIP(SOCKET inSocket);
extern int GetPort(SOCKET socket);
*/

extern bool IsTextUtf8(const char* str, size_t length);
extern bool IsUtf8File(const std::string& fileName);
extern bool ConvertStringTime( const std::string&soucStr ,time_t& ntime );


extern bool IsExistWdayVect(BYTE wday, const vector<BYTE> &wdayVect);

#ifdef __MY_Win_32__
extern void* bcopy(const void* src, void* dest, size_t count);
extern void* bzero(void* dest, size_t count);
#endif

#endif
