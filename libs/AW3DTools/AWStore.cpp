// Definitions related to store table and read table 

#include "AWStore.h"


static AWPersistenceTable*  theWriteTable = NULL;
static AWPersistenceTable*  theReadTable  = NULL;


//WRITE AND READ TABLE MANAGEMENT ROUTINES

//	AWPersistenceTable definitions

AWPersistenceTable::AWPersistenceTable() : AWPersistEntryList()
{
   add(AWPersistEntry(AWnilPersistable, 0));
}



AWPersistenceTable::~AWPersistenceTable() 
{ 
}


int                         
AWPersistenceTable::append(AWPersistable* p)
{
   AWPersistEntry pe(p, -1);
   int retVal = add(pe);
   m_element[retVal].m_objectNumber = retVal;
   //AWTRACE2("StoreTable::append(%i) obj num %i\n", (int)p, (int)retVal);
   return retVal;
}//int AWPersistenceTable::append(AWPersistable* p)


int
AWPersistenceTable::append(AWPersistEntry& p) 
{ 
   int retVal = add(p);
   m_element[retVal].m_objectNumber = retVal;
   return retVal;
}//AWPersistEntry* AWPersistenceTable::append(AWPersistEntry* p)



AWBoolean
AWPersistenceTable::append(const void* item, int& objectNum)
{
   AWPersistEntry temp(item, 0);	// Dummy to use as key.
   int index = find(temp);
   if (AW_NPOS != index)
   {
      objectNum = m_element[index].m_objectNumber;
      return FALSE;
   }
   else 
   {
      index = append(temp);
      objectNum = m_element[index].m_objectNumber;
      return TRUE;
   }
}//AWBoolean AWPersistenceTable::add(const void* item, int& objectNum)



//AWUseReadTable definitions 

AWUseReadTable::AWUseReadTable() 
{
   m_firstUse = FALSE;	// Detect whether this recursion is the first
   m_table = AWUseReadTable::getReadTable();

   if(!m_table) 
   {
      m_table = AWUseReadTable::newReadTable();
      m_firstUse = TRUE;
   }
}//AWUseReadTable::AWUseReadTable() 



AWUseReadTable::~AWUseReadTable() 
{
   if (m_firstUse) 
   {
      AWUseReadTable::freeReadTable();
   }
}//AWUseReadTable::~AWUseReadTable() 


//static functions
AWPersistenceTable* 
AWUseReadTable::getReadTable()
{
   return theReadTable;
}


AWPersistenceTable* 
AWUseReadTable::newReadTable()
{
   theReadTable = new AWPersistenceTable;
   return theReadTable;
}


void 
AWUseReadTable::freeReadTable()
{
   delete theReadTable;
   theReadTable = NULL;
}


AWUseWriteTable::AWUseWriteTable() 
{
   m_firstUse = FALSE;	// Detect whether this recursion is the first
   m_table = AWUseWriteTable::getPersistanceTable();

   if(!m_table) 
   {
      m_table = AWUseWriteTable::newPersistanceTable();
      m_firstUse = TRUE;
   }
}//AWUseWriteTable::AWUseWriteTable() 



AWUseWriteTable::~AWUseWriteTable() 
{
   if (m_firstUse) 
   {
      AWUseWriteTable::freePersistanceTable();
   }
}//AWUseWriteTable::~AWUseWriteTable() 


//static functions
AWPersistenceTable* 
AWUseWriteTable::getPersistanceTable()
{
   return theWriteTable;
}


AWPersistenceTable* 
AWUseWriteTable::newPersistanceTable()
{
   theWriteTable = new AWPersistenceTable;
   return theWriteTable;
}


void 
AWUseWriteTable::freePersistanceTable()
{
   delete theWriteTable;
   theWriteTable = NULL;
}
