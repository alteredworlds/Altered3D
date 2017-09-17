#include "AWRenderState.h"


AWRenderState::AWRenderState()
{
   m_numTexUnits = 1;
}


AWRenderState::~AWRenderState()
{
   resetToDefaults();
}


void                 
AWRenderState::resetToDefaults()
{
   m_blendEnabled          = FALSE;
   m_blendSrcFac           = -1;
   m_blendDstFac           = -1;
   m_depthFunc             = GL_ALWAYS;
   m_alphaTestEnabled      = FALSE;
   m_depthFuncEnabled      = FALSE;
   m_lineWidth             = 1.0f;

   for (int i=0; i<m_numTexUnits; i++)
   {
      m_selectedTexture[i] = -1;
      m_texMat[i].identity();
   }
}
