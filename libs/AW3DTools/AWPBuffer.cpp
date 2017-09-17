#include "AWPBuffer.h"
#include <stdio.h>
#include "AWGLRenderer.h"


PFNWGLCREATEPBUFFERARBPROC      AWPBuffer::wglCreatePbufferARB     = NULL;
PFNWGLGETPBUFFERDCARBPROC       AWPBuffer::wglGetPbufferDCARB      = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC   AWPBuffer::wglReleasePbufferDCARB  = NULL;
PFNWGLDESTROYPBUFFERARBPROC     AWPBuffer::wglDestroyPbufferARB    = NULL;
PFNWGLQUERYPBUFFERARBPROC       AWPBuffer::wglQueryPbufferARB      = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC  AWPBuffer::wglChoosePixelFormatARB = NULL;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC AWPBuffer::wglGetPixelFormatAttribivARB = NULL;


AWPBuffer::AWPBuffer( int w, int h, unsigned int m_mode ) 
 : m_width(w), m_height(h), m_mode(m_mode), 
   m_DC(NULL), m_RC(NULL), m_buf(NULL), 
   m_lastDC(NULL), m_lastRC(NULL),
   m_depthBits(16), m_err(0)
{
}


AWPBuffer::~AWPBuffer()
{
   cleanup();
}//AWPBuffer::~AWPBuffer()



void                            
AWPBuffer::cleanup()
{
   if (m_buf)
   {
      wglDeleteContext(m_RC);
      wglReleasePbufferDCARB(m_buf, m_DC);
      wglDestroyPbufferARB(m_buf);
      m_buf = NULL;
      m_DC  = NULL;
      m_RC  = NULL;
      m_lastDC = NULL;
      m_lastRC = NULL;
   }//if (m_buf)
}//void AWPBuffer::cleanup()



// Check to see if the PBuffer was lost.
// If it was lost, destroy it and then recreate it.
void 
AWPBuffer::handleModeSwitch()
{
   int lost = 0;
   wglQueryPbufferARB(m_buf, WGL_PBUFFER_LOST_ARB, &lost);
   if (lost)
   {
      cleanup();
      init();
   }
}//void AWPBuffer::handleModeSwitch()



AWBoolean 
AWPBuffer::makeCurrent()
{
   m_lastDC = wglGetCurrentDC();
   m_lastRC = wglGetCurrentContext();
   if (!wglMakeCurrent(m_DC, m_RC))
   {
      fprintf( stderr, "AWPBuffer::makeCurrent() failed.\n" );
      return FALSE;
   }//if (!wglMakeCurrent(m_DC, m_RC))
   m_err = glGetError();
   return TRUE;
}//AWBoolean AWPBuffer::makeCurrent()


AWBoolean
AWPBuffer::restoreContext()
{
   AWBoolean retVal = TRUE;
   if (m_lastDC && m_lastRC)
   {
      retVal = wglMakeCurrent(m_lastDC, m_lastRC);
   }//if (m_lastDC && m_lastRC)
   m_lastDC = NULL;
   m_lastRC = NULL;
   m_err = glGetError();
   return retVal;
}//AWBoolean AWPBuffer::restoreContext()



AWBoolean
AWPBuffer::initFnPointers()
{
   wglCreatePbufferARB     = 
      (PFNWGLCREATEPBUFFERARBPROC)GLRenderer::getFunctionByName("wglCreatePbufferARB");
   if (!wglCreatePbufferARB) return FALSE;

   wglGetPbufferDCARB      = 
      (PFNWGLGETPBUFFERDCARBPROC)GLRenderer::getFunctionByName("wglGetPbufferDCARB");
   if (!wglGetPbufferDCARB) return FALSE;

   wglReleasePbufferDCARB  = 
      (PFNWGLRELEASEPBUFFERDCARBPROC)GLRenderer::getFunctionByName("wglReleasePbufferDCARB");
   if (!wglReleasePbufferDCARB) return FALSE;

   wglDestroyPbufferARB    = 
      (PFNWGLDESTROYPBUFFERARBPROC)GLRenderer::getFunctionByName("wglDestroyPbufferARB");
   if (!wglDestroyPbufferARB) return FALSE;

   wglQueryPbufferARB      = 
      (PFNWGLQUERYPBUFFERARBPROC)GLRenderer::getFunctionByName("wglQueryPbufferARB");
   if (!wglQueryPbufferARB) return FALSE;

   wglChoosePixelFormatARB = 
      (PFNWGLCHOOSEPIXELFORMATARBPROC)GLRenderer::getFunctionByName("wglChoosePixelFormatARB");
   if (!wglChoosePixelFormatARB) return FALSE;

   wglGetPixelFormatAttribivARB =
      (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)GLRenderer::getFunctionByName("wglGetPixelFormatAttribivARB");
   if (!wglGetPixelFormatAttribivARB) return FALSE;

   return TRUE;
}//AWBoolean AWPBuffer::initFnPointers()



// This function actually does the creation of the p-m_buf.
// It can only be called once a window has already been created.
AWBoolean
AWPBuffer::init(AWBoolean share)
{
   AWBoolean retVal  = FALSE;
   HDC hdc           = wglGetCurrentDC();
   HGLRC hglrc       = wglGetCurrentContext();

   // Query for a suitable pixel format based on the specified m_mode.
   int   iattributes[2*MAX_ATTRIBS];
   float fattributes[2*MAX_ATTRIBS];
   int   nfattribs = 0;
   int   niattribs = 0;

   // Attribute arrays must be "0" terminated - for simplicity, first
   // just zero-out the array entire, then fill from left to right.
   for ( int a = 0; a < 2*MAX_ATTRIBS; a++ )
   {
      iattributes[a] = 0;
      fattributes[a] = 0;
   }//for ( int a = 0; a < 2*MAX_ATTRIBS; a++ )

   // Since we are trying to create a AWPBuffer, the pixel format we
   // request (and subsequently use) must be "p-m_buf capable".
   iattributes[2*niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
   iattributes[2*niattribs+1] = true;
   niattribs++;

   //AWEngine used in RGBA situations only
   iattributes[2*niattribs  ] = WGL_PIXEL_TYPE_ARB;
   iattributes[2*niattribs+1] = WGL_TYPE_RGBA_ARB;
   niattribs++;

   if ( m_mode & AW_DOUBLE )
   {
      iattributes[2*niattribs  ] = WGL_DOUBLE_BUFFER_ARB;
      iattributes[2*niattribs+1] = true;
      niattribs++;
   }

   if ( m_mode & AW_DEPTH )
   {
      iattributes[2*niattribs  ] = WGL_DEPTH_BITS_ARB;
      iattributes[2*niattribs+1] = 1;
      niattribs++;
   }
   if ( m_mode & AW_STENCIL )
   {
      iattributes[2*niattribs  ] = WGL_STENCIL_BITS_ARB;
      iattributes[2*niattribs+1] = 1;
      niattribs++;
   }
   if ( m_mode & AW_ACCUM )
   {
      iattributes[2*niattribs  ] = WGL_ACCUM_BITS_ARB;
      iattributes[2*niattribs+1] = 1;
      niattribs++;
   }

   iattributes[2*niattribs  ] = WGL_SUPPORT_OPENGL_ARB;
   iattributes[2*niattribs+1] = true;
   niattribs++;

   int format;
   int pformat[MAX_PFORMATS];
   unsigned int nformats;
   if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
   {
      fprintf( stderr, "PBuffer creation error:  wglChoosePixelFormatARB() failed.\n" );
      return FALSE;
   }
   if ( nformats <= 0 )
   {
      fprintf( stderr, "PBuffer creation error:  Couldn't find a suitable pixel format.\n" );
      return FALSE;
   }
   format = pformat[0];

   #if 1
    fprintf( stderr, "nformats = %d\n\n", nformats );
    int values[MAX_ATTRIBS];
	int iatr[MAX_ATTRIBS] = { WGL_PIXEL_TYPE_ARB, WGL_COLOR_BITS_ARB,
		                      WGL_RED_BITS_ARB, WGL_GREEN_BITS_ARB, WGL_BLUE_BITS_ARB,
							  WGL_ALPHA_BITS_ARB, WGL_DEPTH_BITS_ARB, WGL_STENCIL_BITS_ARB, WGL_ACCUM_BITS_ARB,
							  WGL_DOUBLE_BUFFER_ARB, WGL_SUPPORT_OPENGL_ARB, WGL_ACCELERATION_ARB };
	int niatr = 12;
    for ( int j = 0; j < MAX_ATTRIBS; j++ )
        {
        values[j] = false;
        iattributes[j] = iattributes[2*j];
        }
    for ( unsigned int i = 0; i < nformats; i++ )
        {
        if ( !wglGetPixelFormatAttribivARB( hdc, pformat[i], 0, niatr, iatr, values ) )
            {
            fprintf( stderr, "pbuffer creation error:  wglGetPixelFormatAttribiv() failed\n" );
            exit( -1 );
            }
		fprintf( stderr, "%d. pformat = %d\n", i, pformat[i] );
		fprintf( stderr, "--------------------\n" );
        for ( int k = 0; k < niatr; k++ )
            {
            if ( iatr[k] == WGL_PIXEL_TYPE_ARB )
                {
                if ( values[k] == WGL_TYPE_COLORINDEX_ARB )
                    fprintf( stderr, " Pixel type = WGL_TYPE_COLORINDEX_ARB\n" );
                if ( values[k] == WGL_TYPE_RGBA_ARB )
                    fprintf( stderr, " Pixel type = WGL_TYPE_RGBA_ARB\n" );
                }
            if ( iatr[k] == WGL_COLOR_BITS_ARB )
                {
                fprintf( stderr, " Color bits = %d\n", values[k] );
                }
            if ( iatr[k] == WGL_RED_BITS_ARB )
                {
                fprintf( stderr, "      red         %d\n", values[k] );
                }
            if ( iatr[k] == WGL_GREEN_BITS_ARB )
                {
                fprintf( stderr, "      green       %d\n", values[k] );
                }
            if ( iatr[k] == WGL_BLUE_BITS_ARB )
                {
                fprintf( stderr, "      blue        %d\n", values[k] );
                }
            if ( iatr[k] == WGL_ALPHA_BITS_ARB )
                {
                fprintf( stderr, "      alpha       %d\n", values[k] );
                }
            if ( iatr[k] == WGL_DEPTH_BITS_ARB )
                {
                fprintf( stderr, " Depth bits =   %d\n", values[k] );
                }
            if ( iatr[k] == WGL_STENCIL_BITS_ARB )
                {
                fprintf( stderr, " Stencil bits = %d\n", values[k] );
                }
            if ( iatr[k] == WGL_ACCUM_BITS_ARB )
                {
                fprintf( stderr, " Accum bits =   %d\n", values[k] );
                }
            if ( iatr[k] == WGL_DOUBLE_BUFFER_ARB )
                {
                fprintf( stderr, " Double Buffer  = %d\n", values[k] );
                }
            if ( iatr[k] == WGL_SUPPORT_OPENGL_ARB )
                {
                fprintf( stderr, " Support OpenGL = %d\n", values[k] );
                }
            if ( iatr[k] == WGL_ACCELERATION_ARB )
                {
				if ( values[k] == WGL_FULL_ACCELERATION_ARB )
					fprintf( stderr, " Acceleration   = WGL_FULL_ACCELERATION_ARB\n" );
				if ( values[k] == WGL_GENERIC_ACCELERATION_ARB )
					fprintf( stderr, " Acceleration   = WGL_GENERIC_ACCELERATION_ARB\n" );
                }
            }
        fprintf( stderr, "\n" );
        }
    fprintf( stderr, "selected pformat = %d\n", format );
#endif

   // Create the p-m_buf.
   iattributes[0] = 0;
   m_buf = wglCreatePbufferARB( hdc, format, m_width, m_height, iattributes );
   if (!m_buf)
   {
      DWORD err = GetLastError();
      fprintf( stderr, "AWPBuffer creation error:  wglCreateAWPBufferARB() failed\n" );
      if ( err == ERROR_INVALID_PIXEL_FORMAT )
      {
         fprintf( stderr, "error:  ERROR_INVALID_PIXEL_FORMAT\n" );
      }
      else if ( err == ERROR_NO_SYSTEM_RESOURCES )
      {
         fprintf( stderr, "error:  ERROR_NO_SYSTEM_RESOURCES\n" );
      }
      else if ( err == ERROR_INVALID_DATA )
      {
         fprintf( stderr, "error:  ERROR_INVALID_DATA\n" );
      }
      return FALSE;
   }//if (!m_buf)

   // Get the device context.
   m_DC = wglGetPbufferDCARB( m_buf );
   if ( !m_DC )
   {
      fprintf( stderr, "AWPBuffer creation error:  wglGetAWPBufferDCARB() failed\n" );
      return FALSE;
   }

   // Create a gl context for the p-m_buf.
   m_RC = wglCreateContext( m_DC );
   if ( !m_RC )
   {
      fprintf( stderr, "AWPBuffer creation error:  wglCreateContext() failed\n" );
      return FALSE;
   }

   if( share )
   {
      if( !wglShareLists(hglrc, m_RC) )
      {
         fprintf( stderr, "AWPBuffer: wglShareLists() failed\n" );
         return FALSE;
      }
   }
   m_err = glGetError();
   // Determine the actual m_width and m_height we were able to create.
   wglQueryPbufferARB( m_buf, WGL_PBUFFER_WIDTH_ARB, &m_width );
   wglQueryPbufferARB( m_buf, WGL_PBUFFER_HEIGHT_ARB, &m_height );

   AWTRACE2("Created a %d x %d PBuffer\n", m_width, m_height );
   //now find the depth information
   makeCurrent();
   glEnable(GL_DEPTH_TEST);
   glGetIntegerv(GL_DEPTH_BITS, &m_depthBits);
   restoreContext();
   m_err = glGetError();
   return TRUE;
}//AWBoolean AWPBuffer::init(AWBoolean share)
