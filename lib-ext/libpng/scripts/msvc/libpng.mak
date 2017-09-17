# Microsoft Developer Studio Generated NMAKE File, Based on libpng.dsp
!IF "$(CFG)" == ""
CFG=libpng - Win32 DLL
!MESSAGE No configuration specified. Defaulting to libpng - Win32 DLL.
!ENDIF 

!IF "$(CFG)" != "libpng - Win32 DLL" && "$(CFG)" != "libpng - Win32 DLL Debug" && "$(CFG)" != "libpng - Win32 DLL ASM" && "$(CFG)" != "libpng - Win32 DLL Debug ASM" && "$(CFG)" != "libpng - Win32 LIB" && "$(CFG)" != "libpng - Win32 LIB Debug" && "$(CFG)" != "libpng - Win32 DLL VB"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libpng - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libpng - Win32 DLL ASM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libpng - Win32 DLL Debug ASM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libpng - Win32 LIB" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 LIB Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 DLL VB" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "libpng - Win32 DLL"

OUTDIR=.\win32\libpng\dll
INTDIR=.\win32\libpng\dll
# Begin Custom Macros
OutDir=.\win32\libpng\dll
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpng13.dll"

!ELSE 

ALL : "zlib - Win32 DLL" "$(OUTDIR)\libpng13.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DLLCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\png.res"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libpng13.dll"
	-@erase "$(OUTDIR)\libpng13.exp"
	-@erase "$(OUTDIR)\libpng13.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\png.res" /i "..\.." /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\libpng13.pdb" /machine:I386 /def:".\png32ms.def" /out:"$(OUTDIR)\libpng13.dll" /implib:"$(OUTDIR)\libpng13.lib" 
DEF_FILE= \
	".\png32ms.def"
LINK32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\png.res" \
	".\win32\zlib\dll\zlib.lib"

"$(OUTDIR)\libpng13.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug"

OUTDIR=.\win32\libpng\dll_dbg
INTDIR=.\win32\libpng\dll_dbg
# Begin Custom Macros
OutDir=.\win32\libpng\dll_dbg
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpng13d.dll"

!ELSE 

ALL : "zlib - Win32 DLL Debug" "$(OUTDIR)\libpng13d.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DLL DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\png.res"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libpng13d.dll"
	-@erase "$(OUTDIR)\libpng13d.exp"
	-@erase "$(OUTDIR)\libpng13d.ilk"
	-@erase "$(OUTDIR)\libpng13d.lib"
	-@erase "$(OUTDIR)\libpng13d.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\png.res" /i "..\.." /d "_DEBUG" /d PNG_DEBUG=1 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libpng13d.pdb" /debug /machine:I386 /def:".\png32ms.def" /out:"$(OUTDIR)\libpng13d.dll" /implib:"$(OUTDIR)\libpng13d.lib" 
DEF_FILE= \
	".\png32ms.def"
LINK32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\png.res" \
	".\win32\zlib\dll_dbg\zlibd.lib"

"$(OUTDIR)\libpng13d.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL ASM"

OUTDIR=.\win32\libpng\dll_asm
INTDIR=.\win32\libpng\dll_asm
# Begin Custom Macros
OutDir=.\win32\libpng\dll_asm
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpng13a.dll"

!ELSE 

ALL : "zlib - Win32 DLL ASM" "$(OUTDIR)\libpng13a.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DLL ASMCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\png.res"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngvcrd.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libpng13a.dll"
	-@erase "$(OUTDIR)\libpng13a.exp"
	-@erase "$(OUTDIR)\libpng13a.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_USE_PNGVCRD" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\png.res" /i "..\.." /d "NDEBUG" /d "PNG_USE_PNGVCRD" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\libpng13a.pdb" /machine:I386 /def:".\png32ms.def" /out:"$(OUTDIR)\libpng13a.dll" /implib:"$(OUTDIR)\libpng13a.lib" 
DEF_FILE= \
	".\png32ms.def"
LINK32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngvcrd.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\png.res" \
	".\win32\zlib\dll_asm\zliba.lib"

"$(OUTDIR)\libpng13a.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug ASM"

OUTDIR=.\win32\libpng\dll_dbga
INTDIR=.\win32\libpng\dll_dbga
# Begin Custom Macros
OutDir=.\win32\libpng\dll_dbga
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpng13b.dll"

!ELSE 

ALL : "zlib - Win32 DLL Debug ASM" "$(OUTDIR)\libpng13b.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DLL Debug ASMCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\png.res"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngvcrd.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libpng13b.dll"
	-@erase "$(OUTDIR)\libpng13b.exp"
	-@erase "$(OUTDIR)\libpng13b.ilk"
	-@erase "$(OUTDIR)\libpng13b.lib"
	-@erase "$(OUTDIR)\libpng13b.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "PNG_USE_PNGVCRD" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\png.res" /i "..\.." /d "_DEBUG" /d PNG_DEBUG=1 /d "PNG_USE_PNGVCRD" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libpng13b.pdb" /debug /machine:I386 /def:".\png32ms.def" /out:"$(OUTDIR)\libpng13b.dll" /implib:"$(OUTDIR)\libpng13b.lib" 
DEF_FILE= \
	".\png32ms.def"
LINK32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngvcrd.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\png.res" \
	".\win32\zlib\dll_dbga\zlibb.lib"

"$(OUTDIR)\libpng13b.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 LIB"

OUTDIR=.\win32\libpng\lib
INTDIR=.\win32\libpng\lib
# Begin Custom Macros
OutDir=.\win32\libpng\lib
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpng.lib"

!ELSE 

ALL : "zlib - Win32 LIB" "$(OUTDIR)\libpng.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 LIBCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	".\win32\zlib\lib\zlib.lib"

"$(OUTDIR)\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 LIB Debug"

OUTDIR=.\win32\libpng\lib_dbg
INTDIR=.\win32\libpng\lib_dbg
# Begin Custom Macros
OutDir=.\win32\libpng\lib_dbg
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpng.lib"

!ELSE 

ALL : "zlib - Win32 LIB Debug" "$(OUTDIR)\libpng.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 LIB DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	".\win32\zlib\lib_dbg\zlib.lib"

"$(OUTDIR)\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL VB"

OUTDIR=.\win32\libpng\dll_vb
INTDIR=.\win32\libpng\dll_vb
# Begin Custom Macros
OutDir=.\win32\libpng\dll_vb
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\libpngvb13.dll"

!ELSE 

ALL : "zlib - Win32 DLL VB" "$(OUTDIR)\libpngvb13.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"zlib - Win32 DLL VBCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\libpng.pch"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\png.res"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libpngvb13.dll"
	-@erase "$(OUTDIR)\libpngvb13.exp"
	-@erase "$(OUTDIR)\libpngvb13.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /D PNGAPI=__stdcall /Fp"$(INTDIR)\libpng.pch" /Yu"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\png.res" /i "..\.." /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libpng.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\libpngvb13.pdb" /machine:I386 /def:".\png32ms.def" /out:"$(OUTDIR)\libpngvb13.dll" /implib:"$(OUTDIR)\libpngvb13.lib" 
DEF_FILE= \
	".\png32ms.def"
LINK32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\png.res" \
	".\win32\zlib\dll_vb\zlibvb.lib"

"$(OUTDIR)\libpngvb13.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libpng.dep")
!INCLUDE "libpng.dep"
!ELSE 
!MESSAGE Warning: cannot find "libpng.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libpng - Win32 DLL" || "$(CFG)" == "libpng - Win32 DLL Debug" || "$(CFG)" == "libpng - Win32 DLL ASM" || "$(CFG)" == "libpng - Win32 DLL Debug ASM" || "$(CFG)" == "libpng - Win32 LIB" || "$(CFG)" == "libpng - Win32 LIB Debug" || "$(CFG)" == "libpng - Win32 DLL VB"
SOURCE=..\..\png.c

!IF  "$(CFG)" == "libpng - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_USE_PNGVCRD" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "PNG_USE_PNGVCRD" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "NDEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /D PNGAPI=__stdcall /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\png.rc

!IF  "$(CFG)" == "libpng - Win32 DLL"


"$(INTDIR)\png.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug"


"$(INTDIR)\png.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL ASM"


"$(INTDIR)\png.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug ASM"


"$(INTDIR)\png.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 LIB"

!ELSEIF  "$(CFG)" == "libpng - Win32 LIB Debug"

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL VB"


"$(INTDIR)\png.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\pngerror.c

!IF  "$(CFG)" == "libpng - Win32 DLL"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL ASM"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_USE_PNGVCRD" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug ASM"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /D "_WINDOWS" /D "PNG_USE_PNGVCRD" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 LIB"

CPP_SWITCHES=/nologo /ML /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "NDEBUG" /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 LIB Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Zi /Od /I "..\.." /I "..\..\..\zlib" /D "DEBUG" /D "_DEBUG" /D PNG_DEBUG=1 /D "WIN32" /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL VB"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\.." /I "..\..\..\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "PNG_BUILD_DLL" /D "ZLIB_DLL" /D PNGAPI=__stdcall /Fp"$(INTDIR)\libpng.pch" /Yc"png.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pngerror.obj"	"$(INTDIR)\libpng.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\pngget.c

"$(INTDIR)\pngget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngmem.c

"$(INTDIR)\pngmem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngpread.c

"$(INTDIR)\pngpread.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngread.c

"$(INTDIR)\pngread.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngrio.c

"$(INTDIR)\pngrio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngrtran.c

"$(INTDIR)\pngrtran.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngrutil.c

"$(INTDIR)\pngrutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngset.c

"$(INTDIR)\pngset.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngtrans.c

"$(INTDIR)\pngtrans.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngvcrd.c

!IF  "$(CFG)" == "libpng - Win32 DLL"

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug"

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL ASM"


"$(INTDIR)\pngvcrd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug ASM"


"$(INTDIR)\pngvcrd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "libpng - Win32 LIB"

!ELSEIF  "$(CFG)" == "libpng - Win32 LIB Debug"

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL VB"

!ENDIF 

SOURCE=..\..\pngwio.c

"$(INTDIR)\pngwio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngwrite.c

"$(INTDIR)\pngwrite.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngwtran.c

"$(INTDIR)\pngwtran.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\pngwutil.c

"$(INTDIR)\pngwutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\libpng.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "libpng - Win32 DLL"

"zlib - Win32 DLL" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL" 
   cd "."

"zlib - Win32 DLLCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug"

"zlib - Win32 DLL Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL Debug" 
   cd "."

"zlib - Win32 DLL DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL ASM"

"zlib - Win32 DLL ASM" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL ASM" 
   cd "."

"zlib - Win32 DLL ASMCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL ASM" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL Debug ASM"

"zlib - Win32 DLL Debug ASM" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL Debug ASM" 
   cd "."

"zlib - Win32 DLL Debug ASMCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL Debug ASM" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "libpng - Win32 LIB"

"zlib - Win32 LIB" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 LIB" 
   cd "."

"zlib - Win32 LIBCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 LIB" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "libpng - Win32 LIB Debug"

"zlib - Win32 LIB Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 LIB Debug" 
   cd "."

"zlib - Win32 LIB DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 LIB Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "libpng - Win32 DLL VB"

"zlib - Win32 DLL VB" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL VB" 
   cd "."

"zlib - Win32 DLL VBCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\zlib.mak" CFG="zlib - Win32 DLL VB" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

