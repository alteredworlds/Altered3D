# Microsoft Developer Studio Project File - Name="AWSaverGameS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AWSaverGameS - Win32 Static Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AWSaverGameS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AWSaverGameS.mak" CFG="AWSaverGameS - Win32 Static Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AWSaverGameS - Win32 Static Release" (based on "Win32 (x86) Application")
!MESSAGE "AWSaverGameS - Win32 Static DebugInRelease" (based on "Win32 (x86) Application")
!MESSAGE "AWSaverGameS - Win32 Static Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Core/AWSaverGame", ICAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AWSaverGameS - Win32 Static Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AWSaverGame___Win32_Static_Release"
# PROP BASE Intermediate_Dir "AWSaverGame___Win32_Static_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SRelease"
# PROP Intermediate_Dir "SRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Z7 /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "../projects" /I "e:/projects" /I ".." /I "../libs" /I "../lib-ext" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AW_SHAREWARE_REGISTRATION" /D "_AWDISABLECHECKSUM" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DTools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib qmdx.lib lz32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib lz32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /libpath:"../lib-ext/qmdx"

!ELSEIF  "$(CFG)" == "AWSaverGameS - Win32 Static DebugInRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AWSaverGame___Win32_Static_DebugInRelease"
# PROP BASE Intermediate_Dir "AWSaverGame___Win32_Static_DebugInRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SReleaseD"
# PROP Intermediate_Dir "SReleaseD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MT /W3 /GX /Z7 /Od /I "e:/projects" /I ".." /I "../libs" /I "../lib-ext" /D "NDEBUG" /D "_AWDISABLECHECKSUM" /D "__AWTRACE" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AW_SHAREWARE_REGISTRATION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DToolsS.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib qmdx.lib lz32.lib opengl32.lib /nologo /subsystem:windows /pdb:none /machine:I386
# ADD LINK32 opengl32.lib qmdx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib lz32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /libpath:"../lib" /libpath:"../lib-ext/qmdx" /fixed:no

!ELSEIF  "$(CFG)" == "AWSaverGameS - Win32 Static Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AWSaverGame___Win32_Static_Debug"
# PROP BASE Intermediate_Dir "AWSaverGame___Win32_Static_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SDebug"
# PROP Intermediate_Dir "SDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /D "STRICT" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /GX /Zi /Od /I ".." /I "../libs" /I "../lib-ext" /D "__AWTRACE" /D "_DEBUG" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AW_SHAREWARE_REGISTRATION" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DToolsd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib qmdx.lib lz32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib qmdx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib lz32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"../lib-ext/qmdx"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy SDebug\AWSaverGame.exe SDebug\AlteredSaver.scr
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AWSaverGameS - Win32 Static Release"
# Name "AWSaverGameS - Win32 Static DebugInRelease"
# Name "AWSaverGameS - Win32 Static Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AWSettings\AWAutoSerialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AWConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWConfigure3D.cpp
# End Source File
# Begin Source File

SOURCE=.\AWConfigWin.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWContentPage.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWControlsSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWDisplaySettings.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWDownloadMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWGamePage.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWGetSerialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AWMain.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWMinigzip.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWNagDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AWSaverGame.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSceneSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AWSndProviderWin.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSoundSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\AWSpaceGnats.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWTabPage.cpp
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWUserInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AWUserInput.cpp
# End Source File
# Begin Source File

SOURCE=..\libs\AWSerialize\AWVerify.cpp
# End Source File
# Begin Source File

SOURCE=.\AWWinUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\libs\AWSerialize\Md5.cpp
# End Source File
# Begin Source File

SOURCE=.\statics.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AWSettings\AWAutoSerialDlg.h
# End Source File
# Begin Source File

SOURCE=.\AWConfig.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWConfigure3D.h
# End Source File
# Begin Source File

SOURCE=.\AWConfigWin.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWContentPage.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWControlsSettings.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWDisplaySettings.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWDownloadMgr.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWGamePage.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWGetSerialDlg.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWNagDlg.h
# End Source File
# Begin Source File

SOURCE=.\AWSaverGame.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSceneSettings.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSettingsCommon.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\AWSndCommon.h
# End Source File
# Begin Source File

SOURCE=.\AWSndDefs.h
# End Source File
# Begin Source File

SOURCE=.\AWSndProviderWin.h
# End Source File
# Begin Source File

SOURCE=.\AWSound.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWSoundSettings.h
# End Source File
# Begin Source File

SOURCE=.\AWSpaceGnats.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWTabPage.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\AWUserInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\AWUserInput.h
# End Source File
# Begin Source File

SOURCE=..\libs\AWSerialize\AWVerify.h
# End Source File
# Begin Source File

SOURCE=.\AWWinUtils.h
# End Source File
# Begin Source File

SOURCE=..\libs\AWSerialize\MD5.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\AWSettings\res\AlteredSettings.rc
# End Source File
# Begin Source File

SOURCE=..\AWSettings\res\AlteredSettingsRes.h
# End Source File
# Begin Source File

SOURCE=..\AWSettings\res\AWIcon.ico
# End Source File
# Begin Source File

SOURCE=..\AWSettings\res\awlogo.tnt
# End Source File
# End Group
# Begin Source File

SOURCE=..\AWSETTINGS\res\AWUpdate.exe.gz
# End Source File
# End Target
# End Project
