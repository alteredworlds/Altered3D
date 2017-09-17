#include "AWToolsCommon.h"

#include "AWObject.h"
#include "AWTransformController.h"
#include "AWFlightController.h"
#include "AWTexture.h"
#include "AWMesh.h"
#include "AWMaterial.h"
#include "AWList.h"
#include "AWGLRenderer.h"

//WIN32 only?
#include <limits.h> //for UINT_MAX, UCHAR_MAX
#include <float.h>   //for FLT_MAX


#define VER_AWOBJECT_1	100
#define VER_AWOBJECT_2	105
#define VER_AWOBJECT_3	108
#define VER_AWOBJECT_4	110
#define VER_AWOBJECT_5  112
#define VER_AWOBJECT_6  114


//tag array elements that aren't yet filled with new data
const AWPoint        maxPoint(FLT_MAX, FLT_MAX, FLT_MAX); 
const AWPoint        maxTVert(FLT_MAX, FLT_MAX, 0.0f);
const AWColVertex    maxCol(UCHAR_MAX, UCHAR_MAX);



AWDEFINE_VERSIONABLE_PERSISTABLE(AWObject,	CLSID_AWOBJECT, VER_AWOBJECT_6);


AWObject::AWObject(const AWCString& name, AWNode* parent, double life) :
	AWNode(name, parent, life)	
{
    initialValues();
	m_visible.setUsage(AWAnimatedFloat::discrete);
}


AWObject::AWObject(const AWObject& other)
{
    initialValues();
    //DEEP COPY
	*this=other;
	m_ownsVertexData = TRUE;
}


AWObject::~AWObject()
{
	destroyMyData();
}//AWObject::~AWObject()


void
AWObject::initialValues()
{
    m_vertexList	        = NULL;
	m_vertColList           = NULL;
	m_normalList            = NULL;
	m_curAnimMesh           = NULL;
	m_curMesh               = NULL;
	m_curRenderPass         = NULL;
	m_material              = NULL;
	m_ownsVertexData        = TRUE;
	m_radius                = 0;
	m_collide               = 0;
	m_useDynamicLights      = FALSE;
	m_usingFrameBuffer      = FALSE;    //by default we don't interpolate or deform the verts
    m_passes                = 1;        //one pass by default
    m_animMeshes            = 1;        //no dynamic insertions of animations
	m_isVisibleThisFrame    = 0;        //calculated on a per-frame basis
	m_listID                = AW_NOLIST;
    m_separateSpecular      = FALSE;
	m_distanceTest          = 0;        //distances calculated from object center
	m_animStart             = 0.0f;
	m_animNum               = -1;
    m_vertexBuffer          = 0;
    m_colourBuffer          = 0;
    m_normalBuffer          = 0;
    m_texVertexBufer        = 0;
    m_indexBuffer           = 0;
}

void
AWObject::clear()
{
	//with proper usage the following collections:
	//m_passes, m_textureList, m_animMeshes
	//take care of its own sharing INTERNALLY
	m_passes.destroy();
	//m_textureList.destroy();
	m_animMeshes.destroy();
	
	m_vertexList	         = NULL;
	m_vertColList           = NULL;
	m_normalList            = NULL;
	m_curAnimMesh           = NULL;
	m_curMesh               = NULL;
	m_curRenderPass         = NULL;
	m_material              = NULL;
	m_ownsVertexData        = TRUE;
	m_radius                = 0;
	m_collide               = 0;
	//m_twoSided              = FALSE;
	m_useDynamicLights      = FALSE;
	//m_blendType             = AWTexture::B_NONE;
	m_usingFrameBuffer      = FALSE; //by default we don't interpolate or deform the verts
	m_isVisibleThisFrame    = 0; //calculated on a per-frame basis
	m_listID                = AW_NOLIST;
	m_distanceTest          = 0; //distances calculated from object center
	m_animStart             = 0.0f;
	m_animNum               = -1;
}//void AWObject::clear()



void
AWObject::destroyMyData()
{
	//with proper usage the following collections:
	//m_passes, m_textureList, m_animMeshes
	//take care of its own sharing INTERNALLY
	int hadAnimatedMeshes   = m_animMeshes.getNum();
	//checkCurMesh();
	m_animMeshes.destroy();
	m_passes.destroy();
	//m_textureList.destroy();
	
	m_curAnimMesh           = NULL;
	m_parent                = NULL;
	//note that AWObjects NEVER share a frame buffer, so if
	//we're using one we must ALWAYS delete it
	//also delete m_curMesh in the case where there is no animation
	//and no frame buffering
	if (m_usingFrameBuffer)
	{	//assumes objects can't share a frame buffer, for now
		//so this object must own it's frame buffer, if one is in use
		delete m_curMesh;
	}
	else if (m_ownsVertexData && !hadAnimatedMeshes)
	{
		delete m_curMesh;
	}
	//if not interpolated, then either the clone source owns it (m_ownsVertexData = F)
	//or else it's just part of an animated frame collection
	//so just ZERO it now in either case
	m_curMesh = NULL;
	//facelist is owned either by this object or another
	if (m_ownsVertexData)
	{
		m_faceList.destroy();
	}//if (m_ownsVertexData)
	
	m_normalList            = NULL;
	//materials are now NEVER owned by the object
	m_material              = NULL;
	m_vertexList            = NULL;	//data owned by an AWFrame
	m_vertColList           = NULL;	//data owned by an AWFrame
	
	m_animNum               = -1;
	
	m_useDynamicLights      = FALSE;
	//m_blendType             = AWTexture::B_NONE;
}//void AWObject::destroyMyData()



void
AWObject::clearAndDestroy()
{	//break up the fn like this to allow for uncomplicated
	//calls from object destructors. right now we go -
	//~AObject -> automatically calls ~AWNode which calls
	//AWNode::clearAndDestroy . Now in ~AWObject we call
	//clearAndDestroy which calls AWNode::clearAndDestroy - OTT
	AWNode::clearAndDestroy();
	destroyMyData();	
}//void AWObject::clearAndDestroy()



int					
AWObject::getBlendType()			
{
   return m_material ? m_material->getBlendType() : AWTexture::B_NONE;
}//int		AWObject::getBlendType()



int
AWObject::operator==(const AWObject& other)
{
	return this == &other; //same as the default would be
}//int AWObject::operator==(const AWObject&)



AWObject& 
AWObject::operator=(const AWObject& other)
{
	int k=0;
	//clear existing data
	destroyMyData();
	//base class deep copy
	(AWNode&)*this       = (AWNode&)other;
	m_listID		         = other.m_listID;
	m_collide		      = other.m_collide;
	//m_twoSided		      = other.m_twoSided;
	//m_blendType		      = other.m_blendType;
	m_distanceTest       = other.m_distanceTest;
	m_center		         = other.m_center;	
	m_radius		         = other.m_radius;
	m_wsCenter		      = other.m_wsCenter; 
	m_animStart		      = other.m_animStart;
	m_visible		      = other.m_visible;
	m_useDynamicLights   = other.m_useDynamicLights;
	//object references materials anyway - doesn't own them
	m_material		      = other.m_material;
	
	if (other.getNumFaces())
	{	//deep copy all faces
		for (int face = 0; face < other.getNumFaces(); face++)
		{
			m_faceList.add(other.getFace(face)->copy() );
		}
		m_faceList.pack();
	}//if (other.getNumFaces())
	//this should be a deep copy of list, ie new copy
	//of each contained AWTextureEffect (but NOT of the
	//textures referred to by these AWTextureEffect objects
	//m_textureList now OWNS these contained members
	//for (k=0; k<other.m_textureList.getNum(); k++)
	//{
	//   m_textureList.add(other.m_textureList[k]->copy());
	//}
	
	//this should be a deep copy of AWRenderPasses (but not of the
	//textures referred to by these AWRenderPass objects
	//m_passes now OWNS these contained members
	for (k=0; k<other.m_passes.getNum(); k++)
	{
		m_passes.add((AWRenderPass*)other.m_passes[k]->copy());
	}
	
	//the meat of the problem is a deep copy of the VERTEX DATA
	m_curAnimMesh = NULL;
	m_curMesh     = NULL;
	for (int mesh =0; mesh < (int)other.m_animMeshes.getNum(); mesh++)
	{
		m_animMeshes.add(other.m_animMeshes[mesh]->copy());
	}
	m_animMeshes.pack();	
	//new buffer will be generated on calls to setAnimation
	//no buffering for single frame meshes
	m_usingFrameBuffer = other.m_usingFrameBuffer && m_animMeshes.getNum();
	
	//we have animations: m_curMesh will be either
	//(i)  pointer to MESH BUFFER (m_usingFrameBuffer)
	//     which will be created via a call to setAnimation
	//(ii) pointer to one of the m_animMeshes on 'other'
	//in either case we do NOT want to copy the value.
	
	if (!m_usingFrameBuffer && !m_animMeshes.getNum() && other.m_curMesh)
	{	//this is the actual mesh data; no animations/buffering
		m_curMesh = (AWMesh*)other.m_curMesh->copy();
	}//if (other.m_curMesh && !m_animMeshes.entries())
	
	//after a copy we DO own the vertex data
	m_ownsVertexData = TRUE;
	
	m_animNum		  = -1;
	setAnimation(other.m_animNum);
	setPass(0);
	calcMesh(0);
	return *this;
}//AWObject& AWObject::operator=(const AWObject& other)



AWObject& 
AWObject::clone(const AWObject& other)
{
	AWNode::clone((AWNode&)other); //calls virtual clearAndDestroy
	m_curMesh            = NULL;
	m_listID		         = other.m_listID;
	m_collide		      = other.m_collide;
	m_useDynamicLights   = other.m_useDynamicLights;
	//m_blendType		      = other.m_blendType;
	m_distanceTest       = other.m_distanceTest;
	m_center		         = other.m_center;	//object-local space
	m_radius		         = other.m_radius;
	m_wsCenter		      = other.m_wsCenter; 
	
	//shallowCopyOf for clone - m_animMeshes *shares* element data
	//and will *not* delete it
	m_animMeshes.shallowCopyOf(other.m_animMeshes);
	m_usingFrameBuffer   = other.m_usingFrameBuffer && m_animMeshes.getNum();
	if (!m_usingFrameBuffer && !m_animMeshes.getNum())
	{	//copy pointer value accross since 
		//either no animation(s)
		//or no BUFFERED animation
		//other.m_curMesh is therefore the shared mesh data
		m_curMesh         = other.m_curMesh;
	}//if (!m_animMeshes.entries() || !m_usingFrameBuffer)
	
	m_faceList           = other.m_faceList; //the face list itself is shared
	
	//shallowCopyOf for clone - m_textureList *shares* element data
	//and will *not* delete it
	//m_textureList.shallowCopyOf(other.m_textureList);
	//shallowCopyOf for clone - m_passes *shares* element data
	//and will *not* delete it
	m_passes.shallowCopyOf(other.m_passes);
	
	//copy accross the pointer value
	m_material           = other.m_material;
	
	m_visible            = other.m_visible;
	m_animStart          = other.m_animStart;
	//after a clone we do NOT own the vertex data
	m_ownsVertexData     = FALSE;
	
	m_animNum		      = -1;
	setAnimation(other.m_animNum);
	setPass(0);
	calcMesh(0);
	return *this;
}//AWObject& AWObject::clone(const AWObject& other)



AWNode* 
AWObject::clone() const
{
	AWObject* retVal = new AWObject;
	retVal->clone(*this);
	return retVal;
}//AWNode* AWObject::clone() const



void				 
AWObject::addFace(AWFace* face)
{
	m_faceList.add(face);
}//void	AWObject::addFace(AWFace* face)



AWBoolean 	 
AWObject::collide(const AWPoint& center, float radius)
{
	AWBoolean	retVal = FALSE;
	AWPoint		distance, V0;
	AWPoint		objCenter;
	//should first of all do a bounding sphere algorithm for the 
	//OBJECT itself - or a Bounding Box or Oriented Bounding Box match.
	//
	//then check to see if a collision is possible between the supplied
	//sphere and any face on this object
	//
	//initial implementation uses simple bounding sphere algorithm
	//this will be refined to check for intersections with faces
	//
	//need to transform the supplied center from World Space
	//to object-local space
	AWMatrix4 TM, invTM;
	getTransform(TM);
	TM.invert(invTM);
	//the following line is just a test of the inverse matrix code
	//AWMatrix4 isThisIdentity = invTM * TM;
	objCenter = invTM * center;
	for (int faceCnt = 0; faceCnt < getNumTris(); faceCnt++)
	{
		int base = faceCnt*3;
		if ( AWFace::collide( objCenter, radius,
							 getVertex(getIndex(base)),
							 getVertex(getIndex(base+1)),
							 getVertex(getIndex(base+2)) ) )
		{
			retVal = TRUE;
			break;
		}
	}//for (int faceCnt = 0; faceCnt < getNumFaces(); faceCnt++)
	return retVal;
}//AWBoolean AWObject::collide(const AWPoint& center, float radius)


/*
void                    
AWObject::reAssignTexture(AWPersistable* src, AWPersistable* dest)
{
    AWNode::reAssignTexture(src, dest);//calls for children
    for (int i=0; i<(int)m_textureList.getNum(); i++)
    {
        if (src == m_textureList[i])
        {
            m_textureList[i] = dest; 
        }//if (src == m_textureList[i])
    }//for (int i=0; i<m_textureList.entries(); i++)
}//void AWObject::reAssignTexture(AWPersistable* src, AWPersistable* dest)
*/

void	
AWObject::init(double t, AWGLRenderer& renderer)
{
	AWNode::init(t, renderer);
	//find the object-local center and radius
	//object local center is (0,0,0) by definition (ALWAYS?)
	//for now, ignore all animation frames except the first one
	//run through all the vertices and find the maximum distance from
	//the center - this is the radius
	float curRadius = 0.0f;
	m_radius = 0.0f;
	m_center = AWPoint(0.0f, 0.0f, 0.0f);
	if (m_curMesh)
	{
		m_curMesh->init(t); //careful - not called for all meshes on object
		for (int i = 0; i < m_curMesh->getNumVerts(); i++)
		{
			AWPoint* curVert = m_curMesh->getVertex(i);
			if (curVert)
			{
				curRadius = curVert->magnitude();
				if (curRadius > m_radius)
				{
					m_radius = curRadius;
				}//if (curRadius > m_radius)
			}//if (curVert)
		}//for (int i = 0; i < getNumVerts(); i++)
	}//if (m_curMesh)
	
	//must now apply any constant scale factor to this radius, using
	//the largest factor if it's non-uniform(UG)
	AWPoint posn, scale(1.0f, 1.0f, 1.0f);
	AWQuat rotn;
	getTransform(posn, scale, rotn);
	AWTRACEHI1("object %s ", getName().c_str());
	AWTRACEHI3(" scale (%.2f, %.2f, %.2f) ", (double)scale.x, (double)scale.y, (double)scale.z);
	m_radius = (float)fabs(scale.x * m_radius);
	AWTRACEHI1(" radius %f\n", (double)m_radius);
}//void	AWObject::init(float t, GLRenderer& renderer)



AWBoolean			 
AWObject::initMeshBuffer()
{	//setup an AWMesh buffer for interpolated animation, 
	//using curMesh as a template
	//
	//assume that if m_usingFrameBuffer is already true, 
	//then nothing needs to be done.
	AWBoolean retVal = FALSE;
	if (!m_usingFrameBuffer && m_curAnimMesh)
	{	//m_curMesh will point to an AWMesh owned by an AWAnimatedMesh
		//so no memory leak expected when reassigning this pointer
		m_curMesh = (AWMesh*)m_curMesh->copy();
		m_usingFrameBuffer = TRUE;
		retVal = TRUE;
	}
	return retVal;
}//AWBoolean AWObject::initMeshBuffer()



void 
AWObject::calcMesh(double time)
{
	if (m_curAnimMesh)
	{	//animated data case
		AWMesh* buf   = (m_usingFrameBuffer) ? m_curMesh : NULL;
		m_curMesh     = m_curAnimMesh->calcMesh(time, buf);
	}//if (m_curAnimMesh)
	if (m_curMesh)
	{	//update object's pointers based on current frame
		//these are used for performance reasons - investigate
		//efficient removal so that data duplication avoided
		m_vertexList      = (float*)m_curMesh->getVertexList();
		m_vertColList     = (unsigned char*)m_curMesh->getColVertexList();
		m_normalList      = (float*)m_curMesh->getNormalList();
		m_listID          = m_curMesh->getID();   //copy over frame id
	}//if (m_curMesh)
}//void AWObject::calcMesh(float time)



void
AWObject::calcViewDependentMesh(double time, const AWSolidCone& viewCone)
{
	if (m_curMesh)
	{
		m_curMesh->update(getWSTM(), time, viewCone);
	}
}//void AWObject::calcViewDependentMesh(float time, const AWSolidCone& viewCone)




void				
AWObject::setListID(int newID)	
{
	m_listID=newID;
	if (m_curMesh)
	{
		m_curMesh->setID(newID);
	}
	if ((-1 != m_animNum) && m_animMeshes.getNum())
	{
		((AWAnimatedMesh*)m_animMeshes[m_animNum])->getMesh(0)->setID(newID);
	}
}//void	AWObject::setListID(int newID)



int 
AWObject::getIndex(const AWPoint& vert, const AWList<AWPoint>& vl,
                              const AWPoint& norm, const AWList<AWPoint>& nl,
                              const AWPoint& tVert, const AWTVertexList& tvl,
                              const AWColVertex& cVert, const AWList<AWColVertex>& cvl)
{
	int retVal = -1;  //entry with these values NOT found
	AWBoolean foundIt = FALSE;
	for (int k=0; k < vl.getNum(); k++)
	{
		if (vert == vl[k])
		{  //we always have vertices...
			foundIt = TRUE;
		}
		if (k < nl.getNum())
		{  //we have normals on this object
			foundIt &= (norm == nl[k]);
		}
		if (k < cvl.getNum())
		{  //we have color verts on this object
			foundIt &= (cVert == cvl[k]);
		}
		if (k < tvl.getNumTVerts())
		{  //we have texture coords on this object
			foundIt &= tvl.matchesAt(k, tVert);
		} 
		if (foundIt) 
		{  //there's indexed data on this object matching
			//the supplied pattern - return the index...
			retVal = k;
			break;
		}//if (foundIt) 
	}//for (int k=0; k<verts.getNum(); k++)
	return retVal;
}//int AWObject::getIndex(const AWPoint& vert, ... colVert)




void
AWObject::processReferenceMesh(AWMesh* refMesh, AWList<int>& vertMap,
                               AWList<AWPoint>& inNorms,
                               AWTVertexList& inTVerts)
{
	if (!refMesh) return;
	//reference Mesh owns all it's data (doesn't *have* to)
	//subsequent AWMesh onjects representing animation frames
	//typically refer to refMesh's data for invariants
	//e.g.: vertCols, tVerts, possibly normals
	AWFace*              curFace        = NULL;
	//collections to fill and re-arrange
	AWTVertexList        dTVerts(0, inTVerts.getDimension());
	AWList<AWPoint>      dNorms;
	AWList<AWColVertex>  dColVerts;
	AWList<unsigned short> indeces;
	
	//references. If the arrays passed in for Norms, TVerts are empty
	//try getting this information from refMesh instead.
	//note that both may be empty - object may not have norms &/or tVerts
	AWList<AWPoint>& norms  = inNorms.getNum() ? inNorms : refMesh->m_norms;
	AWTVertexList&   tVerts = !inTVerts.getNumTVerts() && refMesh->m_tVerts.getNum() ? 
	*refMesh->m_tVerts[0] : inTVerts;
	
	//map collections, for using re-arrangements
	//on subsequent mesh frames
	AWList<int>          tVertMap;
	AWList<int>          normMap;
	AWList<int>          colVertMap;
	
	int                  numVerts        = refMesh->getNumVerts();
	int                  degenerates     = 0;
	AWBoolean            degenerateTuple = FALSE;
	
	//these index values are pulled from the AWFace
	int                  vIndex, tvIndex, nIndex, cvIndex;
	
	//these values make up the next multi-array entry
	AWPoint              nextVert, nextNorm;
	AWColVertex          nextColVert;
	AWPoint              nextTVert, tmpTVert;
	
	//we should *always* have vertices...
	//vertex collection is not reordered, just potentially *extended*
	//VERTEX and (perhaps) COLOR VERTEX data comes from the mesh
	if (numVerts && refMesh->getVertexList())
	{
		vertMap.fillWith(-1,  numVerts);    
	}
	if (numVerts && refMesh->getNumColVerts())
	{
		dColVerts.fillWith(maxCol, numVerts);
		colVertMap.fillWith(-1,  numVerts);     
	}  
	//NORMAL AND TEXTURE VERTEX INFORMATION comes from AWObject
	if (numVerts && norms.getNum())
	{
		dNorms.fillWith(maxPoint,  numVerts);
		normMap.fillWith(-1,  numVerts); 
	}
	if (numVerts && tVerts.getNumTVerts())
	{
		dTVerts.fillWith(maxTVert, numVerts);
		tVertMap.fillWith(-1,  numVerts);    
	}  
	
	//FACES STORED AT AWObject
	for (int i=0; i< getNumFaces(); i++)
	{
		curFace = getFace(i);
		if (!curFace) continue;   
		//get VA index for this face vertex
		for (int j=0; j<3; j++)
		{  //get normal/tvert/colvert for this face vert
			degenerateTuple = FALSE;
			//retrieve existing values from the AWFace
			vIndex    = curFace->v[j];
			tvIndex   = curFace->Tv[j];
			nIndex    = curFace->n[j];
			cvIndex   = curFace->cV[j];
			//get the vertex data
			if (-1 != vIndex)
			{  
				nextVert  = *refMesh->getVertex(vIndex);
			}
			if (-1 != cvIndex)
			{  //color specified for this vertex
				nextColVert       = *refMesh->getColourVertex(cvIndex);
				if (!(dColVerts[vIndex] == maxCol) && !(dColVerts[vIndex] == nextColVert) )
				{
					//fprintf(stderr, "degenerate colvert found\n");
					degenerates++;
					degenerateTuple = TRUE;
				}
				else 
				{
					colVertMap[vIndex] = cvIndex;
					cvIndex = vIndex;
				}
			}//if (-1 != cvIndex)
			if(-1 != nIndex)
			{  //normal specified for this vertex
				nextNorm          = norms[nIndex];
				if (!(dNorms[vIndex] == maxPoint) && !(dNorms[vIndex] == nextNorm) )
				{
					//fprintf(stderr, "degenerate normal found\n");
					degenerates++;
					degenerateTuple = TRUE;
				}
				else 
				{ 
					normMap[vIndex] = nIndex;
					nIndex = vIndex;
				}
			}//if(-1 != nIndex)
			if (-1 != tvIndex)
			{  //tex coord specified for this vertex
				tVerts.getTVertex(nextTVert, tvIndex);
				dTVerts.getTVertex(tmpTVert, vIndex);
				if ( (tmpTVert != maxTVert) && (tmpTVert != nextTVert) )
				{
					//fprintf(stderr, "degenerate tvert found\n");
					degenerates++;
					degenerateTuple = TRUE;
				}
				else 
				{
					tVertMap[vIndex] = tvIndex;
					tvIndex = vIndex;
				}
			}//if (-1 != tvIndex)       
            
			//insert into new arrays at the VA index posn
			//iff this position is already filled, see if
			//(i)  same value - no problem
			//(ii) different values, so must add alternate VA
			//     index. See if we already have a maching index 
			//     for this vert/normal/colvert/tvert tuple
			if (degenerateTuple)
			{  //check to see if this tuple already exists
				int temp = getIndex(nextVert, refMesh->m_verts,
									nextNorm, dNorms,
									nextTVert, dTVerts,
									nextColVert, refMesh->m_colVerts);
				if (-1 != temp)
				{  //use this index - it's already present
					vertMap[temp] = vIndex;
					vIndex = temp;
					if (-1 != tvIndex) 
					{
						tVertMap[vIndex] = tvIndex;
						tvIndex = vIndex;
					}
					if (-1 != cvIndex) 
					{
						colVertMap[vIndex] = cvIndex;
						cvIndex = vIndex;
					}
					if (-1 != nIndex)  
					{
						normMap[vIndex] = nIndex;
						nIndex  = vIndex; 
					}
				}//if (-1 != temp)
				else
				{  //need to add a new tuple for this degenerate vertex
					if (refMesh->getVertexList())
					{
						vertMap.add(vIndex);
						vIndex = refMesh->m_verts.add(nextVert);
					}
					if (refMesh->getNumColVerts())
					{
						temp = cvIndex;
						cvIndex = dColVerts.add(nextColVert);
						if (cvIndex != vIndex) 
						{
							//fprintf(stderr, "invalid col vert index\n");
						}
						else
						{
							colVertMap.add(temp);
						}                  
					}//if (m_curMesh->getColVertexList())
					
					if (tVerts.getNumTVerts()) 
					{
						temp = tvIndex;
						tvIndex = dTVerts.add(nextTVert);
						if (tvIndex != vIndex)
						{
							//fprintf(stderr, "invalid tex vert index\n");
						}
						else
						{
							tVertMap.add(temp);
						}
					}//if (m_tVerts.getNumTVerts()) 
					if (norms.getNum()) 
					{
						temp = nIndex;
						nIndex = dNorms.add(nextNorm);
						if (nIndex != vIndex)
						{
							//fprintf(stderr, "invalid normal index\n");
						}
						else
						{
							normMap.add(temp);
						}
					}//if (m_norms.getNum())                
				}//else
				curFace->v[j] = vIndex;
			}//if (degenerateTuple)
			else
			{
				vertMap[vIndex] = vIndex;
			}
			if (-1 != tvIndex)
			{  //may be overwriting this value with same - so what.
				dTVerts.setTVertex(nextTVert, tvIndex);
				curFace->Tv[j]    = tvIndex; 
			}
			if (-1 != cvIndex)
			{  //may be overwriting this value with same - so what.
				dColVerts[cvIndex] = nextColVert;
				curFace->cV[j]    = cvIndex; 
			}
			if (-1 != nIndex)
			{  //may be overwriting this value with same - so what.
				dNorms[nIndex]     = nextNorm;
				curFace->n[j]     = nIndex; 
			}        
			indeces.add((unsigned short)vIndex);
		}//for (int j=0; j<3; j++) 
	}//for (int i=0; i< getNumFaces(); i++)
	
	AWTRACE3("Processed: %s with faces %i and %i degenerate vertices\n", 
			 m_name.c_str(), (int)indeces.getNum()/3, (int)degenerates);
	
	indeces.pack();
	dNorms.pack();
	dTVerts.pack();
	dColVerts.pack();
	
	if (dNorms.getNum())
	{  //transfer over to the AWMesh object
		dNorms.setOwnsData(FALSE);
		refMesh->m_norms.shallowCopyOf(dNorms);
		refMesh->m_norms.setOwnsData(TRUE);
	}//if (m_norms.getNum())
	if (dTVerts.getNumTVerts())
	{
		AWTVertexList* tv  = new AWTVertexList(0, dTVerts.getDimension()); 
		//deep copy ie all data
		tv->shallowCopyOf(dTVerts);
		dTVerts.setOwnsData(FALSE);
		tv->setOwnsData(TRUE);
		refMesh->m_tVerts.setSize(0);
		refMesh->m_tVerts.setOwnsList(TRUE);
		refMesh->m_tVerts.setOwnsObjects(TRUE);
		refMesh->m_tVerts.add(tv); 
		refMesh->m_tVerts.pack();
	}//if (dTVerts.getNumTVerts())
	if (dColVerts.getNum())
	{
		dColVerts.setOwnsData(FALSE);
		refMesh->m_colVerts.shallowCopyOf(dColVerts);
		refMesh->m_colVerts.setOwnsData(TRUE);     
	}//if (colVerts)
	refMesh->m_indeces.shallowCopyOf(indeces);
	refMesh->m_indeces.setOwnsData(TRUE);
	indeces.setOwnsData(FALSE);
}//void AWObject::processReferenceMesh(AWMesh* refMesh, AWList<int>& vertMap)


AWBoolean         
AWObject::buildIndexArray(AWList<AWPoint>& norms, AWList<AWTVert>& tVs)
{
	AWBoolean         retVal   = FALSE;  
	AWTVertexList     tVerts;
	tVerts.shallowCopyOf(tVs);
	
	AWMesh*           refMesh  = NULL;
	AWAnimatedMesh*   animMesh = NULL;
	AWList<int>       vertMap;
	int               u        = 0;
	AWList<AWPoint>   verts;
	AWMesh*           nextMesh = NULL;
	int               anim     = 0;
	
	int numAnimMeshSequences   = isMeshAnimated();
	if (m_curMesh && (0 == numAnimMeshSequences))
	{  //mesh is not animated - but we have a frame at m_curMesh
		refMesh = m_curMesh;
		processReferenceMesh(refMesh, vertMap, norms, tVerts);
	}
	vertMap.setSize(0);   
	for (anim=0; anim < numAnimMeshSequences; anim++)
	{  //PROCESS EACH MESH SEQUENCE IN TURN           
		//u                    = 0;
		nextMesh             = NULL;      
		//grab pointer to mesh sequence
		animMesh             = (AWAnimatedMesh*)m_animMeshes[anim];
		if (0 == anim) 
		{  //grab reference (initial) mesh from first mesh sequence
			refMesh  = (AWMesh*)animMesh->getMesh(0);
			processReferenceMesh(refMesh, vertMap, norms, tVerts);
		}
		//now go through and reorder any other frames in this animated mesh
		//(first one's already been taken care of for animation #0)  
		for (int frameNum=(0 == anim)?1:0; frameNum<animMesh->getNumFrames(); frameNum++)
		{
			nextMesh = animMesh->getMesh(frameNum);
			//build new vertex array for each mesh using the map array
			//
			//old animated mesh frames only hold VERTS and COLVERTS
			//vertices owned by the mesh.
			//colVerts owned by the mesh but INVARIANT
			//hence - can remove from mesh and copy by reference
			//indeces don't vary between frames - copy over 
			//by reference - NOT owned, no data duplication
			//
			//GENERATE NEW VERTEX LIST
			verts.setSize(0);
			for (u=0; u<vertMap.getNum(); u++)
			{
				verts.add( nextMesh->m_verts[vertMap[u]] );
			}//for (u=0; u<vertMap.getNum(); u++)       
			//ASSIGN TO MESH - mesh will OWN
			verts.pack();
			nextMesh->m_verts = verts;
			//ASSIGN (PREVIOUSLY CALCULATED) COLVERTS - will NOT OWN
			nextMesh->m_colVerts.shallowCopyOf(refMesh->m_colVerts);
			//ASSIGN (PREVIOUSLY CALCULATED) INDECES  - will NOT OWN
			nextMesh->m_indeces.shallowCopyOf(refMesh->m_indeces);
			//ASSIGN (PREVIOUSLY CALCULATED) NORMS    - will NOT OWN
			nextMesh->m_norms.shallowCopyOf(refMesh->m_norms);
			//ASSIGN (PREVIOUSLY CALCULATED) TVERTS   - will NOT OWN
			//just copy over contained pointers to lists AWList<AWTVert>*
			//as these lists are shared
			nextMesh->m_tVerts.shallowCopyOf(refMesh->m_tVerts);
		}//for (int frameNum=1; frameNum<animMesh->getNumFrames(); frameNum++)     
	}//for (int anim =0; anim < numAnimMeshSequences; anim++)
	//clean up face data, which is no longer required
	if (m_ownsVertexData)
	{
		m_faceList.destroy();
	}
	return retVal;
}//AWBoolean AWObject::buildIndexArray(AWList<AWPoint>& norms, AWTVertexList& tVerts)



AWBoolean         
AWObject::buildIndexArray()
{
	AWList<AWPoint> nms;
	AWList<AWTVert> tVs;
	return buildIndexArray(nms, tVs);
}//AWBoolean AWObject::buildIndexArray()



AWRenderPass*              
AWObject::setPass(int pass)
{
	//need to select the requested AWRenderPass as current
	if (pass >= m_passes.getNum())
	{
		pass = m_passes.getNum()-1;
	}
	if (pass >=0) 
	{
		m_curRenderPass = m_passes[pass];
	}
	return m_curRenderPass;
}//AWRenderPass*  AWObject::setPass(int pass)


void	 
AWObject::setAnimation(int animNo)
{
	//must also create a suitable buffer for this animation  
	//should resume checking /*(m_animNum != animNo) && */ 
	if ((animNo >= 0) && (animNo < (int)m_animMeshes.getNum()))
	{
		m_animNum = animNo;
		m_curAnimMesh = (AWAnimatedMesh*)m_animMeshes[animNo];
		//we have an animated mesh. Object may be using either
		//BUFFERED or NON-BUFFERED animation : m_usingFrameBuffer
		//
		//for a single frame 'animation' turn OFF BUFFERING
		if ( m_usingFrameBuffer && m_curMesh && 
			(1==m_animMeshes.getNum()) && m_curAnimMesh && 
			(1==m_curAnimMesh->getNumFrames()) )
		{  //initially BUFFERED (INTERPOLATED) MESH ANIMATION
			//always own buffer, if any - so can safely delete
			delete m_curMesh;
			//TURN OFF BUFFERING
			m_usingFrameBuffer = FALSE;
			//GRAB INITIAL FRAME
			m_curMesh = m_curAnimMesh->getMesh(0);
		}//if (m_usingFrameBuffer && m_curAnimMesh)
		else if (m_usingFrameBuffer && m_curAnimMesh)
		{  //BUFFERED (INTERPOLATED) MESH ANIMATION
			//always own buffer, if any - so can safely delete
			delete m_curMesh;
			//assign a new buffer
			m_curMesh = m_curAnimMesh->newMeshBuffer();
		}//if (m_usingFrameBuffer && m_curAnimMesh)
	}//if (animNo < m_animMeshes.entries())
}//void AWObject::setAnimation(int setAnimation)



int	 
AWObject::calcViewDependentData(double t, const AWSolidCone& viewCone)
{
	calcViewDependentMesh(t, viewCone);
	//call base class method - takes care of the children
	AWNode::calcViewDependentData(t, viewCone);
	return 1;
}//int calcViewDependentData(float t, const AWSolidCone& viewCone)



int
AWObject::calcVisibility(double t, const AWSolidCone& viewCone)
{
	//call base class method - takes care of the children
	AWNode::calcVisibility(t, viewCone);
	m_isVisibleThisFrame = 0;
	//if (m_name != "Sphere04")
	if ((1.0f == m_visible.getVal()) && viewCone.intersectSphere(getWSCenter(), getRadius()))
	{	//also include zclipp information, such that if the object is in the cone,
		//the distance from the cone vertex (camera)
		//must be less than the z clipping distance
		AWPoint diff(m_wsCenter - viewCone.m_V);
		float separation = diff.magnitude(); 
		if ( separation <= (viewCone.m_farClip + getRadius()))
		{
			switch (m_distanceTest)
			{
				case 1:
					m_isVisibleThisFrame = (float)fabs(diff.x);
					break;
				case 2:
					m_isVisibleThisFrame = (float)fabs(diff.y);
					break;
					
				case 3:
					m_isVisibleThisFrame = (float)fabs(diff.z);
					break;
				case 0:
				default:
					m_isVisibleThisFrame = separation;
					break;
			}//switch (m_distanceTest)
			if (0 == m_isVisibleThisFrame)
			{
				m_isVisibleThisFrame = 1.0f;
			}
		}//if ( separation <= (viewCone.m_farClip + getRadius()))
	}//	if ((1.0f == m_visible.getVal()) && viewCone.intersectSphere(getWSCenter(), getRadius()))
	return 1;
}//int AWObject::calcVisibility(float t, const AWSolidCone& viewCone)



AWBoolean
AWObject::collideWith(AWObject* target, double time)
{
	if (isCollisionSource() && (this != target) && target->isCollide())
	{
		//self-self collisions excluded
		//initial collision test based on WS object-bounding spheres
		AWPoint center(getWSCenter());
		AWPoint targetWSCenter(target->getWSCenter());
		AWPoint distance = center - targetWSCenter;
		float   radius = getRadius();
		
		if (distance.magnitude() <= (radius + target->getRadius()))
		{	//WS bounding-sphere test passed, move to next
			//granularity level of testing
			//
			//OBJECT  -  OBJECT COLLISIONS
			if (target->collide(center, radius))
			{
				//collision detected - take such action as is required
				//before returning
                target->m_animStart=time;
				return TRUE;
			}
		}//if (distance.magnitude() <= (radius + curObject->getRadius()))
	}//if ((this != target) && target->isCollide())
	return FALSE;
}//AWBoolean AWObject::collideWith(AWObject* target, float time)



//TEXTURE METHODS
//return by index into object's materialList collection

AWTexture*	 
AWObject::getTexture(int index)	
{
	AWTexture*     retVal = NULL;
	if (m_material && m_material->getNumTexEffects())
	{
		AWTextureEffect* tmp = m_material->getTextureEffect(0);
		if (tmp)
		{
			retVal = tmp->m_tex;
		}     
	}//if (m_material && m_material->getNumTexEffects())
	return retVal;
}//AWTexture* AWObject::getTexture(int texUnit)


AWBoolean         
AWObject::isTwoSided()
{
   return  m_material ? m_material->getTwoSided() : FALSE;
}//AWBoolean AWObject::isTwoSided()


AWBoolean         
AWObject::wireframe()
{
   return  m_material ? m_material->getWire() : FALSE;
}//AWBoolean AWObject::wireframe()


void              
AWObject::setTwoSided(AWBoolean v)
{
   if (m_material)
   {
      m_material->setTwoSided(v);
   }
}//void AWObject::setTwoSided(AWBoolean v)



AWMaterial*
AWObject::getMaterial()
{
	return m_material;
}


void
AWObject::addMaterial(AWMaterial* mat)
{
	m_material = mat;
}


void 
AWObject::offset(const AWPoint& offset)
{
	AWNode::offset(offset);
	for (int vert = 0; vert < getNumVerts(); vert++)
	{
		getVertex(vert) += offset;
	}//for (int vert=0; vert < object->m_vertexList.entries(); vert++)
}//void AWObject::offset(float xTrans, float yTrans, float zTrans)




void
AWObject::scaleCurrentVertexList(const AWPoint& scale)
{	//scale vertices
	for (int vert = 0; vert < getNumVerts(); vert++)
	{
		AWPoint& temp = getVertex(vert);
		temp *= scale;
	}//for (int vert=0; vert < getNumVerts(); vert++)
}//void AWObject::scaleCurrentVertexList(const AWPoint& scale)



void 
AWObject::scale(const AWPoint& scale)
{
	AWNode::scale(scale);
	//scale vertices in all Frames in every FrameSet 
	if (m_animMeshes.getNum())
	{
		for (int listToScale = 0; listToScale < m_animMeshes.getNum(); listToScale++)
		{
			((AWAnimatedMesh*)m_animMeshes[listToScale])->scale(scale);
		}
	}//if (m_animMeshes.entries())
	else if (m_curMesh)
	{
		m_curMesh->scale(scale);
	}	
}//void AWObject::scale(float xScale, float yScale = -1, float zScale = -1)


//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWObject
//
//////////////////////////////////////////////////////////

void	
AWObject::checkCurMesh()
{
   //return;
   AWAnimatedMesh* animMesh = NULL;
   AWMesh*         cMesh    = NULL;
   for (int k=0; k< m_animMeshes.getNum(); k++)
   {
      animMesh = (AWAnimatedMesh*)m_animMeshes[k];
      for (int jj =0; jj < animMesh->getNumFrames(); jj++)
      {
         cMesh = animMesh->getMesh(jj);
         if (m_curMesh == cMesh)
         {
            fprintf(stderr, "curMesh in AnimMeshes - buffer %d!\n", (int)m_usingFrameBuffer);
         }
      }//for (int jj =0; jj < animMesh->getNumFrames(); jj++)
   }//for (int k=0; k< m_animMeshes.entries(); k++)
}//void	AWObject::checkCurMesh()


void
AWObject::convertTextureList(AWOrdered& textureList, unsigned int version)
{  //designed to convert between texture storage for specific 
	//versions of AWObject. Converts all previously stored textures
	//(assumed to be AWPersistable BUT NOT AWTextureEffect) into 
	//contained pointers in an AWTextureEffect using AW_DIFFUSE
	AWTextureEffect* texEff = NULL;
	AWPersistable*   next   = NULL;
	for (int i=0; i<(int)textureList.getNum(); i++)
	{
		next = textureList[i];
		if (next)
		{
			if (CLSID_AWTEXTUREEFFECT != next->isA())
			{
				texEff = new AWTextureEffect((AWTexture*)next, 0, AWTextureEffect::AW_DIFFUSE);
			}//if (CLSID_AWTEXTUREEFFECT != next->isA()) )
			else
			{
				texEff = (AWTextureEffect*)next;
			}
			if (m_material)
			{
				m_material->addTextureEffect(texEff);
			}//if (m_material)
		}//if (next)
	}//for (int i=0; i<(int)m_textureList.getNum(); i++)   
	if ((version == VER_AWOBJECT_1)&&(0==textureList.getNum()))
	{  //YUK - oh well, need to ship...
		AWTRACE("setting visibility to ZERO\n");
		m_visible.setVal(0.0f);
	}//if ((version == VER_AWOBJECT_1)&&(0==textureList.getNum()))
	//should already be the case, but may as well ensure it here...
	textureList.setOwnsObjects(FALSE); 
	textureList.destroy();
}//void AWObject::convertTextureList(AWOrdered& textureList, unsigned int version)


void                 
AWObject::convertBlendType(unsigned int v, int& blendType)
{
   if (VER_AWOBJECT_1 == v)
   {//need to transform blend types
      switch (blendType)
      {
      case 8: //AW_BLENDTYPE_ALPHATEST:
         blendType = AWTexture::ALPHA_TEST;
         break;
      case 4: //AW_BLENDTYPE_ALPHA:
         blendType = AWTexture::ALPHA_BLEND;
         break;
      case 0 ://AW_BLENDTYPE_NONE:
      default:
         blendType = AWTexture::B_NONE;       
      }//switch (blendType)
   }//if (VER_AWOBJECT_1 == v)
}//void AWObject::convertBlendType(unsigned int v, int& blendType)


void	
AWObject::restoreGuts(AWFile& file)
{
	unsigned int         version;
	int                  numVerts;
	AWList<AWTVert>      tVerts;
	AWList<AWPoint>      norms;
	//does NOT own its contained objects
	AWOrdered            textureList(1, AW_PTRLIST_OWNS_LIST); 
	
	clearAndDestroy();
	restoreVersion(file, &version);   // Get the saved version #
	if (version <= VER_AWOBJECT_6)
	{
		AWNode::restoreGuts(file);
		
		file.Read(numVerts);
		m_animMeshes.read(file, version < VER_AWOBJECT_4);
		file >> m_curAnimMesh;
		file >> m_curMesh;
		
		if (VER_AWOBJECT_1 == version)
		{
			norms.read(file);
			tVerts.read(file);
			if (m_curMesh)
			{
				m_curMesh->m_verts.setOwnsData(m_ownsVertexData);
				m_curMesh->m_colVerts.setOwnsData(m_ownsVertexData);
			}//if (m_curMesh)
		}//if (VER_AWOBJECT_1 == version)
		
		if (version < VER_AWOBJECT_4)
		{  //SPECIAL CASE- we know that (for files < ver4) this face
			//collection may: (i) not be present at all
			//(ii) may be present, but will not be shared with any other
			//object - ie. will NOT have been read before
			m_faceList.readUniquePtr(file);
		}
		else
		{  //now stored like m_textureList etc. - no longer via Pointer
			file >> m_faceList;
		}
		if (version < VER_AWOBJECT_6)
		{
			textureList.read(file, version < VER_AWOBJECT_4);
		}//if (version < VER_AWOBJECT_6)
		file >> m_material;
		if (m_material==NULL)
		{  //new schema ALWAYS has material  
			if (textureList.getNum())
			{  //this object has textures on it, so we build a new material
				//that owns these textures and is referenced by the object
				m_material = new AWMaterial;
			}
			else
			{  //the object doesn't have any textures, and therefore we can use
				//the single 'default' material
				m_material =  new AWMaterial;//GLRenderer::getRenderer().getScene().getDefaultMaterial();
			}
			//in either case, make sure engine knows to use the  
			//OpenGl default values for this texture
			m_material->setUseDefaults(TRUE);
		}//if (m_material==NULL)
		file.Read(m_listID);
		if (version < VER_AWOBJECT_6)
		{
			file >> m_material->m_twoSided;
		}//if (version < VER_AWOBJECT_6)
		file >> m_collide;
		file >> m_useDynamicLights;
		if (version < VER_AWOBJECT_6)
		{
			file >> m_material->m_blendType;
			convertBlendType(version, m_material->m_blendType);
		}//if (version < VER_AWOBJECT_6)
		file >> m_ownsVertexData;
		file >> m_usingFrameBuffer;
		file >> m_visible;
		file >> m_distanceTest;
		if (version >= VER_AWOBJECT_5)
		{
			file >> m_separateSpecular;
		}//if (version >= VER_AWOBJECT_5)
		
		if (version <= VER_AWOBJECT_2)
		{  //MOVE TO NEW DATA FORMAT
			//for VER_AWOBJECT_2 note normal and tVert 
			//data stored WITH THE MESH hence passed 
			//norms & tVerts arrays are empty
			buildIndexArray(norms, tVerts);         
		}//if (version <= VER_AWOBJECT_2) 
		//move textures over to material
		convertTextureList(textureList, version);
		setPass(0);
		setAnimation(0);
		calcMesh(0);		//takes care of initialising m_vertexList  
		//AWTRACE2("   %s has %d texture effects\n", m_name.c_str(), (int)textureList.getNum());
	}//if (version <= VER_AWOBJECT_4)
}//void	AWObject::restoreGuts(AWFile& file)




void        
AWObject::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWNode::saveGuts(file);
	file.Write((int)getNumVerts());
	
	file << m_animMeshes;
	file << m_curAnimMesh;
	file << m_curMesh;
	
	file << m_faceList;
	file << m_material;
	file.Write(m_listID);
	file << m_collide;
	file << m_useDynamicLights;
	file << m_ownsVertexData;
	file << m_usingFrameBuffer;
	file << m_visible;
	file << m_distanceTest;
	file << m_separateSpecular;
}//void AWObject::saveGuts(AWFile& file) const




AWspace     
AWObject::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		    AWNode::binaryStoreSize() +
			sizeof (int) + //used to be m_numVertices
			sizeof (m_collide) +
			AWPERSISTABLE_POINTER_SIZE(m_curAnimMesh)+
			AWPERSISTABLE_POINTER_SIZE(m_curMesh)+
			AWPERSISTABLE_POINTER_SIZE(m_material);

	retVal += m_animMeshes.getStoreSize();	

	retVal += 
         m_faceList.getStoreSize() +
			m_visible.recursiveStoreSize() +
			sizeof (int) +
			sizeof (m_useDynamicLights) +
			sizeof (m_ownsVertexData) +
			sizeof (m_usingFrameBuffer) +
			sizeof (m_distanceTest) +
         sizeof (m_separateSpecular);
	return retVal;
}//AWspace  AWObject::binaryStoreSize() const