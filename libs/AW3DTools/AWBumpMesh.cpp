#include "AWBumpMesh.h"
#include "AWLight.h"

#define VER_AWBumpMesh_1	 100


AWDEFINE_VERSIONABLE_PERSISTABLE(AWBumpMesh, CLSID_AWBUMPMESH, VER_AWBumpMesh_1);


#define UL_EPS 1e-12
#define UL_EPS2 (UL_EPS * UL_EPS)


void   
AWBumpMesh::init(float t)
{
   if (!m_tangentSpace.getNum())
   {
      buildTextureSpace();
   }
}//void AWBumpMesh::init(float t)



void	  
AWBumpMesh::interpolate(AWMesh* b, AWMesh* a, float fac)
{	//interpolate between the two frames, putting the result into
	//this mesh - the buffer
	//REQUIRE: all meshes (srce1, srce2, dest) 
	//have the same number of vertices == val.m_numVerts
	AWMesh::interpolate(b, a, fac);  
   //now interpolate between per-vertex 'tangent space' basis matrices
   //require normalization after interpolation
   if (!b || !a || 
       (CLSID_AWBUMPMESH != b->isA()) || 
       (CLSID_AWBUMPMESH != a->isA())  ) return;
   AWBumpMesh* bBmp = (AWBumpMesh*)b;
   AWBumpMesh* aBmp = (AWBumpMesh*)a;
   if (!bBmp->m_tangentSpace.getNum()||!aBmp->m_tangentSpace.getNum()) return;
   for (int t=0; t<m_tangentSpace.getNum(); t++)
   {
      m_tangentSpace[t] = bBmp->m_tangentSpace[t] + 
                fac * (aBmp->m_tangentSpace[t] - bBmp->m_tangentSpace[t]);
      m_tangentSpace[t].normalizeBasis();
   }//for (int t=0; t<m_tangentSpace.getNum(); t++)
}//void	AWBumpMesh::interpolate(AWMesh* b, AWMesh* a, float fac)



void 
AWBumpMesh::buildTextureSpace(AWBoolean find_duplicate_verts )
{
   //AWQuat temp;
   //temp.fromAxisAngle(AWPoint(0.0f, 1.0f, 0.0f), 90.0f);
   int nVerts = getNumVerts();
   int nTris  = m_indeces.getNum() / 3; 
   //only bump map smooth shaded objects - could generate face
   //normals but hardly a nice effect... 
   //Need (diffuse) tex coords to generate approximate
   //tangent space basis
   if (!getNumNormals() || !getNumTVerts()) return; 
   if (1==getNumTVChannels())
   {  //USUAL CASE, FOR NOW...
      AWPoint zeros(0.0f, 0.0f, 0.0f);
      m_tvHelper.destroy();
      //we have (tangent space) LIGHT VECTOR per vertex
      AWTVertexList* lv = new AWTVertexList(nVerts, 3);
      lv->fillWith(zeros, nVerts);
      m_tVerts.add(lv); //ADD BUMP CHANNEL as chanID# 1
      //and (tangent space) HALF (VIEW) ANGLE per vertex
      AWTVertexList* hv = new AWTVertexList(nVerts, 3);
      hv->fillWith(zeros, nVerts);
      m_tVerts.add(hv); //ADD SPECULAR CHANNEL as chanID# 2
      if (!m_tVerts.getOwnsObjects())
      {  //this will make sure objects are destroyed
         m_tvHelper.add(lv); 
         m_tvHelper.add(hv);
      }
   }//if (1==getNumTVChannels())
   //build the texture space bases -
   AWMatrix3 id; id.identity();
   m_tangentSpace.fillWith(id, nVerts);
	
	AWPoint* av_du = new AWPoint[ nVerts ];
	AWPoint* av_dv = new AWPoint[ nVerts ];

	//Set du/dv accumulation to zero to begin
   //should already have been done by AWPoint constructors above
	calculateVertexGradients( av_du, av_dv );
		
	AWPoint     bi, normal;
	float       dotVal;
   AWMatrix3   basis;

	for(int i = 0; i < nVerts; i++)
	{		
		av_du[i].normalize();
		av_dv[i].normalize();
				
		bi = cross(av_du[i], av_dv[i]);

      normal  = *getNormal(i);		
		normal.normalize();		

		// v coordinates go in opposite direction 
      //from the texture v increase in xyz
		av_dv[i] = -av_dv[i];
		
		dotVal = dot(bi, normal);
		if (dotVal < 0)
		{
			bi = -bi;
		}//if (dotVal < 0)
	
      basis.setTX(av_dv[i]);
      basis.setTY(av_du[i]);
      basis.setTZ(bi);
      m_tangentSpace[i] = basis;
	}//for( i = 0; i < nVerts; i++)

   // Need to search the mesh for vertices with the same spatial coordinate
	// These are vertices duplicated for lathed/wrapped objects to make a
	//   2nd set of texture coordinates at the point in order to avoid bad
	//   texture wrapping
	// In short:  For each vertex, find any other vertices that share it's 
	//   position.  "Average" the tangent space basis calculated above for
	//   these duplicate vertices.  This is not rigorous, but works well 
	//   to fix up 3DStudio models.

	// Check each vert with every other.  There's no reason to check
	//   j with i after doing i with j, so start j from i ( so we test
	//   1 with 2 but never 2 with 1 again).
	// This is a model pre-processing step and only done once.  For large
	//   models, compute this off-line if you have to and store the resultant
		//   data.
   
	if (find_duplicate_verts)
   {
      int      i, j;
      typedef AWList<unsigned int>* matchList;	
      AWPoint diff; 
      float   epsilon     = 1.0e-8f;
      // each vertex gets a list of vertices that match its position
      AWList< AWList<unsigned int>* >  duplicates;
      duplicates.fillWith((AWList<unsigned int>*)NULL, nVerts);
      
		for(i=0; i < nVerts; i++ )
		{
         duplicates[i] = NULL;
			for(j=i+1; j < nVerts; j++ )
			{
            diff = *getVertex(i) - *getVertex(j);
				if( diff.sqrMagnitude() < epsilon )
            {  // if i matches j and k, just record into i.  j will be 
					//  half full as it will only match k, but this is
					//  taken care of when i is processed.
               if (NULL == duplicates[i])
               {
                  duplicates[i] = new AWList<unsigned int>(5);
               }
               if (duplicates[i])
               {
                  duplicates[i]->add(j);
               }
				}//if( diff.sqrMagnitude() < epsilon )
			}//for(j=i+1; j < nVerts; j++ )
		}//for(i=0; i < nVerts; i++ )

		// Now average the tangent spaces & write the new result to
		//  each duplicated vertex
		AWMatrix3   temp;
      int         nDup, dupIndex;
		for(i = 0; i<nVerts; i++)
		{
         nDup = (duplicates[i]) ? duplicates[i]->getNum() : 0;
			if( (nDup > 0) && (nDup < 5) && (m_tangentSpace.getNum() >= i))
         {  // If there are more than 5 vertices sharing this point then
				//  the point is probably some kind of lathe axis node.  No need to
				//  process it here
				temp = m_tangentSpace[i];
				for (j=0; j<nDup; j++)
				{
               dupIndex = (*duplicates[i])[j];
					temp = temp + m_tangentSpace[dupIndex];
				}//for (j=0; j<nDup; j++)

				// Normalize the vectors of the basis matrix
				temp.normalizeBasis();

				// Write this average basis to each overlapped vertex
				m_tangentSpace[i] = temp;
				for (j=0; j<nDup; j++)
				{  //for each of the duplicates higher in the list...
               dupIndex = (*duplicates[i])[j];
					m_tangentSpace[dupIndex] = temp;
               // Kill the duplicate index lists of all vertices of
					//  higher index which overlap this one.
               delete duplicates[dupIndex]; //works OK on NULL...
               duplicates[dupIndex] = NULL;
				}//for (j=0; j<nDup; j++)
			}//if( (nDup > 0) && (nDup < 5) && (m_tangentSpace.getNum() >= i))
			if (NULL != duplicates[i])
			{  //clean up each duplicates list after using it.
				delete duplicates[i];
				duplicates[i] = NULL;
			}//if (NULL != duplicates[i])
		}//for(i = 0; i<nVerts; i++)
	}//if (find_duplicate_verts)

	delete [] av_du;
	delete [] av_dv;
}//void AWBumpMesh::buildTextureSpace(AWBoolean find_duplicate_verts )



void 
AWBumpMesh::calculateVertexGradients(AWPoint* av_du, AWPoint* av_dv)
{  
   AWPoint  du, dv;  
   int      iv[3];
   int      vertcount = getNumVerts();

	// triangle list
   for (int i=0; i<m_indeces.getNum(); i+=3)
   {       
      iv[0] = getIndex(i);
      iv[1] = getIndex(i+1);
      iv[2] = getIndex(i+2);

      if( iv[0] < vertcount && iv[1] < vertcount  && iv[2] < vertcount )
      { 
         calc_gradients( du, dv, iv[0], iv[1], iv[2]);

         av_du[iv[0]] += du;
         av_du[iv[1]] += du;
         av_du[iv[2]] += du;

         av_dv[iv[0]] += dv;
         av_dv[iv[1]] += dv;
         av_dv[iv[2]] += dv;
      }//if( iv[0] < vertcount && iv[1] < vertcount  && iv[2] < vertcount )
   }//for (int i=0; i<m_indeces.getNum(); i+=3)
}//void AWBumpMesh::calculateVertexGradients(AWPoint* av_du, AWPoint* av_dv)


void 
AWBumpMesh::calc_gradients(AWPoint& du, AWPoint& dv, int i0, int i1, int i2)
{
   AWPoint  v[3];
   AWTVert  tv[3]; //all tverts being considered here are 2D

   v[0]  = *getVertex(i0);
   v[1]  = *getVertex(i1);
   v[2]  = *getVertex(i2);

   //we assume diffuse texcoords in 0th channel
   tv[0] = *(AWTVert*)getTVertex(i0);
   tv[1] = *(AWTVert*)getTVertex(i1);
   tv[2] = *(AWTVert*)getTVertex(i2);

   AWPoint edge01( v[1].x - v[0].x, tv[1].x - tv[0].x, tv[1].y - tv[0].y );
   AWPoint edge02( v[2].x - v[0].x, tv[2].x - tv[0].x, tv[2].y - tv[0].y );

   AWPoint cp = cross(edge01, edge02);

   if ( fabs(cp.x) > UL_EPS )
   {
      du.x = -cp.y / cp.x;        
      dv.x = -cp.z / cp.x;
   }

   edge01 = AWPoint( v[1].y - v[0].y, tv[1].x - tv[0].x, tv[1].y - tv[0].y );
   edge02 = AWPoint( v[2].y - v[0].y, tv[2].x - tv[0].x, tv[2].y - tv[0].y );

   cp = cross(edge01, edge02);

   if ( fabs(cp.x) > UL_EPS )
   {
      du.y = -cp.y / cp.x;
      dv.y = -cp.z / cp.x;
   }

   edge01 = AWPoint( v[1].z - v[0].z, tv[1].x - tv[0].x, tv[1].y - tv[0].y );
   edge02 = AWPoint( v[2].z - v[0].z, tv[2].x - tv[0].x, tv[2].y - tv[0].y );

   cp = cross(edge01, edge02);

   if ( fabs(cp.x) > UL_EPS )
   {
      du.z = -cp.y / cp.x;
      dv.z = -cp.z / cp.x;
   }
}//void AWBumpMesh::calc_gradients(AWPoint& du, AWPoint& dv, int i0, int i1, int i2)



void   
AWBumpMesh::update(const AWMatrix4& objTM, float t, const AWSolidCone& viewCone) 
{  //bump mesh now has access to: object wsTM
   //requires access to:          LIGHT  wsTM
   //base class method does nothing at the present time
   if (m_bumpLight)
   {  //without a main light, we don't see bump mapping anyway...
      AWPoint     vert, halfAngle;
      AWPoint     eyePos, eyeDir;
      AWPoint     lightDir, lightPos;
      AWMatrix4   objInvWSTM, lightTM;
      AWMatrix3   tSpace;
      
      lightTM = m_bumpLight->getWSTM();
      lightTM.getTranslation(lightPos);
      //this position is expressed in WORLD SPACE - need in OBJECT SPACE
      objTM.invert(objInvWSTM);
      lightPos = objInvWSTM * lightPos;
      eyePos   = objInvWSTM * viewCone.m_V;
      
      //need to process every vertex in the mesh    
      for (int v=0; (v < getNumVerts()) && 
                    (v < m_tangentSpace.getNum()); v++)
      {
         vert = *getVertex(v);
         //tSpace = m_tangentSpace[v];
         // Subtract vertex position from the light position to compute the
         // (unnormalized) direction vector to the light in OBJECT SPACE. 
         //lightDir = lightPos - vert;
         lightDir = lightPos;
         // Rotate into the vertex's tangent space and store. 
         lightDir = m_tangentSpace[v] * lightDir;    
         
         //want to put in texture vertex index v for channel 1
         //AWTRACEHI4("Setting light vec for vertex %i: (%f, %f, %f)\n", v, lightDir.x, lightDir.y, lightDir.z);
         setTVertex(lightDir, v, 1);
         // Subtract vertex position from the eye position to compute the
         // (unnormalized) direction vector to the eye in object space. 
         eyeDir = eyePos - vert;
         // Rotate into the vertex's tangent space. 
         eyeDir = m_tangentSpace[v] * eyeDir; 
         eyeDir.normalize();
         // Form the half angle vector by adding the normalized light and eye vectors. 
         halfAngle = lightDir.normalize() + eyeDir;
         halfAngle.normalize();
         // Store the normalized half angle vector. 
         setTVertex(halfAngle, v, 2);
      }//for (int v=0; v < getNumVerts(); v++)
   }//if (m_bumpLight)
}//void AWBumpMesh::update(const AWMatrix4& objTM, float t, const AWSolidCone& viewCone)



AWBoolean		
AWBumpMesh::sameAs(AWBumpMesh* other)
{
	AWBoolean retVal = FALSE;
	if ( other && (getNumVerts() == other->getNumVerts()) )
	{	//see if ALL DATA EXCEPT TIME matches
		//ensure all vertices match
		retVal = (m_verts == other->m_verts) && 
               (m_colVerts == other->m_colVerts);
	}//if (other)
	return retVal;
}//AWBoolean AWBumpMesh::sameAs(AWBumpMesh* other)


//persistence for AWBumpMesh
//AWPersistable VIRTUALS
AWBoolean		
AWBumpMesh::isEqual(const AWPersistable* o) const
{
	return (m_time == ((const AWBumpMesh*)o)->m_time);
}//AWBoolean AWBumpMesh::isEqual(const AWPersistable* o) const;



int						
AWBumpMesh::compareTo(const AWPersistable* o) const
{
	return (int)(100.0 * (m_time - ((const AWBumpMesh*)o)->m_time));
}



void    		
AWBumpMesh::restoreGuts(AWFile& file)
{	//clean up any existing data
	
	//read the body
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWBumpMesh_1 >= version)
	{
		AWMesh::restoreGuts(file);
      m_tangentSpace.read(file);
      file >> m_bumpLight;
	}//if (VER_AWBumpMesh_1 >= version)  
}//void AWBumpMesh::restoreGuts(AWFile& file)



void    		
AWBumpMesh::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWMesh::saveGuts(file);
   m_tangentSpace.write(file);
   file << m_bumpLight;
}//void AWBumpMesh::saveGuts(AWFile& file) const



AWspace 
AWBumpMesh::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
      m_tangentSpace.getStoreSize() +
      AWPERSISTABLE_POINTER_SIZE(m_bumpLight)+
		AWMesh::binaryStoreSize();
	return retVal;
}//AWspace AWBumpMesh::binaryStoreSize() const