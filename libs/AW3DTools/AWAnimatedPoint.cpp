#include "AWAnimatedPoint.h"

#define VER_AWAnimatedPoint_1	100
#define VER_AWAnimatedPoint_2	104


AWDEFINE_VERSIONABLE_PERSISTABLE(AWAnimatedPoint, CLSID_AWANIMATEDPOINT, VER_AWAnimatedPoint_2);


//MEMBERS for AWAnimatedPoint
AWAnimatedPoint::AWAnimatedPoint() 
	: m_point(1.0f, 1.0f, 1.0f)
{
	
}//AWAnimatedPoint::AWAnimatedPoint() 



AWAnimatedPoint::AWAnimatedPoint(const AWPoint& point) 
	: m_point(point)
{
}



int 
AWAnimatedPoint::operator==(const AWAnimatedPoint& other)const
{
	return (m_point == other.m_point) &&
				//position animation keys, if any
				//will the rwcollection have a deep ==?
				//don't give a shit for now...
				(m_pointList == other.m_pointList);
}//int AWAnimatedPoint::operator==(const AWAnimatedPoint&)



AWAnimatedPoint& 
AWAnimatedPoint::operator=(const AWAnimatedPoint& other)
{
	m_point=other.m_point;
	//clean up any existing animation data
	clearAnimation();
	//deep copy of all animation keys
	if (other.isAnimated())
	{
		for (int key=0; key<other.m_pointList.getNum(); key++)
		{
			addKey( (AWPositionKey*)other.m_pointList[key]->copy() );
		}
	}	
	return *this;
}//AWAnimatedPoint& AWAnimatedPoint::operator=(const AWAnimatedPoint& other)



int					
AWAnimatedPoint::isAnimated() const
{
	return m_pointList.getNum();
}



void							
AWAnimatedPoint::clearAnimation()
{
	m_pointList.destroy();
}//void	AWAnimatedPoint::clearAnimation()



AWAnimatedPoint::~AWAnimatedPoint()
{
	clearAnimation();
}//AWAnimatedPoint::~AWAnimatedPoint()



int 
AWAnimatedPoint::addKey(AWPositionKey* key)
{
	return m_pointList.add(key);
}//int AWAnimatedPoint::addKey(AWPositionKey* key)



float       
AWAnimatedPoint::getAnimationDuration()const
{
   float retVal = 0.0f;
   int num = m_pointList.getNum();
   if (num)
   {
      retVal = ((AWPositionKey*)m_pointList[num-1])->m_t;
   }
   return retVal;
}//float  AWAnimatedPoint::getAnimationDuration()



int				
AWAnimatedPoint::linearInterp(AWPoint& val, float t, AWBoolean loop)
{
   int retVal = 0;
   int num = m_pointList.getNum();
   // Search upward in given scalar key list
   if (num)
   {
      retVal = 1;
      if (loop)
      {
         float endTime = ((AWPositionKey*)m_pointList[num-1])->m_t;
         if (endTime) t = (float)fmod(t, endTime);
      }
		// Step through position list looking for pair of keys that
		// bracket this t value (or an exact match)
		AWPositionKey* pAfter  = NULL;
		AWPositionKey* pBefore = (AWPositionKey*)m_pointList[0];
		if (pBefore->m_t > t)
		{
			;
		}//if (pBefore->m_t > t)
		else
		{
			int i = 1;
			while (i < num) 
			{
				pAfter = (AWPositionKey*)m_pointList[i++];
				if ((pBefore->m_t <= t) && (pAfter->m_t >= t)) 
				{
					break;
				}
				pBefore = pAfter;
			}//while (i < m_pointList.getNum())
			//compute interpolated values
			float dt = pBefore->m_t;;
			if (pAfter)
			{
				dt = pAfter->m_t - pBefore->m_t;
			}
			if ((pAfter == NULL) || (dt == 0))
			{
				val = pBefore->m_pos;
			}//if ((pAfter == NULL) || (dt == 0))
			else
			{
				float r = (t - pBefore->m_t) / dt;
				val = pBefore->m_pos + r * (pAfter->m_pos - pBefore->m_pos);
			}//else
		}//else		
	}//if (m_pointList)
	return retVal;
}//int	AWAnimatedPoint::linearInterp(AWPoint& val, float t, AWBoolean loop)



void							
AWAnimatedPoint::calc(float t, AWBoolean loop)
{
	AWPoint posn;
	if (linearInterp(posn, t, loop))
	{
		//could lock here, after the calculation has finished
		//just while data is being transferred
		m_point=posn;
	}
}//void AWAnimatedPoint::calc(float t, AWBoolean loop)



void  
AWAnimatedPoint::scale(const AWPoint& scale)
{	
	for (int i = 0; i < m_pointList.getNum(); i++)
	{
		(*(AWPositionKey*)m_pointList[i]) *= scale;
	}//for (int i = 0; i < m_pointList.getNum(); i++)
	m_point *= scale;
}//void AWAnimatedPoint::scale(const AWPoint& scale)



void 
AWAnimatedPoint::offset(const AWPoint& offset)
{
	for (int i = 0; i < (int)m_pointList.getNum(); i++)
	{
		(*(AWPositionKey*)m_pointList[i]) += offset;
	}//for (int i = 0; i < m_pointList.getNum(); i++)
	m_point += offset;
}//void AWAnimatedPoint::offsetWorld(float xTrans, float yTrans, float zTrans)



int 
AWAnimatedPoint::addKey(const AWPoint& displace, float time)
{
	return m_pointList.add(new AWPositionKey(displace, time));
}//int AWAnimatedPoint::addKey(const AWPoint& displace, float time)



void		 
AWAnimatedPoint::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	m_point.restoreGuts(file);
   if (version < VER_AWAnimatedPoint_2)
   {  //SPECIAL CASE- we know that (for files < VER_AWAnimatedPoint_2) this
      //collection may: (i) not be present at all
      //(ii) may be present, but will not be shared with any other
      //object - ie. will NOT have been read before
      m_pointList.readUniquePtr(file);
   }
   else
   {  //no longer stored via Pointer
      file >> m_pointList;
   }
}//void AWAnimatedPoint::restoreGuts(AWFile& file)



void         
AWAnimatedPoint::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	m_point.saveGuts(file);
	file << m_pointList;
}



AWspace      
AWAnimatedPoint::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		sizeof(m_point) +
		m_pointList.getStoreSize();
	return retVal;
}