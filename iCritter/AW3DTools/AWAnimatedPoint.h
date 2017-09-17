#ifndef __AW_ANIMATED_POINT_H
#define __AW_ANIMATED_POINT_H

#include "AWToolsCommon.h"
#include "AWOrdered.h"
#include "AWPositionKey.h"



class AWExport AWAnimatedPoint : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWAnimatedPoint);

   AWAnimatedPoint();
   AWAnimatedPoint(const AWPoint& point);
   virtual				~AWAnimatedPoint();
   virtual void		compact();

   int operator==(const AWAnimatedPoint&)const;
   AWAnimatedPoint& operator=(const AWAnimatedPoint& other);
   //there should be a base class for this stuff, also 'AWKey'
   virtual	int		isAnimated()const;
   virtual void		clearAnimation();
   virtual float     getAnimationDuration()const;
   int					addKey(AWPositionKey* key);

   //state data access methods
   void				   setVal(const AWPoint& posn)	{m_point=posn;}
   void				   getVal(AWPoint& posn )		{posn=m_point;}
   const AWPoint&		getVal()						{return m_point;}

   //calculate the transformation data for a given time
   virtual void		calc(float t, AWBoolean loop=TRUE);

	//Key manipulation members
	virtual	int		addKey(const AWPoint& point, float time);

	//scale all keys belonging to this AWAnimatedPoint
	virtual	void		scale(const AWPoint& scale);
	//move the origin for the AWAnimatedPoint
	virtual	void		offset(const AWPoint& offset);
	
	//dumps out via AWTRACE
   virtual	void     dumpKeys();

	//AWPersistable virtuals
	virtual void		restoreGuts(AWFile& file);
    virtual void     saveGuts(AWFile& file) const;
	virtual AWspace   binaryStoreSize() const;

protected:
	//find value using linear interpolation
	int					linearInterp(AWPoint& val, float t, AWBoolean loop);
	//state data
	AWPoint				m_point;
	//position animation keys, if any
	AWOrdered	      m_pointList;
};//class AWAnimatedPoint : public AWPersistable


inline void
AWAnimatedPoint::compact()
{
	m_pointList.pack();
}//inline void AWAnimatedPoint::compact()

#endif //sentinel __AW_ANIMATED_POINT_H