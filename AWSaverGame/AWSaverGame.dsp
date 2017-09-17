# Microsoft Developer Studio Project File - Name="AWSaverGame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AWSaverGame - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AWSaverGame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AWSaverGame.mak" CFG="AWSaverGame - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AWSaverGame - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AWSaverGame - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AWSaverGame - Win32 DebugInRelease" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/3D Graphics/AWSaverGame", CGNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AWSaverGame - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Z7 /Od /I ".." /I "../libs" /I "../lib-ext" /D "NDEBUG" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /D "WIN32" /D "_WINDOWS" /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 qmdx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib lz32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /machine:I386 /libpath:"../lib-ext/qmdx"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\AWSaverGame.exe Release\AlteredSaver.scr
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AWSaverGame - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".." /I "../libs" /I "../lib-ext" /D "__AWTRACE" /D "__AWTRACEHI" /D "_DEBUG" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AW_SHAREWARE_REGISTRATION" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 qmdx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib lz32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../lib-ext/qmdx"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\AWSaverGame.exe Debug\AlteredSaver.scr
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AWSaverGame - Win32 DebugInRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AWSaverGame___Win32_DebugInRelease"
# PROP BASE Intermediate_Dir "AWSaverGame___Win32_DebugInRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseD"
# PROP Intermediate_Dir "ReleaseD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AWDISABLE_USER_VALIDATION" /D "_AWDISABLECHECKSUM" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Z7 /Od /I ".." /I "../libs" /I "../lib-ext" /D "__AWTRACE" /D "__AWTRACEHI" /D "NDEBUG" /D "_AWDISABLECHECKSUM" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_AW_SHAREWARE_REGISTRATION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AW3DTools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib qmdx.lib lz32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib lz32.lib wsock32.lib comctl32.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /libpath:"../lib-ext/qmdx" /FIXED:NO

!ENDIF 

# Begin Target

# Name "AWSaverGame - Win32 Release"
# Name "AWSaverGame - Win32 Debug"
# Name "AWSaverGame - Win32 DebugInRelease"
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
