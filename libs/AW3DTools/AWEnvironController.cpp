#include "AWEnvironController.h"

#define VER_AWEnvironController_1 100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWEnvironController, CLSID_AWENVIRONCONTROLLER, VER_AWEnvironController_1);


AWEnvironController::AWEnvironController()
{
}


AWEnvironController::~AWEnvironController()
{
}


int						
AWEnvironController::calcTransform(float t, const AWMatrix4& parentTM)
{
	//parentTM will be the wsTM of player
	//need to remove any rotational component (environment is fixed
	//relative to world, so player's rotn state doesn't affect it)
	//and also remove any vertical movement ? - so movement in y
	//but we want to keep the x-z movement, as this ensures the 
	//player doesn't get closer to the environment
	//
	//the localTM data will be constant, representing the initial
	//orientation of the environment w.r.t. the world in MAX
	AWPoint parentTranslation;

	m_wsTM.identity();
	parentTM.getTranslation(parentTranslation);
	//parentTranslation.y=0;
	m_wsTM.setTranslation(parentTranslation);
	//now update with local TM information
	m_wsTM.awTranslate(m_posn.getVal());
	m_wsTM.awRotate(m_rotn.getQuat());
	m_wsTM.awScale(m_scale.getVal());
	
	return 1;
}//int AWEnvironController::calcTransform(float t, const AWMatrix4& parentTM)



//AWPersistable virtuals
void				
AWEnvironController::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWTransformController::restoreGuts(file);	
}//void AWEnvironController::restoreGuts(AWFile& file)



void        		
AWEnvironController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
}//void AWEnvironController::saveGuts(AWFile& file) const



AWspace     		
AWEnvironController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
					 AWTransformController::binaryStoreSize();
	return retVal;
}//AWspace AWEnvironController::binaryStoreSize() const