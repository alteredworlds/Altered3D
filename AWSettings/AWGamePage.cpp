#include "AW3DTools/AWToolsCommon.h"
#include "AWGamePage.h"
#include "AWSettingsDlg.h"

#define SINGLE_GAMELIST_ENTRY_TEXT "goSpaceGnats - the SaverGame!"

AWGamePageDlg::AWGamePageDlg(AWConfigWin& cfg, HINSTANCE hInst, 
                                             HWND parent)
            :	AWTabPageDlg(hInst, parent),
               m_data(cfg)
{	
   m_IDD = IDD_AWGAMEPAGE;
}


AWBoolean 
AWGamePageDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
   AWTabPageDlg::OnInitDialog(hwnd, msg, wParam, lParam);
   SendDlgItemMessage(m_hDlg, IDC_SAVERGAME_LIST, CB_ADDSTRING, (WPARAM)0,
						  (LPARAM)(LPCTSTR)SINGLE_GAMELIST_ENTRY_TEXT);
   int mode = SendDlgItemMessage(m_hDlg, IDC_SAVERGAME_LIST, CB_SETCURSEL, 
						   (WPARAM)0, (LPARAM)0);
   return TRUE;
}//AWBoolean AWGamePageDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



AWBoolean 
AWGamePageDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                        LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (LOWORD(wParam))
   {
   case IDC_PLAY_NOW:
      OnPlayNow();
      break;
   default:
      break;
   }//switch (id) WM_COMMAND
   return retVal;
}//AWBoolean AWGamePageDlg::OnDialogCommand(...)


void             
AWGamePageDlg::OnPlayNow()
{
   if (m_data.getGame().length())
   {
      transferData(TRUE);
      PostMessage(m_parent, WM_COMMAND, (WPARAM)IDOK, (LPARAM)IDC_PLAY_NOW);
   }
   else
   {
      AWCString msg("No SaverGame seems to be installed on your system\n\n");
      msg += "Please use the \'Content\' page to download \'goSpaceGnats\'!\n";
      ::MessageBox(m_hDlg, msg.c_str(), "No SaverGame installed!", MB_OK);
   }
}//void AWGamePageDlg::OnPlayNow()


void
AWGamePageDlg::transferData(AWBoolean fromControls)
{	
	if (fromControls)
	{
      m_data.m_fullScreen = (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_FULL_SCREEN));     
	}//if (fromControls)
	else 
	{
      CheckDlgButton(m_hDlg, IDC_FULL_SCREEN,  
         (TRUE == m_data.m_fullScreen)  ? BST_CHECKED : BST_UNCHECKED);
   }//else //toControls
}//void AlteredLauncherDlg::transferData(AWBoolean fromControls)