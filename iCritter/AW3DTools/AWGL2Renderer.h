#ifndef __AW_GL2RENDERER_H
#define __AW_GL2RENDERER_H

#ifndef _WIN32
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
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
#include "AWGLRenderer.h"
#include "AWGL2RenderState.h"
#include "AWVertexProgram.h"
#include "AWMatrix3.h"

#define GLI_FOG_MODE_LINEAR 0
#define GLI_FOG_MODE_EXP 1
#define GLI_FOG_MODE_EXP2 2


class AWExport AWGL2Renderer : public AWGLRenderer, public AWGL2RenderState
{
public:
	AWGL2Renderer();
	~AWGL2Renderer();

public:
	virtual void			resetData();
    virtual void		    initializeGL();	
    virtual void			shutdownGL();	
    
    virtual void			drawObject(AWObject* theObj);
    virtual void		    displayFrame();
    virtual int				getNumPasses(AWObject* o);
    
    virtual AWBoolean		selectCamera(AWCamera* cam=NULL);
	virtual void			setFogParams();
    
    virtual int				getInternalFormat(int texFormat, AWBoolean allowCompression=TRUE);
	virtual AWBoolean		initTextures();
	virtual void			initTexture(AWTexture* tex);
	virtual void			init2DTexture(AWTexture* tex);
	virtual void            initCubeMapTexture(AWCubeEnvTexture* f, AWBoolean allowCompression=TRUE);
	virtual AWBoolean		setupTexture(AWTexture* t, AWBoolean allowAutoMipmap=TRUE);
    
    virtual void            addVertexProgram(const AWCString& vertexSrc, const AWCString& fragSrc);
    virtual void            addVertexProgram(AWVertexProgram* vProg);
	
    void		            transformViewport(AWTransformController* node);
    void		            displayEnvironment();
    
	void					disableTexUnits(int startunit=0);
    
    void                    enableLighting(AWBoolean on);
    void                    applyMaterialDefaults();
	void					applyMaterial(AWMaterial* mat);
	void					setBlendState(AWRenderPass* pass);
	void					selectTexture(AWTexture* tex, int unitID);
	void					applyTextureState(AWTextureUnit* texUnit, int unitID);
	void					applyObjectState(AWObject* theObj);
	void					restoreObjectState(AWObject* theObj);
    AWBoolean               usesCubeMap(AWObject* obj);
	
	void					setTextureState(AWRenderPass* pass);
	
	void					drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass);
    void					drawObjectAsVBO(AWObject* theObj, AWRenderPass* pass);
    void					bufferObjectAsVBO(AWObject* theObj, AWRenderPass* pass);
	
	void					transformObject(AWNode* node);
	
    void                    initLights();
	void					transformLights();
	void					transformLight(AWLight* curLight);
	
	friend class AWExport	AWWorker;
		
	static  AWBoolean		m_useAlphaTest;
	
	AWBoolean				m_transformReflections;
	
protected:
    AWEffectMgr				m_effectMgr;
	AWBoolean				m_applyObjectTextures;
	float					m_ambient[4];
	AWMaterial*				m_selectedMaterial;
	AWCString				m_captureDir;
	long		            m_captureFrame;
	int						m_imageOutFmt;	
    AWPtrList<AWVertexProgram*> m_vertexPrograms;
    
    AWMatrix4               m_viewTM;
    AWMatrix4               m_projectionTM;
};//class AWExport AWGL2Renderer : public AWRenderer



#endif //sentinel __AW_GL2RENDERER_H
