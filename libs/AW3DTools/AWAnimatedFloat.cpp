#include "AWAnimatedFloat.h"
#include <math.h>


#define VER_AWFloatKey_1			100
#define VER_AWAnimatedFloat_1	   100
#define VER_AWAnimatedFloat_2	   104


AWDEFINE_VERSIONABLE_PERSISTABLE(AWAnimatedFloat, CLSID_AWANIMATEDFLOAT, VER_AWAnimatedFloat_2);
AWDEFINE_VERSIONABLE_PERSISTABLE(AWFloatKey,      CLSID_AWFLOATKEY,      VER_AWFloatKey_1);



//PERSISTENCE AWPersistable virtuals
void							
AWFloatKey::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	file.Read(m_val);
	file.Read(m_t);
}//void	AWFloatKey::restoreGuts(AWFile& file)


void        					
AWFloatKey::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file.Write(m_val);
	file.Write(m_t);
}//void AWFloatKey::saveGuts(AWFile& file) const



AWspace     					
AWFloatKey::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		sizeof (m_val) +
		sizeof (m_t);
	return retVal;
}//AWspace AWFloatKey::binaryStoreSize() const



//MEMBERS for AWAnimatedFloat


int 
AWAnimatedFloat::operator==(const AWAnimatedFloat& other)const
{
	return (m_val == other.m_val) &&
				//position animation keys, if any
				//will the rwcollection have a deep ==?
				//don't give a shit for now...
				(m_valList == other.m_valList);
}//int AWAnimatedFloat::operator==(const AWAnimatedFloat&)



AWAnimatedFloat& 
AWAnimatedFloat::operator=(const AWAnimatedFloat& other)
{
	m_val=other.m_val;
	//clean up any existing animation data
	clearAnimation();
   int num = other.m_valList.getNum();
	//deep copy of all animation keys
	if (num)
	{
		for (int key=0; key<num; key++)
		{
			addKey( (AWFloatKey*)other.m_valList[key]->copy() );
		}
	}	
	return *this;
}//AWAnimatedFloat& AWAnimatedFloat::operator=(const AWAnimatedFloat& other)



int					
AWAnimatedFloat::isAnimated() const
{
	return m_valList.getNum();
}


void							
AWAnimatedFloat::clearAnimation()
{
   m_valList.destroy();
}//void	AWAnimatedFloat::clearAnimation()




AWAnimatedFloat::~AWAnimatedFloat()
{
	clearAnimation();
}//AWAnimatedFloat::~AWAnimatedFloat()


int 
AWAnimatedFloat::addKey(AWFloatKey* key)
{
	return m_valList.add(key);
}//int AWAnimatedFloat::addKey(AWFloatKey* key)



float       
AWAnimatedFloat::getAnimationDuration()const
{
   float retVal = 0.0f;
   int num = m_valList.getNum();
   if (num)
   {
      retVal = ((AWFloatKey*)m_valList[num-1])->m_t;
   }
   return retVal;
}//float  AWAnimatedFloat::getAnimationDuration()



void							
AWAnimatedFloat::calc(float t, AWBoolean loop)
{
   int num = m_valList.getNum();
	if (num)
	{
      if (loop)
      {
         float endTime = ((AWFloatKey*)m_valList[num-1])->m_t;
         if (endTime) t = (float)fmod(t, endTime);
      }        
      // Step through position list looking for pair of keys that
      // bracket this t value (or an exact match)
      AWFloatKey* pAfter  = NULL;
      AWFloatKey* pBefore = (AWFloatKey*)m_valList[0];
      if (pBefore->m_t > t)
      {
         ;
      }//if (pBefore->m_t > t)
		else
		{
         int i = 1;
         while (i < num) 
         {
            pAfter = (AWFloatKey*)m_valList[i++];
            if ((pBefore->m_t <= t) && (pAfter->m_t >= t)) 
            {
               break;
            }
            pBefore = pAfter;
         }//while (i < m_valList->entries())
         if (discrete == m_usage)
         {
            m_val = pBefore->m_val;
         }
			else
			{
            //compute interpolated values
            float dt = pBefore->m_t;
            if (pAfter)
            {
               dt = pAfter->m_t - pBefore->m_t;
            }
            if ((pAfter == NULL) || (dt == 0))
            {
               m_val = pBefore->m_val;
            }//if ((pAfter == NULL) || (dt == 0))
            else
            {
               float r = (t - pBefore->m_t) / dt;
               m_val = pBefore->m_val + r * (pAfter->m_val - pBefore->m_val);
            }//else
			}
		}//else		
	}//if (num)
}//void AWAnimatedFloat::calc(float t)



int 
AWAnimatedFloat::addKey(float val, float time)
{
	return m_valList.add(new AWFloatKey(val, time));
}//int AWAnimatedFloat::addKey(float val, float time)



//AWPersistable virtuals
void		 
AWAnimatedFloat::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	file >> m_val;
   if (version < VER_AWAnimatedFloat_2)
   {  //SPECIAL CASE- we know that (for files < VER_AWAnimatedFloat_2) this
      //collection may: (i) not be present at all
      //(ii) may be present, but will not be shared with any other
      //object - ie. will NOT have been read before
      m_valList.readUniquePtr(file);
   }
   else
   {  //no longer stored via Pointer
      file >> m_valList;
   }
}//void AWAnimatedFloat::restoreGuts(AWFile& file)



void         
AWAnimatedFloat::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << m_val;
	file << m_valList;
}



AWspace      
AWAnimatedFloat::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		sizeof(m_val) +
		m_valList.getStoreSize();
	return retVal;
}


