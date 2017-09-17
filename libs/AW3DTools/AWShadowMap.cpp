#include "AWShadowMap.h"
#include "GL/glext.h"

#define VER_AWSHADOWMAP_1 100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWShadowMap, CLSID_AWSHADOWMAP, VER_AWSHADOWMAP_1);


AWShadowMap::AWShadowMap(const AWCString& name, int size, float bias, float scale) 
           : AWTexture(name)
{
   m_width = m_height = size;
   m_bias  = bias;
   m_range = scale;
}


AWShadowMap::~AWShadowMap()
{
}

//int                  operator==(const AWShadowMap& o);
//NVIDIA/SGI style shadow map - rendered at run time
int          
AWShadowMap::init()
{ 
   return AWTexture::init();
}//int AWShadowMap::init()


//OVERRIDES OF BASE CLASS
void 			         
AWShadowMap::reSize(int xsize, int ysize)
{  //this should never be used - disable
   ;
}


//AWPERSISTABLE VIRTUALS
void         
AWShadowMap::restoreGuts(AWFile& file)
{
   unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWSHADOWMAP_1==version)
	{
		AWTexture::restoreGuts(file);
      file >> m_range;
      file >> m_bias;
	}//if (VER_AWSHADOWMAP_1==version)
}//void AWShadowMap::restoreGuts(AWFile& file)



void         
AWShadowMap::saveGuts(AWFile& file) const
{
   saveVersion(file, uVersion_ );	// version number
	AWTexture::saveGuts(file);
   file << m_range;
   file << m_bias;
}//void AWShadowMap::saveGuts(AWFile& file) const


AWspace      
AWShadowMap::binaryStoreSize() const
{
   AWspace retVal = sizeof uVersion_ +
		AWTexture::binaryStoreSize() +
      sizeof(m_range) + 
      sizeof(m_bias);
	return retVal;
}//AWspace AWShadowMap::binaryStoreSize() const
