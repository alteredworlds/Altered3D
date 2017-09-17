#ifndef _AWRENDERPASS_H
#define _AWRENDERPASS_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWTextureEffect.h"
#include "AWTextureUnit.h"
#include "AWList.h"


class AWExport AWRenderPass : public AWPersistable
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWRenderPass);

   enum RegCombinerMode {AW_CM_NONE, AW_CM_BUMPED, AW_CM_BUMPED_SPECULAR};

   AWRenderPass();
   AWRenderPass(const AWRenderPass& p) {*this=p;}

   virtual ~AWRenderPass();

   virtual int             getNumTexUnits()const    {return m_texUnits.getNum();}
   AWTextureUnit*          getTexUnit(int i)        {return getNumTexUnits()>i ? &m_texUnits[i]:NULL;}
   int                     addTexUnit(const AWTextureUnit& t, AWBoolean grabBlendInfo=FALSE);
   int                     addTexEffect(AWTextureEffect* t, int chanID=-1, AWBoolean grabBlendInfo=FALSE);

   void                    setOpacity(float opacity) {m_opacity = opacity;}
   float                   getOpacity()              {return m_opacity;}

   void                    setBlendType(int v)        {m_blendType=v;}
   void                    setSrcFac(int v)           {m_srcFac=v;}
   void                    setDstFac(int v)           {m_dstFac=v;}
   void                    setDepthFunc(int v)        {m_depthFunc=v;}

   int                     getBlendType()             {return m_blendType;}
   int                     getSrcFac()                {return m_srcFac;}
   int                     getDstFac()                {return m_dstFac;}
   int                     getDepthFunc()             {return m_depthFunc;}   

   AWRenderPass&           operator=(const AWRenderPass& ot);
   int                     operator==(const AWRenderPass& o)const;
   int                     operator!=(const AWRenderPass& o)const 
                              {return !operator==(o);}

	virtual void            restoreGuts(AWFile& file);
   virtual void            saveGuts(AWFile& file) const;

	virtual AWspace         binaryStoreSize() const;

   AWList<AWTextureUnit>   m_texUnits;
   float                   m_opacity;
   int                     m_blendType;
   int                     m_srcFac;
   int                     m_dstFac;
   int                     m_depthFunc;
};//class AWRenderPass


#endif //sentinel _AWRENDERPASS_H