#include "AWLight.h"
#include "AWFlightController.h"
#include "AWTexture.h"

#define VER_AWLIGHT_1	100
#define VER_AWLIGHT_2	104
#define VER_AWLIGHT_3   106

AWDEFINE_VERSIONABLE_PERSISTABLE(AWLight, CLSID_AWLIGHT, VER_AWLIGHT_3);


AWLight::AWLight(const AWCString& name, AWNode* parent, LightType type) 
		: AWNode(name, parent), m_type(type), m_proj(NULL), m_shadow(NULL)
{
	m_on			= TRUE;
	m_attenuation	= AWPoint(0.0f, 0.0f, 0.0f);	//no attenuation
	m_cutOffAngle	= 1.3f; //Random!!
	m_ID			= -1;
}//AWLight::AWLight()



AWLight::~AWLight()
{	
}//AWLight::~AWLight()



int		
AWLight::calcTransform(float t, const AWMatrix4& parentTM)
{
   int retVal = AWNode::calcTransform(t, parentTM);
	calcColor(t);
   if (m_proj)
   {  //need to update the texture matrix of the 
      //projective texture, if any
   }//if (m_proj)
	return retVal;
}//int AWLight::calcTransform(float t, const AWMatrix4& parentTM)



void        
AWLight::restoreGuts(AWFile& file)
{
	unsigned int version;
	int			 temp;
	restoreVersion(file, &version);   // Get the saved version #
   if (version <= VER_AWLIGHT_3)
   {
	   AWNode::restoreGuts(file);
	   file >> temp;
	   m_type = (LightType)temp;
	   file >> m_on;
	   file >> m_colour;
	   file.Read(m_cutOffAngle);
	   m_attenuation.restoreGuts(file);
      if ((version >= VER_AWLIGHT_2) && (AW_SPOT_LIGHT==m_type))
      {
         file >> m_proj;
         if (version >= VER_AWLIGHT_3)
         {
            file >> m_shadow;
         }//if (version >= VER_AWLIGHT_3)
      }//if ((version >= VER_AWLIGHT_2) && (AW_SPOT_LIGHT==m_type))
   }//if (version <= VER_AWLIGHT_2)
}//void AWLight::saveGuts(AWFile& file) const



void        
AWLight::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWNode::saveGuts(file);
	file << (int)m_type;
	file << m_on;
	file << m_colour;
	file.Write(m_cutOffAngle);
	m_attenuation.saveGuts(file);
   if (AW_SPOT_LIGHT==m_type)
   {
      file << m_proj;
      file << m_shadow;
   }//if (AW_SPOT_LIGHT==m_type)
}//void AWLight::saveGuts(AWFile& file) const



AWspace     
AWLight::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWNode::binaryStoreSize() +
		sizeof (m_type) +
		sizeof (m_on) +	
		m_colour.binaryStoreSize() +
		sizeof (m_attenuation) +
		sizeof (m_cutOffAngle);
   if (AW_SPOT_LIGHT==m_type)
   {
      retVal += AWPERSISTABLE_POINTER_SIZE(m_proj);
      retVal += AWPERSISTABLE_POINTER_SIZE(m_shadow);
   }
	return retVal;
}//AWspace  AWLight::binaryStoreSize() const
