#include "AWSettingsCommon.h"
#include "AWSceneSettings.h"
#include "AWSettingsDlg.h"


AWSceneSettingsDlg::AWSceneSettingsDlg(AWConfig& cfg, HINSTANCE hInst, HWND parent)
    :	AWTabPageDlg(hInst, parent), m_data(cfg)
{	
   m_IDD = IDD_SCENES_TAB;
}

   
            
AWBoolean 
AWSceneSettingsDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = AWTabPageDlg::OnInitDialog(hwnd, msg, wParam,  lParam);
   SendDlgItemMessage(m_hDlg, IDC_SWITCH_TIMEOUT_SPIN, UDM_SETRANGE, 
				            (WPARAM)0, (LPARAM) MAKELONG((short)100, (short)1));
   return retVal;
}//AWBoolean AWSceneSettingsDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam,  LPARAM lParam)



AWBoolean 
AWSceneSettingsDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                        LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (LOWORD(wParam))
   {
   case IDC_PICK_SAVER_SCENE:
      pickScene();
      break;  
   case IDC_PREVIEWSAVER:
      transferData(TRUE);
      PostMessage(m_parent, WM_COMMAND, (WPARAM)IDOK, (LPARAM)IDC_PREVIEWSAVER);
      break;
   }//switch (LOWORD(wParam))
   return retVal;
}//AWBoolean AWSceneSettingsDlg::OnDialogCommand(...)
 


void
AWSceneSettingsDlg::transferData(AWBoolean fromControls)
{	//from/to display settings section
	//width/height/bpp fullscreen
   if (!m_hDlg) return;
	if (fromControls)
   {  //retrieve the current / switch mode setting
		AWBoolean junk;
		m_data.m_autoSwitchScenes = (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_AUTO_SCENE_SWITCH));
		m_data.m_sceneSwitchMinutes = GetDlgItemInt( m_hDlg, IDC_SWITCH_TIMEOUT_EDIT, &junk, FALSE);
	}
	else //toControls
	{
		CheckDlgButton(m_hDlg, IDC_AUTO_SCENE_SWITCH, 
			            m_data.m_autoSwitchScenes ? BST_CHECKED : BST_UNCHECKED);
      if (m_data.m_sceneSwitchMinutes < 1)
      {
         m_data.m_sceneSwitchMinutes = 1;
      }
		SetDlgItemInt( m_hDlg, IDC_SWITCH_TIMEOUT_EDIT, m_data.m_sceneSwitchMinutes, FALSE);
      //show the user just the scene name
      AWCString sceneName(AWScene::getSceneName(m_data.m_scene));
		//for openfile dialog
	   SetDlgItemText(m_hDlg, IDC_SAVER_SCENE, sceneName.c_str());
	}//else //toControls
}//void AlteredLauncherDlg::transferData(AWBoolean fromControls)



void
AWSceneSettingsDlg::pickScene()
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[300];       // buffer for filename
	szFile[0]=NULL;
   AWCString dir(AWScene::getSceneDirectory(m_data.m_scene));
	// Initialize OPENFILENAME
	char fileNameOnly[100];
	fileNameOnly[0]=NULL;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hDlg;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Altered Worlds\0*.tnt\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = fileNameOnly;
	ofn.nMaxFileTitle = 99;
	ofn.lpstrTitle  = "Choose your World...";
	ofn.lpstrDefExt = "tnt";
	/*use the initial dir*/
	ofn.lpstrInitialDir = dir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOTESTFILECREATE;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn)==TRUE) 
	{
		//record the full path
		m_data.m_scene = ofn.lpstrFile;
		//but just show the user the file name
      AWCString showName(AWScene::getSceneName(ofn.lpstrFileTitle));
      SetDlgItemText(m_hDlg, IDC_SAVER_SCENE, showName.c_str());
      //now we need to rebuild the scenes list, as we may now be looking
      //at a new directory
      m_data.buildSceneList();
	}//if (GetOpenFileName(&ofn)==TRUE)
}//void AWSceneSettingsDlg::pickScene()
