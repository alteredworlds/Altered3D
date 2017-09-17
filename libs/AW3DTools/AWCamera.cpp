#include "AWCamera.h"
#include "AWFlightController.h"


#define VER_AWCAMERA_1	100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWCamera, CLSID_AWCAMERA, VER_AWCAMERA_1);



AWCamera::~AWCamera()
{	
}//AWCamera::~AWCamera()



void		
AWCamera::scale(const AWPoint& scale)
{
	AWNode::scale(scale);
	//m_fov //?
	m_nearClip *= scale.z;
	if(m_nearClip < 1.0f)
	{
		m_nearClip = 1.0f;
	}
	m_farClip  *= scale.z;
}


AWCamera&			
AWCamera::clone(const AWCamera& other)
{
	AWNode::clone(other);
	m_fov		= other.m_fov;		//Field of View in Degrees
	m_nearClip	= other.m_nearClip;	//far clipping distance
	m_farClip	= other.m_farClip;	//near clipping distance
	return *this;
}


AWNode*		
AWCamera::clone() const 
{
	AWCamera* cam=new AWCamera;
	cam->clone(*this);
	return cam;
} 


void        
AWCamera::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWNode::restoreGuts(file);
	file.Read(m_fov);
	file.Read(m_nearClip);
	file.Read(m_farClip);
}//void AWCamera::saveGuts(AWFile& file) const



void        
AWCamera::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWNode::saveGuts(file);
	file.Write(m_fov);
	file.Write(m_nearClip);
	file.Write(m_farClip);
}//void AWCamera::saveGuts(AWFile& file) const



AWspace     
AWCamera::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWNode::binaryStoreSize() +
		sizeof (m_fov) +	
		sizeof (m_nearClip) +
		sizeof (m_farClip);
	return retVal;
}//AWspace  AWCamera::binaryStoreSize() const
