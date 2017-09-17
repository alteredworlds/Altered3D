#ifndef __AWPERFCNT_H
#define __AWPERFCNT_H

#include "AWToolsCommon.h"

#define NATIVE_TIME float
#define NATIVE_MIN  -999999999.0f
#define NATIVE_MAX  999999999.0f
#define PERCENT_FAC 100.0f

#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

class AWExport AWPerformanceCntr
{
public:
	AWPerformanceCntr();
	AWPerformanceCntr(const AWPerformanceCntr& other)	{*this=other;}	
	AWPerformanceCntr& operator=(const AWPerformanceCntr& other);
	~AWPerformanceCntr();

//start/stop the count
	int		start();
	float	   getTime();
	int		stop();
//reset the max/min values
	void	reset();


	static AWBoolean getTime(LARGE_INTEGER& t);
	static float getFrequency(NATIVE_TIME diff);
	float  difTimeS(const LARGE_INTEGER& a, const LARGE_INTEGER& b);

	float	getFrequency();
	float	getMinFrequency();
	float	getMaxFrequency();


//data members
	static LARGE_INTEGER m_cntrFrequency;
#ifndef _WIN32
	static struct timezone m_tz;
#endif

public:
	LARGE_INTEGER   m_cTime;
	NATIVE_TIME		m_minDiff, m_maxDiff, m_diff;
};//class AWPerformanceCntr


class AWExport AWEnginePerformanceCntr
{
public:
	AWEnginePerformanceCntr();
	~AWEnginePerformanceCntr();

//start/stop the count
	void	zeroTime();
	int		startRender();
	int		stopRender();

	int		startTransform();
	int		stopTransform();

	int		startCollide();
	int		stopCollide();

	void	update(int trisThisFrame);	//updates the overall frame time, inc. max/min values
	void	reset();	//resets max/min values to initial states

	float	getTime();

//get overall results
	float	getFrequency();
	float	getRenderPercent();
	float	getTransformPercent();
	float	getCollidePercent();

	float	getMinFrequency();
	float	getMaxFrequency();
	
	int     getTrisThisFrame()      {return m_curTriangles;}

	int		getMinRenderPercent();
	int		getMinTransformPercent();
	int		getMinCollidePercent();

	int		getMaxRenderPercent();
	int		getMaxTransformPercent();
	int		getMaxCollidePercent();

public:
	AWPerformanceCntr	m_systemTime;
	AWPerformanceCntr	m_renderCnt, m_transformCnt, m_collideCnt;
	NATIVE_TIME	        m_minRenderCnt, m_minTransformCnt,m_minCollideCnt; 	
	NATIVE_TIME	        m_maxRenderCnt,m_maxTransformCnt, m_maxCollideCnt;
	int         	    m_minTimeTriangles, m_maxTimeTriangles, m_curTriangles;
};//class AWEnginePerformanceCntr



//INLINE functions
inline
AWPerformanceCntr::AWPerformanceCntr() :
	m_minDiff(NATIVE_MAX), m_maxDiff(NATIVE_MIN)
{
	if (0 == m_cntrFrequency.u.LowPart)
	{
#ifdef _WIN32
		//find system clock rate
		QueryPerformanceFrequency( &m_cntrFrequency );
#endif
	}//if (0 == m_cntrFrequency)
}//AWPerformanceCntr::AWPerformanceCntr()


inline AWPerformanceCntr&	
AWPerformanceCntr::operator=(const AWPerformanceCntr& other)
{
	m_cTime     = other.m_cTime;
	m_minDiff	= other.m_minDiff;
	m_maxDiff	= other.m_maxDiff;
	m_diff		= other.m_diff;
	return *this;
}


inline
AWPerformanceCntr::~AWPerformanceCntr()
{
}


inline void
AWPerformanceCntr::reset()
{
	m_minDiff = NATIVE_MAX;
	m_maxDiff = NATIVE_MIN;
}


//start/stop the count
inline int		
AWPerformanceCntr::start()
{
	return (int)getTime(m_cTime);
}


inline AWBoolean 
AWPerformanceCntr::getTime(LARGE_INTEGER& t)
{
#ifndef _WIN32
	return (AWBoolean)gettimeofday((struct timeval*)&t.QuadPart, &m_tz);
#else
	return (AWBoolean)QueryPerformanceCounter( &t );
#endif
}//AWBoolean AWPerformanceCntr::getTime(LARGE_INTEGER& t)



inline float
AWPerformanceCntr::difTimeS(const LARGE_INTEGER& a, const LARGE_INTEGER& b)
{
#ifndef _WIN32
	return (float)((struct timeval&)a).tv_sec - (float)((struct timeval&)b).tv_sec +
	                  (float)( ((struct timeval&)a).tv_usec - ((struct timeval&)b).tv_usec ) / 1000000.0f;
#else
	if (m_cntrFrequency.QuadPart)
	{
		return (float)((double)(a.QuadPart - b.QuadPart) / (double)m_cntrFrequency.QuadPart);	
	}
	else return 0.0f;
#endif
}//float AWPerformanceCntr::difTimeS(const LARGE_INTEGER& a, const LARGE_INTEGER& b)



inline float
AWPerformanceCntr::getTime()
{
	LARGE_INTEGER temp;
	getTime(temp);
    return difTimeS(temp, m_cTime);
}//inline float& AWPerformanceCntr::getTime()



inline int		
AWPerformanceCntr::stop()
{
    LARGE_INTEGER temp;
    getTime(temp);
    m_diff = difTimeS(temp, m_cTime);
	if (m_diff < m_minDiff)
		m_minDiff = m_diff;
	else if (m_diff > m_maxDiff)
		m_maxDiff = m_diff;
	return 0;
}//int AWPerformanceCntr::stop()



inline float
AWPerformanceCntr::getFrequency(NATIVE_TIME diff)
{
	if (diff)
	{
		return 1.0f / diff;
	}
	else return 0.0f;
}



inline float
AWPerformanceCntr::getMinFrequency()
{
	return getFrequency(m_maxDiff);
}


inline float
AWPerformanceCntr::getMaxFrequency()
{
	return getFrequency(m_minDiff);
}


inline float	
AWPerformanceCntr::getFrequency()
{
	return getFrequency(m_diff);
}

inline
AWEnginePerformanceCntr::AWEnginePerformanceCntr()
{
	reset();
}



inline
AWEnginePerformanceCntr::~AWEnginePerformanceCntr()
{
}

//start/stop the count
inline void
AWEnginePerformanceCntr::zeroTime()
{
	m_systemTime.start();
}//inline void AWEnginePerformanceCntr::zeroTime()



inline float
AWEnginePerformanceCntr::getTime()
{
	return m_systemTime.getTime();
}//inline float AWEnginePerformanceCntr::getTime()


inline int		
AWEnginePerformanceCntr::startRender()
{
	return m_renderCnt.start();
}


inline int		
AWEnginePerformanceCntr::stopRender()
{
	return m_renderCnt.stop();
}


inline int		
AWEnginePerformanceCntr::startTransform()
{
	return m_transformCnt.start();
}


inline int		
AWEnginePerformanceCntr::stopTransform()
{
	return m_transformCnt.stop();
}


inline int		
AWEnginePerformanceCntr::startCollide()
{
	return m_collideCnt.start();
}


inline int		
AWEnginePerformanceCntr::stopCollide()
{
	return m_collideCnt.stop();
}

inline void	
AWEnginePerformanceCntr::update(int trisThisFrame)
{
	//updates the overall frame time, inc. max/min values
	NATIVE_TIME totalCnt;
	m_curTriangles = trisThisFrame;
	totalCnt = m_transformCnt.m_diff + m_collideCnt.m_diff+ m_renderCnt.m_diff; 	
	if (totalCnt > (m_maxRenderCnt +m_maxTransformCnt+m_maxCollideCnt)) 	 	
	{ 		
		m_maxRenderCnt    = m_renderCnt.m_diff;
		m_maxTransformCnt = m_transformCnt.m_diff;
		m_maxCollideCnt   = m_collideCnt.m_diff;
		m_maxTimeTriangles = trisThisFrame;
	}
	if (totalCnt < (m_minRenderCnt + m_minTransformCnt+m_minCollideCnt)) 	
	{ 		
		m_minRenderCnt    = m_renderCnt.m_diff;
		m_minTransformCnt = m_transformCnt.m_diff;
		m_minCollideCnt   = m_collideCnt.m_diff;
		m_minTimeTriangles= trisThisFrame;
	}
}//void	AWEnginePerformanceCntr::update(int trisThisFrame)


inline void	
AWEnginePerformanceCntr::reset()	//resets max/min values to initial states
{
	m_renderCnt.reset();
	m_collideCnt.reset();
	m_transformCnt.reset();
   m_minTimeTriangles = m_maxTimeTriangles = m_curTriangles = 0;
	m_minRenderCnt = m_minTransformCnt = m_minCollideCnt = NATIVE_MAX;
	m_maxRenderCnt = m_maxTransformCnt = m_maxCollideCnt = NATIVE_MIN;
}


//get overall results
inline float	
AWEnginePerformanceCntr::getFrequency()
{
	return AWPerformanceCntr::getFrequency(m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline float	
AWEnginePerformanceCntr::getRenderPercent()
{
	return (float)m_renderCnt.m_diff * PERCENT_FAC / (m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline float	
AWEnginePerformanceCntr::getTransformPercent()
{
	return (float)m_transformCnt.m_diff * PERCENT_FAC / (m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline float	
AWEnginePerformanceCntr::getCollidePercent()
{
	return (float)m_collideCnt.m_diff * PERCENT_FAC / (m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}


inline float	
AWEnginePerformanceCntr::getMinFrequency()
{
	return AWPerformanceCntr::getFrequency(m_maxRenderCnt + m_maxTransformCnt + m_maxCollideCnt);
}

inline float	
AWEnginePerformanceCntr::getMaxFrequency()
{
	return AWPerformanceCntr::getFrequency(m_minRenderCnt + m_minTransformCnt + m_minCollideCnt);
}

//get individual results
inline int		
AWEnginePerformanceCntr::getMinRenderPercent()
{
	return (int)(m_minRenderCnt * PERCENT_FAC / (m_minRenderCnt + m_minTransformCnt + m_minCollideCnt));
}


inline int		
AWEnginePerformanceCntr::getMinTransformPercent()
{
	return (int)(m_minTransformCnt * PERCENT_FAC / (m_minRenderCnt + m_minTransformCnt + m_minCollideCnt));
}

inline int		
AWEnginePerformanceCntr::getMinCollidePercent()
{
	return (int)(m_minCollideCnt * PERCENT_FAC / (m_minRenderCnt + m_minTransformCnt + m_minCollideCnt));
}

inline int		
AWEnginePerformanceCntr::getMaxRenderPercent()
{
	return (int)(m_maxRenderCnt * PERCENT_FAC / (m_maxRenderCnt + m_maxTransformCnt + m_maxCollideCnt));
}

inline int		
AWEnginePerformanceCntr::getMaxTransformPercent()
{
	return (int)(m_maxTransformCnt * PERCENT_FAC / (m_maxRenderCnt + m_maxTransformCnt + m_maxCollideCnt));
}

inline int		
AWEnginePerformanceCntr::getMaxCollidePercent()
{
	return (int)(m_maxCollideCnt * PERCENT_FAC / (m_maxRenderCnt + m_maxTransformCnt + m_maxCollideCnt));
}



#endif //sentinel __AWPERFCNT_H


