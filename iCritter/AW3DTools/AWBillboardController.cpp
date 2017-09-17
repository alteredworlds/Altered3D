#include "AWBillboardController.h"

#define VER_AWBillboardController_1 100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWBillboardController, CLSID_AWBILLBOARDCONTROLLER, VER_AWBillboardController_1);


AWBillboardController::AWBillboardController() : m_pointSymmetry(FALSE)
{
}


AWBillboardController::~AWBillboardController()
{
}


int						
AWBillboardController::calcTransform(double t, const AWMatrix4& parentTM)
{	
	m_wsTM.identity();
	m_wsTM.awTranslate(m_posn.getVal());
	if (m_pointSymmetry)
	{	//rotates object about its center	
		AWMatrix4 temp(parentTM);
		temp.setTranslation(AWPoint(0,0,0));
		m_wsTM = m_wsTM * temp;
	}
	else
	{	//rotates the object around the y axis so as to always
		//appear face-on to the camera
		//works in all quadrants
		float angle = (float)(180.0*atan2f(parentTM[8], parentTM[10])/M_PI);
		m_wsTM.awRotateY(angle);
	}
	m_wsTM.awRotate(m_rotn.getQuat());
	m_wsTM.awScale(m_scale.getVal());
	return 1;
}//int AWBillboardController::calcTransform(double t, const AWMatrix4& parentTM)





//AWPersistable virtuals
void				
AWBillboardController::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWTransformController::restoreGuts(file);	
	file >> m_pointSymmetry;
}//void AWBillboardController::restoreGuts(AWFile& file)



void        		
AWBillboardController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
	file << m_pointSymmetry;
}//void AWBillboardController::saveGuts(AWFile& file) const



AWspace     		
AWBillboardController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
					 AWTransformController::binaryStoreSize()+
					 sizeof(m_pointSymmetry);
	return retVal;
}//AWspace AWBillboardController::binaryStoreSize() const