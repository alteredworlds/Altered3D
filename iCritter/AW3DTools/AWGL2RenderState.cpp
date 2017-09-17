#include "AWGL2RenderState.h"


AWGL2RenderState::AWGL2RenderState()
{
}



AWGL2RenderState::~AWGL2RenderState()
{
    ;
}

void         
AWGL2RenderState::resetToDefaults()
{
    AWRenderState::resetToDefaults();
    m_activeProgram = NULL;
}

void            
AWGL2RenderState::setLineWidth(float v)
{
    if (m_lineWidth != v)
    {
        glLineWidth(v);
        m_lineWidth = v;
    }
}

void                 
AWGL2RenderState::setAlphatestEnabled(AWBoolean v, float ref)
{
    if (-1 != m_activeProgram->m_alphaTest)
    {
        if (v)
        {
            awUniform1i(m_activeProgram->m_alphaTest, 1);
            m_alphaTestEnabled = TRUE;
            // NOTE: always using a reference value of 0.5, which is hardcoded in
            // the shader for the time being.
        }
        else if (m_alphaTestEnabled)
        {
            m_alphaTestEnabled = FALSE;
            awUniform1i(m_activeProgram->m_alphaTest, 0);
        }
    }
}

void
AWGL2RenderState::enableDepthTest(AWBoolean v)
{
    if (m_depthFuncEnabled != v)
    {
        m_depthFuncEnabled = v;
        if (v) 
            glEnable(GL_DEPTH_TEST);
        else 
            glDisable(GL_DEPTH_TEST);
    }
}

void
AWGL2RenderState::setDepthFunc(int depthFunc)
{
    if (m_depthFunc != depthFunc)
    {
        AWTRACEHI2("depthFunc was %d now %d\n", m_depthFunc, depthFunc);
        m_depthFunc = depthFunc;
        glDepthFunc(m_depthFunc);
    }
}

void                 
AWGL2RenderState::enableBlend(AWBoolean v)
{
    if (m_blendEnabled != v)
    {
        m_blendEnabled = v;
        if (m_blendEnabled) 
            glEnable(GL_BLEND);
        else 
            glDisable(GL_BLEND);
    }
}

void                 
AWGL2RenderState::setBlendFac(int src, int dst)
{
    if ( (src!=m_blendSrcFac) || (dst!=m_blendDstFac) )
    {
        m_blendSrcFac  = src;
        m_blendDstFac  = dst;
        glBlendFunc(m_blendSrcFac, m_blendDstFac);
    }
}

void
AWGL2RenderState::enableVertexAttribArray(AWBoolean v, GLuint slot)
{
    if (-1 != slot)
    {
        AWBoolean isEnabled(FALSE);
        map<GLuint, AWBoolean>::const_iterator pos = m_vertexAttributeArrayState.find(slot);
        // note that not yet present in the map implies not yet used and in default state
        // default state is OFF
        if (pos != m_vertexAttributeArrayState.end())
        {
            isEnabled = pos->second;
        }
        if (v != isEnabled)
        {
            m_vertexAttributeArrayState[slot] = v;
            if (v)
                glEnableVertexAttribArray(slot);
            else
                glDisableVertexAttribArray(slot);
        }
    }
}

void
AWGL2RenderState::awUniform1i(GLint slot, GLint val)
{
    if (-1 != slot)
    {
        map<GLuint, GLuint>::const_iterator pos = m_uniform1i.find(slot);
        if ((pos == m_uniform1i.end()) || (pos->second != val))
        {
            m_uniform1i[slot] = val;
            glUniform1i(slot, val);
        }
    }
}

void
AWGL2RenderState::awVertexAttribf4(GLint slot, float x, float y, float z, float w)
{
    if (-1 != slot)
    {
        // find any cached value for the GLint : slot (vertex attribute of type float[4])
        map<GLuint, AWPoint4>::const_iterator pos = m_uniform4f.find(slot);
        // if there is a cached value, compare to see if anything has changed
        AWPoint4 tmp(x, y, z, w);
        if ((pos == m_uniform4f.end()) ||
            (pos->second != tmp))
        {
            m_uniform4f[slot] = tmp;
            glVertexAttrib4fv(slot, (const float*)&tmp);
        }
    }
}

void            
AWGL2RenderState::setVertexArray(float* v)
{
}

void            
AWGL2RenderState::setVertexColorArray(unsigned char* v)
{
    
}

void            
AWGL2RenderState::setNormalArray(float* v)
{
    
}

void            
AWGL2RenderState::setTexCoordArray(float* v, int texUnit, int dim)
{
    
}
