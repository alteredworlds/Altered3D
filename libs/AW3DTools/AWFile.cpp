// Class AWFile encapsulates ANSI-C file operations 



#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "AWFile.h"
#include "AWCString.h"

#ifndef macintosh
#  include <sys/types.h>        // Defines type used in <sys/stat.h>
#  include <sys/stat.h>         // Looking for statbuf 
#endif

#if defined( macintosh )
# if (defined(__PowerCpp__) || defined(__MWERKS__))
   // for unlink() declaration 
#  include <unix.h> 
# else 
   // for MrCpp
#  include <fcntl.h>
# endif
#elif defined(_WIN32)
#  include <io.h>	// Looking for access() and unlink() in here.
#else
   extern "C" {
     int access(const char*, int);
     int unlink(const char*); // remove() may be more portable in the future 
   }
#endif


// in order to mimic the POSIX access() function. If you have posix
// ignore the comments which apply to a "poor/pure ANSI" replacement
#ifndef R_OK
#  define F_OK	0 // "exists" == "readable" _or_ "appendable"
#  define X_OK	1 // cannot replace. Return FALSE.
#  define W_OK	2 // == "appendable" ("ab" mode)
#  define R_OK	4 // == "readable"   ("rb" mode)
#endif

// Microsoft C++ under NT does not have "access", instead calls 
// it "_access":
#ifdef _WIN32
#   define awaccess _access
#else
#   define awaccess access
#endif

// Determine the local endian format by looking at the low-address byte
// of a int-typed variable with value 1.  If big endian, this will be
// the high-order byte with value 0.  If little endian, it will be the
// low-order byte with value 1;

static int one = 1;

//0 -> little endian
int AWFile::localEndian  = (*((unsigned char*) &one)) ? 0 : 1;
int AWFile::streamEndian = 0; //comes from little endian Intel machines


// Reverse the order of the n bytes pointed to by v.
void 
AWFile::reverse(const void *v, size_t n)
{
  char* p = (char*)v;		   // point to first byte
  char* q = (char*)v + n - 1;	// point to last byte

  while (p < q) 
  {	// swap first <--> nth; second <--> (n-1)th; etc.
    char t = *p;
    *p++   = *q;
    *q--   = t;
  }//while (p < q) 
}//void AWFile::reverse(const void *v, size_t n)
 

/*size_t AWFile::awRead(char* s, size_t sizeOf);
  size_t AWFile::awReadVec(char* s, size_t sizeOf);
  size_t AWFile::awWrite(char* s, size_t sizeOf);
  size_t AWFile::awWriteVec(char* s, size_t sizeOf);

  char*           m_memChunk;
  char*           m_memPtr*/
 
size_t AWFile::awRead(char* s, size_t sizeOf) 
{
   size_t num = 0;
   if (!m_memPtr)
   {
      num = fread(s, sizeOf, 1, m_filep);
   }
   else
   {  //sizeOf is true size of of s in bytes
      char* next = m_memPtr + sizeOf;
      if (next <= m_memChunkEnd)
      {
         num = 1;
         memcpy(s, m_memPtr, sizeOf);
         m_memPtr = next;
      }//if (next <= m_memChunkEnd)
   }
   return num;
}//size_t AWFile::awRead(char* s, size_t sizeOf)


size_t AWFile::awReadVec(char* s, size_t sizeOf, int count) 
{
   size_t num = 0;
   if (!m_memPtr)
   {
      num = fread((char*)s, sizeOf, count, m_filep);
   }
   else
   {  //sizeOf is true size of of s in bytes
      num = sizeOf * count;
      char* next = m_memPtr + num;
      if (next <= m_memChunkEnd)
      {
         memcpy(s, m_memPtr, num);
         m_memPtr = next;
      }//if (next <= m_memChunkEnd)
   }  
   return num;
}//size_t AWFile::awReadVec(char* s, size_t sizeOf, int count)


size_t AWFile::awWrite(char* s, size_t sizeOf)
{
   size_t num = 0;   
   if (!m_memPtr)
   {
      num = fwrite((char*)s, sizeOf, 1, m_filep);
   }
   else
   {  //sizeOf is true size of of s in bytes
      char* next = m_memPtr + sizeOf;
      if (next <= m_memChunkEnd)
      {
         memcpy(m_memPtr, s, sizeOf);
         m_memPtr = next;
      }//if (next <= m_memChunkEnd)
   }  
   return num;
}//size_t AWFile::awWrite(char* s, size_t sizeOf)


size_t AWFile::awWriteVec(const char* s, size_t sizeOf, int count) 
{
   size_t num = 0;
   if (!m_memPtr)
   {
      num = fwrite(s, sizeOf, count, m_filep);
   }
   else
   {  //sizeOf is true size of of s in bytes
      num = sizeOf * count;
      char* next = m_memPtr + num;
      if (next <= m_memChunkEnd)
      {
         memcpy(m_memPtr, s, num);
         m_memPtr = next;
      }//if (next <= m_memChunkEnd)
   }  
   return num;
}//size_t AWFile::awWriteVec(const char* s, size_t sizeOf, int count)



#define READ(s)			  awRead((char*)&s, sizeof(s))
#define READVEC(s,count)  awReadVec((char*)s, sizeof(*s), count)
#define WRITE(s)		     awWrite((char*)&s, sizeof(s))
#define WRITEVEC(s,count) awWriteVec((const char*)s, sizeof(*s), count) 

//#define READ(s)			  fread((char*)&s, sizeof(s), 1, m_filep)
//#define READVEC(s,count)  fread((char*)s, sizeof(*s), count, m_filep)
//#define WRITE(s)		     fwrite((char*)&s, sizeof(s), 1, m_filep)
//#define WRITEVEC(s,count) fwrite((const char*) s, sizeof(*s), count, m_filep)

//#define READ(s) memcpy((char*)&s, m_memPtr, sizeof(s)/sizeof(char)); m_memPtr += sizeof(s)/sizeof(char);
//#define READVEC(s,count)  memcpy((char*)&s, m_memPtr, count*sizeof(s)/sizeof(char)); m_memPtr += count*sizeof(s)/sizeof(char);
//#define WRITE(s) memcpy(m_memPtr, (char*)&s, sizeof(s)/sizeof(char)); m_memPtr += sizeof(s)/sizeof(char);
//#define WRITEVEC(s,count) memcpy(m_memPtr, (char*)&s, count*sizeof(s)/sizeof(char)); m_memPtr += count*sizeof(s)/sizeof(char);

#ifdef AW_CRLF_CONVENTION
   static const char* updateMode = "rb+";
   static const char* newMode = "wb+";
# ifdef AW_NOT_POSIX_FSTAT
   static const char* readMode = "rb";
   static const char* appendMode = "ab";
# endif
#else  /* not AW_CRLF_CONVENTION: */
   static const char* updateMode = "r+";
   static const char* newMode = "w+";
# ifdef AW_NOT_POSIX_FSTAT
   static const char* readMode = "r";
   static const char* appendMode = "a";
# endif
#endif


AWFile::AWFile(const char* name, const char* mode)
   : m_name(NULL), m_filep(NULL), m_memChunk(NULL), m_memPtr(NULL)
{
   openFile(name, mode);   
}//AWFile::AWFile(const char* name, const char* mode)



AWFile::AWFile(const AWCString& name, const char* mode)
   : m_name(NULL), m_filep(NULL), m_memChunk(NULL), m_memPtr(NULL)
{
   openFile(name.c_str(), mode);
}//AWFile::AWFile(const char* name, const char* mode)


AWFile::AWFile(char* chunk, unsigned long sizeOf, const char* mode)
   : m_name(NULL), m_filep(NULL)
{  //processing of memory chunks ... !
   m_memChunk     = chunk;
   m_memPtr       = m_memChunk;
   m_memChunkEnd  = m_memChunk + sizeOf;
   //do not use mode at the moment (!) - READ/WRITE/READ-WRITE
}//AWFile::AWFile(char* chunk, unsigned long sizeOf, const char* mode)


AWBoolean         
AWFile::openFile(const char* name, const char* mode)
{
   if (mode)
   {
      m_filep = fopen(name, mode);
   }
   else
   {
      // No mode specified.  Use default actions:
      mode  = updateMode;
      m_filep = fopen(name, mode);	// Open existing file for update
      if (m_filep == NULL)			// Still not opened?
      {
         mode = newMode;
         m_filep = fopen(name, mode);	// Open new file
      }//if (m_filep == NULL)
   }//else
   m_name = strdup(name);
   return (NULL != m_filep);
}//AWBoolean AWFile::openFile(const char* name, const char* mode)



AWFile::~AWFile()
{
   Close();
   if (m_name)
   {
      free(m_name);
      m_name = NULL;
   }
   m_memChunk = NULL;
   m_memPtr   = NULL;
}//AWFile::~AWFile()



void                  
AWFile::Close()
{
   if (m_filep != NULL)
   {
      fclose(m_filep);
      m_filep = NULL;
   }
}//void AWFile::Close()


AWBoolean AWFile::Exists() 
{
#ifdef AW_NOT_POSIX_FSTAT
  if (!m_filep) return FALSE;
  return TRUE;
#else       // silence compiler warning 
  return awaccess(m_name, 0) >= 0;
#endif
}//AWBoolean AWFile::Exists()


int AWFile::size(const char* name) 
{
   int retVal = 0;
   struct _stat st;
   if (name && _stat(name, &st) != -1 && st.st_size)
   {  
		retVal = st.st_size;
	}//if (name && _stat(name, &st) != -1 && st.st_size)
   return retVal;
}//int AWFile::size(const char* name)  


AWBoolean AWFile::Exists(const char* name)
{
#ifdef AW_NOT_POSIX_FSTAT /* need to do it the hard way */
  FILE *fp = fopen(name, updateMode);
  if (fp) 
  {
    fclose(fp);
    return TRUE;
  }//if (fp) 
  return FALSE;
#else
  return awaccess(name, 0) >= 0;
#endif
}//AWBoolean AWFile::Exists(const char* name)



AWBoolean AWFile::Read(char&    t)           { return READ(t)==1; }

#ifndef AW_NO_OVERLOAD_UCHAR
AWBoolean AWFile::Read(unsigned char&  t)    { return READ(t)==1; }
#endif


AWBoolean AWFile::Read(short&   t)           
{ 
  AWBoolean retVal = READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(short));
  }
  return retVal;
}

AWBoolean AWFile::Read(int&     t)           
{ AWBoolean retVal= READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(int));
  }
  return retVal;
}

AWBoolean AWFile::Read(long&    t)           
{ AWBoolean retVal= READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(long));
  }
  return retVal;
}
AWBoolean AWFile::Read(unsigned short& t)    
{ 
  AWBoolean retVal = READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(unsigned short));
  }
  return retVal;
}

AWBoolean AWFile::Read(unsigned int&   t)
{ 
  AWBoolean retVal = READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(unsigned int));
  }
  return retVal;
}


AWBoolean AWFile::Read(unsigned long&  t)
{ 
  AWBoolean retVal = READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(unsigned long));
  }
  return retVal;
}

AWBoolean AWFile::Read(float&          t) 
{ 
  AWBoolean retVal = READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(float));
  }
  return retVal;
}

AWBoolean AWFile::Read(double&         t) 
{ 
  AWBoolean retVal = READ(t)==1; 
  if (retVal && streamEndian != localEndian) 
  {
    reverse(&t, sizeof(double));
  }
  return retVal;
}
AWBoolean AWFile::Read(char*   p, size_t N)  
{ 
   AWBoolean retVal =  READVEC(p,N)==N;
   if (streamEndian != localEndian) {
    for (int i=0; i<(int)N; i++)
      reverse(p+i, sizeof(char));
  }
   return retVal;
}

AWBoolean AWFile::Read(short*  p, size_t N)
{ 
   AWBoolean retVal =  READVEC(p,N)==N;
   if (streamEndian != localEndian) {
    for (int i=0; i<(int)N; i++)
      reverse(p+i, sizeof(short));
  }
   return retVal;
}
AWBoolean AWFile::Read(int*    p, size_t N)
{ 
   AWBoolean retVal =  READVEC(p,N)==N;
   if (streamEndian != localEndian) {
    for (int i=0; i<(int)N; i++)
      reverse(p+i, sizeof(int));
  }
   return retVal;
}
AWBoolean AWFile::Read(long*   p, size_t N)
{ 
   AWBoolean retVal =  READVEC(p,N)==N;
   if (streamEndian != localEndian) {
    for (int i=0; i<(int)N; i++)
      reverse(p+i, sizeof(long));
  }
   return retVal;
}
AWBoolean AWFile::Read(float*  p, size_t N)
{ 
   AWBoolean retVal =  READVEC(p,N)==N;
   if (streamEndian != localEndian) {
    for (int i=0; i<(int)N; i++)
      reverse(p+i, sizeof(float));
  }
   return retVal;
}
AWBoolean AWFile::Read(double* p, size_t N)
{ 
   AWBoolean retVal =  READVEC(p,N)==N;
   if (streamEndian != localEndian) {
    for (int i=0; i<(int)N; i++)
      reverse(p+i, sizeof(double));
  }
   return retVal;
}
#ifndef AW_NO_OVERLOAD_WCHAR
AWBoolean AWFile::Read(wchar_t& t)           { return READ(t)==1; }
AWBoolean AWFile::Read(wchar_t* p, size_t N) { return READVEC(p,N)==N; }
#endif



AWBoolean AWFile::Read(char* string)
{
  int c;
  while (1) {
    c = fgetc(m_filep);
    if( c==EOF || c=='\0') break;
    *string++ = (char)c;
  }
  *string = '\0';
  return TRUE;
}

AWBoolean AWFile::Write(char t)    { return WRITE(t)==1; }
AWBoolean AWFile::Write(short t)   { return WRITE(t)==1; }
AWBoolean AWFile::Write(int t)     { return WRITE(t)==1; }
AWBoolean AWFile::Write(long t)    { return WRITE(t)==1; }
#ifndef AW_NO_OVERLOAD_UCHAR
AWBoolean AWFile::Write(unsigned char  t) { return WRITE(t)==1; }
#endif
AWBoolean AWFile::Write(unsigned short t) { return WRITE(t)==1; }
AWBoolean AWFile::Write(unsigned int   t) { return WRITE(t)==1; }
AWBoolean AWFile::Write(unsigned long  t) { return WRITE(t)==1; }
AWBoolean AWFile::Write(float          t) { return WRITE(t)==1; }
AWBoolean AWFile::Write(double         t) { return WRITE(t)==1; }
AWBoolean AWFile::Write(const char*   p, size_t N)  { return WRITEVEC(p,N)==N; }
AWBoolean AWFile::Write(const short*  p, size_t N)  { return WRITEVEC(p,N)==N; }
AWBoolean AWFile::Write(const int*    p, size_t N)  { return WRITEVEC(p,N)==N; }
AWBoolean AWFile::Write(const long*   p, size_t N)  { return WRITEVEC(p,N)==N; }
AWBoolean AWFile::Write(const float*  p, size_t N)  { return WRITEVEC(p,N)==N; }
AWBoolean AWFile::Write(const double* p, size_t N)  { return WRITEVEC(p,N)==N; }
#ifndef AW_NO_OVERLOAD_WCHAR
AWBoolean AWFile::Write(wchar_t t)                  { return WRITE(t)==1; }
AWBoolean AWFile::Write(const wchar_t* p, size_t N) { return WRITEVEC(p,N)==N; }
#endif

/*
 * Write up to and including the null terminator.
 * Be careful of non-terminated strings!
 */
AWBoolean AWFile::Write(const char* string) 
{
  return Write(string, strlen(string)+1);
}

/****************************************************************
 *								*
 *		Miscellaneous positioning & query functions	*
 *								*
 ****************************************************************/

/*m_memChunk;
  char*              m_memPtr;
  char*              m_memChunkEnd*/
AWoffset AWFile::CurOffset()            
{ 
   AWoffset retVal = 0;
   if (m_filep) 
   {
      retVal = ftell(m_filep); 
   }
   else if (m_memChunk&&m_memPtr)
   {
      retVal = m_memPtr-m_memChunk;
   }
   return retVal;
}//AWoffset AWFile::CurOffset()


void	   AWFile::ClearErr()             
{ if (m_filep) clearerr(m_filep); }


AWBoolean AWFile::Eof()                  
{ 
   if (m_filep)
   {
      return feof(m_filep); 
   }
   else
   {
      return m_memPtr >= m_memChunkEnd;
   }
}//AWBoolean     AWFile::Eof()


AWBoolean     AWFile::Error()                
{
   AWBoolean retVal = FALSE;
   if (m_filep)
   {
      retVal = ferror(m_filep);
   }
   return retVal;
}//AWBoolean     AWFile::Error()


AWBoolean     AWFile::Flush()                
{ 
   AWBoolean retVal = FALSE;
   if (m_filep)
   {
      retVal = fflush(m_filep) != EOF; 
   }
   return retVal;
}//AWBoolean     AWFile::Flush()  


AWBoolean AWFile::SeekTo(AWoffset offset)
{ 
   return fseek(m_filep, offset,0)>=0;
}//AWBoolean AWFile::SeekTo(AWoffset offset)


AWBoolean  AWFile::SeekToEnd()            
{
   AWBoolean retVal = FALSE;
   if (m_filep)
   {
      retVal = fseek(m_filep, 0, 2) >= 0; 
   }
   return retVal;
}//AWBoolean     AWFile::SeekToEnd()