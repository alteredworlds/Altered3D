#ifndef __AWTOOLDEFS_H
#define __AWTOOLDEFS_H

//#define __AWTRACE 1

#ifdef _WIN32
#ifndef AWExport
#    ifdef  _AWBUILDSHARED
#        define AWExport __declspec(dllexport)
#    else
#		ifdef AW3D_USEDLL
#			define AWExport __declspec(dllimport)
#		else
#			define AWExport
#		endif
#    endif
#endif
#else
#   define AWExport
#endif //ifdef _WIN32

#define awexport AWExport

#ifdef _WIN32
#   define M_PI 3.141592653589793
#endif

#define AW_NO_OVERLOAD_WCHAR 1
//#define AW_NO_OVERLOAD_UCHAR 1
#define AW_NOLIST 0
#define AW_NPOS   -1


typedef unsigned long   AWspace;        // Size of file allocations
typedef int AWBoolean;
#define AWoffset long 
#define awnil NULL
typedef unsigned short AWClassID;


#ifndef TRUE
#  define TRUE  1
#  define FALSE 0
#endif


#ifndef _WIN32
#define DWORD unsigned long
//#define BOOL  bool
#define LONG  long
#define LONGLONG long long
#define __int64  long long
#define BYTE  unsigned char
#define WORD  unsigned short
#define HANDLE long
#define HINSTANCE long
#define LOWORD(l)   ((WORD) (l)) 
#define HIWORD(l)   ((WORD) (((DWORD) (l) >> 16) & 0xFFFF)) 

typedef union _LARGE_INTEGER {
   struct {
      DWORD LowPart;
      LONG  HighPart;
   }u;
   LONGLONG QuadPart;
} LARGE_INTEGER;

typedef struct _RECT {
   LONG left;
   LONG top;
   LONG right;
   LONG bottom;
} RECT;

typedef struct _POINT {
   LONG x;
   LONG y;
} POINT;

#define VK_PRIOR    33     
#define VK_NEXT     34   
#define VK_HOME     35       
#define VK_END      36      
#define VK_SNAPSHOT  44
         
#endif

//support for multi-threaded locking
#ifndef AW_SINGLE_THREADED_ENGINE
	#define AWDECLARE_LOCKABLE AWLock m_mutex; void	acquire(){m_mutex.acquire();} void	release(){m_mutex.release();}
	#define AW_LOCK_BLOCK AWGuard dummyGuard(m_mutex)
	#define AW_GET_LOCK m_mutex.acquire();
	#define AW_RELEASE_LOCK m_mutex.release();
#else
	#define AWDECLARE_LOCKABLE
	#define AW_LOCK_BLOCK
	#define AW_GET_LOCK
	#define AW_RELEASE_LOCK
#endif //#ifndef AW_SINGLE_THREADED_ENGINE

#pragma warning(disable:4996)

#ifndef AWPERSISTABLE_POINTER_SIZE
	#define AWPERSISTABLE_POINTER_SIZE( arg ) (arg) ? arg->recursiveStoreSize() : AWPersistable::nilStoreSize()
#endif

//prevent dumb warning about non-exportable classes used as base
#pragma warning( disable : 4251 )


#ifdef __AWTRACE
#include <stdio.h>

class AWTraceMgr
{
public:
   AWTraceMgr(const char* name);
   ~AWTraceMgr();

   static FILE* m_stream;
};//class AWTraceMgr



inline AWTraceMgr::AWTraceMgr(const char* name)
{
//this is cross platform, but want in console on linux for now...    
   m_stream = freopen(name, "a+", stderr);   
   if( m_stream == NULL )
   {
      fprintf( stdout, "error on freopen\n" );   
   }
   else   
   {
      fprintf( m_stream, "Alteredsaver log file\n" );  
   }
}//AWTraceMgr::AWTraceMgr(const char* name)


inline AWTraceMgr::~AWTraceMgr()
{
   if (m_stream) fclose( m_stream );
   m_stream = NULL;
}//AWTraceMgr::~AWTraceMgr()

#define AWTRACE_INIT(name) FILE* AWTraceMgr::m_stream = NULL;\
   AWTraceMgr theTraceMgr(name);
#else
#define AWTRACE_INIT(name)
#endif


#ifdef __AWTRACE
#define AWTRACE( a)              fprintf(stderr, a)
#define AWTRACE1( a, b)          fprintf(stderr, a, b)
#define AWTRACE2( a, b, c)       fprintf(stderr, a, b, c)
#define AWTRACE3( a, b, c, d)    fprintf(stderr, a, b, c, d)
#define AWTRACE4( a, b, c, d, e)    fprintf(stderr, a, b, c, d, e)
#else
#define AWTRACE( a)
#define AWTRACE1( a, b)
#define AWTRACE2( a, b, c)
#define AWTRACE3( a, b, c, d)
#define AWTRACE4( a, b, c, d, e)
#endif
#ifdef __AWTRACEHI
#define AWTRACEHI( a)            AWTRACE( a)
#define AWTRACEHI1( a, b)        AWTRACE1( a, b)
#define AWTRACEHI2( a, b, c)     AWTRACE2( a, b, c)
#define AWTRACEHI3( a, b, c, d)  AWTRACE3( a, b, c, d)
#define AWTRACEHI4( a, b, c, d, e)  AWTRACE4( a, b, c, d, e)
#else
#define AWTRACEHI( a)
#define AWTRACEHI1( a, b)
#define AWTRACEHI2( a, b, c)
#define AWTRACEHI3( a, b, c, d)
#define AWTRACEHI4( a, b, c, d, e)
#endif


//Class IDs for persistent data
#define CLSID_AWPERSISTABLE         0x8007
#define CLSID_AWNODE				      1125
#define CLSID_AWFACE				      1126
#define CLSID_AWOBJECT				   1127
#define CLSID_AWGLTEXTURE			   1128
#define CLSID_AWSCENE				   1129
#define CLSID_AWCAMERA				   1130
#define CLSID_AWQUAT				      1131
#define CLSID_AWPOSITIONKEY			1140
#define CLSID_AWROTATIONKEY			1141
#define CLSID_AWTRANSFORMCONTROLER	1142
#define CLSID_AWFLIGHTCONTROLLER	   1143
#define CLSID_AWWORKER				   1144
#define CLSID_AWGLANIMTEXTURE		   1145
#define CLSID_AWLIGHT				   1146
#define CLSID_AWANIMATEDPOINT		   1147
#define CLSID_AWANIMATEDQUAT		   1148
#define CLSID_AWFLOCK				   1149
#define CLSID_AWBOIDCONTROLLER		1150
#define CLSID_AWMESH				      1151
#define CLSID_AWANIMATEDMESH		   1152
#define CLSID_AWMATERIAL			   1153
#define CLSID_AWENVIRONCONTROLLER	1154
#define CLSID_AWANIMATEDFLOAT		   1155
#define CLSID_AWFLOATKEY			   1156
#define CLSID_AWBILLBOARDCONTROLLER 1157
#define CLSID_AWLODOBJECT			   1158
#define CLSID_AWSEAOBJECT			   1159
#define CLSID_AWFOG					   1160
#define CLSID_AWROCKET				   1161
#define CLSID_AWROCKETCONTROLLER	   1162
#define CLSID_AWTIMERBARCTL         1163
#define CLSID_AWCUBEENVTEXTURE      1164
#define CLSID_AWTEXTUREEFFECT       1165
#define CLSID_AWRENDERPASS          1166
#define CLSID_AWBUMPMESH            1167
#define CLSID_AWNORMALMAPTEXTURE    1168
#define CLSID_AWTEXTUREUNIT         1169
#define CLSID_AWSTDMATERIAL         1170
#define CLSID_AWSHADOWMAP           1171
#define CLSID_AWVERTEXPROGRAM 1172


#endif //sentinel __AWTOOLDEFS_H