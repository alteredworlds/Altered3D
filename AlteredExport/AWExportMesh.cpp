
#include "AlteredExport.h"
#include <AW3DTools/AWBumpMesh.h>
#include <AW3DTools/AWSea.h>



MeshExtractInfo::MeshExtractInfo()
{
   withFaces      = TRUE;
   withNormals    = TRUE;
   withMaterials  = TRUE;
   withTexCoords  = TRUE;
   withColorVerts = TRUE;
   meshType       = 0;
   matID          = -1;
   refMesh        = NULL;
}//MeshExtractInfo::MeshExtractInfo()



MeshExtractInfo::~MeshExtractInfo()
{
   matIDList.Shrink();
}//MeshExtractInfo::~MeshExtractInfo()



AWNode* 
TnTExprt::ExportGeomObject(INode* node, AWNode* parent)
{  //for game engine, are only interested in objects that can be converted
	//to a TriObject (and thus expressed as triangles)
	AWObject*   retVal      = NULL;
   AWObject*   curObj      = NULL;
   AWBoolean   val1, val2, forceMaterial;
   AWBoolean   hasBumpMap  = FALSE;
   int         numMat      = 0;

   ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj || !os.obj->CanConvertToType(triObjectClassID))
   {
		return retVal;
   }
	// TarGets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
   {
		return retVal;
   }

   //GRAB MATERIAL DATA FOR NODE - can be one or more materials
   forceMaterial = node->GetUserPropBool(_T("SPECULAR"), val1) ||
                   node->GetUserPropBool(_T("MATERIAL"), val2);
   //forceMaterial ensures base material properties are taken
   //even if user is not taking them at global scope.
   numMat        = getMaterialData(node, hasBumpMap, forceMaterial);

	retVal = createAWObjectFromINode(node, parent);
	assignTransformController(node, retVal);
   if (GetIncludeAnim()) 
	{
		ExportAnimKeys(node, retVal);
	}//if (GetIncludeAnim())

   if (1>=numMat)
   {
      exportObjectData(os, node, retVal, hasBumpMap);       
      applyMaterial(retVal, 0);
   }//if (1>=numMat)
   else
   {
      for (int matID=0; matID<numMat; matID++)
      {  //createSubObjectForMapID call creates a new object and
         //inserts it in the children list of retVal.
         curObj = new AWObject(retVal->getName());
         exportObjectData(os, node, curObj, hasBumpMap, matID);
         applyMaterial(curObj, matID);
         //new code can use setController NULL because of AWNode::getMatrix update
         //but this causes older code to crash so for now use a new one that
         //will have an identity transform in its matrix for all t
         curObj->setController(NULL);
         retVal->addChild(curObj);
      }//for (int matID=0; matID<numMat; matID++)
   }//else
 
   //this is the AWObject created for this INode for single material objects
   //or the parent node of the set of AWObjects that make up composite
   clearMaterials();
   return retVal; 
}//AWNode* TnTExprt::ExportGeomObject(INode* node)



void
TnTExprt::exportObjectData(ObjectState& os, INode* node, AWObject* curObj, AWBoolean hasBumpMap, int matID)
{	
   TriObject* tri = (TriObject*)os.obj->ConvertToType(0, triObjectClassID);
   Mesh &mesh = tri->mesh;
	      
   getProperties(node, curObj);	   

   //get TEXTURE VERTEX, FACE INDEX & NORMAL data
   AWBoolean         val            = FALSE;
   AWBoolean         extractNormals = !node->GetUserPropBool(_T("NONORM"), val);

   MeshExtractInfo e;
   e.meshType        = hasBumpMap;
   e.matID           = matID;
   e.withNormals     = extractNormals;
   e.withMaterials   = TRUE;
   e.withTexCoords   = !node->GetUserPropBool(_T("NOTEXCOORDS"), val);
   if (GetIncludeMeshAnim() && IsMeshAnimated(node))
   {  //as ANIMATED MESH
      ExtractAnimMesh(curObj, node, os, e);
   }
   else //as SINGLE MESH
   {  
	   if (curObj->m_curMesh =  ExtractMesh(node, GetStaticFrame(), e))
      {
         curObj->m_curMesh->removeShared();
      }//if (curObj->m_curMesh =  ExtractMesh(node, GetStaticFrame(), e))
   }
	
   // Export the visibility track
   Control* visCont = node->GetVisController();
   if (visCont && DumpFloatKeys(visCont, curObj->m_visible))
   {//non-zero return value means we got some keys
   }
   else
   {  //just set to visible as simple sampling failed
      curObj->m_visible.setVal(1.0f);
   }

   // Delete the working object, if necessary
	if(os.obj != (Object *)tri)
   {
      tri->DeleteThis();
   }
}//void TnTExprt::exportObjectData(INode* node, AWObject* curObj, AWBoolean hasBumpMap, int matID)



void
TnTExprt::getProperties(INode* node, AWObject* obj)
{
   TSTR propVal;
   int distVal=0;
   AWBoolean val  = FALSE;
	if (node->GetUserPropInt(_T("FLAT"), distVal))
	{
		if (distVal == 1) //plane X= constant
		{
			obj->m_distanceTest = 1;
		}
		else if (distVal == 3) //plane Z = constant
		{
			obj->m_distanceTest = 3;
		}
		else //default is plane y = constant
		{
			obj->m_distanceTest = 2;
		}
	}//if (node->GetUserPropInt(_T("FLAT"), distVal))	
	if (node->GetUserPropString(_T("COLLIDE"),propVal))
	{
		if (propVal == CStr(_T("Y")))
		{
			obj->setCollide(TRUE);
		}
		else if (propVal == CStr(_T("S")))
		{
			obj->setIsCollisionSource(TRUE);
		}
		else if (propVal == CStr(_T("T")))
		{
			obj->setIsCollisionTarget(TRUE);
		}
	}//if (node->GetUserPropString(_T("COLLIDE"),propVal))
	val=FALSE;
	obj->setDynamicLighting(!node->GetUserPropBool(_T("NOLIGHT"), val));
   obj->setSeparateSpecular(node->GetUserPropBool(_T("SPECULAR"), val) ? TRUE:FALSE);
	//NOTES - the AWObject (derivative) will be added to the scene's root node
	//but also may end up being the environment or added to the projectile list
	//post processing will remove these from the usual node tree
}//void TnTExprt::getProperties(INode* node, AWObject* obj)



// Return a pointer to a TriObject given an INode or return NULL
// if the node cannot be converted to a TriObject
TriObject* 
TnTExprt::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
   { 
		TriObject* tri = (TriObject*)obj->ConvertToType(t, 
			                  Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else 
   {
		return NULL;
	}
}//TriObject* TnTExprt::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)


//GeomObject output

AWObject*
TnTExprt::createAWObjectFromINode(INode* node, AWNode* parent)
{
	AWObject* obj = NULL;
	AWBoolean val  = FALSE;
	TSTR propVal;
	if (node->GetUserPropBool(_T("PROJECTILE"), val))
	{
		obj = new AWRocket(node->GetName(), parent);
		float floatVal=0.0f;
		//now find the values for SPEED and LIFE
		if (obj && node->GetUserPropFloat(_T("SPEED"), floatVal))
		{
			((AWRocket*)obj)->setSpeed(floatVal);
		}
		if (obj && node->GetUserPropFloat(_T("LIFE"), floatVal))
		{
			((AWRocket*)obj)->setLife(floatVal);
		}	
		theScene.m_nodeList.addProjectile((AWRocket*)obj);
	}
	else if (node->GetUserPropString(_T("SEA"),propVal))
	{
		obj = new AWSeaObject(node->GetName(), parent);
		if (node->GetUserPropString(_T("POSITIVE_FOG"),propVal))
		{
			((AWSeaObject*)obj)->m_posFogName = propVal.data();
		}
		if (node->GetUserPropString(_T("NEGATIVE_FOG"),propVal))
		{
			((AWSeaObject*)obj)->m_negFogName = propVal.data();
		}		
	}
	else obj = new AWObject(node->GetName(), parent);
	//NOTES - the AWObject (derivative) will be added to the scene's root node
	//but also may end up being the environment or added to the projectile list
	//post processing will remove these from the usual node tree
	return obj;
}//AWObject* TnTExprt::createAWObjectFromINode(INode* node, AWNode* parent)
 


int
TnTExprt::addNormal(const Point3& norm, AWMesh& awMesh, int index)
{
   int retVal = -1;
	AWPoint curNormal = transformPoint(norm);
	curNormal.normalize();
   if ((-1!=index) && (index<awMesh.getNumVerts()))
   {
      awMesh.m_norms[index]=curNormal;
      retVal = index;
   }
   else if (index<awMesh.getNumNormals())
   {
      retVal = awMesh.addNormal(curNormal);
   }
   return retVal;
}//int TnTExprt::addNormal(const Point3& norm, AWMesh& awMesh, int index)



int
TnTExprt::addVertex(int vIndex, Mesh& mesh, Matrix3& tm, Matrix3& objTMAfterWSM, 
                    AWMesh& awMesh, int index)
{
   int retVal = -1;
   Point3 vert = mesh.verts[vIndex] * tm;
   AWPoint awVert(vert.x, vert.z, -vert.y);
   if ((-1!=index) && (index<awMesh.getNumVerts()))
   {
      awMesh.m_verts[index]=awVert;
      retVal = index;
   }
   else if (index<awMesh.getNumVerts())
   {
      retVal = awMesh.addVertex(awVert);
   }
   //need to update the scene max/min with transformed data
	vert = mesh.verts[vIndex] * objTMAfterWSM;
	theScene.m_nodeList.updateMaxValues(AWPoint(vert.x, vert.z, -vert.y));
   return retVal;
}//int TnTExprt::addVertex(int vIndex, Mesh& mesh, Matrix3& tm, AWMesh& awMesh, int index)



int
TnTExprt::addTVert(int vIndex,  Mesh& mesh, Matrix3& tm, AWMesh& awMesh)
{  
   int retVal = -1;
   //note that //typedef Point3 UVVert
   UVVert  tVert = mesh.tVerts[vIndex];
   tVert         = tm * tVert;
   AWTVert awTV(tVert.x, 1 - tVert.y);
   //we are adding this TVert to the 0th channel by default
   retVal        = awMesh.addTVertex(awTV);
   return retVal;
}//int TnTExprt::addTVert(int vIndex,  Mesh& mesh, Matrix3& tm, AWMesh& awMesh)



AWMesh* 
TnTExprt::createMesh(INode* node, const MeshExtractInfo& e, int numVerts, float t)
{
    AWMesh*     awMesh = NULL;
   //CREATE THE NEW MESH FOR THIS FRAME
   switch (e.meshType)
   {
   case 1:
      awMesh = new AWBumpMesh( numVerts, t );
      //keep track of this mesh, and the node that data comes from
      //going to need this to assign AWLight* later
      if (awMesh)
      {  //value based collection - insertion is doing a copy anyway
         AWBumpMeshTracker* tmp = new AWBumpMeshTracker((AWBumpMesh*)awMesh, node);
         m_trackBumps.Append(1, &tmp, 10);
      }//if (awMesh)
      break;

   case 0:
   default:
      awMesh = new AWMesh( numVerts, t );
   }//switch (meshType)
   return awMesh;
}//AWMesh* TnTExprt::createMesh(INode* node, const MeshExtractInfo& e, int numVerts, float t)



AWMesh* 
TnTExprt::ExtractMesh(INode* node, TimeValue t, MeshExtractInfo& e)
{  //extract SINGLE FRAME of a mesh for this time value - mesh type for now 
   //selects between STANDARD (AWMesh) and BUMP MESH (AWBumpMesh)
   //
   //EVALUATE THE OBJECT (and hence MESH) AT THIS TIMEVALUE
	Matrix3     objTMAfterWSM  = node->GetObjTMAfterWSM(t);
   //grab negScale now, before matrix changes.
	Matrix3     tm             = objTMAfterWSM * Inverse(node->GetNodeTM(t));
	ObjectState os             = node->EvalWorldState(t);
	if (!os.obj)         return NULL;
		
	AWBoolean needDel;
   Point3	vert;

	TriObject* tri = GetTriObjectFromNode(node, t, needDel);
	if (!tri)            return NULL;
	
	Mesh* mesh = &tri->mesh;
   if (!mesh->numVerts) return NULL;

   AWMesh*     awMesh = createMesh(node, e, mesh->numVerts, TicksToSec(t));
   if (e.refMesh && e.refMesh->getNumVerts())
   {
      AWPoint zeros(0.0f, 0.0f, 0.0f); 
      awMesh->m_verts.fillWith(zeros, e.refMesh->getNumVerts());
      if (e.withNormals && GetIncludeNormals())
      {
         awMesh->m_norms.fillWith(zeros, e.refMesh->getNumNormals());
      }//if (e.withNormals && GetIncludeNormals())
   }//if (e.refMesh && e.refMesh->getNumVerts())
   
   //
   //NOW FILL THE MESH WITH FULL DATASET FOR THIS FRAME
   //consists of =>=>=> 3 * numFaces vertices <=<=<= with matching data in the same
   //index position of each additional array for normals, texture vertices,
   //color vertices + any additional channels of data required.
   //face accessors depend on whether negative scale has been applied
   int  vx[3];
   // Order of vertices: grab counter clockwise if object negatively scaled.
   if (TMNegParity(tm)) 
	{
		vx[0] = 2; vx[1] = 1; vx[2] = 0;
	}//if (negScale)
	else 
	{
		vx[0] = 0; vx[1] = 1; vx[2] = 2;
	}//else
   if (e.withNormals && GetIncludeNormals())
   {
      mesh->buildNormals();
   }//if (getNormals && GetIncludeNormals())
   //
   Face     face;
   Matrix3  uvTransform;
   int      matID;
   for (int ix=0; ix < mesh->getNumFaces(); ix++)
   {
      face = mesh->faces[ix];
      //AWTRACE2("  processing face %d with ID %d\n", ix, (int)face.getMatID());
      //MATERIAL ID - constant in time
      matID = (int)face.getMatID();
      if (e.withMaterials)
      {
         if ((-1!=e.matID)&&(matID!=e.matID)) continue;
         //e.matIDList.Append(1, &matID);
      }//if (e.withFaces)
      //get correct texture matrix for this face/material combination 
      getTextureMatrix(uvTransform, matID);     
      int v;         //index of current 'tuple' of a face
      int vIndex, useIndex;    //working values
      for (int vNum=0; vNum<3; vNum++)
      {
         useIndex = ix*3+vNum;
         if (e.refMesh && (e.refMesh->m_indeces.getNum()>useIndex))
         {
            useIndex = e.refMesh->m_indeces[useIndex];
         }
         else
         {
            useIndex = -1;
         }
         //VERTEX DATA - 
         v = addVertex(face.v[vx[vNum]], *mesh, tm, objTMAfterWSM, *awMesh, useIndex);
         //FACE (INDEX) DATA -
         if (e.withFaces)
         { 
            awMesh->addIndex(v);
         }//if (e.withFaces)
         //NORMAL INFORMATION (OPTIONAL) -
		   if (e.withNormals && GetIncludeNormals())
		   {	// Face and vertex normals.
			   // In MAX a vertex can have more than one normal (but doesn't always have it).
			   // This is depending on the face you are accessing the vertex through.
			   // To Get all information we need to export all three vertex normals
			   // for every face.	
			   Point3 fn = mesh->getFaceNormal(ix);			      
			   vIndex    = face.getVert( vx[vNum] );
			   Point3 vn = GetVertexNormal(mesh, ix, mesh->getRVertPtr(vIndex));
            addNormal(vn, *awMesh, useIndex);
         }//if (getNormals && GetIncludeNormals())
         //TEXTURE COORDINATE INFO (OPTIONAL)
         //NOTE - there will be N such channels, optionally
         //for now there are just 2 - tex coords and color verts
         //each channel may have an associated transformation matrix
		   if (e.withTexCoords && GetExportTextures() && mesh->tvFace)
		   {
			   TVFace& face  = mesh->tvFace[ix];
			   vIndex        = face.t[vx[vNum]];   //index into MAX's tverts list
            UVVert  tVert = mesh->tVerts[vIndex];
            //transform into AWOpenGL format / 2 floats
            tVert         = uvTransform * tVert;
            AWTVert awTV(tVert.x, 1 - tVert.y);
            //we are adding this TVert to the 0th channel by default
            awMesh->addTVertex(awTV); //should be adding this at index v
         }//if (GetExportTextures() && mesh.tvFace)
         if (e.withColorVerts && GetIncludeVertexColors() && mesh->numCVerts)
		   {
            Point3   vc;
            vIndex = mesh->vcFace[ix].t[vx[vNum]];
            vc = mesh->vertCol[vIndex];
            awMesh->m_colVerts.add(AWColVertex(255.0f * vc.x, 255.0f * vc.y, 255.0f * vc.z));
         }//if (GetIncludeVertexColors() && mesh.numCVerts)
      }//for (int vNum=0; vNum<3; vNum++)
   }//for (int ix=0; ix < mesh.getNumFaces(); ix++)
   return awMesh;
}//AWMesh* TnTExprt::ExtractMesh(INode* node, TimeValue t, , MeshExtractInfo& e)



//Animation output
AWBoolean
TnTExprt::GetUserAnimRange(const TCHAR* ranges, int& start, int& nFrames, int upperBound)
{
	AWBoolean retVal = FALSE;
	retVal = (2 == _stscanf(ranges, _T(" [%d , %d ] "), &start, &nFrames));
	if (nFrames > upperBound)
		nFrames = upperBound;
	nFrames = nFrames - start + 1; //user specifies start & end frames INCLUSIVELY
	return retVal;
}//AWBoolean TnTExprt::GetUserAnimRange(ranges, start, nFrames, int upperBound)



AWBoolean 
TnTExprt::ExtractAnimMesh(AWObject* obj, INode* node, 
                          ObjectState& os, MeshExtractInfo& e)
{
   AWBoolean retVal = FALSE;
	Interval animRange  = ip->GetAnimRange();
	// Get validity of the object
	Interval objRange = os.obj->ObjectValidity(GetStaticFrame());
	
	// If the animation range is not fully included in the validity
	// interval of the object, then we're animated.
	if (!objRange.InInterval(animRange)) 
	{
		int numFrames = animRange.Duration() / GetTicksPerFrame();	
		if (numFrames)
		{
			//now we need to divide the full animation sequence into
			//any ranges specified in MAX (as user defined properties)
			//for now, require following syntax from MAX user -
			//NUMANIM=x
			//ANIM1=start1
			//...
			//ANIMX=startX
			//where startX < numFrames
			int val=0;
			if (node->GetUserPropInt(_T("NUMANIM"), val) && val)
			{ //we have multiple animations specified for this object
				//so process each one in turn using the simple MAX interface
				TCHAR animName[50];
				TSTR  ranges;
				int   start, nFrames;
				for (int anim=0; anim<val; anim++)
				{	//construct the correct key name (1 based index for artists...!)
					_stprintf(animName, _T("ANIM%u"), anim+1);
					//and find the frame range from user input
					if (node->GetUserPropString(animName, ranges) &&
						 GetUserAnimRange(ranges, start, nFrames, numFrames))
					{//the ranges have been found for this Anim.
						ExtractAnimMeshSet(obj, node, start, nFrames, e);
					}//if (node->GetUserPropString(animName, ranges))
				}//for (int anim=0; anim<val; anim++)
			}//if (node->GetUserPropInt(_T("NUMANIM"), val) && val)
			else 
			{	//there's just a single mesh anim on this object (by default)
				ExtractAnimMeshSet(obj, node, 0, numFrames, e);
			}
         retVal = TRUE;
         AWBoolean interpolate;
			if (obj->m_animMeshes.getNum() && !node->GetUserPropBool(_T("NOINTERP"), interpolate))
			{
            obj->m_usingFrameBuffer = TRUE;
			}
		}//if (AWobj->m_numFrames)
	}//if (!objRange.InInterval(animRange)) 
	return retVal;
}//AWBoolean TnTExprt::ExtractAnimMesh(AWObject* obj, INode* node, ObjectState& os, MeshExtractInfo& e)



void 
TnTExprt::ExtractAnimMeshSet(AWObject* obj, INode* node, int frameStart, 
                             int numFrames, MeshExtractInfo& e)
{
   if (!obj || !node || !numFrames) return;
   int               meshStep = 1;
   AWMesh*           nextMesh = NULL;
   AWAnimatedMesh*   set		= new AWAnimatedMesh;

   if (node->GetUserPropInt(_T("MESHSTEP"), meshStep) && (meshStep>0))
   {
      //use this retrieved value for meshStep
   }
   else
   {  //we do NOT have a user specified mesh step for this node
      //so use the globally defined value for this export
      meshStep = GetMeshFrameStep();
   }
   //clean up any existing content - which shouldn't be there
   obj->m_animMeshes.destroy();
   //we want to minimize the work done on export - for the sake of time
   //and memory. We can determine from MAX Channels if the following are animated:
   //
   // GEOM_CHAN_NUM       - VERTICES - guarenteed if we got this far...
   // TEXMAP_CHAN_NUM     - TEXTURE COORDINATES 
   // VERT_COLOR_CHAN_NUM - PER-VERTEX COLOURS
   // TOPO_CHAN_NUM       - TOPOLOGICALLY INVARIANT 
   //      Tri Object =>face structure, the materials, and smoothing information
   //      hence the NORMALS? Or are they implied by GEOM_CHAN_NUM?
   //getFaces, getNormals, getMaterials, getTexCoords
   AWBoolean vTopology, vColorVerts, vTexCoords;
   IsMeshAnimated(node, vTopology, vColorVerts, vTexCoords);

   TimeValue start = frameStart * GetTicksPerFrame();
   TimeValue end   = start + numFrames * GetTicksPerFrame();
   TimeValue t = start;
   TimeValue delta = GetTicksPerFrame() * meshStep;
   AWMesh* referenceFrame = NULL;
   while (t <=end)
   {  //for each frame, we want to determine which data is extracted.
      //the first frame is a special case, in that we need to get 
      //ALL REQUIRED DATA for the FIRST FRAME
      //subsequent frames may need only the vertices
      //or may go as far as needing the face indeces
      e.withFaces      = (t == start);                //only for first face
      e.withColorVerts = (t == start);   //first frame only for now...
      e.withTexCoords  = (t == start);                //first frame only
      e.withNormals    = (t == start)||vTopology;     //sample for all frames
      e.refMesh        = referenceFrame;
      
		nextMesh = ExtractMesh(node, t, e);
      if (t == start)
      {
         referenceFrame = nextMesh;
         if (nextMesh) nextMesh->removeShared();
      }//if (frame == frameStart)
		//at this point must check to see if the new frame is different
		//from the previous one. if not, just dump it and move on
		//and check out the next otherwise keep it
		if (!set->insertFrame(nextMesh))
		{
			delete nextMesh;
			nextMesh = NULL;
		}//if (!set->insertFrame(nextMesh))
      t+=delta; //move to next sample frame
      if ((t > end) && (t - end < delta))
      {  //make sure we take a sample frame at the end of the interval.
         t = end;
      }//if ((t > end) && (t - end < delta))
   }//while (t <=end)

   if (set->getNumFrames()>1)
   {
	   obj->m_animMeshes.add(set);
   }
   else if (1==set->getNumFrames())
   {  //grab the mesh from the mesh set. put on object
      //delete the mesh set as not requried.
      AWMesh* m = set->getMesh(0);
      obj->m_curMesh = m;
      set->clear();
      delete set;
   }
   else 
   {
      delete set;
   }
}//void TnTExprt::ExtractAnimMeshSet(AWObject* obj, INode* node, 
///                            int frameStart, int numFrames, MeshExtractInfo& e)

Point3 TnTExprt::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
	{
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) 
		{
			vertexNormal = rv->rn.getNormal();
		}
		else 
		{
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) 
			{
				if (rv->ern[i].getSmGroup() == smGroup)
				{
					vertexNormal = rv->ern[i].getNormal();
					break;

				}			
			}
			if (i == numNormals)	// DAP - ?? Can this ever happen?
				vertexNormal = rv->ern[0].getNormal();

		}
	}
	else 
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}




AWBoolean  
TnTExprt::IsMeshAnimated(INode* node, AWBoolean& vTopology, AWBoolean& vColorVerts, 
                         AWBoolean& vTexCoords)
{
   AWBoolean retVal = FALSE;
   TimeValue tv = GetStaticFrame()*GetTicksPerFrame();
   Interval ivalid, topoValid, colVValid, texCoordValid;
   Object *obj = node->EvalWorldState(tv).obj;
   if (obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0)) 
   {
      // Gotta create a TriObject
      TriObject *triObj;
      BOOL deleteIt = FALSE;
      if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
      {
         triObj = (TriObject *) obj->ConvertToType(tv,
                         Class_ID(TRIOBJ_CLASS_ID, 0));
         // Note that the TriObject should only be deleted
         // if the pointer to it is not equal to the object
         // pointer that called ConvertToType().
         if (obj != triObj) deleteIt = TRUE;
      }
      else 
      {
         return FALSE;
      }
      ivalid = triObj->ChannelValidity(tv, GEOM_CHAN_NUM);
      topoValid = obj->ChannelValidity(tv, TOPO_CHAN_NUM);
      colVValid = obj->ChannelValidity(tv, VERT_COLOR_CHAN_NUM);
      texCoordValid = obj->ChannelValidity(tv, TEXMAP_CHAN_NUM);
      if (deleteIt) triObj->DeleteMe();
   }
   else 
   {
      ivalid = obj->ChannelValidity(tv, GEOM_CHAN_NUM);
      topoValid = obj->ChannelValidity(tv, TOPO_CHAN_NUM);
      colVValid = obj->ChannelValidity(tv, VERT_COLOR_CHAN_NUM);
      texCoordValid = obj->ChannelValidity(tv, TEXMAP_CHAN_NUM);
   }
   Interval animRange = ip->GetAnimRange();
   retVal = !ivalid.InInterval(animRange);
   vTopology = !topoValid.InInterval(animRange);
   vColorVerts = !colVValid.InInterval(animRange);
   vTexCoords  = !texCoordValid.InInterval(animRange);
        return retVal;
}//AWBoolean TnTExprt::IsMeshAnimated(INode* node,...)



AWBoolean
TnTExprt::IsMeshAnimated(INode* node)
{
   AWBoolean retVal = FALSE;
   TimeValue tv = GetStaticFrame()*GetTicksPerFrame();
   Interval ivalid;
   Object *obj = node->EvalWorldState(tv).obj;
   if (obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0)) 
   {
      // Gotta create a TriObject
      TriObject *triObj;
      BOOL deleteIt = FALSE;
      if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
      {
         triObj = (TriObject *) obj->ConvertToType(tv,
                         Class_ID(TRIOBJ_CLASS_ID, 0));
         // Note that the TriObject should only be deleted
         // if the pointer to it is not equal to the object
         // pointer that called ConvertToType().
         if (obj != triObj) deleteIt = TRUE;
      }
      else 
      {
         return FALSE;
      }
      ivalid = triObj->ChannelValidity(tv, GEOM_CHAN_NUM);
      if (deleteIt) triObj->DeleteMe();
   }
   else 
   {
      ivalid = obj->ChannelValidity(tv, GEOM_CHAN_NUM);
   }
   retVal = !ivalid.InInterval(ip->GetAnimRange());
        // Display the interval...
        //TSTR start, end;
        //TimeToString(ivalid.Start(), start);
        //TimeToString(ivalid.End(), end);
        //TCHAR buf[256];
        //_stprintf(buf, _T("%s interval=[%s, %s]"), node->GetName(),
        //        start.data(), end.data());
        //MessageBox(NULL, buf, _T("GEOM_CHAN_NUM Validity"),
        //        MB_OK|MB_ICONINFORMATION);
        return retVal;
}//AWBoolean TnTExprt::IsMeshAnimated(INode* node, int startFrame, int endFrame)


// From the SDK
// How to calculate UV's for face mapped materials.
static Point3 basic_tva[3] = { 
	Point3(0.0,0.0,0.0),Point3(1.0,0.0,0.0),Point3(1.0,1.0,0.0)
};
static Point3 basic_tvb[3] = { 
	Point3(1.0,1.0,0.0),Point3(0.0,1.0,0.0),Point3(0.0,0.0,0.0)
};
static int nextpt[3] = {1,2,0};
static int prevpt[3] = {2,0,1};

void TnTExprt::make_face_uv(Face *f, Point3 *tv)
{
	int na,nhid,i;
	Point3 *basetv;
	/* make the invisible edge be 2->0 */
	nhid = 2;
	if (!(f->flags&EDGE_A))  nhid=0;
	else if (!(f->flags&EDGE_B)) nhid = 1;
	else if (!(f->flags&EDGE_C)) nhid = 2;
	na = 2-nhid;
	basetv = (f->v[prevpt[nhid]]<f->v[nhid]) ? basic_tva : basic_tvb; 
	for (i=0; i<3; i++) {  
		tv[i] = basetv[na];
		na = nextpt[na];
	}
}


//Face Mapped Material functions

AWBoolean TnTExprt::CheckForAndExportFaceMap(Mtl* mtl, Mesh* mesh)
{
	if (!mtl || !mesh) 
   {
		return FALSE;
	}
	
	ULONG matreq = mtl->Requirements(-1);
	
	// Are we using face mapping?
	if (!(matreq & MTLREQ_FACEMAP)) 
   {
		return FALSE;
	}
	
	for (int i=0; i<mesh->getNumFaces(); i++) 
   {
		Point3 tv[3];
		Face* f = &mesh->faces[i];
		make_face_uv(f, tv);
	}
	
	return TRUE;
}


//Misc Utility functions

// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'.
AWBoolean TnTExprt::TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

