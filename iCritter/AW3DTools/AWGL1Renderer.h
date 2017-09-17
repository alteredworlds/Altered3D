#ifndef __AW_GL1RENDERER_H
#define __AW_GL1RENDERER_H

#ifndef _WIN32
#include <OpenGLES/ES2/gl.h>
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
#include "AWGL1RenderState.h"
#include "AWGLRenderer.h"


class AWExport AWGL1Renderer : public AWGLRenderer, public AWGL1RenderState
{
public:
	AWGL1Renderer();
	~AWGL1Renderer();

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
    
    void                    getExtensions();
	
    void		            toggleTransformReflections()        {m_transformReflections=!m_transformReflections;}
	AWBoolean				getTransformReflections()           {return m_transformReflections;}

	AWBoolean				usesCubeMap(AWObject* obj);
	   
	void					setUseCubeEnv(AWBoolean  v)         {m_useCubeEnv=v;}
	AWBoolean				getUseCubeEnv()                     {return m_useCubeEnv;}
	
	void					setUseBump(AWBoolean  v)            {m_useBump=v;}
	AWBoolean				getUseBump()                        {return m_useBump;}
	
	AWBoolean				getUseBumpSpecular()                {return m_useBumpSpecular;}
	void					setUseBumpSpecular(AWBoolean  v)    {m_useBumpSpecular=v;}
	
	void					setUseAlphaTest(AWBoolean v=TRUE)   {m_useAlphaTest=v;}
	AWBoolean				getUseAlphaTest()                   {return m_useAlphaTest;}
	
	void		            transformViewport(AWTransformController* node);
    void		            displayEnvironment();
    
	//draw object geometry
	void					disableTexUnits(int startunit=0);
	
	void					applyMaterial(AWMaterial* mat);
	void					setBlendState(AWRenderPass* pass);
	void					selectTexture(AWTexture* tex, int unitID);
	void					applyTextureState(AWTextureUnit* texUnit, int unitID);
	void					applyObjectState(AWObject* theObj);
	void					restoreObjectState(AWObject* theObj);
	
	void					setTextureState(AWRenderPass* pass);
	
	void					drawObjectAsVertArray(AWObject* theObj, AWRenderPass* pass);
	
	void					transformNode(AWNode* node);
	
    void                    initLights();
	void					transformLights();
	void					transformLight(int lightID, AWLight* curLight);
	
	friend class AWExport	AWWorker;
		
	static  AWBoolean		m_useAlphaTest;
	//DEBUG CODE FOR NVIDIA TEST APP
	AWBoolean				m_transformReflections;
	
protected:
    AWEffectMgr				m_effectMgr;
	AWBoolean				m_useCubeEnv;
	AWBoolean				m_useBump;
	AWBoolean				m_useBumpSpecular;
	AWBoolean				m_applyObjectTextures;
	float					m_ambient[4];
	AWMaterial*				m_selectedMaterial;
	AWCString				m_captureDir;
	long		            m_captureFrame;
	int						m_imageOutFmt;	
	AWBoolean				m_hasBlendColor;
	AWBoolean				m_hasDestAlpha;
	AWBoolean				m_hasVertexPrograms;
	AWBoolean				m_hasTextureShaders;
};//class AWExport AWGL1Renderer : public AWRenderer




inline void		
AWGL1Renderer::transformNode(AWNode* node)
{
	if (node)
	{	//we have a non-null object transformation which must be applied...
		//apply the Wold Space Transform for the object
		glMultMatrixf(node->getMatrix());
	}
}//void	AWGL1Renderer::transformNode(AWNode* node)


#endif //sentinel __AW_GL1RENDERER_H
