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
AWEffectMgr::buildRadeonEffects(GLRenderer& renderer, AWObject* obj, AWRenderPass*& pass)
{  //THREE TEXTURE UNIT APPROACH - bump without specular
   if (!obj || obj->getNumPasses()) return;
   AWTRACE("\nBuilding effects for ATI card...\n");
   AWTextureEffect* tex  = NULL;
   AWMaterial*      mat  = obj->getMaterial();
  //more than one texture - can be one of the following 
   //combinarions: 
   //   [DIFFUSE + BUMP]     or  [DIFFUSE + ENVIRONMENT]
   //or [ENVIRONMENT + BUMP] or  [DIFFUSE + BUMP + ENVIRONMENT]
   //
   AWTextureEffect* diff = NULL;
   AWTextureEffect* env  = NULL;
   AWTextureEffect* bmp  = NULL;
   AWTextureEffect* spec = NULL;
   AWTextureEffect* nrm = NULL;
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
      case AWTextureEffect::AW_BUMP_AND_SPECULAR:
         spec = tex;
      case AWTextureEffect::AW_BUMP:           
         bmp  = tex;
         break; 
      }//switch (tex->m_type)   
   }//for (int i=0; i< obj->getNumTextureEffects(); i++)
   //so now build the relevant set of passes
   //CASE 1: bump map is present & hardware to use it also present
   
   if (bmp && renderer.supportsDot3Combiners() &&
      renderer.getUseBump() && renderer.supportsCubeMap() &&
      renderer.getUseCubeEnv())
   { 
      AWTexture* normalizeTexCube = renderer.generateNormalizeCube(64);  
      //TEX UNIT 0 - NORMALIZATION CUBE MAP - texCoord channel 1
      AWTextureUnit tUnit(normalizeTexCube, 1); //LIGHT vectors in mesh channelID 1
      tUnit.m_texFunc = AWTextureUnit::REPLACE;
      pass->addTexUnit(tUnit);
         
      //TEX UNIT 1 - NORMAL MAP - texCoord channel 0
      //set correct tex env combiners mode 
      tUnit.m_tex    = bmp->m_tex;
      tUnit.m_chanID = 0; //DIFFUSE TEX COORD CHANNEL
      tUnit.m_texFunc = AWTextureUnit::COMBINE;
      tUnit.m_combineMode  = GL_DOT3_RGB_EXT;
      tUnit.m_srce0Name    = GL_PREVIOUS_EXT;
      tUnit.m_srce0Operand = GL_SRC_COLOR;
      tUnit.m_srce1Name    = GL_TEXTURE;
      tUnit.m_srce1Operand = GL_SRC_COLOR;
      pass->addTexUnit(tUnit);
      
      //READEON HAS 3 TEXTURE UNITS - add DIFFUSE texture 
      //TEX UNIT 2 - DIFFUSE MAP - texCoord channel 0 
      if (diff && (renderer.getNumTexUnits()>2))
      {
         tUnit.resetCombinerData();
         tUnit.m_tex    = diff->m_tex;
         tUnit.m_chanID = 0; //DIFFUSE TEX COORD CHANNEL
         tUnit.m_texFunc = AWTextureUnit::MODULATE;
         pass->addTexUnit(tUnit);
      }//if (diff)

      if (spec && renderer.getUseBumpSpecular())
      {
         obj->addPass(pass);
         //SPECULAR PASS - add new pass, set blend mode         
         pass = new AWRenderPass;
         pass->setDepthFunc(GL_EQUAL);
         pass->setBlendType(AWTexture::ALPHA_BLEND);
         pass->setSrcFac(GL_DST_COLOR);
         pass->setDstFac(GL_ONE);
         //add the required textures for this effect.
         //TEX UNIT 0 - NORMALIZATION CUBE MAP - texCoord channel 2
         tUnit.resetCombinerData();
         tUnit.m_tex    = normalizeTexCube; 
         tUnit.m_chanID = 2;  //HALF ANGLE vectors in channelID 2    
         tUnit.m_texFunc = AWTextureUnit::REPLACE;
         pass->addTexUnit(tUnit);
         //TEX UNIT 1 - NORMAL MAP - texCoord channel 0 
         tUnit.m_tex    = bmp->m_tex;
         tUnit.m_chanID = 0; //DIFFUSE TEX COORD CHANNEL
         //set correct tex env combiners mode
         tUnit.m_texFunc = AWTextureUnit::COMBINE;
         tUnit.m_combineMode  = GL_DOT3_RGB_EXT;
         tUnit.m_srce0Name    = GL_PREVIOUS_EXT;
         tUnit.m_srce0Operand = GL_SRC_COLOR;
         tUnit.m_srce1Name    = GL_TEXTURE;
         tUnit.m_srce1Operand = GL_SRC_COLOR;
         pass->addTexUnit(tUnit);
      }//if (spec && renderer.getUseBumpSpecular())
   }//if (bmp etc...) 
   else if (diff||env)
   {  //DIFFUSE + ??
      //diffuse is always the first texture to be used - get the
      //overall initial blend state for the pass from this texture
      AWTRACE("\nBump mapping not present or not supported on this card\n");
      if (diff)
      {
         pass->addTexEffect(diff, 0, TRUE);
      }
      //turn OFF dynamic lighting
      if (env)
      {  //DIFFUSE + ENVIRONMENT + ??
         //we can add the environment to the first pass
         pass->addTexEffect(env);
      }//if (env)                 
   }//if (diff)
}//void AWEffectMgr::buildRadeonEffects(GLRenderer& renderer, AWObject* obj)



void 
AWEffectMgr::buildNVIDIAEffects(GLRenderer& renderer, AWObject* obj, AWRenderPass*& pass)
{
   if (!obj || obj->getNumPasses()) return;
   AWTRACE("\nBuilding effects for NVIDIA or other card...\n");
   AWTextureEffect* tex  = NULL;  
   AWMaterial*      mat  = obj->getMaterial();
   //more than one texture - can be one of the following 
   //combinarions: 
   //   [DIFFUSE + BUMP]     or  [DIFFUSE + ENVIRONMENT]
   //or [ENVIRONMENT + BUMP] or  [DIFFUSE + BUMP + ENVIRONMENT]
   //
   AWTextureEffect* diff = NULL;
   AWTextureEffect* env  = NULL;
   AWTextureEffect* bmp  = NULL;
   AWTextureEffect* nrm  = NULL;
   AWTextureEffect* spec = NULL;
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
      case AWTextureEffect::AW_BUMP_AND_SPECULAR:
         spec = tex;
      case AWTextureEffect::AW_BUMP:           
         bmp  = tex;
         break; 
      case AWTextureEffect::AW_NORMALIZE:
         nrm  = tex;
         break;
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
         
      obj->addPass(pass); 
      pass = new AWRenderPass;
      pass->setDepthFunc(GL_EQUAL);
      if (bmp && renderer.supportsNVRegisterCombiners() &&
         renderer.getUseBump() && renderer.supportsCubeMap() &&
         renderer.getUseCubeEnv())
      {  //setup blend mode for BLEND PASS
         pass->setBlendType(AWTexture::ALPHA_BLEND);
         pass->setSrcFac(GL_DST_COLOR);
         pass->setDstFac(GL_ZERO);
         //turn OFF dynamic lighting
         obj->setDynamicLighting(FALSE);

         //TEX UNIT 0 - BUMP MAP - texCoord channel 0
         AWTextureUnit tUnit(bmp->m_tex, 0);
         tUnit.m_texFunc = AWTextureUnit::MODULATE;
         pass->addTexUnit(tUnit);

         //TEX UNIT1 - NORMALIZATION CUBE MAP - texCoord channel 1
         //get NORMALIZATION CUBE MAP from Renderer
         //  generated and initialized if first time getting this...
         AWTexture* normalizeTexCube = renderer.generateNormalizeCube(64);         
         tUnit.resetCombinerData();
         tUnit.m_tex    = normalizeTexCube; 
         tUnit.m_chanID = 1;  //LIGHT vectors in channelID 1    
         tUnit.m_texFunc = AWTextureUnit::MODULATE;
         pass->addTexUnit(tUnit);        
         //set correct register combiners mode
         pass->setCombinerMode(AWRenderPass::AW_CM_BUMPED);
         
         if (spec && renderer.getUseBumpSpecular())
         {
            obj->addPass(pass);  //add pass as prepared thus far 
            //SPECULAR PASS - configure blend options for the pass          
            pass = new AWRenderPass;   //starting to configure a new pass
            pass->setDepthFunc(GL_EQUAL);
            pass->setBlendType(AWTexture::ALPHA_BLEND);
            pass->setSrcFac(GL_DST_COLOR);      //pass->setSrcFac(GL_DST_ALPHA);           
            pass->setDstFac(GL_ONE); 
            //now setup individual texture units
            //UNIT 0 - Normal Map
            tUnit.resetCombinerData();
            tUnit.m_tex    = spec->m_tex; 
            tUnit.m_chanID = 0; //diffuse tex coord channel positions the normal map  
            tUnit.m_texFunc = AWTextureUnit::MODULATE;
            pass->addTexUnit(tUnit);        
            //UNIT 1 - Normalization cube map, using HALF-ANGLE vectors
            tUnit.resetCombinerData();
            tUnit.m_tex    = normalizeTexCube; 
            tUnit.m_chanID = 2; //diffuse tex coord channel positions the normal map  
            tUnit.m_texFunc = AWTextureUnit::MODULATE;
            pass->addTexUnit(tUnit);
            //set correct register combiners mode
            pass->setCombinerMode(AWRenderPass::AW_CM_BUMPED_SPECULAR);
         }//if (spec && renderer.getUseBumpSpecular())        
      }//if (bmp) 
      //}
   }//if (diff)
   else if (env && bmp)
   {  //ENVIRONMENT + BUMP
   }
}//void AWEffectMgr::buildNVIDIAEffects(GLRenderer& renderer, AWObject* obj, AWRenderPass*& pass)



int 
AWEffectMgr::prepareForRendering(GLRenderer& renderer, AWObject* obj)
{
   if (!obj || obj->getNumPasses()) return 0;
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
   AWTextureEffect* tex  = NULL;
   AWRenderPass*    pass = NULL;
   AWMaterial*      mat  = obj->getMaterial();

   pass = new AWRenderPass; 
   if (renderer.getVendorName().contains("ATI Technologies"))
   {
      buildRadeonEffects(renderer, obj, pass);   
   }
   else
   {
      buildNVIDIAEffects(renderer, obj, pass);        
   }
   if (pass && ( pass->getNumTexUnits() || (mat&&(0==mat->getNumTexEffects())) ) )
   {
      obj->addPass(pass);
   }
   else
   {
      delete pass;
   }
   //clears the list without deleting objects it contains
   //obj->clearTextureEffects();
   obj->setPass(0);
   return obj->getNumPasses();
}//int AWEffectMgr::prepareForRendering(GLRenderer& renderer, AWObject* obj)

