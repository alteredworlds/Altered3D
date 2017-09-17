#include "AWTextureEffect.h"

#define VER_TEXTUREEFFECT_1	100
#define VER_TEXTUREEFFECT_2   101

AWDEFINE_VERSIONABLE_PERSISTABLE(AWTextureEffect, CLSID_AWTEXTUREEFFECT, VER_TEXTUREEFFECT_2);


AWTextureEffect::AWTextureEffect(AWTexture* tex, int chanID, int type, float opacity) :
   m_type(type), m_tex(tex), m_chanID(chanID), m_opacity(opacity)
{
}//AWTextureEffect::AWTextureEffect(...)


AWTextureEffect::~AWTextureEffect()
{
   m_tex = NULL;
}//AWTextureEffect::~AWTextureEffect()



AWTextureEffect&
AWTextureEffect::operator=(const AWTextureEffect& ot)
{
   m_type   = ot.m_type;
   m_tex    = ot.m_tex;
   m_chanID = ot.m_chanID;
   m_opacity= ot.m_opacity;
   return *this;
}//AWTextureEffect::operator=(const AWTextureEffect& other)



int
AWTextureEffect::operator==(const AWTextureEffect& o)const
{
   return isEqual((AWPersistable*)&o);
}//int AWTextureEffect::operator==(const AWTextureEffect& o)



//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWTextureEffect
//
//////////////////////////////////////////////////////////
void
AWTextureEffect::restoreGuts(AWFile& file)
{
   unsigned int   version;
	restoreVersion(file, &version);   // Get the saved version #
	if ( version <= VER_TEXTUREEFFECT_2 )
	{
		AWPersistable::restoreGuts(file);
      file.Read(m_type);
      file >> m_tex;
      file.Read(m_chanID);
      if (version >= VER_TEXTUREEFFECT_2)
      {
         file.Read(m_opacity);
      }//if (version >= VER_TEXTUREEFFECT_2)
	}//if ( version <= VER_TEXTUREEFFECT_2)
}//void	AWTextureEffect::restoreGuts(AWFile& file)



void        
AWTextureEffect::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
   file.Write(m_type);
   file << m_tex;
   file.Write(m_chanID);
   file.Write(m_opacity);
}//void AWTextureEffect::saveGuts(AWFile& file) const



AWspace     
AWTextureEffect::binaryStoreSize() const
{
   AWspace retVal = sizeof uVersion_ +
                     AWPersistable::binaryStoreSize() +
                     sizeof(m_type) + 
                     AWPERSISTABLE_POINTER_SIZE(m_tex) + 
                     sizeof(m_chanID) +
                     sizeof(m_opacity);
   return retVal;
}//AWspace AWTextureEffect::binaryStoreSize() const