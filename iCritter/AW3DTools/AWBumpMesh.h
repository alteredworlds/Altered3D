#ifndef __AWBUMPMESH_H
#define __AWBUMPMESH_H

#include "AWMesh.h"
#include "AWMatrix3.h"

class AWExport AWLight; //forward definition/ could just include


class AWExport AWBumpMesh : public AWMesh
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWBumpMesh);

	AWBumpMesh(int nVerts = 0, float time = 0.0f);
	AWBumpMesh(const AWBumpMesh& other);
	~AWBumpMesh();

	AWBumpMesh& operator=(const AWBumpMesh& other);
	AWBoolean		sameAs(AWBumpMesh* other);

	void			   offset(const AWPoint& offset);
	void			   scale(const AWPoint& scale);

   virtual void   init(float t);
   virtual void	interpolate(AWMesh* b, AWMesh* a, float fac);

   virtual void   update(const AWMatrix4& objTM, float t, const AWSolidCone& viewCone);

   virtual void   buildTextureSpace(AWBoolean find_duplicate_verts=TRUE);
   void           calculateVertexGradients(AWPoint* av_du, AWPoint* av_dv);
   void           calc_gradients(AWPoint& du, AWPoint& dv, int i0, int i1, int i2);	

//AWPersistable VIRTUALS
	AWBoolean		isEqual(const AWPersistable* o) const;
	int				compareTo(const AWPersistable* o) const;
   virtual void   restoreGuts(AWFile& file);
   virtual void   saveGuts(AWFile& file) const;

	virtual AWspace binaryStoreSize() const;

//MEMBER DATA
   AWLight*          m_bumpLight;
//AWMesh holds vertices as: AWList<AWPoint> m_verts;
//need to hold AWMatrix3 for each vertex
   AWList<AWMatrix3> m_tangentSpace;
   //helper to prevent memory leaks - will bo optimized away in the
   //end, I guess!
   AWPtrList<AWTVertexList*>  m_tvHelper;
};//class AWExport AWBumpMesh  : public AWPersistable



inline 
AWBumpMesh::AWBumpMesh( int nVerts, float time)
	: AWMesh(nVerts, time), m_bumpLight(NULL)
{

}



inline
AWBumpMesh::AWBumpMesh(const AWBumpMesh& other)	
{
	*this=other;
}

   

inline 
AWBumpMesh::~AWBumpMesh()	
{	//members distructors are called, deleting contained data
   //unless ownership has been transferred
}



inline AWBumpMesh& 
AWBumpMesh::operator=(const AWBumpMesh& other)
{	//call base class version - nothing added as yet
	(AWMesh&)*this = (const AWMesh&)other;
   m_bumpLight    = other.m_bumpLight;
   m_tangentSpace = other.m_tangentSpace;
	return *this;
}//inline AWBumpMesh& AWBumpMesh::operator=(const AWBumpMesh& other)


#endif //sentinel __AWBUMPMESH_H