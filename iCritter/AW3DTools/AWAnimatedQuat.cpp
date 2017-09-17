#include "AWAnimatedQuat.h"

#define VER_AWAnimatedQuat_1	100
#define VER_AWAnimatedQuat_2	104


AWDEFINE_VERSIONABLE_PERSISTABLE(AWAnimatedQuat,  CLSID_AWANIMATEDQUAT, VER_AWAnimatedQuat_2);


//MEMBERS for AWAnimatedQuat

AWAnimatedQuat::AWAnimatedQuat()
{
	
}//AWAnimatedQuat::AWAnimatedQuat() 



AWAnimatedQuat::AWAnimatedQuat(const AWQuat& rotn) 
	: m_quat(rotn)
{
}


int 
AWAnimatedQuat::operator==(const AWAnimatedQuat& other)const
{
	return (m_quat == other.m_quat) &&
				//rotation animation keys, if any
				//will the rwcollection have a deep ==?
				//don't give a shit for now...
				(m_quatList == other.m_quatList);
}//int AWAnimatedQuat::operator==(const AWAnimatedQuat&)



AWAnimatedQuat& 
AWAnimatedQuat::operator=(const AWAnimatedQuat& other)
{
	m_quat=other.m_quat;
	//clean up any existing animation data
	clearAnimation();
	//deep copy of all animation keys
	if (other.isAnimated())
	{
		for (int key=0; key<(int)other.m_quatList.getNum(); key++)
		{
			addKey( (AWRotationKey*)other.m_quatList[key]->copy() );
		}
	}	
	return *this;
}//AWAnimatedQuat& AWAnimatedQuat::operator=(const AWAnimatedQuat& other)



int					
AWAnimatedQuat::isAnimated()const
{
	return m_quatList.getNum();
}


void							
AWAnimatedQuat::clearAnimation()
{
	m_quatList.destroy();
}//void	AWAnimatedQuat::clearAnimation()




AWAnimatedQuat::~AWAnimatedQuat()
{
	clearAnimation();
}//AWAnimatedQuat::~AWAnimatedQuat()



float       
AWAnimatedQuat::getAnimationDuration()const
{
   float retVal = 0.0f;
   int num = m_quatList.getNum();
   if (num)
   {
      retVal = ((AWRotationKey*)m_quatList[num-1])->m_t;
   }
   return retVal;
}//float  AWAnimatedQuat::getAnimationDuration()



int 
AWAnimatedQuat::slerpInterpolate(AWQuat& val, float t, AWBoolean loop)
{
	int retVal = 0;
   // Search upward in rotation keys list
   int num =  m_quatList.getNum();
   if (num)
   {
      retVal = 1;
      if (loop)
      {
         float endTime = ((AWRotationKey*)m_quatList[num-1])->m_t;
         if (endTime) t = (float)fmod(t, endTime);
      }
      // Step through position list looking for pair of keys that
      // bracket this t value (or an exact match)
      AWRotationKey* pAfter  = NULL;
      AWRotationKey* pBefore = (AWRotationKey*)m_quatList[0];
		if (pBefore->m_t > t)
		{
			;
		}//if (pBefore->t > t)
		else
		{
			int i = 1;
			while (i < num) 
			{
				pAfter = (AWRotationKey*)m_quatList[i++];
				if ((pBefore->m_t <= t) && (pAfter->m_t >= t)) 
				{
					break;
				}
				pBefore = pAfter;
			}//while (i < num) 
			//compute interpolated values
			float dt = pBefore->m_t;
			if (pAfter)
			{
				dt = pAfter->m_t - pBefore->m_t;
			}
			if ((pAfter == NULL) || (dt == 0))
			{
				val = pBefore->m_quat;
			}//if ((pAfter == NULL) || (dt == 0))
			else
			{
				//we have two keys bounding this value, can
				//slerp between them...
				val = slerp((t - pBefore->m_t) / dt, pBefore->m_quat, pAfter->m_quat);
			}//else	
		}//else
	}//if (num)

	return retVal;
}//int AWAnimatedQuat::slerpInterpolate(AWQuat& val, float t, AWBoolean loop)




void							
AWAnimatedQuat::calcQuat(float t, AWBoolean loop)
{
   AWQuat rotn;
   if (slerpInterpolate(rotn, t, loop))
   {
      //could lock here, after the calculation has finished
      //just while data is being transferred
      m_quat=rotn;
   }
}//void AWAnimatedQuat::calcQuat(float t, AWBoolean loop)


int 
AWAnimatedQuat::addKey(AWRotationKey* key)
{
	return m_quatList.add(key);
}//int AWAnimatedQuat::addKey(AWRotationKey* key)



int 
AWAnimatedQuat::addQuat(const AWQuat& rotn, float time)
{
	return addKey(new AWRotationKey(rotn, time));
}//int AWAnimatedQuat::addQuat(const AWQuat& rotn, float time)


void 
AWAnimatedQuat::rotate(const AWQuat& q)
{
	m_quat = m_quat * q;	
	AWRotationKey* key  = NULL;
	for (int i = 0; i < m_quatList.getNum(); i++)
	{
		key = (AWRotationKey*)m_quatList[i];
		key->m_quat = key->m_quat * q;
		
	}//for (int i = 1; i < m_quatList < entries(); i++)
}//void AWAnimatedQuat::rotate(const AWQuat& q)



void
AWAnimatedQuat::dumpKeys()
{
   if (m_quatList.getNum())
   {
      AWRotationKey* key = NULL;
      AWQuat         q;
      AWTRACE1("AWAnimatedQuat::dumpKeys() - %i keys\n", (int)m_quatList.getNum());
      for (int i=0; i<m_quatList.getNum(); i++)
      {
         key  = (AWRotationKey*)m_quatList[i];
         if (key)
         {
            q = key->m_quat;
            AWTRACE4("Processed Quat(w,x,y,z) (%f, %f, %f, %f) ", 
                     (double)q.w, (double)q.x, (double)q.y, (double)q.z);
            AWTRACE1(" at %f\n", (double)key->m_t);
         }//if (key)
      }//for (int i=0; i<m_quatList->entries(); i++)
   }//if (m_quatList.getNum())
}//void AWAnimatedQuat::dumpKeys()



void 
AWAnimatedQuat::preProcessRotationKeys()
{	// arrange for consecutive quaternions to have acute angle
    //if ( dot(q1,q2) < 0.0 )
    //    q2 = -q2;
	//m_rotn.preProcessRotationKeys();
   AWRotationKey* first    =  NULL;
	AWRotationKey* second   =  NULL;
	if (m_quatList.getNum() > 1)
	{  
      for (int i=0; i<(m_quatList.getNum()-1); i++ )
      {
         first  = (AWRotationKey*)m_quatList[i]; 
         second = (AWRotationKey*)m_quatList[i+1];
         if (dot(first->m_quat, second->m_quat) < 0.0f)
         {
            second->m_quat = -second->m_quat;
         }
      }
	}//if (m_quatList.getNum() > 1)
}//void AWAnimatedQuat::preProcessRotationKeys()



//AWPersistable virtuals
//int				compareTo(const AWPersistable* o) const;
//AWBoolean			isEqual(const AWPersistable* o)const;
void		 
AWAnimatedQuat::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	m_quat.restoreGuts(file);	
   if (version < VER_AWAnimatedQuat_2)
   {  //SPECIAL CASE- we know that (for files < VER_AWAnimatedQuat_2) this
      //collection may: (i) not be present at all
      //(ii) may be present, but will not be shared with any other
      //object - ie. will NOT have been read before
      m_quatList.readUniquePtr(file);
   }
   else
   {  //no longer stored via Pointer
      file >> m_quatList;
   }
}//void AWAnimatedQuat::restoreGuts(AWFile& file)



void         
AWAnimatedQuat::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	m_quat.saveGuts(file);
	file << m_quatList;
}



AWspace      
AWAnimatedQuat::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		sizeof(m_quat) +
		m_quatList.getStoreSize();
	return retVal;
}


