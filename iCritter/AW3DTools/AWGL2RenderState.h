#ifndef _AW_GL2RENDER_STATE_H
#define _AW_GL2RENDER_STATE_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWMatrix4.h"
#include "AWTextureUnit.h"
#include "AWRenderState.h"
#include <OpenGLES/ES2/glext.h>
#include "AWVertexProgram.h"
#include "AWPoint4.h"
#include <map>


class AWExport AWGL2RenderState : public AWRenderState
{  
public:
    AWGL2RenderState();
    ~AWGL2RenderState();
    
    virtual void            resetToDefaults();
    
    virtual void            setLineWidth(float v);
    virtual void            enableDepthTest(AWBoolean v);
    virtual void            setDepthFunc(int depthFunc);
    virtual void            enableBlend(AWBoolean v=TRUE);
    virtual void            setBlendFac(int src, int dst);
    virtual void            setTexMatrix(const AWMatrix4& m, int texUnit){;}
    virtual void            setTexMatrixIdentity(int texUnit){;}
    virtual void            setTexMode(int mode, int texUnit);
    virtual void            setAlphatestEnabled(AWBoolean v, float ref=0.5f);
    
    virtual void            setVertexArray(float* v);
    virtual void            setVertexColorArray(unsigned char* v);
    virtual void            setNormalArray(float* v);
    virtual void            setTexCoordArray(float* v, int texUnit, int dim=2);
    
    void                    activeTexture(GLenum texture);
    
    void                    enableVertexAttribArray(GLuint slot);
    void                    disableVertexAttribArray(GLuint slot);
    
    void                    awUniform1i(GLint slot, GLint val);
    void                    awVertexAttribf4(GLint slot, float x, float y, float z, float w);
    void                    awUseProgram(AWVertexProgram* program);
    
protected:
    void                    enableVertexAttribArray(AWBoolean v, GLuint slot);
  
protected:
    AWVertexProgram*        m_activeProgram;
    map<GLuint, AWBoolean>  m_vertexAttributeArrayState;
    map<GLuint, GLuint>     m_uniform1i;
    map<GLuint, AWPoint4>   m_uniform4f;
    GLenum                  m_activeTexture;
};


inline void
AWGL2RenderState::activeTexture(GLenum texture)
{
    if (m_activeTexture != texture)
    {
        m_activeTexture = texture;
        glActiveTexture(texture);
    }
}

inline void
AWGL2RenderState::awUseProgram(AWVertexProgram* program)
{
    if ((m_activeProgram != program) && (NULL != program))
    {
        m_activeProgram = program;
        glUseProgram(program->getProgram());
    }
}

inline void
AWGL2RenderState::setTexMode(int mode, int texUnit)
{  //valid choices for mode are:
    //AW_NO_TEXTURE, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_ARB 
    m_texMode[texUnit] = mode;
}

inline void
AWGL2RenderState::enableVertexAttribArray(GLuint slot)
{
    enableVertexAttribArray(TRUE, slot);
}

inline void
AWGL2RenderState::disableVertexAttribArray(GLuint slot)
{
    enableVertexAttribArray(FALSE, slot);
}

#endif //sentinel _AW_GL2RENDER_STATE_H