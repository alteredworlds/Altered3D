#ifndef __AWPERFCNT_H
#define __AWPERFCNT_H

#include "AWToolsCommon.h"

#define NATIVE_TIME double
#define NATIVE_MIN  -999999999.0
#define NATIVE_MAX  999999999.0
#define PERCENT_FAC 100.0

#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#include <QuartzCore/QuartzCore.h>
#endif

class AWExport AWPerformanceCntr
{
public:
	AWPerformanceCntr();
	AWPerformanceCntr(const AWPerformanceCntr& other)	{*this=other;}	
	AWPerformanceCntr& operator=(const AWPerformanceCntr& other);
	~AWPerformanceCntr();

//start/stop the count
	void		start();
	double	   getTime();
	void		stop();
//reset the max/min values
	void	   reset();
	
	static double doublePrecisionSeconds();

	static double getFrequency(NATIVE_TIME diff);

	double	getFrequency();
	double	getMinFrequency();
	double	getMaxFrequency();


//data members
	static LARGE_INTEGER m_cntrFrequency;
#ifndef _WIN32
	static struct timezone m_tz;
#endif

public:
	LARGE_INTEGER   m_cTime;
	NATIVE_TIME		m_minDiff, m_maxDiff, m_diff;
	//
	double m_startTime;
};//class AWPerformanceCntr


class AWExport AWEnginePerformanceCntr
{
public:
	AWEnginePerformanceCntr();
	~AWEnginePerformanceCntr();

//start/stop the count
	void	zeroTime();
	void		startRender();
	void		stopRender();

	void		startTransform();
	void		stopTransform();

	void		startCollide();
	void		stopCollide();

	void	update(int trisThisFrame);	//updates the overall frame time, inc. max/min values
	void	reset();	//resets max/min values to initial states

	double	getTime();

//get overall results
	double	getFrequency();
	double	getRenderPercent();
	double	getTransformPercent();
	double	getCollidePercent();

	double	getMinFrequency();
	double	getMaxFrequency();
	
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
AWPerformanceCntr::AWPerformanceCntr() 
{
	reset();
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
	m_startTime = other.m_startTime;
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
	m_startTime = 0.0;
}


//start/stop the count
inline void		
AWPerformanceCntr::start()
{
	m_startTime = doublePrecisionSeconds();
}


inline double
AWPerformanceCntr::getTime()
{
	return doublePrecisionSeconds() - m_startTime;
}//inline float& AWPerformanceCntr::getTime()


inline void		
AWPerformanceCntr::stop()
{
	m_diff = getTime();
	if (m_diff < m_minDiff)
		m_minDiff = m_diff;
	else if (m_diff > m_maxDiff)
		m_maxDiff = m_diff;
}//int AWPerformanceCntr::stop()


inline double
AWPerformanceCntr::getFrequency(NATIVE_TIME diff)
{
	if (diff)
	{
		return 1.0 / diff;
	}
	else return 0.0;
}


inline double
AWPerformanceCntr::getMinFrequency()
{
	return getFrequency(m_maxDiff);
}


inline double
AWPerformanceCntr::getMaxFrequency()
{
	return getFrequency(m_minDiff);
}


inline double	
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


inline double
AWEnginePerformanceCntr::getTime()
{
	return m_systemTime.getTime();
}//inline float AWEnginePerformanceCntr::getTime()


inline void		
AWEnginePerformanceCntr::startRender()
{
	m_renderCnt.start();
}


inline void		
AWEnginePerformanceCntr::stopRender()
{
	m_renderCnt.stop();
}


inline void		
AWEnginePerformanceCntr::startTransform()
{
	m_transformCnt.start();
}


inline void		
AWEnginePerformanceCntr::stopTransform()
{
	m_transformCnt.stop();
}


inline void		
AWEnginePerformanceCntr::startCollide()
{
	m_collideCnt.start();
}


inline void		
AWEnginePerformanceCntr::stopCollide()
{
	m_collideCnt.stop();
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
inline double	
AWEnginePerformanceCntr::getFrequency()
{
	return AWPerformanceCntr::getFrequency(m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline double	
AWEnginePerformanceCntr::getRenderPercent()
{
	return (double)m_renderCnt.m_diff * PERCENT_FAC / (m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline double	
AWEnginePerformanceCntr::getTransformPercent()
{
	return (double)m_transformCnt.m_diff * PERCENT_FAC / (m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline double	
AWEnginePerformanceCntr::getCollidePercent()
{
	return (double)m_collideCnt.m_diff * PERCENT_FAC / (m_transformCnt.m_diff + m_collideCnt.m_diff + m_renderCnt.m_diff);
}

inline double	
AWEnginePerformanceCntr::getMinFrequency()
{
	return AWPerformanceCntr::getFrequency(m_maxRenderCnt + m_maxTransformCnt + m_maxCollideCnt);
}

inline double	
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


