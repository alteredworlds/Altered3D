#ifndef __AWFILE_H__
#define __AWFILE_H__

// Class AWFile encapsulates ANSI-C binary file operations

// MS-DOS, and some other operating systems differentiate between
// TEXT and BINARY files. Text files use a CR/LF convention to end lines.
// AWFile is intended to model *binary* files, so shut off this translation.
// This requires opening the file in an explicit "binary" mode if supplying
// mode parameter. 
// AWFile myFile("junk.dat", "wb+");

#include "AWTooldefs.h"
#ifdef _WIN32
#define AW_CRLF_CONVENTION 1
#endif

#include <stdio.h>

class AWExport AWFile;

#define USES_AWFILEIO(className)                           \
inline friend AWFile& operator>>(AWFile& f, className& it)      { it.read(f); return f; } \
inline friend AWFile& operator<<(AWFile& f, const className& it){ it.write(f); return f; } \
inline friend AWFile& operator>>(AWFile& f, className*& it)     { it=new className;it->read(f);return f;} \
inline friend AWFile& operator<<(AWFile& f, const className* it){if (it) it->write(f); return f;}

class AWExport AWCString;

class AWExport AWFile
{
  AWFile(const AWFile&);            // Not implemented!
  void    operator=(const AWFile&); // Not implemented!
public:
  AWFile(const AWCString& name, const char* mode = awnil);
  AWFile(const char* name, const char* mode = awnil);
  AWFile(char* chunk, unsigned long sizeOf, const char* mode = awnil);
  ~AWFile();

  void                  Close();
       
  const char*      GetName()   const {return m_name;    }
  FILE*            GetStream() const {return m_filep;       }
  AWBoolean             isValid()   const {return (m_filep!=awnil) || (m_memChunk&&m_memPtr&&m_memChunkEnd);}
  AWBoolean             Exists();
  static AWBoolean      Exists(const char* name);
  static int            size(const char* name);
       
  AWBoolean             Read(char& c);
  AWBoolean             Read(short& i);
  AWBoolean             Read(int& i);
  AWBoolean             Read(long& i);
#ifndef AW_NO_OVERLOAD_WCHAR
  AWBoolean             Read(wchar_t& w);
#endif
#ifndef AW_NO_OVERLOAD_UCHAR
  AWBoolean             Read(unsigned char& c);
#endif
  AWBoolean             Read(unsigned short&);
  AWBoolean             Read(unsigned int&  );
  AWBoolean             Read(unsigned long& );
  AWBoolean             Read(float& );
  AWBoolean             Read(double&);

  AWBoolean             Read(char*  , size_t N);
  AWBoolean             Read(short* , size_t N);
  AWBoolean             Read(int*   , size_t N);
  AWBoolean             Read(long*  , size_t N);
#ifndef AW_NO_OVERLOAD_WCHAR
  AWBoolean             Read(wchar_t* w, size_t N);
#endif
#ifndef AW_NO_OVERLOAD_UCHAR
  AWBoolean             Read(unsigned char* c, size_t N)
        {return Read((char*)c, N);}
#endif
  AWBoolean             Read(unsigned short* i, size_t N)
        {return Read((short*)i, N);}
  AWBoolean             Read(unsigned int*   i, size_t N)
        {return Read((int  *)i, N);}
  AWBoolean             Read(unsigned long*  i, size_t N)
        {return Read((long *)i, N);}
  AWBoolean             Read(float* , size_t N);
  AWBoolean             Read(double*, size_t N);

  // Read to null terminator or EOF; no CR/LF xlation done. Beware of overflow.
  AWBoolean             Read(char* string);     
  
  AWBoolean             Write(char c);
  AWBoolean             Write(short s);
  AWBoolean             Write(int i);
  AWBoolean             Write(long l);
#ifndef AW_NO_OVERLOAD_WCHAR
  AWBoolean             Write(wchar_t w);
#endif
#ifndef AW_NO_OVERLOAD_UCHAR
  AWBoolean             Write(unsigned char c);
#endif
  AWBoolean             Write(unsigned short s);
  AWBoolean             Write(unsigned int i);
  AWBoolean             Write(unsigned long l);
  AWBoolean             Write(float f);
  AWBoolean             Write(double d);

  AWBoolean             Write(const char* string);
  AWBoolean             Write(const short* , size_t N);
  AWBoolean             Write(const int*   , size_t N);
  AWBoolean             Write(const long*  , size_t N);
#ifndef AW_NO_OVERLOAD_WCHAR
  AWBoolean             Write(const wchar_t* w, size_t N);
#endif
#ifndef AW_NO_OVERLOAD_UCHAR
  AWBoolean             Write(const unsigned char* c, size_t N)
        {return Write((const char*)c, N);}
#endif
  AWBoolean             Write(const unsigned short* i, size_t N)
        {return Write((const short*)i, N);}
  AWBoolean             Write(const unsigned int*   i, size_t N)
          {return Write((const int  *)i, N);}
  AWBoolean             Write(const unsigned long*  i, size_t N)
        {return Write((const long *)i, N);}
  AWBoolean             Write(const float*  , size_t N);
  AWBoolean             Write(const double* , size_t N);
  AWBoolean             Write(const char* string, size_t N);

  AWoffset         CurOffset(); // Returns current offset of file
  void             ClearErr();  // reset to !Eof() and !Error()
  AWBoolean             Eof();           // TRUE if file at EOF
  AWBoolean             Error();     // TRUE if the file has had an error.
  AWBoolean             Flush();     // Writes all pending output
  AWBoolean             IsEmpty();   // TRUE if the file is empty
  AWBoolean             SeekTo(AWoffset offset); /* offset from beginning of file */
  AWBoolean             SeekToBegin()   {return SeekTo(0);}
  AWBoolean             SeekToEnd();

protected:
   AWBoolean         openFile(const char* f, const char* m);
  char*              m_name;
  FILE*              m_filep;
  static int         streamEndian;
  static int         localEndian;
  static void        reverse(const void *v, size_t n);

  size_t             awRead(char* s, size_t sizeOf);
  size_t             awReadVec(char* s, size_t sizeOf, int count);
  size_t             awWrite(char* s, size_t sizeOf);
  size_t             awWriteVec(const char* s, size_t sizeOf, int count);

//ADDITIONS FOR USING MEMORY CHUNK as 
//data source instead of file
  char*              m_memChunk;
  char*              m_memPtr;
  char*              m_memChunkEnd;
};//class AWExport AWFile


inline AWFile& operator<<(AWFile& f, char a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, short a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, int a)
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, long a) 
{ f.Write(a); return f; }

#ifndef AW_NO_OVERLOAD_WCHAR
inline AWFile& operator<<(AWFile& f, wchar_t a) 
{ f.Write(a); return f; }
#endif

#ifndef AW_NO_OVERLOAD_UCHAR
inline AWFile& operator<<(AWFile& f, unsigned char a) 
{ f.Write(a); return f; }
#endif

inline AWFile& operator<<(AWFile& f, unsigned short a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, unsigned int a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, unsigned long a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, float a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, double a) 
{ f.Write(a); return f; }

inline AWFile& operator<<(AWFile& f, const char * a) 
{ f.Write(a); return f; }

inline AWFile& operator>>(AWFile& f, char& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, short& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, int& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, long& a) 
{ f.Read(a); return f; }

#ifndef AW_NO_OVERLOAD_WCHAR
inline AWFile& operator>>(AWFile& f, wchar_t& a) 
{ f.Read(a); return f; }
#endif

#ifndef AW_NO_OVERLOAD_UCHAR
inline AWFile& operator>>(AWFile& f, unsigned char& a) 
{ f.Read(a); return f; }
#endif 

inline AWFile& operator>>(AWFile& f, unsigned short& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, unsigned int& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, unsigned long& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, float& a) 
{ f.Read(a); return f; }

inline AWFile& operator>>(AWFile& f, double& a)
{ f.Read(a); return f; }

#endif  /* __AWFILE_H__ */
