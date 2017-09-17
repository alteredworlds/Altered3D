//	FILE: awfoghelp.h
//	DESCRIPTION:  AWFog helper
//	CREATED BY: Tom Gilbert based on example by Don Brittain
//	HISTORY: created Jan 8th 1999
//	Copyright (c) 1998, All Rights Reserved.

#ifndef __AW_FOG_HELP_H 
#define __AW_FOG_HELP_H

#define AWFOGHELP_CLASS_ID	Class_ID(0xb177bb3, 0x515812ed)

class AWFogHelpObject: public HelperObject 
{
	friend class AWFogHelpObjCreateCallBack;
	friend AWBoolean CALLBACK AWFogHelpObjectDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
public:

	// Class vars
	static Mesh			mesh;
	static short		meshBuilt;
	static HWND			hProtHelpParams;
	static IObjParam*	iObjParams;
	AWBoolean				created;

	// Snap suspension flag (TRUE during creation only)
	AWBoolean				suspendSnap;

	// Object parameters		
	short				editting;

	//transfer data from/to controls
	void	UpdateFogType(AWBoolean fromUI);
	void	UpdateFogColour(AWBoolean fromUI);
	void	UpdateFogStart(AWBoolean fromUI);
	void	UpdateFogEnd(AWBoolean fromUI);

	//persisted data
	int			fogType;
	COLORREF	fogColour;
	float		fogStart;
	float		fogEnd;

	Interval ivalid;

	//  inherited virtual methods for Reference-management
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
	   PartID& partID, RefMessage message );
	void BuildMesh();
	void UpdateUI(TimeValue t);
	void GetMat(TimeValue t, INode* inod, ViewExp *vpt, Matrix3& mat);
	AWBoolean GetTargetPoint(TimeValue t, Point3* q);
	int DrawLines(TimeValue t, INode *inode, GraphicsWindow *gw, int drawing );

	AWFogHelpObject();
	~AWFogHelpObject();

	//  inherited virtual methods:

	// From BaseObject
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
	TCHAR *GetObjectName() { return GetString(IDS_AWFOG_HELPER); }

	// From Object
	ObjectState Eval(TimeValue time);
	void InitNodeName(TSTR& s) { s = GetString(IDS_AWFOG_HELPER); }
	Interval ObjectValidity(TimeValue time);
	void Invalidate();
	int DoOwnSelectHilite() { return 1; }

	// From GeomObject
	int IntersectRay(TimeValue t, Ray& r, float& at);
	void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, AWBoolean useSel );

	// Animatable methods
	void DeleteThis() { delete this; }
	Class_ID ClassID() { return AWFOGHELP_CLASS_ID; }  
	void GetClassName(TSTR& s) { s = TSTR(GetString(IDS_AWFOG_HELPER)); }

	TSTR SubAnimName(int i) { return TSTR(GetString(IDS_AWFOG_PARAMETERS));}

	// From ref
	RefTargetHandle Clone(RemapDir& remap = NoRemap());
	int NumRefs();
	RefTargetHandle GetReference(int i);
	void SetReference(int i, RefTargetHandle rtarg);

	// IO
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	IColorSwatch* fogColorSwatch;
};				


#endif
