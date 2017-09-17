#ifndef _AW_CUBE_GEN_H
#define _AW_CUBE_GEN_H

#include "AWCubeEnvTexture.h"


//stores association between an auto Cube Env Map
//and the set of objects to which it applies.
class AWExport AWCubeGen
{
public:
   AWCubeGen(AWCubeEnvTexture* c=NULL) : m_tex(c), m_objs(1, 0), m_vis(1) {;}
   ~AWCubeGen(){;}

   void     addObject(AWObject* obj);
   void     hideAllObjects();
   void     showAllObjects();
   int      getNumObjects()         {return m_objs.getNum();}

   friend AWFile& operator>>(AWFile& file, AWCubeGen& obj){return file;}
   friend AWFile& operator<<(AWFile& file, AWCubeGen& obj){return file;}
   friend AWFile& operator>>(AWFile& file, AWCubeGen*& obj){obj=NULL;return file;}
   friend AWFile& operator<<(AWFile& file, AWCubeGen* obj){return file;}

   //the auto-generating cube map texture
   AWCubeEnvTexture*    m_tex;
   //the set of objects which use this texture
   AWPtrList<AWObject*> m_objs;
   //the calculated visibility 'values' 
   //for each object for current frame
   AWList<int>          m_vis;
};//class AWExport AWCubeGen



inline void   
AWCubeGen::hideAllObjects()
{
   AWObject* obj = NULL;
   if (m_vis.getNum() != m_objs.getNum())
   {
      m_vis.destroy();
      m_vis.fillWith(0, m_objs.getNum());
   }//if (m_vis.getNum() != m_objs.getNum())
   for (int i=0; i<m_objs.getNum(); i++)
   {
      obj = m_objs[i];
      if (obj) 
      {
         m_vis[i] = (int)obj->m_visible.getVal();
         obj->m_visible.setVal(0.0f);
      }//if (obj) 
   }//for (int i=0; i<m_objs.getnum(); i++)
}//void AWCubeGen::hideAllObjects()



inline void   
AWCubeGen::showAllObjects()
{
   AWObject* obj = NULL;
   for (int i=0; i<m_objs.getNum(); i++)
   {
      if ((obj = m_objs[i])) 
      {
         obj->m_visible.setVal(i<m_objs.getNum() ? m_vis[i] : 0.0f);
      }//if (obj = m_objs[i]) 
   }//for (int i=0; i<m_objs.getnum(); i++)
}//void AWCubeGen::showAllObjects()



inline void     
AWCubeGen::addObject(AWObject* obj)
{
   if (obj)
   {  //add the object to the list
      m_objs.add(obj);
      //preserve the current visibility information
      m_vis.add((int)obj->m_visible.getVal());
   }
}//inline void AWCubeGen::addObject(AWObject* obj)
#endif