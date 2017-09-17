#include "AWFactory.h"


AWFactory* theFactory = NULL; // Pointer to the one-of-a-kind global factory



AWFactory::AWFactory()
{
}


AWFactory::~AWFactory()
{
}
  

void
AWFactory::addFunction(AWcreatorFunction uc, AWClassID id)
{  //only add if not already present
   addUnique( AWClassIDAssociation(uc, id) );
}//void AWFactory::addFunction(AWcreatorFunction uc, AWClassID id)


void
AWFactory::removeFunction(AWClassID id)
{
   remove( AWClassIDAssociation(NULL, id) );
}



AWcreatorFunction
AWFactory::getFunction(AWClassID id)
{
   AWcreatorFunction retVal = NULL;
   AWClassIDAssociation dummy(NULL, id);
   int index = find(dummy);
   if (AW_NPOS != index)
   {
      retVal = m_element[index].m_fnPtr;
   }
   return retVal;
}//AWcreatorFunction AWFactory::getFunction(AWClassID id)



AWPersistable*
AWFactory::create(AWClassID id)
{
  AWcreatorFunction fn = getFunction(id);	// Get the creator function
  return fn ? (*fn)() : NULL;		// Invoke it if one was found.
}


//FUNCTION INTERFACE
awexport AWFactory*  getAWFactory()
{
  if (theFactory==NULL) 
  {
     theFactory = new AWFactory;
  }
  return theFactory;
}//AWFactory* awexport getAWFactory()



void awDeleteFactory()
{
  delete theFactory;
  theFactory = NULL; 
}//void awDeleteFactory()



awexport void
awAddToFactory(AWcreatorFunction fn, AWClassID id)
{
  AWFactory* f = getAWFactory();
  if (f) f->addFunction(fn, id);
}//void awexport awAddToFactory(AWcreatorFunction fn, AWClassID id)



awexport AWPersistable* 
awCreateFromFactory(AWClassID id)
{
  AWPersistable* ret = NULL;

  AWFactory* f = getAWFactory();
  if (f) 
  {
    ret = f->create(id);
  }//if (f) 
  return ret;
}//AWPersistable* awexport awCreateFromFactory(AWClassID id)



awexport void
awRemoveFromFactory(AWClassID id)
{
   AWFactory* f = getAWFactory();
   if (f) 
   {
      f->removeFunction(id);
      if (!f->getNum()) 
      {
         awDeleteFactory();
      }
   }//if (f) 
}//void awexport awRemoveFromFactory(AWClassID id)
