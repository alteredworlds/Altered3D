#include "AWGL1RenderState.h"


AWGL1RenderState::AWGL1RenderState()
{
   m_hasCubeMap = FALSE;
}


AWGL1RenderState::~AWGL1RenderState()
{
   ;
}


void                 
AWGL1RenderState::resetToDefaults()
{
    AWRenderState::resetToDefaults();
    //
    m_vertArrayEnabled      = FALSE;
    m_colArrayEnabled       = FALSE;
    m_normArrayEnabled      = FALSE;
    m_combinersEnabled      = 0;
    
    for (int i=0; i<m_numTexUnits; i++)
    {
        m_texFunc[i]         = AWTextureUnit::E_INIT;
        m_texMode[i]         = AW_NO_TEXTURE;
        m_genMode[i]         = AWTexture::G_INIT;
        m_texArrayEnabled[i] = FALSE;
        m_texGenSEnabled[i] = FALSE;
        m_texGenTEnabled[i] = FALSE;
        m_texGenREnabled[i] = FALSE;
        m_texGenQEnabled[i] = FALSE;
    }
}


#ifdef __AWTRACE
static const char* envStr[] = {"GL_MODULATE", "GL_DECAL", "GL_BLEND", "GL_REPLACE", "GL_COMBINE", "UNKNOWN"};
const char* AWGL1RenderState::getTexEnvStr(int texEnv)
{
   const char* retVal = NULL;
   switch(texEnv)
   {
   case AWTextureUnit::MODULATE:
      retVal = envStr[0];
      break;

   case AWTextureUnit::DECAL:
      retVal = envStr[1];
      break;

   case AWTextureUnit::BLEND:
      retVal = envStr[2];
      break;

   case AWTextureUnit::REPLACE:
      retVal = envStr[3];
      break;

   case AWTextureUnit::COMBINE:
      retVal = envStr[4];
      break;

   case AWTextureUnit::E_INIT:
      retVal = envStr[5];
   }//switch(tex->m_texFunc) //EnvMode
   return retVal;
}//const char* AWGL1RenderState::getTexEnvStr(int texEnv)


static const char* genStr[] = {"DISABLED", "SPHERE", "REFLECT", "UNKNOWN"};
const char* AWGL1RenderState::getTexGenStr(int texEnv)
{
   const char* retVal = NULL;
   switch(texEnv)
   {
   case AWTexture::G_NONE:
      retVal = genStr[0];
      break;

   case AWTexture::SPHERE:
      retVal = genStr[1];
      break;

   case AWTexture::REFLECT:
      retVal = genStr[2];
      break;

   default:
   case AWTexture::G_INIT:
      retVal = genStr[3];
   }//switch(tex->m_texFunc) //EnvMode
   return retVal;
}//const char* AWGL1RenderState::getTexGenStr(int texEnv)
#endif
