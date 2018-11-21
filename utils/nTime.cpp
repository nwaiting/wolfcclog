#include "nTime.h"

#ifndef __MY_Win_32__
long nTime::timeZone = 0L;
long nTime::adjustTime = 0L;

bool nTime::setTimeZone()
{
	if (0L == timeZone)
	{
		tzset();
		timeZone = timezone;
		return true;
	}
	return false;
}

void nTime::setAdjustTime(const int offset)
{
	adjustTime -= offset;
}

#endif
