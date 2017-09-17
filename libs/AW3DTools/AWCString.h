#ifndef __AW_CSTRING_H
#define __AW_CSTRING_H

#include <string>
#include "AWFile.h"

using namespace std;
//typedef string AWCString;

#pragma warning(disable: 4275)
class AWCString : public string
{
public:
   USES_AWFILEIO(AWCString);

   AWCString(const char* c = "");
   ~AWCString(){erase();}

   operator const char*() const {return c_str();}
   AWBoolean contains(const char* c) const;
   AWCString&   remove(string::size_type startPos, string::size_type num = string::npos);
   void   eqSubString(const AWCString& s, string::size_type startPos, string::size_type endPos = string::npos);

   int    binaryStoreSize() const;

   int    read(AWFile& file);
   int    write(AWFile& file) const;
};//class AWCString : public string
#pragma warning(default: 4275)


inline 
AWCString::AWCString(const char* c) : string(c)
{
}



inline AWBoolean      
AWCString::contains(const char* c) const
{  //case insensitive would be ideal - 
   return (string::npos != find(c));
}//inline AWBoolean AWCString::contains(const char* c)



inline AWCString&   
AWCString::remove(string::size_type startPos, string::size_type num)
{
   if (string::npos == num)
   {  //default param implies remove up to end of string
      num = length()-startPos;
   }//if (string::npos == endPos)
   replace(startPos, num, "");
   return *this;
}//AWCString& AWCString::remove(string::size_type startPos, string::size_type num)



inline void
AWCString::eqSubString(const AWCString& s, string::size_type startPos, 
                       string::size_type endPos)
{  //assign to this string the sub string with given start / end pos
   if (string::npos == endPos)
   {  //default param indicates copy to end
      endPos = s.length()-1;
   }
   if ((startPos < s.length()) && (endPos < s.length()) )
   {
      *this = s;
      replace(endPos+1, length()-endPos-1, "");
      if (startPos>0)
      {
         replace(0, startPos, "");
      }
   }
}


inline int   
AWCString::binaryStoreSize() const
{
   return sizeof(int) + size() * sizeof(char);
}


inline int 
AWCString::read(AWFile& file)
{  //this is a very SLOW implementation - needs work
   size_t len;
   
   erase();
   // read the number of characters, then the string itself,
   // without the terminating null:
   file.Read(len);
   /*
   if (len)
   {
      char* tmp = new char[len+1];
      file.Read(tmp, len);
      tmp[len] = 0;
      *this = tmp;
      delete tmp;
   }
   */
   char c;
   while (len)
   {
      file.Read(c);
      *this += c;
      len--;
   }
   return len;
}//inline int AWCString::read(AWFile& file)



inline int 
AWCString::write(AWFile& file) const
{
   size_t len = size();
   // Store the number of characters, then the string itself,
   // without the terminating null:
   file.Write(len);
   file.Write(c_str(), len);
   return len;
}//inline int AWCString::write(AWFile& file) const

#endif //sentinel __AW_CSTRING_H