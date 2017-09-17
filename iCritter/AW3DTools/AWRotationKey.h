#ifndef __AWROTATIONKEY_H
#define __AWROTATIONKEY_H

#include "AWToolDefs.h"
#include "AWQuat.h"



class AWExport AWRotationKey : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWRotationKey);

	AWRotationKey() : m_quat(0.0f, 0.0f, 0.0f, 0.0f), m_t(0.0f){;}
	AWRotationKey(float X, float Y, float Z, float W, float T)
		: m_quat(W, X, Y, Z), m_t(T) {;}
	AWRotationKey(const AWQuat& quat, float t)
		: m_quat(quat), m_t(t) {;}
	virtual ~AWRotationKey(){;}

	//AWPersistable virtuals
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

	//data
	AWQuat	m_quat;
	float	m_t;
};//class AWRotationKey : public AWPersistable

#endif //sentinel __AWROTATIONKEY_H
