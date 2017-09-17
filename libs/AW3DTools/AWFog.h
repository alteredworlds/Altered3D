#ifndef __AWFOG_H
#define __AWFOG_H

#include "AWToolsCommon.h"
#include "AWPoint.h"
#include "AWCString.h"


class AWExport AWFog : public AWPersistable  
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWFog);

	AWFog(const AWCString& name="");
	virtual			~AWFog();

	//int				operator==(const AWFog& o)const;
	AWFog&			operator=(const AWFog& o);

	virtual void	scale(const AWPoint& scale);

//AWPersistable virtuals
	virtual void	restoreGuts(AWFile& file);
    virtual void    saveGuts(AWFile& file) const;

	virtual AWspace binaryStoreSize() const;

//data
	AWCString		m_name;
	int				m_type;
	AWPoint			m_colour;
	float			m_startDist;
	float			m_endDist;
};//class AWFog : public AWPersistable 

#endif //sentinel __AWFOG_H
