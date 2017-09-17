# Microsoft Developer Studio Generated NMAKE File, Based on zlib.dsp
!IF "$(CFG)" == ""
CFG=zlib - Win32 DLL
!MESSAGE No configuration specified. Defaulting to zlib - Win32 DLL.
!ENDIF 

!IF "$(CFG)" != "zlib - Win32 DLL" && "$(CFG)" != "zlib - Win32 DLL Debug" && "$(CFG)" != "zlib - Win32 DLL ASM" && "$(CFG)" != "zlib - Win32 DLL Debug ASM" && "$(CFG)" != "zlib - Win32 LIB" && "$(CFG)" != "zlib - Win32 LIB Debug" && "$(CFG)" != "zlib - Win32 DLL VB"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zlib - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zlib - Win32 DLL ASM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zlib - Win32 DLL Debug ASM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zlib - Win32 LIB" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 LIB Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 DLL VB" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "zlib - Win32 DLL"

OUTDIR=.\win32\zlib\dll
INTDIR=.\win32\zlib\dll
# Begin Custom Macros
OutDir=.\win32\zlib\dll
# End Custom Macros

ALL : "$(OUTDIR)\zlib.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zlib.res"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.dll"
	-@erase "$(OUTDIR)\zlib.exp"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zlib.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\zlib.pdb" /machine:I386 /def:".\zlib.def" /out:"$(OUTDIR)\zlib.dll" /implib:"$(OUTDIR)\zlib.lib" 
DEF_FILE= \
	".\zlib.def"
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\zlib.res"

"$(OUTDIR)\zlib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

OUTDIR=.\win32\zlib\dll_dbg
INTDIR=.\win32\zlib\dll_dbg
# Begin Custom Macros
OutDir=.\win32\zlib\dll_dbg
# End Custom Macros

ALL : "$(OUTDIR)\zlibd.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zlib.res"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlibd.dll"
	-@erase "$(OUTDIR)\zlibd.exp"
	-@erase "$(OUTDIR)\zlibd.ilk"
	-@erase "$(OUTDIR)\zlibd.lib"
	-@erase "$(OUTDIR)\zlibd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zlib.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\zlibd.pdb" /debug /machine:I386 /def:".\zlib.def" /out:"$(OUTDIR)\zlibd.dll" /implib:"$(OUTDIR)\zlibd.lib" 
DEF_FILE= \
	".\zlib.def"
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\zlib.res"

"$(OUTDIR)\zlibd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

OUTDIR=.\win32\zlib\dll_asm
INTDIR=.\win32\zlib\dll_asm
# Begin Custom Macros
OutDir=.\win32\zlib\dll_asm
# End Custom Macros

ALL : "$(OUTDIR)\zliba.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gvmat32c.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zlib.res"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zliba.dll"
	-@erase "$(OUTDIR)\zliba.exp"
	-@erase "$(OUTDIR)\zliba.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zlib.res" /i "..\.." /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gvmat32.obj /nologo /dll /incremental:no /pdb:"$(OUTDIR)\zliba.pdb" /machine:I386 /def:".\zlib.def" /out:"$(OUTDIR)\zliba.dll" /implib:"$(OUTDIR)\zliba.lib" 
DEF_FILE= \
	".\zlib.def"
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gvmat32c.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\zlib.res"

"$(OUTDIR)\zliba.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

OUTDIR=.\win32\zlib\dll_dbga
INTDIR=.\win32\zlib\dll_dbga
# Begin Custom Macros
OutDir=.\win32\zlib\dll_dbga
# End Custom Macros

ALL : "$(OUTDIR)\zlibb.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gvmat32c.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zlib.res"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlibb.dll"
	-@erase "$(OUTDIR)\zlibb.exp"
	-@erase "$(OUTDIR)\zlibb.ilk"
	-@erase "$(OUTDIR)\zlibb.lib"
	-@erase "$(OUTDIR)\zlibb.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zlib.res" /i "..\.." /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gvmat32d.obj /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\zlibb.pdb" /debug /machine:I386 /def:".\zlib.def" /out:"$(OUTDIR)\zlibb.dll" /implib:"$(OUTDIR)\zlibb.lib" 
DEF_FILE= \
	".\zlib.def"
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gvmat32c.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\zlib.res"

"$(OUTDIR)\zlibb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

OUTDIR=.\win32\zlib\lib
INTDIR=.\win32\zlib\lib
# Begin Custom Macros
OutDir=.\win32\zlib\lib
# End Custom Macros

ALL : "$(OUTDIR)\zlib.lib"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

OUTDIR=.\win32\zlib\lib_dbg
INTDIR=.\win32\zlib\lib_dbg
# Begin Custom Macros
OutDir=.\win32\zlib\lib_dbg
# End Custom Macros

ALL : "$(OUTDIR)\zlib.lib"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

OUTDIR=.\win32\zlib\dll_vb
INTDIR=.\win32\zlib\dll_vb
# Begin Custom Macros
OutDir=.\win32\zlib\dll_vb
# End Custom Macros

ALL : "$(OUTDIR)\zlibvb.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zlib.pch"
	-@erase "$(INTDIR)\zlib.res"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlibvb.dll"
	-@erase "$(OUTDIR)\zlibvb.exp"
	-@erase "$(OUTDIR)\zlibvb.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zlib.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\zlibvb.pdb" /machine:I386 /def:".\zlib.def" /out:"$(OUTDIR)\zlibvb.dll" /implib:"$(OUTDIR)\zlibvb.lib" 
DEF_FILE= \
	".\zlib.def"
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\zlib.res"

"$(OUTDIR)\zlibvb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("zlib.dep")
!INCLUDE "zlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "zlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "zlib - Win32 DLL" || "$(CFG)" == "zlib - Win32 DLL Debug" || "$(CFG)" == "zlib - Win32 DLL ASM" || "$(CFG)" == "zlib - Win32 DLL Debug ASM" || "$(CFG)" == "zlib - Win32 LIB" || "$(CFG)" == "zlib - Win32 LIB Debug" || "$(CFG)" == "zlib - Win32 DLL VB"
SOURCE=..\..\..\zlib\adler32.c

"$(INTDIR)\adler32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\zlib\compress.c

"$(INTDIR)\compress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\zlib\crc32.c

"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\zlib\deflate.c

"$(INTDIR)\deflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\zlib\contrib\asm386\gvmat32c.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"


"$(INTDIR)\gvmat32c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"


"$(INTDIR)\gvmat32c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

!ENDIF 

SOURCE=..\..\..\zlib\gzio.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yc"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gzio.obj"	"$(INTDIR)\zlib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\infblock.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\infcodes.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\inffast.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\inflate.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\inftrees.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\infutil.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\zlib\trees.c

"$(INTDIR)\trees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\zlib\uncompr.c

"$(INTDIR)\uncompr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\zlib\msdos\zlib.rc

!IF  "$(CFG)" == "zlib - Win32 DLL"


"$(INTDIR)\zlib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\zlib.res" /i "\Projects\zlib\msdos" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"


"$(INTDIR)\zlib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\zlib.res" /i "\Projects\zlib\msdos" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"


"$(INTDIR)\zlib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\zlib.res" /i "..\.." /i "\Projects\zlib\msdos" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"


"$(INTDIR)\zlib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\zlib.res" /i "..\.." /i "\Projects\zlib\msdos" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"


"$(INTDIR)\zlib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\zlib.res" /i "\Projects\zlib\msdos" /d "NDEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\..\..\zlib\zutil.c

!IF  "$(CFG)" == "zlib - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\..\..\zlib" /D "_DEBUG" /D "WIN32" /D "_WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /D "DYNAMIC_CRC_TABLE" /D "ASMV" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zlib - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ZLIB_DLL" /Fp"$(INTDIR)\zlib.pch" /Yu"zutil.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\zlib.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

