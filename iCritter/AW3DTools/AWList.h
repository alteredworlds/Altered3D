#ifndef AW_GENERIC_LIST_H
#define AW_GENERIC_LIST_H

//  A generic template list class.  
//  Fairly typical of the list example you would 
//  find in any c++ book.

#include <stdio.h>
#include "AWToolDefs.h"
#include "AWFile.h"
#include <memory.h>


#define AW_NPOS -1


template <class Type> class AWExport AWList
{
public:
   USES_AWFILEIO(AWList<Type>);

   AWList(int s=0, int ownsData=1);
   virtual ~AWList();

   void              destroy();
   void	            allocate(int s);
   void	            setSize(int s);
   void	            pack();
   int 	            add(Type);
   int	            addUnique(Type);
   int 	            contains(Type);
   int               find(const Type t);
   void	            remove(Type);
   virtual void	   delIndex(int i); 

   const int         getOwnsData()     {return m_ownsData;}
   void              setOwnsData(int v){m_ownsData=v;}

   virtual const int getNum() const    {return m_num;}
   const int         getSize() const   {return m_size;}
   Type*             getList() const   {return m_element;}
   //modifies THIS list to be a shallow copy of o
   const AWList<Type>&  shallowCopyOf(const AWList<Type>& o);
   AWList<Type>&     fillWith(Type a, unsigned int num);

   Type&             operator[](int i) {return m_element[i];}
   const Type&       operator[](int i)const {return m_element[i];}
   AWList<Type>&     operator=(const AWList<Type>& other);
   int               operator==(const AWList<Type>& other) const;
 
   virtual size_t    getStoreSize() const;
   virtual int       read(AWFile& file);
   virtual int       write(AWFile& file) const; 

   template <class U> friend AWFile& operator>>(AWFile& file, AWList<U>& obj);
   template <class U> friend AWFile& operator<<(AWFile& file, AWList<Type>& obj);
   template <class U> friend AWFile& operator>>(AWFile& file, AWList<Type>*& obj);
   template <class U> friend AWFile& operator<<(AWFile& file, const AWList<Type>* obj);

protected:
   int               m_ownsData;
   Type*	            m_element;
   int		         m_num;
   int		         m_size; 
};//template <class Type> class AWList 



template <class Type>
inline AWList<Type>::AWList(int s, int ownsData) : m_ownsData(ownsData)
{
	m_num      = 0;
	m_size     = 0;
	m_element  = NULL;
	if (s) allocate(s);
}//template <class Type> AWList<Type>::AWList(int s)



template <class Type>
inline AWList<Type>::~AWList()
{
	destroy();
}



template <class Type>
inline void AWList<Type>::destroy()
{
	if (m_ownsData) delete [] m_element;
   m_element = NULL;
   m_num      = 0;
	m_size     = 0;
}//template <class Type> inline void AWList<Type>::destroy()



template <class Type>
inline int AWList<Type>::operator==(const AWList<Type>& other) const
{
   int retVal = 1;
   for (int i=0; i<m_num; i++)
   {
      retVal &= m_element[i] == other[i];
   }
   return retVal;
}//int AWList<Type>::operator==(const AWList<Type>& other)



template <class Type>
inline const AWList<Type>& AWList<Type>::shallowCopyOf(const AWList<Type>& o)
{
   //copies m_num and the POINTER element list
   //sets ownsdata to FALSE
   setSize(0);
   if (o.getNum())
   {     
      m_num       = o.m_num;
      m_size      = o.m_num;
      m_element   = o.m_element;
      m_ownsData  = 0;
   }//if (o.getNum())
   return *this;
}//const AWList<Type>& AWList<Type>::shallowCopyOf(const AWList<Type>& o)



template <class Type>
inline AWList<Type>& AWList<Type>::operator=(const AWList<Type>& other)
{
	// copy all elements from other into this list.
	// get rid of any existing contents
   // this is ALWAYS a SHALLOW COPY	
   //note that if other.m_num size == m_size nothin is done
   //if other.m_num is smaller than m_size clear and reallocate
	setSize(other.m_num);
   memcpy(m_element, other.m_element, sizeof(Type) * other.m_num);
	return *this;
}//AWList<Type>& AWList<Type>::operator=(const AWList<Type>& other)



template <class Type>
inline AWList<Type>& AWList<Type>::fillWith(Type a, unsigned int num)
{
   setSize(0);
	for (unsigned int i=0; i<num; i++)
   {
      add(a);
   }//for (int i=0; i<num; i++)
   pack();
	return *this;
}//template <class Type> AWList<Type>& AWList<Type>::fillWith(Type a, unsigned int num)



template <class Type>
inline void AWList<Type>::allocate(int s)
{
   if (s > 0)
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
	         if (m_ownsData)
            {
               delete [] old;
            }
         }
         m_ownsData  = TRUE;
      }//if (s > m_num)
   }//if (s > 0)
}//template <class Type> void AWList<Type>::allocate(int s)



template <class Type>
inline void AWList<Type>::setSize(int s)
{
	if ( !s || (s < m_size) ) 
   { 
      if(m_element && m_ownsData) 
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
}//template <class Type> void AWList<Type>::setSize(int s)



template <class Type>
inline void AWList<Type>::pack()
{
	allocate(m_num);
}//template <class Type> void AWList<Type>::pack()



template <class Type>
inline int AWList<Type>::add(Type t)
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
}//template <class Type> int AWList<Type>::add(Type t)



template <class Type>
inline int AWList<Type>::find(const Type t)
{  //requires operator== meaningful for contained items
   //returns -1 if item not found
   //otherwise returns index of FIRST MATCH
	int retVal = AW_NPOS;
	for (int i=0; i<m_num; i++) 
   {
		if (m_element[i] == t) 
      {
         retVal = i;
         break;
      }//if (m_element[i] == t)
	}//for (i=0; i<m_num; i++) 
	return retVal;
}//template <class Type> int AWList<Type>::find(const Type t)



template <class Type>
inline int AWList<Type>::contains(Type t)
{  //requires operator== meaningful for contained items
   //returns the number of items found that match.   
	int count=0;
	for (int i=0; i<m_num; i++) 
   {
		if (m_element[i] == t) 
      {
         count++;
      }//if (m_element[i] == t)
	}//for (i=0; i<m_num; i++) 
	return count;
}//template <class Type> int AWList<Type>::contains(Type t)



template <class Type>
inline int AWList<Type>::addUnique(Type t)
{  //if there's already one present, don't add this one,
   //just return the index of the (first) matching entry
   int retVal = find(t);
   if (AW_NPOS == retVal)
   {  //we need to add one
      retVal = add(t);
   }
   return retVal;
}//template <class Type> void AWList<Type>::addUnique(Type t)



template <class Type>
inline void AWList<Type>::delIndex(int i)
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
}//template <class Type> void AWList<Type>::delIndex(int i)



template <class Type>
inline void AWList<Type>::remove(Type t)
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
}//template <class Type> void AWList<Type>::remove(Type t)



template <class Type>
inline size_t AWList<Type>::getStoreSize() const
{
   return sizeof(m_num) + (m_ownsData) ? sizeof(Type) * m_num : 0;
   //how do we cope with Type == pointer type?
   //need to sum element[i]->getStoreSize()?
}//template <class Type> size_t AWList<Type>::getStoreSize()



template <class Type>
inline int AWList<Type>::read(AWFile& file)
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
}//template <class Type> int AWList<Type>::read(AWFile& file)



template <class Type>
inline int AWList<Type>::write(AWFile& file) const
{
   int numToWrite = m_ownsData ? m_num : 0;
   file.Write(numToWrite);
   for (int i=0; i<numToWrite; i++)
   {
      file << m_element[i];
   }
   return numToWrite;
}//template <class Type> int AWList<Type>::read(AWFile& file, int numToRead)



//reference based insertion & extraction operators
template <class Type>
inline AWFile& operator>>(AWFile& file, AWList<Type>& it)
{
   it.read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWList<Type>& it)



template <class Type>
inline AWFile& operator<<(AWFile& file, const AWList<Type>& it)
{
   it.write(file);
   return file;
}//AWFile& operator<<(AWFile& file, const AWList<Type>& it)



//pointer based insertion and extraction operators
template <class Type>
inline AWFile& operator>>(AWFile& file, AWList<Type>*& it)
{
   it = new AWList<Type>;
   it->read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWList<Type>*& it)



template <class Type>
inline AWFile& operator<<(AWFile& file, const AWList<Type>* it)
{
   if (it) it->write(file);
   return file;
}//AWFile& operator<<(AWFile& file, const AWList<Type>* it)

#endif //sentinel AW_GENERIC_LIST_H