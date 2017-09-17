# Microsoft Developer Studio Project File - Name="zlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 DebugInRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Core/lib-ext/zlib/nt", BJBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib___Win32_Release0"
# PROP BASE Intermediate_Dir "zlib___Win32_Release0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "win32\lib"
# PROP Intermediate_Dir "win32\lib"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 DebugInRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib___Win32_DebugInRelease"
# PROP BASE Intermediate_Dir "zlib___Win32_DebugInRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "win32\libd"
# PROP Intermediate_Dir "win32\libd"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Z7 /Od /D "NDEBUG" /D "WIN32" /D "_LIB" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zlib___Win32_Debug"
# PROP BASE Intermediate_Dir "zlib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\win32\lib_dbg"
# PROP Intermediate_Dir ".\win32\lib_dbg"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\win32\lib_dbg\zlibd.lib"
# ADD LIB32 /nologo /out:".\win32\lib_dbg\zlibd.lib"

!ENDIF 

# Begin Target

# Name "zlib - Win32 Release"
# Name "zlib - Win32 DebugInRelease"
# Name "zlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\adler32.c
# End Source File
# Begin Source File

SOURCE=..\compress.c
# End Source File
# Begin Source File

SOURCE=..\crc32.c
# End Source File
# Begin Source File

SOURCE=..\deflate.c
# End Source File
# Begin Source File

SOURCE=..\gzio.c
# End Source File
# Begin Source File

SOURCE=..\infblock.c
# End Source File
# Begin Source File

SOURCE=..\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\inffast.c
# End Source File
# Begin Source File

SOURCE=..\inflate.c
# End Source File
# Begin Source File

SOURCE=..\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\infutil.c
# End Source File
# Begin Source File

SOURCE=..\trees.c
# End Source File
# Begin Source File

SOURCE=..\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\zlib.def
# End Source File
# Begin Source File

SOURCE=..\msdos\zlib.rc
# End Source File
# Begin Source File

SOURCE=..\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\deflate.h
# End Source File
# Begin Source File

SOURCE=..\infblock.h
# End Source File
# Begin Source File

SOURCE=..\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\inffast.h
# End Source File
# Begin Source File

SOURCE=..\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\infutil.h
# End Source File
# Begin Source File

SOURCE=..\trees.h
# End Source File
# Begin Source File

SOURCE=..\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib.h
# End Source File
# Begin Source File

SOURCE=..\zutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\VMSREADME.txt
# End Source File
# End Target
# End Project
