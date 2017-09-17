#ifndef __AWPERSISTABLE_H
#define __AWPERSISTABLE_H


#include "AWToolDefs.h"
#include "AWFactory.h"
// Declarations for AWPersistable: an abstract base class 

extern awexport void           awAddToFactory(AWcreatorFunction, AWClassID);
extern awexport AWPersistable* awCreateFromFactory(AWClassID);
extern awexport void           awRemoveFromFactory(AWClassID);

// miscellany 
extern awexport void           awDestroy(AWPersistable*, void*);
extern awexport AWBoolean      awIsEqualFun(const void* a, const void* b);



// The macro AWDECLARE_PERSISTABLE should be included in the declaration
// of any class that derives from AWPersistable.                        

#define AWDECLARE_PERSISTABLE(className)                             \
  public:                                                           \
    friend class AWInit##className;                                 \
    static AWPersistable*  awCreateFN##className();                 \
    static const AWClassID  m_myID;                                     \
  public:                                                            \
    virtual AWPersistable*  copy() const;                            \
    virtual AWClassID       isA() const;                             \
    friend awexport AWFile& operator>>(AWFile& f,  className*& pCl); \
    inline friend AWFile&   operator<<(AWFile& f,const className& Cl) \
            { Cl.recursiveSaveOn(f); return f; }                      \
    inline friend  AWFile&  operator>>(AWFile& f, className& Cl)      \
            { AWPersistable::recursiveRestoreFrom(f,&Cl); return f; }      

// For each class derived from AWPersistable, EXACTLY ONE macro:      
//   AWDEFINE_PERSISTABLE(className, AWClassID)                      
//                                                                     
// should be included in ONE ".cpp" file to implement several common
// functions for classing deriving from AWPersistable.  These macros  
// serve several purposes:                                             
// 1) To provide a definition for copy().                              
// 2) To provide a definition for isA().                                
// 3) To define a "creator function" to be inserted into the            
//    one-of-a-kind global AWFactory pointed to by theFactory.

#define AWDEFINE_PERSISTABLE(className, id)                 \
const AWClassID className::m_myID = id;                     \
AWPersistable* className::copy() const { return new className(*this); } \
AWClassID className::isA() const  { return m_myID; }       \
AWPersistable* className::awCreateFN##className() \
{ return new className; } \
class AWInit##className {     \
public: AWInit##className();        \
~AWInit##className(); };      \
AWInit##className::AWInit##className() \
{                                          \
   awAddToFactory(className::awCreateFN##className, className::m_myID);\
}            \
AWInit##className::~AWInit##className()  \
{                   \
   awRemoveFromFactory(className::m_myID);  \
}                                  \
AWInit##className constructMeFirst##className;      \
        \
awexport AWFile&                           \
operator>>(AWFile& f, className*& pCl)  \
{AWPersistable* p=0;AWPersistable::tryRecursiveRestore(f,p); pCl = (className*)p;return f;}                 





#define AWDECLARE_VERSIONABLE_PERSISTABLE(className) \
private:                                             \
   static const unsigned int uVersion_;              \
public:                                              \
   virtual unsigned int        getVersion() const;   \
   AWDECLARE_PERSISTABLE(className)

#define AWDEFINE_VERSIONABLE_PERSISTABLE(className, id, nVer)    \
  const unsigned int className::uVersion_ = nVer;                \
  unsigned int className::getVersion() const                     \
    { return uVersion_; }                                        \
  AWDEFINE_PERSISTABLE(className, id)


#define AWDECLARE_VERSIONABLE_PERSISTABLE_ALT(className) \
public:                                              \
   virtual unsigned int        getVersion() const;   \
   AWDECLARE_PERSISTABLE(className)

#define AWDEFINE_VERSIONABLE_PERSISTABLE_ALT(className, id, nVer)    \
  unsigned int className::uVersion_ = nVer;                \
  unsigned int className::getVersion() const                     \
    { return uVersion_; }                                        \
  AWDEFINE_PERSISTABLE(className, id)



//AWPersistable
class AWExport AWPersistable
{
  AWDECLARE_PERSISTABLE(AWPersistable)

public:
  AWPersistable();

// virtual functions
  virtual                       ~AWPersistable();

  virtual AWspace               binaryStoreSize() const;
  virtual int                   compareTo(const AWPersistable*) const;
  virtual AWBoolean             isEqual(const AWPersistable*) const;
  static  inline AWspace        nilStoreSize()
        { return sizeof(AWClassID) + sizeof(int); }
  virtual void                  restoreGuts(AWFile&);
  virtual void                  saveGuts(AWFile&) const;
  AWspace                       recursiveStoreSize() const;
  static AWPersistable*         recursiveRestoreFrom(AWFile&       file,
                                                     AWPersistable*p = 0);
  static void                   tryRecursiveRestore(AWFile&       file,
                                                     AWPersistable*& p);
  void                          recursiveSaveOn(AWFile&) const;

  virtual unsigned int          getVersion() const { return 0; }
  static void                   saveVersion(AWFile&, unsigned int);
  static void                   restoreVersion(AWFile&, unsigned int*);

private:
  static const long             m_startToken;
  static const AWClassID        m_existsToken; 
};


awexport AWFile& operator<<(AWFile& file, const AWPersistable* p);

extern awexport AWPersistable* getAWNilPersistable();

#define AWnilPersistable (getAWNilPersistable()) 

#endif //sentinel  __AWPERSISTABLE_H 

