#include "AWSettingsCommon.h"
#include <regstr.h>
#include <AWSaverGame/AWWinUtils.h>
#include "AWConfigure3D.h"


/////////////////////////////////////////////////////////////////////////////
// AWConfigure3DDlg

AWBoolean CALLBACK AWConfigure3DDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	AWBoolean retVal = FALSE;
	AWConfigure3DDlg* dlg = (AWConfigure3DDlg*)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) 
    {
		case WM_INITDIALOG:
			dlg = (AWConfigure3DDlg*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)dlg );
			dlg->initDialog(hDlg);
			return TRUE;			

		case WM_DESTROY:
			return FALSE;

		case WM_MOUSEACTIVATE:
			return FALSE;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			return FALSE;

		case WM_COMMAND:			
			switch( LOWORD(wParam) ) 
			{
         case IDC_3DCONFIG_HELP:
            dlg->help();
            break;
         case IDC_USE_COMPILEDVERTEXARRAYS:
            dlg->selectCVAs();
            break;
         case IDC_USE_DISPLAYLISTS:
            dlg->selectDLists();
            break;
         case IDC_USE_TEXCOMPRESS:
            dlg->selectTexCompress();
            break;
         case IDC_USE_VERTEXARRAYS:
             dlg->selectVAs();
             break;
         case IDC_USE_BUMP:
             dlg->selectBump();
             break;
         case IDC_USE_BUMP_SPECULAR:
            dlg->selectBumpSpecular();
            break;
         case IDC_USE_CUBEENV:
             dlg->selectCubeEnv();
             break;
			case IDOK:
            dlg->ok();
         case IDCANCEL:
             dlg->close();
				break;
			}
			return FALSE;

		default:
			return FALSE;
	}//switch ( message ) 
}//AWBoolean CALLBACK AWConfigure3DDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )




int
AWConfigure3DDlg::DoModal()
{
	return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
		m_parent ? m_parent : GetActiveWindow(), (DLGPROC)DialogProc, (LPARAM)this); 

}


AWConfigure3DDlg::AWConfigure3DDlg(AWConfig& cfg, HINSTANCE hInst, HWND parent) : 
                             m_hWnd(NULL), m_parent(parent), 
							 m_hInst(hInst), m_cfg(cfg)
{
}



void 
AWConfigure3DDlg::initDialog(HWND hwnd)
{
    m_hWnd = hwnd;
    //load the icon to display in the task bar, etc.
   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    transferData(TRUE);
}//void AWConfigure3DDlg::initDialog(HWND hwnd)




void
AWConfigure3DDlg::transferData(AWBoolean toControls)
{
   if (!m_hWnd) return;
   int index = 0;
   if (toControls)
   {
      EnableWindow(GetDlgItem(m_hWnd, IDC_USE_CUBEENV),       m_cfg.getCubeEnvAvailable());
      EnableWindow(GetDlgItem(m_hWnd, IDC_USE_BUMP),          m_cfg.getBumpAvailable());
      EnableWindow(GetDlgItem(m_hWnd, IDC_USE_BUMP_SPECULAR), m_cfg.getBumpAvailable());
      CheckDlgButton(m_hWnd, IDC_USE_CUBEENV,  
                  m_cfg.getUseCubeEnv()  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_BUMP,  
                  m_cfg.getUseBump()  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_BUMP_SPECULAR,  
                  m_cfg.getUseBumpSpecular()  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_VERTEXARRAYS,  
                  m_cfg.m_useVertexArrays  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_DISPLAYLISTS,  
                  m_cfg.m_useDisplayLists  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS,
                  m_cfg.m_useCVAs ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_TEXCOMPRESS,
                  m_cfg.getUseTexCompress() ? BST_CHECKED : BST_UNCHECKED);
    }//if (toControls)
    else //FROM CONTROLS
    {
        m_cfg.m_useVertexArrays = (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_VERTEXARRAYS));
        m_cfg.m_useDisplayLists = (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_DISPLAYLISTS));
        m_cfg.m_useCVAs = (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS));
        m_cfg.setUseBump(BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_BUMP));
        m_cfg.setUseBumpSpecular(BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_BUMP_SPECULAR));
        m_cfg.setUseCubeEnv(BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_CUBEENV));
        m_cfg.setUseTexCompress(BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_TEXCOMPRESS));
    }//else
}//void AWConfigure3DDlg::transferData(AWBoolean toControls)



void    
AWConfigure3DDlg::selectTexCompress()
{
    if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_TEXCOMPRESS))
    {
       ;
    }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
}//void AWConfigure3DDlg::selectTexCompress()



void    
AWConfigure3DDlg::selectCVAs()
{
    if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
    {
       if ((BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_DISPLAYLISTS)))
       {
          //CheckDlgButton(m_hWnd, IDC_USE_DISPLAYLISTS, BST_UNCHECKED);
          lockArraysInformation();
       }
       CheckDlgButton(m_hWnd, IDC_USE_VERTEXARRAYS, BST_CHECKED);
    }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
}//void AWConfigure3DDlg::selectCVAs()


void    
AWConfigure3DDlg::selectVAs()
{
    if (BST_UNCHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_VERTEXARRAYS))
    {
       CheckDlgButton(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS, BST_UNCHECKED);
    }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
}//void AWConfigure3DDlg::selectVAs()



void
AWConfigure3DDlg::lockArraysInformation()
{
   ::MessageBox(m_hWnd, "Objects using Display Lists will not call glLockArrays",
                        "Information - Please note that:", MB_OK);
}//void AWConfigure3DDlg::lockArraysInformation()



void    
AWConfigure3DDlg::selectBump()
{
   if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_BUMP))
   {
      CheckDlgButton(m_hWnd, IDC_USE_CUBEENV, BST_CHECKED);
   }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
   else
   {
      CheckDlgButton(m_hWnd, IDC_USE_BUMP_SPECULAR, BST_UNCHECKED);
   }
}//void  AWConfigure3DDlg::selectBump()


void    
AWConfigure3DDlg::selectBumpSpecular()
{
   if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_BUMP_SPECULAR))
   {
      CheckDlgButton(m_hWnd, IDC_USE_CUBEENV, BST_CHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_BUMP, BST_CHECKED);
   }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
}//void  AWConfigure3DDlg::selectBumpSpecular()


void    
AWConfigure3DDlg::selectCubeEnv()
{
   if (BST_CHECKED != IsDlgButtonChecked(m_hWnd, IDC_USE_CUBEENV))
   {
      CheckDlgButton(m_hWnd, IDC_USE_BUMP, BST_UNCHECKED);
      CheckDlgButton(m_hWnd, IDC_USE_BUMP_SPECULAR, BST_UNCHECKED);
   }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
}//void AWConfigure3DDlg::selectCubeEnv()


void    
AWConfigure3DDlg::selectDLists()
{
   if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_DISPLAYLISTS))
   {
      if ((BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS)))
      {
         //CheckDlgButton(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS, BST_UNCHECKED);
         lockArraysInformation();
      }
   }//if (BST_CHECKED == IsDlgButtonChecked(m_hWnd, IDC_USE_COMPILEDVERTEXARRAYS))
}//void  AWConfigure3DDlg::selectDLists()


void
AWConfigure3DDlg::help()
{
    AWWinUtils::showHTMLHelp(m_cfg.m_help);
}//void AWConfigure3DDlg::help()



void
AWConfigure3DDlg::ok()
{
    transferData(FALSE);
}//void AWConfigure3DDlg::ok()


void
AWConfigure3DDlg::close()
{
    EndDialog(m_hWnd, 0);
}
