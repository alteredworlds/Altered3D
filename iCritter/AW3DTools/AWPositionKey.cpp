#include "AWPositionKey.h"

#define VER_AWPositionKey_1			100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWPositionKey,	CLSID_AWPOSITIONKEY, VER_AWPositionKey_1);

//
//	AWPositionKey
//


//PERSISTENCE AWPersistable virtuals
void							
AWPositionKey::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	AWPersistable::restoreGuts(file);
	m_pos.restoreGuts(file);
	file.Read(m_t);
}//void	AWPositionKey::restoreGuts(AWFile& file)



void        					
AWPositionKey::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	m_pos.saveGuts(file);
	file.Write(m_t);
}//void AWPositionKey::saveGuts(AWFile& file) const




AWspace     					
AWPositionKey::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		sizeof (m_pos) +
		sizeof (m_t);
	return retVal;
}//AWspace AWPositionKey::binaryStoreSize() const