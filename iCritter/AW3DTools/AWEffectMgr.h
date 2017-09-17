#ifndef __AW_EFFECTMGR_H
#define __AW_EFFECTMGR_H

#include "AWToolsCommon.h"
#include "AWObject.h"

//forward definition
class AWExport AWGL1Renderer;

class AWExport AWEffectMgr
{
public:
   //enum EffectType   {AW_DIFFUSE, AW_REFLECT, AW_BUMP, AW_SPECULAR};

   AWEffectMgr() {reset();}

   virtual ~AWEffectMgr();

   int      prepareForRendering(AWGLRenderer& renderer, AWObject* obj);
	
	void		buildEffects(AWGLRenderer& renderer, AWObject* obj, AWRenderPass*& pass);


   void     reset();
};//class AWEffectMgr



#endif //sentinel __AW_EFFECTMGR_H