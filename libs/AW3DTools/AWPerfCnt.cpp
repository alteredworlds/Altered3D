#ifdef _WIN32
#   include <windows.h>
#endif
#include "AWPerfCnt.h"


//data members 
LARGE_INTEGER AWPerformanceCntr::m_cntrFrequency = {0, 0};
#ifndef _WIN32
struct timezone AWPerformanceCntr::m_tz = {0, 0};
#endif


