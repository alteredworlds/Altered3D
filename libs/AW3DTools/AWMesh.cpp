#include "AWMesh.h"
#include <float.h>   //for FLT_MAX
#include <limits.h> //for UINT_MAX, UCHAR_MAX

#define VER_AWMesh_1	 100
#define VER_AWAnimatedMesh_1 100

#define VER_AWMesh_2 VER_AWMesh_1 + 1
#define VER_AWMesh_3 VER_AWMesh_2 + 1
#define VER_AWMesh_4 VER_AWMesh_3 + 1

#define VER_AWAnimatedMesh_2 VER_AWAnimatedMesh_1 + 2

//tag array elements that aren't yet filled with new data

extern const AWPoint        maxPoint(FLT_MAX, FLT_MAX, FLT_MAX); 
extern const AWColVertex    maxCol(UCHAR_MAX, UCHAR_MAX);


AWDEFINE_VERSIONABLE_PERSISTABLE(AWAnimatedMesh, CLSID_AWANIMATEDMESH, VER_AWAnimatedMesh_2);
AWDEFINE_VERSIONABLE_PERSISTABLE(AWMesh, CLSID_AWMESH, VER_AWMesh_4);

//members for AWMesh

void 
AWMesh::offset(const AWPoint& offset)
{
}//void AWMesh::offset(const AWPoint& offset)


void 
AWMesh::scale(const AWPoint& scale)
{
	//scale vertices
	for (int vert = 0; vert < getNumVerts(); vert++)
	{
		(*getVertex(vert)) *= scale;
	}//for (int vert=0; vert < getNumVerts(); vert++)
}//void AWMesh::scale(const AWPoint& scale)



void	  
AWMesh::interpolate(AWMesh* b, AWMesh* a, float fac)
{	//interpolate between the two frames, putting the result into
	//the supplied buffer
   //BASE REQUIREMENT:
   //All frames have SAME NUMBER OF FACES and that for all frames
   //the faces are specified in the same order.
   //
   //BEST DESIGN - IMPLEMENTED:
   //Interpolate based on FACE - i.e. get each set of verts using 
   //face indeces and store result of interpolating between
   //in THIS buffer mesh. Doesn't even require constant face 
   //indeces so long as the ones in the buffer mesh frame are 
   //self-consistent.
   //
   //OLD IMPLEMENTATION - SIMPLE AND FAST (ideally):
	//REQUIRE: all meshes (srce1, srce2, dest) 
	//have the same NUMBER OF VERTICES
   //REQUIRE: face indeces constant.
   //HENCE: we know that indeces match accross all lists
   //and thus vert 1 always appears in the same face & position 
   //we claim that getVertex(vertex_index) will map to 
   //getVertex(vertex_index) in any frame. 
   //Hence interpolate btwn...
   if (!a || !b) return;
   //iterate through all face on the mesh, using the index array
   //and assuming all faces are triangular, consisting of
   //3 consecutive indeces.
   int v, va, vb;
   int numTriangles = m_indeces.getNum() / 3;
   for (int face=0; face<numTriangles; face++)
   {
      int base = face * 3;     
      for (int vert=0; vert<3; vert++)
      {
         v  = getIndex(base+vert);
         va = a->getIndex(base+vert);
         vb = b->getIndex(base+vert);

         (*getVertex(v)) = *b->getVertex(vb) + 
					fac * (*a->getVertex(va) - *b->getVertex(vb));
         //interpolate between normals, iff the lists are 
         //different. use fast, efficient pointer comparison
         if (b->getNormalList() != a->getNormalList())
         {
            (*getNormal(v)) = *b->getNormal(vb) + 
				      fac * (*a->getNormal(va) - *b->getNormal(vb));
         }//if (b->getNormalList() != a->getNormalList())
      }//for (int vert=0; vert<3; vert++)
   }//for (int face=0; face<numTriangles; face++)
}//void	AWMesh::interpolate(AWMesh* b, AWMesh* a, float fac)




void
AWMesh::getTuple(int i, AWPoint& v, AWPoint& n, AWColVertex& c, AWPoint& tv)
{   
   if (i<getNumColVerts()) 
   {
      c = *getColourVertex(i);
   }
   if (i<getNumVerts()) 
   {
      v = *getVertex(i);
   }
   if (i<getNumNormals())  
   {
      n = *getNormal(i);
   }
   if (getNumTVChannels() && (i<getNumTVerts()))
   {
      float* tmp = getTVertex(i);
      tv.x= tmp[0];
      if (2<=getTVertexListDim())
      {
         tv.y= tmp[1];
      }
      if (3<=getTVertexListDim())
      {
         tv.z= tmp[2];
      }
   }//if (getNumTVChannels() && (i<getNumTVerts()))
}//void AWMesh::getTuple(int i, AWPoint& v, AWPoint& n, AWColVertex& c, AWPoint& tv)



void
AWMesh::removeTupleAndRemapIndex(unsigned int index, unsigned int newIndex)
{
   m_verts.delIndex(index);;
   m_norms.delIndex(index); 
   m_colVerts.delIndex(index);
   if (getNumTVChannels() && m_tVerts[0])
   {
      m_tVerts[0]->delIndex(index);
   }//if (getNumTVChannels())

   //remap any faces using the old index value
   for (int i=0; i<m_indeces.getNum(); i++)
   {
      if (m_indeces[i]==index)
      {
         m_indeces[i]=newIndex;
      }//if (m_indeces[i]==(unsigned int)index)
      else if (m_indeces[i]>index)
      {
         m_indeces[i] = m_indeces[i]-1;
      }//else if (m_indeces[i]>index)
   }//for (i=0; i<m_indeces.getNum(); i++)
}//void AWMesh::removeTupleAndRemapIndex(unsigned int src, unsigned int dst)



void
AWMesh::removeShared()
{
   AWPoint     v, v1, n, n1, tv, tv1;
   AWColVertex c, c1;
   int         i,j;
   for (i=getNumVerts()-1; i>0; i--)
   {  //checking tuple at index value i
      getTuple(i, v, n, c, tv);
      for (j=i-1; j>=0; j--)
      {  //against tuple at index value j
         getTuple(j, v1, n1, c1, tv1);
         if ((v==v1)&&(n==n1)&&(c==c1)&&(tv==tv1))
         {//if matches - 
            //for all faces that reference this tuple
            //by specifying index i
            //we can now specify index j instead.
            removeTupleAndRemapIndex(i, j);
            break;
         }//if ((v==v1)&&(n==n1)&&(c==c1)&&(tv==tv1))         
      }//for (j=i-1; j>=0; j++)
   }//for (i=getNumVerts()-1; i>0; i--)
}//void AWMesh::removeShared()



void 
AWAnimatedMesh::offset(const AWPoint& offset)
{
}//void AWAnimatedMesh::offset(const AWPoint& offset)



AWBoolean		
AWMesh::sameAs(AWMesh* other)
{
	AWBoolean retVal = FALSE;
	if ( other && (getNumVerts() == other->getNumVerts()) )
	{	//see if ALL DATA EXCEPT TIME matches
		//ensure all vertices match
		retVal = (m_verts == other->m_verts) && 
               (m_colVerts == other->m_colVerts);
	}//if (other)
	return retVal;
}//AWBoolean AWMesh::sameAs(AWMesh* other)




AWBoolean
AWAnimatedMesh::insertFrame(AWMesh* data)
{	//check to see if this new frame actually has different data to the last one
	//and if so, keep it
	AWBoolean retVal = FALSE;
	if (data)
	{
		int lastOne = m_frames.getNum()-1;
		if ( (0 <= lastOne) && data->sameAs( (AWMesh*)m_frames[lastOne] ))
		{	//the new frame is identical to the last one - don't insert it
			//and return FALSE
			;
		}
		else
		{
			m_frames.add(data);
			retVal = TRUE;
		}
	}
	return retVal;
}//AWBoolean AWAnimatedMesh::insertFrameAt(int frameNum, AWMesh* data)




void 
AWAnimatedMesh::scale(const AWPoint& scale)
{
	//scale vertices in all Frames in every FrameSet 
	for (int setToScale=0; setToScale<(int)m_frames.getNum(); setToScale++)
	{
		((AWMesh*)m_frames[setToScale])->scale(scale);
	}
}//void AWAnimatedMesh::scale(const AWPoint& scale)



int				
AWAnimatedMesh::linearInterp(AWMesh* val, AWMesh* pBefore, AWMesh* pAfter, float t)
{
	int retVal = 0;
	if (!val || !pBefore) return retVal;
	//compute interpolated values
	float dt = pBefore->m_time;
	if (pAfter)
	{
		dt = pAfter->m_time - pBefore->m_time;
	}
	if ((pAfter == NULL) || (dt == 0))
	{	//just copy the frame into the supplied buffer
		*val = *pBefore;
	}//if ((pAfter == NULL) || (dt == 0))
	else
	{
		float r = (t - pBefore->m_time) / dt;
		val->interpolate(pBefore, pAfter, r);
	}//else
	return retVal;
}//int	AWAnimatedPoint::linearInterp(AWMesh* val, AWMesh* pBefore, AWMesh* pAfter, float t)



AWMesh*        
AWAnimatedMesh::newMeshBuffer()
{
   AWMesh* retVal = NULL;
   if (m_frames.getNum())
   {
      retVal = (AWMesh*)m_frames[0]->copy();
   }
   return retVal;
}//AWMesh* AWAnimatedMesh::newMeshBuffer()



AWMesh*		 
AWAnimatedMesh::calcMesh(float t, AWMesh* buf)
{
	AWMesh* retVal = NULL;
	//should really -
	//find frames that bracket the supplied time
	//if buffering, then interpolate between them and copy to buffer
	//
	//otherwise return the lower bounding frame for discrete mesh deltas
   //for # frames = 1, there's no calculation to be done.
   //buffer (if any) has already been 'primed' via 'setAnimation'
   //or m_curMesh set to correct value if no buffering
   if (1==m_frames.getNum() && !buf)
   {
      retVal = getMesh(0);
   }
   else if (m_frames.getNum())
	{
		float endTime = ((AWMesh*)m_frames[m_frames.getNum()-1])->m_time;
		if (endTime) t = (float)fmod(t, endTime);
		// Step through position list looking for pair of keys that
		// bracket this t value (or an exact match)
		AWMesh* pAfter  = NULL;
		AWMesh* pBefore = (AWMesh*)m_frames[0];
		if (pBefore->m_time > t)
		{
			;
		}//if (pBefore->m_time > t)
		else
		{
			int i = 1;
			while (i < m_frames.getNum()) 
			{
				pAfter = (AWMesh*)m_frames[i++];
				if ((pBefore->m_time <= t) && (pAfter->m_time >= t)) 
				{
					break;
				}
				pBefore = pAfter;
			}//while (i < m_frames.getNum())
		}//else	
		if (buf)
		{	//buffered version interpolates between keyframes
			//then copies the results into the supplied buffer
			//which is then returned as the result
			linearInterp(buf, pBefore, pAfter, t);
			retVal = buf;
			//special case - if buffered, but there's only one frame, then
			//we can safely copy over the listID at this time
			//right now if one AWAnimatedMesh on an object is buffered, they
			//all have to be, even if there's just one frame
			if (1 == m_frames.getNum())
			{
				buf->setID(pBefore ? pBefore->getID() : AW_NOLIST);
			}
			else buf->setID(AW_NOLIST);
		}
		else
		{	//unbuffered version returns pointer to frame in m_frames
			retVal = pBefore;	 
		}
	}//if (m_frames)	
	return retVal;
}//AWMesh* AWAnimatedMesh::calcMesh(float t,  AWMesh* buf)



//persistence for AWMesh
void    		
AWMesh::restoreGuts(AWFile& file)
{  //clean up any existing data
	m_verts.setSize(0);
   m_colVerts.setSize(0);
   m_norms.setSize(0);
   m_tVerts.setSize(0);
   m_indeces.setSize(0);
	//read the body
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWMesh_4 >= version)
	{
		AWPersistable::restoreGuts(file);
		file.Read(m_time);
//read vertices
		m_verts.read(file);		
//read colour verts, if any
//versions 2 and greater store directly					
		if (VER_AWMesh_2 <= version)
		{
         m_colVerts.read(file);
		}
		else if (VER_AWMesh_1 == version)
		{	//convert from older file format that stores 3 floats for 
			//each vertex colour
         AWList<AWPoint> tempVerts;
         AWColVertex tColVert;
			tempVerts.read(file);
			for (int i=0; i<tempVerts.getNum(); i++)
			{               
            tColVert.x = (unsigned int)(255.0f * tempVerts[i].x);
            tColVert.y = (unsigned int)(255.0f * tempVerts[i].y);
            tColVert.z = (unsigned int)(255.0f * tempVerts[i].z);
				m_colVerts.add(tColVert);
			}
         m_colVerts.pack(); //necessary after multiple 'adds'; auto for 'read'
		}//else if (VER_AWMesh_1 == version)
      
      if (version >= VER_AWMesh_3)
      {  //ver 3 or higher...
         m_norms.read(file);
         if (VER_AWMesh_3 == version)
         {
            AWList < AWList<AWTVert>* > testMe;
            file >> testMe;
            if (testMe.getNum() && testMe[0])
            {
               AWTVertexList* tmp2 = new AWTVertexList;
               tmp2->shallowCopyOf(*testMe[0]);
               tmp2->setOwnsData(testMe[0]->getOwnsData());
               testMe[0]->setOwnsData(FALSE);
               m_tVerts.add(tmp2);
            }
            m_tVerts.pack();
         }//if (VER_AWMesh_3 == version)
         else
         {
            m_tVerts.read(file);
         }
         m_indeces.read(file); 
      }//if (VER_AWMesh_3 == version)

	}//if (VER_AWMesh_3 <= version)
}//void AWMesh::restoreGuts(AWFile& file)



void    		
AWMesh::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file.Write(m_time);
//note that a frame always owns its *vertex* data
   m_verts.write(file);
//the following items require special treatment as
//mesh does *not* necessarily own this data - sole case
//is animated mesh collection where colVerts (for example) 
//don't change between frames
	m_colVerts.write(file);
   m_norms.write(file);
   m_tVerts.write(file);
   m_indeces.write(file); 
}//void AWMesh::saveGuts(AWFile& file) const




AWspace 
AWMesh::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		sizeof(m_time) +
		m_verts.getStoreSize() +
		m_colVerts.getStoreSize() +
      m_norms.getStoreSize() +
      m_tVerts.getStoreSize() +
      m_indeces.getStoreSize();

   for (int i=0; i< m_tVerts.getNum(); i++)
   {
      retVal += m_tVerts[i]->getStoreSize();
   }

	return retVal;
}//AWspace AWMesh::binaryStoreSize() const 



//members for AWAnimatedMesh
void    		
AWAnimatedMesh::restoreGuts(AWFile& file)
{
	//clean up any existing vertex data
	m_frames.destroy();
	//read the body
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (version <= VER_AWAnimatedMesh_2)
	{
		AWPersistable::restoreGuts(file);
      m_frames.read(file, version == VER_AWAnimatedMesh_1);
      postLoadProcess();
      AWTRACE1("  AnimatedMesh loaded with %i frames\n", (int)m_frames.getNum());
	}//if (version <= VER_AWAnimatedMesh_2)
}//void AWAnimatedMesh::restoreGuts(AWFile& file)


void
AWAnimatedMesh::postLoadProcess()
{  //now post-process the meshes collection, taking care of 
   //sharing of non-animated data members.
   //each mesh definitely owns vertex data, but 
   //may share: m_colVerts, m_norms, m_tVerts, m_indeces
   if (!m_frames.getNum()) return;
   //grab the starting frame that *should* own all the data
   AWMesh& startFrame = *(AWMesh*)m_frames[0];
   //now iterate through and ensure that all subsequent meshes use
   //the data from the first frame in absence of 'real' data of own
   for (int i=1; i<m_frames.getNum(); i++)
   {
      AWMesh& nextMesh = *(AWMesh*)m_frames[i];
      //perhaps don't need the test to see if start frame owns its
      //data - shouldn't make any difference
      if (!nextMesh.m_norms.getNum())//&& startFrame.m_norms.m_ownsData)
      {
         nextMesh.m_norms.shallowCopyOf(startFrame.m_norms);
      }
      if (!nextMesh.m_colVerts.getNum() )//&& startFrame.m_colVerts.m_ownsData)
      {
         nextMesh.m_colVerts.shallowCopyOf(startFrame.m_colVerts);
      }
      if (!nextMesh.m_tVerts.getNum())// && startFrame.m_tVerts.m_ownsData)
      {
         nextMesh.m_tVerts.shallowCopyOf(startFrame.m_tVerts);
      }
      if (!nextMesh.m_indeces.getNum())// && startFrame.m_indeces.m_ownsData)
      {
         nextMesh.m_indeces.shallowCopyOf(startFrame.m_indeces);
      }
      nextMesh.init(0.0f);
   }//for (int i=1; i<m_frames.getNum(); i++)
}//void AWAnimatedMesh::postLoadProcess()




void    		
AWAnimatedMesh::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	//note that a frame always owns its vertex data
	file << m_frames;
}//void AWAnimatedMesh::saveGuts(AWFile& file) const



AWspace 
AWAnimatedMesh::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		m_frames.getStoreSize();
	return retVal;
}//AWspace AWAnimatedMesh::binaryStoreSize() const 