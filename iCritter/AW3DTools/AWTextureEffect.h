#ifndef _AWTEXTUREEFFECT_H
#define _AWTEXTUREEFFECT_H

#include "AWToolsCommon.h"
#include "AWQuat.h"
#include "AWPoint.h"
#include "AWMatrix4.h"
#include "AWTexture.h"


class AWExport AWTextureEffect : public AWPersistable
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWTextureEffect);

   enum EffectType   {AW_DIFFUSE, AW_REFLECT, AW_BUMP, AW_NORMALIZE, AW_SPECULAR, AW_BUMP_AND_SPECULAR};

   AWTextureEffect(AWTexture* other=NULL,  int chanID=-1, 
                   int type=AW_DIFFUSE, float opacity=1.0f);
   AWTextureEffect(const AWTextureEffect& p) {*this=p;}

   virtual ~AWTextureEffect();

   AWTextureEffect&     operator=(const AWTextureEffect& ot);
   int                  operator==(const AWTextureEffect& o)const;
   int                  operator!=(const AWTextureEffect& o)const 
                              {return !operator==(o);}

	virtual void         restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;

	virtual AWspace      binaryStoreSize() const;

   int                  m_type;
   AWTexture*           m_tex;
   int                  m_chanID;
   float                m_opacity;
   AWMatrix4            m_uvTransform;
};//class AWTextureEffect



#endif //sentinel _AWTEXTUREEFFECT_H