#ifndef __AWSOUND_H
#define __AWSOUND_H

#include "AWSndCommon.h"

#ifdef _WIN32
#include <mmsystem.h>
#endif


class  AWSound
{
public:
	AWSound();

	AWSound(const AWSound& other) {*this=other;}
	int		 operator==(const AWSound& other)const;
	AWSound& operator=(const AWSound& other);


	AWCString	m_name;
	
	int			m_ID;
   int         m_channelID;

   AWVector    m_pos3;

    void*      m_pSnd;
};//class AWSound


inline
AWSound::AWSound() : m_pSnd(NULL), m_ID(-1), m_channelID(-1)
{
    m_pos3.x = m_pos3.y = m_pos3.z = 0.0f;
}



inline int		
AWSound::operator==(const AWSound& other)const
{
	return ((m_name == other.m_name)
		    && (m_pSnd == other.m_pSnd)
			 && (m_ID   == other.m_ID));
}


inline AWSound&
AWSound::operator=(const AWSound& other)
{
   m_name		     = other.m_name;
   m_pSnd		     = other.m_pSnd;
   m_ID		       = other.m_ID;
   m_channelID   = other.m_channelID;
   m_pos3.x      = other.m_pos3.x;
   m_pos3.y      = other.m_pos3.y;
   m_pos3.z      = other.m_pos3.z;
	return *this;
}

#endif //sentinel __AWSOUND_H


