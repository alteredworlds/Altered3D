#ifndef __GLRENDERER_H
#define __GLRENDERER_H

#ifndef _WIN32
#include <GL/gl.h>
#else

#endif

#include "AWObject.h"
#include "AWRocket.h"
#include "AWScene.h"
#include "AWWorker.h"
#include "AWTexture.h"
#include "AWLight.h"
#include "AWPerfCnt.h"
#include "AWImageProcessor.h"
#include "AWCubeEnvTexture.h"
#include "AWNormalMapTexture.h"
#include "AWEffectMgr.h"
#include "AWRenderState.h"
#include "AWPBuffer.h"
#include "AWShadowMap.h"



class AWExport AWQuat;	 //forward
class AWExport AWMaterial; //forward

class AWExport AWRenderer
{
};


class AWExport GLRenderer : public AWRenderer, public AWRenderState
{
public:
   

public:
   //static GLRenderer&   getRenderer();
   //static void          destroyRenderer();
   static void*         createContextAndSelect(void* hdc);
   static void*         destroyContext(void* hrc);
   static AWBoolean     setupPixelFormat(void* hdc, int bpp);
   static void          makeCurrent(void* hdc, void* hrc);
   static void*         getFunctionByName(char* n);

   //helper functions to change/track OpenGL state
   //top level init/resize/shutdown
   void		            initialize(int width, int height);	
   void		            resizeGL(int width, int height);
   void		            shutdown();	
   void                 resetData();
   void                 setScene(AWScene& s)             {m_scene=&s;}

   const AWCString&     getVendorName()                {return m_driverVendorName;}

   void		            togglePause();
   AWBoolean	         getPaused()						      {return m_paused;}

   AWCamera*            getHelperCam()       {if (!m_helperCam) m_helperCam=new AWCamera;return m_helperCam;}
 
   void		            toggleTransformReflections()     {m_transformReflections=!m_transformReflections;}
   AWBoolean	         getTransformReflections()			{return m_transformReflections;}

   void                 toggleDrawNormals()              {m_drawNormals=!m_drawNormals;}
   AWBoolean	         getDrawNormals()						{return m_drawNormals;}

   void                 toggleShowTextures()             {m_showTextures=!getShowTextures();}
   AWBoolean	         getShowTextures()						{return m_showTextures && getScene().m_textureList.getNum();}

   void                 togglePolygonMode();

//note that the following 2 bools are used to communicate from
//AWWorker to AWSpaceGnats - should really be done a little differently
   void                 setSwitchLevel(AWBoolean v=TRUE) {m_switchLevel=v;}
   void                 setLevelFailed(AWBoolean v=TRUE) {m_levelFailed=v;}
   AWBoolean            getSwitchLevel()                 {return m_switchLevel;}
   AWBoolean            getLevelFailed()                 {return m_levelFailed;}
   AWBoolean            getTimeToExplode()               {return m_timeToExplode;}
   void                 setTimeToExplode(AWBoolean v=TRUE) {m_timeToExplode=v;}

   void                 initTimeBar(float t);
   void                 pause();
   AWBoolean            usesCubeMap(AWObject* obj);

	//helpers
	void		            initializeGL();
	void		            initializeObjects();
   
   int                  getInternalFormat(int texFormat, AWBoolean allowCompression=TRUE);
   AWBoolean	         initTextures();
   void                 initTexture(AWTexture* tex);
   void                 init2DTexture(AWTexture* tex);
   void                 init1DTexture(AWTexture* tex);
   void                 initCubeEnvTexture(AWCubeEnvTexture* f, AWBoolean allowCompression=TRUE);
   void                 initNormalMapTexture(AWNormalMapTexture* tex);
   void                 initShadowMap(AWShadowMap* m);
   AWBoolean            setupTexture(AWTexture* t, AWBoolean allowAutoMipmap=TRUE);

   AWPBuffer*           getShadowBuffer(int size=512);
   AWTexture*           get1DProjectHelper();
   AWCubeEnvTexture*    generateNormalizeCube(int size);
   AWCubeEnvTexture*    getNormalizeCube()            {return m_normalizeCube;}
   void                 setNormalizeCube(AWCubeEnvTexture* t) {m_normalizeCube=t;}

   void                 buildDynamicMaps();
   AWBoolean            renderCubeMap(AWObject* obj, AWCubeEnvTexture* t);
   void                 renderProjectors();
   void                 renderShadows();
   void                 disableBackProjection();
   void                 updateShadowMap(AWLight* l);

	void		            shutdownGL();
   void                 cleaupDisplayLists();
   void                 makeRasterFont();
   void                 outputString(const char* s);
   void                 setShowStatistics(AWBoolean show){m_showStats=show;}
   AWBoolean            getShowStatistics()              {return m_showStats;}

   void                 displayText();

   void                 combinerMode(int mode);

   int			         isExtensionSupported(const char* extension);
   void		            getExtensions();

   AWBoolean            supportsTexEnvCombine()         {return m_hasTexEnvCombine;}
   AWBoolean            supportsNVRegisterCombiners()   {return m_hasNVRegCombine;}
   AWBoolean            supportsDot3Combiners()         {return m_hasDot3Combine;}
   AWBoolean            supportsCubeMap()               {return m_hasCubeEnv;}
   AWBoolean            supportsS3Compression()         {return m_hasS3Compression;}
   AWBoolean            supportsAutoMipmap()            {return m_hasAutoMipmap;}

   void                 setUseCubeEnv(AWBoolean  v)         {m_useCubeEnv=v;}
   AWBoolean            getUseCubeEnv()                     {return m_useCubeEnv;}

   void                 setUseBump(AWBoolean  v)            {m_useBump=v;}
   AWBoolean            getUseBump()                        {return m_useBump;}

   void                 setUseS3Compression(AWBoolean  v)   {m_useS3Compression=v;}
   AWBoolean            getUseS3Compression()               {return m_useS3Compression;}
   
   AWBoolean            getUseBumpSpecular()                {return m_useBumpSpecular;}
   void                 setUseBumpSpecular(AWBoolean  v)    {m_useBumpSpecular=v;}

   void		            setCompileVertArrays(AWBoolean v)   {m_useCVAs=v;}
   AWBoolean	         getCompileVertArrays()			      {return m_useCVAs;}

   void		            setUseVertArrays(AWBoolean v)       {m_useVertArrays=v;}
   AWBoolean	         getUseVertArrays()			         {return m_useVertArrays;}

   void		            setUseDisplayLists(AWBoolean v)     {m_useDisplayLists=v;}
   AWBoolean	         getUseDisplayLists()			         {return m_useDisplayLists;}

   void                 setUseAlphaTest(AWBoolean v=TRUE)   {m_useAlphaTest=v;}
   AWBoolean            getUseAlphaTest()                   {return m_useAlphaTest;}

   void                 setUseRageProHack(AWBoolean v=TRUE) {m_applyRageProHack=v;}
   AWBoolean            getUseRageProHack()                 {return m_applyRageProHack;}
   
   void		            startCapture(const AWCString& fileName, int fmt=IMG_FMT_JPG);
   void		            captureScreen();
	void		            stopCapture();
	AWBoolean	         captureEnabled()			  {return NULL != m_captureBuf;}

	const float          getTime()				      {return m_time;}
   const float          evalTime();

	void		            transformViewport(AWTransformController* node);
	void		            updatePosn(float speed, const AWPoint& look);
	void		            display(float speed, float roll, float yaw, float pitch, 
						              float viewPitch, float viewYaw);
	void		            switchRecordMode();
	void		            switchPlaybackMode();

	void		            backCamera();
	void		            forwardCamera();

	AWBoolean            loadScene(const AWCString& fullPath);
	AWBoolean	         saveScene(const AWCString& fullPath="");
   const AWCString&     getFilename()              {return m_filename;}
   void                 setFilename(const AWCString& n) {m_filename=n;}

   void		            displayEnvironment();
   void		            displayFrame();
   AWBoolean		      compileAllObjects(AWBoolean call = TRUE, AWBoolean force = FALSE);
   int		            compileObject(AWObject* obj);
   void                 compileObjectCurMesh(AWObject* obj, int& listID);

   void		            showRenderStatistics();

	void		            showAllOpaqueObjects();
	void		            showAllTransparentObjects();
   void                 showAllAlphaTestObjects();
	//test for object visibility before drawing - View Frustrum culling
	AWBoolean	         isVisible(AWObject* obj);
	//draw object geometry
   void                 disableTexUnits(int startunit=0);
   
   AWBoolean            usingLighting()         {return m_usingDynLights;}
	void		            applyMaterial(AWMaterial* mat);
   void                 setBlendState(AWRenderPass* pass);
   void                 selectTexture(AWTexture* tex, int unitID);
   void                 applyTextureState(AWTextureUnit* texUnit, int unitID);
   void                 applyObjectState(AWObject* theObj);
   void                 restoreObjectState(AWObject* theObj);

   void		            setTextureState(AWRenderPass* pass);
   int                  getNumPasses(AWObject* o);

   void		            applyTexCoordsImmediate(AWObject* theObj, AWRenderPass* pass, int curVertIndex);
   void		            drawObjectImmediate(AWObject* theObj, AWRenderPass* pass);

	void		            drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass, AWBoolean useCVAEXT=TRUE);

   void		            drawObject(AWObject* theObj);
   void                 drawObjectNormals(AWObject* theObj);
	void		            transformNode(AWNode* node);

	void		            transformLights();
	void		            transformLight(int lightID, AWLight* curLight);

   AWBoolean	         initLights();
   AWBoolean	         initCamera();
   AWBoolean	         selectCamera(AWCamera* cam=NULL);
   void		            setFogParams();

   void                 setFontListBase(int base)   {m_fontListBase=base;}
   int                  getFontListBase()           {return m_fontListBase;}
   void                 cleanupFont();

   int                  getNumRenderedTriangles()   {return m_trisThisFrame;}

   void		            toggleFog();
   void		            useFog(int fogVal);

   AWScene&	            getScene() {return *m_scene;}

	friend class AWExport AWWorker;

	HANDLE		         m_thread;

   AWEffectMgr          m_effectMgr;

	//performance counters
	AWEnginePerformanceCntr m_perfCntr;

   static  int          m_bpp;
   static  AWBoolean    m_useAlphaTest;
   //DEBUG CODE FOR NVIDIA TEST APP
   AWBoolean            m_transformReflections;

protected:
   //static  GLRenderer*	m_staticThis;
//MEMBER DATA
   AWCString            m_filename;         //last file loaded into m_scene
   AWCString            m_driverVendorName;
   AWBoolean	         m_usingDynLights;

   AWBoolean            m_useCVAs;
   AWBoolean	         m_useDisplayLists;
   AWBoolean	         m_useVertArrays;
   AWBoolean            m_useCubeEnv;
   AWBoolean            m_useBump;
   AWBoolean            m_useBumpSpecular;
   AWBoolean            m_useS3Compression;
   AWBoolean            m_applyObjectTextures;

   AWBoolean            m_drawNormals;
   AWBoolean            m_showTextures;

   AWScene*		         m_scene;
   float                m_ambient[4];
   float                m_background[4];

   int                  m_curPolyMode;    //helper for rendering

   AWPBuffer*           m_shadowBuffer;
   AWTexture*           m_projHelper;
   AWCubeEnvTexture*    m_normalizeCube;
   AWCamera*            m_helperCam;
   AWMaterial*          m_selectedMaterial;

   float		            m_aspect;
   int			         m_width, m_height;
   BYTE*		            m_captureBuf;
   AWCString	         m_captureDir;
   long		            m_captureFrame;
   float		            m_time;
   float		            m_pauseTime;
    
   AWBoolean	         m_paused;
   int			         m_imageOutFmt;
   int                  m_trisThisFrame;
   AWBoolean	         m_loop;
   AWBoolean	         m_switchLevel;
   AWBoolean            m_levelFailed;
   AWBoolean            m_timeToExplode;
   AWBoolean            m_showStats;
   AWBoolean            m_applyRageProHack;
   
   AWBoolean            m_hasDisplayLists;
   AWBoolean            m_hasTexEnvCombine;
   AWBoolean            m_hasBlendColor;
   AWBoolean            m_hasDestAlpha;
   AWBoolean            m_hasSeparateSpecular;
   AWBoolean            m_hasVertexPrograms;
   AWBoolean            m_hasTextureShaders;
   AWBoolean            m_hasNVRegCombine;
   AWBoolean            m_hasDot3Combine;
   AWBoolean            m_hasCubeEnv;
   AWBoolean            m_hasS3Compression;
   AWBoolean            m_hasAutoMipmap;

   AWBoolean            m_hasPBuffer;
   AWBoolean            m_hasSGIShadowMaps; 

   float                m_explodeDist;

   GLuint               m_fontListBase; 
   GLuint               m_sceneListBase; 
   GLuint               m_glErr;
   AWMatrix4            m_viewTM;
public:
   GLRenderer() ;
   ~GLRenderer();
};//class AWExport GLRenderer : public AWRenderer




inline void		
GLRenderer::transformNode(AWNode* node)
{
   if (node)// && theObj->getTransform(posn, scale, rotn))
   { //we have a non-null object transformation which must be applied...
      //apply the Wold Space Transform for the object
      glMultMatrixf(node->getMatrix());
   }//if (node && node->getTransform(posn, scale, rotn))
}//void	GLRenderer::transformNode(AWNode* node)


#endif //sentinel __GLRENDERER_H
