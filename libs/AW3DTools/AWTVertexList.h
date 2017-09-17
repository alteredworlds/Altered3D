#ifndef __AWTVertexList_H
#define __AWTVertexList_H


#include "AWToolsCommon.h"
#include "AWList.h"
#include "AWTVert.h"
#include "AWPoint.h"

typedef AWExport AWList<float> AWFloatList;

#pragma warning(disable: 4275)
class AWExport AWTVertexList : public AWFloatList
{
public:
   AWTVertexList(const AWTVertexList& p) {*this=p;}
   AWTVertexList(int s=0, int dim=2, int ownsData=1) : 
                           AWFloatList(s*dim, ownsData), m_dimension(dim)  {;}

   ~AWTVertexList()     {;}

   AWTVertexList&       operator=(const AWTVertexList& ot);
   int                  operator==(const AWTVertexList& o)const;
   AWTVertexList*       copy()const {return new AWTVertexList(*this);}
   const AWTVertexList& shallowCopyOf(const AWList<AWTVert>& o);
   const AWTVertexList& shallowCopyOf(const AWTVertexList& o);

   virtual size_t       getStoreSize() const;

   virtual const int    getNumTVerts() const;
   virtual void         delIndex(int i);
 
   int                  getDimension()       {return m_dimension;}
   void                 setDimension(int v)  {m_dimension=v;}

   float*               getTVertex(int n)const    {return &m_element[m_dimension*n];}
   int                  getTVertex(AWPoint& nextTVert, int tvIndex) const;

   int                  setTVertex(const AWPoint& nextTVert, int tvIndex);

   int                  find(const AWPoint& a) const;
   int                  find(const AWTVert& a) const;

   AWTVertexList&       fillWith(const AWPoint& a, unsigned int num);
   int                  add(const AWPoint& a);

   AWTVertexList&       fillWith(const AWTVert& a, unsigned int num);
   int 	               add(const AWTVert& a);

   int                  matchesAt(int index, const AWPoint& p) const;


   friend AWFile&       operator>>(AWFile& file,      AWTVertexList& obj);
   friend AWFile&       operator<<(AWFile& file,      AWTVertexList& obj);

   friend AWFile&       operator>>(AWFile& file,      AWTVertexList*& obj);
   friend AWFile&       operator<<(AWFile& file,      AWTVertexList* obj);


   virtual int			   write(AWFile& file) const;
	virtual int			   read(AWFile& file);

   int			         read(AWFile& file, AWBoolean noDimension);

   int                  m_dimension;
};//class AWTVertexList



inline const AWTVertexList& 
AWTVertexList::shallowCopyOf(const AWTVertexList& o)
{  //copy over the list data pointer and correct 
   //number of elements and so on.
   AWFloatList::shallowCopyOf((const AWFloatList&)o);
   m_dimension = o.m_dimension;
   return *this;
}//const AWTVertexList& AWTVertexList::shallowCopyOf(const AWTVertexList& o)



inline AWTVertexList& 
AWTVertexList::operator=(const AWTVertexList& ot)
{
   (AWFloatList&)*this = (const AWFloatList&)ot;
   m_dimension         = ot.m_dimension;
   return *this;
}//AWTVertexList& AWTVertexList::operator=(const AWTVertexList& ot)



inline int 
AWTVertexList::operator==(const AWTVertexList& o)const
{
   return (m_dimension == o.m_dimension) && 
           ((AWFloatList&)*this == (const AWFloatList&)o);
}//int AWTVertexList::operator==(const AWTVertexList& o)const


inline const int   
AWTVertexList::getNumTVerts() const    
{  //want to return the number of texture vertices in the list
   //taking into account the diemnsion of the vertices
   return m_dimension ? m_num / m_dimension : 0;
}//const int AWTVertexList::getNumTVerts() const



inline size_t 
AWTVertexList::getStoreSize() const
{
   return AWFloatList::getStoreSize() + sizeof(m_dimension);
}//inline size_t AWTVertexList::getStoreSize() const



inline int                  
AWTVertexList::find(const AWPoint& p) const
{
   int       retVal = -1;
   float*    tmp    = NULL;
   AWBoolean cmp    = FALSE;
   for (int k=0; k<getNumTVerts(); k++)
   {
      float* tmp    = getTVertex(k);
      if (tmp)
      {
         cmp = p.x==tmp[0] && p.y==tmp[1];
         if (3==m_dimension)
         {
            cmp &= p.z==tmp[2];
            retVal  = k;
         }//if (3==m_dimension)
      }//if (tmp)
      if (-1!=retVal) break;
   }//for (int k=0; k<getNumTVerts(); k++)
   return retVal;
}//int AWTVertexList::find(const AWPoint& a) const



inline int                  
AWTVertexList::find(const AWTVert& p) const
{
   int       retVal = -1;
   float*    tmp    = NULL;
   AWBoolean cmp    = FALSE;
   for (int k=0; k<getNumTVerts(); k++)
   {
      float* tmp    = getTVertex(k);
      if (tmp)
      {
         cmp = p.x==tmp[0] && p.y==tmp[1];
      }//if (tmp)
      if (-1!=retVal) break;
   }//for (int k=0; k<getNumTVerts(); k++)
   return retVal;
}//int AWTVertexList::find(const AWTVert& a) const



inline int 
AWTVertexList::matchesAt(int tvIndex, const AWPoint& p) const
{
   int retVal = -1;
   float* tmp = getTVertex(tvIndex);
   if (tmp)
   {
      retVal = p.x==tmp[0] && p.y==tmp[1];
      if (3==m_dimension)
      {
         retVal &= p.z==tmp[2];
      }//if (3==m_dimension)
   }//if (tmp)
   return retVal;
}//inline int AWTVertexList::matchesAt(int index, const AWPoint& p) const



inline int
AWTVertexList::setTVertex(const AWPoint& nextTVert, int tvIndex)
{
   int    retVal = -1;
   float* v = getTVertex(tvIndex);
   if (v)
   {
      v[0] = nextTVert.x;
      v[1] = nextTVert.y;
      if (3 == m_dimension)
      {
         v[2] = nextTVert.z;
      }//if (3 == m_dimension)
   }//if (v)
   return retVal;
}//inline int AWTVertexList::setTVertex(const AWPoint& nextTVert, int tvIndex);



inline AWTVertexList& 
AWTVertexList::fillWith(const AWPoint& a, unsigned int num)
{
   setSize(0);
	for (unsigned int i=0; i<num; i++)
   {
      add(a);
   }//for (int i=0; i<num; i++)
   pack();
	return *this;
}//inline AWTVertexList& AWTVertexList::fillWith(const AWPoint& a, unsigned int num)



inline AWTVertexList& 
AWTVertexList::fillWith(const AWTVert& a, unsigned int num)
{
   setSize(0);
	for (unsigned int i=0; i<num; i++)
   {
      add(a);
   }//for (int i=0; i<num; i++)
   pack();
	return *this;
}//inline AWTVertexList& AWTVertexList::fillWith(const AWTVert& a, unsigned int num)



inline int 	               
AWTVertexList::add(const AWTVert& a)
{  //returns the index into the sequence
   //of AWTVert, IFF the list is of dim 2
   int retVal = -1;
   AWFloatList::add(a.x);
   AWFloatList::add(a.y);
   if (3==m_dimension)
   {
      AWFloatList::add(0.0f); 
   }//if (2==m_dimension)
   return getNumTVerts()-1;
}//inline int AWTVertexList::add(const AWTVert& a)



inline int 	               
AWTVertexList::add(const AWPoint& a)
{
   int retVal = -1;
   AWFloatList::add(a.x);
   AWFloatList::add(a.y);
   if (3 == m_dimension)
   {    
      AWFloatList::add(a.z);
   }//if (3 == m_dimension)
   return getNumTVerts()-1;
}//inline int AWTVertexList::add(const AWPoint& a)



inline int                  
AWTVertexList::getTVertex(AWPoint& a, int tvIndex) const
{
   int retVal = -1;
   float* v = getTVertex(tvIndex);
   if (v)
   {
      a.x = v[0];
      a.y = v[1];     
      if (3==m_dimension)
      {
         a.z = v[2];
      }
      else
      {
         a.z = 0.0f;
      }
   }//if (v)
   return retVal;
}//inline int AWTVertexList::getTVertex(AWPoint& a, int tvIndex)



inline AWFile& operator>>(AWFile& file, AWTVertexList& it)
{
   it.read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWTVertexList& it)



inline AWFile& operator<<(AWFile& file, AWTVertexList& it)
{
  it.write(file);
  return file;
}//AWFile& operator<<(AWFile& file, AWTVertexList& it)



inline AWFile& operator>>(AWFile& file, AWTVertexList*& it)
{
   it = new AWTVertexList;
   it->read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWTVertexList*& it)



inline AWFile& operator<<(AWFile& file, AWTVertexList* it)
{
   if (it) it->write(file);
   return file;
}//AWFile& operator<<(AWFile& file, AWTVertexList* it)


#pragma warning(default: 4275)
#endif //sentinel __AWTVertexList_H