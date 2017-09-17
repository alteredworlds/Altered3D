#include "AWSea.h"
#include "AWGLRenderer.h"

#define VER_AWSEAOBJECT_1 100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWSeaObject, CLSID_AWSEAOBJECT, VER_AWSEAOBJECT_1);



AWSeaObject::AWSeaObject(const AWCString& name, AWNode* parent)
: AWObject(name, parent), m_posFog(-1), m_negFog(-1), m_renderer(NULL)
{
}


AWSeaObject::AWSeaObject(const AWSeaObject& other)
{
	*this = other;
}


AWSeaObject::~AWSeaObject()
{
}

AWObject&			
AWSeaObject::operator=(const AWSeaObject& other)
{
	(AWObject&)*this = (const AWObject&)other;
	m_curFog = other.m_curFog;
	m_posFog = other.m_posFog;
	m_negFog = other.m_negFog;
   m_posFogName = other.m_posFogName;
   m_negFogName = other.m_negFogName;
   m_renderer   = other.m_renderer;
	return *this;
}


int					
AWSeaObject::operator==(const AWSeaObject& other)
{
	return this == &other; //same as default
}


//AWPersistable virtuals
void		
AWSeaObject::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWSEAOBJECT_1 == version)
	{
		AWObject::restoreGuts(file);
		file.Read(m_curFog);
		file.Read(m_posFog);
		file.Read(m_negFog);
	}
}//void	AWSeaObject::restoreGuts(AWFile& file)



void		
AWSeaObject::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWObject::saveGuts(file);
	file.Write(m_curFog);
	file.Write(m_posFog);
	file.Write(m_negFog);
}



AWspace		
AWSeaObject::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ + 
					 AWObject::binaryStoreSize() +
					 sizeof(m_curFog) +
					 sizeof(m_posFog) +
					 sizeof(m_negFog);
	return retVal;
}
/*
protected:
	//-1 for OFF, anything else for ON (at the moment)
	int					getFog(float t, const AWSolidCone& viewCone);
	int					m_curFog;
*/

int					
AWSeaObject::getFog(double t, const AWSolidCone& viewCone)
{	//returns a fog ID to use
	//or -1 for none
	int retVal = -1;
	if (getController())
	{	//this version for case where there's a controller assigned
		AWPoint seaSurface;
		getController()->getWSPosition(seaSurface);
		//expresses the ON / OFF condition
		if (seaSurface.y >= viewCone.m_V.y)
		{	//FOG SHOULD BE ON!
			retVal = m_negFog; //we're below sea level
		}
		else
		{
			retVal = m_posFog; //above sea level
		}
	}//if (getController())
	return retVal;
}//int	AWSeaObject::getFog(float t, const AWSolidCone& viewCone)



int
AWSeaObject::calcViewDependentData(double t, const AWSolidCone& viewCone)
{
	AWObject::calcViewDependentData(t, viewCone);
	int useFog = getFog(t, viewCone);
	if (m_curFog != useFog)
	{
		m_curFog = useFog;
      if (m_renderer)
      {
		   m_renderer->useFog(m_curFog);
      }//if (m_renderer)
	}//if (m_curFog != useFog)
	return 1;
}//int AWSeaObject::calcViewDependentData(double t, const AWSolidCone& viewCone)
