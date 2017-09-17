#include "AWTextureUnit.h"
//following required only because haven't 
//redefined constants in 'AW_?' form yet
#include <OpenGLES/ES2/glext.h>


#define VER_AWTEXTUREUNIT_1 100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWTextureUnit,  CLSID_AWTEXTUREUNIT, VER_AWTEXTUREUNIT_1);


AWTextureUnit::AWTextureUnit(AWTexture* tex, int chanID) : 
         m_tex(tex), m_chanID(chanID)
{
   resetCombinerData();
}



void                 
AWTextureUnit::resetCombinerData()
{
   m_texFunc      = MODULATE;
    m_combineMode  = 0x2100;//GL_MODULATE;
	m_srce0Name    = GL_TEXTURE;//GL_PREVIOUS_EXT;
   m_srce0Operand = GL_SRC_COLOR; 
   m_srce1Name    = GL_TEXTURE;
   m_srce1Operand = GL_SRC_COLOR;
}//void AWTextureUnit::resetCombinerData()



void         
AWTextureUnit::restoreGuts(AWFile& file)
{
   unsigned int   version;
	restoreVersion(file, &version);   // Get the saved version #
	if ( version <= VER_AWTEXTUREUNIT_1 )
	{
		AWPersistable::restoreGuts(file);
      file.Read(m_chanID);
      file >> m_tex;     
	}//if ( version <= VER_AWTEXTUREUNIT_1 )
}//void AWTextureUnit::restoreGuts(AWFile& file)



void         
AWTextureUnit::saveGuts(AWFile& file) const
{
   saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
   file.Write(m_chanID);
   file << m_tex;
}//void AWTextureUnit::saveGuts(AWFile& file) const




AWspace      
AWTextureUnit::binaryStoreSize() const
{
   AWspace retVal = sizeof uVersion_ +
                     AWPersistable::binaryStoreSize() +
                     AWPERSISTABLE_POINTER_SIZE(m_tex) + 
                     sizeof(m_chanID);
   return retVal;
}//AWspace AWTextureUnit::binaryStoreSize() const
