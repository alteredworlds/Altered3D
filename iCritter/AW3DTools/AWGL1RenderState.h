#ifndef _AW_GL1RENDER_STATE_H
#define _AW_GL1RENDER_STATE_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWMatrix4.h"
#include "AWTextureUnit.h"
#include "AWRenderState.h"
#include <OpenGLES/ES1/glext.h>


class AWExport AWGL1RenderState : public AWRenderState
{  
public:
    AWGL1RenderState();
    ~AWGL1RenderState();
    
    virtual void         resetToDefaults();
    
    virtual void         setNumTexUnits(int v);
    virtual void         setLineWidth(float v);
        
    void                 setHasCubeMap(AWBoolean v);
    AWBoolean            getHasCubeMap();
    
    virtual void         enableDepthTest(AWBoolean v);        
    virtual void         setDepthFunc(int depthFunc);
    virtual void         enableBlend(AWBoolean v=TRUE);
    virtual void         setBlendFac(int src, int dst);
    
    void                 setTexFunc(AWTextureUnit& t, int texUnit);
    AWTextureUnit::EnvMode   getTexFunc(int texUnit);
    
    void                 setTexMatrix(const AWMatrix4& m, int texUnit);
    void                 setTexMatrixIdentity(int texUnit);
    
    void                 setTexMode(int mode, int texUnit);
    
    virtual void         setVertexArray(float* v);
    virtual void         setVertexColorArray(unsigned char* v);
    virtual void         setNormalArray(float* v);
    virtual void         setTexCoordArray(float* v, int texUnit, int dim=2);    
    virtual void         setAlphatestEnabled(AWBoolean v, float ref=0.5f);
        
    void                 setCombinersEnabled(int n);
    int                  getCombinersEnabled();

protected:  
    AWBoolean            m_hasCubeMap;
    AWTextureUnit::EnvMode   m_texFunc[MAX_TEX_UNITS]; 
    AWTexture::GenMode   m_genMode[MAX_TEX_UNITS];
    AWBoolean            m_texGenSEnabled[MAX_TEX_UNITS];
    AWBoolean            m_texGenTEnabled[MAX_TEX_UNITS];
    AWBoolean            m_texGenREnabled[MAX_TEX_UNITS];
    AWBoolean            m_texGenQEnabled[MAX_TEX_UNITS];    
    int                  m_combinersEnabled;
  
#ifdef __AWTRACE
   const char*          getTexEnvStr(int texEnv);
   const char*          getTexGenStr(int texEnv);
#endif
};



inline void            
AWGL1RenderState::setLineWidth(float v)
{
    if (m_lineWidth != v)
    {
        glLineWidth(v);
        m_lineWidth = v;
    }
}

inline void                 
AWGL1RenderState::setCombinersEnabled(int n)
{
   m_combinersEnabled = n;
}


inline int                  
AWGL1RenderState::getCombinersEnabled()
{
   return m_combinersEnabled;
}


inline void                 
AWGL1RenderState::setAlphatestEnabled(AWBoolean v, float ref)
{
    if (v)
    {
        glEnable(GL_ALPHA_TEST);
        m_alphaTestEnabled = TRUE;
        glAlphaFunc(GL_GEQUAL, ref);
    }
    else if (m_alphaTestEnabled)
    {
        m_alphaTestEnabled = FALSE;
        glDisable(GL_ALPHA_TEST);
    } 
}


inline void              
AWGL1RenderState::setTexCoordArray(float* v, int texUnit, int dim)
{
    if (v)
	{
        if (!m_texArrayEnabled[texUnit])
        {
            m_texArrayEnabled[texUnit] = TRUE;
            AWTRACEHI1("glEnableClientState(GL_TEXTURE_COORD_ARRAY) unit %i\n", texUnit);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
		glTexCoordPointer(dim, GL_FLOAT, 0, v);
	}
	else 
    {
        m_texArrayEnabled[texUnit] = FALSE;
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        AWTRACEHI1("glDisableClientState(GL_TEXTURE_COORD_ARRAY) unit %i\n", texUnit);
    }
}


inline void                 
AWGL1RenderState::setNormalArray(float* v)
{
   if (v)
   {
      if (!m_normArrayEnabled)
      {
         m_normArrayEnabled = TRUE;
         AWTRACEHI("glEnableClientState(GL_NORMAL_ARRAY)\n");
         glEnableClientState(GL_NORMAL_ARRAY);
      }
      glNormalPointer(GL_FLOAT, 0, v);
   }
   else if (m_normArrayEnabled)
   {
      m_normArrayEnabled = FALSE;
      AWTRACEHI("glDisableClientState(GL_NORMAL_ARRAY)\n");
      glDisableClientState(GL_NORMAL_ARRAY);
   }
}


inline void                 
AWGL1RenderState::setVertexColorArray(unsigned char* v)
{
   if (v)
	{
      if (!m_colArrayEnabled)
      {
         AWTRACEHI("glEnableClientState(GL_COLOR_ARRAY)\n");
         m_colArrayEnabled = TRUE;
         glEnableClientState(GL_COLOR_ARRAY);
      }
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, v);
	}
	else if (m_colArrayEnabled)
   {
      AWTRACEHI("glDisableClientState(GL_COLOR_ARRAY)\n");
      m_colArrayEnabled = FALSE;
		glDisableClientState(GL_COLOR_ARRAY);
   }
}


inline void                 
AWGL1RenderState::setVertexArray(float* v)
{
   if (v)
   {
      if (!m_vertArrayEnabled)
      {
         AWTRACEHI("glEnableClientState(GL_VERTEX_ARRAY)\n");
         m_vertArrayEnabled = TRUE;
         glEnableClientState(GL_VERTEX_ARRAY);
      }
      glVertexPointer(3, GL_FLOAT, 0, v);
   }
   else if (m_vertArrayEnabled)
   {
      AWTRACEHI("glDisableClientState(GL_VERTEX_ARRAY)\n");
      m_vertArrayEnabled = FALSE;
      glDisableClientState(GL_VERTEX_ARRAY);
   }
}


inline void
AWGL1RenderState::setTexMatrixIdentity(int texUnit)
{
   if (!m_texMat[texUnit].isIdentity())
   {
      AWTRACEHI1("Setting GL_TEXTURE to Identity for texUnit %i\n", texUnit);
      m_texMat[texUnit].identity();
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
   }
}


inline void
AWGL1RenderState::setTexMatrix(const AWMatrix4& m, int texUnit)
{
   if (m != m_texMat[texUnit])
   {  //change required
      m_texMat[texUnit] = m;
      glMatrixMode(GL_TEXTURE);
      glLoadMatrixf(m_texMat[texUnit].m_mat);
      glMatrixMode(GL_MODELVIEW);
      AWTRACEHI4("[0:%f  4:%f  8:%f 12:%f]\n", (double)m[0] ,(double)m[4], (double)m[8], (double)m[12]);
      AWTRACEHI4("[1:%f  5:%f  9:%f 13:%f]\n", (double)m[1] ,(double)m[5], (double)m[9], (double)m[13]);
      AWTRACEHI4("[2:%f  6:%f 10:%f 14:%f]\n", (double)m[2] ,(double)m[6], (double)m[10], (double)m[14]);
      AWTRACEHI4("[3:%f  7:%f 11:%f 15:%f]\n", (double)m[3] ,(double)m[7], (double)m[11], (double)m[15]);
   }//if (m != m_texMat[texUnit])
}


inline void 
AWGL1RenderState::setTexMode(int mode, int texUnit)
{  //valid choices for mode are:
   //AW_NO_TEXTURE, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_ARB 
   if (m_texMode[texUnit] != mode)
   {  //need to change mode. Disable current mode first
      if (AW_NO_TEXTURE != m_texMode[texUnit])
      {  //if we've ever enabled a real value
         glDisable(m_texMode[texUnit]);
      }
      m_texMode[texUnit] = mode;
      if (AW_NO_TEXTURE!=mode) 
      {
         glEnable(mode);
      }
   }
}


inline AWTextureUnit::EnvMode                  
AWGL1RenderState::getTexFunc(int texUnit)
{
   return m_texFunc[texUnit];
}


inline void
AWGL1RenderState::setTexFunc(AWTextureUnit& t, int texUnit)
{
   if (!m_combinersEnabled && (m_texFunc[texUnit] != t.m_texFunc))
   {
      AWTRACEHI3("texUnit %d texEnv from %s to %s\n", (int)texUnit,
                   getTexEnvStr(m_texFunc[texUnit]), getTexEnvStr(t.m_texFunc));
      switch(t.m_texFunc) //EnvMode {MODULATE, DECAL, BLEND, REPLACE};
      {
      case AWTextureUnit::DECAL:
         glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
         break;

      case AWTextureUnit::BLEND:
         glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
         break;

      case AWTextureUnit::REPLACE:
         glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
         break;

      default:
      case AWTextureUnit::MODULATE:
         glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
         break;
      }
      m_texFunc[texUnit] = t.m_texFunc;
   }
}


inline void                 
AWGL1RenderState::setBlendFac(int src, int dst)
{
   if ( (src!=m_blendSrcFac) || (dst!=m_blendDstFac) )
   {
      m_blendSrcFac  = src;
      m_blendDstFac  = dst;
      glBlendFunc(m_blendSrcFac, m_blendDstFac);
   }
}


inline void
AWGL1RenderState::enableDepthTest(AWBoolean v)
{
   if (m_depthFuncEnabled != v)
   {
      m_depthFuncEnabled = v;
      if (v) glEnable(GL_DEPTH_TEST);
      else glDisable(GL_DEPTH_TEST);
   }
}


inline AWBoolean                  
AWGL1RenderState::getHasCubeMap()
{
   return m_hasCubeMap;
}



inline void                  
AWGL1RenderState::setHasCubeMap(AWBoolean v)
{
   m_hasCubeMap=v;
}


inline void                 
AWGL1RenderState::enableBlend(AWBoolean v)
{
   if (m_blendEnabled != v)
   {
      m_blendEnabled = v;
      if (m_blendEnabled) glEnable(GL_BLEND);
      else glDisable(GL_BLEND);
   }
}


inline void
AWGL1RenderState::setDepthFunc(int depthFunc)
{
   if (m_depthFunc != depthFunc)
   {
      AWTRACEHI2("depthFunc was %d now %d\n", m_depthFunc, depthFunc);
      m_depthFunc = depthFunc;
      glDepthFunc(m_depthFunc);
   }
}


inline void                 
AWGL1RenderState::setNumTexUnits(int v)
{
   if (v > MAX_TEX_UNITS)
   {
      v = MAX_TEX_UNITS;
   }
   m_numTexUnits = v;
}

#endif //sentinel _AW_GL1RENDER_STATE_H