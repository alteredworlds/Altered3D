#include "AWSettingsCommon.h"
#include "AWDisplaySettings.h"
#include "AWConfigure3D.h"
#include "AWSettingsDlg.h"

#define NUM_DISPLAY_MODES 6
static char* modes[] =
                        {"640 x 480   16bit",
                         "800 x 600   16bit",
                         "1024 x 768  16bit",
                         "640 x 480   32bit",
                         "800 x 600   32bit",
                         "1024 x 768  32bit"
                        };

AWDisplaySettingsDlg::AWDisplaySettingsDlg(AWConfig& cfg,  
                                           HINSTANCE hInst, HWND parent) :
               AWTabPageDlg(hInst, parent),m_data(cfg)
{
   m_IDD = IDD_DISPLAYSETTINGS_TAB;
}


AWBoolean 
AWDisplaySettingsDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                        LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (LOWORD(wParam))
   {
   case IDC_CONFIGURE3D:
      configure3D();
      break;

   case IDC_CURRENT_SETTINGS_CHECK: 
      currentSettingsCheck();
      break;

   case IDC_SWITCH_SETTINGS_CHECK:
      switchSettingsCheck();
      break;
   default:
      break;
   }//switch (id) WM_COMMAND
   return retVal;
}//AWBoolean AWDisplaySettingsDlg::OnDialogCommand(...)



void
AWDisplaySettingsDlg::transferData(AWBoolean fromControls)
{	//from/to display settings section
	//width/height/bpp fullscreen
   if (!m_hDlg) return;
	if (fromControls)
   {  //retrieve the current / switch mode setting
		m_data.m_specifyMode = (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_SWITCH_SETTINGS_CHECK));
		//get the width/height/bpp setting
		//for now, do simple map of index -> width, height
		int mode = SendDlgItemMessage(m_hDlg, IDC_DISPLAY_MODE_LIST, CB_GETCURSEL, 
								(WPARAM)0, (LPARAM)0);
		switch (mode)
		{
		case 0://"640 x 480   16bit"
			m_data.m_width=640;
			m_data.m_height=480;
			m_data.m_bpp=16;
			break;
		case 1://"800 x 600   16bit"
		default:
			m_data.m_width=800;
			m_data.m_height=600;
			m_data.m_bpp=16;
			break;
		case 2://"1024 x 768  16bit"
			m_data.m_width=1024;
			m_data.m_height=768;
			m_data.m_bpp=16;
			break;
		case 3://"640 x 480   32bit"
			m_data.m_width=640;
			m_data.m_height=480;
			m_data.m_bpp=32;
			break;
		case 4://"800 x 600   32bit"
			m_data.m_width=800;
			m_data.m_height=600;
			m_data.m_bpp=32;
			break;
		case 5://"1024 x 768  32bit"
			m_data.m_width=1024;
			m_data.m_height=768;
			m_data.m_bpp=32;
			break;
		}

	}
	else //toControls
	{
      AWCString windowText("3D Configuration - OpenGL");
      SetWindowText(GetDlgItem(m_hDlg, IDC_SELECT_OPENGL), windowText.c_str());
		
		CheckDlgButton(m_hDlg, IDC_CURRENT_SETTINGS_CHECK, 
						m_data.m_specifyMode ? BST_UNCHECKED : BST_CHECKED);
		CheckDlgButton(m_hDlg, IDC_SWITCH_SETTINGS_CHECK, 
						m_data.m_specifyMode ? BST_CHECKED : BST_UNCHECKED);
		EnableWindow(GetDlgItem(m_hDlg, IDC_DISPLAY_MODE_LIST), m_data.m_specifyMode);		
		fillDisplayModeList(); //does nothing if there are already entries in the list
		//select the correct index based on the gamemode
		int index = 0;
		if (m_data.m_bpp == 16)
		{
			switch (m_data.m_width)
			{
			case 640:
				index = 0; 
				break;
			case 800:
				index = 1;
				break;
			case 1024:
				index = 2;
				break;
			}
		}
		else if (m_data.m_bpp == 32)
		{
			switch (m_data.m_width)
			{
			case 640:
				index = 3; 
				break;
			case 800:
				index = 4;
				break;
			case 1024:
				index = 5;
				break;
			}
		}
		SendDlgItemMessage(m_hDlg, IDC_DISPLAY_MODE_LIST, CB_SETCURSEL, 
								(WPARAM)index, (LPARAM)0);
	}//else //toControls
}//void AlteredLauncherDlg::transferData(AWBoolean fromControls)



void
AWDisplaySettingsDlg::currentSettingsCheck()
{
    //if we're setting this, clear the other one
	if (IsDlgButtonChecked(m_hDlg, IDC_CURRENT_SETTINGS_CHECK)==BST_CHECKED)
	{
		CheckDlgButton(m_hDlg, IDC_SWITCH_SETTINGS_CHECK, BST_UNCHECKED);
		EnableWindow(GetDlgItem(m_hDlg, IDC_DISPLAY_MODE_LIST), FALSE);
	}
}//void AWDisplaySettingsDlg::currentSettingsCheck()



void
AWDisplaySettingsDlg::switchSettingsCheck()
{
    //if we're setting this, clear the other one
	if (IsDlgButtonChecked(m_hDlg, IDC_SWITCH_SETTINGS_CHECK)==BST_CHECKED)
	{
		CheckDlgButton(m_hDlg, IDC_CURRENT_SETTINGS_CHECK, BST_UNCHECKED);
		EnableWindow(GetDlgItem(m_hDlg, IDC_DISPLAY_MODE_LIST), TRUE);
	}
}//void AWDisplaySettingsDlg::switchSettingsCheck()



void
AWDisplaySettingsDlg::pickScene()
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[300];       // buffer for filename
	szFile[0]=NULL;
	AWCString dir(m_data.m_scene);
   if (string::npos != dir.find(".tnt"))
	{//need to remove the filename for specification of initial directory
	 //for openfile dialog
		size_t index=0;
		if (string::npos != (index = dir.rfind("\\")))
		{
			if (index+1 < dir.length()) 
         {
            dir.remove(index+1);
         }
		}
	}//if (dir.contains(".tnt", AWCString::ignoreCase))
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
		SetDlgItemText(m_hDlg, IDC_SAVER_SCENE, ofn.lpstrFileTitle);
	}
}//void AWDisplaySettingsDlg::pickScene()



void        
AWDisplaySettingsDlg::configure3D()
{
    AWConfigure3DDlg configure3DDlg(m_data, m_hInst, m_hDlg);
    configure3DDlg.DoModal();
}//void  AWDisplaySettingsDlg::configure3D()



void
AWDisplaySettingsDlg::fillDisplayModeList()
{	
	int num = SendDlgItemMessage(m_hDlg, IDC_DISPLAY_MODE_LIST, CB_GETCOUNT, 0, 0);
	if (0 == num)
	{
		for (int modeNum=0; modeNum < NUM_DISPLAY_MODES; modeNum++)
		{
			SendDlgItemMessage(m_hDlg, IDC_DISPLAY_MODE_LIST, CB_ADDSTRING, (WPARAM)0,
						  (LPARAM)(LPCTSTR)modes[modeNum]);
		}//for
	}//if (LB_ERR != sel)
	//SendDlgItemMessage(m_hDlg, IDC_DISPLAY_MODE_LIST, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);                
}//void AWDisplaySettingsDlg::fillDisplayModeList()