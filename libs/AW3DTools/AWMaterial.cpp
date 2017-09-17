#include "AWMaterial.h"

#define VER_AWMaterial_1	100
#define VER_AWMaterial_2	102



AWDEFINE_VERSIONABLE_PERSISTABLE(AWMaterial, CLSID_AWMATERIAL, VER_AWMaterial_2);


AWMaterial::AWMaterial(const AWCString& name) :
	m_ambient(0.2f, 0.2f, 0.2f),
	m_diffuse(0.8f, 0.8f, 0.8f),
	m_shininess(0.0f),
	m_twoSided(FALSE),
   m_blendType(AWTexture::B_NONE),
   m_texEffects(1),
   m_useDefaults(FALSE),
   m_name(name),
   m_wire(0)
{	//assign the default values for OpenGL materials
	//specular is already at the defualt (0,0,0)
	//emissive colour is already at the default (0,0,0)
}//AWMaterial::AWMaterial()



AWMaterial::~AWMaterial()
{  //material DOES own the texture effects?	
}//AWMaterial::~AWMaterial()



AWTexture*                    
AWMaterial::findTexture(AWTexture* tex)
{
   AWTexture* retVal = NULL;
   if (tex)
   {
      AWTextureEffect* te = NULL;
      for (int i=0; i<getNumTexEffects(); i++)
      {
         if (te = getTextureEffect(i))
         {
            if (tex == te->m_tex)
            {
               retVal = tex;
               break;
            }//if (tex == te->m_tex)
         }//if (te = getTextureEffect(i))
      }//for (int i=0; i<getNumTexEffecst(); i++)
   }//if (tex)
   return retVal;
}//AWTexture* AWMaterial::findTexture(AWTexture* tex)



void   
AWMaterial::dump()
{  //output trance information
   AWTRACE3("  Ambient:  (%5f, %5f, %5f)\n", (double)m_ambient[0], (double)m_ambient[1], (double)m_ambient[2]);
   AWTRACE3("  Diffuse:  (%5f, %5f, %5f)\n", (double)m_diffuse[0], (double)m_diffuse[1], (double)m_diffuse[2]);
   AWTRACE3("  Specular: (%5f, %5f, %5f)\n", (double)m_specular[0], (double)m_specular[1], (double)m_specular[2]);
   AWTRACE3("  Emmissive:(%5f, %5f, %5f)\n", (double)m_emissive[0], (double)m_emissive[1], (double)m_emissive[2]);
	AWTRACE2("  Shininess %f   TwoSided %i\n", (double)m_shininess, (int)m_twoSided);
}//void AWMaterial::dump()


void        
AWMaterial::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
   if (version <= VER_AWMaterial_2)
   {
		AWPersistable::restoreGuts(file);
		m_ambient.restoreGuts(file);
		m_diffuse.restoreGuts(file);
		m_specular.restoreGuts(file);
		file >> m_shininess;
		m_emissive.restoreGuts(file);
		file >> m_twoSided;
      if (version >= VER_AWMaterial_2)
      {
         file >> m_name;
         file >> m_useDefaults;
         file >> m_wire;
         file >> m_texEffects;
      }//if (version >= VER_AWMaterial_2)
   }//if (version <= VER_AWMaterial_2)
}//void AWMaterial::restoreGuts(AWFile& file) const



void        
AWMaterial::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	m_ambient.saveGuts(file);
	m_diffuse.saveGuts(file);
	m_specular.saveGuts(file);
	file << m_shininess;
	m_emissive.saveGuts(file);
	file << m_twoSided;
   file << m_name;
   file << m_useDefaults;
   file << m_wire;
   file << m_texEffects;
}//void AWMaterial::saveGuts(AWFile& file) const



AWspace     
AWMaterial::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
      m_name.binaryStoreSize() +
		sizeof (m_ambient) +
		sizeof (m_diffuse) +
		sizeof (m_specular) +
		sizeof (m_shininess) +
		sizeof (m_emissive) +
		sizeof (m_twoSided) +
      sizeof (m_wire) +
      sizeof (m_useDefaults) +
      m_texEffects.getStoreSize();
	return retVal;
}//AWspace  AWMaterial::binaryStoreSize() const