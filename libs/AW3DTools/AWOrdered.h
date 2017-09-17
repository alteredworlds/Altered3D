#ifndef AW_ORDERED_COLLECTION_OF_PERSISTABLES_H
#define AW_ORDERED_COLLECTION_OF_PERSISTABLES_H

#include <stdio.h>
#include "AWToolDefs.h"
#include "AWPtrList.h"
#include "AWStore.h"
#include "AWPersistable.h"


typedef AWExport AWPtrList<AWPersistable*> AWPersistableList;

#pragma warning(disable: 4275)
class AWExport AWOrdered : public AWPersistableList 
{
public:
   AWOrdered(int s=0, int ownsData=AW_PTRLIST_OWNS_OBJECTS|AW_PTRLIST_OWNS_LIST) : AWPersistableList(s, ownsData){;}

   virtual AWPersistable* remove(AWPersistable* r);
   virtual int            find(AWPersistable* t);
 
   virtual size_t getStoreSize() const;

//helper functions to convert from older format
   virtual int    write(AWFile& file) const;
   virtual int    readUniquePtr(AWFile& file);
   virtual int    read(AWFile& file,      AWBoolean convOldFmt);
};//class AWExport AWOrdered : public AWPersistableList 


inline int
AWOrdered::find(AWPersistable* t)
{  //requires operator== meaningful for contained items
   //returns -1 if item not found
   //otherwise returns index of FIRST MATCH
	int retVal = AW_NPOS;
	for (int i=0; i<m_num; i++) 
   {  //because of the specialized nature of this list, we know that
      //all contained items are derived from AWPersistable - compare
      //via virtual fn isEqual
		if (m_element[i] && m_element[i]->isEqual(t)) 
      {
         retVal = i;
         break;
      }//if (m_element[i] == t)
	}//for (i=0; i<m_num; i++) 
	return retVal;
}//inline int AWOrdered::find(AWPersistable* t)


inline AWPersistable* 
AWOrdered::remove(AWPersistable* r)
{
   AWPersistable* retVal = NULL;
	if (r)
	{
      int index = find(r);
      if (AW_NPOS != index)
      {
         retVal = (AWPersistable*)m_element[index];
         delIndex(index);
      }//if (AW_NPOS != index)
	}//if (r)
	return retVal;
}//inline AWPersistable* AWOrdered::remove(AWPersistable*)



inline size_t 
AWOrdered::getStoreSize() const
{  //we know that this list contains AWPersistables - so
   //get the full recursive store size with this call
   size_t retVal = AWPersistableList::getStoreSize();
   for (int i=0; i<getNum(); i++)
   {
      if (m_element[i])
      {
         retVal += m_element[i]->recursiveStoreSize();
      }//if (m_element[i])
   }//for (int i=0; i<getNum(); i++)
   return retVal;
}//inline size_t AWOrdered::getStoreSize() const



inline int 
AWOrdered::readUniquePtr(AWFile& file)
{  //SPECIAL CASE- we know that this collection, saved as
   //?WOrdered* - may: 
   //(i) not be present at all (NULL POINTER => empty collection)
   //(ii) may be present, but will not be shared with any other
   //object - ie. will NOT have been read before
   AWPersistable* a     = NULL;
   size_t         num;
   unsigned short classID; 
   int            dummy = 0;

   destroy();

   file.Read(classID);
   if (0xFFFF == classID)
   {  //object has been read before - which in this case
      //means that it's NULL, in fact...
      file.Read(dummy); //reading index into read table - should be 0
   }//if (AWPersistable::fileRefFlag == classID)
   else
   {
      AWUseReadTable storeTable;

      file.Read(num);
      storeTable.append(AWnilPersistable);
      while (num && !file.Eof())
      {
         file >> a;
         add(a);
         num--;
      }//while (numLeftToRead && !file.eof())
      pack();
   }//else
   return getNum();  
}//inline int AWOrdered::readUniquePtr(AWFile& file)



inline int 
AWOrdered::read(AWFile& file, AWBoolean convOldFmt)
{
   AWPersistable* a = NULL;
   size_t         num;
   int  numLeftToRead;
   destroy();
   if (convOldFmt)
   {  //read in initial junk that is now to be discarded    
      unsigned short classID; 
      int dummy = 0;
      file.Read(classID);
      file.Read(num);
      AWUseReadTable storeTable;
      storeTable.append(AWnilPersistable);
      numLeftToRead = num;
   }
   else
   {   
      file.Read(numLeftToRead);
   }
   while (numLeftToRead && !file.Eof())
   {
      file >> a;
      add(a);
      numLeftToRead--;
   }//while (numLeftToRead && !file.eof())
   pack();
   return m_num;
}//inline int AWOrdered::read(AWFile& file, AWBoolean convOldFmt)



inline int AWOrdered::write(AWFile& file) const
{
   //for AWOrdered, we always write the contents.
   //the contained AWPersistable takes care of writing
   //just a reference if this object has been written before
   file.Write(m_num);
   for (int i=0; i<m_num; i++)
   {
      file << m_element[i];
   }
   return m_num;
}//int AWOrdered::write(AWFile& file)



#pragma warning(default: 4275)
#endif //sentinel #define AW_ORDERED_COLLECTION_OF_PERSISTABLES_H