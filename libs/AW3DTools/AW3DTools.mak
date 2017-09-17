# Microsoft Developer Studio Generated NMAKE File, Based on AW3DTools.dsp
!IF "$(CFG)" == ""
CFG=AW3DTools - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AW3DTools - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AW3DTools - Win32 Release" && "$(CFG)" != "AW3DTools - Win32 Debug" && "$(CFG)" != "AW3DTools - Win32 DebugInRelease"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AW3DTools.mak" CFG="AW3DTools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AW3DTools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AW3DTools - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AW3DTools - Win32 DebugInRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "AW3DTools - Win32 Release"

OUTDIR=.\../lib
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../lib
# End Custom Macros

ALL : "$(OUTDIR)\AW3DTools.dll"


CLEAN :
	-@erase "$(INTDIR)\AWAnimatedFloat.obj"
	-@erase "$(INTDIR)\AWAnimatedPoint.obj"
	-@erase "$(INTDIR)\AWAnimatedQuat.obj"
	-@erase "$(INTDIR)\AWAnimatedTexture.obj"
	-@erase "$(INTDIR)\AWBillboardController.obj"
	-@erase "$(INTDIR)\AWBoidController.obj"
	-@erase "$(INTDIR)\AWBumpMesh.obj"
	-@erase "$(INTDIR)\AWCamera.obj"
	-@erase "$(INTDIR)\AWCubeEnvTexture.obj"
	-@erase "$(INTDIR)\AWEffectMgr.obj"
	-@erase "$(INTDIR)\AWEnvironController.obj"
	-@erase "$(INTDIR)\AWFace.obj"
	-@erase "$(INTDIR)\AWFactory.obj"
	-@erase "$(INTDIR)\AWFile.obj"
	-@erase "$(INTDIR)\AWFlightController.obj"
	-@erase "$(INTDIR)\AWFlock.obj"
	-@erase "$(INTDIR)\AWFog.obj"
	-@erase "$(INTDIR)\AWGLRenderer.obj"
	-@erase "$(INTDIR)\AWImageProcessor.obj"
	-@erase "$(INTDIR)\AWLight.obj"
	-@erase "$(INTDIR)\AWMaterial.obj"
	-@erase "$(INTDIR)\AWMatrix4.obj"
	-@erase "$(INTDIR)\AWMesh.obj"
	-@erase "$(INTDIR)\AWNode.obj"
	-@erase "$(INTDIR)\AWNormalMapTexture.obj"
	-@erase "$(INTDIR)\AWObject.obj"
	-@erase "$(INTDIR)\AWPerfCnt.obj"
	-@erase "$(INTDIR)\AWPersistable.obj"
	-@erase "$(INTDIR)\AWPoint.obj"
	-@erase "$(INTDIR)\AWPositionKey.obj"
	-@erase "$(INTDIR)\AWQuat.obj"
	-@erase "$(INTDIR)\AWRenderPass.obj"
	-@erase "$(INTDIR)\AWRenderState.obj"
	-@erase "$(INTDIR)\AWRocket.obj"
	-@erase "$(INTDIR)\AWRotationKey.obj"
	-@erase "$(INTDIR)\AWScene.obj"
	-@erase "$(INTDIR)\AWSea.obj"
	-@erase "$(INTDIR)\AWStdMaterial.obj"
	-@erase "$(INTDIR)\AWStore.obj"
	-@erase "$(INTDIR)\AWTexture.obj"
	-@erase "$(INTDIR)\AWTextureEffect.obj"
	-@erase "$(INTDIR)\AWTextureUnit.obj"
	-@erase "$(INTDIR)\AWTimerBarCtl.obj"
	-@erase "$(INTDIR)\AWTransformController.obj"
	-@erase "$(INTDIR)\AWTVertexList.obj"
	-@erase "$(INTDIR)\AWVertexProgram.obj"
	-@erase "$(INTDIR)\AWWorker.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AW3DTools.dll"
	-@erase "$(OUTDIR)\AW3DTools.exp"
	-@erase "$(OUTDIR)\AW3DTools.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_AWBUILDSHARED" /D "STRICT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AW3DTools.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib /nologo /dll /pdb:none /machine:I386 /out:"$(OUTDIR)\AW3DTools.dll" /implib:"$(OUTDIR)\AW3DTools.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AWAnimatedFloat.obj" \
	"$(INTDIR)\AWAnimatedPoint.obj" \
	"$(INTDIR)\AWAnimatedQuat.obj" \
	"$(INTDIR)\AWAnimatedTexture.obj" \
	"$(INTDIR)\AWBillboardController.obj" \
	"$(INTDIR)\AWBoidController.obj" \
	"$(INTDIR)\AWBumpMesh.obj" \
	"$(INTDIR)\AWCamera.obj" \
	"$(INTDIR)\AWCubeEnvTexture.obj" \
	"$(INTDIR)\AWEffectMgr.obj" \
	"$(INTDIR)\AWEnvironController.obj" \
	"$(INTDIR)\AWFace.obj" \
	"$(INTDIR)\AWFactory.obj" \
	"$(INTDIR)\AWFile.obj" \
	"$(INTDIR)\AWFlightController.obj" \
	"$(INTDIR)\AWFlock.obj" \
	"$(INTDIR)\AWFog.obj" \
	"$(INTDIR)\AWGLRenderer.obj" \
	"$(INTDIR)\AWImageProcessor.obj" \
	"$(INTDIR)\AWLight.obj" \
	"$(INTDIR)\AWMaterial.obj" \
	"$(INTDIR)\AWMatrix4.obj" \
	"$(INTDIR)\AWMesh.obj" \
	"$(INTDIR)\AWNode.obj" \
	"$(INTDIR)\AWNormalMapTexture.obj" \
	"$(INTDIR)\AWObject.obj" \
	"$(INTDIR)\AWPerfCnt.obj" \
	"$(INTDIR)\AWPersistable.obj" \
	"$(INTDIR)\AWPoint.obj" \
	"$(INTDIR)\AWPositionKey.obj" \
	"$(INTDIR)\AWQuat.obj" \
	"$(INTDIR)\AWRenderPass.obj" \
	"$(INTDIR)\AWRenderState.obj" \
	"$(INTDIR)\AWRocket.obj" \
	"$(INTDIR)\AWRotationKey.obj" \
	"$(INTDIR)\AWScene.obj" \
	"$(INTDIR)\AWSea.obj" \
	"$(INTDIR)\AWStdMaterial.obj" \
	"$(INTDIR)\AWStore.obj" \
	"$(INTDIR)\AWTexture.obj" \
	"$(INTDIR)\AWTextureEffect.obj" \
	"$(INTDIR)\AWTextureUnit.obj" \
	"$(INTDIR)\AWTimerBarCtl.obj" \
	"$(INTDIR)\AWTransformController.obj" \
	"$(INTDIR)\AWTVertexList.obj" \
	"$(INTDIR)\AWVertexProgram.obj" \
	"$(INTDIR)\AWWorker.obj"

"$(OUTDIR)\AW3DTools.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AW3DTools - Win32 Debug"

OUTDIR=.\../lib
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\../lib
# End Custom Macros

ALL : "$(OUTDIR)\AW3DToolsd.dll"


CLEAN :
	-@erase "$(INTDIR)\AWAnimatedFloat.obj"
	-@erase "$(INTDIR)\AWAnimatedPoint.obj"
	-@erase "$(INTDIR)\AWAnimatedQuat.obj"
	-@erase "$(INTDIR)\AWAnimatedTexture.obj"
	-@erase "$(INTDIR)\AWBillboardController.obj"
	-@erase "$(INTDIR)\AWBoidController.obj"
	-@erase "$(INTDIR)\AWBumpMesh.obj"
	-@erase "$(INTDIR)\AWCamera.obj"
	-@erase "$(INTDIR)\AWCubeEnvTexture.obj"
	-@erase "$(INTDIR)\AWEffectMgr.obj"
	-@erase "$(INTDIR)\AWEnvironController.obj"
	-@erase "$(INTDIR)\AWFace.obj"
	-@erase "$(INTDIR)\AWFactory.obj"
	-@erase "$(INTDIR)\AWFile.obj"
	-@erase "$(INTDIR)\AWFlightController.obj"
	-@erase "$(INTDIR)\AWFlock.obj"
	-@erase "$(INTDIR)\AWFog.obj"
	-@erase "$(INTDIR)\AWGLRenderer.obj"
	-@erase "$(INTDIR)\AWImageProcessor.obj"
	-@erase "$(INTDIR)\AWLight.obj"
	-@erase "$(INTDIR)\AWMaterial.obj"
	-@erase "$(INTDIR)\AWMatrix4.obj"
	-@erase "$(INTDIR)\AWMesh.obj"
	-@erase "$(INTDIR)\AWNode.obj"
	-@erase "$(INTDIR)\AWNormalMapTexture.obj"
	-@erase "$(INTDIR)\AWObject.obj"
	-@erase "$(INTDIR)\AWPerfCnt.obj"
	-@erase "$(INTDIR)\AWPersistable.obj"
	-@erase "$(INTDIR)\AWPoint.obj"
	-@erase "$(INTDIR)\AWPositionKey.obj"
	-@erase "$(INTDIR)\AWQuat.obj"
	-@erase "$(INTDIR)\AWRenderPass.obj"
	-@erase "$(INTDIR)\AWRenderState.obj"
	-@erase "$(INTDIR)\AWRocket.obj"
	-@erase "$(INTDIR)\AWRotationKey.obj"
	-@erase "$(INTDIR)\AWScene.obj"
	-@erase "$(INTDIR)\AWSea.obj"
	-@erase "$(INTDIR)\AWStdMaterial.obj"
	-@erase "$(INTDIR)\AWStore.obj"
	-@erase "$(INTDIR)\AWTexture.obj"
	-@erase "$(INTDIR)\AWTextureEffect.obj"
	-@erase "$(INTDIR)\AWTextureUnit.obj"
	-@erase "$(INTDIR)\AWTimerBarCtl.obj"
	-@erase "$(INTDIR)\AWTransformController.obj"
	-@erase "$(INTDIR)\AWTVertexList.obj"
	-@erase "$(INTDIR)\AWVertexProgram.obj"
	-@erase "$(INTDIR)\AWWorker.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AW3DToolsd.dll"
	-@erase "$(OUTDIR)\AW3DToolsd.exp"
	-@erase "$(OUTDIR)\AW3DToolsd.lib"
	-@erase "$(OUTDIR)\AW3DToolsd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /GX /Zi /Od /D "__AWTRACE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_AWBUILDSHARED" /D "STRICT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AW3DTools.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AW3DToolsd.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AW3DToolsd.dll" /implib:"$(OUTDIR)\AW3DToolsd.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AWAnimatedFloat.obj" \
	"$(INTDIR)\AWAnimatedPoint.obj" \
	"$(INTDIR)\AWAnimatedQuat.obj" \
	"$(INTDIR)\AWAnimatedTexture.obj" \
	"$(INTDIR)\AWBillboardController.obj" \
	"$(INTDIR)\AWBoidController.obj" \
	"$(INTDIR)\AWBumpMesh.obj" \
	"$(INTDIR)\AWCamera.obj" \
	"$(INTDIR)\AWCubeEnvTexture.obj" \
	"$(INTDIR)\AWEffectMgr.obj" \
	"$(INTDIR)\AWEnvironController.obj" \
	"$(INTDIR)\AWFace.obj" \
	"$(INTDIR)\AWFactory.obj" \
	"$(INTDIR)\AWFile.obj" \
	"$(INTDIR)\AWFlightController.obj" \
	"$(INTDIR)\AWFlock.obj" \
	"$(INTDIR)\AWFog.obj" \
	"$(INTDIR)\AWGLRenderer.obj" \
	"$(INTDIR)\AWImageProcessor.obj" \
	"$(INTDIR)\AWLight.obj" \
	"$(INTDIR)\AWMaterial.obj" \
	"$(INTDIR)\AWMatrix4.obj" \
	"$(INTDIR)\AWMesh.obj" \
	"$(INTDIR)\AWNode.obj" \
	"$(INTDIR)\AWNormalMapTexture.obj" \
	"$(INTDIR)\AWObject.obj" \
	"$(INTDIR)\AWPerfCnt.obj" \
	"$(INTDIR)\AWPersistable.obj" \
	"$(INTDIR)\AWPoint.obj" \
	"$(INTDIR)\AWPositionKey.obj" \
	"$(INTDIR)\AWQuat.obj" \
	"$(INTDIR)\AWRenderPass.obj" \
	"$(INTDIR)\AWRenderState.obj" \
	"$(INTDIR)\AWRocket.obj" \
	"$(INTDIR)\AWRotationKey.obj" \
	"$(INTDIR)\AWScene.obj" \
	"$(INTDIR)\AWSea.obj" \
	"$(INTDIR)\AWStdMaterial.obj" \
	"$(INTDIR)\AWStore.obj" \
	"$(INTDIR)\AWTexture.obj" \
	"$(INTDIR)\AWTextureEffect.obj" \
	"$(INTDIR)\AWTextureUnit.obj" \
	"$(INTDIR)\AWTimerBarCtl.obj" \
	"$(INTDIR)\AWTransformController.obj" \
	"$(INTDIR)\AWTVertexList.obj" \
	"$(INTDIR)\AWVertexProgram.obj" \
	"$(INTDIR)\AWWorker.obj"

"$(OUTDIR)\AW3DToolsd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AW3DTools - Win32 DebugInRelease"

OUTDIR=.\../lib
INTDIR=.\ReleaseD
# Begin Custom Macros
OutDir=.\../lib
# End Custom Macros

ALL : "$(OUTDIR)\AW3DToolsRD.dll"


CLEAN :
	-@erase "$(INTDIR)\AWAnimatedFloat.obj"
	-@erase "$(INTDIR)\AWAnimatedPoint.obj"
	-@erase "$(INTDIR)\AWAnimatedQuat.obj"
	-@erase "$(INTDIR)\AWAnimatedTexture.obj"
	-@erase "$(INTDIR)\AWBillboardController.obj"
	-@erase "$(INTDIR)\AWBoidController.obj"
	-@erase "$(INTDIR)\AWBumpMesh.obj"
	-@erase "$(INTDIR)\AWCamera.obj"
	-@erase "$(INTDIR)\AWCubeEnvTexture.obj"
	-@erase "$(INTDIR)\AWEffectMgr.obj"
	-@erase "$(INTDIR)\AWEnvironController.obj"
	-@erase "$(INTDIR)\AWFace.obj"
	-@erase "$(INTDIR)\AWFactory.obj"
	-@erase "$(INTDIR)\AWFile.obj"
	-@erase "$(INTDIR)\AWFlightController.obj"
	-@erase "$(INTDIR)\AWFlock.obj"
	-@erase "$(INTDIR)\AWFog.obj"
	-@erase "$(INTDIR)\AWGLRenderer.obj"
	-@erase "$(INTDIR)\AWImageProcessor.obj"
	-@erase "$(INTDIR)\AWLight.obj"
	-@erase "$(INTDIR)\AWMaterial.obj"
	-@erase "$(INTDIR)\AWMatrix4.obj"
	-@erase "$(INTDIR)\AWMesh.obj"
	-@erase "$(INTDIR)\AWNode.obj"
	-@erase "$(INTDIR)\AWNormalMapTexture.obj"
	-@erase "$(INTDIR)\AWObject.obj"
	-@erase "$(INTDIR)\AWPerfCnt.obj"
	-@erase "$(INTDIR)\AWPersistable.obj"
	-@erase "$(INTDIR)\AWPoint.obj"
	-@erase "$(INTDIR)\AWPositionKey.obj"
	-@erase "$(INTDIR)\AWQuat.obj"
	-@erase "$(INTDIR)\AWRenderPass.obj"
	-@erase "$(INTDIR)\AWRenderState.obj"
	-@erase "$(INTDIR)\AWRocket.obj"
	-@erase "$(INTDIR)\AWRotationKey.obj"
	-@erase "$(INTDIR)\AWScene.obj"
	-@erase "$(INTDIR)\AWSea.obj"
	-@erase "$(INTDIR)\AWStdMaterial.obj"
	-@erase "$(INTDIR)\AWStore.obj"
	-@erase "$(INTDIR)\AWTexture.obj"
	-@erase "$(INTDIR)\AWTextureEffect.obj"
	-@erase "$(INTDIR)\AWTextureUnit.obj"
	-@erase "$(INTDIR)\AWTimerBarCtl.obj"
	-@erase "$(INTDIR)\AWTransformController.obj"
	-@erase "$(INTDIR)\AWTVertexList.obj"
	-@erase "$(INTDIR)\AWVertexProgram.obj"
	-@erase "$(INTDIR)\AWWorker.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AW3DToolsRD.dll"
	-@erase "$(OUTDIR)\AW3DToolsRD.exp"
	-@erase "$(OUTDIR)\AW3DToolsRD.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Z7 /Od /D "__AWTRACE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_AWBUILDSHARED" /D "STRICT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AW3DTools.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"$(OUTDIR)\AW3DToolsRD.dll" /implib:"$(OUTDIR)\AW3DToolsRD.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AWAnimatedFloat.obj" \
	"$(INTDIR)\AWAnimatedPoint.obj" \
	"$(INTDIR)\AWAnimatedQuat.obj" \
	"$(INTDIR)\AWAnimatedTexture.obj" \
	"$(INTDIR)\AWBillboardController.obj" \
	"$(INTDIR)\AWBoidController.obj" \
	"$(INTDIR)\AWBumpMesh.obj" \
	"$(INTDIR)\AWCamera.obj" \
	"$(INTDIR)\AWCubeEnvTexture.obj" \
	"$(INTDIR)\AWEffectMgr.obj" \
	"$(INTDIR)\AWEnvironController.obj" \
	"$(INTDIR)\AWFace.obj" \
	"$(INTDIR)\AWFactory.obj" \
	"$(INTDIR)\AWFile.obj" \
	"$(INTDIR)\AWFlightController.obj" \
	"$(INTDIR)\AWFlock.obj" \
	"$(INTDIR)\AWFog.obj" \
	"$(INTDIR)\AWGLRenderer.obj" \
	"$(INTDIR)\AWImageProcessor.obj" \
	"$(INTDIR)\AWLight.obj" \
	"$(INTDIR)\AWMaterial.obj" \
	"$(INTDIR)\AWMatrix4.obj" \
	"$(INTDIR)\AWMesh.obj" \
	"$(INTDIR)\AWNode.obj" \
	"$(INTDIR)\AWNormalMapTexture.obj" \
	"$(INTDIR)\AWObject.obj" \
	"$(INTDIR)\AWPerfCnt.obj" \
	"$(INTDIR)\AWPersistable.obj" \
	"$(INTDIR)\AWPoint.obj" \
	"$(INTDIR)\AWPositionKey.obj" \
	"$(INTDIR)\AWQuat.obj" \
	"$(INTDIR)\AWRenderPass.obj" \
	"$(INTDIR)\AWRenderState.obj" \
	"$(INTDIR)\AWRocket.obj" \
	"$(INTDIR)\AWRotationKey.obj" \
	"$(INTDIR)\AWScene.obj" \
	"$(INTDIR)\AWSea.obj" \
	"$(INTDIR)\AWStdMaterial.obj" \
	"$(INTDIR)\AWStore.obj" \
	"$(INTDIR)\AWTexture.obj" \
	"$(INTDIR)\AWTextureEffect.obj" \
	"$(INTDIR)\AWTextureUnit.obj" \
	"$(INTDIR)\AWTimerBarCtl.obj" \
	"$(INTDIR)\AWTransformController.obj" \
	"$(INTDIR)\AWTVertexList.obj" \
	"$(INTDIR)\AWVertexProgram.obj" \
	"$(INTDIR)\AWWorker.obj"

"$(OUTDIR)\AW3DToolsRD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AW3DTools.dep")
!INCLUDE "AW3DTools.dep"
!ELSE 
!MESSAGE Warning: cannot find "AW3DTools.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AW3DTools - Win32 Release" || "$(CFG)" == "AW3DTools - Win32 Debug" || "$(CFG)" == "AW3DTools - Win32 DebugInRelease"
SOURCE=.\AWAnimatedFloat.cpp

"$(INTDIR)\AWAnimatedFloat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWAnimatedPoint.cpp

"$(INTDIR)\AWAnimatedPoint.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWAnimatedQuat.cpp

"$(INTDIR)\AWAnimatedQuat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWAnimatedTexture.cpp

"$(INTDIR)\AWAnimatedTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWBillboardController.cpp

"$(INTDIR)\AWBillboardController.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWBoidController.cpp

"$(INTDIR)\AWBoidController.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWBumpMesh.cpp

"$(INTDIR)\AWBumpMesh.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWCamera.cpp

"$(INTDIR)\AWCamera.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWCubeEnvTexture.cpp

"$(INTDIR)\AWCubeEnvTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWEffectMgr.cpp

"$(INTDIR)\AWEffectMgr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWEnvironController.cpp

"$(INTDIR)\AWEnvironController.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFace.cpp

"$(INTDIR)\AWFace.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFactory.cpp

"$(INTDIR)\AWFactory.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFile.cpp

"$(INTDIR)\AWFile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFlightController.cpp

"$(INTDIR)\AWFlightController.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFlock.cpp

"$(INTDIR)\AWFlock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFog.cpp

"$(INTDIR)\AWFog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWGLRenderer.cpp

"$(INTDIR)\AWGLRenderer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWImageProcessor.cpp

"$(INTDIR)\AWImageProcessor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWLight.cpp

"$(INTDIR)\AWLight.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWMaterial.cpp

"$(INTDIR)\AWMaterial.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWMatrix4.cpp

"$(INTDIR)\AWMatrix4.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWMesh.cpp

"$(INTDIR)\AWMesh.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWNode.cpp

"$(INTDIR)\AWNode.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWNormalMapTexture.cpp

"$(INTDIR)\AWNormalMapTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWObject.cpp

"$(INTDIR)\AWObject.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWPerfCnt.cpp

"$(INTDIR)\AWPerfCnt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWPersistable.cpp

"$(INTDIR)\AWPersistable.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWPoint.cpp

"$(INTDIR)\AWPoint.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWPositionKey.cpp

"$(INTDIR)\AWPositionKey.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWQuat.cpp

"$(INTDIR)\AWQuat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWRenderPass.cpp

"$(INTDIR)\AWRenderPass.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWRenderState.cpp

"$(INTDIR)\AWRenderState.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWRocket.cpp

"$(INTDIR)\AWRocket.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWRotationKey.cpp

"$(INTDIR)\AWRotationKey.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWScene.cpp

"$(INTDIR)\AWScene.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWSea.cpp

"$(INTDIR)\AWSea.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWStdMaterial.cpp

"$(INTDIR)\AWStdMaterial.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWStore.cpp

"$(INTDIR)\AWStore.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTexture.cpp

"$(INTDIR)\AWTexture.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTextureEffect.cpp

"$(INTDIR)\AWTextureEffect.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTextureUnit.cpp

"$(INTDIR)\AWTextureUnit.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTimerBarCtl.cpp

"$(INTDIR)\AWTimerBarCtl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTransformController.cpp

"$(INTDIR)\AWTransformController.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTVertexList.cpp

"$(INTDIR)\AWTVertexList.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWVertexProgram.cpp

"$(INTDIR)\AWVertexProgram.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWWorker.cpp

"$(INTDIR)\AWWorker.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

