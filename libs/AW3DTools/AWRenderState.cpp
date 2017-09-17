#include "AWRenderState.h"



AWRenderState::AWRenderState()
{
   m_hasCubeMap = FALSE;
   m_numTexUnits = 1;
   resetToDefaults();
}



AWRenderState::~AWRenderState()
{
   resetToDefaults();
}



void                 
AWRenderState::resetToDefaults()
{
   m_vertArrayEnabled      = FALSE;
   m_colArrayEnabled       = FALSE;
   m_normArrayEnabled      = FALSE;
   m_polyMode              = GL_FILL;
   m_blendEnabled          = FALSE;
   m_blendSrcFac           = -1;
   m_blendDstFac           = -1;
   m_depthFunc             = GL_ALWAYS;
   m_alphaTestEnabled      = FALSE;
   m_depthFuncEnabled      = FALSE;
   m_combinersEnabled      = 0;
   m_lineWidth             = 1.0f;

   for (int i=0; i<m_numTexUnits; i++)
   {
      m_selectedTexture[i] = -1;
      m_texFunc[i]         = AWTextureUnit::E_INIT;
      m_texMode[i]         = AW_NO_TEXTURE;
      m_genMode[i]         = AWTexture::G_INIT;
      m_texMat[i].identity();
      m_texArrayEnabled[i] = FALSE;
      m_texGenSEnabled[i] = FALSE;
      m_texGenTEnabled[i] = FALSE;
      m_texGenREnabled[i] = FALSE;
      m_texGenQEnabled[i] = FALSE;
   }//for (int i=0; i<MAX_TEX_UNITS; i++)
}//void  AWRenderState::resetToDefaults()



#ifdef __AWTRACE
static char* envStr[] = {"GL_MODULATE", "GL_DECAL", "GL_BLEND", "GL_REPLACE", "GL_COMBINE", "UNKNOWN"};
const char* AWRenderState::getTexEnvStr(int texEnv)
{
   char* retVal = NULL;
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
}//const char* AWRenderState::getTexEnvStr(int texEnv)


static char* genStr[] = {"DISABLED", "SPHERE", "REFLECT", "UNKNOWN"};
const char* AWRenderState::getTexGenStr(int texEnv)
{
   char* retVal = NULL;
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
}//const char* AWRenderState::getTexGenStr(int texEnv)
#endif
