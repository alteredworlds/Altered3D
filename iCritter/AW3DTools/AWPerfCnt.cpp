#ifdef _WIN32
#   include <windows.h>
#else
#	import <mach/mach.h>
#	import <mach/mach_time.h>
#endif

#include "AWPerfCnt.h"


//data members 
LARGE_INTEGER AWPerformanceCntr::m_cntrFrequency = {0, 0};
#ifndef _WIN32
struct timezone AWPerformanceCntr::m_tz = {0, 0};
#endif

double 
AWPerformanceCntr::doublePrecisionSeconds()
{
	static uint64_t		timebase = 0;
	uint64_t			time, nanos;
	double				seconds;
	
	// calculate the time base for this platform, only on the first time through
	if (timebase == 0)
	{
		mach_timebase_info_data_t	timebaseInfo;
		mach_timebase_info(&timebaseInfo);
		timebase = timebaseInfo.numer / timebaseInfo.denom;
	}
	
	// calculate time
	time = mach_absolute_time();
	nanos = time * timebase;
	
	// convert from nanoseconds to seconds
	seconds = (double)nanos * 1.0e-9;
	
	return seconds;
}


