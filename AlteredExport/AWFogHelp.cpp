//FILE: AWFog.cpp
//DESCRIPTION:  AWFog helper
//CREATED BY: Tom Gilbert based on example by Don Brittain
//HISTORY: created Jan 8th 1999
//Copyright (c) 1998, All Rights Reserved.

#include "AlteredExport.h"
#include "AWFogHelp.h"

//------------------------------------------------------
#define AWFOGHELPER_FOGTYPE_CHUNK		910
#define AWFOGHELPER_COLOUR_CHUNK		911
#define AWFOGHELPER_FOGSTART_CHUNK		912
#define AWFOGHELPER_FOGEND_CHUNK		913



static AWBoolean creating = FALSE;

class AWFogHelpClassDesc : public ClassDesc 
{
public:
	int 			IsPublic() { return 1; }
	void *			Create(AWBoolean loading = FALSE) { return new AWFogHelpObject; }
	const TCHAR *	ClassName() { return GetString(IDS_AWFOG_HELPER); }
	SClass_ID		SuperClassID() { return HELPER_CLASS_ID; }
	Class_ID 		ClassID() { return AWFOGHELP_CLASS_ID; }
	const TCHAR* 	Category() { return _T("AlteredWorlds");  }
	int 			BeginCreate(Interface *i)	{ 
						creating = TRUE;
						return ClassDesc::BeginCreate(i); }
	int 			EndCreate(Interface *i)	{ 
						creating = FALSE; 
						return ClassDesc::EndCreate(i); }
	void			ResetClassParams(AWBoolean fileReset) {  }
};

static AWFogHelpClassDesc awFogHelpClassDesc;

ClassDesc* GetAWFogHelpDesc() { return &awFogHelpClassDesc; }

// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

// class variable for measuring tape class.
Mesh AWFogHelpObject::mesh;
short AWFogHelpObject::meshBuilt=0;
HWND AWFogHelpObject::hProtHelpParams = NULL;
IObjParam *AWFogHelpObject::iObjParams;



// Handles the work of actually picking the target.
class PickAWFogObject : public PickModeCallback, PickNodeCallback 
{
public:
	AWFogHelpObject *ph;
	int doingPick;
	CommandMode *cm;
	HWND hDlg;
	PickAWFogObject () {}

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

AWBoolean PickAWFogObject::Filter(INode *node) 
{
	return node==NULL ? FALSE : TRUE;
}

AWBoolean PickAWFogObject::HitTest (IObjParam *ip, HWND hWnd, ViewExp *vpt, IPoint2 m, int flags) 
{	
	INode *node = ip->PickNode(hWnd, m, this);
	return Filter (node);
}

AWBoolean PickAWFogObject::Pick (IObjParam *ip, ViewExp *vpt) 
{
	INode *node = vpt->GetClosestHit();
	assert(node);
	int res = TRUE;

	ph->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ph->Invalidate();
	
	if(creating) {
		ph->BeginEditParams(ip, BEGIN_EDIT_CREATE, NULL);
		ip->SetCommandMode(cm);
		ip->RedrawViews(ip->GetTime());
		return FALSE;
	}
	return TRUE;
}

void PickAWFogObject::EnterMode(IObjParam *ip) 
{
	doingPick = 1;
}

void PickAWFogObject::ExitMode(IObjParam *ip) 
{
	doingPick = 0;
}

static PickAWFogObject pickCB;


AWBoolean CALLBACK AWFogHelpObjectDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	ISpinnerControl*	spin;
	AWFogHelpObject *ph = (AWFogHelpObject *)GetWindowLong( hDlg, GWL_USERDATA );	
	if ( !ph && message != WM_INITDIALOG ) return FALSE;

	switch ( message ) {
		case WM_INITDIALOG:
			//get the helper object passed into this static fn.
			ph = (AWFogHelpObject*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)ph );
			SetDlgFont( hDlg, ph->iObjParams->GetAppHFont() );
			//now initialise the controls
			//fill fogType combo with the relevant strings in correct order
			SendMessage(GetDlgItem(hDlg,IDC_FOGTYPE_COMBO), CB_ADDSTRING, 
									0, (LPARAM)_T("LINEAR"));
			SendMessage(GetDlgItem(hDlg,IDC_FOGTYPE_COMBO), CB_ADDSTRING, 
									0, (LPARAM)_T("EXP"));
			SendMessage(GetDlgItem(hDlg,IDC_FOGTYPE_COMBO), CB_ADDSTRING, 
									0, (LPARAM)_T("EXP2"));
			//and now ensure the correct one is selected
			ph->UpdateFogType(FALSE); //update control selection FROM data
			//need to init & set the colour swatch custom control
			// Initialize the Color Swatch Control
			ph->fogColorSwatch = GetIColorSwatch(GetDlgItem(hDlg, IDC_FOG_COLOUR_SWATCH), 	
								 RGB(255,255,255), _T("New Fog Color"));
			{
				ph->UpdateFogColour(FALSE);
			}
			// setup the spinner controls for the fog start
			spin = GetISpinner(GetDlgItem(hDlg, IDC_FOGSTART_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_FOGSTART_EDIT), EDITTYPE_POS_FLOAT ); 
			spin->SetLimits(0, 99999999, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(ph->fogStart, FALSE);
			ReleaseISpinner(spin);
			// setup the spinner controls for the fog end 
			spin = GetISpinner(GetDlgItem(hDlg, IDC_FOGEND_SPINNER)); 
			spin->LinkToEdit(GetDlgItem(hDlg,IDC_FOGEND_EDIT), EDITTYPE_POS_FLOAT ); 
			spin->SetLimits(0, 99999999, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(ph->fogEnd, FALSE);
			ReleaseISpinner(spin);
			return FALSE;			

		case WM_DESTROY:
			if (ph->fogColorSwatch) 
			{
				ReleaseIColorSwatch(ph->fogColorSwatch);
				ph->fogColorSwatch = NULL;
			}	
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
			return FALSE;


		case CC_COLOR_CHANGE: 
		{
			// This message is sent when the user is changing the color
			// using the ColorSwatch control. lParam contains a pointer
			// to the ColorSwatch control - so get the new colour
			if (ph->fogColorSwatch)
				ph->fogColour = ph->fogColorSwatch->GetColor();
			break;
		}

		default:
			return FALSE;
	}
	return TRUE;
}//AWBoolean CALLBACK AWFogHelpObjectDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )



void
AWFogHelpObject::UpdateFogType(AWBoolean fromUI)
{//transfers data to/from the control
	//control shows a list of fog types:
	//LINEAR
	//EXPONENTIAL
	//EXP2
	//store index in helper, not string
	if (fromUI)
	{	//get data from the user interface
		fogType = SendMessage(GetDlgItem(hProtHelpParams,IDC_FOGTYPE_COMBO), CB_GETCURSEL, 0, 0);
	}
	else
	{	//update the user interface with the current data
		SendMessage(GetDlgItem(hProtHelpParams,IDC_FOGTYPE_COMBO), CB_SETCURSEL, 
						(WPARAM)fogType, 0);
	}
}//void AWFogHelpObject::UpdateFogType(AWBoolean fromUI)


void
AWFogHelpObject::UpdateFogColour(AWBoolean fromUI)
{//transfers data to/from the numboids  control
	if (fromUI)
	{	//get data from the user interface
		if (fogColorSwatch)
		{
			fogColour = fogColorSwatch->GetColor();
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		if (fogColorSwatch)
		{
			fogColorSwatch->SetColor(fogColour, FALSE);
		}
	}
}//void AWFogHelpObject::UpdateFogColour(AWBoolean fromUI)



void
AWFogHelpObject::UpdateFogStart(AWBoolean fromUI)
{//transfers data to/from the fog start control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_FOGSTART_SPINNER)); 
		if (spin)
		{
			fogStart = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_FOGSTART_SPINNER)); 
		if (spin)
		{
			spin->SetValue(fogStart, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFogHelpObject::UpdateFogStart(AWBoolean fromUI)


void
AWFogHelpObject::UpdateFogEnd(AWBoolean fromUI)
{//transfers data to/from the fog end control
	ISpinnerControl* spin=NULL;
	if (fromUI)
	{	//get data from the user interface
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_FOGEND_SPINNER)); 
		if (spin)
		{
			fogEnd = spin->GetFVal();
			ReleaseISpinner(spin);
		}
	}//if (fromUI)
	else
	{	//update the user interface with the current data
		spin = GetISpinner(GetDlgItem(hProtHelpParams, IDC_FOGEND_SPINNER)); 
		if (spin)
		{
			spin->SetValue(fogEnd, FALSE);
			ReleaseISpinner(spin);
		}
	}
}//void AWFogHelpObject::UpdateFogEnd(AWBoolean fromUI)





void 
AWFogHelpObject::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	iObjParams = ip;
	editting = TRUE;
	
	if ( !hProtHelpParams ) 
	{
		hProtHelpParams = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_AWFOG_HELPER),
				AWFogHelpObjectDialogProc, 
				GetString(IDS_AWFOG_PARAMETERS), 
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
AWFogHelpObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *prev)
{
	editting = FALSE;
	//update the data from the controls
	UpdateFogType(TRUE);
	UpdateFogColour(TRUE);
	UpdateFogStart(TRUE);
	UpdateFogEnd(TRUE);

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
AWFogHelpObject::BuildMesh()
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
AWFogHelpObject::UpdateUI(TimeValue t)
{
	if ( hProtHelpParams &&	GetWindowLong(hProtHelpParams,GWL_USERDATA)==(LONG)this ) 
	{
		//update the UI from the data
		UpdateFogType(FALSE);
		UpdateFogColour(FALSE);
		UpdateFogStart(FALSE);
		UpdateFogEnd(FALSE);
	}
}//void AWFogHelpObject::UpdateUI(TimeValue t)


AWFogHelpObject::AWFogHelpObject() : HelperObject(), fogColorSwatch(NULL)
{	
	editting = 0;
	created = 0;	
	suspendSnap = FALSE;
	ivalid.SetEmpty();
	BuildMesh();

	fogType		= 0;
	fogColour	= RGB(255, 255, 255);
	fogStart	= 0.0f;
	fogEnd		= 1000.0f;
}

AWFogHelpObject::~AWFogHelpObject()
{
	DeleteAllRefsFromMe();
}


int AWFogHelpObject::NumRefs()
{
	return 1;
}

RefTargetHandle AWFogHelpObject::GetReference(int i)
{
	return NULL;
}

void AWFogHelpObject::SetReference(int i, RefTargetHandle rtarg)
{
}

class AWFogHelpObjCreateCallBack: public CreateMouseCallBack 
{
	AWFogHelpObject *ph;
public:
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	void SetObj(AWFogHelpObject *obj) { ph = obj; }
};

int AWFogHelpObjCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat ) 
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

static AWFogHelpObjCreateCallBack protHelpCreateCB;

CreateMouseCallBack* AWFogHelpObject::GetCreateMouseCallBack() 
{
	protHelpCreateCB.SetObj(this);
	return &protHelpCreateCB;
}

void AWFogHelpObject::GetMat(TimeValue t, INode* inode, ViewExp* vpt, Matrix3& tm) 
{
	tm = inode->GetObjectTM(t);
	tm.NoScale();
	float scaleFactor = vpt->NonScalingObjectSize()*vpt->GetVPWorldWidth(tm.GetTrans())/(float)360.0;
	tm.Scale(Point3(scaleFactor,scaleFactor,scaleFactor));
}

AWBoolean AWFogHelpObject::GetTargetPoint(TimeValue t, Point3 *pt)
{
	return FALSE;
}

void AWFogHelpObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, AWBoolean useSel )
{
	box = mesh.getBoundingBox(tm);
}

void AWFogHelpObject::GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) 
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

void AWFogHelpObject::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
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

int AWFogHelpObject::DrawLines(TimeValue t, INode *inode, GraphicsWindow *gw, int drawing )
{
	Point3 pt[3];
	Matrix3 tm(TRUE);
	gw->setTransform(tm);

	tm = inode->GetObjectTM(t);
	pt[0] = tm.GetTrans();
	if(gw->checkHitCode())
		pt[0].x = 1.0f;
	return gw->checkHitCode();
}

// From BaseObject

int AWFogHelpObject::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) 
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

void AWFogHelpObject::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt) 
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

int AWFogHelpObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) 
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
	
	DrawLines(t, inode, gw, 1);
	UpdateUI(t);
	return(0);
}


// From GeomObject
int AWFogHelpObject::IntersectRay(TimeValue t, Ray& r, float& at) { return(0); }

//
// Reference Managment:
//

// This is only called if the object MAKES references to other things.
RefResult AWFogHelpObject::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
     PartID& partID, RefMessage message ) 
{
	switch (message) {
	case REFMSG_CHANGE:
		if (iObjParams)
			UpdateUI(iObjParams->GetTime());
		ivalid.SetEmpty();
		break;
	case REFMSG_TARGET_DELETED:
		if (iObjParams)
			UpdateUI(iObjParams->GetTime());
		break;
	}
	return REF_SUCCEED;
}

ObjectState AWFogHelpObject::Eval(TimeValue time)
{
	return ObjectState(this);
}

void AWFogHelpObject::Invalidate()
{
	ivalid.SetEmpty();
}

Interval AWFogHelpObject::ObjectValidity(TimeValue t) 
{
	if (ivalid.Empty())
		ivalid = Interval(t,t);
	else {
		ivalid.SetInfinite();
		UpdateUI(t);
	}
	return ivalid;	
}


RefTargetHandle AWFogHelpObject::Clone(RemapDir& remap) 
{
	AWFogHelpObject* newob = new AWFogHelpObject();
	newob->fogType = fogType;
	newob->fogColour = fogColour;
	newob->fogStart	= fogStart;
	newob->fogEnd = fogEnd;
	return(newob);
}



// IO
IOResult 
AWFogHelpObject::Save(ISave *isave) 
{
	//flockCopyObject = GetFlockCopyObject();
	//numBoids		= GetNumBoids();
	ULONG nb;
	isave->BeginChunk(AWFOGHELPER_FOGTYPE_CHUNK);
	isave->Write(&fogType, sizeof(fogType), &nb);
	isave->EndChunk();
	isave->BeginChunk(AWFOGHELPER_COLOUR_CHUNK);
	isave->Write(&fogColour, sizeof(fogColour), &nb);
	isave->EndChunk();
	isave->BeginChunk(AWFOGHELPER_FOGSTART_CHUNK); 
	isave->Write(&fogStart, sizeof(fogStart), &nb);
	isave->EndChunk();
	isave->BeginChunk(AWFOGHELPER_FOGEND_CHUNK);
	isave->Write(&fogEnd, sizeof(fogEnd), &nb);
	isave->EndChunk();
	return IO_OK;
}//IOResult AWFogHelpObject::Save(ISave *isave) 



IOResult 
AWFogHelpObject::Load(ILoad *iload) 
{
	IOResult res;
	ULONG	 nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case AWFOGHELPER_FOGTYPE_CHUNK:
			res=iload->Read(&fogType, sizeof(fogType), &nb);
			break;
		case AWFOGHELPER_COLOUR_CHUNK:
			res=iload->Read(&fogColour, sizeof(fogColour), &nb);
			break;
		case AWFOGHELPER_FOGSTART_CHUNK: 
			res=iload->Read(&fogStart, sizeof(fogStart), &nb);
			break;
		case AWFOGHELPER_FOGEND_CHUNK:
			res=iload->Read(&fogEnd, sizeof(fogEnd), &nb);
			break;
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
	}
	return IO_OK;
}//IOResult AWFogHelpObject::Load(ILoad *iload)




