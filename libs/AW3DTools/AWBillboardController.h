#ifndef __AW_BILLBOARD_CONTROLLER_H
#define __AW_BILLBOARD_CONTROLLER_H

#include "AWTransformController.h"


class AWExport AWBillboardController : public AWTransformController
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWBillboardController);

   AWBillboardController();
   virtual				~AWBillboardController();

   //calculate the transformation data for a given time
   virtual int			calcTransform(float t, const AWMatrix4& parentTM);

   virtual void		restoreGuts(AWFile& file);
   virtual void      saveGuts(AWFile& file) const;
   virtual AWspace   binaryStoreSize() const;

   AWBoolean			m_pointSymmetry;
};//class AWBillboardController : public AWTransformController


#endif //sentinel __AW_BILLBOARD_CONTROLLER_H