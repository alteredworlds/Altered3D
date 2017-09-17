#include "AWFog.h"


#define VER_AWFOG_1 100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWFog, CLSID_AWFOG, VER_AWFOG_1);



AWFog::AWFog(const AWCString& name) : 
				 m_type(0), 
				 m_colour(1.0f, 1.0f, 1.0f),
				 m_name(name),
				 m_startDist(0.0f),
				 m_endDist(1000.0f)
{
}




AWFog::~AWFog()
{
}


/*int 
AWFog::operator==(const AWFog& o)const
{	//same as default
	return *this == o;
}*/


AWFog& 
AWFog::operator=(const AWFog& o)
{
	m_name		=o.m_name;
	m_type		=o.m_type;
	m_colour	=o.m_colour;
	m_startDist	=o.m_startDist;
	m_endDist	=o.m_endDist;
	return *this;
}


void
AWFog::scale(const AWPoint& scale)
{
	m_startDist *= scale.z;
	m_endDist   *= scale.z;
}//void AWFog::scale(const AWPoint& scl)

//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWFog
//
//////////////////////////////////////////////////////////
void		
AWFog::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWFOG_1 == version)
	{
		AWPersistable::restoreGuts(file);
		file >> m_name;
		file.Read(m_type);
		m_colour.restoreGuts(file);
		file.Read(m_startDist);
		file.Read(m_endDist);
	}
}//void	AWFog::restoreGuts(AWFile& file)



void        
AWFog::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << m_name;
	file.Write(m_type);
	m_colour.saveGuts(file);
	file.Write(m_startDist);
	file.Write(m_endDist);
}



AWspace     
AWFog::binaryStoreSize() const
{
	AWspace count = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		m_name.binaryStoreSize() +
		sizeof(m_type) + sizeof(m_startDist) + sizeof(m_colour) + sizeof(m_endDist); 
	return count;
}

