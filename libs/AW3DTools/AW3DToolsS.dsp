# Microsoft Developer Studio Project File - Name="AW3DToolsS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AW3DToolsS - Win32 Static Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AW3DToolsS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AW3DToolsS.mak" CFG="AW3DToolsS - Win32 Static Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AW3DToolsS - Win32 Static DebugInRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "AW3DToolsS - Win32 Static Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AW3DToolsS - Win32 Static Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/3D Graphics/AW3DTools/AW3DToolsS", IINAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AW3DToolsS - Win32 Static DebugInRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AW3DToolsS___Win32_Static_DebugInRelease"
# PROP BASE Intermediate_Dir "AW3DToolsS___Win32_Static_DebugInRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SReleaseD"
# PROP Intermediate_Dir "SReleaseD"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Z7 /Od /D "NDEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /D "__AWTRACE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MT /W3 /GR /GX /Z7 /Od /I ".." /D "NDEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /D "__AWTRACE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\AW3DToolsRDS.lib"
# ADD LIB32 /nologo /out:"SReleaseD\AW3DToolsRDS.lib"

!ELSEIF  "$(CFG)" == "AW3DToolsS - Win32 Static Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AW3DToolsS___Win32_Static_Release"
# PROP BASE Intermediate_Dir "AW3DToolsS___Win32_Static_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "SRelease"
# PROP Intermediate_Dir "SRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "../projects" /I ".." /D "NDEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AW3DToolsS - Win32 Static Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AW3DToolsS___Win32_Static_Debug"
# PROP BASE Intermediate_Dir "AW3DToolsS___Win32_Static_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SDebug"
# PROP Intermediate_Dir "SDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /G5 /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MTd /W3 /GX /Zi /Od /I ".." /D "__AWTRACE" /D "_DEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\AW3DToolsdS.lib"
# ADD LIB32 /nologo /out:"SDebug\AW3DToolsdS.lib"

!ENDIF 

# Begin Target

# Name "AW3DToolsS - Win32 Static DebugInRelease"
# Name "AW3DToolsS - Win32 Static Release"
# Name "AW3DToolsS - Win32 Static Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AWAnimatedFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\AWAnimatedPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\AWAnimatedQuat.cpp
# End Source File
# Begin Source File

SOURCE=.\AWAnimatedTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\AWBillboardController.cpp
# End Source File
# Begin Source File

SOURCE=.\AWBoidController.cpp
# End Source File
# Begin Source File

SOURCE=.\AWBumpMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\AWCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\AWCubeEnvTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\AWEffectMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\AWEnvironController.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFace.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFile.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFlightController.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFlock.cpp
# End Source File
# Begin Source File

SOURCE=.\AWFog.cpp
# End Source File
# Begin Source File

SOURCE=.\AWGLRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\AWImageProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\AWLight.cpp
# End Source File
# Begin Source File

SOURCE=.\AWMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\AWMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=.\AWMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\AWNode.cpp
# End Source File
# Begin Source File

SOURCE=.\AWNormalMapTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\AWObject.cpp
# End Source File
# Begin Source File

SOURCE=.\AWPBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\AWPerfCnt.cpp
# End Source File
# Begin Source File

SOURCE=.\AWPersistable.cpp
# End Source File
# Begin Source File

SOURCE=.\AWPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\AWPositionKey.cpp
# End Source File
# Begin Source File

SOURCE=.\AWQuat.cpp
# End Source File
# Begin Source File

SOURCE=.\AWRenderPass.cpp
# End Source File
# Begin Source File

SOURCE=.\AWRenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\AWRocket.cpp
# End Source File
# Begin Source File

SOURCE=.\AWRotationKey.cpp
# End Source File
# Begin Source File

SOURCE=.\AWScene.cpp
# End Source File
# Begin Source File

SOURCE=.\AWSea.cpp
# End Source File
# Begin Source File

SOURCE=.\AWShadowMap.cpp
# End Source File
# Begin Source File

SOURCE=.\AWStore.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTextureEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTextureUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTimerBarCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTransformController.cpp
# End Source File
# Begin Source File

SOURCE=.\AWTVertexList.cpp
# End Source File
# Begin Source File

SOURCE=.\AWVertexProgram.cpp
# End Source File
# Begin Source File

SOURCE=.\AWWorker.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AWAnimatedFloat.h
# End Source File
# Begin Source File

SOURCE=.\AWAnimatedPoint.h
# End Source File
# Begin Source File

SOURCE=.\AWAnimatedQuat.h
# End Source File
# Begin Source File

SOURCE=.\AWAnimatedTexture.h
# End Source File
# Begin Source File

SOURCE=.\AWBillboardController.h
# End Source File
# Begin Source File

SOURCE=.\AWBoidController.h
# End Source File
# Begin Source File

SOURCE=.\AWBumpMesh.h
# End Source File
# Begin Source File

SOURCE=.\AWCamera.h
# End Source File
# Begin Source File

SOURCE=.\AWCString.h
# End Source File
# Begin Source File

SOURCE=.\AWCubeEnvTexture.h
# End Source File
# Begin Source File

SOURCE=.\AWCubeGen.h
# End Source File
# Begin Source File

SOURCE=.\AWEffectMgr.h
# End Source File
# Begin Source File

SOURCE=.\AWEnvironController.h
# End Source File
# Begin Source File

SOURCE=.\AWFace.h
# End Source File
# Begin Source File

SOURCE=.\AWFactory.h
# End Source File
# Begin Source File

SOURCE=.\AWFile.h
# End Source File
# Begin Source File

SOURCE=.\AWFlightController.h
# End Source File
# Begin Source File

SOURCE=.\AWFlock.h
# End Source File
# Begin Source File

SOURCE=.\AWFog.h
# End Source File
# Begin Source File

SOURCE=.\AWGLRenderer.h
# End Source File
# Begin Source File

SOURCE=.\AWImageProcessor.h
# End Source File
# Begin Source File

SOURCE=.\AWLight.h
# End Source File
# Begin Source File

SOURCE=.\AWList.h
# End Source File
# Begin Source File

SOURCE=.\AWMaterial.h
# End Source File
# Begin Source File

SOURCE=.\AWMatrix3.h
# End Source File
# Begin Source File

SOURCE=.\AWMatrix4.h
# End Source File
# Begin Source File

SOURCE=.\AWMesh.h
# End Source File
# Begin Source File

SOURCE=.\AWNode.h
# End Source File
# Begin Source File

SOURCE=.\AWNormalMapTexture.h
# End Source File
# Begin Source File

SOURCE=.\AWObject.h
# End Source File
# Begin Source File

SOURCE=.\AWOrdered.h
# End Source File
# Begin Source File

SOURCE=.\AWPBuffer.h
# End Source File
# Begin Source File

SOURCE=.\AWPerfCnt.h
# End Source File
# Begin Source File

SOURCE=.\AWPersistable.h
# End Source File
# Begin Source File

SOURCE=.\AWPoint.h
# End Source File
# Begin Source File

SOURCE=.\AWPositionKey.h
# End Source File
# Begin Source File

SOURCE=.\AWPtrList.h
# End Source File
# Begin Source File

SOURCE=.\AWQuat.h
# End Source File
# Begin Source File

SOURCE=.\AWRenderPass.h
# End Source File
# Begin Source File

SOURCE=.\AWRenderState.h
# End Source File
# Begin Source File

SOURCE=.\AWRocket.h
# End Source File
# Begin Source File

SOURCE=.\AWRotationKey.h
# End Source File
# Begin Source File

SOURCE=.\AWScene.h
# End Source File
# Begin Source File

SOURCE=.\AWSea.h
# End Source File
# Begin Source File

SOURCE=.\AWShadowMap.h
# End Source File
# Begin Source File

SOURCE=.\AWSphrCone.h
# End Source File
# Begin Source File

SOURCE=.\AWStore.h
# End Source File
# Begin Source File

SOURCE=.\AWTexture.h
# End Source File
# Begin Source File

SOURCE=.\AWTextureEffect.h
# End Source File
# Begin Source File

SOURCE=.\AWTextureUnit.h
# End Source File
# Begin Source File

SOURCE=.\AWTimerBarCtl.h
# End Source File
# Begin Source File

SOURCE=.\AWTooldefs.h
# End Source File
# Begin Source File

SOURCE=.\AWToolsCommon.h
# End Source File
# Begin Source File

SOURCE=.\AWTransformController.h
# End Source File
# Begin Source File

SOURCE=.\AWTVert.h
# End Source File
# Begin Source File

SOURCE=.\AWTVertexList.h
# End Source File
# Begin Source File

SOURCE=.\AWVertexProgram.h
# End Source File
# Begin Source File

SOURCE=.\AWWorker.h
# End Source File
# End Group
# End Target
# End Project
