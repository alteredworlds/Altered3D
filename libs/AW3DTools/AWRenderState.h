#ifndef _AW_RENDER_STATE_H
#define _AW_RENDER_STATE_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWMatrix4.h"
#include "AWTextureUnit.h"
#include "GL/glext.h"


#define MAX_TEX_UNITS 8


class AWExport AWRenderState
{  //used to minimize (OpenGL) state changes
public:
   AWRenderState();
   ~AWRenderState();

   void                 resetToDefaults();

   float                getLineWidth();

   void                 setNumTexUnits(int v);
   int                  getNumTexUnits();

   void                 setHasCubeMap(AWBoolean v);
   AWBoolean            getHasCubeMap();

   void                 enableDepthTest(AWBoolean v);
   AWBoolean            getDepthTestEnabled();

   void                 setDepthFunc(int depthFunc);
   int                  getDepthFunc();

   void                 setPolygonMode(int m);
   int                  getPolygonMode();

   void                 enableBlend(AWBoolean v=TRUE);
   AWBoolean            getBlendEnabled();

   int                  getBlendSrcFac();
   int                  getBlendDstFac();
   void                 setBlendFac(int src, int dst);

   void                 setTexFunc(AWTextureUnit& t, int texUnit);
   AWTextureUnit::EnvMode   getTexFunc(int texUnit);

   void                 enableTexGenR(int texUnit, AWBoolean v=TRUE);
   void                 enableTexGenS(int texUnit, AWBoolean v=TRUE);
   void                 enableTexGenT(int texUnit, AWBoolean v=TRUE);
   void                 enableTexGenQ(int texUnit, AWBoolean v=TRUE);
   void                 setTexgenPlanes(GLenum plane_type);

   void                 setTexGenMode(AWTexture::GenMode func, int texUnit);
   AWTexture::GenMode   getTexGenMode(int texUnit);

   void                 setTexMatrix(const AWMatrix4& m, int texUnit);
   void                 setTexMatrixIdentity(int texUnit);

   void                 setTexMode(int mode, int texUnit);
   int                  getTexMode(int texUnit);

   void                 setSelectedTex(int texID, int texUnit);
   int                  getSelectedTex(int texUnit);

   void                 setVertexArray(float* v);
   AWBoolean            getVertexArrayEnabled();

   void                 setVertexColorArray(unsigned char* v);
   AWBoolean            getVertexColorArrayEnabled();

   void                 setNormalArray(float* v);
   AWBoolean            getNormalArrayEnabled();

   void                 setTexCoordArray(float* v, int texUnit, int dim=2);
   AWBoolean            getTexCoordArrayEnabled(int texUnit);

   void                 setAlphatestEnabled(AWBoolean v, float ref=0.5f);
   AWBoolean            getAlphaTestEnabled();

   void                 setCombinersEnabled(int n);
   int                  getCombinersEnabled();

protected:  
   AWBoolean            m_hasCubeMap;
   int                  m_numTexUnits;
//indicates if functionality currently enabled - STATE
   int                  m_depthFunc;
   int                  m_depthFuncEnabled;
   int                  m_polyMode;
   AWBoolean            m_blendEnabled;
   int                  m_blendSrcFac;
   int                  m_blendDstFac;
   float                m_lineWidth;
   AWTextureUnit::EnvMode   m_texFunc[MAX_TEX_UNITS]; 
   AWTexture::GenMode   m_genMode[MAX_TEX_UNITS];
   AWMatrix4            m_texMat[MAX_TEX_UNITS];
   int                  m_texMode[MAX_TEX_UNITS];
   int                  m_selectedTexture[MAX_TEX_UNITS];
   AWBoolean            m_texGenSEnabled[MAX_TEX_UNITS];
   AWBoolean            m_texGenTEnabled[MAX_TEX_UNITS];
   AWBoolean            m_texGenREnabled[MAX_TEX_UNITS];
   AWBoolean            m_texGenQEnabled[MAX_TEX_UNITS];

   AWBoolean	         m_texArrayEnabled[MAX_TEX_UNITS];
   AWBoolean	         m_vertArrayEnabled;
   AWBoolean	         m_colArrayEnabled;
   AWBoolean	         m_normArrayEnabled;

   AWBoolean            m_alphaTestEnabled;  
   int                  m_combinersEnabled;
    
  
#ifdef __AWTRACE
   const char*          getTexEnvStr(int texEnv);
   const char*          getTexGenStr(int texEnv);
#endif
};//class AWExport AWRenderState


inline float                 
AWRenderState::getLineWidth()
{
   return m_lineWidth;
}//inline float AWRenderState::getLineWidth()


inline void                 
AWRenderState::setCombinersEnabled(int n)
{
   m_combinersEnabled = n;
}//inline void AWRenderState::setCombinersEnabled(int n)


inline int                  
AWRenderState::getCombinersEnabled()
{
   return m_combinersEnabled;
}//inline int AWRenderState::getCombinersEnabled()


inline void                 
AWRenderState::setAlphatestEnabled(AWBoolean v, float ref)
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
}//inline void AWRenderState::setAlphatestEnabled(AWBoolean v, float ref)


inline AWBoolean            
AWRenderState::getAlphaTestEnabled()
{
   return m_alphaTestEnabled;
}//inline AWBoolean AWRenderState::getAlphaTestEnabled()



inline void              
AWRenderState::setTexCoordArray(float* v, int texUnit, int dim)
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
	}//if (theObj->m_tVertexList)
	else //if (m_texArrayEnabled[texUnit])
   {
      m_texArrayEnabled[texUnit] = FALSE;
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      AWTRACEHI1("glDisableClientState(GL_TEXTURE_COORD_ARRAY) unit %i\n", texUnit);
   }//else if (m_texArrayEnabled)
}//inline void AWRenderState::setTexCoordArray(float* v)



inline AWBoolean            
AWRenderState::getTexCoordArrayEnabled(int texUnit)
{
   return m_texArrayEnabled[texUnit];
}//inline AWBoolean AWRenderState::getTexCoordArrayEnabled(int texUnit)


inline void                 
AWRenderState::setNormalArray(float* v)
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
   }//if (theObj->m_normalList)
   else if (m_normArrayEnabled)
   {
      m_normArrayEnabled = FALSE;
      AWTRACEHI("glDisableClientState(GL_NORMAL_ARRAY)\n");
      glDisableClientState(GL_NORMAL_ARRAY);
   }//else if (m_normArrayEnabled)
}//inline void AWRenderState::setNormalArray(float* v)


inline AWBoolean            
AWRenderState::getNormalArrayEnabled()
{
   return m_normArrayEnabled;
}//inline AWBoolean AWRenderState::getNormalArrayEnabled()



inline void                 
AWRenderState::setVertexColorArray(unsigned char* v)
{
   if (v)
	{
      if (!m_colArrayEnabled)
      {
         AWTRACEHI("glEnableClientState(GL_COLOR_ARRAY)\n");
         m_colArrayEnabled = TRUE;
         glEnableClientState(GL_COLOR_ARRAY);
      }
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, v);
	}//if (theObj->m_vertColList)
	else if (m_colArrayEnabled)
   {
      AWTRACEHI("glDisableClientState(GL_COLOR_ARRAY)\n");
      m_colArrayEnabled = FALSE;
		glDisableClientState(GL_COLOR_ARRAY);
   }//else if (m_colArrayEnabled)
}//void AWRenderState::setVertexColorArray(unsigned char* v)



inline AWBoolean            
AWRenderState::getVertexColorArrayEnabled()
{
   return m_colArrayEnabled;
}



inline void                 
AWRenderState::setVertexArray(float* v)
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
   }//if (v)
   else if (m_vertArrayEnabled)
   {
      AWTRACEHI("glDisableClientState(GL_VERTEX_ARRAY)\n");
      m_vertArrayEnabled = FALSE;
      glDisableClientState(GL_VERTEX_ARRAY);
   }//else if (m_vertArrayEnabled)
}//inline void AWRenderState::setVertexArray(float* v)



inline AWBoolean            
AWRenderState::getVertexArrayEnabled()
{
   return m_vertArrayEnabled;
}


inline void
AWRenderState::setTexMatrixIdentity(int texUnit)
{
   if (!m_texMat[texUnit].isIdentity())
   {
      AWTRACEHI1("Setting GL_TEXTURE to Identity for texUnit %i\n", texUnit);
      m_texMat[texUnit].identity();
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
   }//if (!m_texMat[texUnit].isIdentity())
}//inline void AWRenderState::setTexMatrixIdentity(int texUnit)



inline void
AWRenderState::setTexMatrix(const AWMatrix4& m, int texUnit)
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
}//void AWRenderState::setTexMatrix(const AWMatrix4& m, int texUnit)


inline void
AWRenderState::setSelectedTex(int texID, int texUnit)
{  
   m_selectedTexture[texUnit] = texID;
}//void AWRenderState::setSelectedTex(AWTexture* tex, int texUnit)



inline int                 
AWRenderState::getSelectedTex(int texUnit)
{
   return m_selectedTexture[texUnit];
}//inline int AWRenderState::getSelectedTex(int texUnit)



inline void 
AWRenderState::setTexMode(int mode, int texUnit)
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
   }//if (m_texMode[texUnit] != mode)
}//inline void AWRenderState::setTexMode(int mode, int texUnit)


inline int 
AWRenderState::getTexMode(int texUnit)
{
   return m_texMode[texUnit];
}//inline int AWRenderState::getTexMode(int texUnit)



inline AWTexture::GenMode                  
AWRenderState::getTexGenMode(int texUnit)
{
   return m_genMode[texUnit];
}//inline AWTexture::GenMode AWRenderState::getTexGenMode(int texUnit)


inline void
AWRenderState::enableTexGenS(int texUnit, AWBoolean v)
{
   if (m_texGenSEnabled[texUnit]!=v)
   {
      if (v)
      {
         glEnable(GL_TEXTURE_GEN_S);        
      }
      else
      {
         glDisable(GL_TEXTURE_GEN_S);
      }
      m_texGenSEnabled[texUnit]=v;
   }//if (m_texGenSEnabled[texUnit]!=v)
}//inline void AWRenderState::enableTexGenS(int texUnit, AWBoolean v)



inline void
AWRenderState::enableTexGenT(int texUnit, AWBoolean v)
{
   if (m_texGenTEnabled[texUnit]!=v)
   {
      if (v)
      {
         glEnable(GL_TEXTURE_GEN_T);
      }
      else
      {
         glDisable(GL_TEXTURE_GEN_T);
      }
      m_texGenTEnabled[texUnit]=v;
   }//if (m_texGenTEnabled[texUnit]!=v)
}//inline void AWRenderState::enableTexGenT(int texUnit, AWBoolean v)


inline void
AWRenderState::enableTexGenR(int texUnit, AWBoolean v)
{
   if (m_texGenREnabled[texUnit]!=v)
   {
      if (v)
      {
         glEnable(GL_TEXTURE_GEN_R);
      }
      else
      {
         glDisable(GL_TEXTURE_GEN_R);
      }
      m_texGenREnabled[texUnit]=v;
   }//if (m_texGenREnabled[texUnit]!=v)
}//inline void AWRenderState::enableTexGenR(int texUnit, AWBoolean v)


inline void
AWRenderState::enableTexGenQ(int texUnit, AWBoolean v)
{
   if (m_texGenQEnabled[texUnit]!=v)
   {
      if (v)
      {
         glEnable(GL_TEXTURE_GEN_Q);
      }
      else
      {
         glDisable(GL_TEXTURE_GEN_Q);
      }
      m_texGenQEnabled[texUnit]=v;
   }//if (m_texGenQEnabled[texUnit]!=v)
}//inline void AWRenderState::enableTexGenQ(int texUnit, AWBoolean v)



inline void 
AWRenderState::setTexgenPlanes(GLenum plane_type)
{
   float row1[] = {1.0f, 0.0f, 0.0f, 0.0f};
   float row2[] = {0.0f, 1.0f, 0.0f, 0.0f};
   float row3[] = {0.0f, 0.0f, 1.0f, 0.0f};
   float row4[] = {0.0f, 0.0f, 0.0f, 1.0f};
   glTexGenfv(GL_S, plane_type, row1);
   glTexGenfv(GL_T, plane_type, row2);
   glTexGenfv(GL_R, plane_type, row3);
   glTexGenfv(GL_Q, plane_type, row4);
}//inline void AWRenderState::setTexgenPlanes(GLenum plane_type)


inline void
AWRenderState::setTexGenMode(AWTexture::GenMode func, int texUnit)
{
   if (m_genMode[texUnit] != func)
   {
      AWTRACEHI3("texUnit %d genMode from %s to %s\n", (int)texUnit, 
                                              getTexGenStr(m_genMode[texUnit]), 
                                              getTexGenStr(func));
      switch(func) //enum GenMode {G_NONE,     SPHERE,      REFLECT}
      {
         case AWTexture::G_NONE:
            enableTexGenS(texUnit, FALSE);
            enableTexGenT(texUnit, FALSE);
            enableTexGenR(texUnit, FALSE);
            enableTexGenQ(texUnit, FALSE);
            break;

         case AWTexture::SPHERE:
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            enableTexGenS(texUnit);
            enableTexGenT(texUnit);
            enableTexGenR(texUnit, FALSE);
            enableTexGenQ(texUnit, FALSE);
            break;

         case AWTexture::REFLECT:
            if (m_hasCubeMap)
            {
               glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
               glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
               glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
               enableTexGenS(texUnit);
               enableTexGenT(texUnit);
               enableTexGenR(texUnit);
               enableTexGenQ(texUnit, FALSE);
            }//if (m_hasCubeMap)
            break;

         case AWTexture::EYE_LINEAR:
            enableTexGenS(texUnit);
            enableTexGenT(texUnit);
            enableTexGenR(texUnit);
            enableTexGenQ(texUnit);
            setTexgenPlanes(GL_EYE_PLANE);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
            glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 	         
            break;
      }//switch(m_genMode) //enum GenMode
      m_genMode[texUnit] = func;
   }//if (m_genMode[texUnit] != func)
}//inline void AWRenderState::setTexGenMode(AWTexture::GenMode func, int texUnit)



inline AWTextureUnit::EnvMode                  
AWRenderState::getTexFunc(int texUnit)
{
   return m_texFunc[texUnit];
}//inline int AWRenderState::getTexFunc(int texUnit)



//AWRenderState::setTexFunc(AWTexture::EnvMode func, int texUnit)
inline void
AWRenderState::setTexFunc(AWTextureUnit& t, int texUnit)
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

      case AWTextureUnit::COMBINE:
         //need to specify the desired combination in detail
         //also should specify ALPHA component if necessary
         glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
         glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  t.m_combineMode);
         glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  t.m_srce0Name);
         glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, t.m_srce0Operand);
         glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  t.m_srce1Name);
         glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, t.m_srce1Operand);
         break;

      default:
      case AWTextureUnit::MODULATE:
         glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
         break;
      }//switch(tex->m_texFunc) //EnvMode
      m_texFunc[texUnit] = t.m_texFunc;
   }//if (!m_combinersEnabled && (m_texFunc[texUnit] != func))
}//inline void AWRenderState::setTexFunc(AWTextureUnit& t, int texUnit)



inline int                  
AWRenderState::getBlendSrcFac()
{
   return m_blendSrcFac;
}


inline int                  
AWRenderState::getBlendDstFac()
{
   return m_blendDstFac;
}


inline void                 
AWRenderState::setBlendFac(int src, int dst)
{
   if ( (src!=m_blendSrcFac) || (dst!=m_blendDstFac) )
   {
      m_blendSrcFac  = src;
      m_blendDstFac  = dst;
      glBlendFunc(m_blendSrcFac, m_blendDstFac);
   }//if ( (src!=m_blendSrcFac) || (dst!=m_blendDstFac) )
}//inline void AWRenderState::setBlendFac(int src, int dst)



inline void
AWRenderState::setPolygonMode(int m)
{
   if (m_polyMode != m)
   {
      m_polyMode = m;
      glPolygonMode(GL_FRONT_AND_BACK, m_polyMode);
   }
}//inline void AWRenderState::setPolygonMode(int m)



inline int                  
AWRenderState::getPolygonMode()
{
   return m_polyMode;
}//inline int AWRenderState::getPolygonMode()



inline void
AWRenderState::enableDepthTest(AWBoolean v)
{
   if (m_depthFuncEnabled != v)
   {
      m_depthFuncEnabled = v;
      if (v) glEnable(GL_DEPTH_TEST);
      else glDisable(GL_DEPTH_TEST);
   }//if (m_depthFuncEnabled != v)
}//inline void AWRenderState::enableDepthTest(AWBoolean v)



inline AWBoolean                  
AWRenderState::getHasCubeMap()
{
   return m_hasCubeMap;
}



inline void                  
AWRenderState::setHasCubeMap(AWBoolean v)
{
   m_hasCubeMap=v;
}


inline void                 
AWRenderState::enableBlend(AWBoolean v)
{
   if (m_blendEnabled != v)
   {
      m_blendEnabled = v;
      if (m_blendEnabled) glEnable(GL_BLEND);
      else glDisable(GL_BLEND);
   }
}//inline void AWRenderState::enableBlend(AWBoolean v)



inline AWBoolean                  
AWRenderState::getBlendEnabled()
{
   return m_blendEnabled;
}


inline AWBoolean                  
AWRenderState::getDepthTestEnabled()
{
   return m_depthFuncEnabled;
}


inline void
AWRenderState::setDepthFunc(int depthFunc)
{
   if (m_depthFunc != depthFunc)
   {
      AWTRACEHI2("depthFunc was %d now %d\n", m_depthFunc, depthFunc);
      m_depthFunc = depthFunc;
      glDepthFunc(m_depthFunc);
   }//if (m_depthFunc != depthFunc)
}//inline void AWRenderState::setDepthFunc(int depthFunc)



inline int                  
AWRenderState::getDepthFunc()
{
   return m_depthFunc;
}

inline void                 
AWRenderState::setNumTexUnits(int v)
{
   if (v > MAX_TEX_UNITS)
   {
      v = MAX_TEX_UNITS;
   }//if (v > MAX_TEX_UNITS)
   m_numTexUnits = v;
}//inline void AWRenderState::setNumTexUnits(int v)


inline int                  
AWRenderState::getNumTexUnits()
{
   return m_numTexUnits;
}

#endif //sentinel _AW_RENDER_STATE_H