/**********************************************************************
 *<
	FILE: flockhelp.h

	DESCRIPTION:  AWFlock helper

	CREATED BY: Tom Gilbert based on example by Don Brittain

	HISTORY: created October 8th 1998

 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/

#ifndef __AW_FLOCK_HELP_H 
#define __AW_FLOCK_HELP_H

#define AWFLOCKHELP_CLASS_ID	Class_ID(0x29a64116, 0x5e275504)

class AWFlockHelpObject: public HelperObject 
{
	friend class ProtHelpObjCreateCallBack;
	friend AWBoolean CALLBACK ProtHelpParamDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	friend void resetProtParams();
public:

	// Class vars
	static Mesh mesh;
	static short meshBuilt;
	static HWND hProtHelpParams;
	static IObjParam *iObjParams;
	AWBoolean created;

	// Snap suspension flag (TRUE during creation only)
	AWBoolean suspendSnap;

	// Object parameters		
	short editting;
	INode* boidNode;
	//transfer data from/to controls
	void	UpdateFlockCopyObject(AWBoolean fromUI);
	void	UpdateSpeed(AWBoolean fromUI);
	void	UpdateCollideWeight(AWBoolean fromUI);
	void	UpdateInfluenceRadius(AWBoolean fromUI);
	void	UpdateNumBoids(AWBoolean fromUI);
	void	UpdateMinY(AWBoolean fromUI);
	void	UpdateMaxY(AWBoolean fromUI);
    void    UpdateStartDisabled(AWBoolean fromUI);
    void    UpdateLevelTime(AWBoolean fromUI);

	//persisted data
	int		numBoids;
	AWBoolean	flockCopyObject;
	float	influenceRadius;
	float	collisionFraction;
	float	speed;
	float	minY;
	float	maxY;
    AWBoolean    startDisabled;
    float   levelTime;

	Interval ivalid;

	//  inherited virtual methods for Reference-management
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
	   PartID& partID, RefMessage message );
	void BuildMesh();
	void UpdateUI(TimeValue t);
	void GetMat(TimeValue t, INode* inod, ViewExp *vpt, Matrix3& mat);
	AWBoolean GetTargetPoint(TimeValue t, Point3* q);
	int DrawLines(TimeValue t, INode *inode, GraphicsWindow *gw, int drawing );

	AWFlockHelpObject();
	~AWFlockHelpObject();

	//  inherited virtual methods:

	// From BaseObject
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
	TCHAR *GetObjectName() { return GetString(IDS_AWFLOCK_HELPER); }

	// From Object
	ObjectState Eval(TimeValue time);
	void InitNodeName(TSTR& s) { s = GetString(IDS_AWFLOCK_HELPER); }
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
	Class_ID ClassID() { return AWFLOCKHELP_CLASS_ID; }  
	void GetClassName(TSTR& s) { s = TSTR(GetString(IDS_AWFLOCK_HELPER)); }

	TSTR SubAnimName(int i) { return TSTR(GetString(IDS_AWFLOCK_PARAMETERS));}

	// From ref
	RefTargetHandle Clone(RemapDir& remap = NoRemap());
	int NumRefs();
	RefTargetHandle GetReference(int i);
	void SetReference(int i, RefTargetHandle rtarg);

	// IO
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
};				


#endif
