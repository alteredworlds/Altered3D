#include "AWTimerBarCtl.h"

#define VER_AWTimerBarCtl_1 100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWTimerBarController, CLSID_AWTIMERBARCTL, VER_AWTimerBarCtl_1);


AWTimerBarController::AWTimerBarController() : 
            m_timeout(0.0f), m_interval(0.0f)
{
}


AWTimerBarController::~AWTimerBarController()
{
}


int						
AWTimerBarController::calcTransform(double t, const AWMatrix4& parentTM)
{
    double fac = m_timeout - t;
    AWPoint useScale(getScale());
    if ((fac >= 0.0) && (m_timeout > 0.0))
    {
        useScale.x = fac / m_interval;  
    }
    else
    {
        m_timeout = 0.0;
        useScale.x = 0.0f;
    }
    setScale(useScale);
    return AWTransformController::calcTransform(t, parentTM);
}//int AWTimerBarController::calcTransform(double t, const AWMatrix4& parentTM)





//AWPersistable virtuals
//int				compareTo(const AWPersistable* o) const;
//AWBoolean			isEqual(const AWPersistable* o)const;

void				
AWTimerBarController::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWTransformController::restoreGuts(file);	
}//void AWTimerBarController::restoreGuts(AWFile& file)



void        		
AWTimerBarController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
}//void AWTimerBarController::saveGuts(AWFile& file) const



AWspace     		
AWTimerBarController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
					 AWTransformController::binaryStoreSize();
	return retVal;
}//AWspace AWTimerBarController::binaryStoreSize() const

