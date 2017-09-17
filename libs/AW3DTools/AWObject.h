#ifndef __OPENGL_3DATA_H
#define __OPENGL_3DATA_H


#include "AWToolsCommon.h"
#include "AWNode.h"
#include "AWTVert.h"
#include "AWMesh.h"
#include "AWFace.h"
#include "AWAnimatedFloat.h"
#include "AWRenderPass.h"
#include "AWOrdered.h"


class AWExport AWFlightController;		//forward definition
class AWExport AWScene;					//forward definition
class AWExport AWTexture;			//forward definition
class AWExport AWMaterial;


#define AW_NO_COLLIDE			   0x00000000
#define AW_COLLISION_SOURCE		0x00000001
#define AW_COLLISION_TARGET		0x00000010


class AWExport AWObject	: public AWNode
{	
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWObject);

   AWObject(const AWCString& name = "", AWNode* parent=NULL, 
             float life=AW_INFINITE_LIFE);
	AWObject(const AWObject& other);
	virtual				~AWObject();

	AWObject&			operator=(const AWObject& other);
	int					operator==(const AWObject&);
	AWObject&			clone(const AWObject& other);
	virtual AWNode*	clone() const; 
	virtual void		compact();

	void				   destroyMyData();
	virtual void		clearAndDestroy();
	virtual void		clear();

	//AWNode virtuals
	virtual void		offset(const AWPoint& offset);
	virtual void		scale(const AWPoint& scale);

	void				   scaleCurrentVertexList(const AWPoint& scale);

	AWBoolean			isMeshAnimated()	
							{return m_animMeshes.getNum();}
	AWBoolean			initMeshBuffer();//for use by EXPORTER only
	void				   calcMesh(float time);
   void              calcViewDependentMesh(float time, const AWSolidCone& viewCone);
	void				   setAnimation(int animNo);

	//update the object state with the values for the specified time
	virtual int			calcTransform(float t, const AWMatrix4& parentTM);
	//update the object state with the view-dependent values at t
	virtual int			calcViewDependentData(float t, const AWSolidCone& viewCone);
   
   //update the view-dependent visibility value at t
	virtual int		   calcVisibility(float t, const AWSolidCone& viewCone); 


	AWPoint				getWSCenter()		{return m_wsCenter;}
	AWPoint&			   getCenter()       {return m_center;}
	float				   getRadius()       {return m_radius;}

//these versions cast to AWPoint*
	AWPoint&			   getVertex(int i)     {return *(AWPoint*)&m_vertexList[3*i];}
	AWPoint&			   getNormal(int i)     {return *(AWPoint*)&m_normalList[3*i];}
	//AWTVert&			   getTVertex(int i, int set=0)  {return *(AWTVert*)&m_tVertexList[2*i];}
   AWColVertex&      getColVertex(int i, int set=0){return *(AWColVertex*)&m_vertColList[3*i];}

//and these do not, might possibly be quicker
	const float*		getVertex3fv(int i)              {return &m_vertexList[3*i];}
	const float*		getNormal3fv(int i)              {return &m_normalList[3*i];}
  // const float*		getTexCoordfv(int i, int tUnit=0); 
	const unsigned char* getColour3ubv(int i)          {return &m_vertColList[3*i];}

   int               getIndex(int i)                  {return m_curMesh?m_curMesh->getIndex(i):0;}
   int               getNumIndeces()const             {return m_curMesh?m_curMesh->m_indeces.getNum():0;}

   int               getNumVerts()const               {return m_curMesh?m_curMesh->m_verts.getNum():0;}
   AWBoolean         buildIndexArray();
   int               buildFaceListForMatID(AWObject& retVal, int matID);
   AWBoolean         buildIndexArray(AWList<AWPoint>& norms,
                                     AWList<AWTVert>& tVerts);
   void              processReferenceMesh(AWMesh* m, AWList<int>& vertMap,
                                          AWList<AWPoint>& norms,
                                          AWTVertexList& tVerts);
   //int               mapFaceListForMatID(int matID);
   //void              packVerts(AWMesh* m, AWList<int>& vertMap);
   //AWObject*         createSubObjectForMapID(int matID);
   int               getIndex(const AWPoint& vert, const AWList<AWPoint>& vl,
                              const AWPoint& norm, const AWList<AWPoint>& nl,
                              const AWPoint& tVert, const AWTVertexList& tvl,
                              const AWColVertex& colVert, const AWList<AWColVertex>& cvl);
   unsigned int*     getIndexArray()                  {return m_curMesh?m_curMesh->getIndexList():NULL;}
  
   float*            getTVertexList(int s=0);
   int               getTVertexListDim(int s=0);

	//calculate bounding sphere data for all faces on this object
   virtual void      init(float t, GLRenderer& renderer);
   void	            checkCurMesh();
	

	AWBoolean			collide(const AWPoint& center, float radius);
	virtual AWBoolean	collideWith(AWObject* target, float time=0.0f);

//TEXTURES & MATERIAL
public:
	AWBoolean			getIsVisible()			  {return (AWBoolean)m_isVisibleThisFrame;}
	void				   setIsVisible(AWBoolean v, AWBoolean andKids=FALSE);
	void				   addVisibility(AWBoolean v, float t) {m_visible.addKey((float)v,t);}

   int					getBlendType();
   AWBoolean         isTwoSided();
   AWBoolean         wireframe();
   void              setTwoSided(AWBoolean v);

   AWBoolean         useSeparateSpecular()            {return m_separateSpecular;}
   void              setSeparateSpecular(AWBoolean b) {m_separateSpecular=b;}

   AWRenderPass*     setPass(int pass);
   AWRenderPass*     getCurPass()         {return m_curRenderPass;}
   //float             getOpacity();
   //void              updateTexCoordInfo();
   int               getNumPasses()       {return m_passes.getNum();}
   int               addPass(AWRenderPass* pass);
   AWRenderPass*     getPass(int i)       {return m_passes[i];}

   //used only for sorting objects by texture
	AWTexture*	      getTexture(int index=0);	

	AWMaterial*	      getMaterial();
	void				   addMaterial(AWMaterial* mat);

	int					getListID()const		{return m_listID;}
	void				   setListID(int newID);	
	AWBoolean			getDynamicLighting()	{return m_useDynamicLights;}
	void				   setDynamicLighting(AWBoolean val)
												{m_useDynamicLights=val;}

//COLLISION DETECTION SUPPORT
	void				   setCollide(AWBoolean val) 
							   {if (val) 
							      m_collide |= (AW_COLLISION_SOURCE + AW_COLLISION_TARGET); 
							    else m_collide=0;}
	void				   setIsCollisionTarget(AWBoolean val) 
	   {if (val) m_collide |= AW_COLLISION_TARGET; else m_collide &= !AW_COLLISION_TARGET;}
	void				   setIsCollisionSource(AWBoolean val)
	   {if (val) m_collide |= AW_COLLISION_SOURCE; else m_collide &= !AW_COLLISION_SOURCE;}
	AWBoolean			isCollisionTarget() {return m_collide & AW_COLLISION_TARGET;}
	AWBoolean			isCollisionSource() {return m_collide & AW_COLLISION_SOURCE;}
	AWBoolean			isCollide()			{return m_collide;}
													

protected:
//member data associated with TEXTURES / MATERIALS
	AWMaterial* 	      m_material;
   //following collection used to hold AWTextures but now holds
   //AWTextureEffects. Old models will be converted to use 
   //DIFFUSE type effect, thus retaining existing behaviour
	//AWOrdered			   m_textureList;	
   void                 convertTextureList(AWOrdered& textureList, unsigned int version);
   void                 convertBlendType(unsigned int v, int& blendType);
   //new list holding AWRenderPass objects after preparation for
   //rendering on the client (graphics display) side.
   AWPtrList<AWRenderPass*> m_passes;
	//AWBoolean			   m_twoSided;
	int					   m_listID;		//GL state member data
	//int					   m_blendType;	//alpha test, alpha_blend, none (default)

public:
	float				      m_isVisibleThisFrame;//NOT persisted, cloned or copied - run-time only
	AWAnimatedFloat	   m_visible;		//adds (discrete) visibility anim
	int					   m_distanceTest; //0 => center  1=>x 2=>y 3=>z
	//the pointers below are kept for speed reasons
	//and will be initialised on calcMesh call
	float*				   m_vertexList;     //from m_curMesh
	unsigned char*		   m_vertColList;    //from m_curMesh
	float*				   m_normalList;     //from m_curMesh
   //initialised via setPass(int n)
   AWRenderPass*        m_curRenderPass;
    
	//list of AWAnimatedMeshs for animated vertex data
	int					   m_animNum;			//not persisted
	AWOrdered			   m_animMeshes;
	AWAnimatedMesh*		m_curAnimMesh;
	AWMesh*				   m_curMesh;		//the vertex buffer if interpolating,
										//otherwise a pointer to the cur frame
	//note that face list doesn't have to be used - but can be
	AWOrdered			   m_faceList;
   int					   getNumTris()const {return getNumIndeces()/3;}
	int					   getNumFaces()const	
                           {return m_faceList.getNum();}

	AWFace*				   getFace(int i)		   
                           {return (AWFace*)m_faceList[i];}

	const AWFace*		   getFace(int i)const	
                           {return (AWFace*)m_faceList[i];}

	void				      addFace(AWFace* face);

	AWBoolean			   m_ownsVertexData;
	AWBoolean			   m_usingFrameBuffer; //is the mesh interpolated / deformed?
	AWPoint				   m_center;	//object-local space
	float				      m_radius;
	//world space center after calcTransform - not persisted
	AWPoint				   m_wsCenter; 
	

	DWORD				      m_collide;
	AWBoolean			   m_useDynamicLights;
   AWBoolean			   m_separateSpecular;

//AWPersistable virtuals
	//int				      compareTo(const AWPersistable*) const{return -1;}
   //virtual void         reAssignTexture(AWPersistable* src, AWPersistable* dest);
	virtual void		   restoreGuts(AWFile& file);
   virtual void		   saveGuts(AWFile& file) const;

	virtual AWspace	   binaryStoreSize() const;
};//class AWObject	: public AWPersistable



//inline int               
//AWObject::getNumTextures()
//{  //how many textures are used by the current render pass
//   int retVal = 0;
///   if (m_curRenderPass)
//   {
//      retVal = m_curRenderPass->getNumTexUnits();  
//   }//if (m_curRenderPass)
//   return retVal;
//}//int AWObject::getNumTextures()


//inline int               
//AWObject::getChannelID(int texUnit)
//{
//   int retVal = -1;
//   AWTextureUnit*  tmp    = NULL;
//   if (m_curRenderPass && (tmp = m_curRenderPass->getTexUnit(texUnit)))
//   {
//      retVal = tmp->m_chanID;
//   }//if (m_curRenderPass && (tmp = m_curRenderPass->getTexEffect(texUnit)))
//   return retVal;
//}//int AWObject::getChannelID(int texUnit=0)


inline float*            
AWObject::getTVertexList(int s) 
{
   float* retVal = NULL;
   if (m_curRenderPass && m_curMesh)
   {
      AWTextureUnit* texUnit = m_curRenderPass->getTexUnit(s);
      if (texUnit)
      {
         int chanID = texUnit->m_chanID;
         if (-1 != chanID)
         {
            retVal = m_curMesh->getTVertexList(chanID);
         }
      }//if (texUnit)
   }//if (m_curRenderPass && m_curMesh)
   return retVal;
}//inline float* AWObject::getTVertexList(int s)



inline int            
AWObject::getTVertexListDim(int s) 
{
   int retVal = 0;
   if (m_curRenderPass && m_curMesh)
   {
      AWTextureUnit* texUnit = m_curRenderPass->getTexUnit(s);
      if (texUnit)
      {
         int chanID = texUnit->m_chanID;
         if (-1 != chanID)
         {
            retVal = m_curMesh->getTVertexListDim(chanID);
         }//if (-1 != chanID)
      }//if (texUnit)
   }//if (m_curRenderPass && m_curMesh)
   return retVal;
}//inline float* AWObject::getTVertexListDim(int s)



//inline float               
//AWObject::getOpacity()
//{  //how many textures are used by the current render pass
//   float retVal = 1.0f;
//   if (m_curRenderPass)
//   {
//      retVal = m_curRenderPass->getOpacity();  
//   }//if (m_curRenderPass)
//   return retVal;
//}//float AWObject::getOpacity()


inline int               
AWObject::addPass(AWRenderPass* pass)   
{
   if (0==getNumPasses())
   {
      m_passes.setOwnsObjects(TRUE);
   }
   return m_passes.add(pass);
}//inline int AWObject::addPass(AWRenderPass* pass)



inline void	
AWObject::compact()
{
	m_faceList.pack();
	m_animMeshes.pack();
	//compact each animated mesh
	for (int k=0; k<m_animMeshes.getNum(); k++)
	{
		((AWAnimatedMesh*)m_animMeshes[k])->compact();
	}
	//m_textureList.pack();
   m_passes.pack();
	m_visible.compact();
}//inline void AWObject::compact()



inline void
AWObject::setIsVisible(AWBoolean v, AWBoolean andKids)
{
	m_visible.setVal((float)v);
	if (andKids)
	{
		for (int i=0; i< (int)m_children.getNum(); i++)
		{
			if (getChild(i)->isA() == CLSID_AWOBJECT)
			{//YUK
				((AWObject*)getChild(i))->setIsVisible(v, andKids);
			}
		}
	}
}//inline void AWObject::setIsVisible(AWBoolean v, AWBoolean andKids)



inline int			
AWObject::calcTransform(float t, const AWMatrix4& parentTM)
{
	int retVal = AWNode::calcTransform(t, parentTM);
	calcMesh(t);
	m_visible.calc(t);
	m_wsCenter = getWSTM()*m_center;
	return retVal;
}//inline int AWObject::calcTransform(float t, const AWMatrix4& parentTM)


#endif //sentinel __OPENGL_3DATA_H