#ifndef __AW_ANIMATED_QUAT_H
#define __AW_ANIMATED_QUAT_H

#include "AWToolDefs.h"
#include "AWOrdered.h"
#include "AWRotationKey.h"


class AWExport AWAnimatedQuat : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWAnimatedQuat);

   AWAnimatedQuat();
   AWAnimatedQuat(const AWQuat& point);
   virtual				~AWAnimatedQuat();

   int operator==(const AWAnimatedQuat& other)const;
   AWAnimatedQuat& operator=(const AWAnimatedQuat& other);

   virtual void		compact();

   virtual	int		isAnimated()const;
   virtual void		clearAnimation();
   virtual float     getAnimationDuration()const;

	//state data access methods
	void				   setQuat(const AWQuat& rotn){m_quat=rotn;}
	void				   getQuat(AWQuat& rotn )		{rotn=m_quat;}
	const AWQuat&		getQuat()					   {return m_quat;}

   //calculate the transformation data for a given time
   virtual void		calcQuat(float t, AWBoolean loop=TRUE);

   virtual void		rotate(const AWQuat& q);

   //Key manipulation members
   virtual	int		addQuat(const AWQuat& point, float time);
   int					addKey(AWRotationKey* key);

   //pre-process quaternion rotation keys to ensure validity
   virtual	void		preProcessRotationKeys();
   //dumps out via AWTRACE
   virtual	void     dumpKeys();

   //AWPersistable virtuals
   virtual void		restoreGuts(AWFile& file);
   virtual void      saveGuts(AWFile& file) const;
   virtual AWspace   binaryStoreSize() const;

protected:
	//find value using slerp interpolation
	int					slerpInterpolate(AWQuat& val, float t, AWBoolean loop);
	//state data
	AWQuat				m_quat;
	//animation keys, if any
   AWOrdered         m_quatList;
};//class AWAnimatedQuat : public AWPersistable


inline void
AWAnimatedQuat::compact()
{
   m_quatList.pack();
}//inline void AWAnimatedQuat::compact()


#endif //sentinel __AW_ANIMATED_QUAT_H