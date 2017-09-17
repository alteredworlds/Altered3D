#ifndef __AW_SEA_H
#define __AW_SEA_H


#include "AWToolsCommon.h"
#include "AWObject.h"

class AWExport GLRenderer; //forward definition


class AWExport AWSeaObject : public AWObject
{	
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWSeaObject);

   AWSeaObject(const AWCString& name = "", AWNode* parent=NULL);
	AWSeaObject(const AWSeaObject& other);
	virtual				~AWSeaObject();

	AWObject&			operator=(const AWSeaObject& other);
	int					operator==(const AWSeaObject&);

   virtual void      init(float t, GLRenderer& renderer);

	virtual int			calcViewDependentData(float t, const AWSolidCone& viewCone);

//AWPersistable virtuals
	virtual void		restoreGuts(AWFile& file);
    virtual void		saveGuts(AWFile& file) const;

	virtual AWspace		binaryStoreSize() const;

	//0 for OFF, anything else for ON (at the moment)
	int					getFog(float t, const AWSolidCone& viewCone);
	int					m_curFog;
	int					m_posFog, m_negFog;
	AWCString			m_posFogName, m_negFogName;
   GLRenderer*       m_renderer;
};//class AWSeaObject : public AWObject



inline void      
AWSeaObject::init(float t, GLRenderer& renderer)
{
   m_renderer = &renderer;
   AWObject::init(t, renderer);
}//inline void AWSeaObject::init(float t, GLRenderer& renderer)


#endif //sentinel __AW_SEA_H