#ifndef _AWTextureUnit_H
#define _AWTextureUnit_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWMatrix4.h"


class AWExport AWTextureUnit : public AWPersistable
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWTextureUnit);

    enum EnvMode   {MODULATE,   DECAL,       BLEND,  REPLACE, COMBINE, E_INIT};

   AWTextureUnit(AWTexture* tex=NULL, int chanID=0);
   AWTextureUnit(const AWTextureUnit& p) {*this=p;}

   virtual              ~AWTextureUnit(){;}

   AWTextureUnit&       operator=(const AWTextureUnit& ot);
   int                  operator==(const AWTextureUnit& o)const;
   int                  operator!=(const AWTextureUnit& o)const 
                              {return !operator==(o);}

   void                 resetCombinerData();

	virtual void         restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;

	virtual AWspace      binaryStoreSize() const;

   int                  m_chanID;
   AWTexture*           m_tex;
   AWMatrix4            m_texMatrix;
   //TexEnf data
   EnvMode			      m_texFunc;     //for glTexEnvf

   int                  m_combineMode;
   int                  m_srce0Name;
   int                  m_srce0Operand;
   int                  m_srce1Name;
   int                  m_srce1Operand;
};//class AWTextureUnit



inline AWTextureUnit&       
AWTextureUnit::operator=(const AWTextureUnit& ot)
{
   m_chanID       = ot.m_chanID;
   m_tex          = ot.m_tex;
   m_combineMode  = ot.m_combineMode;
   m_srce0Name    = ot.m_srce0Name;
   m_srce0Operand = ot.m_srce0Operand;
   m_srce1Name    = ot.m_srce1Name;
   m_srce1Operand = ot.m_srce1Operand;
   m_texFunc      = ot.m_texFunc;

   m_texMatrix    = ot.m_texMatrix;
   return *this;
}//AWTextureUnit& AWTextureUnit::operator=(const AWTextureUnit& ot)


inline int                  
AWTextureUnit::operator==(const AWTextureUnit& o)const
{
   return   (m_chanID       == o.m_chanID) && (m_tex == o.m_tex) &&
            (m_combineMode  == o.m_combineMode) &&
            (m_srce0Name    == o.m_srce0Name) &&
            (m_srce0Operand == o.m_srce0Operand) &&
            (m_srce1Name    == o.m_srce1Name) &&
            (m_srce1Operand == o.m_srce1Operand) &&
            (m_texFunc      == o.m_texFunc);
}//int AWTextureUnit::operator==(const AWTextureUnit& o)const


#endif //sentinel _AWTextureUnit_H