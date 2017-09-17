# Tom Gilbert, based on MS VC++ 6.0 Studio generated file


BMODE = RELEASE
BINDING = STATIC
THREAD = SINGLE

NULL=
AWExport=


CPP       = g++
LIBRARIAN = ar
LINKER    = g++

ENVOPTS =
ifeq ($(BMODE), DEBUG)
	DBGNAME=        g
	LIBDBGOPT=      -g
	DBGOPT=         -g
	OPTIMIZE=
else
	OPTIMIZE= -O3
endif


ifeq ($(BINDING), STATIC)
	LIBTYPE   =
	LIBEXTENSION = a
	USEPIC =
	LINKFLAGS=
else
	LIBTYPE   = d
	LIBEXTENSION = so
	USEPIC = -fPIC
	LINKFLAGS = -shared $(USEPIC)
endif


#library location
LIBDIR=         ../lib

#OpenGL include path
GLINCL=		    /usr/X11R6/include/

# derive name of output library based on selected options
LIBNAME=        libAW3DTools$(LIBTYPE)$(DBGNAME)$(THRNAME)
# Full name (including suffix) of the AW3Dgl library:
LIBFULL=        $(LIBDIR)/$(LIBNAME).$(LIBEXTENSION)


###################################################################
#
# Set any relevant defines (DEFOPTS= -Dxxx ...)

CPPFLAGS=-I$(GLINCL) $(OPTIMIZE) $(USEPIC) $(DBGOPT) -DSTRICT -DGL_GLEXT_PROTOTYPES
# -D__AWTRACE -D__AWTRACEHI -DNOMINMAX -DTVZ=PDT8PST 


OBJECTS= \
	AWAnimatedFloat.obj AWAnimatedPoint.obj AWAnimatedQuat.obj \
	AWAnimatedTexture.obj AWBillboardController.obj AWBoidController.obj \
	AWBumpMesh.obj AWCamera.obj AWCubeEnvTexture.obj AWEffectMgr.obj \
	AWEnvironController.obj AWFace.obj AWFactory.obj AWFile.obj \
	AWFlightController.obj AWFlock.obj AWFog.obj AWGLRenderer.obj  \
	AWImageProcessor.obj AWLight.obj AWMaterial.obj AWMatrix4.obj \
	AWMesh.obj AWNode.obj AWNormalMapTexture.obj AWObject.obj \
	AWPerfCnt.obj AWPersistable.obj AWPoint.obj AWPositionKey.obj \
	AWQuat.obj AWRenderPass.obj AWRocket.obj AWRotationKey.obj \
	AWScene.obj AWSea.obj AWStore.obj AWTexture.obj AWTextureEffect.obj \
	AWTimerBarCtl.obj AWTransformController.obj AWTVertexList.obj \
	AWWorker.obj 


all : $(OBJECTS)
	@echo "LINK..."	
	-mkdir $(LIBDIR)
ifeq ($(BINDING), STATIC)	
	$(LIBRARIAN) rc  $(LIBFULL) $(OBJECTS)
else
	$(LINKER) $(LINKFLAGS) $(OBJECTS) -o $(LIBFULL)
endif


clean :
	-rm *.obj


$(OBJECTS): %.obj : %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@
