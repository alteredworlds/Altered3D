// Definitions for the abstract base class AWPersistable
#include "AWFactory.h"
#include "AWToolsCommon.h"
#include "AWPersistable.h"

#include "AWFile.h"
#include "AWStore.h"



// constants for persistence
// AWFile start of file 
const long         AWPersistable::m_startToken  = 0x1111;
const long         compatibleStartToken = 0x8888;

// AWFile: next item is reference, not classID
const AWClassID    AWPersistable::m_existsToken = 0xFFFF;


static AWPersistable* theNilPersistable = NULL;


awexport AWPersistable*  getAWNilPersistable()
{
  if (!theNilPersistable) theNilPersistable = new AWPersistable;
  return theNilPersistable;
}
                                 

AWDEFINE_PERSISTABLE(AWPersistable, CLSID_AWPERSISTABLE)


AWPersistable::AWPersistable() { }
AWPersistable::~AWPersistable() { }


AWspace
AWPersistable::binaryStoreSize() const {return 0;}



AWBoolean
AWPersistable::isEqual(const AWPersistable* c) const 
{ 
   return this==c; 
}


int
AWPersistable::compareTo(const AWPersistable* c) const
{
  return this==c ? 0 : (this>c ? 1 : -1);
}


void
AWPersistable::saveVersion(AWFile& f, unsigned int unVersion)
{
   f << unVersion;
}


void
AWPersistable::restoreVersion(AWFile& f, unsigned int* pVersion)
{
   f >> *pVersion;
}


AWspace
AWPersistable::recursiveStoreSize() const
{
   AWspace total = 0;
   int objectNum;

   AWUseWriteTable storeTable; 
   if (storeTable.firstUse())
   {// check for first recursion
      total =  sizeof(m_startToken);
      total += sizeof(unsigned int); //version
   }

   if(storeTable.append(this, objectNum)) 
   {
      total += sizeof(AWClassID) + binaryStoreSize();
   }
   else
   {
      total += sizeof(m_existsToken) + sizeof(objectNum);
   }

   return total;
}//AWspace AWPersistable::recursiveStoreSize() const



void
AWPersistable::recursiveSaveOn(AWFile& file) const
{
   int objectNum;  
   AWUseWriteTable storeTable;
   if(storeTable.firstUse())	// check for first recursion
   {
      file.Write(m_startToken);
   }
   if(storeTable.append(this, objectNum))
   {
      AWClassID clid = isA();
      file.Write(clid);
      saveGuts(file);		// Write the guts of the object.
   }//if(storeTable.add(this, objectNum))
   else 
   {
      file.Write(m_existsToken);	// Object has been previously stored,
      file.Write(objectNum);	   // so write flag and object number.
   }//else
}//void AWPersistable::recursiveSaveOn(AWFile& file) const



AWPersistable*
AWPersistable::recursiveRestoreFrom(AWFile& file, AWPersistable* obj)
{
   int            objectNum;
   AWClassID      clID;

   AWUseReadTable readTable;
   if(readTable.firstUse())
   {
      long isFileStart;
      file.Read(isFileStart);
      if(file.Eof())       return awnil;
      if(file.Error())     return awnil; // leaving error state in AWFile
      if((m_startToken != isFileStart)&&(compatibleStartToken!=isFileStart)) throw;
   }//if(readTable.firstUse())

   // Get the class ID.
   file.Read(clID);
   if(file.Eof() || file.Error())
   {
      return awnil; // leaving error state in AWFile
   }

   // Check to see if it's the special flag, signally a previously read object:
   if(clID == m_existsToken)
   {
      // Object has already been read.  In this case, the user cannot
      // have supplied an address, unless it matches what is in the
      // readTable.
      file.Read(objectNum);
      if(file.Eof() || file.Error())
      {
         return awnil; // leaving error state in AWFile
      }

      // Check for a bad object number or an object address
      // that does not match what is in the readTable:
      if ( objectNum >= readTable.getNum() ||
         (obj && obj != (const void*)readTable[objectNum].m_item) )
      {
         throw;
      }
      else 
      {
         obj = (AWPersistable*)readTable[objectNum].m_item;
      }

      // Detect the nil object and convert it to a nil pointer:
      if (obj==AWnilPersistable)	
      {
         obj = awnil;
      }
   }
   else
   {   // Object has not been read.
      // If the user has not passed in a preallocated object, 
      // ask the factory to make one for us, given the class ID:
      if (!obj)
      {
         obj = awCreateFromFactory(clID);
      }
      if (obj)
      {
         readTable.append(obj);		// All is ok.  Add to read table...
         obj->restoreGuts(file);		// ... then restore its internals
      }
      else
      {  // Oops.  We don't know how to make an object of this type.
         // Throw an exception:
         throw;
      }
   }
   return obj;
}//AWPersistable* AWPersistable::recursiveRestoreFrom(AWFile& file, AWPersistable* obj)


void
AWPersistable::tryRecursiveRestore(AWFile& file, AWPersistable*& obj)
{
   int            objectNum;
   AWClassID      clID;

   AWUseReadTable readTable;
   if(readTable.firstUse())
   {
      long isFileStart;
      file.Read(isFileStart);
      if(file.Eof())       return;
      if(file.Error())     return;
      if ( (m_startToken!=isFileStart)&&(compatibleStartToken!=isFileStart) ) throw;
   }

   // Get the class ID.
   file.Read(clID);
   if(file.Eof() || file.Error()) return;

   // Check to see if it's the special flag, signally a previously read object:
   if(clID == m_existsToken)
   {  // Object has already been read.  In this case, the user cannot
      // have supplied an address, unless it matches what is in the
      // readTable.
      file.Read(objectNum);
      if(file.Eof() || file.Error()) return;

      // Check for a bad object number or an object address
      // that does not match what is in the readTable:
      if (objectNum >= readTable.getNum() ||
            (obj && obj != (AWPersistable*)readTable[objectNum].m_item) )
      {
         throw;
      }
      else
      {
         obj = (AWPersistable*)readTable[objectNum].m_item;
      }
      // Detect the nil object and convert it to a nil pointer        
      if (obj==AWnilPersistable)	
      {
         obj = awnil;
      }
   }
   else
   {  // Object has not been read.
      // If the user has not passed in a preallocated object, 
      // ask the factory to make one for us, given the class ID:
      if (!obj)
      {
         obj = awCreateFromFactory(clID);
      }
      if (obj)
      {
         readTable.append(obj);		// All is ok.  Add to read table...
         obj->restoreGuts(file);		// ... then restore its internals
      }
      else
      {  // Oops.  We don't know how to make an object of this type.
         // Throw an exception:
         throw;
      }
   }
}//void AWPersistable::tryRecursiveRestore(AWFile& file, AWPersistable*& obj)


awexport AWFile&
operator<<(AWFile& file, const AWPersistable* p)
{
   if (p)
   {
      p->recursiveSaveOn(file);
   }
   else
   {
      AWnilPersistable->recursiveSaveOn(file);
   }
   return file;
}//awexport AWFile& operator<<(AWFile& file, const AWPersistable* p)


void
AWPersistable::restoreGuts(AWFile&) { }

void
AWPersistable::saveGuts(AWFile&) const { }
