#include "AWRotationKey.h"

#define VER_AWRotationKey_1			100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWRotationKey,	CLSID_AWROTATIONKEY, VER_AWRotationKey_1);

//	AWRotationKey
//

void							
AWRotationKey::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	file >> m_quat;
	file.Read(m_t);
}



void        					
AWRotationKey::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << m_quat;
	file.Write(m_t);
}//void AWRotationKey::saveGuts(AWFile& file) const



AWspace     					
AWRotationKey::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		m_quat.binaryStoreSize() +
		sizeof (m_t);
	return retVal;
}
