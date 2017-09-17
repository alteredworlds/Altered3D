# Microsoft Developer Studio Generated NMAKE File, Based on AW3DToolsS.dsp
!IF "$(CFG)" == ""
CFG=AW3DToolsS - Win32 Static Debug
!MESSAGE No configuration specified. Defaulting to AW3DToolsS - Win32 Static Debug.
!ENDIF 

!IF "$(CFG)" != "AW3DToolsS - Win32 Static DebugInRelease" && "$(CFG)" != "AW3DToolsS - Win32 Static Release" && "$(CFG)" != "AW3DToolsS - Win32 Static Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "AW3DToolsS - Win32 Static DebugInRelease"

OUTDIR=.\../SReleaseD
INTDIR=.\../SReleaseD

ALL : "..\..\lib\AW3DToolsRDS.lib"


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
	-@erase "..\..\lib\AW3DToolsRDS.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MT /W3 /GX /Z7 /Od /D "NDEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /D "__AWTRACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AW3DToolsS.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\AW3DToolsRDS.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\AWWorker.obj" \
	"$(INTDIR)\AWVertexProgram.obj"

"..\..\lib\AW3DToolsRDS.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AW3DToolsS - Win32 Static Release"

OUTDIR=.\../SRelease
INTDIR=.\../SRelease

ALL : "..\..\lib\AW3DToolsS.lib"


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
	-@erase "..\..\lib\AW3DToolsS.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AW3DToolsS.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\AW3DToolsS.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\AWWorker.obj" \
	"$(INTDIR)\AWVertexProgram.obj"

"..\..\lib\AW3DToolsS.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AW3DToolsS - Win32 Static Debug"

OUTDIR=.\../SDebug
INTDIR=.\../SDebug

ALL : "..\..\lib\AW3DToolsdS.lib"


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
	-@erase "..\..\lib\AW3DToolsdS.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /D "STRICT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AW3DToolsS.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\AW3DToolsdS.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\AWWorker.obj" \
	"$(INTDIR)\AWVertexProgram.obj"

"..\..\lib\AW3DToolsdS.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AW3DToolsS.dep")
!INCLUDE "AW3DToolsS.dep"
!ELSE 
!MESSAGE Warning: cannot find "AW3DToolsS.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AW3DToolsS - Win32 Static DebugInRelease" || "$(CFG)" == "AW3DToolsS - Win32 Static Release" || "$(CFG)" == "AW3DToolsS - Win32 Static Debug"
SOURCE=..\AWAnimatedFloat.cpp

"$(INTDIR)\AWAnimatedFloat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWAnimatedPoint.cpp

"$(INTDIR)\AWAnimatedPoint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWAnimatedQuat.cpp

"$(INTDIR)\AWAnimatedQuat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWAnimatedTexture.cpp

"$(INTDIR)\AWAnimatedTexture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWBillboardController.cpp

"$(INTDIR)\AWBillboardController.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWBoidController.cpp

"$(INTDIR)\AWBoidController.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWBumpMesh.cpp

"$(INTDIR)\AWBumpMesh.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWCamera.cpp

"$(INTDIR)\AWCamera.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWCubeEnvTexture.cpp

"$(INTDIR)\AWCubeEnvTexture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWEffectMgr.cpp

"$(INTDIR)\AWEffectMgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWEnvironController.cpp

"$(INTDIR)\AWEnvironController.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWFace.cpp

"$(INTDIR)\AWFace.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWFactory.cpp

"$(INTDIR)\AWFactory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWFile.cpp

"$(INTDIR)\AWFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWFlightController.cpp

"$(INTDIR)\AWFlightController.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWFlock.cpp

"$(INTDIR)\AWFlock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWFog.cpp

"$(INTDIR)\AWFog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWGLRenderer.cpp

"$(INTDIR)\AWGLRenderer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWImageProcessor.cpp

"$(INTDIR)\AWImageProcessor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWLight.cpp

"$(INTDIR)\AWLight.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWMaterial.cpp

"$(INTDIR)\AWMaterial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWMatrix4.cpp

"$(INTDIR)\AWMatrix4.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWMesh.cpp

"$(INTDIR)\AWMesh.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWNode.cpp

"$(INTDIR)\AWNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWNormalMapTexture.cpp

"$(INTDIR)\AWNormalMapTexture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWObject.cpp

"$(INTDIR)\AWObject.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWPerfCnt.cpp

"$(INTDIR)\AWPerfCnt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWPersistable.cpp

"$(INTDIR)\AWPersistable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWPoint.cpp

"$(INTDIR)\AWPoint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWPositionKey.cpp

"$(INTDIR)\AWPositionKey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWQuat.cpp

"$(INTDIR)\AWQuat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWRenderPass.cpp

"$(INTDIR)\AWRenderPass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWRenderState.cpp

"$(INTDIR)\AWRenderState.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWRocket.cpp

"$(INTDIR)\AWRocket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWRotationKey.cpp

"$(INTDIR)\AWRotationKey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWScene.cpp

"$(INTDIR)\AWScene.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWSea.cpp

"$(INTDIR)\AWSea.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWStdMaterial.cpp

"$(INTDIR)\AWStdMaterial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWStore.cpp

"$(INTDIR)\AWStore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWTexture.cpp

"$(INTDIR)\AWTexture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWTextureEffect.cpp

"$(INTDIR)\AWTextureEffect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWTextureUnit.cpp

"$(INTDIR)\AWTextureUnit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWTimerBarCtl.cpp

"$(INTDIR)\AWTimerBarCtl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWTransformController.cpp

"$(INTDIR)\AWTransformController.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWTVertexList.cpp

"$(INTDIR)\AWTVertexList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWVertexProgram.cpp

"$(INTDIR)\AWVertexProgram.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWWorker.cpp

"$(INTDIR)\AWWorker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

