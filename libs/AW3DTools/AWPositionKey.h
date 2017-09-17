#ifndef __AWPOSITIONKEY_H
#define __AWPOSITIONKEY_H

#include "AWToolsCommon.h"
#include "AWPoint.h"


class AWExport AWPositionKey : public AWPersistable
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWPositionKey);

   AWPositionKey() : m_t(0.0f)  {;}

   AWPositionKey(float x, float y, float z, float t)
      : m_pos(x, y, z), m_t(t) {;}

   AWPositionKey(const AWPoint& pos, float t)
      : m_pos(pos), m_t(t)	 {;}

   virtual              ~AWPositionKey(){;}

   //AWPersistable virtuals
   virtual void		   restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;
   virtual AWspace      binaryStoreSize() const;

   const AWPositionKey& operator*= (const AWPoint& q);
   const AWPositionKey& operator+= (const AWPoint& q);
   friend AWPositionKey operator* (float c, const AWPositionKey& p);

   //data
   AWPoint              m_pos;
   float                m_t;
};//class AWPositionKey : public AWPersistable



inline const AWPositionKey&
AWPositionKey::operator*= (const AWPoint& q)
{
	m_pos *= q;
	return *this;
}//inline const AWPositionKey& AWPositionKey::operator*= (const AWPoint& q)



inline const AWPositionKey&
AWPositionKey::operator+= (const AWPoint& q)
{
	m_pos += q;
	return *this;
}//inline const AWPositionKey& AWPositionKey::operator+= (const AWPoint& q)


#endif //sentinel __AWPOSITIONKEY_H