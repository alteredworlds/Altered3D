#include "AWToolsCommon.h"
#include <math.h>
#include <time.h>

#include "AWGLRenderer.h"
#include "AWFlightController.h"
#include "AWFace.h"
#include "AWLight.h"
#include "AWMaterial.h"
#include <GL/glext.h>
#include "AWVertexProgram.h"
#include "AWPBuffer.h"

#ifndef _WIN32                                     
#include <dlfcn.h>
#endif
#define FRAME_STEP (1.0f / 24.0f)

AWBoolean msgOut = FALSE;

//#include "GL/glut.h"


//globals for OpenGL extensions

//NOTE some gl.h now define ARB multitex as part of OpenGL
//this has been disabled for the current header GL/gl.h , as Windows
//is falling behind
PFNGLLOCKARRAYSEXTPROC           glLockArrays             = NULL;
PFNGLUNLOCKARRAYSEXTPROC         glUnlockArrays           = NULL;
PFNGLMULTITEXCOORD2FVARBPROC     glMultiTexCoord2fv    = NULL;
PFNGLMULTITEXCOORD3FVARBPROC     glMultiTexCoord3fv    = NULL;
PFNGLACTIVETEXTUREARBPROC        glActiveTexture       = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTexture = NULL;
PFNGLBLENDCOLOREXTPROC           glBlendColorEXTAW       = NULL;

#ifdef _WIN32
// NV_register_combiners command function pointers 
PFNGLCOMBINERPARAMETERFVNVPROC				glCombinerParameterfvNV;
PFNGLCOMBINERPARAMETERIVNVPROC				glCombinerParameterivNV;
PFNGLCOMBINERPARAMETERFNVPROC				glCombinerParameterfNV;
PFNGLCOMBINERPARAMETERINVPROC				glCombinerParameteriNV;
PFNGLCOMBINERINPUTNVPROC					glCombinerInputNV;
PFNGLCOMBINEROUTPUTNVPROC					glCombinerOutputNV;
PFNGLFINALCOMBINERINPUTNVPROC				glFinalCombinerInputNV;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC		glGetCombinerInputParameterfvNV;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC		glGetCombinerInputParameterivNV;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC		glGetCombinerOutputParameterfvNV;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC		glGetCombinerOutputParameterivNV;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputfvNV;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputivNV;
#endif

//AWVertexProgram prog1;

/*
PFNGLPOINTPARAMETERFEXTPROC			glPointParameterfEXT = NULL;
PFNGLPOINTPARAMETERFVEXTPROC		glPointParameterfvEXT = NULL;
*/

//This function has been optimized. Lighting not takes place in local space.. so no more conversions to world space!
const char SpecularCubeMapShader[]=
{
"!!VP1.0"

//Convert Vertex position into clip space, using combined matrix. (modelview and projection)
"DP4 o[HPOS].x,v[OPOS],c[4];\
 DP4 o[HPOS].y,v[OPOS],c[5];\
 DP4 o[HPOS].z,v[OPOS],c[6];\
 DP4 o[HPOS].w,v[OPOS],c[7];"

//Get unit length eye vector to vertex.
"ADD R2,c[10],-R0;\
 DP3 R2.w,R2,R2;\
 RSQ R2.w,R2.w;\
 MUL R2,R2,R2.w;"

//Convert the normal with the inverse transpose modelview matrix, and normalize.
"DP3 R3.x,v[NRML],c[12];\
 DP3 R3.y,v[NRML],c[13];\
 DP3 R3.z,v[NRML],c[14];\
 DP3 R3.w,R3,R3;\
 RSQ R3.w,R3.w;\
 MUL R3,R3,R3.w;"

//Calculate cube mapping section. 
"DP3 R5,R2,R3;\
 ADD R5,R5,R5;\
 MAD o[TEX0].xyz,-R3,R5,R2;"

//Create vector from vertex to light position, and normalize. Light position here is in local space!!
"ADD R3,c[19],-v[OPOS];\
 DP3 R3.w,R3,R3;\
 RSQ R3.w,R3.w;\
 MUL R3,R3,R3.w;"

//Dot product normal, and light vector, multiply by color, add on ambient, and store.
"DP3 R4.x,R3,v[NRML];\
 MAD o[COL0],v[COL0],R4.x,c[17];"

//Multiply dot product of light vector and normal, by 0.5, to give direct facing reading of 0.5
//Store these in x and Y component of 2nd texture unit.
"MUL o[TEX1].xy,R4.x,c[9].x;\
 END"
};


//GLRenderer*		GLRenderer::m_staticThis = NULL;
int				GLRenderer::m_bpp = 16; //default is 16 bits per pixel
AWBoolean      GLRenderer::m_useAlphaTest = TRUE;

/*
GLRenderer&
GLRenderer::getRenderer()
{
   if (!m_staticThis)
   {
      m_staticThis = new GLRenderer();
      if (!m_staticThis) throw; //big problem...
   }//if (!m_staticThis)
   return *m_staticThis;
}//GLRenderer* GLRenderer::getRenderer()


void          
GLRenderer::destroyRenderer()
{
   delete m_staticThis;
   m_staticThis = NULL;
}//void GLRenderer::destroyRenderer()
*/


static GLenum faceMap[6] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB  
};


#ifndef _WIN32
static void* libHandle = NULL;
#endif







GLRenderer::GLRenderer()
	:  m_switchLevel(FALSE),
      m_levelFailed(FALSE),
      m_timeToExplode(FALSE),
      m_usingDynLights(FALSE),
      m_selectedMaterial(NULL),
      m_useVertArrays(FALSE),
      m_useDisplayLists(FALSE),
      m_useCVAs(FALSE),
      m_useBump(FALSE),
      m_useCubeEnv(FALSE),
      m_useS3Compression(FALSE),
      m_captureBuf(NULL),
      m_captureFrame(0),
      m_time(0),
      m_fontListBase(AW_NOLIST),
      m_sceneListBase(AW_NOLIST),
      m_trisThisFrame(0),
      m_loop(FALSE),
      m_showStats(FALSE),
      m_pauseTime(0.0f),
      m_paused(FALSE),
      m_applyRageProHack(FALSE),
      m_drawNormals(FALSE),
      m_showTextures(TRUE),
      m_glErr(0),
      m_hasDisplayLists(TRUE),
      m_hasBlendColor(FALSE),
      m_hasDestAlpha(FALSE),
      m_hasSeparateSpecular(FALSE),
      m_hasTexEnvCombine(FALSE),
      m_normalizeCube(NULL),
      m_helperCam(NULL),
      m_projHelper(NULL),
      m_hasVertexPrograms(FALSE),
      m_hasNVRegCombine(FALSE),
      m_hasCubeEnv(FALSE),
      m_hasDot3Combine(FALSE),
      m_hasS3Compression(FALSE),
      m_applyObjectTextures(TRUE),
      m_hasTextureShaders(FALSE),
      m_shadowBuffer(NULL),
      m_scene(NULL)
{
	//communication hack...
   m_useAlphaTest = TRUE;
   //DEBUG CODE FOR NVIDIA TEST APP
   m_transformReflections = TRUE;
}



GLRenderer::~GLRenderer()
{
   if (m_normalizeCube)
   {
      delete m_normalizeCube;
      m_normalizeCube = NULL;
   }
}//GLRenderer::~GLRenderer()


//used to reset data in saver when new scene is loaded
//we keep the same HWND & Window, but kill the old GL Context
//and re-init a new one
void
GLRenderer::resetData()
{
   m_glErr = 0;
   m_switchLevel = FALSE;
   m_levelFailed = FALSE;
   m_timeToExplode = FALSE;
   m_usingDynLights = FALSE;
   m_useAlphaTest = TRUE;
   m_selectedMaterial = NULL;
   m_captureBuf = NULL;
   m_captureFrame = 0;
   m_time = 0;
   m_fontListBase = AW_NOLIST;
   m_sceneListBase = AW_NOLIST;
   m_trisThisFrame = 0;
   m_loop = FALSE;
   m_showStats = FALSE;
   m_pauseTime = 0.0f;
   m_paused = FALSE;
   m_applyRageProHack = FALSE;
   m_transformReflections = TRUE;
   m_drawNormals = FALSE;
   m_showTextures = TRUE;
   m_applyObjectTextures = TRUE;
   m_normalizeCube = NULL;  
}//void GLRenderer::resetData()



void
GLRenderer::makeCurrent(void* hdc, void* hrc)
{
#ifdef _WIN32
#ifdef __AWTRACE
   AWTRACE2("currentdc %d   current rc %d\n", (int)wglGetCurrentDC(), (int)wglGetCurrentContext());
   if (!wglMakeCurrent((HDC)hdc, (HGLRC)hrc))
   {
      AWTRACE1("wglMakeCurrent Failed - last error is %d\n", (int)GetLastError());
   }
   int err = glGetError();
   if (err)
   {
      AWTRACE1("glGetError is %d\n", err);
   }
#endif  
   wglMakeCurrent((HDC)hdc, (HGLRC)hrc);
#endif
}//void GLRenderer::makeCurrent(void* hdc, void* hrc)



AWBoolean
GLRenderer::setupPixelFormat(void* vhdc, int bpp)
{
#ifdef _WIN32
   HDC hdc = (HDC)vhdc;
   PIXELFORMATDESCRIPTOR pfd, *ppfd;
   int pixelformat;
   m_bpp = bpp;
   int zBits = (16 == m_bpp) ? 16 : 24;	//for TNT
   int aBits = 0;//(16 == m_bpp) ? 0  : 16;	//for TNT
   ppfd = &pfd;

   memset(ppfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
   ppfd->nVersion = 1;
   ppfd->dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
   ppfd->dwLayerMask = PFD_MAIN_PLANE;
   ppfd->iPixelType = PFD_TYPE_RGBA;
   ppfd->cColorBits = bpp;
   ppfd->cDepthBits = zBits;
   ppfd->cAccumBits = 0;
   ppfd->cStencilBits = 0;
   ppfd->cAlphaBits  = aBits;

   pixelformat = ChoosePixelFormat(hdc, ppfd);
   if ((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0) {
      AWTRACE("Error: ChoosePixelFormat failed\n");
      return FALSE;
   }

   if (pfd.dwFlags & PFD_NEED_PALETTE) {
      AWTRACE("Error: Needs palette\n");
      return FALSE;
   }

   if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) {
      AWTRACE("Error: SetPixelFormat failed\n");
      return FALSE;
   }

   //obtain detailed information about // the device context's pixel format
   //for debugging purposes only
   memset(ppfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), ppfd);

#endif //#ifdef _WIN32
    return TRUE;
}//AWBoolean GLRenderer::setupPixelFormat(HDC hdc, int bpp)



void*
GLRenderer::createContextAndSelect(void* hdc)
{ //void* 'cos of ActiveX control
#ifdef _WIN32
   void* ghRC = (void*)wglCreateContext((HDC)hdc);
   wglMakeCurrent((HDC)hdc, (HGLRC)ghRC);
   return ghRC;
#endif
}//void* GLRenderer::createContextAndSelect(void* hdc)



void*
GLRenderer::destroyContext(void* hrc)
{
#ifdef _WIN32
   AWTRACE3("delete rc %d   with currentdc %d   current rc %d\n", (int)hrc, (int)wglGetCurrentDC(), (int)wglGetCurrentContext());
   if (hrc==wglGetCurrentContext())
   {
      //this context is selected - must unselect
      AWTRACE("  wglMakeCurrent(NULL,  NULL)\n");
      wglMakeCurrent(NULL,  NULL);
   }//if (hrc==wglGetCurrentContext())
   if (hrc)
      wglDeleteContext((HGLRC)hrc);
   hrc = NULL;
   return hrc;
#endif
}//void* GLRenderer::destroyContext(void* hrc)



void*
GLRenderer::getFunctionByName(char* n)
{
   void* retVal = NULL;
#ifdef _WIN32
   retVal   = (void*)wglGetProcAddress(n);
#else
   retVal   = (void*)dlsym(libHandle, n);
#endif
   return retVal;
}//void* GLRenderer::getFunctionByName(char* n)



void
GLRenderer::getExtensions()
{
#ifndef _WIN32
   if (!libHandle) libHandle = dlopen(NULL, RTLD_LAZY);
#endif
   if (isExtensionSupported("GL_EXT_compiled_vertex_array"))
   {
      glLockArrays   = (PFNGLLOCKARRAYSEXTPROC)getFunctionByName("glLockArraysEXT");
      glUnlockArrays = (PFNGLUNLOCKARRAYSEXTPROC)getFunctionByName("glUnlockArraysEXT");
   }//if (isExtensionSupported("GL_EXT_compiled_vertex_array"))
   AWTRACE1("  GL_EXT_compiled_vertex_array supported:\t%i\n", (int)(NULL!=glLockArrays));

   if (isExtensionSupported("GL_EXT_blend_color"))
   {
      glBlendColorEXTAW = (PFNGLBLENDCOLOREXTPROC)getFunctionByName("glBlendColorEXT");
   }
   AWTRACE1("  GL_EXT_blend_color supported:\t%i\n", (int)(NULL!=glBlendColorEXTAW));

   m_hasS3Compression = isExtensionSupported("GL_EXT_texture_compression_s3tc");
   AWTRACE1("  GL_EXT_texture_compression_s3tc supported:\t%i\n", (int)m_hasS3Compression);
   m_useS3Compression &= m_hasS3Compression;

   m_hasAutoMipmap    = isExtensionSupported("GL_SGIS_generate_mipmap");
   AWTRACE1("  SGIS_generate_mipmap supported:\t%i\n", (int)m_hasAutoMipmap);

   m_hasCubeEnv = isExtensionSupported("GL_EXT_texture_cube_map")||isExtensionSupported("GL_ARB_texture_cube_map");
   AWTRACE1("  GL_EXT_texture_cube_map supported:\t%i ", (int)m_hasCubeEnv);
   setHasCubeMap(getUseCubeEnv() && m_hasCubeEnv);
   if (!getHasCubeMap())
   {  //sphere mapping doesn't work doing this...
      //it's not view dependent, in fact - correct from one view pos only.
      m_transformReflections = FALSE;
      AWTRACE("but has been DISABLED");
   }//if (!getHasCubeMap())
   AWTRACE("\n");

   int num;
   glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &num);
   setNumTexUnits(num);
   AWTRACE1("  Number of texture units:\t%i\n", (int)num);

   m_hasTexEnvCombine = isExtensionSupported("GL_EXT_texture_env_combine");
   AWTRACE1("  GL_EXT_texture_env_combine supported:\t%i\n", (int)m_hasTexEnvCombine);

   m_hasDot3Combine = isExtensionSupported("GL_ARB_texture_env_dot3") ||
      isExtensionSupported("GL_EXT_texture_env_dot3");

   if (isExtensionSupported("GL_ARB_multitexture"))
   {
      glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVARBPROC)getFunctionByName("glMultiTexCoord2fvARB");
      glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVARBPROC)getFunctionByName("glMultiTexCoord3fvARB");
      glActiveTexture    = (PFNGLACTIVETEXTUREARBPROC)getFunctionByName("glActiveTextureARB");
      glClientActiveTexture =
         (PFNGLCLIENTACTIVETEXTUREARBPROC)getFunctionByName("glClientActiveTextureARB");
   }//if (isExtensionSupported("GL_ARB_multitexture"))
   AWTRACE1("  GL_ARB_multitexture supported:\t%i\n", (NULL!=glMultiTexCoord2fv));

   //support for separate specular colour?
   m_hasSeparateSpecular = isExtensionSupported("GL_EXT_separate_specular_color");
   AWTRACE1("  GL_EXT_separate_specular_color supported:\t%i\n", (int)m_hasSeparateSpecular);
   //NVIDIA specific Vertex Program support
   if (isExtensionSupported("GL_NV_vertex_program"))
   {
      m_hasVertexPrograms = TRUE;
      AWVertexProgramBase::initFnPointers(); 
   }//if (isExtensionSupported("GL_NV_vertex_program"))
   if (isExtensionSupported("GL_NV_texture_shader"))
   {
      m_hasTextureShaders = TRUE;
   }//if (isExtensionSupported("GL_NV_texture_shader"))

   //if (isExtensionSupported("WGL_ARB_pbuffer"))
   //{
   m_hasPBuffer =  AWPBuffer::initFnPointers(); 
   //}//if (isExtensionSupported("WGL_ARB_pbuffer"))

   m_hasSGIShadowMaps = isExtensionSupported("GL_SGIX_shadow");


   AWTRACE1("  GL_NV_vertex_program supported:\t%i\n", (int)m_hasVertexPrograms);
   //NVIDIA specific REGISTER COMBINER SUPPORT
   #ifdef _WIN32
   m_hasNVRegCombine = isExtensionSupported("GL_NV_register_combiners");
   AWTRACE1("  GL_NV_register_combiners supported:\t%i ", (int)m_hasNVRegCombine);
   if (getUseBump() && m_hasNVRegCombine)
   {     
	   glCombinerParameterfvNV = 
         (PFNGLCOMBINERPARAMETERFVNVPROC)getFunctionByName("glCombinerParameterfvNV");
      glCombinerParameterivNV =
         (PFNGLCOMBINERPARAMETERIVNVPROC)getFunctionByName("glCombinerParameterivNV");
      glCombinerParameterfNV =
         (PFNGLCOMBINERPARAMETERFNVPROC)getFunctionByName("glCombinerParameterfNV");
      glCombinerParameteriNV =
         (PFNGLCOMBINERPARAMETERINVPROC)getFunctionByName("glCombinerParameteriNV");
      glCombinerInputNV =
         (PFNGLCOMBINERINPUTNVPROC)getFunctionByName("glCombinerInputNV");
      glCombinerOutputNV =
         (PFNGLCOMBINEROUTPUTNVPROC)getFunctionByName("glCombinerOutputNV");
      glFinalCombinerInputNV =
         (PFNGLFINALCOMBINERINPUTNVPROC)getFunctionByName("glFinalCombinerInputNV");
      glGetCombinerInputParameterfvNV=
         (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)getFunctionByName("glGetCombinerInputParameterfvNV");
      glGetCombinerInputParameterivNV =
         (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)getFunctionByName("glGetCombinerInputParameterivNV");
      glGetCombinerOutputParameterfvNV=
         (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)getFunctionByName("glGetCombinerOutputParameterfvNV");
      glGetCombinerOutputParameterivNV=
         (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)getFunctionByName("glGetCombinerOutputParameterivNV");
      glGetFinalCombinerInputfvNV=
         (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)getFunctionByName("glGetFinalCombinerInputfvNV");
      glGetFinalCombinerInputivNV=
         (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)getFunctionByName("glGetFinalCombinerInputivNV");
   }//if (isExtensionSupported("GL_NV_register_combiners"))
   else
   {
      AWTRACE("but has been DISABLED");
      //m_hasNVRegCombine = FALSE;
   }
   #endif
   AWTRACE("\n");
   //check to see if DESTINATION ALPHA supported
   GLint alphaBits;
   glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
   m_hasDestAlpha = (alphaBits > 0);
   AWTRACE1("  Destination ALPHA supported:\t%i\n", (int)m_hasDestAlpha);

#ifndef _WIN32
   if (libHandle)
   {
      dlclose(libHandle);
      libHandle = NULL;
   }//if (libHandle)
#endif
}//void GLRenderer::getExtensions()



//following fn. is Mark Kilgard code...
int
GLRenderer::isExtensionSupported(const char *extension)
{
   const GLubyte *extensions = NULL;
   const GLubyte *start;
   GLubyte *where, *terminator;

   /* Extension names should not have spaces. */
   where = (GLubyte *) strchr(extension, ' ');
   if (where || *extension == '\0')
      return 0;

   extensions = glGetString(GL_EXTENSIONS);

   // It takes a bit of care to be fool-proof about parsing the
   // OpenGL extensions string.  Don't be fooled by sub-strings,
   // etc.
   start = extensions;
   for (;;)
   {
      where = (GLubyte*)strstr((const char*)start, extension);
      if (!where)
         break;
      terminator = where + strlen(extension);
      if (where == start || *(where - 1) == ' ')
         if (*terminator == ' ' || *terminator == '\0')
            return 1;
      start = terminator;
   }//for (;;)
   return 0;
}//int GLRenderer::isExtensionSupported(const char *extension)



void
GLRenderer::initialize(int width, int height)
{
   m_width = width;
   m_height= height;
   initializeGL();
   initializeObjects();
   m_perfCntr.zeroTime();
   getScene().m_nodeList.resetTimeBar(0.0f);
   m_pauseTime=0.0f;

   m_loop = (AWBoolean)getScene().getLoopTime(); //zero == FALSE for no loopign
   m_time = 0.0f;
}//void GLRenderer::initialize(int width, int height)



void
GLRenderer::makeRasterFont()
{
#ifdef _WIN32
   SetLastError(NO_ERROR);
   HDC curDC = wglGetCurrentDC();
   if (NULL == curDC) return;
   // make the system font the device context's selected font
   HFONT old = (HFONT)SelectObject(curDC, GetStockObject(SYSTEM_FONT));
   //find a suitable starting point for the font display lists
   m_fontListBase = 1;
   while (m_hasDisplayLists && glIsList(m_fontListBase))
   {
      m_fontListBase++;
   }
   //wglUseFontBitmaps: defines n display lists in the current context, starting 
   //at m_fontListBase. Each display list consists of a single call to glBitmap. 
   //
   //glBitmap: fragments are generated for each pixel corresponding to a 1 in 
   //the bitmap image. These fragments are generated using the current raster 
   //z-coordinate, color or color index, and current raster texture coordinates. 
   //They are then treated just as if they had been generated by a point, line, 
   //or polygon, including texture mapping, fogging, and all per-fragment 
   //operations such as alpha and depth testing.
   wglUseFontBitmaps(curDC, 0, 255, m_fontListBase);
   DWORD err = GetLastError();
   if ((NO_ERROR != err) && (ERROR_INVALID_PARAMETER != err))
   {
      m_fontListBase =AW_NOLIST;
   }
   SelectObject(curDC, old);
#endif
}//void GLRenderer::makeRasterFont()



void
GLRenderer::cleanupFont()
{//for now we create in awgame and so also destroy there
   if (AW_NOLIST != m_fontListBase)
   {
      glDeleteLists(m_fontListBase, 256);
      m_fontListBase=AW_NOLIST;
   }//if (m_fontListBase)
}//void GLRenderer::cleanupFont()



void
GLRenderer::outputString(const char* s)
{
   if (AW_NOLIST != m_fontListBase)
   {
      glListBase(m_fontListBase);
      glCallLists((GLsizei)strlen(s), GL_UNSIGNED_BYTE, (GLubyte*)s);
      if (AW_NOLIST != m_sceneListBase)
      {
         glListBase(m_sceneListBase);
      }//if (AW_NOLIST != m_sceneListBase)
   }//if (m_fontListBase)
}//void GLRenderer::outputString(const char* s)



void
GLRenderer::captureScreen()
{
	if (m_width && m_height && m_captureBuf)
	{
		//first read the pixel data into a suitable buffer, taken at full size
		glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_captureBuf);
		//now save this data to the requested file name
		char numStr[20];
		sprintf(numStr, "scrn%04i", m_captureFrame++);
      AWCString useFileName(m_captureDir);
      useFileName += numStr;
		AWImageProcessor::saveRGB(useFileName,
						   m_captureBuf, m_width, m_height,
						   m_imageOutFmt);

	}//if (m_width && m_height)
}//void GLRenderer::captureScreen()



void
GLRenderer::startCapture(const AWCString& dirName, int fmt)
{
	if (NULL == m_captureBuf)
	{
		m_captureBuf = new BYTE[m_width * m_height * 3];
	}
	m_captureDir = dirName;
	m_captureFrame = 0;
	m_imageOutFmt = fmt;
}//void GLRenderer::startCapture()



void
GLRenderer::stopCapture()
{
	delete [] m_captureBuf;
	m_captureBuf = NULL;
	m_captureFrame=0;
}//void GLRenderer::stopCapture()



void
GLRenderer::toggleFog()
{
	AWBoolean bVal = !getScene().getUseFog();
	getScene().setUseFog(bVal);
	setFogParams();
}


void
GLRenderer::useFog(int fogVal)
{	//make sure we really need to change state
	if (fogVal != getScene().getUseFog())
	{	//desired state is different to the current state, so change
		getScene().setUseFog(fogVal);
		setFogParams();
	}
}//void GLRenderer::useFog(int fogVal)



void
GLRenderer::setFogParams()
{	//only called if a state change is required and on initialisation
   //apply the scene background colour unless fog is being used.
   getScene().getBackground(*(AWPoint*)m_background);
   m_background[3]=0.0f;
   //m_background[0]=0.0f;m_background[1]=0.0f;m_background[2]=0.0f;m_background[3]=0.0f;
	if (-1 != getScene().getUseFog())
	{
		AWPoint awcol = getScene().getFogColour();
		m_background[0]=awcol.x;m_background[1]=awcol.y;
      m_background[2]=awcol.z;m_background[3]=1.0f;
		glEnable(GL_FOG);
      AWTRACE4("Enable GL_FOG colour: (%5f, %5f, %5f, %5f)\n", (double)m_background[0], (double)m_background[1], (double)m_background[2], (double)m_background[3]);
		//glHint(GL_FOG_HINT, GL_FASTEST);
		switch (getScene().getFogType())
		{
		case 1:
			glFogi(GL_FOG_MODE, GL_EXP);
			glFogf(GL_FOG_DENSITY, getScene().getFogStart());
			break;
		case 2:
			glFogi(GL_FOG_MODE, GL_EXP2);
			glFogf(GL_FOG_DENSITY, getScene().getFogStart());
			break;
		default:
			glFogi(GL_FOG_MODE, GL_LINEAR);
			glFogf(GL_FOG_START, getScene().getFogStart());
			glFogf(GL_FOG_END,   getScene().getFogEnd());
			break;
		}//switch
		glFogfv(GL_FOG_COLOR, m_background);
	}
	else
	{	//back to the default clear colour?
      AWTRACE("Disable GL_FOG\n");
		glDisable(GL_FOG);
	}
   AWTRACE4("Setting clear colour: (%5f, %5f, %5f, %5f)\n", (double)m_background[0], (double)m_background[1], (double)m_background[2], (double)m_background[3]);
	glClearColor(m_background[0], m_background[1], m_background[2], m_background[3]);
}//void GLRenderer::setFogParams()



void
GLRenderer::backCamera()
{
	if (getScene().m_nodeList.getNumCameras())
	{
      getScene().m_nodeList.setViewRot(0.0f, 0.0f);
		int camIndex = getScene().m_nodeList.findCamera(getScene().m_nodeList.getCurCamera());
		camIndex--;
		camIndex = abs(camIndex % getScene().m_nodeList.getNumCameras());
		getScene().m_nodeList.setCurCamera(getScene().m_nodeList.getCamera(camIndex));
		selectCamera();
	}
}//void GLRenderer::backCamera()



void
GLRenderer::forwardCamera()
{
	if (getScene().m_nodeList.getNumCameras())
	{
      getScene().m_nodeList.setViewRot(0.0f, 0.0f);
		int camIndex = getScene().m_nodeList.findCamera(getScene().m_nodeList.getCurCamera());
		camIndex++;
		camIndex %= getScene().m_nodeList.getNumCameras();
		getScene().m_nodeList.setCurCamera(getScene().m_nodeList.getCamera(camIndex));
		selectCamera();
	}
}//void GLRenderer::forwardCamera()



AWBoolean
GLRenderer::selectCamera(AWCamera* cam)
{  //incoming camera may be NULL, which means we should
   //use the current scene camera
   AWCamera* curCam = cam ? cam : getScene().m_nodeList.getCurCamera();
	if (curCam)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
        double ymax = curCam->m_nearClip * tan( curCam->m_fov * M_PI / 360.0);
        //double xmin = ymin * m_aspect;
        double xmax = ymax * m_aspect;
		glFrustum(-xmax, xmax, -ymax, ymax, curCam->m_nearClip, curCam->m_farClip);
		glMatrixMode(GL_MODELVIEW);
	}//if (curCam)
	return (NULL != curCam);
}//AWBoolean GLRenderer::selectCamera(AWCamera* cam)



AWBoolean
GLRenderer::initCamera()
{
	if (m_width && m_height)
		m_aspect = (GLfloat)m_width/(GLfloat)m_height;
	return selectCamera();
}//AWBoolean GLRenderer::initCamera()



void
GLRenderer::initializeGL()
{
   AWBoolean useAlphaTestThisScene = m_useAlphaTest;
   useAlphaTestThisScene &= !(m_applyRageProHack && getScene().m_fogList.getNum());
   getScene().m_nodeList.setUseAlphaTest(useAlphaTestThisScene);

   m_driverVendorName = (char*)glGetString(GL_VENDOR);
   AWTRACE1("\n\nOpenGL Vendor: %s  ", m_driverVendorName.c_str());
   AWTRACE1("Version %s\n", glGetString(GL_VERSION));
   AWTRACE1("Supported Extensions: %s\n\n\n", glGetString(GL_EXTENSIONS));
   getExtensions();
   m_glErr = glGetError();
   resetToDefaults();
   //clear colour set as part of setFogParams
   glClearDepth(1.0);
   glShadeModel(GL_SMOOTH);

   //ambient used in gl lighting and in bump mapping
	if (getScene().m_nodeList.getNumLights())
	{
      getScene().getAmbient(*(AWPoint*)m_ambient);
      m_ambient[3]= 1.0f;
		m_usingDynLights = TRUE;
      AWTRACE("Enable GL_LIGHTING\n");
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		//initialise the scene's global ambient light settings
      AWTRACE3(" supplying Ambient (%f, %f, %f)\n", (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambient);
		//now initialise any individual light sources
		initLights();
	}//if (getScene().m_lightList.entries())
   
   setDepthFunc(GL_LEQUAL);
   enableDepthTest(TRUE);
   m_glErr = glGetError();
   //not strictly necessary here - called anyway as part of inittextures
   if (getScene().m_shadowers.getNum())
   {  //init the shadow buffer here - use diemnsions of 
      //FIRST SHADOWER
      AWLight* curProj = getScene().m_shadowers[0];
      if (curProj && curProj->m_shadow)
      {  
         getShadowBuffer(curProj->m_shadow->m_width);
      }//if (curProj && curProj->m_shadow)
   }//if (getScene().m_shadowers.getNum())

   initTextures();//calls glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   if (m_hasSeparateSpecular)
   {  //default value is OFF since this effect looks really bad for some objects
      glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR_EXT);
   }//if (m_hasSeparateSpecular)

   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);

   initCamera();
   
   glLineWidth(getLineWidth());
   //if fog is enabled, the clear colour will be changed to match the fog colour
   //otherwise the scene value will be applied (gl default for older scenes)
   setFogParams();
   m_glErr = glGetError();

   //prog1.bind();
   //prog1.load(SpecularCubeMapShader);
   //prog1.enable();
}//void GLRenderer::initializeGL()


void           
GLRenderer::togglePolygonMode()
{
   int polyMode;;
   switch (getPolygonMode())
   {   
   case GL_FILL:
      polyMode  = GL_LINE;
      break;
   default:
   case GL_POINT:
   case GL_LINE:
      polyMode  = GL_FILL;
      break;  
   }//switch (m_polyMode)
   setPolygonMode(polyMode);
}//void GLRenderer::togglePolygonMode()



void
GLRenderer::initializeObjects()
{
	//init object data e.g. bounding spheres
	getScene().m_nodeList.init(0.0f, *this);

	//zero time here so that any time sensitive settings use t=0
	m_perfCntr.zeroTime();
	makeRasterFont();

   if (m_useDisplayLists)
   {
	   m_hasDisplayLists = compileAllObjects();
   }//if (m_useDisplayLists)

   getScene().m_nodeList.startup(0.0f, *this);
}//void GLRenderer::initializeObjects()



void
GLRenderer::cleaupDisplayLists()
{  //clean up any display lists
   AWTRACE("Clean up Display Lists\n");
   int numDeleted = 0;
   AWObject* obj = NULL;
   int j = 0;
   for (j=0; j<getScene().m_nodeList.getNumObjects(); j++)
   {  //iteration should include all objects that may have been
      //assigned lists - includes projectiles
      obj = getScene().m_nodeList.getObject(j);
      //-1 case is to deal with legacy files - should now be AW_NOLIST
      //but may as well be careful
      if ((AW_NOLIST != obj->getListID()) && (-1 != obj->getListID()))
      {
         AWTRACE1(" Delete list ID: %i\n", (int)obj->getListID());
         numDeleted++;
         glDeleteLists(obj->getListID(), 1);
         obj->setListID(AW_NOLIST);
      }//if ((AW_NOLIST != obj->getListID()) && (-1 != obj->getListID()))
   }//for (i=0; i<getNumObjects(); i++)
   //do projectiles, if any and one for each projectile, if any
   for (j=0; j < getScene().m_nodeList.getNumProjectiles(); j++)
   {
      AWRocket* nextProj = getScene().m_nodeList.getProjectile(j);
      if ((AW_NOLIST != nextProj->getListID()) && (-1 != nextProj->getListID()))
      {
         AWTRACE1(" Delete list ID: %i\n", (int)nextProj->getListID());
         numDeleted++;
         glDeleteLists(nextProj->getListID(), 1);
         nextProj->setListID(AW_NOLIST);
      }//if ((AW_NOLIST != obj->getListID()) && (-1 != obj->getListID()))
   }//for (int proj=0; proj < getScene().m_nodeList.getNumProjectiles(); proj++)
   AWObject* env = getScene().m_nodeList.getEnvironment();
   if (env)
   {
      if ((AW_NOLIST != env->getListID()) && (-1 != env->getListID()))
      {
         AWTRACE1(" Delete list ID: %i\n", (int)env->getListID());
         numDeleted++;
         glDeleteLists(env->getListID(), 1);
         env->setListID(AW_NOLIST);
      }//if ((AW_NOLIST != obj->getListID()) && (-1 != obj->getListID()))
   }//if (env)
   m_sceneListBase = AW_NOLIST;
   AWTRACE1(" Deleted %i Display Lists\n\n", numDeleted);
}//void GLRenderer::cleaupDisplayLists()



void
GLRenderer::shutdownGL()
{  //shut off any gl states or whatever
	//for example, need to delete all the texture objects
   int i = 0;
   int numTexturesDeleted = 0;
   for (i = 0; i < getScene().m_textureList.getNum(); i++)
   {
      AWTexture* curFrame = NULL;
      AWTexture* mat      = (AWTexture*)getScene().m_textureList[i];
      for (int frame = 0; frame < mat->getNumFrames(); frame++)
      {  //delete each texture object in turn
         curFrame          = mat->getTexFromArrayIndex(frame);
         AWTRACE1(" Delete texture ID: %i\n", (int)curFrame->m_texID);
         glDeleteTextures(1, &curFrame->m_texID);
         curFrame->m_texID = 0;
         numTexturesDeleted++;
      }//for (int frame = 0; frame < mat->getNumFrames(); frame++)
   }//for (int i = 0; i < getScene().getNumTextures(); i++)
   if (m_normalizeCube)
   {  //delete normalization cube map, if any
      glDeleteTextures(1, &m_normalizeCube->m_texID);
      AWTRACE1(" Delete texture ID: %i\n", (int)m_normalizeCube->m_texID);
      delete m_normalizeCube;
      m_normalizeCube = NULL;
      numTexturesDeleted++;
   }//if (m_normalizeCube)
   if (m_shadowBuffer)
   {
      delete m_shadowBuffer;
      m_shadowBuffer = NULL;
   }//if (m_shadowBuffer)
   m_projHelper = NULL;//added to scene and cleaned up above
   AWTRACE1("Deleted %i textures\n", numTexturesDeleted);
	cleaupDisplayLists();
   //prog1.destroy();
   cleanupFont();
	int numLights = getScene().m_nodeList.getNumLights();
	for (int light=0; light<numLights; light++)
	{
      AWTRACE1("Disable GL_LIGHT %i\n", (int)light);
		glDisable(GL_LIGHT0 + light);
	}//for (int light=0; light<numLights; light++)
   float def[] = {0.2f, 0.2f, 0.2f, 1.0f};	
   getScene().setAmbient(AWPoint(0.2f, 0.2f, 0.2f));
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT , def);//back to default
	if (numLights)
	{
      AWTRACE("Disable GL_LIGHTING\n");
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
	}
	m_usingDynLights = FALSE;
	glDisable(GL_FOG);
   disableTexUnits();
   setVertexArray(NULL);
   setVertexColorArray(NULL);
   setNormalArray(NULL);

   m_showStats = FALSE;
}//void GLRenderer::shutdownGL()



void
GLRenderer::shutdown()
{
	shutdownGL();
	getScene().clearAndDestroy();
   resetData();
   if (m_helperCam)
   {
      delete m_helperCam;
      m_helperCam = NULL;
   }//if (m_helperCam)
}//void GLRenderer::shutdown()



void
GLRenderer::resizeGL(int width, int height)
{
	if (width && height)
	{
		m_height = height;
		m_width  = width;
		glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
		initCamera();
	}//if (width && height)
}//void GLRenderer::resizeGL(int width, int height)



AWCubeEnvTexture*           
GLRenderer::generateNormalizeCube(int size)
{
   AWCubeEnvTexture* retVal = NULL;
   if (!(retVal = getNormalizeCube()))
   {
      retVal = new AWCubeEnvTexture("CubeNormMap");
      if (retVal)
      {
         retVal->generateNormalizeVectorCube(size);
         setNormalizeCube(retVal);
         //no compression allowed on this texture!
         initCubeEnvTexture(retVal, FALSE);
      }//if (retVal)
   }//if (!getNormalizeCube())
   return retVal;
}//AWCubeEnvTexture* GLRenderer::generateNormalizeCube(int size)



void
GLRenderer::transformLight(int lightID, AWLight* curLight)
{
	if (curLight)
	{
		float pos[]	 = {0.0f, 0.0f, 0.0f, 1.0f};
		float dir[]	 = {0.0f, 0.0f, 1.0f, 0.0f};
      float dirSpot[] = {0.0f, 0.0f, -1.0f};
		glPushMatrix();
		transformNode(curLight);
		switch (curLight->m_type)
		{
		case AWLight::AW_POINT_LIGHT:
			//use GL_POSITION with w=1 to specify POSITION
			glLightfv(lightID, GL_POSITION, pos);
			break;
		case AWLight::AW_DIRECTIONAL_LIGHT:
			//use GL_POSITION with w=0 to specify DIRECTION
			glLightfv(lightID, GL_POSITION, dir);
			break;
		case AWLight::AW_SPOT_LIGHT:
			//specify direction
			glLightfv(lightID, GL_SPOT_DIRECTION, dirSpot);
			//use GL_POSITION with w=1 to specify position
			glLightfv(lightID, GL_POSITION, pos);
			break;
		}//switch (curLight->m_type
		glPopMatrix();
	}//if (curLight)
}//void GLRenderer::transformLight(int lightID, )



void
GLRenderer::transformLights()
{	//designed to cope with animated lights - apply settings per frame
	AWPoint col;
	int numLights = getScene().m_nodeList.getNumLights();
   if (getScene().isAmbientAnimated())
   {
      AWTRACE3("Setting animated AMBIENT light to (%f, %f, %f)\n", (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
      getScene().getAmbient(*(AWPoint*)m_ambient);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambient);
   }//if (getScene().isAmbientAnimated())
	for (int light = 0; light < numLights; light++)
	{
		AWLight* curLight = getScene().m_nodeList.getLight(light);
      if (curLight && curLight->m_on)
      {
		   transformLight(curLight->m_ID, curLight);
		   if (curLight->isColorAnimated())
		   {
			   curLight->getColor(col);
			   float color[] = {col.x, col.y, col.z, 1.0f};
			   glLightfv(curLight->m_ID, GL_AMBIENT, color);
			   glLightfv(curLight->m_ID, GL_DIFFUSE, color);
            glLightfv(curLight->m_ID, GL_DIFFUSE, color);
		   }//if (curLight->isColorAnimated())
      }//if (curLight && curLight->m_on)
	}//for (int light = 0; light < numLights; light++)
}//void GLRenderer::transformLights()



AWBoolean
GLRenderer::initLights()
{
	AWBoolean retVal = TRUE;
   //static const float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	int numLights = getScene().m_nodeList.getNumLights();
	for (int light = 0; light < numLights; light++)
	{
		AWPoint  colour;
      int      lightID = GL_LIGHT0 + light;
		AWLight* curLight = getScene().m_nodeList.getLight(light);
      if (curLight && curLight->m_on)
      {
		   curLight->getColor(colour);
		   float    diffuse[] = {colour.x, colour.y, colour.z, 1.0f};
		   
		   curLight->m_ID	= lightID;
         AWTRACE1(" Enable GL_LIGHT %i\n", light);
		   glEnable(lightID);
		   glLightfv(lightID, GL_AMBIENT, diffuse);
		   glLightfv(lightID, GL_DIFFUSE, diffuse);
		   glLightfv(lightID, GL_SPECULAR, diffuse);
		   switch (curLight->m_type)
		   {
		   case AWLight::AW_SPOT_LIGHT:
			   glLightf(lightID, GL_SPOT_CUTOFF, curLight->m_cutOffAngle);
			   break;
		   }//switch (curLight->m_type
      }//if (curLight && curLight->m_on)
	//The default ambient light intensity is (0.0, 0.0, 0.0, 1.0).
	//The default diffuse intensity is (0.0, 0.0, 0.0, 1.0) for all lights other
	//than light zero. The default diffuse intensity of light zero is
	//(1.0, 1.0, 1.0, 1.0).
	} //for (int light = 0; light < numLights; light++)
	return retVal;
}//AWBoolean GLRenderer::initLights()



//converts texture format (eg GL_RGB) into correct form for 
//'internal format' parameter to glTexImage2D (eg GL_RGB8)
//depends on overall renderer policy & target platform speed?
//dependencies on 16bit /vs. 32 bit?
int            
GLRenderer::getInternalFormat(int texFormat, AWBoolean allowCompression)
{
   int internalFormat = texFormat;
   switch (internalFormat)
   {
   case GL_BGR_EXT:
   case GL_RGB:
      internalFormat = getUseS3Compression() && allowCompression ? 
                              GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_RGB8;
      break;
  
   case GL_BGRA_EXT:
   case GL_RGBA:
      internalFormat = getUseS3Compression() && allowCompression ? 
                              GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_RGBA8;
      break;

   case GL_DEPTH_COMPONENT:
      internalFormat = GL_DEPTH_COMPONENT24_SGIX;
      if (getShadowBuffer()&&(16==getShadowBuffer()->getDepthBits()))
      {
         internalFormat = GL_DEPTH_COMPONENT16_SGIX;
      }//if (getShadowBuffer()&&(16!=getShadowBuffer()->m_depthBits))
      break;

   default:
      break;
   }//switch (internalFormat)
   return internalFormat;
}//int GLRenderer::getInternalFormat(int texFormat, AWBoolean allowCompression)



AWBoolean
GLRenderer::setupTexture(AWTexture* t, AWBoolean allowAutoMipmap)
{
   glEnable(t->m_target);
   glGenTextures(1, &t->m_texID);
   glBindTexture(t->m_target, t->m_texID);

   AWTRACE4(" Texture init %s: %s  ID: %i  mipMap: %i\n", 
               t->getTargetName(), t->m_name.c_str(), 
               (int)t->m_texID, (int)t->m_mipMap);
   glTexParameteri(t->m_target, GL_TEXTURE_WRAP_S, t->m_wrap);
   glTexParameteri(t->m_target, GL_TEXTURE_WRAP_T, t->m_wrap);
   glTexParameteri(t->m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(t->m_target, GL_TEXTURE_MIN_FILTER, 
                   t->m_mipMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
   if (supportsAutoMipmap() && allowAutoMipmap && t->m_mipMap)
   {
      glTexParameteri(t->m_target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
   }//if (supportsAutoMipmap() && allowAutoMipmap && t->m_mipMap)
   if (CLSID_AWSHADOWMAP==t->isA())
   {
      glTexParameteri(t->m_target, GL_TEXTURE_COMPARE_SGIX, GL_TRUE);
      glTexParameteri(t->m_target, GL_TEXTURE_COMPARE_OPERATOR_SGIX, GL_TEXTURE_LEQUAL_R_SGIX);
   }//if (CLSID_AWSHADOWMAP==t->isA())

   return (0 == (m_glErr = glGetError()));
}//AWBoolean GLRenderer::setupTexture(AWTexture* t, AWBoolean allowAutoMipmap)



void
GLRenderer::initCubeEnvTexture(AWCubeEnvTexture* f, AWBoolean allowCompression)
{
   if (!f) return;
   if (getHasCubeMap())
   {
      if (!setupTexture(f)) return;
      AWTexture* nextFace = NULL;
      for (int face=0; face<AWCubeEnvTexture::NUM_FACES; face++)
      {
         int level = 0;
         nextFace = f->getFace(enum AWCubeEnvTexture::FaceEnum(face));
         if (nextFace)
         {
            while (nextFace->getDataPtr())
            {
               glTexImage2D( faceMap[face], level, 
                             getInternalFormat(nextFace->m_format, allowCompression),
		                       nextFace->m_width, nextFace->m_height,
                             0, nextFace->m_format, nextFace->getDataType(), 
                             nextFace->getDataPtr() );
               m_glErr = glGetError();

               if (!f->m_mipMap || supportsAutoMipmap() ||
                   (nextFace->m_width==1 && nextFace->m_height==1))  break;

               int neww = (nextFace->m_width<2)  ? 1 : nextFace->m_width/2;
               int newh = (nextFace->m_height<2) ? 1 : nextFace->m_height/2;
               nextFace->reSize(neww, newh);
               level++;
            }//while (nextFace->getDataPtr())

            delete [] nextFace->getDataPtr();
		      nextFace->getDataPtr() = NULL;
         }//if (nextFace)
	   }//for (int face=0; face<AWCubeEnvTexture::NUM_FACES; face++) 
      glDisable(f->m_target);
   }//if (getHasCubeMap())
   else
   {
      AWTRACE1(" Texture was of type (GL_TEXTURE_CUBE_MAP_ARB): %s - transforming to SPHERE\n", 
               f->m_name.c_str());
      f->transformToSphereMap();
      init2DTexture(f);
   }
}//void GLRenderer::initCubeEnvTexture(AWCubeEnvTexture* f, AWBoolean allowCompression)



void           
GLRenderer::initNormalMapTexture(AWNormalMapTexture* tex)
{
   int level = 0;
   if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target))
   {  //we must NOT use auto-generated mipmaps - hence FALSE
      if (!setupTexture(tex, FALSE)) return;   
      AWTRACE("  Initialising NORMAL MAP\n");
      while (1)
      {  //make sure we do NOT use texture compression for this map 
         glTexImage2D( tex->m_target, level, getInternalFormat(tex->m_format, FALSE),
		          tex->m_width, tex->m_height,
                0, tex->m_format, tex->getDataType(), tex->getDataPtr() );
         m_glErr = glGetError();

         if (!tex->m_mipMap||(tex->m_width==1&&tex->m_height==1))  break;

         int neww = (tex->m_width<2)  ? 1 : tex->m_width/2;
         int newh = (tex->m_height<2) ? 1 : tex->m_height/2;
         //normal map reSize takes care of correct downsampling
         tex->reSize(neww, newh);
         level++;
      }//while (1)
	   delete [] tex->getDataPtr();
	   tex->getDataPtr() = NULL;
   }//if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target))
}//void GLRenderer::initNormalMapTexture(AWNormalMapTexture* tex)



void
GLRenderer::initShadowMap(AWShadowMap* tex)
{
   if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target))
   {
      setupTexture(tex);
      glTexImage2D(tex->m_target, 0, getInternalFormat(tex->m_format),
			             tex->m_width, tex->m_height,
                      0, tex->m_format, tex->getDataType(), 0);
      m_glErr = glGetError();
   }//if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target)
}//void GLRenderer::initShadowMap(AWShadowMap* tex)



void           
GLRenderer::init2DTexture(AWTexture* tex)
{
   if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_2D==tex->m_target))
   {
      setupTexture(tex);     
      //new addition - AUTO MIPMAP GENERATION if available
      int level = 0;
      while (1)
      {
         glTexImage2D( tex->m_target, level, getInternalFormat(tex->m_format),
			             tex->m_width, tex->m_height,
                      0, tex->m_format, tex->getDataType(), tex->getDataPtr() );
         m_glErr = glGetError();

         if ( !tex->m_mipMap || supportsAutoMipmap() || 
             (tex->m_width==1 && tex->m_height==1)    )  break;

         int neww = (tex->m_width<2)  ? 1 : tex->m_width/2;
         int newh = (tex->m_height<2) ? 1 : tex->m_height/2;

         tex->reSize(neww, newh);
         level++;
      }//while (1)

		//can now delete the loaded texture to conserve memory useage
		delete [] tex->getDataPtr();
		tex->getDataPtr() = NULL;
   }//if (tex && tex->m_width && tex->m_height)
}//void GLRenderer::init2DTexture(AWTexture* tex)



void           
GLRenderer::init1DTexture(AWTexture* tex)
{
   if (tex && tex->m_width && tex->m_height && (GL_TEXTURE_1D==tex->m_target))
   {
      setupTexture(tex);     
      //new addition - AUTO MIPMAP GENERATION if available
      int level = 0;
      while (1)
      {
         glTexImage1D( tex->m_target, level, getInternalFormat(tex->m_format),
			              tex->m_width,  0, tex->m_format, 
                       tex->getDataType(), tex->getDataPtr() );
         m_glErr = glGetError();

         if ( !tex->m_mipMap || supportsAutoMipmap() || 
             (tex->m_width==1) )  break;

         int neww = (tex->m_width<2)  ? 1 : tex->m_width/2;

         tex->reSize(neww);
         level++;
      }//while (1)

		//can now delete the loaded texture to conserve memory useage
		delete [] tex->getDataPtr();
		tex->getDataPtr() = NULL;
   }//if (tex && tex->m_width && tex->m_height)
}//void GLRenderer::init1DTexture(AWTexture* tex)



void           
GLRenderer::initTexture(AWTexture* tex)
{
   if (!tex) return;
   //perform any required pre-calc / init / whatever
   tex->init();
   if (CLSID_AWCUBEENVTEXTURE == tex->isA())
   {
      initCubeEnvTexture((AWCubeEnvTexture*)tex);
   }
   else if (CLSID_AWNORMALMAPTEXTURE == tex->isA())
   {
      initNormalMapTexture((AWNormalMapTexture*)tex);
   }
   else if (CLSID_AWSHADOWMAP == tex->isA())
   {
      initShadowMap((AWShadowMap*)tex);
   }
   else if (GL_TEXTURE_2D==tex->m_target)
   {
		init2DTexture(tex);
   }
   else if (GL_TEXTURE_1D==tex->m_target)
   {
      init1DTexture(tex);
   }
}//void GLRenderer::initTexture(AWTexture* tex)



AWBoolean
GLRenderer::initTextures()
{
   AWTRACE("\n");
	AWBoolean retVal = TRUE;
   glEnable(GL_TEXTURE_2D);
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	int numMaterials = getScene().m_textureList.getNum();
	for (int i = 0; i < numMaterials; i++)
	{
		AWTexture* mat = (AWTexture*)getScene().m_textureList[i];
		AWTRACE2("Texture: %s has %d frames\n", mat->m_name.c_str(), (int)mat->getNumFrames());
		for (int frame = 0; frame < mat->getNumFrames(); frame++)
		{
			AWTexture* curFrame = mat->getTexFromArrayIndex(frame);
         initTexture(curFrame);
         AWTRACE2("  init frame: %i with ID %d\n", frame, (int)curFrame->m_texID);
		}//for (int frame = 0; frame < mat->getNumFrames(); frame++)
	}//for (int i = 0; i < numMaterials; i++)
   if (m_normalizeCube)
   {
      initTexture(m_normalizeCube);
   }//if (m_normalizeCube)
	AWTRACE("\n");
	return retVal;
}//AWBoolean GLRenderer::initTextures()



void
GLRenderer::displayEnvironment()
{
   AWObject* env = getScene().m_nodeList.getEnvironment();
   if (env)
   {
      enableDepthTest(FALSE);
      if (usingLighting()) glDisable(GL_LIGHTING);
      drawObject(env);
		if (usingLighting()) glEnable(GL_LIGHTING);
      enableDepthTest(TRUE);
   }//if (env)
}//void  GLRenderer::displayEnvironment()



AWPBuffer*
GLRenderer::getShadowBuffer(int size)
{
   if (!m_shadowBuffer)
   {
      m_shadowBuffer = new AWPBuffer(size, size, AW_SINGLE|AW_DEPTH|AW_STENCIL);
      if (m_shadowBuffer)
      {
         m_shadowBuffer->init(TRUE);
      }//if (m_shadowBuffer)
   }//if (!m_shadowBuffer)
   return m_shadowBuffer;
}//AWPBuffer* GLRenderer::getShadowBuffer(int size)



void
GLRenderer::updateShadowMap(AWLight* l)
{ 
   if (l && l->m_shadow && (CLSID_AWSHADOWMAP==l->m_shadow->isA()))
   {
      AWCamera*      curCam    = getScene().m_nodeList.getCurCamera();
      double         nearClip  = (curCam) ? curCam->m_nearClip : 1.0f;
      double         farClip   = (curCam) ? curCam->m_farClip  : 1.0f;
      AWMatrix4      viewTM;
      AWShadowMap*   shadowMap = (AWShadowMap*)l->m_shadow;

      l->getViewTM(viewTM);
      if (shadowMap)
      {
         AWPBuffer* pb = getShadowBuffer(shadowMap->m_width);
         pb->makeCurrent();
         m_applyObjectTextures = FALSE;

         glViewport(0, 0, shadowMap->m_width, shadowMap->m_width);
         glMatrixMode(GL_PROJECTION);
         glLoadIdentity();
		   double ymax = curCam->m_nearClip * tan( curCam->m_fov * M_PI / 360.0);
		   glFrustum(-ymax, ymax, -ymax, ymax, nearClip, farClip);
         glMatrixMode(GL_MODELVIEW);
         glPushMatrix();
         //set the correct VIEW matrix
         glLoadMatrixf(viewTM.m_mat);  
         glPolygonOffset(shadowMap->getRange(), shadowMap->getBias());
         //Only need depth values so avoid writing colors for speed
         glColorMask(0,0,0,0);
         glDisable(GL_LIGHTING);         
	      glClear(GL_DEPTH_BUFFER_BIT);
	      glEnable(GL_POLYGON_OFFSET_FILL);        
         showAllOpaqueObjects();
	      glDisable(GL_POLYGON_OFFSET_FILL);
         glEnable(GL_LIGHTING);
         glColorMask(1,1,1,1);

         glPopMatrix();
	      
         glBindTexture(shadowMap->m_target, shadowMap->m_texID);
	      glCopyTexSubImage2D(shadowMap->m_target, 0, 0, 0, 0, 0, shadowMap->m_width, shadowMap->m_height);
         m_glErr = glGetError();
         
         pb->restoreContext();
         m_glErr = glGetError();
         //gl state should be tracked PER CONTEXT but is not -
         //so reset tex unit related state once we're done.
         m_applyObjectTextures = TRUE;
         disableTexUnits();
      }//if (shadowMap)
   }//if (l && l->m_shadow && (CLSID_AWSHADOWMAP==l->m_shadow->isA()))
}//void GLRenderer::updateShadowMap(AWLight* l)



void
GLRenderer::renderProjectors()
{  //we can render n textures at a time where n is the number
   //of texture units on the card. Therefore in the projective
   //pass we can render n lights at once by using a texture unit
   //per light. For now, just render one light per pass as proof
   //of concept
   if (0==getScene().m_projectors.getNum()) return;

   m_applyObjectTextures    = FALSE;

   AWMatrix4      projViewTM, projWSTM, camViewTM;
   AWSolidCone    viewCone;
   AWLight*       curProj   = NULL;
   AWCamera*      curCam    = getScene().m_nodeList.getCurCamera();
   double         nearClip  = (curCam) ? curCam->m_nearClip : 1.0f;
   double         farClip   = (curCam) ? curCam->m_farClip  : 1.0f;
   double         ymax      = 0.0f;
   //double         planeEqn[4];

   curCam->getViewTM(camViewTM);

   viewCone.m_farClip       = (float)farClip;
   
   AWRenderPass   proj;
   proj.setBlendType(AWTexture::ALPHA_BLEND);
   proj.setSrcFac(GL_ONE);
   proj.setDstFac(GL_ONE);
   proj.setDepthFunc(GL_EQUAL);

   AWTextureUnit tUnit;
   tUnit.m_texFunc = AWTextureUnit::REPLACE;
   proj.addTexUnit(tUnit);

   //AWTexture* oneDhelper = get1DProjectHelper();
   
   if (usingLighting()) glDisable(GL_LIGHTING);
   for (int i=0; i<getScene().m_projectors.getNum(); i++)
   {   
      glPushMatrix();
      transformViewport(getScene().m_nodeList.getViewTransformer());
      curProj = getScene().m_projectors[i];
      if (curProj && curProj->m_proj)
      {  //determine view cone and cull based on forward visibility
         //from light - helps to minimize backwards projection
         curProj->getViewCone(viewCone, projWSTM);
         projWSTM.invert(projViewTM);  //find VIEW matrix        
         proj.getTexUnit(0)->m_tex = curProj->m_proj;
        //proj.getTexUnit(1)->m_tex = oneDhelper;
         
        //planeEqn calculated as follows -
         //we need projector ws posn - viewCone.m_V
         //projector ws direction    - viewCone.m_A - already normalized
         //AWPoint tmp(viewCone.m_A);
         //tmp = AWPoint(0,0,-1);
         //tmp.normalize();

         ///AWPoint tmp2(viewCone.m_V);
         ///tmp2 = AWPoint(0,0,0);
         ///planeEqn[0] = (double)tmp.x;
         //planeEqn[1] = (double)tmp.y;
         //planeEqn[2] = (double)tmp.z;
         //planeEqn[3] = (double)dot(tmp, tmp2);
         
         //glClipPlane(GL_CLIP_PLANE0, planeEqn);
         //glEnable(GL_CLIP_PLANE0);

         setBlendState(&proj);
         setTextureState(&proj);
         
         if (glActiveTexture) glActiveTexture(GL_TEXTURE0_ARB);
         glMatrixMode(GL_TEXTURE);        
	      glLoadIdentity();
	      glTranslatef(.5, .5, 0);
	      glScalef(.5, .5, 1);
         ymax = nearClip * tan(2.0 * curProj->m_cutOffAngle * M_PI / 360.0);
	      glFrustum(-ymax, ymax, -ymax, ymax, nearClip, farClip);  
         glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
         glMultMatrixf(projViewTM.m_mat);
	      glMatrixMode(GL_MODELVIEW);
         
         m_glErr = glGetError();
         
         //update the scene visibility flags for this render
         getScene().m_nodeList.calcVisibility(m_time, viewCone);

	      glDepthMask(GL_FALSE);
         m_glErr = glGetError();
         //render the whole scene
	      showAllOpaqueObjects();
         glDepthMask(GL_TRUE);

         //glDisable(GL_CLIP_PLANE0);
      }//if (curProj)
      glPopMatrix();     
   }//for (int i=0; i<getScene().m_projectors.getNum(); i++)
   //make sure we're leaving GL in a suitable state
   if (usingLighting()) glEnable(GL_LIGHTING);
   if (glActiveTexture) glActiveTexture(GL_TEXTURE0_ARB);
   setTexGenMode(AWTexture::G_NONE, 0);
   glMatrixMode(GL_TEXTURE);        
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

   if (glActiveTexture) glActiveTexture(GL_TEXTURE1_ARB);
   setTexGenMode(AWTexture::G_NONE, 1);
   glMatrixMode(GL_TEXTURE);        
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
   m_applyObjectTextures = TRUE;
}//void GLRenderer::renderProjectors()


void
GLRenderer::renderShadows()
{ 
   if (0==getScene().m_shadowers.getNum()) return;
   if (!m_hasPBuffer || !m_hasSGIShadowMaps) return;

   m_applyObjectTextures = FALSE;
  
   AWMatrix4      projViewTM, projWSTM;
   AWSolidCone    viewCone;
   AWLight*       curProj   = NULL;
   AWCamera*      curCam    = getScene().m_nodeList.getCurCamera();
   double         nearClip  = (curCam) ? curCam->m_nearClip : 1.0f;
   double         farClip   = (curCam) ? curCam->m_farClip  : 1.0f;
   double         ymax      = 0.0f;

   viewCone.m_farClip       = (float)farClip;
   
   AWRenderPass   proj;
   proj.setBlendType(AWTexture::ALPHA_BLEND);
   proj.setSrcFac(GL_ONE);
   proj.setDstFac(GL_ONE);
   proj.setDepthFunc(GL_EQUAL);

   AWTextureUnit tUnit;
   tUnit.m_texFunc = AWTextureUnit::REPLACE;
   proj.addTexUnit(tUnit);

   tUnit.m_texFunc = AWTextureUnit::MODULATE;
   proj.addTexUnit(tUnit);
   
   if (usingLighting()) glDisable(GL_LIGHTING);
   for (int i=0; i<getScene().m_shadowers.getNum(); i++)
   {         
      curProj = getScene().m_shadowers[i];
      //update this light's shadow map by rendering scene
      //using the light as 'camera'
      float rCoordScale = 0.5f;
      float rCoordBias  = 0.5f;

      if (curProj && curProj->m_shadow && curProj->m_proj)
      {  //determine view cone and cull based on forward visibility
         //from light - helps to minimize backwards projection
         curProj->getViewCone(viewCone, projWSTM);
         projWSTM.invert(projViewTM);  //find VIEW matrix        
         proj.getTexUnit(0)->m_tex = curProj->m_proj;
         proj.getTexUnit(1)->m_tex = curProj->m_shadow;
         
         glPushMatrix();
         setBlendState(&proj);
         setTextureState(&proj);
         disableBackProjection();
         //
         //SPOTLIGHT IMAGE - UNIT0
         //we're using EYE LINEAR texgen & REPLACE
         //projecting from curProj
         if (glActiveTexture) glActiveTexture(GL_TEXTURE0_ARB);
         glMatrixMode(GL_TEXTURE);        
	      glLoadIdentity();
	      glTranslatef(.5, .5, .5f);
	      glScalef(.5, .5, .5f);
         ymax = nearClip * tan(2.0 * curProj->m_cutOffAngle * M_PI / 360.0);
	      glFrustum(-ymax, ymax, -ymax, ymax, nearClip, farClip);  
         glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
         glMultMatrixf(projViewTM.m_mat);
	      glMatrixMode(GL_MODELVIEW);
         //
         //SHADOW MAP      - UNIT1
         //we're using EYE LINEAR texgen & MODULATE
         //projecting from curProj
         if (glActiveTexture) glActiveTexture(GL_TEXTURE1_ARB);
         glMatrixMode(GL_TEXTURE);        
	      glLoadIdentity();
	      glTranslatef(.5, .5, rCoordScale);
	      glScalef(.5, .5, rCoordBias);
	      glFrustum(-ymax, ymax, -ymax, ymax, nearClip, farClip);
         glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
         glMultMatrixf(projViewTM.m_mat);
	      glMatrixMode(GL_MODELVIEW);
        
         //update the scene visibility flags for this render
         getScene().m_nodeList.calcVisibility(m_time, viewCone);

	      glDepthMask(GL_FALSE);
         m_glErr = glGetError();
         //render the whole scene        
	      showAllOpaqueObjects();
         glDepthMask(GL_TRUE);

         glPopMatrix(); 
      }//if (curProj)
   }//for (int i=0; i<getScene().m_projectors.getNum(); i++)
   //make sure we're leaving GL in a suitable state
  
   if (usingLighting()) glEnable(GL_LIGHTING);
   //we altered texture matrix for unit0
   if (glActiveTexture) glActiveTexture(GL_TEXTURE0_ARB);
   setTexGenMode(AWTexture::G_NONE, 0);
   glMatrixMode(GL_TEXTURE);        
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
   //and also for unit1
   if (glActiveTexture) glActiveTexture(GL_TEXTURE1_ARB);
   setTexGenMode(AWTexture::G_NONE, 1);
   glMatrixMode(GL_TEXTURE);        
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
   m_applyObjectTextures = TRUE;
}//void GLRenderer::renderShadows()



AWBoolean           
GLRenderer::renderCubeMap(AWObject* obj, AWCubeEnvTexture* t)
{  //all objects which should not be rendered have been tagged as
   //not visible. therefore we can render the frame 6 times to
   //build up the cube map. we need to set the viewport to the correct
   //texture dimensions for one of the cube faces
   //
   //then take 6 shots, updating the texture
   //each time. For machines without AUTO MIPMAP
   //real-time textures are non-mipmapped.
   //we are safe in the knowledge that this object itself
   //won't appear in the scene beacause it is still invisible.
   //
   //best way to go about this seems to be to 
   // CREATE HELPER CAMERA. Orient to world space axes 
   // but center to object space origin.
   // use standard camera selection calls to initialise all
   // necessary data members for each render, including 
   // update of visibility data
   if (!obj || !t || !getHelperCam()) return FALSE;
   if (!t->needsUpdate(m_time))       return TRUE;

   AWCamera* oldCam = getScene().m_nodeList.getCurCamera();
   AWCamera& cam    = *getHelperCam();

   float oldWidth   = (float)m_width;
   float oldHeight  = (float)m_height;
  
   cam.m_nearClip   = t->getNearClip();
   cam.m_farClip    = t->getFarClip();
   cam.m_fov        = 90.0f;

   getScene().m_nodeList.setCurCamera(&cam);
   //following call changes m_width & m_height
   //calls glViewport, initialises the projection matrix
   resizeGL(t->m_width, t->m_height);  

   AWTexture*  facep = NULL;
   AWSolidCone viewCone;
   AWPoint     posn;
   AWMatrix4   wsTM;
   viewCone.setAngle(cam.m_fov);
   viewCone.m_farClip = t->getFarClip();
   obj->getPosn(posn);   
   glLoadIdentity();
   for (int face=0; face<AWCubeEnvTexture::NUM_FACES; face++)
   {
      facep = t->getFace(enum AWCubeEnvTexture::FaceEnum(face));
      if (facep)
      {  //render the entire frame       
         glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
         glPushMatrix();

         glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
         glRotatef(AWCubeEnvTexture::m_mapRots[face][0],
                   AWCubeEnvTexture::m_mapRots[face][1],
                   AWCubeEnvTexture::m_mapRots[face][2],
                   AWCubeEnvTexture::m_mapRots[face][3]);
         glTranslatef(-posn.x, -posn.y, -posn.z);
         //find the view cone for this render frustrum
         glGetFloatv(GL_MODELVIEW_MATRIX, m_viewTM.m_mat);
         m_viewTM.invert(wsTM);
         wsTM.getDirection(viewCone.m_A);
	      viewCone.m_A = -viewCone.m_A.normalize();
	      wsTM.getTranslation(viewCone.m_V);
         //update the scene visibility flags for this render
         getScene().m_nodeList.calcVisibility(m_time, viewCone);
         
         displayEnvironment();
         transformLights();
	      showAllOpaqueObjects();
         if (getScene().m_nodeList.getNumAlphaTestObjects())
         {
            showAllAlphaTestObjects();
         }//if (getScene().m_nodeList.getNumAlphaTestObjects())
         if (getScene().m_nodeList.getNumTransparentObjects())
         {
            glDepthMask(GL_FALSE);
            showAllTransparentObjects();
            glDepthMask(GL_TRUE);
         }//if (getScene().m_nodeList.getNumTransparentObjects())

         renderProjectors();
         renderShadows();
         
         //Now we need to grab the backbuffer into the texture.
		   glEnable(GL_TEXTURE_CUBE_MAP_ARB);
		   glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, t->getTextureID());
		   glCopyTexSubImage2D(faceMap[face], 0, 0, 0, 0, 0, t->m_width, t->m_height);
		   glDisable(GL_TEXTURE_CUBE_MAP_ARB);

         enableBlend(FALSE);
         setAlphatestEnabled(FALSE);
         combinerMode(AWRenderPass::AW_CM_NONE);
         disableTexUnits();
         glPopMatrix();
      }//if (facep)
   }//for (int face=0; face<AWCubeEnvTexture::NUM_FACES; face++)
   //restore old camera conditions
   t->setLastUpdate(m_time);
   getScene().m_nodeList.setCurCamera(oldCam); 
   resizeGL((int)oldWidth, (int)oldHeight); 
   return TRUE;
}//AWBoolean GLRenderer::renderCubeMap(AWObject* obj, AWCubeEnvTexture* t)



AWTexture*     
GLRenderer::get1DProjectHelper()
{
   static const int projHelperLen = 32;//32 pixels wide
   static const int comp = 4;//GL_RGBA means 4 components
   if (!m_projHelper)
   {
      m_projHelper = new AWTexture("projHelper");
      getScene().m_textureList.add(m_projHelper);
      m_projHelper->m_target  = GL_TEXTURE_1D;
      m_projHelper->m_format  = GL_RGBA;
      m_projHelper->m_wrap    = GL_CLAMP_TO_EDGE;
      m_projHelper->m_width   = projHelperLen; 
      m_projHelper->m_height  = 1;
      m_projHelper->m_mipMap  = 0;
      m_projHelper->m_genMode = AWTexture::EYE_LINEAR;
      m_projHelper->m_pImage  = new unsigned char[comp * m_projHelper->m_width];
      AWrgba pixel(255, 255, 255, 255);
      //image contents - format is GL_RGBA8
      int index = 0;
      for (int p=0; p<projHelperLen; p++)
      {
         index = p*comp;
         m_projHelper->m_pImage[index]  = pixel.r;
         m_projHelper->m_pImage[index+1]= pixel.g;
         m_projHelper->m_pImage[index+2]= pixel.b;
         if (comp==4)
         {
            m_projHelper->m_pImage[index+3]= pixel.a;
         }
         pixel.r = pixel.g = pixel.b = (p<16)?255:0;
      }//for (int p=0; p<projHelperLen; p++)
      initTexture(m_projHelper);
   }//if (!m_projHelper)
   return m_projHelper;
}//AWTexture* GLRenderer::get1DProjectHelper()



void
GLRenderer::disableBackProjection()
{
   float cullMode[4] = {GL_GEQUAL, GL_GEQUAL, GL_GEQUAL, GL_GEQUAL};

   if (FALSE)//glActiveTexture) 
   {
      glEnable(GL_TEXTURE_SHADER_NV);
      glActiveTexture(GL_TEXTURE0_ARB);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glActiveTexture(GL_TEXTURE1_ARB);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_CULL_FRAGMENT_NV);
      glTexEnvfv(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, cullMode);

      glActiveTexture(GL_TEXTURE2_ARB);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);

      glActiveTexture(GL_TEXTURE3_ARB);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);

      glActiveTexture(GL_TEXTURE0_ARB);
   }
}//void GLRenderer::disableBackProjection()



void
GLRenderer::buildDynamicMaps()
{
   int               numAutoCubes = getScene().m_autoCubes.getNum();
   int               cube     = 0;
   AWObject*         obj      = NULL;
   AWCubeGen*        cubeGen  = NULL;
   
   //FIRST - go through all of the textures and set the objects
   //that are using them to NOT VISIBLE in the scene.
   for (cube=0; cube<numAutoCubes; cube++)
   {
      if (cubeGen = getScene().m_autoCubes[cube])
      {
         cubeGen->hideAllObjects();      
      }//if (cubeGen = getScene().m_autoCubes[cube])
   }//for (int cube =0; cube<getScene().m_autoCubes.getNum(); cube++)
   //SECOND - render the cube maps IFF required.
   for (cube=0; cube<numAutoCubes; cube++)
   {
      if ((cubeGen = getScene().m_autoCubes[cube]) && cubeGen->m_objs.getNum() &&
          (obj = cubeGen->m_objs[0]))
      {
         //supply first object to orient the base camera
         if (renderCubeMap(obj, cubeGen->m_tex))
         {  //restore object(s) visibility after sucessful render
            cubeGen->showAllObjects();
         }//if (renderCubeMap(obj, cubeGen->m_tex))
      }//if (cubeGen = getScene().m_autoCubes[cube])
   }//for (int cube=0; cube<numAutoCubes; cube++)
   //NOW render all SHADOW MAPS required for the scene
   for (int i=0; i<getScene().m_shadowers.getNum(); i++)
   {         
      AWLight* curProj = getScene().m_shadowers[i];
      if (curProj && curProj->m_shadow)
      {  //determine view cone and cull based on forward visibility
         //from light - helps to minimize backwards projection
         updateShadowMap(curProj);
      }//if (curProj && curProj->m_shadow)
   }//for (int i=0; i<getScene().m_shadowers.getNum(); i++)
   //restore camera and viewport settings for standard render IFF required.
   if (numAutoCubes||getScene().m_shadowers.getNum())
   {
      glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
	   initCamera();
      getScene().m_nodeList.calcVisibility(m_time);
   }//if (numAutoCubes)
}//void GLRenderer::buildDynamicMaps()



void
GLRenderer::displayFrame()
{
   AWTRACEHI("\n\n\nSTART FRAME\n");
   m_trisThisFrame=0;
  
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   m_glErr = glGetError();
   glPushMatrix();
   // VIEWPORT TRANSFORMATION
   //
   transformViewport(getScene().m_nodeList.getViewTransformer());

   //DISPLAY BACKGROUND
   displayEnvironment();
   // DISPLAY SCENE CONTENTS
   //
   transformLights();

	showAllOpaqueObjects();
   //now draw ALPHA TEST objects
   if (getScene().m_nodeList.getNumAlphaTestObjects())
   {
      showAllAlphaTestObjects();
   }//if (getScene().m_nodeList.getNumAlphaTestObjects())

   //render any ALPHA BLENDED objects
   if (getScene().m_nodeList.getNumTransparentObjects())
   {
      glDepthMask(GL_FALSE);
      showAllTransparentObjects();
      glDepthMask(GL_TRUE);
   }//if (getScene().m_nodeList.getNumTransparentObjects())

   renderProjectors();
   renderShadows();

   enableBlend(FALSE);
   setAlphatestEnabled(FALSE);
   combinerMode(AWRenderPass::AW_CM_NONE);
   disableTexUnits();

   displayText();
   glPopMatrix();
	//this is NOT PLATFORM INDEPENDENT
	//but needs to be here for speed right now...
#ifdef _WIN32
   SwapBuffers(wglGetCurrentDC());
#endif
}//void GLRenderer::displayFrame()



const float
GLRenderer::evalTime()
{
   if (m_paused) return m_time;
   if (!m_captureBuf)
   {
      m_time = m_perfCntr.getTime() - m_pauseTime;
   }
   else
   {  //if capturing the screen advance the time by one frame
      m_time += FRAME_STEP;
   }
   if (m_loop && (m_time >= getScene().getLoopTime()))
   {
      m_time=0.0f;
      m_perfCntr.zeroTime();
      m_pauseTime=0.0f;
   }
   return m_time;
}//const float GLRenderer::evalTime()



void
GLRenderer::pause()
{
    m_pauseTime = m_perfCntr.getTime() - m_pauseTime;
}//void GLRenderer::pause(float t)



void
GLRenderer::togglePause()
{
    pause();
    m_paused = !m_paused;
}//void GLRenderer::togglePause()



void
GLRenderer::initTimeBar(float timeout)
{
    getScene().m_nodeList.initTimeBar(timeout, m_time);
}//void GLRenderer::initTimeBar(float t)



void
GLRenderer::display(float speed, float pitch, float yaw, float roll,
					float viewPitch, float viewYaw)
{  
   AWTRACEHI4("GLRenderer::display(speed:%4f pitch:%4f yaw:%4f roll %4f)\n", (double)speed, (double)pitch, (double)yaw, (double)roll);
   AWTRACEHI2("   (viewPitch:%4f viewYaw:%4f)\n", (double)viewPitch, (double)viewYaw);
   m_perfCntr.startTransform(); //start transform performance counter
	//update the Viewport TM based on user input for this frame
   getScene().update(pitch, yaw, roll, viewPitch, viewYaw, speed, m_time);	
	m_perfCntr.stopTransform();  //stop the transform performance counter
   m_perfCntr.startRender();    //start render performance counter
   //now iterate through the textures  
   //set the background color ONCE at this top level
   if (getScene().isBackgroundAnimated() && (-1 == getScene().getUseFog()))
   {
      getScene().getBackground(*(AWPoint*)m_background);
      AWTRACEHI4("Setting animated clear colour: (%5f, %5f, %5f, %5f)\n", (double)m_background[0], (double)m_background[1], (double)m_background[2], (double)m_background[3]);
	   glClearColor(m_background[0], m_background[1], m_background[2], m_background[3]);
   }//if (getScene().isAmbientAnimated())
   buildDynamicMaps();
	displayFrame();  
   m_perfCntr.stopRender();
   //render sequential files when in render mode - very slow!
	if (m_captureBuf) captureScreen();
	
	m_perfCntr.startCollide();
	getScene().m_nodeList.collideAllObjects(m_time);
	m_perfCntr.stopCollide();
	m_perfCntr.update(m_trisThisFrame);
}//void GLRenderer::display(float speed, float roll, float yaw, float pitch)



void
GLRenderer::displayText()
{
   if (getShowStatistics() && m_width && m_height)
   {
      enableDepthTest(FALSE);
      glDisable(GL_FOG);
      if (usingLighting()) glDisable(GL_LIGHTING);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho (0.0, m_width, 0.0, m_height, -1.0, 1.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glColor3f(1.0f,1.0f,1.0f);
      glRasterPos2i(20, 60);
      showRenderStatistics();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);

      enableDepthTest(TRUE);
      if (-1 != getScene().getUseFog()) glEnable(GL_FOG);
      if (usingLighting()) glEnable(GL_LIGHTING);
   }//if (getShowStatistics() && m_width && m_height)
}//void GLRenderer::displayText()



void
GLRenderer::transformViewport(AWTransformController* node)
{	// VIEWPORT TRANSFORMATION
	if (node)
   {  //wsTM of nodes in the scene does not include the viewTM
		//so need to 'prime' the gl stack with the viewTM
		node->getViewTM(m_viewTM);
		glLoadMatrixf(m_viewTM.m_mat);
	}//if (node)
}//void GLRenderer::transformViewport(AWTransformController* node)



void
GLRenderer::applyMaterial(AWMaterial* mat)
{
   float stupid[] = {0.0f, 0.0f, 0.0f, 1.0f};
   GLenum faces = GL_FRONT_AND_BACK;
	if (mat && !mat->getUseDefaults())
	{
      if (m_selectedMaterial != mat)
      {
         AWTRACEHI1("Apply Material %i\n", (int)mat);
         #ifdef __AWTRACEHI
         mat->dump(); 
         #endif
		   m_selectedMaterial = mat;
		   faces = mat->m_twoSided ? GL_FRONT_AND_BACK : GL_FRONT;
       		   
		   mat->m_ambient.assignToFloat4(stupid);
		   glMaterialfv(faces, GL_AMBIENT, stupid);

		   mat->m_diffuse.assignToFloat4(stupid);
		   glMaterialfv(faces, GL_DIFFUSE, stupid);

		   mat->m_specular.assignToFloat4(stupid);
		   glMaterialfv(faces, GL_SPECULAR, stupid);

		   mat->m_emissive.assignToFloat4(stupid);
		   glMaterialfv(faces, GL_EMISSION, stupid);
		   glMaterialf(faces,  GL_SHININESS, mat->m_shininess);
      }//if (m_selectedMaterial != mat)
	}//if (mat)
   else if (m_selectedMaterial)//must apply default values
   { 
      AWTRACEHI("Apply Default Material settings\n");
      m_selectedMaterial = NULL;
      float shiny = 0.0f;
      stupid[0]=0.2f; stupid[1]=0.2f; stupid[2]=0.2f; stupid[3]=1.0f;
      glMaterialfv(faces, GL_AMBIENT, stupid);

		stupid[0]=0.8f; stupid[1]=0.8f; stupid[2]=0.8f; stupid[3]=1.0f;
		glMaterialfv(faces, GL_DIFFUSE, stupid);

		stupid[0]=0.0f; stupid[1]=0.0f; stupid[2]=0.0f; stupid[3]=1.0f;
		glMaterialfv(faces, GL_SPECULAR, stupid);
      glMaterialfv(faces, GL_EMISSION, stupid);		
		glMaterialf(faces,  GL_SHININESS, shiny);
   }
}//void GLRenderer::applyMaterial(AWMaterial* mat)



void
GLRenderer::applyTextureState(AWTextureUnit* texUnit, int unitID)
{  //ensure gl call made only if mode *CHANGES*
   m_glErr = glGetError();
   if (!texUnit || !texUnit->m_tex) return;

   AWTexture* tex    = texUnit->m_tex;
   setTexFunc(*texUnit, unitID);
   setTexGenMode(tex->m_genMode, unitID);
   //now alter TEXTURE MATRIX if required
   AWMatrix4 texMatrix; //constructor inits to identity
   if ( ((AWTexture::REFLECT == tex->m_genMode)||(AWTexture::SPHERE == tex->m_genMode)) 
          && m_transformReflections)
   {  //NOTE - can rotate an (s,t,r) vector from one space to another:
      //if cube map texture is oriented in world space
      //and M is the matrix transform that moves from world space
      //to eye coordinates, you can load the inverse of the
      //affine portion of M into the texture matrix to
      //rotate eye-space reflection or normal vectors generated by
      //GL_REFLECTION_MAP_EXT or GL_NORMAL_MAP_EXT
      //back into world space.

      //***************BUG - NVIDIA BUG***********
      //DISPLAY LISTS CRASH...
      texMatrix = m_viewTM;
      texMatrix.invertAffine(); 
      setTexMatrix(texMatrix, unitID);
   }//if ((AWTexture::REFLECT == tex->m_genMode) && m_transformReflections)
   else
   {
      setTexMatrixIdentity(unitID);
   } 
   m_glErr = glGetError();
}//void GLRenderer::applyTextureState(AWTextureUnit* texUnit, int unitID)



void           
GLRenderer::selectTexture(AWTexture* tex, int unitID)
{
   if (!tex) return;
   int  texID  = tex->getTextureID(); 
   if (glActiveTexture) 
   {
      AWTRACEHI1("Activate texUnit: %i - ", texUnit);
      glActiveTexture(GL_TEXTURE0_ARB + unitID);
   }
   int test = getSelectedTex(unitID);
   if ( (test != texID) || (AW_NO_TEXTURE == getTexMode(unitID)) )
	{
		setSelectedTex(texID, unitID);
      setTexMode(tex->m_target, unitID);
		glBindTexture(tex->m_target, texID);
      AWTRACEHI3(" bind %s ID %d %s\n", tex->getTargetName(), texID, tex->m_name.c_str());  
	}//if ( (test != texID) || (AW_NO_TEXTURE == getTexMode(unitID)) )
}//void GLRenderer::selectTexture(AWTexture* tex, int unitID)



void
GLRenderer::disableTexUnits(int startunit)
{
   for (int i=startunit; i<getNumTexUnits(); i++)
   { 
      AWTRACEHI1("Disable texUnit %i\n", i);
      if (glActiveTexture) 
      {
         glActiveTexture(GL_TEXTURE0_ARB + i);
         setTexMode(AW_NO_TEXTURE, i);
         setTexGenMode(AWTexture::G_NONE, i);
         setSelectedTex(-1, i);
         setTexCoordArray(NULL, i, 2);   
      }//if (glActiveTexture)
   }//for (i=numTex; i<MAX_TEX_UNITS; i++)
}//void GLRenderer::disableTexUnits(int startunit)



void
GLRenderer::setTextureState(AWRenderPass* pass)
{
   //set correct states for as many textures as we can render in
   //given render pass. Max number == NUM TEX UNITS for given platform
   AWTRACEHI("\nsetTextureState\n");
   //we need to be able to determine the correct textures for
   //UNIT0 and UNIT1 for current render pass
   int            i           = 0;
   AWTextureUnit* texUnit     = NULL;
   int            numTexUnits = pass->getNumTexUnits();  
   for (i=0; i<numTexUnits; i++)
   {    
      texUnit = pass->getTexUnit(i);
      if (texUnit)
      {
         selectTexture(texUnit->m_tex, i);
         applyTextureState(texUnit, i);
		   m_glErr = glGetError();
      }//if (tex)
   }//for (int i=0; i<numTex; i++)
   disableTexUnits(numTexUnits);
   AWTRACEHI("\n");
}//void GLRenderer::setTextureState(AWRenderPass* pass)



int
GLRenderer::getNumPasses(AWObject* o)
{
   int retVal = 0;
   if (o)
   {
      retVal = o->getNumPasses();
      AWTRACEHI2("\n\n\nOBJECT: %s has %i passes\n", o->getName().c_str(), retVal);
      if (0==retVal)
      {  //first attempt at drawing results in preparation
         //for rendering. Make sure this path is followed for
         //all object types        
         m_effectMgr.prepareForRendering(*this, o);
         retVal = o->getNumPasses();
         if (0==retVal)
         {
            o->setIsVisible(FALSE);
         }
         AWTRACE1(" - prepared for rendering with %d passes\n", retVal);
      }//if (0==retVal)
      AWTRACEHI("\n");
   }//if (o)
   return retVal;
}//int GLRenderer::getNumPasses(AWObject* o)



void
GLRenderer::setBlendState(AWRenderPass* pass)
{
   if (!pass) return;
   float opacity   = pass->getOpacity();
   int   blendType = pass->getBlendType();
   int   srcFac    = pass->getSrcFac();
   int   dstFac    = pass->getDstFac();
   int   depthFunc = pass->getDepthFunc();

   setDepthFunc(depthFunc);
   if ((1.0f != opacity) && (100.0f != opacity))
   {  //we need to perform blending
      enableBlend(TRUE);
      //specified constant factor
      glBlendColorEXTAW(opacity, opacity, opacity, 1.0f);
      setBlendFac(GL_CONSTANT_COLOR_EXT, GL_ONE_MINUS_CONSTANT_COLOR_EXT);
   }//if (100!=opacity)
   else if (AWTexture::ALPHA_BLEND == blendType)
   {
      enableBlend(TRUE);
      setBlendFac(srcFac, dstFac);
   }//else if (AWTexture::ALPHA_BLEND == blendType)
   else
   {
      enableBlend(FALSE);
   }
   setAlphatestEnabled(AWTexture::ALPHA_TEST == blendType);
   combinerMode(pass->getCombinerMode());
}//void GLRenderer::setBlendState(AWRenderPass* pass)



void
GLRenderer::drawObject(AWObject* theObj)
{
   if (theObj && theObj->getIsVisible())
	{
      int numPasses = getNumPasses(theObj);
      if (0==numPasses) return;
      if (!m_applyObjectTextures) numPasses = 1;

      m_trisThisFrame += theObj->getNumTris();
      glPushMatrix();			//preserve the viewTM
      transformNode(theObj);	//apply the object wsTM

      AWRenderPass* curPass = NULL;
      for (int pass=0; pass<numPasses; pass++)
      {
         AWTRACEHI1("\n\nRendering PASS %i\n\n", pass);
         curPass = theObj->setPass(pass); 

         if (m_applyObjectTextures)
         {
            setBlendState(curPass); 
            setTextureState(curPass);
         }//if (m_applyObjectTextures)

         AWTRACEHI1("Drawing: %s\t\t", theObj->getName().c_str());
         if ( m_useDisplayLists && (AW_NOLIST != theObj->getListID()) )
         {
            AWTRACEHI1(" - DList %d\n", (int)theObj->getListID());
            glCallList(theObj->getListID());
         }
         else if (m_useVertArrays && theObj->getIndexArray())
         {
            AWTRACEHI(" - VertArray\n");
#ifdef __AWTRACEHI
            if (pass>0)
            {
               AWTRACEHI1("  rendering pass %i\n", (int)pass);
            }
#endif
            drawObjectAsVertArray(theObj, curPass);
         }
         else
         {
            AWTRACEHI(" - Immediate\n");
            drawObjectImmediate(theObj, curPass);
         }
      }//for (int pass=0; pass<theObj->getNumPasses(); pass++)
      if (m_drawNormals)
      {
         drawObjectNormals(theObj);
      }//if (m_drawNormals)

      glPopMatrix();	//restore the viewTM
      AWTRACEHI1("\nFinished drawing %s\n\n", theObj->getName().c_str());
   }//if (theObj && theObj->getIsVisible())
}//void GLRenderer::drawObject(AWObject* theObj)




void
GLRenderer::drawObjectNormals(AWObject* theObj)
{  // Draw Normals
   if (theObj && theObj->getIndexArray())
   {
      AWTRACEHI1("Draw Normals for %s\n", theObj->getName().c_str());
      float normalScale = 0.1f * theObj->getRadius();
      setDepthFunc(GL_LEQUAL);
      enableBlend(FALSE);
      disableTexUnits();     
      if (usingLighting()) 
      {
         AWTRACEHI("Disable lighting\n");
         glDisable(GL_LIGHTING);
      }//if (usingLighting()) 
      if (getCombinersEnabled()) 
      {
         combinerMode(AWRenderPass::AW_CM_NONE);
      }//if (getCombinersEnabled()) 
      glLineWidth(3.0f);
      glBegin(GL_LINES);
      for (int index =0; index < theObj->getNumIndeces(); index++)
      {
         AWPoint v( theObj->getVertex( theObj->getIndex(index) ) );
         glColor3f(0.0f, 0.0f, 1.0f);
         glVertex3f(v.x, v.y, v.z);
         v += normalScale * theObj->getNormal( theObj->getIndex(index) );
         glColor3f(1.0f, 0.0f, 0.0f);
         glVertex3f(v.x, v.y, v.z);
         m_glErr = glGetError();
      }//for (int index =0; index < theObj->getNumIndeces(); index++)
      glEnd();
      glLineWidth(getLineWidth());
      if (usingLighting()) 
      {
         AWTRACEHI("Enable lighting\n");
         glEnable(GL_LIGHTING);
      }//if (usingLighting())
   }//if (theObj && theObj->getIndexArray())
}//void GLRenderer::drawObjectNormals(AWObject* theObj)



void
GLRenderer::showAllAlphaTestObjects()
{	//list is sorted in ascending order of depth, so need to show it the
	//other way round (furtherst away FIRST)
   AWTRACE1("\nGLRenderer::showAllAlphaTestObjects for %i objects\n", 
               (int)getScene().m_nodeList.getNumAlphaTestObjects());
	int curObjIndex = getScene().m_nodeList.getNumAlphaTestObjects()-1;
	while (0 <= curObjIndex)
	{
		AWObject* curObj = getScene().m_nodeList.getAlphaTestObject(curObjIndex);
      drawObject(curObj);
		curObjIndex--;
	}//while (curObjIndex < (int)m_objects.entries())
}//void	GLRenderer::showAllAlphaTestObjects()



void
GLRenderer::showAllTransparentObjects()
{	//list is sorted in ascending order of depth, so need to show it the
	//other way round (furtherst away FIRST)
   AWTRACE1("\nGLRenderer::showAllTransparentObjects for %i objects\n", 
               (int)getScene().m_nodeList.getNumTransparentObjects());
	int curObjIndex = getScene().m_nodeList.getNumTransparentObjects()-1;
	while (0 <= curObjIndex)
	{
		AWObject* curObj = getScene().m_nodeList.getTransparentObject(curObjIndex);
      drawObject(curObj);
		curObjIndex--;
	}//while (curObjIndex < (int)m_objects.entries())
}//void	GLRenderer::showAllTransparentObjects()



void
GLRenderer::showAllOpaqueObjects()
{
   AWTRACE1("\nGLRenderer::showAllOpaqueObjects for %i objects\n", 
               (int)getScene().m_nodeList.getNumOpaqueObjects());
	int curObjIndex = 0;
	while (curObjIndex < getScene().m_nodeList.getNumOpaqueObjects())
	{
		AWObject* curObj = getScene().m_nodeList.getOpaqueObject(curObjIndex);
      drawObject(curObj);
		curObjIndex++;
	}//while (curObjIndex < (int)m_objects.entries())
}//void	GLRenderer::showAllOpaqueObjects()



void
GLRenderer::applyObjectState(AWObject* theObj)
{
   if (theObj->isTwoSided()) glDisable(GL_CULL_FACE);
   if (usingLighting() && !theObj->getDynamicLighting()) 
   {
      AWTRACEHI("Disable GL_LIGHTING\n");
      glDisable(GL_LIGHTING);
   }
   else if (m_hasSeparateSpecular && theObj->useSeparateSpecular())
   {
      glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
   }
   if (theObj->wireframe())
   {
      m_curPolyMode = getPolygonMode();
      setPolygonMode(GL_LINE);
   }//if (theObj->wireframe())
}//void GLRenderer::applyObjectState(AWObject* theObj)



void
GLRenderer::restoreObjectState(AWObject* theObj)
{
   if (theObj->isTwoSided()) glEnable(GL_CULL_FACE);
   if (usingLighting() && !theObj->getDynamicLighting()) 
   {
      AWTRACEHI("Enable GL_LIGHTING\n");
      glEnable(GL_LIGHTING);
   }
   else if (m_hasSeparateSpecular && theObj->useSeparateSpecular())
   {
      glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR_EXT);
   }
   if (theObj->wireframe())
   {  //revert to previous polygon mode for rest of scene
      setPolygonMode(m_curPolyMode);
   }//if (theObj->wireframe())
}//void GLRenderer::restoreObjectState(AWObject* theObj)



void
GLRenderer::drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass, AWBoolean useCVAEXT)
{
   if (theObj && theObj->getIndexArray() && pass)
   {
      applyMaterial(theObj->getMaterial());
      applyObjectState(theObj);
      //without vertex colors, we should use white to blend with textures...
      if (!theObj->m_vertColList) glColor3f(1, 1, 1);
      //enable relevant array capabilities
      setVertexArray(theObj->m_vertexList);
      setVertexColorArray(theObj->m_vertColList);
      setNormalArray(theObj->m_normalList);
		//texture coordinate arrays - note that for units not used this pass
      //the tex cooord arrays disabled by passing (NULL, unit, 0)
      for (int unit=0; unit<getNumTexUnits(); unit++)
      {
         if (glClientActiveTexture) glClientActiveTexture(GL_TEXTURE0_ARB+unit);
         setTexCoordArray(m_applyObjectTextures?theObj->getTVertexList(unit):NULL, unit, theObj->getTVertexListDim(unit));   
      }
      //useCVAEXT will be FALSE when compiling
		if (m_useCVAs && useCVAEXT && glLockArrays && glUnlockArrays) 
      {
         AWTRACEHI1("glLockArrays(0, %i)...", (int)theObj->getNumVerts());
         glLockArrays(0, theObj->getNumVerts());
      }//if (m_useCVAs && glLockArrays && glUnlockArrays)

      glDrawElements(GL_TRIANGLES, theObj->getNumIndeces(), GL_UNSIGNED_INT, theObj->getIndexArray());

		if (m_useCVAs && useCVAEXT && glLockArrays && glUnlockArrays) 
      {
         AWTRACEHI("  glUnlockArrays()\n");
         glUnlockArrays();
      }//if (m_useCVAs && glLockArrays && glUnlockArrays) 
      restoreObjectState(theObj);
	}//if (theObj)
}//void GLRenderer::drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass, AWBoolean useCVAEXT)



void
GLRenderer::applyTexCoordsImmediate(AWObject* theObj, AWRenderPass* pass, int curVertIndex)
{
   if (!pass) return;
   int    num  = pass->getNumTexUnits();
   float* data = NULL;
   int    dim  = 0;
   for (int unit=0; unit < num; unit++)
   {
      dim  = theObj->getTVertexListDim(unit);
      data = theObj->getTVertexList(unit);     
      if (data && dim)
      {
         data += curVertIndex * dim;    
         if (num>1)
         {
            if (3==dim)
            {
               glMultiTexCoord3fv(GL_TEXTURE0_ARB + unit, data);
               AWTRACEHI4("\tunit%d glMultiTexCoord3fv(%f, %f, %f)\n",
                           unit, (double)data[0], (double)data[1], 
                           (double)data[2]);
            }//if (3==dim)
            else if (2==dim)
            {  //default is 2 dimensional texture coordinates
               glMultiTexCoord2fv(GL_TEXTURE0_ARB + unit, data);
               AWTRACEHI3("\tunit%d glMultiTexCoord2fv(%f, %f)\n",
                           unit, (double)data[0], (double)data[1]);  
            }//else if (2==dim)
         }//if (num>1)
         else
         {
            if (3==dim)
            {
               glTexCoord3fv(data);
               AWTRACEHI3("\tglTexCoord3fv(%f, %f, %f)\n", 
                           (double)data[0], (double)data[1], 
                           (double)data[2]);
            }//if (3==dim)
            else if (2==dim)
            {  //default is 2 dimensional texture coordinates
               glTexCoord2fv(data);
               AWTRACEHI2("\tglTexCoord2fv(%f, %f)\n", 
                           (double)data[0], (double)data[1]);
            }//else if (2==dim)
         }//else
      }//if (data && dim)
   }//for (int unit=0; unit < num; unit++)
}//void GLRenderer::applyTexCoordsImmediate(AWObject* theObj, int curVertIndex)



void		
GLRenderer::drawObjectImmediate(AWObject* theObj, AWRenderPass* pass)
{
	if (theObj)
	{
		applyMaterial(theObj->getMaterial());
      int curVertIndex = -1;
      int numIndeces = theObj->getNumIndeces();
		applyObjectState(theObj);
		//loop through faces in object and draw the corresponding triangles	
		if (!theObj->m_vertColList) glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);
      for (int i=0; i< numIndeces; i++)
      {
         curVertIndex = theObj->getIndex(i);
         if (theObj->m_normalList)
			{
            AWTRACEHI4("index %i\tglNormal3fv(%f, %f, %f)\n", curVertIndex, 
               (double)theObj->getNormal3fv(curVertIndex)[0], 
               (double)theObj->getNormal3fv(curVertIndex)[1], 
               (double)theObj->getNormal3fv(curVertIndex)[2]);
				glNormal3fv(theObj->getNormal3fv(curVertIndex));
			}
         //for production code should inline the following function or just 
         //directly include in this section
         applyTexCoordsImmediate(theObj, pass, curVertIndex);   
			if (theObj->m_vertColList)
			{
            AWTRACEHI4("index %i\tglColor3ubv(%u, %u, %u)\n", curVertIndex, 
               (unsigned int)theObj->getColour3ubv(curVertIndex)[0], 
               (unsigned int)theObj->getColour3ubv(curVertIndex)[1], 
               (unsigned int)theObj->getColour3ubv(curVertIndex)[2]);
				glColor3ubv(theObj->getColour3ubv(curVertIndex));
			}
         glVertex3fv( theObj->getVertex3fv(curVertIndex) );
         AWTRACEHI4("index %i\tglVertex3fv(%f, %f, %f)\n", curVertIndex, 
               (double)theObj->getVertex3fv(curVertIndex)[0], 
               (double)theObj->getVertex3fv(curVertIndex)[1], 
               (double)theObj->getVertex3fv(curVertIndex)[2]);
      }//for (int i=0; i< numIndeces; i++)
      glEnd();
		restoreObjectState(theObj);
	}//if (theObj)
}//void	GLRenderer::drawObjectImmediate(AWObject* theObj)
 


AWBoolean
GLRenderer::usesCubeMap(AWObject* obj)
{  //ASSUME: we know we have (at least) one pass 
   //if we enter this function
   AWBoolean retVal = FALSE;
   AWRenderPass* pass = obj->getPass(0);
   for (int i=0; i<pass->getNumTexUnits(); i++)
   {
      AWTextureUnit* tUnit = pass->getTexUnit(i);
      retVal = (tUnit && tUnit->m_tex && (CLSID_AWCUBEENVTEXTURE==tUnit->m_tex->isA()));
      if (retVal) break;
   }//for (int i=0; i<pass->getNumTexUnits(); i++)
   return retVal;
}//AWBoolean GLRenderer::usesCubeMap(AWObject* obj)



void
GLRenderer::compileObjectCurMesh(AWObject* obj, int& listID)
{	//NOTE - objects can share meshes/faces (CLONES)
	//in these cases want to re-use the Display List multiple times,
	//rather then generating a new display list for each cloned object
	//so query the mesh directly for its ID, which will be -1 iff no
	//corresponding display list has yet been generated
	if (obj && obj->m_curMesh)
	{
      int numPasses = getNumPasses(obj);
      if ((1!=numPasses) || usesCubeMap(obj)) return;
      AWRenderPass* pass = obj->setPass(0);

		obj->calcMesh(0);//make sure we pick up any listID from shared resources
      //this will work only when ONLY ONE list per object
      int objListID = (-1 == obj->getListID()) ? AW_NOLIST : obj->getListID();
		if (AW_NOLIST == objListID)
		{
			glPushMatrix();
			transformNode(obj);//NOT required, most likely
         AWTRACE2("Compiling %s with Display List ID: %i -", obj->getName().c_str(), listID);
			//setBlendState(obj); 
         //textureObject(obj);
         glNewList(listID, GL_COMPILE);
         if (m_useVertArrays && obj->getIndexArray())
         {
            AWTRACE(" rendering using VERTEX ARRAYS\n");
            //FALSE second parameter disables use of CVAs even 
            //if selected in the GUI/configuration
            drawObjectAsVertArray(obj, pass, FALSE);
         }
         else
         {
            AWTRACE(" rendering using immediate GL_TRIANGLES\n");
            drawObjectImmediate(obj, pass);
         }
			glEndList();
			glPopMatrix();
			
			objListID = listID;
		}//if (AW_NOLIST == objListID)
		obj->setListID(objListID);//sets the listID on m_curMesh also
	}//if (obj && obj->m_curMesh)
}//void GLRenderer::compileObjectCurMesh(AWObject* obj, int& listID)



int	
GLRenderer::compileObject(AWObject* obj)
{  
   int listID = AW_NOLIST;
   if (obj)
   {
      //obj->setListID(AW_NOLIST);//ensure list support initially turned OFF
      if (obj->isMeshAnimated())
      {  //really tells us there is more than one mesh set for the
         //object Check to see if any of these Mesh sets are in fact
         //not animated, ie. just a single frame. In this case we
         //can use a display list to optimize
         AWAnimatedMesh* nextMesh=NULL;
         for (int i=0; i < obj->m_animMeshes.getNum(); i++)
         {
            nextMesh = (AWAnimatedMesh*)obj->m_animMeshes[i];
            //the inclusion of (1 == nextMesh->getNumFrames()) is a hack
	         //designed to prevent any objects with animated meshes
	         //from using Display Lists -  need one per-frame,
	         //don't support that yet?
            if (nextMesh && (1 == nextMesh->getNumFrames()))
            {//we've found a suitable mesh for a Display List
               obj->setAnimation(i);
               listID = glGenLists(1);
               compileObjectCurMesh(obj, listID);
            }//if (nextMesh && (1 == nextMesh->getNumFrames()))
         }//for (int i=0; i < (int)obj->m_animMeshes.entries(); i++)
      }//if (obj->isMeshAnimated())
      else
      {  //mesh is not animated - YES we can use a display list
         //just one for this object
         listID = glGenLists(1);
         compileObjectCurMesh(obj, listID);
      }//else
   }//if (theObj)
   return listID;
}//int GLRenderer::compileObject(AWObject* obj)



//COMPILED or DISPLAY LIST VERSIONS - should be more cleanly reconciled into
//the codebase above making better use of OO design
AWBoolean	
GLRenderer::compileAllObjects(AWBoolean call, AWBoolean force)
{  //first need to determine if the implementation supports display lists
   //generate a single list to test - return value of ZERO indicates
   //display lists not supported
   AWBoolean retVal = FALSE;
   if (!m_useDisplayLists) return retVal;
   m_sceneListBase   = glGenLists(1);  
   if (m_sceneListBase)
   {
      glDeleteLists(m_sceneListBase, 1);
      retVal = TRUE;
      //iterate through each AWObject in scene and compile
      AWObject*	obj	         = NULL;  
      for (int i=0; i<getScene().m_nodeList.getNumObjects(); i++)
      {
         obj = getScene().m_nodeList.getObject(i);
         compileObject(obj);
      }//for (int i=0; i<getNumObjects(); i++)
      
		//do projectiles, if any and one for each projectile, if any
      for (int proj=0; proj < getScene().m_nodeList.getNumProjectiles(); proj++)
      {
         AWRocket* nextProj = getScene().m_nodeList.getProjectile(proj);
         compileObject(nextProj);
      }//for (int proj=0; proj < getScene().m_nodeList.getNumProjectiles(); proj++)
      //now compile the environment
      AWObject* env = getScene().m_nodeList.getEnvironment();
      if (env) env->setListID(AW_NOLIST);
      if (env  && !env->isMeshAnimated())
      {
         compileObject(env);
         AWTRACE2("Compiling Environment %s with Display List ID: %i \n", env->getName().c_str(), env->getListID());
      }//if (env  && !env->isMeshAnimated())
	}//if (m_sceneListBase)
   int temp;
   glGetIntegerv(GL_LIST_BASE, &temp);
   m_sceneListBase = temp;
	return retVal;
}//AWBoolean GLRenderer::compileAllObjects(AWBoolean force)



void		
GLRenderer::switchRecordMode()
{
	m_time = 0;
	m_perfCntr.zeroTime();
	getScene().m_nodeList.init(0, *this);
	getScene().m_nodeList.getTransformer()->switchRecordMode();
	//getScene().switchCameraRecordMode();
}//void	GLRenderer::switchRecordMode()


void		
GLRenderer::switchPlaybackMode()
{
	m_time = 0;
	m_perfCntr.zeroTime();
	getScene().m_nodeList.init(0, *this);
	//getScene().switchCameraPlaybackMode();
	getScene().m_nodeList.getTransformer()->switchPlaybackMode();
}//void	GLRenderer::switchPlaybackMode()



void
GLRenderer::showRenderStatistics()
{
	//char msg[400];
	//sprintf(msg, "Min. FPS: %2.0f \n(ms):  render %2.2f, transform %2.2f, collide %2.2f\n\nMax. FPS: %2.0f\n(ms): render %2.2f, transform %2.2f, collide %2.2f\n\nLast FPS: %2.0f for %i triangles\n(ms): render %2.2f, transform %2.2f, collide %2.2f", 
	//show minimum values
	/*
	m_perfCntr.getMinFrequency(),
	m_perfCntr.m_maxRenderCnt,
	m_perfCntr.m_maxTransformCnt,
	m_perfCntr.m_maxCollideCnt,
	//show maximum values
	glScene.m_perfCntr.getMaxFrequency(),
	m_perfCntr.m_minRenderCnt,
	m_perfCntr.m_minTransformCnt,
	m_perfCntr.m_minCollideCnt,*/
	//show current framerate
	char msg[300];
	sprintf(msg, "FPS: %3d for %6i triangles: rendered in  %2.2fms  awTransform %2.2fms",
	(int)m_perfCntr.getFrequency(),
	//show number of triangles sent to OpenGL
	getNumRenderedTriangles(),
	//get ms data for render/transform/collide
	m_perfCntr.m_renderCnt.m_diff*1000.0f,
	m_perfCntr.m_transformCnt.m_diff*1000.0f);
	//m_perfCntr.m_collideCnt.m_diff*1000.0f);
	outputString(msg);
}//void GLRenderer::showRenderStatistics()



//NVIDIA TIPS for efficient use of REGISTER COMBINERS
//   o  Changing some or all the register combiner state is not much
//      more expensive than changing just a single piece of register
//      combiner state.  If any register combiner state is updated, the
//      OpenGL driver simply marks the register combiners hardware state
//      "dirty" and re-validates the complete register combiners state at
//      the next glBegin.  The revalidation process (in the current driver)
//      updates the complete register combiner hardware state.

//   o  Register combiner state validatations are not substantially more
//      expensive than validating core OpenGL texture environment state.

//   o  It makes sense to collect all your application's various
//      combiner configurations into a single register combiners
//      validation infrastructure.  To switch combiner modes, just call
//      a single routine to configure a particular enumerated combiner
//      configuration.

//      That is the approach used in this example.

//   o  If you have multiple objects that are rendered with the same set
//      of multiple passes using different register combiners
//      configurations, render all the objects that share a particular
//      register combiners configuration before changing the the register
//      combiners configuration for a different pass.

//   o  Be sure to comment code that programs register combiner
//      configurations.  Otherwise, you will find it very hard to remember
//      what a particular register combiner configuration is supposed to
//      accomplish.



void
GLRenderer::combinerMode(int mode)
{
   switch (mode) 
   {
   case AWRenderPass::AW_CM_NONE:
      AWTRACEHI("Disable register combiners\n");
      setCombinersEnabled(0);
      glDisable(GL_REGISTER_COMBINERS_NV);
      /* //equivalent to the above, except for alpha?
      glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
      glFinalCombinerInputNV(GL_VARIABLE_A_NV,
      GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
      glFinalCombinerInputNV(GL_VARIABLE_B_NV,
      GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      glFinalCombinerInputNV(GL_VARIABLE_D_NV,
      GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      glFinalCombinerInputNV(GL_VARIABLE_G_NV,
      GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
      glEnable(GL_REGISTER_COMBINERS_NV);*/
      break;

   case AWRenderPass::AW_CM_BUMPED:
      AWTRACEHI3("Enable register combiners for AW_CM_BUMPED with ambient: (%f, %f, %f)\n", (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
      setCombinersEnabled(1); //actually used to track on/off only
      glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);

      glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, m_ambient);
      //glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, m_ambient);

      // Argb = 3x3 matrix column1 = expand(texture0rgb) = N'
      glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV,
                        GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
      // Brgb = expand(texture1rgb) = L 
      glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV,
                        GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);

      // spare0rgb = Argb dot Brgb = expand(texture0rgb) dot 
      //    expand(texture1rgb) = L dot N'  
      glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB,
                         GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
                         GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);

      // Aa = 1  
      glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV,
                        GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
      // Ba = expand(texture1b) = Lz  
      glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV,
                        GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);
      // Ca = 1  
      glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV,
                        GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
      // Da = expand(texture1b) = Lz  
      glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV,
                        GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);

      // spare0a = 4*(1*Lz + 1*Lz) = 8*expand(texture1a)  
      glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA,
                         GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,
                         GL_SCALE_BY_FOUR_NV, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

      // A = spare0a = per-pixel self-shadowing term  
      glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_SPARE0_NV, 
                             GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
      glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, 
                             GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, 
                             GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // G = spare0a = diffuse illumination contribution = L dot N' 
      glFinalCombinerInputNV(GL_VARIABLE_G_NV,
                             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
      //final stage computes: A*B + (1-A)*C + D
      glEnable(GL_REGISTER_COMBINERS_NV);
      break;

   case AWRenderPass::AW_CM_BUMPED_SPECULAR:
      AWTRACEHI("Enable register combiners for AW_CM_BUMPED_SPECULAR");
      setCombinersEnabled(2); //actually used to track on/off only
      glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
    
      // GENERAL Combiner ZERO, RGB portion.  
      // Argb = 3x3 matrix column1 = expand(texture0rgb) = N'  
      glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV,
                        GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
      // Brgb = expand(texture1rgb) = L (or H if specular)  
      glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV,
                        GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);

      // spare0rgb = Argb dot Brgb = expand(texture0rgb) dot 
      //    expand(texture1rgb) = H dot N' 
      glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB,
                         GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
                         GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);

      // Aa = texture1b = unexpanded Lz (or Hz if specular)   
      glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV,
                        GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
      // Ba = 1 
      glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV,
                        GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);

      // spare0a = 1 * texture1b = unexpanded Lz (or Hz if specular)  
      glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA,
                         GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV,
                         GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

      // GENERAL Combiner ONE, RGB portion.  
      // Argb = 0  
      glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV,
                        GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // Brgb = 0  
      glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV,
                        GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // Crgb = spare0rgb = H dot N'  
      glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV,
                        GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // Drgb = spare0rgb = H dot N' 
      glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV,
                        GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB);

      // spare0rgb = ((spare0a >= 0.5) ? spare0rgb^2 : 0) = 
      //    ((H dot N > 0) ? (H dot N')^2 : 0)  
      glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB,
                         GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV,
                         GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_TRUE);

      // GENERAL Combiner ONE, Alpha portion.  
      // Discard all outputs  
      glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, 
                         GL_DISCARD_NV, GL_DISCARD_NV, GL_DISCARD_NV, 
                         GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
      
      // FINAL Combiner. 
      // A = EF 
      glFinalCombinerInputNV(GL_VARIABLE_A_NV,
                             GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // B = EF  
      glFinalCombinerInputNV(GL_VARIABLE_B_NV,
                             GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // C = zero  
      glFinalCombinerInputNV(GL_VARIABLE_C_NV,
                             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // D = zero = no extra specular illumination contribution  
      glFinalCombinerInputNV(GL_VARIABLE_D_NV,
                             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // E = spare0rgb = diffuse illumination contribution = H dot N' 
      glFinalCombinerInputNV(GL_VARIABLE_E_NV,
                             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
      // F = spare0rgb = diffuse illumination contribution = H dot N' 
      glFinalCombinerInputNV(GL_VARIABLE_F_NV,
                             GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
    
      // specular RGB color = A*E*F     = specular modulated by 
      //   self-shadowing term 
      // G = spare0a = diffuse illumination contribution = H dot N' 
      glFinalCombinerInputNV(GL_VARIABLE_G_NV,
                             GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
      glEnable(GL_REGISTER_COMBINERS_NV);
      break;
   }//switch (mode) 
   AWTRACEHI1("Number of Combiners enabled: %i\n", (int)getCombinersEnabled());
}//void GLRenderer::enableCombiners(int mode)