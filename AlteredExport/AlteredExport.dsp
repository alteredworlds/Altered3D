# Microsoft Developer Studio Project File - Name="AlteredExport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ALTEREDEXPORT - WIN32 MAX4 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AlteredExport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AlteredExport.mak" CFG="ALTEREDEXPORT - WIN32 MAX4 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AlteredExport - Win32 MAX4 DebugInRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AlteredExport - Win32 MAX4 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AlteredExport - Win32 MAX3 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AlteredExport - Win32 MAX3 DebugInRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AlteredExport - Win32 gmax4 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/3D Graphics/AlteredExport", ZGNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AlteredExport - Win32 MAX4 DebugInRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AlteredExport___Win32_MAX4_DebugInRelease"
# PROP BASE Intermediate_Dir "AlteredExport___Win32_MAX4_DebugInRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R4ReleaseD"
# PROP Intermediate_Dir "R4ReleaseD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Z7 /Od /D "WIN32" /D "NDEBUG" /D "__AWTRACE" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /Z7 /Od /I "C:\3DSMAX4\Maxsdk\Include" /I "E:\projects" /D "WIN32" /D "NDEBUG" /D "__AWTRACE" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DToolsRD.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib util.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib /nologo /stack:0x400000 /dll /pdb:none /debug /machine:I386 /out:"ReleaseD\AlteredExportD.dle"
# ADD LINK32 AW3DToolsRD.lib maxutil.lib util.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib zlibS.lib /nologo /stack:0x400000 /dll /pdb:none /debug /machine:I386 /out:"R4ReleaseD\AlteredExportD.dle" /libpath:"C:\3dsmax4\maxsdk\lib" /libpath:"E:\projects\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy R4ReleaseD\AlteredExportd.dle c:\3dsmax4\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AlteredExport - Win32 MAX4 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AlteredExport___Win32_MAX4_Release"
# PROP BASE Intermediate_Dir "AlteredExport___Win32_MAX4_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R4Release"
# PROP Intermediate_Dir "R4Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\3DSMAX4\Maxsdk\Include" /I "E:\projects" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DTools.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib /nologo /dll /machine:I386 /out:"Release/AlteredExport.dle"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 AW3DTools.lib maxutil.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib zlibS.lib /nologo /dll /pdb:none /machine:I386 /out:"R4Release/AlteredExport.dle" /libpath:"C:\3dsmax4\maxsdk\lib" /libpath:"E:\projects\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy R4Release\AlteredExport.dle c:\3dsmax4\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AlteredExport - Win32 MAX3 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AlteredExport___Win32_MAX3_Release"
# PROP BASE Intermediate_Dir "AlteredExport___Win32_MAX3_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R3Release"
# PROP Intermediate_Dir "R3Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\3DSMAX3\Maxsdk\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\3DSMAX3\Maxsdk\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DTools.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib maxutil.lib core.lib bmm.lib mesh.lib geom.lib gfx.lib /nologo /dll /pdb:none /machine:I386 /out:"Release/AlteredExport.dle" /libpath:"C:\3dsmax3\maxsdk\lib"
# ADD LINK32 AW3DTools.lib maxutil.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib zlibS.lib /nologo /dll /pdb:none /machine:I386 /out:"R3Release/AlteredExport.dle" /libpath:"C:\3dsmax3\maxsdk\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy R3Release\AlteredExport.dle c:\3dsmax3\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AlteredExport - Win32 MAX3 DebugInRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AlteredExport___Win32_MAX3_DebugInRelease"
# PROP BASE Intermediate_Dir "AlteredExport___Win32_MAX3_DebugInRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "R3ReleaseD"
# PROP Intermediate_Dir "R3ReleaseD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Z7 /Od /I "C:\3DSMAX3\Maxsdk\Include" /D "WIN32" /D "NDEBUG" /D "__AWTRACE" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MD /W3 /GX /Z7 /Od /I "C:\3DSMAX3\Maxsdk\Include" /D "WIN32" /D "NDEBUG" /D "__AWTRACE" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DToolsRD.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib util.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib /nologo /stack:0x400000 /dll /pdb:none /debug /machine:I386 /out:"ReleaseD\AlteredExportD.dle" /libpath:"C:\3dsmax3\maxsdk\lib"
# ADD LINK32 AW3DToolsRD.lib util.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib zlibS.lib /nologo /stack:0x400000 /dll /pdb:none /debug /machine:I386 /out:"R3ReleaseD\AlteredExportD.dle" /libpath:"C:\3dsmax3\maxsdk\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy R3ReleaseD\AlteredExportd.dle c:\3dsmax3\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AlteredExport - Win32 gmax4 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AlteredExport___Win32_gmax4_Release"
# PROP BASE Intermediate_Dir "AlteredExport___Win32_gmax4_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "GMAXRelease"
# PROP Intermediate_Dir "GMAXRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\3DSMAX4\Maxsdk\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "C:\3DGraphics\gmaxdev\maxsdk\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DTools.lib maxutil.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib zlibS.lib /nologo /dll /pdb:none /machine:I386 /out:"R4Release/AlteredExport.dle" /libpath:"C:\3dsmax4\maxsdk\lib"
# ADD LINK32 AW3DTools.lib maxutil.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib zlibS.lib /nologo /dll /pdb:none /machine:I386 /out:"GMAXRelease/AlteredExport.dle" /libpath:"C:\3DGraphics\gmaxdev\maxsdk\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy R4Release\AlteredExport.dle c:\3dsmax4\plugins
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AlteredExport - Win32 MAX4 DebugInRelease"
# Name "AlteredExport - Win32 MAX4 Release"
# Name "AlteredExport - Win32 MAX3 Release"
# Name "AlteredExport - Win32 MAX3 DebugInRelease"
# Name "AlteredExport - Win32 gmax4 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AlteredExport.cpp
# End Source File
# Begin Source File

SOURCE=.\AlteredExport.def
# End Source File
# Begin Source File

SOURCE=.\AWAnimOut.cpp
# End Source File
# Begin Source File

SOURCE=..\awsavergame\AWConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSaverGame\AWConfigWin.cpp
# End Source File
# Begin Source File

SOURCE=.\AWCreateExecutable.cpp
# End Source File
# Begin Source File

SOURCE=.\AWExportCameras.cpp
# End Source File
# Begin Source File

SOURCE=.\AWExportHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\AWExportLights.cpp
# End Source File
# Begin Source File

SOURCE=.\AWExportMain.cpp
# End Source File
# Begin Source File

SOURCE=.\AWExportMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFlockHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFogHelp.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWMinigzip.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTransforms.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSerialize\AWVerify.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSaverGame\AWWinUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSerialize\Md5.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlteredExport.h
# End Source File
# Begin Source File

SOURCE=..\AWSaverGame\AWConfig.h
# End Source File
# Begin Source File

SOURCE=..\AWSaverGame\AWConfigWin.h
# End Source File
# Begin Source File

SOURCE=.\AWFlockHelp.h
# End Source File
# Begin Source File

SOURCE=.\AWFogHelp.h
# End Source File
# Begin Source File

SOURCE=..\AWSerialize\AWVerify.h
# End Source File
# Begin Source File

SOURCE=..\AWSaverGame\AWWinUtils.h
# End Source File
# Begin Source File

SOURCE=..\AWSerialize\MD5.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AlteredExport.rc
# End Source File
# Begin Source File

SOURCE=.\res\AWIcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\awsavergame\SRelease\AWSaverGame.exe
# End Source File
# End Target
# End Project
