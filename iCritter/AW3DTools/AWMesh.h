#ifndef __AWMeshS_H
#define __AWMeshS_H

#include "AWToolsCommon.h"
#include "AWPoint.h"
#include "AWList.h"
#include "AWPtrList.h"
#include "AWOrdered.h"
#include "AWTVert.h"
#include "AWTVertexList.h"
#include "AWQuat.h"
#include "AWSphrCone.h"


#define MAX_NUM_TVERT_CHANNELS 10


class AWExport AWMesh : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWMesh);

	AWMesh(int nVerts = 0, float time = 0.0f);
	AWMesh(const AWMesh& other);
	~AWMesh();

	AWMesh& operator=(const AWMesh& other);
	AWBoolean		sameAs(AWMesh* other);

	void			   offset(const AWPoint& offset);
	void			   scale(const AWPoint& scale);

   virtual void   init(float t) {;}
	virtual void	interpolate(AWMesh* b, AWMesh* a, float fac);
   virtual void   update(const AWMatrix4& objTM, float t, const AWSolidCone& viewCone) {;}

   void           setID(int newID)        {m_ID=newID;}
   int            getID()                 {return m_ID;}

   const int      getNumVerts() const     {return m_verts.getNum();}
   const int      getNumColVerts()const   {return m_colVerts.getNum();}
   const int      getNumNormals() const   {return m_norms.getNum();}
   const int      getNumTVerts(int s=0)const {return m_tVerts.getNum() > s ? m_tVerts[s]->getNumTVerts():0;} 
   const int      getNumTVChannels()      {return m_tVerts.getNum();}

   int            addVertex(const AWPoint& v);
   int            addTVertex(const AWPoint& t, int chanID=0);
   int            addTVertex(const AWTVert& t, int chanID=0);
   int            addColorVertex(const AWColVertex& t);
   int            addNormal(const AWPoint& t);
   int            addIndex(const unsigned short i);


   AWPoint*		   getVertex(int i)       {return &m_verts[i];}
   AWColVertex*   getColourVertex(int i)  {return &m_colVerts[i];}
   AWPoint*		   getNormal(int i)        {return &m_norms[i];}
   //Texture vertex is a special case, as it can hold either 2|3D vectors
   //basic unit of data is now float and getTVertex must return float
   //until we have a type AWTVertex that could be float[2] OR float[3] -
   //for now this is much better done at the level of the container
   float*		   getTVertex(int i, int s=0) {return m_tVerts[s]->getTVertex(i);}
   void           setTVertex(const AWPoint& p, int i, int s=0);
   unsigned short getIndex(int i)         {return m_indeces[i];}

   AWPoint*       getVertexList()         {return m_verts.getList();}
   AWColVertex*   getColVertexList()      {return m_colVerts.getList();}
   float*         getTVertexList(int s=0) {return m_tVerts.getNum() > s ? m_tVerts[s]->getList():NULL;}
   int            getTVertexListDim(int s=0) {return m_tVerts.getNum() > s ? m_tVerts[s]->getDimension():0;} 
   AWPoint*       getNormalList()         {return m_norms.getList();}
   unsigned short*  getIndexList()          {return m_indeces.getList();}

//AWPERSISTABLE VIRTUALS
   virtual void   restoreGuts(AWFile& file);
   virtual void   saveGuts(AWFile& file) const;

	virtual AWspace binaryStoreSize() const;

   void           removeShared();

//MEMBER DATA
   AWList<AWPoint>            m_verts;
   AWList<AWPoint>            m_norms; 
   AWList<AWColVertex>        m_colVerts;
   AWPtrList<AWTVertexList*>  m_tVerts;   //may be multiple sets of TVerts
   AWList<unsigned short>     m_indeces;  //face indeces
	
	float			               m_time;  //time key for the frame
   //NOT PERSISTED
   int                        m_ID;

protected:
   void getTuple(int i, AWPoint& v, AWPoint& n, AWColVertex& c, AWPoint& tv);
   void  removeTupleAndRemapIndex(unsigned int src, unsigned int dst);
};//class AWspace AWMesh  : public AWPersistable

   class tmpColV 
	{ 
	public:
		unsigned char x;
		unsigned char y;
		unsigned char z;
		
		friend AWFile& operator>>(AWFile& file, tmpColV& obj);
		friend AWFile& operator<<(AWFile& file, tmpColV& obj);
	};

inline AWFile& operator<<(AWFile& file, tmpColV& it)
{
	file.Write(it.x);
	file.Write(it.y);
	file.Write(it.z);
	return file;
}//AWFile& operator<<(AWFile& file, tmpColV& it)


inline AWFile& operator>>(AWFile& file, tmpColV& it)
{
   file.Read(it.x);
	file.Read(it.y);
	file.Read(it.z);
   return file;
}//AWFile& operator>>(AWFile& file, tmpColV& it)

class AWExport AWAnimatedMesh : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWAnimatedMesh);

	AWAnimatedMesh(int numFrames=1);
	AWAnimatedMesh(const AWAnimatedMesh& other)	{*this=other;}
	~AWAnimatedMesh();
	AWAnimatedMesh& operator=(const AWAnimatedMesh& other);

	virtual void	compact();
   virtual void   clear();

   void           postLoadProcess();
   AWMesh*			calcMesh(double t, AWMesh* buf=NULL);
   int				linearInterp(AWMesh* val, AWMesh* pBefore, AWMesh* pAfter, float t);
   //virtual void   update(const AWMatrix4& objTM, float t, const AWSolidCone& viewCone);

   AWMesh*        newMeshBuffer();
   AWBoolean		insertFrame(AWMesh* data);
   int				getNumFrames()	{return m_frames.getNum();}
   AWMesh*			getMesh(int i)	{return (AWMesh*)m_frames[i];}		

   //helpers
   void			   offset(const AWPoint& offset);
   void			   scale(const AWPoint& scale);

   virtual void    restoreGuts(AWFile& file);
   virtual void    saveGuts(AWFile& file) const;

   virtual AWspace binaryStoreSize() const;

//MEMBER DATA
protected:
	AWOrdered		m_frames;
};//class AWspace AWAnimatedMesh : public AWPersistable




inline 
AWMesh::AWMesh( int nVerts, float time)
	: m_verts(nVerts), m_colVerts(nVerts), 
     m_time(time), m_ID(AW_NOLIST)
{

}

inline
AWMesh::AWMesh(const AWMesh& other)	:
	m_verts(other.getNumVerts()), m_colVerts(other.getNumVerts()), 
    m_time(0.0f), m_ID(AW_NOLIST)
{
	*this=other;
}

   

inline 
AWMesh::~AWMesh()	
{	//distructors are called for all data members 
   //deleting contained data unless 
   //ownership has been transferred
}//inline AWMesh::~AWMesh()


inline int            
AWMesh::addVertex(const AWPoint& v)
{
   return m_verts.add(v);
}//inline int AWMesh::addVertex(const AWPoint& v)



inline int            
AWMesh::addTVertex(const AWPoint& t, int chanID)
{
   int retVal = -1;
   if ((chanID>m_tVerts.getNum()) && (chanID<MAX_NUM_TVERT_CHANNELS))
   {
      for (int i=m_tVerts.getNum(); i<=chanID; i++)
      {
         m_tVerts.add(new AWTVertexList);
      }//for (int i=m_tVerts.getNum(); i<chanID; i++)
   }//if ((chanID>m_tVerts.getNum() && (chanID<MAX_NUM_TVERT_CHANNELS))
   if (chanID<m_tVerts.getNum())
   {
      retVal = m_tVerts[chanID]->add(t);
   }//if (chanID<m_tVerts.getNum())
   return retVal;
}//inline int AWMesh::addTVertex(const AWPoint& t, int chanID)



inline int            
AWMesh::addTVertex(const AWTVert& t, int chanID)
{
   int retVal = -1;
   if ((chanID>=m_tVerts.getNum()) && (chanID<MAX_NUM_TVERT_CHANNELS))
   {
      for (int i=m_tVerts.getNum(); i<=chanID; i++)
      {
         m_tVerts.add(new AWTVertexList);
      }//for (int i=m_tVerts.getNum(); i<chanID; i++)
   }//if ((chanID>m_tVerts.getNum() && (chanID<MAX_NUM_TVERT_CHANNELS))
   if (chanID<m_tVerts.getNum())
   {
      retVal = m_tVerts[chanID]->add(t);
   }//if (chanID<m_tVerts.getNum())
   return retVal;
}//inline int AWMesh::addTVertex(const AWTVert& t, int chanID)



inline int            
AWMesh::addIndex(const unsigned short i)
{
   return m_indeces.add(i);
}//inline int AWMesh::addIndex(const unsigned short i)



inline int            
AWMesh::addColorVertex(const AWColVertex& t)
{
   return m_colVerts.add(t);
}//inline int AWMesh::addColorVertex(const AWPoint& t)



inline int            
AWMesh::addNormal(const AWPoint& t)
{
   return m_norms.add(t);
}//inline int AWMesh::addNormal(const AWPoint& t)


inline void           
AWMesh::setTVertex(const AWPoint& p, int i, int s)
{
   if (s < m_tVerts.getNum())
   {  //valid texture coordinate 'channel'
      m_tVerts[s]->setTVertex(p, i);
   }//if (s < m_tVerts.getNum())
}//void AWMesh::setTVertex(const AWPoint& p, int i, int s)

/*
inline void 
AWMesh::quickassign(const AWMesh& other)
{ //shallow copy of vertex data
   m_verts        = other.m_verts;
   m_colVerts     = other.m_colVerts;
   m_ID           = other.m_ID;
   m_norms        = other.m_norms;
   m_indeces      = other.m_indeces;
   m_tVerts       = other.m_tVerts;   //may be multiple sets of TVerts

   //BUG? what about time -?
}//inline void AWMesh::copy(const AWMesh& other)
*/


inline AWMesh& 
AWMesh::operator=(const AWMesh& other)
{	//DEEP COPY of vertex data
	m_time         = other.m_time;
	m_norms        = other.m_norms;
   m_verts        = other.m_verts;
   m_colVerts     = other.m_colVerts;
   m_indeces      = other.m_indeces;
   m_ID           = other.m_ID;

   for (int i=0; i< other.m_tVerts.getNum(); i++)
   {
      if (other.m_tVerts[i]->getNum())
      {
         m_tVerts.add((AWTVertexList*)other.m_tVerts[i]->copy());
      }//if (numTVerts)
   }//for (int i=0; i< other.m_tVerts.getNum(); i++)
   m_tVerts.pack();
   m_tVerts.setOwnsList(TRUE);
   m_tVerts.setOwnsObjects(TRUE);
	return *this;
}//inline AWMesh& AWMesh::operator=(const AWMesh& other)



inline 
AWAnimatedMesh::AWAnimatedMesh(int numFrames) : m_frames(numFrames)
{
}


inline 
AWAnimatedMesh::~AWAnimatedMesh()
{	//always assumes ownership of all AWMeshs
	//m_frames.destroy();
}


inline void
AWAnimatedMesh::compact()
{
	m_frames.pack();
}



inline void
AWAnimatedMesh::clear()
{
   for (int i=0; i<m_frames.getNum(); i++)
   {
      m_frames[i] = NULL;
   }
}//inline void AWAnimatedMesh::clear()



inline AWAnimatedMesh& 
AWAnimatedMesh::operator=(const AWAnimatedMesh& other)
{
	//this should be a DEEP copy (check somewhere 
	//else isn't relying on this shit)
   m_frames.destroy();
	for (int i =0; i < (int)other.m_frames.getNum(); i++)
	{
		m_frames.add((AWMesh*)other.m_frames[i]->copy());
	}
	return *this;
}


#endif //sentinel __AWMeshS_H