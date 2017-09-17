#include "AWTransformController.h"


#define VER_AWTransformController_1	100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWTransformController,	CLSID_AWTRANSFORMCONTROLER, VER_AWTransformController_1);


//MEMBERS for AWTransformController

AWTransformController::AWTransformController() : 
        m_lastUpdate(0), m_loop(TRUE)
{
	
}//AWTransformController::AWTransformController() 


AWTransformController& 
AWTransformController::operator=(const AWTransformController& other)
{
	//don't bother assigning the wsTM matrix, as this is recalculated
	// each frame from the other data anyway
	m_posn	= other.m_posn;
	m_scale	= other.m_scale;
	m_rotn	= other.m_rotn;
	m_lastUpdate = other.m_lastUpdate;
	m_loop   = other.m_loop;
	return *this;
}//AWTransformController::operator=(const AWTransformController& other)


int  
AWTransformController::operator==(const AWTransformController& other)
{
	//don't bother comparing the matrix, as this is recalculated
	//each frame from the other data anyway
	return  (m_posn	 == other.m_posn) &&
			(m_scale == other.m_scale) &&
			(m_rotn	 == other.m_rotn);
}//int AWTransformController::operator==(const AWTransformController&)


int					
AWTransformController::isAnimated()
{
	return (m_posn.isAnimated() | m_scale.isAnimated() | m_rotn.isAnimated());
}


void							
AWTransformController::clearAnimation()
{
	m_posn.clearAnimation();
	m_scale.clearAnimation();
	m_rotn.clearAnimation();
}//void	AWTransformController::clearAnimation()


float       
AWTransformController::getAnimationDuration()
{ //find the last animation key and return its time value, for now
    float retVal = 0.0f;
    if (m_posn.isAnimated())
    {
        retVal = m_posn.getAnimationDuration();
    }
    if (m_scale.isAnimated() && (retVal < m_scale.getAnimationDuration()))
    {
        retVal = m_scale.getAnimationDuration();
    }
    if (m_rotn.isAnimated() && (retVal < m_rotn.getAnimationDuration()))
    {
        retVal = m_rotn.getAnimationDuration();
    }
    return retVal;
}//float  AWTransformController::getAnimationDuration()


AWTransformController::~AWTransformController()
{
	clearAnimation();
}//AWTransformController::~AWTransformController()


void									
AWTransformController::rotate(const AWQuat& rot)
{
	//apply suplied quaternion to self, including animation keys
	m_rotn.rotate(rot);
}//void	AWTransformController::rotate(const AWQuat& rot)


int							
AWTransformController::calcTransform(double t, const AWMatrix4& parentTM)
{
	//calculate the local transform data for this frame
	m_rotn.calcQuat(t, m_loop);
	m_scale.calc(t, m_loop);
	m_posn.calc(t, m_loop);
	//initial state is parentTM
	m_wsTM=parentTM;
	//now update with local TM information
	m_wsTM.awTranslate(m_posn.getVal());
	m_wsTM.awRotate(m_rotn.getQuat());
	m_wsTM.awScale(m_scale.getVal());	
	return 1;
}//int AWTransformController::calcTransform(double t, const AWMatrix4& parentTM)


void      
AWTransformController::dumpKeys()
{ 
	m_posn.dumpKeys();
	m_rotn.dumpKeys();
}//void AWTransformController::dumpKeys()


void 
AWTransformController::preProcessRotationKeys()
{	// arrange for consecutive quaternions to have acute angle
    //if ( dot(q1,q2) < 0.0 )
    //    q2 = -q2;
	m_rotn.preProcessRotationKeys();
}//void AWTransformController::preProcessRotationKeys()


int			 
AWTransformController::addRotation(const AWQuat& rot, float time)
{
	return m_rotn.addQuat(rot, time);
}


int
AWTransformController::addScale(const AWPoint& val, float t)
{
	return m_scale.addKey(val, t);
}//int AWTransformController::addScale(const AWPoint& val, float t)


int 
AWTransformController::addTranslation(const AWPoint& displace, float time)
{
	return m_posn.addKey(displace, time);
}//int AWTransformController::addTranslation(const AWPoint& displace, float time)


void  
AWTransformController::scale(const AWPoint& scale)
{
	m_posn.scale(scale);
}//void AWTransformController::scale(const AWPoint& scale)


void 
AWTransformController::offset(const AWPoint& offset)
{
	m_posn.offset(offset);
}//void AWTransformController::offsetWorld(float xTrans, float yTrans, float zTrans)


//AWPersistable virtuals
//int				compareTo(const AWPersistable* o) const;
//AWAWBooleanean			isEqual(const AWPersistable* o)const;
void		 
AWTransformController::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	file >> m_posn;
	file >> m_rotn;
	file >> m_scale;
}

void         
AWTransformController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << m_posn;
	file << m_rotn;
	file << m_scale;
}

AWspace      
AWTransformController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		m_posn.binaryStoreSize() +
		m_rotn.binaryStoreSize() +
		m_scale.binaryStoreSize();
	return retVal;
}