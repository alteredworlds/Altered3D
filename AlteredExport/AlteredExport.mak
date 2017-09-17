# Microsoft Developer Studio Generated NMAKE File, Based on AlteredExport.dsp
!IF "$(CFG)" == ""
CFG=ALTEREDEXPORT - WIN32 RELEASE
!MESSAGE No configuration specified. Defaulting to ALTEREDEXPORT - WIN32 RELEASE.
!ENDIF 

!IF "$(CFG)" != "AlteredExport - Win32 Release" && "$(CFG)" != "AlteredExport - Win32 DebugInRelease"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AlteredExport.mak" CFG="ALTEREDEXPORT - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AlteredExport - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AlteredExport - Win32 DebugInRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "AlteredExport - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\AlteredExport.dle"


CLEAN :
	-@erase "$(INTDIR)\AlteredExport.obj"
	-@erase "$(INTDIR)\AlteredExport.res"
	-@erase "$(INTDIR)\AWAnimOut.obj"
	-@erase "$(INTDIR)\AWConfig.obj"
	-@erase "$(INTDIR)\AWConfigWin.obj"
	-@erase "$(INTDIR)\AWCreateExecutable.obj"
	-@erase "$(INTDIR)\AWExportMain.obj"
	-@erase "$(INTDIR)\AWFlockHelp.obj"
	-@erase "$(INTDIR)\AWFogHelp.obj"
	-@erase "$(INTDIR)\AWTextures.obj"
	-@erase "$(INTDIR)\AWTransforms.obj"
	-@erase "$(INTDIR)\AWVerify.obj"
	-@erase "$(INTDIR)\AWWinUtils.obj"
	-@erase "$(INTDIR)\Md5.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AlteredExport.dle"
	-@erase "$(OUTDIR)\AlteredExport.exp"
	-@erase "$(OUTDIR)\AlteredExport.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AlteredExport.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AlteredExport.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=AW3DTools.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AlteredExport.pdb" /machine:I386 /def:".\AlteredExport.def" /out:"$(OUTDIR)\AlteredExport.dle" /implib:"$(OUTDIR)\AlteredExport.lib" 
DEF_FILE= \
	".\AlteredExport.def"
LINK32_OBJS= \
	"$(INTDIR)\AlteredExport.obj" \
	"$(INTDIR)\AWAnimOut.obj" \
	"$(INTDIR)\AWConfig.obj" \
	"$(INTDIR)\AWConfigWin.obj" \
	"$(INTDIR)\AWCreateExecutable.obj" \
	"$(INTDIR)\AWExportMain.obj" \
	"$(INTDIR)\AWFlockHelp.obj" \
	"$(INTDIR)\AWFogHelp.obj" \
	"$(INTDIR)\AWTextures.obj" \
	"$(INTDIR)\AWTransforms.obj" \
	"$(INTDIR)\AWVerify.obj" \
	"$(INTDIR)\AWWinUtils.obj" \
	"$(INTDIR)\Md5.obj" \
	"$(INTDIR)\AlteredExport.res" \
	"..\..\MAXSDK\lib\maxutil.lib" \
	"..\..\MAXSDK\lib\core.lib" \
	"..\..\MAXSDK\lib\bmm.lib" \
	"..\..\MAXSDK\lib\mesh.lib" \
	"..\..\MAXSDK\lib\geom.lib" \
	"..\..\MAXSDK\lib\gfx.lib"

"$(OUTDIR)\AlteredExport.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AlteredExport - Win32 DebugInRelease"

OUTDIR=.\ReleaseD
INTDIR=.\ReleaseD
# Begin Custom Macros
OutDir=.\ReleaseD
# End Custom Macros

ALL : "$(OUTDIR)\AlteredExportD.dle"


CLEAN :
	-@erase "$(INTDIR)\AlteredExport.obj"
	-@erase "$(INTDIR)\AlteredExport.res"
	-@erase "$(INTDIR)\AWAnimOut.obj"
	-@erase "$(INTDIR)\AWConfig.obj"
	-@erase "$(INTDIR)\AWConfigWin.obj"
	-@erase "$(INTDIR)\AWCreateExecutable.obj"
	-@erase "$(INTDIR)\AWExportMain.obj"
	-@erase "$(INTDIR)\AWFlockHelp.obj"
	-@erase "$(INTDIR)\AWFogHelp.obj"
	-@erase "$(INTDIR)\AWTextures.obj"
	-@erase "$(INTDIR)\AWTransforms.obj"
	-@erase "$(INTDIR)\AWVerify.obj"
	-@erase "$(INTDIR)\AWWinUtils.obj"
	-@erase "$(INTDIR)\Md5.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AlteredExportD.dle"
	-@erase "$(OUTDIR)\AlteredExportD.exp"
	-@erase "$(OUTDIR)\AlteredExportD.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Z7 /Od /D "WIN32" /D "NDEBUG" /D "__AWTRACE" /D "_WINDOWS" /D "AW3D_USEDLL" /D "_USRDLL" /D "STRICT" /D "_AWDISABLE_USER_VALIDATION" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AlteredExport.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AlteredExport.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=AW3DToolsRD.lib core.lib bmm.lib geom.lib gfx.lib mesh.lib util.lib lz32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB winmm.lib /nologo /stack:0x2000000 /dll /pdb:none /debug /machine:I386 /def:".\AlteredExport.def" /out:"$(OUTDIR)\AlteredExportD.dle" /implib:"$(OUTDIR)\AlteredExportD.lib" 
DEF_FILE= \
	".\AlteredExport.def"
LINK32_OBJS= \
	"$(INTDIR)\AlteredExport.obj" \
	"$(INTDIR)\AWAnimOut.obj" \
	"$(INTDIR)\AWConfig.obj" \
	"$(INTDIR)\AWConfigWin.obj" \
	"$(INTDIR)\AWCreateExecutable.obj" \
	"$(INTDIR)\AWExportMain.obj" \
	"$(INTDIR)\AWFlockHelp.obj" \
	"$(INTDIR)\AWFogHelp.obj" \
	"$(INTDIR)\AWTextures.obj" \
	"$(INTDIR)\AWTransforms.obj" \
	"$(INTDIR)\AWVerify.obj" \
	"$(INTDIR)\AWWinUtils.obj" \
	"$(INTDIR)\Md5.obj" \
	"$(INTDIR)\AlteredExport.res" \
	"..\..\MAXSDK\lib\maxutil.lib" \
	"..\..\MAXSDK\lib\core.lib" \
	"..\..\MAXSDK\lib\bmm.lib" \
	"..\..\MAXSDK\lib\mesh.lib" \
	"..\..\MAXSDK\lib\geom.lib" \
	"..\..\MAXSDK\lib\gfx.lib"

"$(OUTDIR)\AlteredExportD.dle" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\ReleaseD
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AlteredExportD.dle"
   copy ReleaseD\AlteredExportd.dle c:\3dsmax3\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AlteredExport.dep")
!INCLUDE "AlteredExport.dep"
!ELSE 
!MESSAGE Warning: cannot find "AlteredExport.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AlteredExport - Win32 Release" || "$(CFG)" == "AlteredExport - Win32 DebugInRelease"
SOURCE=.\AlteredExport.cpp

"$(INTDIR)\AlteredExport.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWAnimOut.cpp

"$(INTDIR)\AWAnimOut.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\awsavergame\AWConfig.cpp

"$(INTDIR)\AWConfig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWSaverGame\AWConfigWin.cpp

"$(INTDIR)\AWConfigWin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\AWCreateExecutable.cpp

"$(INTDIR)\AWCreateExecutable.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWExportMain.cpp

"$(INTDIR)\AWExportMain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFlockHelp.cpp

"$(INTDIR)\AWFlockHelp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWFogHelp.cpp

"$(INTDIR)\AWFogHelp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTextures.cpp

"$(INTDIR)\AWTextures.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AWTransforms.cpp

"$(INTDIR)\AWTransforms.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\AWSerialize\AWVerify.cpp

"$(INTDIR)\AWVerify.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWSaverGame\AWWinUtils.cpp

"$(INTDIR)\AWWinUtils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\AWSerialize\Md5.cpp

"$(INTDIR)\Md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\res\AlteredExport.rc

!IF  "$(CFG)" == "AlteredExport - Win32 Release"


"$(INTDIR)\AlteredExport.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\AlteredExport.res" /i "res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "AlteredExport - Win32 DebugInRelease"


"$(INTDIR)\AlteredExport.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\AlteredExport.res" /i "res" /d "NDEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

