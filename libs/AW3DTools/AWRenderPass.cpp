#include "AWRenderPass.h"

#define VER_RENDERPASS_1	100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWRenderPass, CLSID_AWRENDERPASS, VER_RENDERPASS_1);

/*  AWList<AWTextureEffect> m_texEffects;*/

AWRenderPass::AWRenderPass() : m_opacity(100)
{
   m_blendType    = AWTexture::B_NONE;
   m_srcFac       = GL_SRC_ALPHA;
   m_dstFac       = GL_ONE_MINUS_SRC_ALPHA;
   m_depthFunc    = GL_LEQUAL;
   m_combinerMode = AW_CM_NONE;
}//AWRenderPass::AWRenderPass(...)


AWRenderPass::~AWRenderPass()
{
   m_texUnits.destroy();
}//AWRenderPass::~AWRenderPass()


int                     
AWRenderPass::addTexEffect(AWTextureEffect* t, int chanID, AWBoolean grabBlendInfo)
{
   AWTextureUnit temp;
   if (t)
   {
      temp.m_tex    = t->m_tex;
      temp.m_chanID = chanID;
   }  
   return  addTexUnit(temp, grabBlendInfo);
}//int AWRenderPass::addTexEffect(AWTextureEffect* t, int chanID=-1,  AWBoolean grabBlendInfo)


int                  
AWRenderPass::addTexUnit(const AWTextureUnit& t, AWBoolean grabBlendInfo)
{
   if (grabBlendInfo && t.m_tex)
   {
      m_blendType = t.m_tex->m_blendMode;
      if (AWTexture::ALPHA_BLEND == m_blendType)
      {
         m_srcFac    = GL_SRC_ALPHA;
         m_dstFac    = GL_ONE_MINUS_SRC_ALPHA;
         m_depthFunc = GL_LEQUAL;
      }
      else if (AWTexture::ALPHA_TEST == m_blendType)
      {
         ;//we only have one set of options for ALPHA_TEST
      }

   }//if (grabBlendInfo && t.m_tex)
   return m_texUnits.add(t);
}//int  AWRenderPass::addTexUnit(const AWTextureUnit& t), AWBoolean grabBlendInfo



AWRenderPass&
AWRenderPass::operator=(const AWRenderPass& ot)
{
   //copy of this AWRenderPass -
   //
   //AWTextureEffect list is VALUE based with small entries
   //so copying the list implies copying all data
   //the contained tex pointer within AWTextureEffect
   //is copied over directly - (no deep texture copy here)
   for (int i=0; i<ot.getNumTexUnits(); i++)
   {
      addTexUnit(ot.m_texUnits[i]);
   }//if (ot.getNumTexEffects())
   return *this;
}//AWRenderPass::operator=(const AWRenderPass& other)



int
AWRenderPass::operator==(const AWRenderPass& o)const
{
   return isEqual((AWPersistable*)&o);
}//int AWRenderPass::operator==(const AWRenderPass& o)




//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWRenderPass
//
//////////////////////////////////////////////////////////
void
AWRenderPass::restoreGuts(AWFile& file)
{
   unsigned int   version;
	restoreVersion(file, &version);   // Get the saved version #
	if ( VER_RENDERPASS_1 >= version )
	{
		AWPersistable::restoreGuts(file);
      m_texUnits.read(file);
      file.Read(m_opacity);
	}//if ( VER_RENDERPASS_1 >= version )  
}//void	AWRenderPass::restoreGuts(AWFile& file)



void        
AWRenderPass::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
   m_texUnits.write(file);
   file.Write(m_opacity);
}//void AWRenderPass::saveGuts(AWFile& file) const



AWspace     
AWRenderPass::binaryStoreSize() const
{
   AWspace retVal = sizeof uVersion_ +
                     AWPersistable::binaryStoreSize() +
                     m_texUnits.getStoreSize() +
                     sizeof(m_opacity);
   return retVal;
}//AWspace AWRenderPass::binaryStoreSize() const