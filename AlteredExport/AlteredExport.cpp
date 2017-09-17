//************************************************************************** 
//* Export.cpp	- TnT Binary File Exporter
//* 
//* By Tom Gilbert
//*
//*
//* This module contains the DLL startup functions
//*
//* Copyright (c) 1998, All Rights Reserved. 
//***************************************************************************
 

#include "AlteredExport.h"
#include "bmmlib.h"
#include <AW3DTools/AWAnimatedTexture.h>
#include <AW3DTools/AWFlightController.h>
#include <AW3DTools/AWFlock.h>
#include <AW3DTools/AWMaterial.h>
#include <AW3DTools/AWFog.h>
#include <AW3DTools/AWSea.h>


HINSTANCE hInstance;
int controlsInit = FALSE;

static AWBoolean showPrompts;
static int lastTexSizeLimitValue = 0;
#define MAX_TEX_LIMIT 2048

//ofstream output("c:\\temp\\output.txt");


// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
#define TNTEXPT_CLASS_ID	Class_ID(0x60ef6373, 0x22dd362a)


AWBoolean WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
	hInstance = hinstDLL;

	// Initialize the custom controls. This should be done only once.
	if (!controlsInit) 
	{
		controlsInit = TRUE;
		InitCustomControls(hInstance);
		InitCommonControls();
	}//if (!controlsInit)
	
	return (TRUE);
}


__declspec( dllexport ) const TCHAR* LibDescription() 
{
	return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS 
__declspec( dllexport ) int LibNumberClasses() 
{
	return 3;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
	switch(i) 
	{
	case 0: return GetTnTExprtDesc();
	case 1: return GetAWHelpDesc();
	case 2: return GetAWFogHelpDesc();
	default: return 0;
	}
}//__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 


__declspec( dllexport ) ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}

class TnTExprtClassDesc:public ClassDesc {
public:
	int				IsPublic() { return 1; }
	void*			Create(AWBoolean loading = FALSE) { return new TnTExprt; } 
	const TCHAR*	ClassName() { return GetString(IDS_TNTEXPRT); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; } 
	Class_ID		ClassID() { return TNTEXPT_CLASS_ID; }
	const TCHAR*	Category() { return GetString(IDS_CATEGORY); }
};

static TnTExprtClassDesc TnTExprtExpDesc;

ClassDesc* GetTnTExprtDesc()
{
	return &TnTExprtExpDesc;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
}


void
TnTExprt::initialise()
{
	// These are the default values that will be active when 
	// the exporter is ran the first time.
	// After the first session these options are sticky.
	bIncludeMesh = TRUE;
	bIncludeAnim = TRUE;
	bIncludeMtl =  FALSE;
	bIncludeAnimTextures = TRUE;
	bIncludeMeshAnim =  TRUE;
	bIncludeCamLightAnim = TRUE;
	bIncludeIKJoints = TRUE;
	bIncludeNormals  =  TRUE;
	bIncludeTextureCoords = TRUE;
	bIncludeVertexColors = TRUE;
	bIncludeObjGeom = TRUE;
	bIncludeObjShape = TRUE;
	bIncludeObjCamera = TRUE;
	bIncludeObjLight = TRUE;
	bIncludeObjHelper = TRUE;
	bAlwaysSample = FALSE;
	nKeyFrameStep = 2;
	nMeshFrameStep = 2;
   nTextureFrameStep = 2;
	nPrecision = 4;
	nStaticFrame = 0;
   bLimitTextureSize = TRUE;
   nTextureSizeLimit = 256;

	exportTextures = TRUE;
	exportCentered = TRUE;
	exportInScene = TRUE;
	shouldScale = FALSE;
	scaleRange = 2000.0;
	m_loopScene = FALSE;
   bMakeEXEScene = FALSE;
   SetUseLimitTextureSize(TRUE);
   SetTextureSizeLimit(256);

   m_cfg.loadExporterInfo();
   bShowMakeEXEScene = m_cfg.getShowMakeExe();
}



TnTExprt::TnTExprt() : m_selCamera(NULL)
{
	initialise();	
}

TnTExprt::~TnTExprt()
{
	for (int i=0; i < m_fogNames.Count(); i++)
	{
		delete m_fogNames[i];
		m_fogNames[i] = NULL;
	}
}

int TnTExprt::ExtCount()
{
	return 1;
}

const TCHAR * TnTExprt::Ext(int n)
{
	switch(n) {
	case 0:
		// This cause a static string buffer overwrite
		// return GetString(IDS_EXTENSION1);
		return _T("TNT");
	}
	return _T("");
}

const TCHAR * TnTExprt::LongDesc()
{
	return GetString(IDS_LONGDESC);
}

const TCHAR * TnTExprt::ShortDesc()
{
	return GetString(IDS_SHORTDESC);
}

const TCHAR * TnTExprt::AuthorName() 
{
	return _T("Tom Gilbert");
}

const TCHAR * TnTExprt::CopyrightMessage() 
{
	return GetString(IDS_COPYRIGHT);
}

const TCHAR * TnTExprt::OtherMessage1() 
{
	return _T("");
}

const TCHAR * TnTExprt::OtherMessage2() 
{
	return _T("");
}

unsigned int TnTExprt::Version()
{
	return 100;
}

static AWBoolean CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		CenterWindow(hWnd,GetParent(hWnd)); 
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}       

void TnTExprt::ShowAbout(HWND hWnd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}



void FillCameraCombo(UINT comboID, HWND hDlg, TnTExprt* exp)
{
	TSTR name = "Active View";
	AWBoolean lastCameraIsValidChoice = FALSE;
	int c = exp->m_cameras.Count();
	//int i = SendMessage(GetDlgItem(hDlg,comboID),
	//						  CB_ADDSTRING, 0, (LPARAM)name.data());
	//SendMessage(GetDlgItem(hDlg,comboID), CB_SETITEMDATA,
	//				i, (LPARAM)NULL);
	for (int i = 0; i < c; i++) 
	{
		// add the name to the list
		TSTR curName = exp->m_cameras[i]->GetName();
		if (exp->GetChosenCameraName() == curName) lastCameraIsValidChoice = TRUE;
		int ind = SendMessage(GetDlgItem(hDlg,comboID),
							  CB_ADDSTRING, 0, (LPARAM)curName.data());
		SendMessage(GetDlgItem(hDlg,comboID), CB_SETITEMDATA,
					ind, (LPARAM)exp->m_cameras[i]);
    
	}//for (i = 0; i < c; i++) 
	
	if (!lastCameraIsValidChoice && c) 
	{	
		name = exp->m_cameras[0]->GetName();
		exp->SetChosenCameraName(name);
	}
	else
	{
		name = exp->GetChosenCameraName();
	}
	// try to set the current selection to the current camera
	SendMessage( GetDlgItem(hDlg, comboID), CB_SELECTSTRING, 0, (LPARAM)name.data());
}//void FillCameraCombo(UINT comboID, HWND hDlg, TnTExprt* exp)



// Dialog proc
static AWBoolean CALLBACK ExportDlgProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	Interval			animRange;
	ISpinnerControl*	spin;
   int            spinnerID;
	char				buf[30];
	int					i;
   
   HICON hIcon;

	TnTExprt *exp = (TnTExprt*)GetWindowLong(hWnd,GWL_USERDATA); 
	switch (msg) {
	case WM_INITDIALOG:
		exp = (TnTExprt*)lParam;
      //load the icon to display in the task bar, etc.
      hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_AWICONE), 
                                     IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
      SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		SetWindowLong(hWnd,GWL_USERDATA,lParam); 
		CenterWindow(hWnd, GetParent(hWnd));
		CheckDlgButton(hWnd, IDC_EXPORT_CENTERED, exp->GetExportCentered()); 
		CheckDlgButton(hWnd, IDC_SHOULD_SCALE, exp->GetShouldScale()); 
		CheckDlgButton(hWnd, IDC_LOOP_SCENE, exp->GetLoopScene());
		sprintf( buf, "%10.3f", exp->GetScaleRange() );
		SetDlgItemText( hWnd, IDC_SCALE_RANGE, buf);

		CheckDlgButton(hWnd, IDC_INTERNAL_TEXTURES, exp->GetExportInScene());
		CheckDlgButton(hWnd, IDC_NO_EXPORTED_TEXTURES, !exp->GetExportTextures()); 

		//CheckDlgButton(hWnd, IDC_MESHDATA, exp->GetIncludeMesh()); 
		CheckDlgButton(hWnd, IDC_ANIMKEYS, exp->GetIncludeAnim()); 
		CheckDlgButton(hWnd, IDC_MATERIAL, exp->GetIncludeMtl());
		CheckDlgButton(hWnd, IDC_MESHANIM, exp->GetIncludeMeshAnim()); 
		CheckDlgButton(hWnd, IDC_TEXTUREANIM, exp->GetIncludeAnimTextures());
		CheckDlgButton(hWnd, IDC_CAMLIGHTANIM, exp->GetIncludeCamLightAnim()); 
		CheckDlgButton(hWnd, IDC_IKJOINTS, exp->GetIncludeIKJoints()); 
		CheckDlgButton(hWnd, IDC_NORMALS,  exp->GetIncludeNormals()); 
		CheckDlgButton(hWnd, IDC_TEXCOORDS,exp->GetIncludeTextureCoords()); 
		CheckDlgButton(hWnd, IDC_VERTEXCOLORS,exp->GetIncludeVertexColors()); 
		CheckDlgButton(hWnd, IDC_OBJ_GEOM,exp->GetIncludeObjGeom()); 
		CheckDlgButton(hWnd, IDC_OBJ_SHAPE,exp->GetIncludeObjShape()); 
		CheckDlgButton(hWnd, IDC_OBJ_CAMERA,exp->GetIncludeObjCamera()); 
		CheckDlgButton(hWnd, IDC_OBJ_LIGHT,exp->GetIncludeObjLight()); 
		CheckDlgButton(hWnd, IDC_OBJ_HELPER,exp->GetIncludeObjHelper());
      CheckDlgButton(hWnd, IDC_MAKEEXECUTABLE,  exp->GetMakeExeScene());
      ShowWindow(GetDlgItem(hWnd, IDC_MAKEEXECUTABLE), 
                  exp->GetShowMakeExeScene() ? SW_SHOW : SW_HIDE);  
      
		CheckRadioButton(hWnd, IDC_RADIO_USEKEYS, IDC_RADIO_SAMPLE, 
			exp->GetAlwaysSample() ? IDC_RADIO_SAMPLE : IDC_RADIO_USEKEYS);
		
		// setup the spinner controls for the controller key sample rate 
		spin = GetISpinner(GetDlgItem(hWnd, IDC_CONT_STEP_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_CONT_STEP), EDITTYPE_INT ); 
		spin->SetLimits(1, 100, TRUE); 
		spin->SetScale(1.0f);
		spin->SetValue(exp->GetKeyFrameStep() ,FALSE);
		ReleaseISpinner(spin);
		
		// setup the spinner controls for the mesh definition sample rate 
		spin = GetISpinner(GetDlgItem(hWnd, IDC_MESH_STEP_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_MESH_STEP), EDITTYPE_INT ); 
		spin->SetLimits(1, 100, TRUE); 
		spin->SetScale(1.0f);
		spin->SetValue(exp->GetMeshFrameStep() ,FALSE);
		ReleaseISpinner(spin);

		// setup the spinner controls for the texture animation sample rate
		spin = GetISpinner(GetDlgItem(hWnd, IDC_TEXTURE_STEP_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_TEXTURE_STEP), EDITTYPE_INT ); 
		spin->SetLimits(1, 100, TRUE); 
		spin->SetScale(1.0f);
		spin->SetValue(exp->GetTextureFrameStep() ,FALSE);
		ReleaseISpinner(spin);

      //set the spinner controls for the texture limit size 
      spin = GetISpinner(GetDlgItem(hWnd, IDC_LIMITTEX_SPIN)); 
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_LIMITTEX_STEP), EDITTYPE_INT ); 
		spin->SetLimits(64, MAX_TEX_LIMIT+1, TRUE); 
		spin->SetScale(1.0f);
		spin->SetValue(exp->GetTextureSizeLimit(), FALSE);
		ReleaseISpinner(spin);
      lastTexSizeLimitValue = exp->GetTextureSizeLimit();

      //should we use the above setting
      CheckDlgButton(hWnd, IDC_LIMIT_TEXTURES, exp->GetUseLimitTextureSize());
      
		//Fill the Camera/View combo box from the pre-fetched list of cameras in the scene
		FillCameraCombo(IDC_CAMERA_COMBO, hWnd, exp);

		// Enable / disable mesh options
		EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), exp->GetIncludeMesh());
		EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), exp->GetIncludeMesh());
		EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), exp->GetIncludeMesh());
		break;

	case CC_SPINNER_CHANGE:
		spin      = (ISpinnerControl*)lParam; 
      spinnerID = LOWORD(wParam);
      if (lastTexSizeLimitValue &&(IDC_LIMITTEX_SPIN == spinnerID))
      {  //we need  to mak sure that this control shows POW 2 only
         int newVal = spin->GetIVal();
         newVal = AWTexture::fitToNextNearestPowerOf2(lastTexSizeLimitValue, newVal > lastTexSizeLimitValue);
         newVal = (newVal > MAX_TEX_LIMIT) ? MAX_TEX_LIMIT : newVal;
         spin->SetValue(newVal, FALSE);
         lastTexSizeLimitValue = newVal;
      }
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_INTERNAL_TEXTURES:
		case IDC_EXTERNAL_TEXTURES:
			CheckDlgButton(hWnd, IDC_TEXCOORDS, BST_CHECKED); 
			EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), TRUE);
			break;
		case IDC_NO_EXPORTED_TEXTURES:
			CheckDlgButton(hWnd, IDC_TEXCOORDS, BST_UNCHECKED); 
			EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), FALSE);
			break;
		case IDOK:
         exp->SetMakeExeScene( exp->GetShowMakeExeScene() ? 
                               IsDlgButtonChecked(hWnd, IDC_MAKEEXECUTABLE) : FALSE );
			exp->SetIncludeAnim(IsDlgButtonChecked(hWnd, IDC_ANIMKEYS)); 
			exp->SetIncludeMtl(IsDlgButtonChecked(hWnd, IDC_MATERIAL)); 
			exp->SetIncludeMeshAnim(IsDlgButtonChecked(hWnd, IDC_MESHANIM)); 
			exp->SetIncludeCamLightAnim(IsDlgButtonChecked(hWnd, IDC_CAMLIGHTANIM)); 
			exp->SetIncludeIKJoints(TRUE); 
			exp->SetIncludeNormals(IsDlgButtonChecked(hWnd, IDC_NORMALS));
			exp->SetIncludeTextureCoords(IsDlgButtonChecked(hWnd, IDC_TEXCOORDS)); 
			exp->SetIncludeVertexColors(IsDlgButtonChecked(hWnd, IDC_VERTEXCOLORS)); 
         exp->SetIncludeObjGeom(TRUE); 
			exp->SetIncludeObjShape(TRUE); 
			exp->SetIncludeObjCamera(TRUE); 
			exp->SetIncludeObjLight(IsDlgButtonChecked(hWnd, IDC_OBJ_LIGHT)); 
			exp->SetIncludeObjHelper(TRUE);
			exp->SetAlwaysSample(IsDlgButtonChecked(hWnd, IDC_RADIO_SAMPLE));

			exp->SetExportCentered(IsDlgButtonChecked(hWnd, IDC_EXPORT_CENTERED )); 
			exp->SetShouldScale(IsDlgButtonChecked(hWnd, IDC_SHOULD_SCALE));
			exp->SetLoopScene(FALSE);
			exp->SetIncludeAnimTextures(IsDlgButtonChecked(hWnd, IDC_TEXTUREANIM));

			
			if (exp->GetShouldScale()) 
			{
				GetDlgItemText(hWnd, IDC_SCALE_RANGE, buf,	30);
				exp->SetScaleRange(atof(buf));
			}

			i = SendMessage(GetDlgItem(hWnd,IDC_CAMERA_COMBO),
                                    CB_GETCURSEL, 0, 0);
			if (i != CB_ERR) 
			{
				//sets current camera to item data for selected combo item
				//NOTE currently this may be NULL if Active View (default) is selected
				//first record the name of the chosen camera for next time
				//now get the camera pointer
				exp->SetCamera((INode *)
						   SendMessage(GetDlgItem(hWnd, IDC_CAMERA_COMBO),
									   CB_GETITEMDATA, (WPARAM)i,
									   0));
			}//if (index != CB_ERR)
			else exp->SetCamera(NULL);
			exp->SetExportInScene(IsDlgButtonChecked(hWnd, IDC_INTERNAL_TEXTURES));
			exp->SetExportTextures(!IsDlgButtonChecked(hWnd, IDC_NO_EXPORTED_TEXTURES));
         exp->SetUseLimitTextureSize(IsDlgButtonChecked(hWnd, IDC_LIMIT_TEXTURES));

			spin = GetISpinner(GetDlgItem(hWnd, IDC_CONT_STEP_SPIN)); 
			exp->SetKeyFrameStep(spin->GetIVal()); 
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd, IDC_MESH_STEP_SPIN)); 
			exp->SetMeshFrameStep(spin->GetIVal());
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd, IDC_TEXTURE_STEP_SPIN)); 
			exp->SetTextureFrameStep(spin->GetIVal());
			ReleaseISpinner(spin);

         spin = GetISpinner(GetDlgItem(hWnd, IDC_LIMITTEX_SPIN)); 
			exp->SetTextureSizeLimit(spin->GetIVal());
			ReleaseISpinner(spin);

			EndDialog(hWnd, 1);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;

      case IDHELP:
         exp->ShowHelp();
         break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}   


void
TnTExprt::ShowHelp()
{
   m_cfg.loadExporterInfo();
   AWWinUtils::showHTMLHelp(m_cfg.getExportHelp());
}//void TnTExprt::ShowHelp()



// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}





//NOTES - we can move some of this processing into the engine rather
//than the exporter. we do not need to populate the flat lists now.
void
TnTExprt::postProcess(Interface* ip)
{	//after the node enumeration is complete, perform necessary
   //housekeeping so that the Altered World is self-consistent
   //
   //FIRST - populate the flat lists from m_root
   theScene.compact();
   theScene.m_nodeList.addNodeAndChildrenToFlatList(&theScene.m_nodeList.getRoot());
   //now init all camera objects, and make sure we have a current camera
   AWCamera* curCam = initCamera(ip);

   //process PROJECTILES and associated EFFECT objects
   theScene.m_nodeList.initProjectiles();
   theScene.m_nodeList.initBillboardedNodes(curCam);

   //scale the scene if desired
   if (GetShouldScale()&&GetScaleRange()) 
   {  //acts on m_nodeList and m_projectiles
      theScene.fitToRange(GetScaleRange());
   }//if (GetShouldScale()&&GetScaleRange()) 

	//if the user has selected LOOP SCENE, set this flag on the scene
	//new 090599 feature
	if (GetLoopScene())
	{
		Interval interval(ip->GetAnimRange());
		theScene.setLoopTime(TicksToSec(interval.End()-interval.Start()));
	}
	else
		theScene.setLoopTime(0.0f);
   //now deal with AWBumpMesh LIGHT allocation
   for (int bump=0; bump<m_trackBumps.Count(); bump++)
   {  //see if the INode has user data "BUMPLIGHT" - if so find 
      //the corresponding AWLight, assign to AWBumpMesh::m_bumpLight      
      TSTR strVal;
      AWLight* l = theScene.m_nodeList.getNumLights() ? 
                        theScene.m_nodeList.getLight(0) : NULL;
      if (m_trackBumps[bump]->m_node && 
          m_trackBumps[bump]->m_node->GetUserPropString(_T("BUMPLIGHT"), strVal))
	   {  //light name should now be in strVal - try and find it
         AWLight* tmpl = theScene.m_nodeList.getLightByName(strVal.data());
         if (tmpl)
         {
            l = tmpl;
         }		   	
	   }//if (node->GetUserPropString(_T("BUMPLIGHT"), strVal))
      if (m_trackBumps[bump]->m_mesh)
      {
         m_trackBumps[bump]->m_mesh->m_bumpLight = l;
      }//if (m_trackBumps[bump].m_mesh)
   }//for (int bump=0; bump<m_trackBumps.Count(); bump++)
}//void TnTExprt::postProcess()



// Start the exporter!
// This is the real entrypoint to the exporter. After the user has selected
// the filename (and he's prompted for overwrite etc.) this method is called.
int TnTExprt::DoExport(const TCHAR* filename, ExpInterface* ei, Interface* i, AWBoolean suppressPrompts, DWORD options) 
{
#ifdef __AWTRACE
   FILE* stream = freopen( "AlteredExport.log", "wt", stderr );   
   if( stream == NULL )
   {
      fprintf( stdout, "error on freopen\n" );   
   }
   else   
   {
      fprintf( stream, "AlteredExport log file\n" );  
   }
#endif
	try
	{
		// set a global prompt display switch
		showPrompts = suppressPrompts ? FALSE : TRUE;

		// Grab the interface pointer.
		ip = i;

		// get the options the user selected the last time
		ReadConfig();

		//NOTE: VRML exporter uses a usre specified STATIC FRAME. We're gonna use the first frame
		//instead as can't see what useful purpose a user specified static frame serves
		nStaticFrame = i->GetTime();
		//nStaticFrame = i->GetAnimRange().Start() / GetTicksPerFrame();
		// get a total node count by traversing the scene
		// We don't really need to do this, but it doesn't take long, and
		// it is nice to have an accurate progress bar.
		nTotalNodeCount = 0;
		nCurNode = 0;
		PreProcess(ip->GetRootNode(), nTotalNodeCount);
      if (0==m_cameras.Count())
      {
        ::MessageBox(NULL, "Please put a Camera in this scene and try this export again.", "SORRY - You'll Need a Camera!", MB_OK);
         return 1;
      }
		if(showPrompts) 
		{
			// Prompt the user with our dialogbox, and get all the options.
			if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_TNTEXPORT_DLG),
				ip->GetMAXHWnd(), ExportDlgProc, (LPARAM)this)) 
			{
				return 1;
			}
		}//if(showPrompts) 
		else 
		{	// set default parameters here
			initialise();
		}
      //new collection allowing assignment of AWLight* in AWBumpMesh, if any
      clearTrackBumps();
      
		// Startup the progress bar.
		ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, fn, NULL);
		
		// First we get global information for scene 
		ExportGlobalInfo();

		int numChildren = ip->GetRootNode()->NumberOfChildren();
		
		// Call our node enumerator.
		// The nodeEnum function will recurse into itself and 
		// export each object found in the scene.	
		for (int idx=0; idx<numChildren; idx++) 
		{	//these nodes are all the first level children of the
			//root node. 
			if (ip->GetCancel()) break;		
			nodeEnum(ip->GetRootNode()->GetChildNode(idx), &theScene.m_nodeList.getRoot());
		}//for (int idx=0; idx<numChildren; idx++) 

      if (ip->GetCancel())
      {
         clearTrackBumps();
         clearFogNames();
         clearCameras();
         clearMaterials();
         return 0;
      }
		   postProcess(ip);
		   
		   //all data has been collected - now save the scene
         if (AWFile::Exists(filename))
         {  //delete file before writing
            remove(filename);
         }
		   AWFile of(filename, "wb");
		   if (of.isValid())
		   {
			   of << theScene;
			   of.Flush();
		   }
         of.Close();
         if (GetMakeExeScene())
            produceExeVersion(filename);
	   
		   //We're done. Finish the progress bar.
		   ip->ProgressEnd();
		   //enumerate the textures
		   AWCString exportMsg(filename);
		   if (GetExportTextures() && theScene.m_textureList.getNum())
		   {
			   exportMsg += "\n\nUsing Texture sources:\n\n";
			   AWTexture* tex = NULL;
			   for (int i=0; i < (int)theScene.m_textureList.getNum(); i ++)
			   {
				   tex = (AWTexture*)theScene.m_textureList[i];
				   exportMsg += "Material Name: " + tex->m_name;
				   exportMsg += "  file name: "   + tex->m_fileName;
				   exportMsg += "\n";
			   }
			   exportMsg += "\n";
		   }//if (GetExportTextures() && theScene.m_textureList.entries())
         clearTrackBumps();
         clearFogNames();
         clearCameras();
         clearMaterials();
         // Write the current options to be used next time around.
		   WriteConfig();

         exportMsg += "\n\nPlease click YES to view now, NO to finish.\n";
		   if (IDYES == ::MessageBox(NULL, exportMsg, "Scene has been exported as:", MB_YESNO))
		   {
			   ShellExecute( NULL, 
				   "open",  // pointer to string that specifies operation to perform 
				   filename,  // pointer to filename or folder name string 
				   NULL, // pointer to string that specifies executable-file parameters 
				   NULL,  // pointer to string that specifies default directory 
				   SW_SHOW);   // whether file is shown when opened 
		   }	
	}
	catch (...)
	{
		return 0;
	}

#ifdef __AWTRACE
   fprintf( stream, "AlteredExport Complete\n" ); 
   fclose( stream );
#endif
	return 1;
}//int TnTExprt::DoExport(const TCHAR* filename, ExpInterface* ei, Interface* i, AWBoolean suppressPrompts) 


// This method is the main object exporter.
// It is called once of every node in the scene. The objects are
// exported as they are encoutered.

// Before recursing into the children of a node, we will export it.
// The benefit of this is that a nodes parent is always before the
// children in the resulting file. This is desired since a child's
// transformation matrix is optionally relative to the parent.

AWBoolean 
TnTExprt::nodeEnum(INode* node, AWNode* parent) 
{
	nCurNode++;
	ip->ProgressUpdate((int)((float)nCurNode/nTotalNodeCount*100.0f)); 

	// Stop recursing if the user pressed Cancel 
	if (ip->GetCancel()) return FALSE;

	AWNode* thisNode = NULL;
    AWBoolean transitionNode = FALSE;
	// If this node is a group head, all children are 
	// members of this group. The node will be a dummy node and the node name
	// is the actualy group name.
	
	// The ObjectState is a 'thing' that flows down the pipeline containing
	// all information about the object. By calling EvalWorldState() we tell
	// max to eveluate the object at end of the pipeline.
	ObjectState os = node->EvalWorldState(GetStaticFrame()*GetTicksPerFrame()); 

	// The obj member of ObjectState is the actual object we will export.
	if (os.obj && !node->IsHidden()) 
	{	//We look at the super class ID to determine the type of the object.
		//The Export helper functions all take the parent paramenter (the parent of any
		//new node created) and return the new node.
		switch(os.obj->SuperClassID()) 
		{
		case GEOMOBJECT_CLASS_ID: 
			if (GetIncludeObjGeom())  
			{
				thisNode = ExportGeomObject(node, parent); 
			}
			break;

		case SHAPE_CLASS_ID:
			if (GetIncludeObjShape()) 
         {  //for current Game Engine, all objects must be in TRIANGLE mesh
				//so treat a shape as a regular mesh object
				thisNode = ExportGeomObject(node, parent);
			}
			break;

		case CAMERA_CLASS_ID:
			if (GetIncludeObjCamera()) 
			{
				thisNode = ExportCameraObject(node, parent);
			}
			break;

		case LIGHT_CLASS_ID:
			if (GetIncludeObjLight()) 
				thisNode = ExportLightObject(node, parent); 
			break;
			
		case HELPER_CLASS_ID:
			if (GetIncludeObjHelper()) 
				thisNode = ExportHelperObject(node, parent); 
			break;
		}//switch(os.obj->SuperClassID()) 
	}//if (os.obj && !node->IsHidden())
   else if (node->NumberOfChildren())
   {  //hidden nodes acting as parents in an object heirachy
      //will need to be exported in case any child is visible
      thisNode = ExportDummyNode(node);
   }//else
	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
    if (node->IsGroupHead() && thisNode && 
         thisNode->getName().contains( _T("levelEnd")))
    {
        theScene.m_nodeList.setLevelEnd(thisNode);
    }
    else if (thisNode && parent)
    {
        parent->addChild(thisNode);
    }
	for (int c = 0; c < node->NumberOfChildren(); c++) 
	{
		if ( !nodeEnum(node->GetChildNode(c), thisNode) )
		{
			return FALSE;
		}
	}//for (int c = 0; c < node->NumberOfChildren(); c++)
	
	// If thie is true here, it is the end of the group we started above.
	if (node->IsGroupHead()) 
	{
	}

	return TRUE;
}//AWBoolean TnTExprt::nodeEnum(INode* node, int indentLevel) 




void 
TnTExprt::PreProcess(INode* node, int& nodeCount)
{
	nodeCount++;
	
	//for any node of type camera, add to list of Camera nodes to display in the 
	//export dialog box
	const ObjectState& os = node->EvalWorldState(nStaticFrame*GetTicksPerFrame());
   Object* ob = os.obj;
   if ((ob != NULL) && (ob->SuperClassID() == CAMERA_CLASS_ID) )
   {
      m_cameras.Append(1, &node);
   }// if ((ob != NULL) && (ob->SuperClassID() == CAMERA_CLASS_ID) )
	
	// For each child of this node, we recurse into ourselves 
	// and increment the counter until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) 
	{
		PreProcess(node->GetChildNode(c), nodeCount);
	}
}//void TnTExprt::PreProcess(INode* node, int& nodeCount)


/****************************************************************************

 Configuration.
 To make all options "sticky" across sessions, the options are read and
 written to a configuration file every time the exporter is executed.

 ****************************************************************************/

TSTR TnTExprt::GetCfgFilename()
{
	TSTR filename;
	
	filename += ip->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += CFGFILENAME;

	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x06

AWBoolean TnTExprt::ReadConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	// First item is a file version
	int fileVersion = _getw(cfgStream);

	if (fileVersion > CFG_VERSION) {
		// Unknown version
		fclose(cfgStream);
		return FALSE;
	}

	//SetIncludeMesh(fgetc(cfgStream));
	SetIncludeAnim(fgetc(cfgStream));
	SetIncludeMtl(fgetc(cfgStream));
	SetIncludeMeshAnim(fgetc(cfgStream));
	SetIncludeCamLightAnim(fgetc(cfgStream));
	SetIncludeIKJoints(fgetc(cfgStream));
	SetIncludeNormals(fgetc(cfgStream));
	SetIncludeTextureCoords(fgetc(cfgStream));
	SetIncludeObjGeom(fgetc(cfgStream));
	SetIncludeObjShape(fgetc(cfgStream));
	SetIncludeObjCamera(fgetc(cfgStream));
	SetIncludeObjLight(fgetc(cfgStream));
	SetIncludeObjHelper(fgetc(cfgStream));
	SetAlwaysSample(fgetc(cfgStream));

	if (fileVersion > 0x04)
	{
		SetLoopScene(fgetc(cfgStream));
	}

	SetMeshFrameStep(_getw(cfgStream));
	SetKeyFrameStep(_getw(cfgStream));

	// Added for version 0x02
	if (fileVersion > 0x01) 
	{
		SetIncludeVertexColors(fgetc(cfgStream));
	}

	// Added for version 0x03
	if (fileVersion > 0x02) 
	{
		SetPrecision(_getw(cfgStream));
	}

	// Added for version 0x04
	if (fileVersion > 0x03) 
	{
		SetExportCentered( _getw(cfgStream) );			
		SetShouldScale( fgetc(cfgStream) );
		SetExportTextures( fgetc(cfgStream) );
		SetExportInScene( fgetc(cfgStream) );
		SetIncludeAnimTextures( fgetc(cfgStream));

		double c_style_bullshit = GetScaleRange();
		fread( &c_style_bullshit, sizeof(c_style_bullshit), 1, cfgStream );
		SetScaleRange(c_style_bullshit);

		c_style_bullshit = GetTextureFrameStep();
		fread( &c_style_bullshit, sizeof(c_style_bullshit), 1, cfgStream );
		SetTextureFrameStep(c_style_bullshit);
		SetIncludeNormals( fgetc(cfgStream) );
		//
		//first attempt at reading the chosen camera string
		int camStringLen = _getw(cfgStream);
		TCHAR* temp = new TCHAR[camStringLen+1];
		fread(temp, sizeof(TCHAR), camStringLen, cfgStream);
		temp[camStringLen] = 0;
		chosenCamera = temp;
		delete [] temp;
	}

   if (fileVersion >= 0x06)
   {
      SetMakeExeScene(fgetc(cfgStream));
      SetShowMakeExeScene(fgetc(cfgStream));
   }
	fclose(cfgStream);

	return TRUE;
}

void TnTExprt::WriteConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;

	// Write CFG version
	_putw(CFG_VERSION,				cfgStream);

	//fputc(GetIncludeMesh(),			cfgStream);
	fputc(GetIncludeAnim(),			cfgStream);
	fputc(GetIncludeMtl(),			cfgStream);
	fputc(GetIncludeMeshAnim(),		cfgStream);
	fputc(GetIncludeCamLightAnim(),	cfgStream);
	fputc(GetIncludeIKJoints(),		cfgStream);
	fputc(GetIncludeNormals(),		cfgStream);
	fputc(GetIncludeTextureCoords(),	cfgStream);
	fputc(GetIncludeObjGeom(),		cfgStream);
	fputc(GetIncludeObjShape(),		cfgStream);
	fputc(GetIncludeObjCamera(),	cfgStream);
	fputc(GetIncludeObjLight(),		cfgStream);
	fputc(GetIncludeObjHelper(),	cfgStream);
	fputc(GetAlwaysSample(),		cfgStream);
	fputc(GetLoopScene(),			cfgStream);
	_putw(GetMeshFrameStep(),		cfgStream);
	_putw(GetKeyFrameStep(),		cfgStream);
	fputc(GetIncludeVertexColors(),	cfgStream);
	_putw(GetPrecision(),			cfgStream);

	_putw(GetExportCentered(), cfgStream );			
	fputc(GetShouldScale(), cfgStream );
	fputc(GetExportTextures(), cfgStream );
	fputc(GetExportInScene(), cfgStream );
	fputc(GetIncludeAnimTextures(), cfgStream );

	double c_style_bullshit = GetScaleRange();
	fwrite( &c_style_bullshit, sizeof(c_style_bullshit), 1, cfgStream );

	c_style_bullshit = GetTextureFrameStep();
	fwrite( &c_style_bullshit, sizeof(c_style_bullshit), 1, cfgStream );

	fputc(GetIncludeNormals(), cfgStream );
	//first attempt at writing the chosen camera string
	_putw((int)chosenCamera.length(), cfgStream);
	fwrite(chosenCamera, sizeof(TCHAR), chosenCamera.length(), cfgStream);
   fputc(bMakeEXEScene,		cfgStream);
   fputc(bShowMakeEXEScene,		cfgStream);
	fclose(cfgStream);
}
