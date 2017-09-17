#ifndef __AWSTORETABLE_H__
#define __AWSTORETABLE_H__

//Definitions for internal classes used to store AWPersistables

#include "AWPersistable.h"


extern AWExport AWPersistable* getAWNilPersistable();

#define AWnilPersistable (getAWNilPersistable())


//class linking pointer to object to storage number
class AWExport AWPersistEntry
{
public:
   USES_AWFILEIO(AWPersistEntry)

   const void*   m_item;
   int           m_objectNumber;

   AWPersistEntry();
   AWPersistEntry(const void* c, int n);
   int operator==(const AWPersistEntry& other) const;

   virtual int    read(AWFile& file)         {return 0;}
   virtual int    write(AWFile& file) const  {return 0;}
};//struct AWExport AWPersistEntry 


typedef AWExport AWList<AWPersistEntry> AWPersistEntryList;


//  AWPersistenceTable declarations 
#pragma warning(disable: 4275)
class AWExport AWPersistenceTable : public AWPersistEntryList
{
public:
  AWPersistenceTable();
  ~AWPersistenceTable();

  int                         append(AWPersistable*);
  int                         append(AWPersistEntry&);
  AWBoolean                   append(const void*, int&);
};//class AWExport AWPersistenceTable : public AWPersistEntryList
#pragma warning(default: 4275)


class AWExport AWUseReadTable 
{
  AWBoolean             m_firstUse;
  AWPersistenceTable*   m_table;

  AWUseReadTable(const AWUseReadTable&); // Not defined
  void operator=(const AWUseReadTable&); // Not defined

public:
  AWUseReadTable();
  ~AWUseReadTable();
  AWBoolean          firstUse() const          { return m_firstUse; }

  int                append(AWPersistable* p)  { return m_table->append(p); }

  int                getNum()                  { return m_table->getNum(); }
  AWPersistEntry&    operator[] (int i)        { return (*m_table)[i]; }

  static AWPersistenceTable*  getReadTable();
  static AWPersistenceTable*  newReadTable();
  static void                 freeReadTable();
};//class AWExport AWUseReadTable



class AWExport AWUseWriteTable 
{
  AWBoolean             m_firstUse;
  AWPersistenceTable*   m_table;

  AWUseWriteTable(const AWUseWriteTable&); // Not defined
  void operator=(const AWUseWriteTable&); // Not defined

public:
  AWUseWriteTable();
  ~AWUseWriteTable();
  AWBoolean          firstUse() const          { return m_firstUse; }

  AWBoolean          append(const void* p, int& i){ return m_table->append(p, i); }

  int                getNum()                  { return m_table->getNum(); }
  AWPersistEntry&    operator[] (int i)        { return (*m_table)[i]; }

  static AWPersistenceTable*  getPersistanceTable();
  static AWPersistenceTable*  newPersistanceTable();
  static void                 freePersistanceTable();
};//class AWExport AWUseWriteTable



inline AWPersistEntry::AWPersistEntry()
: m_item(NULL),  m_objectNumber(-1)
{
}


inline AWPersistEntry::AWPersistEntry(const void* c, int n)
    : m_item(c),  m_objectNumber(n) 
{ }


inline int 
AWPersistEntry::operator==(const AWPersistEntry& other) const
{
   return m_item == other.m_item;
}//inline int AWClassIDAssociation::operator==(const AWClassIDAssociation& other) const



#endif  //sentinel  __AWSTORETABLE_H__
