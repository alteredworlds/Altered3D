#ifndef __AWCAMERA_H
#define __AWCAMERA_H


#include "AWNode.h"

class AWExport AWCamera : public AWNode
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWCamera);

	AWCamera(const AWCString& name = "", AWNode* parent=NULL,
		   float fov = 45.0f, float nearClip = 3.0f, float farClip = 30.0f)
		: AWNode(name, parent), m_fov(fov), m_nearClip(nearClip), m_farClip(farClip) {;}
	virtual		~AWCamera();

	AWCamera&			clone(const AWCamera& other);
	virtual AWNode*		clone() const;

   virtual void		getViewCone(AWSolidCone& viewCone);
   virtual void		getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM);

	void		scale(const AWPoint& scale);
	virtual void restoreGuts(AWFile& file);
    virtual void saveGuts(AWFile& file) const;
	virtual AWspace binaryStoreSize() const;
	//virtual void debugDump(ofstream& out)const;

public:
	float		m_fov;		//Field of View in Degrees
	float		m_nearClip;	//far clipping distance
	float		m_farClip;	//near clipping distance
};//class AWExport AWCamera : public AWNode



//this should NOT BE NECESSARY if Visual C++ worked correctly...
inline void		
AWCamera::getViewCone(AWSolidCone& viewCone)
{
   AWNode::getViewCone(viewCone);
}//inline void	AWCamera::getViewCone(AWSolidCone& viewCone)



inline void
AWCamera::getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM)
{
   AWNode::getViewCone(viewCone, wsTM);
   viewCone.setAngle(m_fov);
}//inline void AWCamera::getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM)


#endif //sentinel __AWCAMERA_H
