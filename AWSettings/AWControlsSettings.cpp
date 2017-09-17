#include "AW3DTools/AWToolsCommon.h"
#include "AWControlsSettings.h"
#include <shlobj.h>

//WARNING - THESE ARE ALSO DEFINED IN AWUserInput.h
#define USE_KEYBOARD 0
#define USE_JOYSTICK 1
#define USE_MOUSE    2

AWBoolean    AWControlsSettingsDlg::m_joystick = -1;



AWControlsSettingsDlg::AWControlsSettingsDlg(AWConfigWin& cfg, HINSTANCE hInst, 
                                             HWND parent)
            :	AWTabPageDlg(hInst, parent), m_data(cfg)
{	
   m_IDD = IDD_AWCONTROLS_TAB;
}



AWBoolean 
AWControlsSettingsDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                        LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (LOWORD(wParam))
   {
   case IDC_PICK_SCREENSHOT_FOLDER:
      pickScreenshotFolder();
      break;
   }//switch (LOWORD(wParam))
   return retVal;
}//AWBoolean AWControlsSettingsDlg::OnDialogCommand(...)



AWBoolean     
AWControlsSettingsDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = AWTabPageDlg::OnInitDialog(hwnd, msg, wParam, lParam);
   //m_joystick 0 none   1 found    -1 not yet tested
   m_joystick = 0; //disable until debugged.
   if ((-1 == m_joystick) && m_data.isGame())
   {  //we have not yet checked for a joystick 
      if(joyGetNumDevs())
      {
         JOYINFO joyinfo;
         m_joystick =  (joyGetPos(JOYSTICKID1,&joyinfo)== JOYERR_NOERROR) ? 1 : 0; 
      }
   }//if ((-1 == m_joystick) && m_data.isGame())
   if (1 != m_joystick) 
   {  //no joystick, so make sure this box is not checked
      CheckDlgButton(m_hDlg, IDC_JOY_RADIO, BST_UNCHECKED);
   }
   EnableWindow(GetDlgItem(m_hDlg, IDC_JOY_RADIO), 1 == m_joystick);
   //EnableWindow(GetDlgItem(m_hDlg, IDC_MOUSE_RADIO), m_data.isGame()); 
   return retVal;
}//AWBoolean AWControlsSettingsDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



void
AWControlsSettingsDlg::transferData(AWBoolean fromControls)
{
   if (!m_hDlg) return;
	if (fromControls)
	{
      if (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_JOY_RADIO))
      {
         m_data.m_userInputMethod = USE_JOYSTICK;
      }
      else if (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_MOUSE_RADIO))
      {
         m_data.m_userInputMethod = USE_MOUSE;
      }
      else
      {
         m_data.m_userInputMethod = USE_KEYBOARD;
      }
      m_data.m_flipY = (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_AWINVERT_Y));
	}//if (fromControls)
	else 
	{
      CheckDlgButton(m_hDlg, IDC_JOY_RADIO,  
         (USE_JOYSTICK == m_data.m_userInputMethod)  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hDlg, IDC_KEY_RADIO,  
         (USE_KEYBOARD == m_data.m_userInputMethod)  ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hDlg, IDC_MOUSE_RADIO,  
         (USE_MOUSE == m_data.m_userInputMethod)     ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(m_hDlg, IDC_AWINVERT_Y,  
         m_data.m_flipY ? BST_CHECKED : BST_UNCHECKED);
     
      char shortForm[MAX_PATH];
      strcpy(shortForm, m_data.m_snapshots.c_str());
      int test = DlgDirList(m_hDlg, shortForm, 0, IDC_SCREENSHOT_FOLDER, 
                            DDL_DIRECTORY | DDL_DRIVES | DDL_EXCLUSIVE);
   }//else //toControls
}//void AlteredLauncherDlg::transferData(AWBoolean fromControls)



// SetSelProc
// Callback procedure to set the initial selection of the browser. 
 
int CALLBACK SetSelProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
if (uMsg==BFFM_INITIALIZED)
{
  ::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData );
}
 
return 0;
}
 
 
// DirectoryPicker
// Display a folder browser allowing the user to browse for a directory. 
 
AWBoolean 
DirectoryPicker( HWND hWnd, AWCString& title, AWCString& dir, 
                          const AWCString &initial )
 
// Input arguments:
// hWnd:  Parent window of dialog
// title  Prompt string at top of dialog
// initial  Initial selection (eg "c:\windows")
//
// Output arguments:
// dir  Selected directory.
 
{
   AWBoolean r = FALSE;
   LPMALLOC pMalloc;
   char buffer[MAX_PATH];
 
   // Retrieve the task memory allocator.
 
   if (SUCCEEDED(::SHGetMalloc(&pMalloc)))
   {
      // Fill in a BROWSEINFO structure, setting the flag to indicate   // that we are only interested in file system directories.

      BROWSEINFO bi;
      bi.hwndOwner = hWnd;
      bi.pidlRoot = NULL;
      bi.pszDisplayName = buffer;
      bi.lpszTitle = (LPCSTR) title.c_str();
      bi.ulFlags = BIF_RETURNONLYFSDIRS;
      bi.iImage = 0;
      bi.lpfn = SetSelProc;
      bi.lParam = (LPARAM)(LPCSTR)initial.c_str();

      // Display the browser.
      ITEMIDLIST * item_list =  ::SHBrowseForFolder(&bi);

      // If the user selected a folder..
      if (item_list)
      {
         // Convert the item ID to a pathname, and copy it to the
         // out parameter.
         if (::SHGetPathFromIDList(item_list, buffer))
         {
            dir = buffer;
            r = TRUE;
         }//if (::SHGetPathFromIDList(item_list, buffer))
         // Free the PIDL allocated by SHBrowseForFolder
         pMalloc->Free(item_list);
      }//if (item_list)
      // Release the shell's allocator
      pMalloc->Release();
   }
   return r;
}//AWBoolean DirectoryPicker( HWND hWnd, AWCString& title, AWCString& dir, 
 //                         const AWCString &initial )


void
AWControlsSettingsDlg::pickScreenshotFolder()
{
   AWCString gotDir(m_data.m_snapshots);
   if (gotDir.length() && (gotDir.at(gotDir.length()-1) == '\\'))
   {
      gotDir.at(gotDir.length()-1) = 0;
   }
   AWBoolean done = DirectoryPicker(m_hDlg, AWCString("Screen shots will end up in this folder:"), 
                               gotDir, gotDir);
	if (done)
	{
		m_data.m_snapshots = gotDir;
      m_data.m_snapshots += "\\";
      char shortForm[MAX_PATH];
      strcpy(shortForm, m_data.m_snapshots.c_str());
	   DlgDirList(m_hDlg, shortForm, 0, IDC_SCREENSHOT_FOLDER, 
                   DDL_DIRECTORY | DDL_DRIVES | DDL_EXCLUSIVE);
		//SetDlgItemText(m_hDlg, IDC_SCREENSHOT_FOLDER, gotDir);
	}
}//void AWControlsSettingsDlg::pickScreenshotFolder()
