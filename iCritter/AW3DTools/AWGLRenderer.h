#ifndef __AW_GLRENDERER_H
#define __AW_GLRENDERER_H


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


class AWExport AWQuat;      //forward
class AWExport AWMaterial;  //forward
class AWExport AWVertexProgram;


class AWExport AWGLRenderer
{
public:
	AWGLRenderer();
	virtual ~AWGLRenderer();

public:
    int						isExtensionSupported(const char* extension);	
    const AWCString&		getVendorName()  {return m_driverVendorName;}
	
    virtual AWBoolean		initCamera();
    virtual AWBoolean		selectCamera(AWCamera* cam=NULL)=0;
	virtual void		    initialize(int width, int height);	
    virtual void		    resizeGL(int width, int height);
    
    virtual void		    initializeGL()=0;
    virtual void			shutdownGL()=0;

	virtual void		    initializeObjects();
    virtual void		    shutdown();		
    virtual void			resetData();
    virtual void		    display(float viewZoom);
    virtual void		    displayFrame()=0;
    virtual int				getNumPasses(AWObject* o);
	virtual void			drawObject(AWObject* theObj)=0;
    virtual void			useFog(int fogVal);
    virtual void			setFogParams();
    
    AWScene&				getScene() {return *m_scene;}
    void					setScene(AWScene& s) {m_scene=&s;}
	
	void		            togglePause();
	AWBoolean				getPaused() {return m_paused;}
    void					pause();
    
    AWBoolean				usingFog()                          {return m_usingFog;}
    void                    setUsingFog(AWBoolean value)        {m_usingFog = value;}
    AWBoolean				userAllowsFog()                     {return m_userAllowsFog;}
    void                    setUserAllowsFog(AWBoolean value)   {m_userAllowsFog = value;}
    
    AWBoolean				usingLighting()                     {return m_usingLights;}
    void                    setUsingLighting(AWBoolean value)   {m_usingLights = value;}
    
    AWBoolean				userAllowsLighting()                     {return m_userAllowsLighting;}
    void                    setUserAllowsLighting(AWBoolean value)   {m_userAllowsLighting = value;}
    
    void                    setUserAllowsVertexColours(AWBoolean value)   {m_userAllowsVertexColours = value;}
    AWBoolean				userAllowsVertexColours() {return m_userAllowsVertexColours;}
    
    AWBoolean				captureEnabled() {return NULL != m_captureBuf;}
	
	virtual int				getInternalFormat(int texFormat, AWBoolean allowCompression=TRUE)=0;
	virtual AWBoolean		initTextures()=0;
	virtual void			initTexture(AWTexture* tex)=0;
	virtual void			init2DTexture(AWTexture* tex)=0;
	virtual void            initCubeMapTexture(AWCubeEnvTexture* f, AWBoolean allowCompression=TRUE)=0;
	virtual AWBoolean		setupTexture(AWTexture* t, AWBoolean allowAutoMipmap=TRUE)=0;	
    
    virtual void            addVertexProgram(const AWCString& vertexSrc, const AWCString& fragSrc){;}
    virtual void            addVertexProgram(AWVertexProgram* vProg){;}
	
	const double            getTime()  {return m_time;}
	const double            evalTime();
    
	void		            backCamera();
	void		            forwardCamera();
	
	const AWCString&		getFilename() {return m_filename;}
	void					setFilename(const AWCString& n) {m_filename=n;}
    
	void		            showAllOpaqueObjects();
	void		            showAllTransparentObjects();
	void					showAllAlphaTestObjects();
	int						getNumRenderedTriangles()   {return m_trisThisFrame;}	
    
	void					toggleFog(); 
    
    void					startCapture(const AWCString& fileName, int fmt=IMG_FMT_JPG);
    void                    stopCapture();
    void                    captureScreen();
	
protected:
    static void*            m_libHandle;
    
    AWCString				m_filename; // last scene
    AWCString				m_driverVendorName;
	AWScene*		        m_scene;
    float					m_background[4];
	float		            m_aspect;
	int						m_width, m_height;
    double		            m_time;
	double		            m_pauseTime;
    AWBoolean				m_paused;
    AWBoolean				m_loop;
	AWEnginePerformanceCntr m_perfCntr;
    unsigned int			m_glErr;
	BYTE*		            m_captureBuf;
    int						m_trisThisFrame;    
    AWMatrix4				m_viewTM;
    AWBoolean               m_usingFog;
    AWBoolean				m_usingLights;
    AWBoolean               m_userAllowsLighting;
    AWBoolean               m_userAllowsFog;
    AWBoolean               m_userAllowsVertexColours;
};//class AWExport AWGLRenderer


#endif //sentinel __AW_GLRENDERER_H
