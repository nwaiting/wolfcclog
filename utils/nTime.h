#ifndef __MY_Win_32__
#ifndef _NTIME_H_
#define _NTIME_H_

#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "typeDefine.h"
/// @brief	秒,分,时,日,周的毫秒数的宏.FOREVER代表不再执行,是UINT的最大值
#define ONE_SEC (1000)
#define HALF_SEC (500)
#define TEN_SEC (10000)
#define ONE_MIN (60000)
#define HALF_MIN (30000)
#define ONE_HOUR (3600000)
#define HALF_HOUR (1800000)
#define ONE_DAY (86400000)
#define HALF_DAY (43200000)
#define ONE_WEEK (604800000)
#define FOREVER (4294967294)

class nTime
{
    public:
		/// @brief	设置时区
		static bool setTimeZone();
        /// @brief	设置系统相对时间
        /// @param	offset 偏移量(秒)
        static void setAdjustTime(const int offset = 0);

		/// @brief	保留的时区信息
		//			程序初始化的时候设置，运行过程中一般不变
        static long timeZone;
        /// @brief	调整系统相对时间
        // 			主要用于在线调整服务器运行时间而不影响系统时间
        static long adjustTime;

    public:
        nTime()
        {
            now();
        }
        nTime(const nTime &rt)
        {
            _tv = rt._tv;
            _msec = rt._msec;
            _tm_ok = false;
        }
        nTime(const UINT timeInSec)
        {
            _tv.tv_sec = timeInSec + timeZone + adjustTime;
            _tv.tv_usec = 0;
            _msec = timeInSec * 1000;
            _tm_ok = false;
        }

		/// @brief 调用得到当前的系统时间
        void now()
        {
			/// @brief 获得timespec真实时间,用来填充结构 
            gettimeofday(&_tv, NULL);
            _msec = sec() * 1000L + _tv.tv_usec/1000;
            _tm_ok = false;
        }

        inline unsigned long sec() const
        {
            return _tv.tv_sec - timeZone - adjustTime;
        }

        inline unsigned long msec() const
        {
            return _msec;
        }

        inline unsigned long usec() const
        {
            return sec() * 1000000L + _tv.tv_usec;
        }

		/// @brief 计时器延迟
		/// @param delay 延迟的时间(毫秒)
        inline void addDelay(const unsigned long delay)
        {
            _tv.tv_usec += (delay * 1000L);
            _tv.tv_sec += (_tv.tv_usec / 1000000L);
            _tv.tv_usec %= 1000000L;
            _msec = sec() * 1000L + _tv.tv_usec / 1000L;
            _tm_ok = false;
        }

        inline nTime & operator= (const nTime &rt)
        {
            _tv = rt._tv;
            _msec = rt._msec;
            _tm_ok = false;
            return *this;
        }

        inline bool operator > (const nTime &rt) const
        {
            if (_msec <= rt._msec)
                return false;
            else
                return true;
        }

        inline bool operator >= (const nTime &rt) const
        {
            if (_msec >= rt._msec)
                return true;
            else
                return false;
        }

        inline bool operator < (const nTime &rt) const
        {
            if ( this->_msec >= rt._msec)
                return false;
            else
                return true;
        }

        inline bool operator <= (const nTime &rt) const
        {
            if ( this->_msec > rt._msec)
                return false;
            else
                return true;
        }

        inline bool operator == (const nTime &rt) const
        {
            return (rt._msec == _msec);
        }

        /// @brief	计时器消逝的时间，单位毫秒
        /// @param	rt 当前时间，需要比较的时间
        /// @return	计时器消逝的时间，单位毫秒
        unsigned long elapse(const nTime &rt) const
        {
            long val = rt.msec() - msec();
            if (val < 0L)
                return 0L;
            else
                return val;
        }

        /// @brief	得到本地时间
        /// @param	tv1 需要输出的时间结构
        /// @param	timeValue 输入时间
        /// @return	
        static void getLocalTime(struct tm & tv1, time_t timValue)
        {
            timValue = timValue - adjustTime - timeZone;
            gmtime_r(&timValue, &tv1);
        }

    protected:
        /// @brief	真实时间(精确到微秒级)
        struct timeval _tv;
        /// @brief	毫秒数
        unsigned long _msec;	
        /// @brief	标志_tm结构体数据是否有效
        bool _tm_ok;
};

class nDayTime : public nTime
{
    public:
        nDayTime()
        {
        }

    public:
        /// @brief	获得dayDiff前/后的日期.
        /// @param	dayDiff 正数表示以后,负数表示之前
        int getMDays(short dayDiff)
        {
            if(dayDiff == 0)
            {
                return getMDays();
            }
            time_t time = _tv.tv_sec + dayDiff * (ONE_DAY/1000);
            struct tm tv;
            getLocalTime(tv, time);
            return tv.tm_mday;
        }

        /// @brief	检查_tm结构体是否有效
        void checkTM()
        {
            if(!_tm_ok) 
            { 
                getLocalTime(_tm, _tv.tv_sec); 
                _tm_ok = true; 
            } 
        }

        /// @brief 	对时间进行格式化
        // 			按照指定的格式把时间格式化成字符串形式
        /// @param 	buffer 输出字符串缓冲
        /// @param 	bufferlen 输出字符串缓冲大小
        /// @param 	format 时间格式，请参见man strftime
        /// @return	
        UINT format(char *buffer, size_t bufferlen, const char *format = "%C/%m/%d %T")
        {
            checkTM();
            return strftime(buffer, bufferlen, format, &_tm);
        }

        /// @brief	获取分钟内的秒数
        /// @param	
        /// @return	分钟内的秒数
        int getSeconds()
        {
            checkTM();
            return _tm.tm_sec;
        }

        int getMinutes()
        {
            checkTM();
            return _tm.tm_min;
        }

        int getHours()
        {
            checkTM();
            return _tm.tm_hour;
        }

        /// @brief	获取一月内的天数
        /// @param	
        /// @return	一月内的天数
        int getMDays()
        {
            checkTM();
            return _tm.tm_mday;
        }

        /// @brief	获取一年内的月份数
        /// @param	
        /// @return	一年内的月份数
        int getMonths()
        {
            checkTM();
            return _tm.tm_mon;
        }

        /// @brief	获取自1970后的年数
        /// @param	
        /// @return	自1970后的年数
        int getYears()
        {
            checkTM();
            return _tm.tm_year;
        }

        /// @brief	获取一周内的天数
        /// @param	
        /// @return	一周内的天数
        int getWDays()
        {
            checkTM();
            return _tm.tm_wday;
        }

        /// @brief	获取一年内的天数
        /// @param	
        /// @return	一年内的天数
        int getYDays()
        {
            checkTM();
            return _tm.tm_yday;
        }

    private:
        /// @brief	存放日期数据
        struct tm _tm;
};
#endif
#endif

