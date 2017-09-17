/**********************************************************************
 *<
	FILE: flockhelp.cpp

	DESCRIPTION:  AWFlock helper

	CREATED BY: Tom Gilbert based on example by Don Brittain

	HISTORY: created October 8th 1998

 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/
#include "AlteredExport.h"
#include "AWFlockHelp.h"

//------------------------------------------------------
#define FLOCKHELPER_NUMBOIDS_CHUNK			898
#define FLOCKHELPER_COPYOBJECT_CHUNK		899
#define FLOCKHELPER_INFLUENCERADIUS_CHUNK	900
#define FLOCKHELPER_COLLWEIGHT_CHUNK		901
#define FLOCKHELPER_SPEED_CHUNK				902
#define FLOCKHELPER_MAXY_CHUNK				903
#define FLOCKHELPER_MINY_CHUNK				904
#define FLOCKHELPER_DISABLE_CHUNK			905
#define FLOCKHELPER_LEVELTIME_CHUNK         906



static AWBoolean creating = FALSE;

class AWFlockHelpClassDesc : public ClassDesc 
{
public:
	int 			IsPublic() { return 1; }
	void *			Create(AWBoolean loading = FALSE) { return new AWFlockHelpObject; }
	const TCHAR *	ClassName() { return GetString(IDS_AWFLOCK_HELPER); }
	SClass_ID		SuperClassID() { return HELPER_CLASS_ID; }
	Class_ID 		ClassID() { return AWFLOCKHELP_CLASS_ID; }
	const TCHAR* 	Category() { return _T("AlteredWorlds");  }
	int 			BeginCreate(Interface *i)	{ 
						creating = TRUE;
						return ClassDesc::BeginCreate(i); }
	int 			EndCreate(Interface *i)	{ 
						creating = FALSE; 
						return ClassDesc::EndCreate(i); }
	void			ResetClassParams(AWBoolean fileReset) { if(fileReset) resetProtParams(); }
};

static AWFlockHelpClassDesc flockHelpClassDesc;

ClassDesc* GetAWHelpDesc() { return &flockHelpClassDesc; }

// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

// class variable for measuring tape class.
Mesh AWFlockHelpObject::mesh;
short AWFlockHelpObject::meshBuilt=0;
HWND AWFlockHelpObject::hProtHelpParams = NULL;
IObjParam *AWFlockHelpObject::iObjParams;

void resetProtParams() 
{
}

// Handles the work of actually picking the target.
class PickFlockObject : public PickModeCallback, PickNodeCallback 
{
public:
	AWFlockHelpObject *ph;
	int doingPick;
	CommandMode *cm;
	HWND hDlg;
	PickFlockObject () {}

	AWBoolean HitTest (IObjParam *ip, HWND hWnd, ViewExp *vpt, IPoint2 m, int flags);
	AWBoolean Pick (IObjParam *ip, ViewExp *vpt);
	void EnterMode(IObjParam *ip);
	void ExitMode(IObjParam *ip);

	// Allow right-clicking out of mode.
	AWBoolean RightClick (IObjParam *ip,ViewExp *vpt) { return TRUE; }
	
	// Is supposed to return a PickNodeCallback-derived class: we qualify!
	PickNodeCallback *GetFilter() {return this;}
	
	// PickNodeCallback methods:
	AWBoolean Filter(INode *node);
};

AWBoolean PickFlockObject::Filter(INode *node) 
{
	return node==NULL ? FALSE : TRUE;
}

AWBoolean PickFlockObject::HitTest (IObjParam *ip, HWND hWnd, ViewExp *vpt, IPoint2 m, int flags) 
{	
	INode *node = ip->PickNode(hWnd, m, this);
	return Filter (node);
}

AWBoolean PickFlockObject::Pick (IObjParam *ip, ViewExp *vpt) 
{
	INode *node = vpt->GetClosestHit();
	assert(node);
	int res = TRUE;

	if(node->MakeReference(FOREVER,ph) != REF_SUCCEED)
		ph->boidNode = NULL;
	else
		ph->boidNode = node;
	ph->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ph->Invalidate();
	SetDlgItemText(hDlg, IDC_AWFLOCK_OBJECT_NAME, 
			ph->boidNode ? node->GetName() : _T(""));

	if(creating) {
		ph->BeginEditParams(ip, BEGIN_EDIT_CREATE, NULL);
		ip->SetCommandMode(cm);
		ip->RedrawViews(ip->GetTime());
		return FALSE;
	}
	return TRUE;
}

void PickFlockObject::EnterMode(IObjParam *ip) 
{
	ICustButton *iBut = GetICustButton(GetDlgItem(hDlg, IDC_AWFLOCK_PICK_OBJECT));
	if (iBut) {
		iBut->SetCheck(TRUE);
		ReleaseICustButton(iBut);
	}
	doingPick = 1;
}

void PickFlockObject::ExitMode(IObjParam *ip) 
{
	ICustButton *iBut = GetICustButton(GetDlgItem(hDlg, IDC_AWFLOCK_PICK_OBJECT));
	if (iBut) {
		iBut->SetCheck(FALSE);
		ReleaseICustButton(iBut);
	}
	doingPick = 0;
}

static PickFlockObject pickCB;

AWBoolean CALLBACK ProtHelpParamDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	ICustButton *iBut;
	ISpinnerControl*	spin;
	AWFlockHelpObject *ph = (AWFlockHelpObject *)GetWindowLong( hDlg, GWL_USERDATA );	
	if ( !ph && message != WM_INITDIALOG ) return FALSE;

	switch ( message ) {
		case WM_INITDIALOG:
			ph = (AWFlockHelpObject *)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)ph );
			SetDlgFont( hDlg, ph->iObjParams->GetAppHFont() );
			if( iBut = GetICustButton(GetDlgItem(hDlg, IDC_AWFLOCK_PICK_OBJECT))) 
			{
				iBut->SetType (CBT_CHECK);
				iBut->SetHighlightColor (GREEN_WASH);
				ReleaseICustButton(iBut);
			}
			ph->UpdateFlockCopyObject(FALSE); //update control from data
            ph->UpdateStartDisabled(FALSE);
			// setup the spinner controls for the number of boids 
			spin = GetISpinner(GetDlgItem(hDlg, IDC_NUMBOIDS_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_NUMBOIDS_EDIT), EDITTYPE_INT ); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(ph->numBoids, FALSE);
			ReleaseISpinner(spin);
			// setup the spinner controls for the influence radius
			spin = GetISpinner(GetDlgItem(hDlg, IDC_INRADIUS_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_INRADIUS_EDIT), EDITTYPE_POS_FLOAT ); 
			spin->SetLimits(0, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(ph->influenceRadius, FALSE);
			ReleaseISpinner(spin);
			// setup the spinner controls for the speed 
			spin = GetISpinner(GetDlgItem(hDlg, IDC_BOIDSPEED_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_BOIDSPEED_EDIT), EDITTYPE_POS_FLOAT ); 
			spin->SetLimits(0, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(ph->speed, FALSE);
			ReleaseISpinner(spin);
			// setup the spinner controls for the collision weight
			spin = GetISpinner(GetDlgItem(hDlg, IDC_COLWEIGHT_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_COLWEIGHT_EDIT), EDITTYPE_POS_FLOAT ); 
			spin->SetLimits(0.0f, 0.99f, TRUE); 
			spin->SetScale(0.05f);
			spin->SetValue(ph->collisionFraction, FALSE);
			ReleaseISpinner(spin);
			//setup the params for the minimum Y bound
			spin = GetISpinner(GetDlgItem(hDlg, IDC_MINY_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_MINY_EDIT), EDITTYPE_FLOAT ); 
			spin->SetLimits(-100000000.0f, 100000000.0f, TRUE); 
			spin->SetScale(10.0f);
			spin->SetValue(ph->minY, FALSE);
			ReleaseISpinner(spin);
			//setup the params for the maximum Y bound
			spin = GetISpinner(GetDlgItem(hDlg, IDC_MAXY_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_MAXY_EDIT), EDITTYPE_FLOAT ); 
			spin->SetLimits(-100000000.0f, 100000000.0f, TRUE); 
			spin->SetScale(10.0f);
			spin->SetValue(ph->maxY, FALSE);
			ReleaseISpinner(spin);
            //setup the params for the level time
			spin = GetISpinner(GetDlgItem(hDlg, IDC_LEVELTIME_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_LEVELTIME_EDIT), EDITTYPE_FLOAT ); 
			spin->SetLimits(0.0f, 100000000.0f, TRUE); 
			spin->SetScale(10.0f);
			spin->SetValue(ph->levelTime, FALSE);
			ReleaseISpinner(spin);
			return FALSE;			

		case WM_DESTROY:
			return FALSE;

		case WM_MOUSEACTIVATE:
			ph->iObjParams->RealizeParamPanel();
			return FALSE;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ph->iObjParams->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;

		case WM_COMMAND:			
			switch( LOWORD(wParam) ) {
			case IDC_AWFLOCK_PICK_OBJECT:
				if(ph->created == FALSE) 
				{
					if(iBut = GetICustButton(GetDlgItem(hDlg, IDC_AWFLOCK_PICK_OBJECT))) 
					{
						iBut->SetCheck(FALSE);
						ReleaseICustButton(iBut);
					}
					break;
				}
				pickCB.ph = ph;
				pickCB.hDlg = hDlg;
				if(pickCB.doingPick) 
					ph->iObjParams->SetCommandMode(pickCB.cm);
				else
					pickCB.cm = ph->iObjParams->GetCommandMode();
				ph->iObjParams->SetPickMode (&pickCB);
				ph->UpdateUI(ph->iObjParams->GetTime());
				ph->iObjParams->RedrawViews (ph->iObjParams->GetTime());
				break;
			}
			return FALSE;

		default:
			return FALSE;
	}
}


void
AWFlockHelpObject::UpdateFlockCopyObject(AWBoolean fromUI)
{//transfers data to/from the flock copy object control
	if (fromUI)
	{	//get data from the user interface
		flockCopyObject = IsDlgButtonChecked(hProtHelpParams,IDC_FLOCKHELP_COPY);
	}
	else
	{	//update the user interface with the current data
		CheckDlgButton(hProtHelpParams,IDC_FLOCKHELP_MOVE,FALSE);
		CheckDlgButton(hProtHelpParams,IDC_FLOCKHELP_COPY,FALSE);
		if (flockCopyObject)
		{
			CheckDlgButton(hProtHelpParams,IDC_FLOCKHELP_COPY,TRUE);
		}
		else
		{
			CheckDlgButton(hProtHelpParams,IDC_FLOCKHELP_MOVE,TRUE);
		}
	}
}//void AWFlockHelpObject::UpdateFlockCopyObject(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateNumBoids(AWBoolean fromUI)
{//transfers data to/from the numboids  control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_NUMBOIDS_SPINNER)); 
		if (spin)
		{
			numBoids = spin->GetIVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_NUMBOIDS_SPINNER)); 
		if (spin)
		{
			spin->SetValue(numBoids, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateNumBoids(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateStartDisabled(AWBoolean fromUI)
{
    if (fromUI)
    {
        startDisabled = IsDlgButtonChecked(hProtHelpParams, IDC_DISABLE_FLOCK);
    }
    else
    {
        CheckDlgButton(hProtHelpParams, IDC_DISABLE_FLOCK, startDisabled ? BST_CHECKED : BST_UNCHECKED);
    }
}//void AWFlockHelpObject::UpdateStartDisabled(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateInfluenceRadius(AWBoolean fromUI)
{//transfers data to/from the influence radius control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_INRADIUS_SPINNER)); 
		if (spin)
		{
			influenceRadius = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_INRADIUS_SPINNER)); 
		if (spin)
		{
			spin->SetValue(influenceRadius, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateInfluenceRadius(AWBoolean fromUI)

/*collisionFraction;
	float	speed*/
void
AWFlockHelpObject::UpdateCollideWeight(AWBoolean fromUI)
{//transfers data to/from the collision weight control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_COLWEIGHT_SPINNER)); 
		if (spin)
		{
			collisionFraction = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_COLWEIGHT_SPINNER)); 
		if (spin)
		{
			spin->SetValue(collisionFraction, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateCollideWeight(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateLevelTime(AWBoolean fromUI)
{//transfers data to/from the collision weight control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_LEVELTIME_SPINNER)); 
		if (spin)
		{
			levelTime = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_LEVELTIME_SPINNER)); 
		if (spin)
		{
			spin->SetValue(levelTime, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateLevelTime(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateMinY(AWBoolean fromUI)
{//transfers data to/from the collision weight control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_MINY_SPINNER)); 
		if (spin)
		{
			minY = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_MINY_SPINNER)); 
		if (spin)
		{
			spin->SetValue(minY, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateMinY(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateMaxY(AWBoolean fromUI)
{//transfers data to/from the collision weight control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_MAXY_SPINNER)); 
		if (spin)
		{
			maxY = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_MAXY_SPINNER)); 
		if (spin)
		{
			spin->SetValue(maxY, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateMinY(AWBoolean fromUI)


void
AWFlockHelpObject::UpdateSpeed(AWBoolean fromUI)
{//transfers data to/from the collision weight control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_BOIDSPEED_SPINNER)); 
		if (spin)
		{
			speed = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_BOIDSPEED_SPINNER)); 
		if (spin)
		{
			spin->SetValue(speed, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFlockHelpObject::UpdateSpeed(AWBoolean fromUI)









void 
AWFlockHelpObject::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	iObjParams = ip;
	editting = TRUE;
	
	if ( !hProtHelpParams ) 
	{
		hProtHelpParams = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_AWFLOCK_HELPER),
				ProtHelpParamDialogProc, 
				GetString(IDS_AWFLOCK_PARAMETERS), 
				(LPARAM)this );	
		ip->RegisterDlgWnd(hProtHelpParams);
	} 
	else 
	{
		SetWindowLong( hProtHelpParams, GWL_USERDATA, (LONG)this );
	}
	UpdateUI(ip->GetTime());
}

		
void 
AWFlockHelpObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *prev)
{
	editting = FALSE;
	//update the data from the controls
	UpdateFlockCopyObject(TRUE);
    UpdateStartDisabled(TRUE);
	UpdateNumBoids(TRUE);
	UpdateSpeed(TRUE);
	UpdateCollideWeight(TRUE);
	UpdateInfluenceRadius(TRUE);
	UpdateMinY(TRUE);
	UpdateMaxY(TRUE);
    UpdateLevelTime(TRUE);

	if ( flags&END_EDIT_REMOVEUI ) {		
		ip->UnRegisterDlgWnd(hProtHelpParams);
		ip->DeleteRollupPage(hProtHelpParams);
		hProtHelpParams = NULL;				
	} 
	else {
		SetWindowLong( hProtHelpParams, GWL_USERDATA, 0 );
	}
	iObjParams = NULL;
}




void 
AWFlockHelpObject::BuildMesh()
{
	if(meshBuilt)
		return;
	int nverts = 7;
	int nfaces = 8;
	mesh.setNumVerts(nverts);
	mesh.setNumFaces(nfaces);

	float d = 5.0f;
	float ds = 0.866f * d;
	float h = 2.0f * 0.866f * 5.0f;

	mesh.setVert(0, Point3(0.0f, 0.0f, 0.0f ));
	mesh.setVert(1, Point3(  -d,  -ds, -h ));
	mesh.setVert(2, Point3(   d,  -ds, -h ));
	mesh.setVert(3, Point3(0.0f,   ds, -h ));
	mesh.setVert(4, Point3(  -d,  -ds,  h ));
	mesh.setVert(5, Point3(   d,  -ds,  h ));
	mesh.setVert(6, Point3(0.0f,   ds,  h ));
	mesh.faces[0].setVerts(0, 1, 2);
	mesh.faces[0].setEdgeVisFlags(1,1,1);
	mesh.faces[1].setVerts(0, 2, 3);
	mesh.faces[1].setEdgeVisFlags(1,1,1);
	mesh.faces[2].setVerts(0, 3, 1);
	mesh.faces[2].setEdgeVisFlags(1,1,1);
	mesh.faces[3].setVerts(3, 2, 1);
	mesh.faces[3].setEdgeVisFlags(1,1,1);
	mesh.faces[4].setVerts(0, 5, 4);
	mesh.faces[4].setEdgeVisFlags(1,1,1);
	mesh.faces[5].setVerts(0, 6, 5);
	mesh.faces[5].setEdgeVisFlags(1,1,1);
	mesh.faces[6].setVerts(0, 4, 6);
	mesh.faces[6].setEdgeVisFlags(1,1,1);
	mesh.faces[7].setVerts(4, 5, 6);
	mesh.faces[7].setEdgeVisFlags(1,1,1);
#if 1
	// whoops- rotate 90 about x to get it facing the right way
	Matrix3 mat;
	mat.IdentityMatrix();
	mat.RotateX(DegToRad(90.0));
	for (int i=0; i<nverts; i++)
		mesh.getVert(i) = mat*mesh.getVert(i);
#endif
//	mesh.buildNormals();
	meshBuilt = 1;
}



void 
AWFlockHelpObject::UpdateUI(TimeValue t)
{
	if ( hProtHelpParams &&	GetWindowLong(hProtHelpParams,GWL_USERDATA)==(LONG)this ) {
		TCHAR buf[256];

		if(boidNode) 
		{
			GetDlgItemText(hProtHelpParams, IDC_AWFLOCK_OBJECT_NAME, buf, sizeof(buf));
			if(_tcscmp(buf, boidNode->GetName()))
				SetDlgItemText(hProtHelpParams, IDC_AWFLOCK_OBJECT_NAME, boidNode->GetName());
		}
		else
			SetDlgItemText(hProtHelpParams, IDC_AWFLOCK_OBJECT_NAME, _T(""));

		//update the UI from the data
		UpdateFlockCopyObject(FALSE);
        UpdateStartDisabled(FALSE);
		UpdateSpeed(FALSE);
		UpdateCollideWeight(FALSE);
		UpdateInfluenceRadius(FALSE);
		UpdateNumBoids(FALSE);
		UpdateMinY(FALSE);
		UpdateMaxY(FALSE);
        UpdateLevelTime(FALSE);
	}
}//void AWFlockHelpObject::UpdateUI(TimeValue t)


AWFlockHelpObject::AWFlockHelpObject() : HelperObject() 
{
	influenceRadius = 10.0f;
	collisionFraction = 0.8f;
	speed = 0.5f;

	editting = 0;
	created = 0;
	boidNode = NULL;
	numBoids = 0;
	suspendSnap = FALSE;
	flockCopyObject=FALSE; //default is to MOVE
	ivalid.SetEmpty();
	minY = 0.0;
	maxY = 0.0;
    levelTime = 0.0f;
    startDisabled = FALSE; //default is ENABLED for all flocks
	BuildMesh();
}

AWFlockHelpObject::~AWFlockHelpObject()
{
	DeleteAllRefsFromMe();
}


int AWFlockHelpObject::NumRefs()
{
	return 1;
}

RefTargetHandle AWFlockHelpObject::GetReference(int i)
{
	return boidNode;
}

void AWFlockHelpObject::SetReference(int i, RefTargetHandle rtarg)
{
	boidNode = (INode *)rtarg;
}

class ProtHelpObjCreateCallBack: public CreateMouseCallBack 
{
	AWFlockHelpObject *ph;
public:
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	void SetObj(AWFlockHelpObject *obj) { ph = obj; }
};

int ProtHelpObjCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat ) 
{	
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
		case 0:
			ph->suspendSnap = TRUE;
			mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
			break;
		case 1:
			mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
			if (msg==MOUSE_POINT) {
				ph->suspendSnap = FALSE;
				return 0;
			}
			break;			
		}
	} else if (msg == MOUSE_ABORT) {		
		return CREATE_ABORT;
	}
	return 1;
}

static ProtHelpObjCreateCallBack protHelpCreateCB;

CreateMouseCallBack* AWFlockHelpObject::GetCreateMouseCallBack() 
{
	protHelpCreateCB.SetObj(this);
	return &protHelpCreateCB;
}

void AWFlockHelpObject::GetMat(TimeValue t, INode* inode, ViewExp* vpt, Matrix3& tm) 
{
	tm = inode->GetObjectTM(t);
	tm.NoScale();
	float scaleFactor = vpt->NonScalingObjectSize()*vpt->GetVPWorldWidth(tm.GetTrans())/(float)360.0;
	tm.Scale(Point3(scaleFactor,scaleFactor,scaleFactor));
}

AWBoolean AWFlockHelpObject::GetTargetPoint(TimeValue t, Point3 *pt)
{
	if(boidNode) {
		Matrix3 tm = boidNode->GetObjectTM(t);
		*pt = tm.GetTrans();
		return TRUE;
	}
	return FALSE;
}

void AWFlockHelpObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, AWBoolean useSel )
{
	box = mesh.getBoundingBox(tm);
}

void AWFlockHelpObject::GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) 
{
	Matrix3 m = inode->GetObjectTM(t);
	Point3 pt;
	Point3 q[4];
	float scaleFactor = vpt->GetVPWorldWidth(m.GetTrans())/360.0f;
	box = mesh.getBoundingBox();
	box.Scale(scaleFactor);

	if (GetTargetPoint(t, &pt))
		box += Inverse(m) * pt;
}

void AWFlockHelpObject::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{
	int i, nv;
	Matrix3 tm;
	Point3 pt;
	GetMat(t,inode,vpt,tm);
	nv = mesh.getNumVerts();
	box.Init();
	for (i=0; i<nv; i++) 
		box += tm*mesh.getVert(i);

	if (GetTargetPoint(t, &pt))
		box += pt;
}

int AWFlockHelpObject::DrawLines(TimeValue t, INode *inode, GraphicsWindow *gw, int drawing )
{
	Point3 pt[3];
	Matrix3 tm(TRUE);
	gw->setTransform(tm);

	tm = inode->GetObjectTM(t);
	pt[0] = tm.GetTrans();
	if(boidNode) {
		pt[1] = boidNode->GetObjectTM(t).GetTrans();
		gw->polyline(2, pt, NULL, NULL, FALSE, NULL);
	}
	if(gw->checkHitCode())
		pt[0].x = 1.0f;
	return gw->checkHitCode();
}

// From BaseObject

int AWFlockHelpObject::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) 
{
	HitRegion hitRegion;
	DWORD	savedLimits;
	int res;
	Matrix3 m;
	GraphicsWindow *gw = vpt->getGW();	
	Material *mtl = gw->getMaterial();
	MakeHitRegion(hitRegion,type,crossing,4,p);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	GetMat(t,inode,vpt,m);
	gw->setTransform(m);
	// if we get a hit on the mesh, we're done
	gw->clearHitCode();
	if (mesh.select( gw, mtl, &hitRegion, flags & HIT_ABORTONHIT )) 
		return TRUE;
	// if not, check the target line, and set the pair flag if it's hit
	gw->clearHitCode();
	res = DrawLines(t, inode, gw, 1);
	gw->setRndLimits(savedLimits);
	return res;
}

void AWFlockHelpObject::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt) 
{
	if(suspendSnap)
		return;

	Matrix3 tm = inode->GetObjectTM(t);	
	GraphicsWindow *gw = vpt->getGW();	
	gw->setTransform(tm);

	Matrix3 invPlane = Inverse(snap->plane);

	// Make sure the vertex priority is active and at least as important as the best snap so far
	if(snap->vertPriority > 0 && snap->vertPriority <= snap->priority) {
		Point2 fp = Point2((float)p->x, (float)p->y);
		Point2 screen2;
		IPoint3 pt3;

		Point3 thePoint(0,0,0);
		// If constrained to the plane, make sure this point is in it!
		if(snap->snapType == SNAP_2D || snap->flags & SNAP_IN_PLANE) {
			Point3 test = thePoint * tm * invPlane;
			if(fabs(test.z) > 0.0001)	// Is it in the plane (within reason)?
				return;
		}
		gw->wTransPoint(&thePoint,&pt3);
		screen2.x = (float)pt3.x;
		screen2.y = (float)pt3.y;

		// Are we within the snap radius?
		int len = (int)Length(screen2 - fp);
		if(len <= snap->strength) {
			// Is this priority better than the best so far?
			if(snap->vertPriority < snap->priority) {
				snap->priority = snap->vertPriority;
				snap->bestWorld = thePoint * tm;
				snap->bestScreen = screen2;
				snap->bestDist = len;
			}
			else if(len < snap->bestDist) {
				snap->priority = snap->vertPriority;
				snap->bestWorld = thePoint * tm;
				snap->bestScreen = screen2;
				snap->bestDist = len;
			}
		}
	}
}

int AWFlockHelpObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) 
{
	Matrix3 m;
	GraphicsWindow *gw = vpt->getGW();
	Material *mtl = gw->getMaterial();

	created = TRUE;
	GetMat(t,inode,vpt,m);
	gw->setTransform(m);
	DWORD rlim = gw->getRndLimits();
	gw->setRndLimits(GW_WIREFRAME|GW_BACKCULL);
	if (inode->Selected()) 
		gw->setColor( LINE_COLOR, GetSelColor());
	else if(!inode->IsFrozen())
		gw->setColor( LINE_COLOR, GetUIColor(COLOR_TAPE_OBJ));
	mesh.render( gw, mtl, NULL, COMP_ALL);
	
	// calc angle
	//lastAngle = 0.0;
	#define RadToDegDbl	(180.0 / 3.141592653589793)
	/*
	if(refNode[0] && refNode[1]) {
		Point3 origin = m.GetTrans();
		Point3 vec1 = refNode[0]->GetObjectTM(t).GetTrans() - origin;
		Point3 vec2 = refNode[1]->GetObjectTM(t).GetTrans() - origin;
		float len1 = Length(vec1);
		float len2 = Length(vec2);
		if(len1 > 0.00001f && len2 > 0.00001f) {
			double cosAng = (double)DotProd(vec1, vec2) / (double)(len1 * len2);
			if(cosAng <= 0.99999)	// beyond float accuracy!
				lastAngle = acos(cosAng) * RadToDegDbl;
		}
	}*/
#if 0
	Point3 pt(0,0,0);
	TCHAR buf[32];
	_stprintf(buf, "%g", lastAngle);
	gw->setColor(TEXT_COLOR, GetUIColor(COLOR_TAPE_OBJ));
	gw->text(&pt, buf);
#endif
	DrawLines(t, inode, gw, 1);
	UpdateUI(t);
	return(0);
}


// From GeomObject
int AWFlockHelpObject::IntersectRay(TimeValue t, Ray& r, float& at) { return(0); }

//
// Reference Managment:
//

// This is only called if the object MAKES references to other things.
RefResult AWFlockHelpObject::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
     PartID& partID, RefMessage message ) 
{
	switch (message) {
	case REFMSG_CHANGE:
		if (iObjParams)
			UpdateUI(iObjParams->GetTime());
		ivalid.SetEmpty();
		break;
	case REFMSG_TARGET_DELETED:
		if (hTarget == boidNode)
			boidNode = NULL;
		if (iObjParams)
			UpdateUI(iObjParams->GetTime());
		break;
	}
	return REF_SUCCEED;
}

ObjectState AWFlockHelpObject::Eval(TimeValue time)
{
	return ObjectState(this);
}

void AWFlockHelpObject::Invalidate()
{
	ivalid.SetEmpty();
}

Interval AWFlockHelpObject::ObjectValidity(TimeValue t) 
{
	if (ivalid.Empty())
		ivalid = Interval(t,t);
	else {
		ivalid.SetInfinite();
		if(boidNode)
			boidNode->GetNodeTM(t, &ivalid);
		UpdateUI(t);
	}
	return ivalid;	
}

RefTargetHandle AWFlockHelpObject::Clone(RemapDir& remap) 
{
	AWFlockHelpObject* newob = new AWFlockHelpObject();
	newob->boidNode = boidNode;
	newob->numBoids	= numBoids;
	newob->flockCopyObject = flockCopyObject;
    newob->startDisabled = startDisabled;
	return(newob);
}



// IO
IOResult 
AWFlockHelpObject::Save(ISave *isave) 
{
	//flockCopyObject = GetFlockCopyObject();
	//numBoids		= GetNumBoids();
	ULONG nb;
	isave->BeginChunk(FLOCKHELPER_NUMBOIDS_CHUNK);
	isave->Write(&numBoids, sizeof(numBoids), &nb);
	isave->EndChunk();
	isave->BeginChunk(FLOCKHELPER_COPYOBJECT_CHUNK);
	isave->Write(&flockCopyObject, sizeof(flockCopyObject), &nb);
	isave->EndChunk();
    isave->BeginChunk(FLOCKHELPER_DISABLE_CHUNK);
	isave->Write(&startDisabled, sizeof(startDisabled), &nb);
	isave->EndChunk();  
	isave->BeginChunk(FLOCKHELPER_INFLUENCERADIUS_CHUNK); 
	isave->Write(&influenceRadius, sizeof(influenceRadius), &nb);
	isave->EndChunk();
	isave->BeginChunk(FLOCKHELPER_COLLWEIGHT_CHUNK);
	isave->Write(&collisionFraction, sizeof(collisionFraction), &nb);
	isave->EndChunk();
	isave->BeginChunk(FLOCKHELPER_SPEED_CHUNK);
	isave->Write(&speed, sizeof(speed), &nb);
	isave->EndChunk();
	isave->BeginChunk(FLOCKHELPER_MINY_CHUNK);
	isave->Write(&minY, sizeof(minY), &nb);
	isave->EndChunk();
	isave->BeginChunk(FLOCKHELPER_MAXY_CHUNK);
	isave->Write(&maxY, sizeof(maxY), &nb);
	isave->EndChunk();
    isave->BeginChunk(FLOCKHELPER_LEVELTIME_CHUNK);
    isave->Write(&levelTime, sizeof(levelTime), &nb);
    isave->EndChunk();
	return IO_OK;
}//IOResult AWFlockHelpObject::Save(ISave *isave) 



IOResult 
AWFlockHelpObject::Load(ILoad *iload) 
{
	IOResult res = IO_OK;
    AWBoolean readMoreChunks = TRUE;
	ULONG	 nb;
	while (readMoreChunks && (IO_OK == (res=iload->OpenChunk())) ) 
    {
		switch(iload->CurChunkID())  
        {
		case FLOCKHELPER_NUMBOIDS_CHUNK:
			res=iload->Read(&numBoids, sizeof(numBoids), &nb);
			break;
		case FLOCKHELPER_COPYOBJECT_CHUNK:
			res=iload->Read(&flockCopyObject, sizeof(flockCopyObject), &nb);
			break;
		case FLOCKHELPER_INFLUENCERADIUS_CHUNK: 
			res=iload->Read(&influenceRadius, sizeof(influenceRadius), &nb);
			break;
		case FLOCKHELPER_COLLWEIGHT_CHUNK:
			res=iload->Read(&collisionFraction, sizeof(collisionFraction), &nb);
			break;
		case FLOCKHELPER_SPEED_CHUNK:
			res=iload->Read(&speed, sizeof(speed), &nb);
			break;
        case FLOCKHELPER_DISABLE_CHUNK:
            res=iload->Read(&startDisabled, sizeof(startDisabled), &nb);
            break;
		case FLOCKHELPER_MINY_CHUNK:
			res=iload->Read(&minY, sizeof(minY), &nb);
			break;
		case FLOCKHELPER_MAXY_CHUNK:
			res=iload->Read(&maxY, sizeof(maxY), &nb);
			break;
        case FLOCKHELPER_LEVELTIME_CHUNK:
            res=iload->Read(&levelTime, sizeof(levelTime), &nb);
            break;
        default:
            readMoreChunks = FALSE;
            break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
	}
	return IO_OK;
}//IOResult AWFlockHelpObject::Load(ILoad *iload)




