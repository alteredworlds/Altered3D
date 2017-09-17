#ifndef __AWFACTORY_H__
#define __AWFACTORY_H__

#include "AWToolDefs.h"
#include "AWList.h"

// AWFactory --- can create an instance of a registered class

class AWExport AWPersistable;

//function to create an AWPersistable with no parameters
typedef AWPersistable*  (*AWcreatorFunction)();


//struct linking classID to creation function
class AWExport AWClassIDAssociation
{
public:
   USES_AWFILEIO(AWClassIDAssociation);

   AWClassID		    m_classID;		// key
   AWcreatorFunction  m_fnPtr;

   AWClassIDAssociation();
   AWClassIDAssociation(AWcreatorFunction uc, AWClassID id);

   int operator==(const AWClassIDAssociation& other) const;
   virtual int    read(AWFile& file)         {return 0;}
   virtual int    write(AWFile& file) const  {return 0;}
};//struct AWExport AWClassIDAssociation 


typedef AWExport AWList<AWClassIDAssociation> AWFactoryList;


#pragma warning(disable: 4275)
class AWExport AWFactory : public AWFactoryList 
{ 
public:
  AWFactory();
  ~AWFactory(); 

  void                  addFunction(AWcreatorFunction, AWClassID);
  AWPersistable*        create(AWClassID); 
  AWcreatorFunction         getFunction(AWClassID);
  void                  removeFunction(AWClassID);
};
#pragma warning(default: 4275)

 AWExport  AWFactory*   getAWFactory();


extern AWFactory* theFactory; // The one-of-a-kind global factory.



inline AWClassIDAssociation::AWClassIDAssociation()
: m_classID(0),  m_fnPtr(NULL)
{
}


inline AWClassIDAssociation::AWClassIDAssociation(AWcreatorFunction uc, AWClassID id)
    : m_classID(id),  m_fnPtr(uc) 
{ }


inline int 
AWClassIDAssociation::operator==(const AWClassIDAssociation& other) const
{
   return m_classID == other.m_classID;
}//inline int AWClassIDAssociation::operator==(const AWClassIDAssociation& other) const


#endif // sentinel __AWFACTORY_H__ 