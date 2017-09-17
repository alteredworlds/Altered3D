#ifndef AW_GENERIC_LIST_OF_POINTERS_H
#define AW_GENERIC_LIST_OF_POINTERS_H

//  A generic template list class - contains POINTERS to type  
//  Fairly typical of the list example you would 
//  find in any c++ book.

#include <stdio.h>
#include "AWToolDefs.h"
#include "AWFile.h"

#define AW_PTRLIST_OWNS_OBJECTS     0x00000001
#define AW_PTRLIST_OWNS_LIST        0x00000010

#define AW_PTRLIST_NOT_OWNS_OBJECTS 0x11111110
#define AW_PTRLIST_NOT_OWNS_LIST    0x11111101

template <class Type> class AWExport AWPtrList
{
public:
   USES_AWFILEIO(AWPtrList<Type>);

   AWPtrList(int s=0, int ownsData=AW_PTRLIST_OWNS_OBJECTS|AW_PTRLIST_OWNS_LIST);
   virtual ~AWPtrList();
   //DATA OWNERSHIP
   //a pointer list may or may not own:
   //(i)  the contained objects
   //(ii) the array that contains the pointers
   const int         getOwnsObjects()const      
                        {return m_ownsData&AW_PTRLIST_OWNS_OBJECTS;}
   void              setOwnsObjects(int v) 
                        {if (v) m_ownsData|=AW_PTRLIST_OWNS_OBJECTS; 
                         else   m_ownsData&=AW_PTRLIST_NOT_OWNS_OBJECTS;}

   const int         getOwnsList()const        
                        {return m_ownsData&AW_PTRLIST_OWNS_LIST;}
   void              setOwnsList(int v)      
                        {if (v) m_ownsData|=AW_PTRLIST_OWNS_LIST; 
                         else   m_ownsData&=AW_PTRLIST_NOT_OWNS_LIST;}

   void              destroy();
   void	            allocate(int s);
   void	            setSize(int s);
   void	            pack();
   int 	            add(Type);
   int	            addUnique(Type);
   int 	            contains(Type);
   virtual int       find(Type t);
   void	            remove(Type);
   void	            delIndex(int i);
   virtual const int getNum() const    {return m_num;}
   const int         getSize() const   {return m_size;}
   Type*             getList() const   {return m_element;}
   //modifies THIS list to be a shallow copy of o
   const AWPtrList&  shallowCopyOf(const AWPtrList& o);
   AWPtrList&        fillWith(Type a, unsigned int num);

   Type&             operator[](int i) {return m_element[i];}
   const Type&       operator[](int i)const {return m_element[i];}
   AWPtrList<Type>&  operator=(const AWPtrList<Type>& other);
   int               operator==(const AWPtrList<Type>& other) const;
 
   virtual size_t    getStoreSize() const;
   virtual int       read(AWFile& file);
   virtual int       write(AWFile& file) const;

#if 1 //#ifdef _WIN32
   friend AWFile& operator>>(AWFile& file, AWPtrList<Type>& obj);
   friend AWFile& operator<<(AWFile& file, const AWPtrList<Type>& obj);

   friend AWFile& operator>>(AWFile& file, AWPtrList<Type>*& obj);
   friend AWFile& operator<<(AWFile& file, const AWPtrList<Type>* obj);
#else
   friend AWFile& operator>><Type>(AWFile& file, AWPtrList<Type>& obj);
   friend AWFile& operator<< <Type>(AWFile& file, const AWPtrList<Type>& obj);

   friend AWFile& operator>><Type>(AWFile& file, AWPtrList<Type>*& obj);
   friend AWFile& operator<< <Type>(AWFile& file, const AWPtrList<Type>* obj);
#endif
protected:
   int               m_ownsData;
   Type*	            m_element;
   int		         m_num;
   int		         m_size; 
};//template <class Type> class AWPtrList 



template <class Type>
AWPtrList<Type>::AWPtrList(int s, int ownsData) : m_ownsData(ownsData)
{
	m_num      = 0;
	m_size     = 0;
	m_element  = NULL;
	if (s) allocate(s);
}//template <class Type> AWPtrList<Type>::AWPtrList(int s)



template <class Type>
AWPtrList<Type>::~AWPtrList()
{
	destroy();
}



template <class Type>
inline void AWPtrList<Type>::destroy()
{	
   //whole point of POINTER LIST - delete contained objects
   for (int i=0; m_element&&i<getNum(); i++)
   {
      if (getOwnsObjects()) 
      {
         delete m_element[i];
      }//if (getOwnsObjects()) 
      if (getOwnsList()) m_element[i] = NULL;
   }//for (i=0; i<getNum(); i++)
   if (getOwnsList())
   {
      delete [] m_element;
   }//if (getOwnsList())
   m_element = NULL;
   m_num      = 0;
	m_size     = 0;
}//template <class Type> inline void AWPtrList<Type>::destroy()



template <class Type>
inline int AWPtrList<Type>::operator==(const AWPtrList<Type>& other) const
{
   int retVal = m_num==other.m_num;
   for (int i=0; retVal && i<m_num; i++)
   {
      retVal &= m_element[i] == other[i];
   }
   return retVal;
}//int AWPtrList<Type>::operator==(const AWPtrList<Type>& other)



template <class Type>
inline const AWPtrList<Type>& AWPtrList<Type>::shallowCopyOf(const AWPtrList<Type>& o)
{  //copies m_num and the POINTER element list
   setSize(0);
   if (o.getNum())
   {
      m_num       = o.m_num;
      m_size      = o.m_num;
      m_element   = o.m_element;
      setOwnsObjects(FALSE);
      setOwnsList(FALSE);
   }//if (o.getNum())
   return *this;
}//const AWPtrList<Type>& AWPtrList<Type>::shallowCopyOf(const AWPtrList<Type>& o)



template <class Type>
inline AWPtrList<Type>& AWPtrList<Type>::operator=(const AWPtrList<Type>& other)
{
	// copy all elements from other into this list.
	// get rid of any existing contents
   // this is ALWAYS a SHALLOW COPY	
   //note that if other.m_num size == m_size nothin is done
   //if other.m_num is smaller than m_size clear and reallocate
	setSize(other.m_num);
   memcpy(m_element, other.m_element, sizeof(Type) * other.m_num);
   setOwnsObjects(FALSE);
   setOwnsList(TRUE);
	return *this;
}//AWPtrList<Type>& AWPtrList<Type>::operator=(const AWPtrList<Type>& other)



template <class Type>
inline AWPtrList<Type>& AWPtrList<Type>::fillWith(Type a, unsigned int num)
{
   setSize(0);
	for (unsigned int i=0; i<num; i++)
   {
      add(a);
   }//for (int i=0; i<num; i++)
   pack();
	return *this;
}//template <class Type> AWPtrList<Type>& AWPtrList<Type>::fillWith(Type a, unsigned int num)



template <class Type>
inline void AWPtrList<Type>::allocate(int s)
{
   if ( s>0 )
   {
      if (s >= m_num)
      {  //asked to add more (POSITIVE) space than already have
         //or pack existing items
	      Type* old   = m_element;
	      m_size      = s;
	      m_element   = new Type[s];
         if (old)
         {
	         for(int i=0; i<m_num; i++)
            {
		         m_element[i]=old[i];
	         }
	         if (getOwnsList())
            {
               delete [] old;
            }//if (getOwnsList())
         }//if (old)
         setOwnsList(TRUE);
      }//if (s > m_num)
   }//if (s > 0)
}//template <class Type> void AWPtrList<Type>::allocate(int s)



template <class Type>
inline void AWPtrList<Type>::setSize(int s)
{
	if ( !s || (s < m_size) ) 
   { 
      if(m_element && getOwnsList()) 
      {
         delete [] m_element;
      }
      m_element = NULL;
      m_size    = 0;
      m_num     = 0;
   }//if (s==0) 
	if (s) 
   {	
      allocate(s); 
   }//else
	m_num=s;
}//template <class Type> void AWPtrList<Type>::setSize(int s)



template <class Type>
inline void AWPtrList<Type>::pack()
{
	allocate(m_num);
}//template <class Type> void AWPtrList<Type>::pack()



template <class Type>
inline int AWPtrList<Type>::add(Type t)
{  //duplicates allowed
   int retVal = -1;
	if ( m_num <= m_size )
   {
	   if(m_num==m_size) 
      {
		   allocate( (m_size) ? m_size*2 : 16 );
	   }
      retVal = m_num++;
	   m_element[retVal] = t;    
   }//if (m_num<=m_size)
   return retVal;
}//template <class Type> int AWPtrList<Type>::add(Type t)



template <class Type>
inline int AWPtrList<Type>::find(Type t)
{  //requires operator== meaningful for contained items
   //returns -1 if item not found
   //otherwise returns index of FIRST MATCH
	int retVal = AW_NPOS;
	for (int i=0; i<m_num; i++) 
   {  
		if (m_element[i]  == t) 
      {
         retVal = i;
         break;
      }//if (m_element[i] == t)
	}//for (i=0; i<m_num; i++) 
	return retVal;
}//template <class Type> int AWPtrList<Type>::find(Type t)



template <class Type>
inline int AWPtrList<Type>::contains(Type t)
{  //requires operator== meaningful for contained items
   //returns the number of items found that match.   
	int count=0;
	for (i=0; i<m_num; i++) 
   {
		if (m_element[i] == t) 
      {
         count++;
      }//if (m_element[i] == t)
	}//for (i=0; i<m_num; i++) 
	return count;
}//template <class Type> int AWPtrList<Type>::contains(Type t)



template <class Type>
inline int AWPtrList<Type>::addUnique(Type t)
{  //if there's already one present, don't add this one,
   //just return the index of the (first) matching entry
   int retVal = find(t);
   if (AW_NPOS == retVal)
   {  //we need to add one
      retVal = add(t);
   }
   return retVal;
}//template <class Type> void AWPtrList<Type>::addUnique(Type t)



template <class Type>
inline void AWPtrList<Type>::delIndex(int i)
{
	if (i < m_num)
   {
	   m_num--;
	   while (i < m_num)
      {
		   m_element[i] = m_element[i+1];
		   i++;
	   }//while (i < m_num)
   }//if (i<m_num)
}//template <class Type> void AWPtrList<Type>::delIndex(int i)



template <class Type>
inline void AWPtrList<Type>::remove(Type t)
{  //deletes first instance matching via operator==
	int i;
	for (i=0; i<m_num; i++) 
   {
		if (m_element[i] == t) 
      {
			break;
		}
	}//for (i=0; i<m_num; i++) 
	delIndex(i);
}//template <class Type> void AWPtrList<Type>::remove(Type t)



template <class Type>
inline size_t AWPtrList<Type>::getStoreSize() const
{
   return sizeof(m_num) + (getOwnsObjects()) ? sizeof(Type) * m_num : 0;
   //how do we cope with Type == pointer type?
   //need to sum element[i]->getStoreSize()?
}//template <class Type> size_t AWPtrList<Type>::getStoreSize()



template <class Type>
inline int AWPtrList<Type>::read(AWFile& file)
{
   destroy();
   Type a;
   int  numLeftToRead;
   file.Read(numLeftToRead);
   while (numLeftToRead && !file.Eof())
   {
      file >> a;
      add(a);
      numLeftToRead--;
   }//while (numLeftToRead && !file.eof())
   pack();
   return m_num;
}//template <class Type> int AWPtrList<Type>::read(AWFile& file)



template <class Type>
inline int AWPtrList<Type>::write(AWFile& file) const
{
   int numToWrite = getOwnsObjects() ? m_num : 0;
   file.Write(numToWrite);
   for (int i=0; i<numToWrite; i++)
   {
      file << m_element[i];
   }
   return numToWrite;
}//template <class Type> int AWPtrList<Type>::read(AWFile& file, int numToRead)



//reference based insertion & extraction operators
template <class Type>
inline AWFile& operator>>(AWFile& file, AWPtrList<Type>& it)
{
   it.read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWPtrList<Type>& it)



template <class Type>
inline AWFile& operator<<(AWFile& file, const AWPtrList<Type>& it)
{
   it.write(file);
   return file;
}//AWFile& operator<<(AWFile& file, const AWPtrList<Type>& it)



//pointer based insertion and extraction operators
template <class Type>
inline AWFile& operator>>(AWFile& file, AWPtrList<Type>*& it)
{
   it = new AWPtrList<Type>;
   it->read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWPtrList<Type>*& it)



template <class Type>
inline AWFile& operator<<(AWFile& file, const AWPtrList<Type>* it)
{
   if (it) it->write(file);
   return file;
}//AWFile& operator<<(AWFile& file, const AWPtrList<Type>* it)

#endif //sentinel AW_GENERIC_LIST_OF_POINTERS_H