#ifndef __AWENVIRON_CONTROLLER_H
#define __AWENVIRON_CONTROLLER_H

#include "AWTransformController.h"


class AWExport AWEnvironController : public AWTransformController
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWEnvironController);

	AWEnvironController();
	virtual			~AWEnvironController();
	virtual int		calcTransform(float t, const AWMatrix4& parentTM);

//AWPersistable virtuals
	virtual void	restoreGuts(AWFile& file);
    virtual void    saveGuts(AWFile& file) const;
	virtual AWspace binaryStoreSize() const;
};//class AWEnvironController : public AWTransformController

#endif //sentinel __AWENVIRON_CONTROLLER_H