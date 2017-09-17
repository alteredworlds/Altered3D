#ifndef __AWMATERIAL_H
#define __AWMATERIAL_H


#include "AWToolsCommon.h"
#include "AWPoint.h"
#include "AWPtrList.h"
#include "AWTextureEffect.h"
#include "AWMatrix3.h"
#include "AWQuat.h"
#include "AWMatrix4.h"


class AWExport AWMaterial : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWMaterial);

	AWMaterial(const AWCString& name="");
	virtual			               ~AWMaterial();

   int operator==(const AWMaterial& other) {return isEqual(&other);}

   virtual AWBoolean		         isEqual(const AWPersistable* o)const;
   virtual void                  dump();
   void                          setTwoSided(AWBoolean v)   {m_twoSided=v;}
   AWBoolean                     getTwoSided()              {return m_twoSided;}

   void                          setWire(int v)             {m_wire=v;}
   int                           getWire()                  {return m_wire;}

   int					            getBlendType()			      {return m_blendType;}
	void				               setBlendType(int val)	   {m_blendType=val;}

   AWBoolean                     getUseDefaults()            {return m_useDefaults;}
   void                          setUseDefaults(AWBoolean v) {m_useDefaults=v;}

   void                          addTextureEffect(AWTextureEffect* tex);
   int                           getNumTexEffects()         {return m_texEffects.getNum();}
   AWTextureEffect*              getTextureEffect(int i)    {return m_texEffects[i];}

   AWTexture*                    findTexture(AWTexture* tex);


//AWPersistable virtuals
	virtual void	               restoreGuts(AWFile& file);
   virtual void	               saveGuts(AWFile& file) const;

	virtual AWspace               binaryStoreSize() const;

public:
   AWCString                     m_name;
	AWPoint			               m_ambient;
	AWPoint			               m_diffuse;
	AWPoint			               m_specular;
	float			                  m_shininess;
	AWPoint			               m_emissive;
	AWBoolean		               m_twoSided; 
   int		                     m_wire;
   int					            m_blendType;
   int                           m_useDefaults;
   AWPtrList<AWTextureEffect*>   m_texEffects;  
};//class AWMaterial : public AWPersistable



inline AWBoolean		         
AWMaterial::isEqual(const AWPersistable* o) const
{
   if (!o) return FALSE;
   AWMaterial* tmp = (AWMaterial*)o;
   AWBoolean retVal= 0==m_name.compare(tmp->m_name);
   retVal &= m_ambient==tmp->m_ambient   && m_diffuse==tmp->m_diffuse &&
               m_specular==tmp->m_specular && m_shininess==tmp->m_shininess &&
               m_emissive==tmp->m_emissive && m_twoSided==tmp->m_twoSided &&
               m_wire==tmp->m_wire         && m_blendType==tmp->m_blendType &&
               m_useDefaults==tmp->m_useDefaults && m_texEffects==tmp->m_texEffects;
   return retVal;
}//inline AWBoolean AWMaterial::isEqual(const AWPersistable* o) const



//insert a nwe Texture into the object's materialList
//note that the AWMaterial does NOT delete the contents of this list
inline void				 
AWMaterial::addTextureEffect(AWTextureEffect* tex)
{
	m_texEffects.add(tex);
}//void AWMaterial::addTextureEffect(AWTextureEffect* tex)


#endif //sentinel __AWMATERIAL_H