#ifndef __AW_TRANSFORM_CONTROLLER_H
#define __AW_TRANSFORM_CONTROLLER_H

#include "AWToolsCommon.h"
#include "AWAnimatedQuat.h"
#include "AWAnimatedPoint.h"
#include "AWMatrix4.h"


//NOTES - to allow decent extensibility, should actually define this as a 
//abstract base class with no mwmber data (apart from m_wsTM?)
//so that this basic controller becomes a derived class of this new base.
//
//but for now this is too much hassle for too little gain, and it is uncertain
//how performance might (or might not) be impacted

class AWExport AWTransformController : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWTransformController);

   AWTransformController();
	//copy constructor = calls operator=
	AWTransformController(const AWTransformController& other)
									{*this=other;}
	virtual				~AWTransformController();

	virtual void		compact();

	AWTransformController& operator=(const AWTransformController& other);
	int operator==(const AWTransformController&);

	virtual AWBoolean   isAlive(float time)				{return TRUE;}
	virtual	int			isAnimated();
    virtual AWBoolean   isLooped()                      {return m_loop;}
    virtual void        setIsLooped(AWBoolean v)        {m_loop=v;}
	virtual void		clearAnimation();
    virtual float       getAnimationDuration();

	//state data access methods
	//set/get the individual components of the Transformation state
	void				setPosn(const AWPoint& newPosn) {m_posn.setVal(newPosn);}
	void				getPosn(AWPoint& pos)			{pos=m_posn.getVal();}
	const AWPoint&	getPosn()					   	{return m_posn.getVal();}

    void                setLastUpdate(float t)          {m_lastUpdate=t;}
    float               getLastUpdate()                 {return m_lastUpdate;}

	void				setRotn(const AWQuat& newRotn)	{m_rotn.setQuat(newRotn);}
	const AWQuat&		getRotn()						{return m_rotn.getQuat();}
	void				getRotn(AWQuat& rotn)			{m_rotn.getQuat(rotn);}

	void				setScale(const AWPoint& newScale){m_scale.setVal(newScale);}
	const AWPoint&		getScale()						{return m_scale.getVal();}
	void				getScale(AWPoint& scale)		{scale=m_scale.getVal();}

	void				getWSDirection(AWPoint& dir)	{m_wsTM.getDirection(dir);}
	void				getWSPosition(AWPoint& pos)		{m_wsTM.getTranslation(pos);}
	void				getWSTM(AWMatrix4& mat)			{mat=m_wsTM;}
	const AWMatrix4&	getWSMatrix()					{return m_wsTM;}
	float*				getMatrix()						{return m_wsTM.m_mat;}
	void				setWSTM(const AWMatrix4& tm)	{m_wsTM=tm;}
	virtual int			getViewTM(AWMatrix4& viewTM);

	//act on all keys
	virtual void		rotate(const AWQuat& rot);

	//get/set the Transformation data (only) with a single function call
	void				setTransform(const AWPoint& posn, const AWPoint& scale,
									 const AWQuat& quat)
									{setPosn(posn);setRotn(quat);setScale(scale);}

	virtual	int			getTransform(AWPoint& posn, AWPoint& scale, 
									AWQuat& quat)
									{getPosn(posn);getScale(scale);getRotn(quat);return 1;}

	//return rotational component in Angle/Axis form INCLUDING scale
	virtual	int		getTransform(AWPoint& posn, AWPoint& scale, AWPoint& axis, 
								 float& angle)
					{getPosn(posn);getScale(scale);AWQuat rotn;getRotn(rotn);
					 rotn.toAxisAngle(axis, angle);return 1;}
	//calculate the transformation data for a given time
	virtual int			calcTransform(float t, const AWMatrix4& parentTM);
	//virtual int			calcInverseTransform(float t, const AWMatrix4& parentTM);

	//Key manipulation members
	virtual	int			addRotation(const AWQuat& rot, float time);
	virtual	int			addTranslation(const AWPoint& displace, float time);
	virtual int			addScale(const AWPoint& val, float t);

	//scale all keys belonging to this Transformation Controller
	virtual	void		scale(const AWPoint& scale);
	//move the origin for the transformations
	virtual	void		offset(const AWPoint& offset);
	//pre-process quaternion rotation keys to ensure validity
	virtual	void		preProcessRotationKeys();
   //output key information via AWTRACE
   virtual void      dumpKeys();
	//AWPersistable virtuals
	//int				compareTo(const AWPersistable* o) const;
	//AWBoolean			isEqual(const AWPersistable* o)const;
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

protected:
	//state data
    //time of last update, initially 0, not persisted
    float				m_lastUpdate; 
    //persisted
	AWMatrix4			m_wsTM;
	AWAnimatedPoint		m_posn;
	AWAnimatedPoint		m_scale;
	AWAnimatedQuat		m_rotn;
    AWBoolean           m_loop;
};//class AWTransformController : public AWPersistable


inline void
AWTransformController::compact()
{
	m_posn.compact();
	m_scale.compact();
	m_rotn.compact();
}//void AWTransformController::compact()


inline int
AWTransformController::getViewTM(AWMatrix4& viewTM)
{
	//View Transform is inverse of World Space transform
	m_wsTM.invert(viewTM);
	return 1;
}//int AWTransformController::getViewTM(AWMatrix4& viewTM)

#endif //sentinel __AW_TRANSFORM_CONTROLLER_H