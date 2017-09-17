#ifndef _AW_SHADOW_MAP_H
#define _AW_SHADOW_MAP_H


#include "AWToolsCommon.h"
#include "AWList.h"
#include "AWTexture.h"

class AWExport AWShadowMap : public AWTexture
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWShadowMap);

   AWShadowMap(const AWCString& name = "", int size=512, float bias=4.0f, float scale=1.1f);

   AWShadowMap(const AWShadowMap& other) {*this == other;}

   ~AWShadowMap();

//int                  operator==(const AWShadowMap& o);
//NVIDIA/SGI style shadow map - rendered at run time
   virtual int          init();
   float                getBias()          {return m_bias;}
   void                 setBias(float v)   {m_bias=v;}

   float                getRange()         {return m_range;}
   void                 setRange(float v)  {m_range=v;}

   virtual int          getDataType()      {return GL_UNSIGNED_INT;}



//OVERRIDES OF BASE CLASS
	void 			         reSize(int xsize, int ysize);
//AWPERSISTABLE VIRTUALS
	virtual void         restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;

	virtual AWspace      binaryStoreSize() const;

protected:
   float                m_range;
   float                m_bias;
   int                  m_dataType;
};//class AWExport AWShadowMap : public AWTexture

#endif //_AW_SHADOW_MAP_H