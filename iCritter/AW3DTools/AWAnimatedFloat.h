#ifndef __AW_ANIMATED_FLOAT_H
#define __AW_ANIMATED_FLOAT_H

#include "AWToolDefs.h"
#include "AWOrdered.h"


class AWExport AWFloatKey : public AWPersistable
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWFloatKey);

   AWFloatKey(float val=0.0f, float t=0.0f)
      : m_val(val), m_t(t) {;}
   virtual ~AWFloatKey(){;}

   //AWPersistable virtuals
   virtual void		restoreGuts(AWFile& file);
   virtual void      saveGuts(AWFile& file) const;
   virtual AWspace   binaryStoreSize() const;

   //data
   float	            m_val;
   float	            m_t;
};//class AWFloatKey : public AWPersistable



class AWExport AWAnimatedFloat : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWAnimatedFloat);

	enum AnimateMethod {discrete=0, interpolate} m_usage;

	AWAnimatedFloat(float val=0.0f) : m_val(val) 
									{m_usage=interpolate;}
	virtual				~AWAnimatedFloat();

	int operator==(const AWAnimatedFloat&)const;
	AWAnimatedFloat& operator=(const AWAnimatedFloat& other);
	//there should be a base class for this stuff, also 'AWKey'
   virtual void		compact();
   virtual void		setUsage(AnimateMethod how){m_usage=how;}

   virtual	int		isAnimated()const;
   virtual void		clearAnimation();
   virtual float     getAnimationDuration()const;
   int					addKey(AWFloatKey* key);

	//state data access methods
	void				   setVal(float val)		{m_val=val;}
	float				   getVal()				{return m_val;}

	//calculate the transformation data for a given time
	virtual void		calc(float t, AWBoolean loop=TRUE);

	//Key manipulation members
	virtual	int		addKey(float val, float time);

   //AWPersistable virtuals
   //int				   compareTo(const AWPersistable* o) const;
   //float			   isEqual(const AWPersistable* o)const;
   virtual void		restoreGuts(AWFile& file);
   virtual void      saveGuts(AWFile& file) const;
   virtual AWspace   binaryStoreSize() const;

protected:
	//no interpolation for booleans!
	//state data
	float			      m_val;
	//position animation keys, if any
	AWOrdered	      m_valList;
};//class AWAnimatedFloat : public AWPersistable


inline void
AWAnimatedFloat::compact()
{
	m_valList.pack();
}//void AWAnimatedFloat::compact()


#endif //sentinel __AW_ANIMATED_FLOAT_H