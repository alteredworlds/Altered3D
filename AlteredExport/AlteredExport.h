//************************************************************************** 
//* TnTExprt.h	- Ascii File Exporter
//* 
//* By Tom Gilbert
//*
//* January 20, 1997 CCJ Initial coding
//*
//* Class definition 
//*
//* Copyright (c) 1997, All Rights Reserved. 
//***************************************************************************

#ifndef __TnTExprt__H
#define __TnTExprt__H

#include "Max.h"
#include "res/resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <AW3DTools/AWObject.h>
#include <AW3DTools/AWGLRenderer.h>
#include <AW3DTools/AWLight.h>
#include <AW3DTools/AWBumpMesh.h>
#include <AWSaverGame/AWConfigWin.h>
#include <AWSaverGame/AWWinUtils.h>

#include <vector>
#include "AWExportTok.h"
#include <AW3DTools/AWMaterial.h>


extern ClassDesc* GetTnTExprtDesc();
extern ClassDesc* GetAWHelpDesc();
extern ClassDesc* GetAWFogHelpDesc();



extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#define VERSION			300			// Version number * 100
//#define FLOAT_OUTPUT	_T("%4.4f")	// Float precision for output
#define CFGFILENAME		_T("AlteredExport.cfg")	// Configuration file
#define ALMOST_ZERO 1.0e-3f
static float fTwoPi = 2.0f * PI;
static float fRadToDeg = 360.0f / fTwoPi;

class MeshExtractInfo 
{
public:
   MeshExtractInfo();
   ~MeshExtractInfo();

   AWBoolean withFaces;
   AWBoolean withNormals;
   AWBoolean withMaterials;
   AWBoolean withTexCoords;
   AWBoolean withColorVerts;
   int       meshType;
   int       matID;
   Tab<int>	 matIDList;
   AWMesh*   refMesh;
};//class MeshExtractInfo


class AWBumpMeshTracker
{
public:
   AWBumpMeshTracker(AWBumpMesh* mesh=NULL, INode* node=NULL) :
      m_mesh(mesh), m_node(node) {;}
   AWBumpMeshTracker(const AWBumpMeshTracker& o) {*this=o;}
   AWBumpMeshTracker& operator=(const AWBumpMeshTracker& o) 
                  {m_mesh=o.m_mesh; m_node=o.m_node; return *this;}
   //equality by pointer comparison
   int operator==(const AWBumpMeshTracker& o)const {return this==&o;}
   
   AWBumpMesh* m_mesh;
   INode*      m_node;
};//class AWBumpMeshTracker



enum AWTrType{AW_STANDARD=0, AW_FLIGHT, AW_ENVIRON, AW_BILLBOARD_Y, AW_BILLBOARD_ALL, AW_TIMERBAR};

// This is the main class for the exporter.

class TnTExprt : public SceneExport {
public:
	TnTExprt();
	~TnTExprt();

	// SceneExport methods
	int    ExtCount();     // Number of extensions supported 
	const TCHAR * Ext(int n);     // Extension #n (i.e. "ASC")
	const TCHAR * LongDesc();     // Long ASCII description (i.e. "Ascii Export") 
	const TCHAR * ShortDesc();    // Short ASCII description (i.e. "Ascii")
	const TCHAR * AuthorName();    // ASCII Author name
	const TCHAR * CopyrightMessage();   // ASCII Copyright message 
	const TCHAR * OtherMessage1();   // Other message #1
	const TCHAR * OtherMessage2();   // Other message #2
   const TCHAR * getCubeFaceExt(int f);
	unsigned int Version();     // Version number * 100 (i.e. v3.01 = 301) 
	void	ShowAbout(HWND hWnd);  // Show DLL's "About..." box
   void  ShowHelp();  // Show DLL's "Help..." 
	int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file

	// Other methods

	// Node enumeration
	BOOL	nodeEnum(INode* node, AWNode* parent=NULL);
	void	PreProcess(INode* node, int& nodeCount);
	void	postProcess(Interface* ip);
   //utility to see if n is a camera or light
   void checkForNegZ(INode* n, AWBoolean& self, AWBoolean& parent);

	// High level export
	void	ExportGlobalInfo();
	AWNode*	ExportGeomObject(INode* node, AWNode* parent=NULL); 
	AWNode*	ExportLightObject(INode* node, AWNode* parent = NULL); 
	AWNode*	ExportCameraObject(INode* node, AWNode* parent = NULL); 
	void	ExportShapeObject(INode* node); 
	AWNode*	ExportHelperObject(INode* node, AWNode* parent = NULL);
   AWNode*	ExportDummyNode(INode* node=NULL);
	void	     getStdMaterial(AWMaterial* awMat, StdMat* mat);
   AWBoolean  produceExeVersion(const AWCString& filename);
   AWBoolean  updateBinaryResource(WORD resID, const AWCString& exeFile, const AWCString& filename);

	// Mid level export
	//AWMesh* ExportMesh(INode* node, TimeValue t, 
   //                   AWList<AWPoint>& norms, AWTVertexList& tvList,
   //                   BOOL willOwnNormsTVerts=TRUE, int meshType=0); 
	void	ExportAnimKeys(INode* node, AWNode* awNode,
									BOOL usesNegativeY = FALSE);
	void	ExportMaterial(INode* node); 
	//BOOL	ExportAnimMesh(AWObject* obj, INode* node, ObjectState& os, 
   //                     AWList<AWPoint>& norms, AWTVertexList& tvList, int meshType=0);
	//void	ExportAnimMeshSet(AWObject* obj, INode* node, int frameStart, int frameEnd, 
   //                        AWList<AWPoint>& norms, AWTVertexList& tvList, int meshType=0);
	BOOL	GetUserAnimRange(const TCHAR* ranges, int& start, int& nFrames, int upperBound);
	//void	ExportIKJoints(INode* node);
//	void	ExportNodeTM(INode* node);
//	void	ExportNodeHeader(INode* node, TCHAR* type);
	void	ExportCameraSettings(AWCamera& awCam, CameraState* cs, 
								 CameraObject* cam, TimeValue t);
	void	ExportLightSettings(AWLight& awLight, LightState* ls, 
								GenLight* light, TimeValue t, BOOL isAnimatedData=FALSE);
	// Low level export
	void	DumpPosSample(INode* node, AWTransformController* controller, Matrix3& tm); 
	void	DumpRotSample(INode* node, AWTransformController* controller); 
	void	DumpScaleSample(INode* node, AWTransformController* controller, Matrix3& tm);
	
	void	DumpPoint3Keys(Control* cont, AWAnimatedPoint& animPt, AWBoolean transform=FALSE);
	int 	DumpFloatKeys(Control* cont, AWAnimatedFloat& flt);

	void	DumpPosKeys(Control* cont, AWTransformController* controller, Matrix3& tm);
	void	DumpRotKeys(INode* n, Control* cont, AWTransformController* controller, Matrix3& tm, BOOL lookNegY);
	void	DumpScaleKeys(Control* cont, AWTransformController* controller, Matrix3& tm);

   void  exportObjectData(ObjectState& os, INode* n, AWObject* curObj, AWBoolean hasBumpMap, int matID=-1);
   int   getNumSubMaterials(INode* node);
   int   getMapIndexIfAlreadyLoaded(Texmap* map, const AWCString& mapName);

   AWMesh*   createMesh(INode* node, const MeshExtractInfo& e, int numVerts, float t);
   void      ExtractAnimMeshSet(AWObject* obj, INode* node, int frameStart, int numFrames, MeshExtractInfo& e);
   AWBoolean ExtractAnimMesh(AWObject* obj, INode* node, ObjectState& os, MeshExtractInfo& e);
   AWMesh*   ExtractMesh(INode* node, TimeValue t, MeshExtractInfo& e);
   int   addVertex(int vIndex, Mesh& mesh, Matrix3& tm, Matrix3& objTMAfterWSM, AWMesh& awMesh, int index=-1);
   int   addTVert(int  vIndex, Mesh& mesh, Matrix3& tm, AWMesh& awMesh);
   int   addNormal(const Point3& norm, AWMesh& awMesh, int index=-1);


   AWBoolean  IsMeshAnimated(INode* node);
   AWBoolean  IsMeshAnimated(INode* node, AWBoolean& varyingTopology, AWBoolean& varyingColorVerts, AWBoolean& varyingTexCoords);

	int	    getMaterialData(INode* n, int& meshType, AWBoolean forceMaterial);
   //AWObject* buildObjectForMatID(AWObject* srce, int matID=-1);
	Bitmap*   getMAXBitmap(Texmap* map, BOOL& hasAlpha, float& frameStart, float& frameEnd, 
					     AWCString& name, float useFrame = 0.0f);
	BOOL	initTexture(AWTexture* tex, Bitmap* maxMap, BOOL useAlpha=FALSE, 
						float alphaFac=1.0f, BOOL alphaFromRGB=FALSE, 
                  BOOL luminance=FALSE);

   AWTexture* getMapData(const AWCString& matName, Texmap* tex, 
                         float alphaFac, BOOL alphaFromRGB=FALSE,
                         BOOL useNormalMapTex=FALSE);
   void        getTextureMatrix(Matrix3& uvTransform, int matID=0);
   void        getTexMatrix(Matrix3& m, AWTextureEffect* texEffect);
   void        getTexMatrix(AWTextureEffect* texEffect, Texmap* map);
   AWCubeEnvTexture* initCubeTexture(Texmap* map, const AWCString& matName, 
                                     const AWCString& fileName,  float alphaFac, 
                                     BOOL alphaFromRGB);
   void  getBaseFileNameAndExtension(AWCString& fName, AWCString& baseExt);

	AWMaterial* getStandardMaterialData(Mtl* mtl, int& meshType, 
                                       AWBoolean forceMaterial=FALSE);
	AWTexture* dumpAnimatedTextureSample(Texmap* map, float frameStart, float frameEnd, 
									  const AWCString& materialName, AWCString& fileName, 
									  BOOL useAlpha=FALSE, float alphaFac=1.0f,
									  BOOL alphaFromRGB=FALSE, BOOL useNormalMapTex=FALSE);
	void	transformQuaternion(const Quat& srce, AWPoint& axis, float& angle);
	AWQuat	transformQuaternion(const Quat& quat);
	AWPoint transformScale(const AffineParts& parts);	
	AWPoint transformPoint(const Point3& pt);
	AWPoint transformAxis(Point3& pt);
	BOOL	isMatrixIdentity(Matrix3& tm);
   AWPoint getAWPoint(const Point3& p, AWBoolean transform=FALSE);
   void     getProperties(INode* node, AWObject* obj);

	//returns FALSE if the Matrix is the Identity
	BOOL	getTransformFromMatrix3(Matrix3& tm, AWPoint& posn, 
									AWPoint& scale, AWQuat& rotn, 
									BOOL lookNegY);

	//returns FALSE if the node Transform is the Identity
	BOOL	getNodeTransform(INode* node, const TimeValue& t, 
							 AWPoint& posn, AWPoint& scale, AWQuat& rotn);
	
	AWObject*	createAWObjectFromINode(INode* node, AWNode* parent);
	AWCamera*	initCamera(Interface* ip);
	void		initBillboardedNodes(AWCamera* cam);
	
	void		assignTransformController(INode* node, AWNode* obj);

	//pretty much self explanatory...
	void assignCameraForActiveViewport(Interface* i);

	// Misc methods
	Point3	GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	BOOL	CheckForAndExportFaceMap(Mtl* mtl, Mesh* mesh); 
	void	make_face_uv(Face *f, Point3 *tv);
	BOOL	TMNegParity(Matrix3 &m);
//	TSTR	GetIndent(int indentLevel);
	TCHAR*	FixupName(TCHAR* name);
	void	CommaScan(TCHAR* buf);
	BOOL	CheckForAnimation(INode* node, BOOL& pos, BOOL& rot, BOOL& scale);
	TriObject*	GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
	BOOL	IsKnownController(Control* cont);

	// A collection of overloaded value to string converters.
	TSTR	Format(int value);
	TSTR	Format(float value);
	TSTR	Format(Color value);
	TSTR	Format(Point3 value); 
	TSTR	Format(AngAxis value); 
	TSTR	Format(Quat value);
	TSTR	Format(ScaleValue value);

	// Configuration methods
	TSTR	GetCfgFilename();
	BOOL	ReadConfig();
	void	WriteConfig();
	void	initialise();
	
   void        SetMakeExeScene(BOOL b)    {bMakeEXEScene=b;}
   void        SetShowMakeExeScene(BOOL b)    {bShowMakeEXEScene=b;}
	// Interface to member variables
   inline BOOL GetMakeExeScene()        { return bMakeEXEScene;}
   inline BOOL GetShowMakeExeScene()    { return bShowMakeEXEScene;}

   inline BOOL GetUseLimitTextureSize()   { return bLimitTextureSize;}
   inline int  GetTextureSizeLimit()      { return nTextureSizeLimit;}
	inline int	GetTextureFrameStep()		{ return nTextureFrameStep;}
	inline BOOL	GetIncludeAnimTextures()	{ return bIncludeAnimTextures; }
	inline BOOL	GetIncludeMesh()			{ return bIncludeMesh; }
	inline BOOL	GetIncludeAnim()			{ return bIncludeAnim; }
	inline BOOL	GetIncludeMtl()				{ return bIncludeMtl; }
	inline BOOL	GetIncludeMeshAnim()		{ return bIncludeMeshAnim; }
	inline BOOL	GetIncludeCamLightAnim()	{ return bIncludeCamLightAnim; }
	inline BOOL	GetIncludeIKJoints()		{ return bIncludeIKJoints; }
	inline BOOL	GetIncludeNormals()			{ return bIncludeNormals; }
	inline BOOL	GetIncludeTextureCoords()	{ return bIncludeTextureCoords; }
	inline BOOL	GetIncludeVertexColors()	{ return bIncludeVertexColors; }
	inline BOOL	GetIncludeObjGeom()			{ return bIncludeObjGeom; }
	inline BOOL	GetIncludeObjShape()		{ return bIncludeObjShape; }
	inline BOOL	GetIncludeObjCamera()		{ return bIncludeObjCamera; }
	inline BOOL	GetIncludeObjLight()		{ return bIncludeObjLight; }
	inline BOOL	GetIncludeObjHelper()		{ return bIncludeObjHelper; }
	inline BOOL	GetAlwaysSample()			{ return bAlwaysSample; }
	inline int	GetMeshFrameStep()			{ return nMeshFrameStep; }
	inline int	GetKeyFrameStep()			{ return nKeyFrameStep; }
	inline int	GetPrecision()				{ return nPrecision; }
	inline TimeValue GetStaticFrame()		{ return nStaticFrame; }
	inline Interface*	GetInterface()		{ return ip; }
	inline INode*	GetCamera()				{ return m_selCamera;}
	inline const TSTR& GetChosenCameraName(){ return chosenCamera;}
	inline BOOL GetLoopScene()				{ return m_loopScene;}

	inline int	GetExportCentered()			{ return exportCentered;}				// export centered? 0 => NO
	inline BOOL	GetShouldScale()			{ return shouldScale; }
	inline double GetScaleRange()			{ return scaleRange; }
	inline BOOL	GetExportTextures()			{ return exportTextures; }
	inline BOOL	GetExportInScene()			{ return exportInScene; }

   inline void SetUseLimitTextureSize(BOOL v)   { bLimitTextureSize=v;}
   inline void SetTextureSizeLimit(int v)       { nTextureSizeLimit=v;}
	inline void SetLoopScene(BOOL v)				   {m_loopScene=v;}
	inline void SetChosenCameraName(const TSTR& n)  {chosenCamera=n;}
	inline void	SetTextureFrameStep(BOOL val)		{ nTextureFrameStep = val;}
	inline void	SetIncludeAnimTextures(BOOL val)	{ bIncludeAnimTextures = val; }
	inline void SetCamera(INode* newVal)			{ m_selCamera = newVal; if (newVal) chosenCamera = newVal->GetName();}
	inline void	SetIncludeMesh(BOOL val)			{ bIncludeMesh = val; }
	inline void	SetIncludeAnim(BOOL val)			{ bIncludeAnim = val; }
	inline void	SetIncludeMtl(BOOL val)				{ bIncludeMtl = val; }
	inline void	SetIncludeMeshAnim(BOOL val)		{ bIncludeMeshAnim = val; }
	inline void	SetIncludeCamLightAnim(BOOL val)	{ bIncludeCamLightAnim = val; }
	inline void	SetIncludeIKJoints(BOOL val)		{ bIncludeIKJoints = val; }
	inline void	SetIncludeNormals(BOOL val)			{ bIncludeNormals = val; }
	inline void	SetIncludeTextureCoords(BOOL val)	{ bIncludeTextureCoords = val; }
	inline void	SetIncludeVertexColors(BOOL val)	{ bIncludeVertexColors = val; }
	inline void	SetIncludeObjGeom(BOOL val)			{ bIncludeObjGeom = val; }
	inline void	SetIncludeObjShape(BOOL val)		{ bIncludeObjShape = val; }
	inline void	SetIncludeObjCamera(BOOL val)		{ bIncludeObjCamera = val; }
	inline void	SetIncludeObjLight(BOOL val)		{ bIncludeObjLight = val; }
	inline void	SetIncludeObjHelper(BOOL val)		{ bIncludeObjHelper = val; }
	inline void	SetAlwaysSample(BOOL val)			{ bAlwaysSample = val; }
	inline void SetMeshFrameStep(int val)			{ nMeshFrameStep = val; }
	inline void SetKeyFrameStep(int val)			{ nKeyFrameStep = val; }
	inline void SetPrecision(int val)				{ nPrecision = val; }
	inline void SetStaticFrame(TimeValue val)		{ nStaticFrame = val; }


	inline void	SetExportCentered(int newVal)		{ exportCentered = newVal; }				// export centered? 0 => NO
	inline void	SetShouldScale(BOOL	 newVal)		{ shouldScale = newVal; }
	inline void	SetScaleRange(double newVal)		{ scaleRange = newVal; }
	inline void	SetExportTextures(BOOL newVal)		{ exportTextures = newVal; }
	inline void	SetExportInScene(BOOL newVal)		{ exportInScene = newVal; }

	void        applyMaterial(AWObject* obj, int matID);

   Tab<INode*>	            m_cameras;	//full list of Cameras

protected:
   static TCHAR*           suffixes[6];
   Tab<AWBumpMeshTracker*> m_trackBumps;
   //holds materials for a single object
   Tab<AWMaterial*>        m_mat;
   
	Tab<TSTR*>	            m_fogNames; //list of names of all the fog helpers
							//used to match name with index posn.

	//int			            getFogIndex(const AWCString& name);

   void                    clearFogNames();
   void                    clearTrackBumps();
   //MAX Cameras are not owned by this exporter (rather the MAX scene graph)
   void                    clearCameras();
   //AWMaterials are not owned by the exporter - clear doesn't delete them
   void                    clearMaterials();

   int                     getNumMaterials()  {return (int)m_mat.Count();}
   AWMaterial*             getMaterial(int i);
   
   int                     addMaterial(AWMaterial* m);

	AWScene	theScene;
   AWConfigWin    m_cfg;

 
private:
	INode*		m_selCamera;
	//AWObject*	m_curObject;			//constructor must set to null
	int		exportCentered;				// export centered? 0 => NO
	BOOL	shouldScale;
	double	scaleRange;
	BOOL	exportTextures;
	BOOL	exportInScene;

	BOOL    m_loopScene;
	BOOL	bIncludeMesh;
	BOOL	bIncludeAnim;
	BOOL	bIncludeMtl;
	BOOL	bIncludeMeshAnim;
	BOOL	bIncludeCamLightAnim;
	BOOL	bIncludeIKJoints;
	BOOL	bIncludeNormals;
	BOOL	bIncludeTextureCoords;
	BOOL	bIncludeObjGeom;
	BOOL	bIncludeObjShape;
	BOOL	bIncludeObjCamera;
	BOOL	bIncludeObjLight;
	BOOL	bIncludeObjHelper;
	BOOL	bAlwaysSample;
	BOOL	bIncludeVertexColors;
	BOOL	bIncludeAnimTextures;
   BOOL	bLimitTextureSize;
   BOOL     bMakeEXEScene;
   BOOL     bShowMakeEXEScene;


   int      nTextureSizeLimit;
	int		nTextureFrameStep;
	int		nMeshFrameStep;
	int		nKeyFrameStep;
	int		nPrecision;
	TSTR	chosenCamera;
 	TimeValue	nStaticFrame;

	Interface*	ip;
	FILE*		pStream;
	int			nTotalNodeCount;
	int			nCurNode;
	TCHAR		szFmtStr[16];
};

#endif // __TnTExprt__H

