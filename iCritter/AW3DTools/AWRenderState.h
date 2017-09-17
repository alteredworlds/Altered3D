#ifndef _AW_RENDER_STATE_H
#define _AW_RENDER_STATE_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWMatrix4.h"
#include "AWTextureUnit.h"
#include <OpenGLES/ES1/glext.h>


#define MAX_TEX_UNITS 8

//
// used to minimize (OpenGL) state changes
//
class AWExport AWRenderState
{  
public:
    AWRenderState();
    ~AWRenderState();
    
    virtual void            resetToDefaults();
    
    virtual void            setLineWidth(float v)=0;
    float                   getLineWidth();
    
    virtual void            setNumTexUnits(int v);
    int                     getNumTexUnits();
    
    virtual void            enableDepthTest(AWBoolean v)=0;
    AWBoolean               getDepthTestEnabled();
    
    virtual void            setDepthFunc(int depthFunc)=0;
    int                     getDepthFunc();
    
    virtual void            enableBlend(AWBoolean v=TRUE)=0;
    AWBoolean               getBlendEnabled();
    
    int                     getBlendSrcFac();
    int                     getBlendDstFac();
    virtual void            setBlendFac(int src, int dst)=0;
        
    virtual void            setTexMatrix(const AWMatrix4& m, int texUnit)=0;
    virtual void            setTexMatrixIdentity(int texUnit)=0;
    
    // used to indicate if there is a texture for a particular texUnit
    //  and if so, whether 2D or CubeMap
    virtual void            setTexMode(int mode, int texUnit)=0;
    int                     getTexMode(int texUnit);
    
    // used to associate a texture ID with a specific texUnit 
    virtual void            setSelectedTex(int texID, int texUnit);
    int                     getSelectedTex(int texUnit);
    
    virtual void            setAlphatestEnabled(AWBoolean v, float ref=0.5f)=0;
    AWBoolean               getAlphaTestEnabled();
    
    virtual void            setVertexArray(float* v)=0;
    AWBoolean               getVertexArrayEnabled();
    
    virtual void            setVertexColorArray(unsigned char* v)=0;
    AWBoolean               getVertexColorArrayEnabled();
    
    virtual void            setNormalArray(float* v)=0;
    AWBoolean               getNormalArrayEnabled();
    
    virtual void            setTexCoordArray(float* v, int texUnit, int dim=2)=0;
    AWBoolean               getTexCoordArrayEnabled(int texUnit);


protected:  
    int                     m_numTexUnits;
    int                     m_depthFunc;
    int                     m_depthFuncEnabled;
    AWBoolean               m_blendEnabled;
    int                     m_blendSrcFac;
    int                     m_blendDstFac;
    float                   m_lineWidth;
    AWBoolean               m_alphaTestEnabled; 
    int                     m_selectedTexture[MAX_TEX_UNITS];
    int                     m_texMode[MAX_TEX_UNITS];
    AWMatrix4               m_texMat[MAX_TEX_UNITS];
    AWBoolean               m_texArrayEnabled[MAX_TEX_UNITS];
    AWBoolean               m_vertArrayEnabled;
    AWBoolean               m_colArrayEnabled;
    AWBoolean               m_normArrayEnabled;
};


inline float                 
AWRenderState::getLineWidth()
{
   return m_lineWidth;
}


inline AWBoolean            
AWRenderState::getAlphaTestEnabled()
{
   return m_alphaTestEnabled;
}


inline int                 
AWRenderState::getSelectedTex(int texUnit)
{
   return m_selectedTexture[texUnit];
}

inline void
AWRenderState::setSelectedTex(int texID, int texUnit)
{  
    m_selectedTexture[texUnit] = texID;
}

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
   }
   m_numTexUnits = v;
}

inline int                  
AWRenderState::getNumTexUnits()
{
   return m_numTexUnits;
}

inline int 
AWRenderState::getTexMode(int texUnit)
{
    return m_texMode[texUnit];
}

inline AWBoolean            
AWRenderState::getVertexArrayEnabled()
{
    return m_vertArrayEnabled;
}

inline AWBoolean            
AWRenderState::getVertexColorArrayEnabled()
{
    return m_colArrayEnabled;
}

inline AWBoolean            
AWRenderState::getNormalArrayEnabled()
{
    return m_normArrayEnabled;
}

inline AWBoolean            
AWRenderState::getTexCoordArrayEnabled(int texUnit)
{
    return m_texArrayEnabled[texUnit];
}


#endif //sentinel _AW_RENDER_STATE_H