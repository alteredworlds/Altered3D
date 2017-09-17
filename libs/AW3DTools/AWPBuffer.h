#ifndef _AW_PBUFFER_H
#define _AW_PBUFFER_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "GL/wglext.h"
#include "AWToolsCommon.h"

#define AW_SINGLE      0
#define AW_DOUBLE      2     /* GLUT_DOUBLE  */ 
#define AW_DEPTH      16     /* GLUT_DEPTH   */
#define AW_STENCIL    32     /* GLUT_STENCIL */ 
#define AW_ACCUM       4     /* GLUT_ACCUM   */ 
 

#define MAX_PFORMATS 256
#define MAX_ATTRIBS  32

class AWPBuffer
{
public:
    AWPBuffer(int width, int height, unsigned int mode);
    ~AWPBuffer();

    void                            cleanup();
    void                            handleModeSwitch();
    AWBoolean                       makeCurrent();
    AWBoolean                       restoreContext();
    AWBoolean                       init(AWBoolean share = FALSE);

    static AWBoolean                initFnPointers();
    int                             getDepthBits() {return m_depthBits;}

#ifdef _WIN32
    static PFNWGLCREATEPBUFFERARBPROC      wglCreatePbufferARB;
    static PFNWGLGETPBUFFERDCARBPROC       wglGetPbufferDCARB;
    static PFNWGLRELEASEPBUFFERDCARBPROC   wglReleasePbufferDCARB;
    static PFNWGLDESTROYPBUFFERARBPROC     wglDestroyPbufferARB;
    static PFNWGLQUERYPBUFFERARBPROC       wglQueryPbufferARB;
    static PFNWGLCHOOSEPIXELFORMATARBPROC  wglChoosePixelFormatARB;
    static PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
#endif

private:
    HDC                             m_DC;     // Handle to a device context.
    HGLRC                           m_RC;     // Handle to a GL context.
    HDC                             m_lastDC;     // Handle to a device context.
    HGLRC                           m_lastRC;     // Handle to a GL context.
    HPBUFFERARB                     m_buf;    // Handle to a pbuffer.
    int                             m_width;
    int                             m_height;
    unsigned int                    m_mode;  //Flags indicating the type of pbuffer.
    int                             m_depthBits;
    int                             m_err;
};//class AWPBuffer


#endif //sentinel _AW_PBUFFER_H