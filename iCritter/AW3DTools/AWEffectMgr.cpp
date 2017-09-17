#include "AWEffectMgr.h"
#include "AWGLRenderer.h"
#include "AWMaterial.h"


AWEffectMgr::~AWEffectMgr()
{
   reset();
}


void 
AWEffectMgr::reset()
{
      
}//void AWEffectMgr::reset()


void 
AWEffectMgr::buildEffects(AWGLRenderer& renderer, AWObject* obj, AWRenderPass*& pass)
{
   if (!obj || obj->getNumPasses()) 
		return;
   AWTextureEffect* tex  = NULL;  
   AWMaterial*      mat  = obj->getMaterial();
   //more than one texture - can be one of the following 
   //combinarions: 
   //   [DIFFUSE + BUMP]     or  [DIFFUSE + ENVIRONMENT]
   //or [ENVIRONMENT + BUMP] or  [DIFFUSE + BUMP + ENVIRONMENT]
   //
   AWTextureEffect* diff = NULL;
   AWTextureEffect* env  = NULL;
   //AWTextureEffect* bmp  = NULL;
   //AWTextureEffect* nrm  = NULL;
   //AWTextureEffect* spec = NULL;
   for (int i=0; i< mat->getNumTexEffects(); i++)
   {
      tex = mat->getTextureEffect(i);
      switch (tex->m_type)
      {
			default:
			case AWTextureEffect::AW_DIFFUSE:
				diff = tex;
				break;
			case AWTextureEffect::AW_REFLECT:
				env  = tex;
				break;
			//case AWTextureEffect::AW_BUMP_AND_SPECULAR:
			//	spec = tex;
			//case AWTextureEffect::AW_BUMP:           
			//	bmp  = tex;
			//	break; 
			//case AWTextureEffect::AW_NORMALIZE:
			//	nrm  = tex;
			//	break;
      }//switch (tex->m_type)   
   }//for (int i=0; i< obj->getNumTextureEffects(); i++)
   //so now build the relevant set of passes
   if (diff||env)
   {  //DIFFUSE + ??
      //diffuse is always the first texture to be used - get the
      //overall initial blend state for the pass from this texture
      if (diff) 
      {
         pass->addTexEffect(diff, 0, TRUE);
      }
      if (env)
      {  //DIFFUSE + ENVIRONMENT + ??
         //we can add the environment to the first pass
         pass->addTexEffect(env);
      }//if (env)    
   }//if (diff)
}//void AWEffectMgr::buildEffects(GLRenderer& renderer, AWObject* obj, AWRenderPass*& pass)


int 
AWEffectMgr::prepareForRendering(AWGLRenderer& renderer, AWObject* obj)
{
	int retVal = 0;
   if (!obj || obj->getNumPasses()) 
		return retVal;
	AWTRACE("Prepare for rending");
   //object may / will be supplied as follows:
   //m_texture list contains series of AWTextureEffect objects
   //m_passes list contains NOTHING
   //
   //this function therefore goes through the list of
   //AWTextureEffects and builds a suitable set of
   //AWRenderPass data structures for the detected video card
   //or OpenGL features
   //
   //REQUIRED INFORMATION:
   //
   //NUM_TEXUNITS
   //GL_EXT_texture_env_combine or equivalent?
   //m_hasCubeMap  (will be needed when catering for 'other-card' situations
   //               where exact effect details vary)
   //m_numTexUnits
   //m_hasTexEnvCombine 
   //enum EffectType   {AW_DIFFUSE, AW_REFLECT, AW_BUMP, AW_SPECULAR};
   AWRenderPass*    pass = new AWRenderPass; 
   AWMaterial*      mat  = obj->getMaterial();
	buildEffects(renderer, obj, pass);
   if (pass && ( pass->getNumTexUnits() || (mat&&(0==mat->getNumTexEffects())) ) )
   {
      obj->addPass(pass);
   }
   else
   {
      delete pass;
   }
	retVal = obj->getNumPasses();
   if (retVal > 0)
	{
		obj->setPass(0);
	}
	AWTRACE2(" with %d pass%s\n", retVal, (retVal > 1) ? "es" : "");
   return retVal;
}//int AWEffectMgr::prepareForRendering(GLRenderer& renderer, AWObject* obj)

