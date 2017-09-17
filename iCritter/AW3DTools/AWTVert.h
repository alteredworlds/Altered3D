#ifndef __AWTVERT_H
#define __AWTVERT_H

#include "AWTooldefs.h"

//class AWTVert has been depreciated. We no longer use arrays of AWTVert
//but instead AWTVertexList which can hold float[2] or float[3] depending on
//setting of m_dimension member
//
class AWExport AWTVert 
{
public:
   AWTVert(const AWTVert& p) {*this=p;}
   AWTVert(float ix=0, float iy=0)       : x(ix), y(iy){;}

   ~AWTVert() {;}

   AWTVert& operator=(const AWTVert& ot) {x = ot.x; y = ot.y; return *this;}
   int operator==(const AWTVert& o)const {return x==o.x && y==o.y;}
   int operator!=(const AWTVert& o)const {return !operator==(o);}

   friend AWFile& operator>>(AWFile& file, AWTVert& obj);
   friend AWFile& operator<<(AWFile& file, AWTVert& obj);

   void			saveGuts(AWFile& file) const;
	void			restoreGuts(AWFile& file);

   float       x;
   float       y;
};//class AWTVert



inline AWFile& operator>>(AWFile& file, AWTVert& it)
{
   it.restoreGuts(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWTVert& it)



inline AWFile& operator<<(AWFile& file, AWTVert& it)
{
  it.saveGuts(file);
  return file;
}//AWFile& operator<<(AWFile& file, AWTVert& it)

#endif //sentinel __AWTVERT_H