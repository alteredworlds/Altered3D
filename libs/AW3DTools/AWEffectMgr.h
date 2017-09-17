#ifndef __AW_EFFECTMGR_H
#define __AW_EFFECTMGR_H

#include "AWToolsCommon.h"
#include "AWObject.h"

//forward definition
class AWExport GLRenderer;

class AWExport AWEffectMgr
{
public:
   //enum EffectType   {AW_DIFFUSE, AW_REFLECT, AW_BUMP, AW_SPECULAR};

   AWEffectMgr() {reset();}

   virtual ~AWEffectMgr();

   int      prepareForRendering(GLRenderer& renderer, AWObject* obj);
   void     buildRadeonEffects(GLRenderer& renderer, AWObject* obj, AWRenderPass*& pass);

   void     buildNVIDIAEffects(GLRenderer& renderer, AWObject* obj, AWRenderPass*& pass);
   void     reset();
};//class AWEffectMgr



#endif //sentinel __AW_EFFECTMGR_H