//be very careful that this pragma isn't masking a real problem
//put all this inline and into header later
#include "AWTVertexList.h"


void 
AWTVertexList::delIndex(int i)
{  //deletes the i+1th Texture Vertex
   if (!m_dimension) return;
   //m_num is number of FLOATs in list
   //each Texture Vertex consists of float[m_dimension]
   //index of first float making up this TV is
   int floatIndex = i * m_dimension;
	if (floatIndex < m_num)
   {
	   m_num -= m_dimension;   
	   while (floatIndex < m_num)
      {		   
         for (int k=0; k<m_dimension; k++)
         {
            m_element[floatIndex+k] = m_element[floatIndex+k+m_dimension];
         }
		   floatIndex += m_dimension;
	   }//while (floatIndex < m_num)
   }//if (floatIndex < m_num)
}//void AWTVertexList::delIndex(int i)



const AWTVertexList& 
AWTVertexList::shallowCopyOf(const AWList<AWTVert>& o)
{  //copy over the list data pointer and correct 
   //number of elements and so on.
   //this is legacy collection with float[2] data
   m_element   = (float*)o.getList();
   m_ownsData  = FALSE;
   m_dimension = 2;
   //make sure m_num and m_size correctly reflect the
   // ***NUMBER OF FLOATS*** in the inherited AWList<float>
   m_num     = o.getNum()  * m_dimension;
   m_size    = o.getSize() * m_dimension; 
   return *this;
}//const AWTVertexList& AWTVertexList::shallowCopyOf(const AWList<AWTVert>& o)



//READ and WRITE need optimization

int	
AWTVertexList::write(AWFile& file) const
{
   int numToWrite = m_ownsData ? m_num : 0;
   file.Write(m_dimension);
   file.Write(numToWrite);
   file.Write((float*)m_element, numToWrite);
   return numToWrite;
}//int AWTVertexList::write(AWFile& file) const


int	
AWTVertexList::read(AWFile& file)
{
   destroy();  
   file.Read(m_dimension);
   file.Read(m_num);
   if (m_num)
   {
      allocate(m_num);
      file.Read((float*)m_element, m_num);
   }//if (m_num)
   return m_num;
}//int AWTVertexList::read(AWFile& file)



int	
AWTVertexList::read(AWFile& file, AWBoolean noDimension)
{
	m_dimension = 2;
	return AWFloatList::read(file);
}//int AWTVertexList::read(AWFile& file, AWBoolean noDimension)