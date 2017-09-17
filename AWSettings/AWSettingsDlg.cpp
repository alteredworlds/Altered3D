#include "AWSettingsCommon.h"
#include "AWSettingsDlg.h"

#ifndef _AW_MINIMAL_CONFIG_GUI
#define AW_TAB_SAVER_PAGE       0
#define AW_TAB_DISPLAY_PAGE     1
#define AW_TAB_SOUND_PAGE       2
#define AW_TAB_CONTROLS_PAGE    3
#define AW_TAB_CONTENT_PAGE     4
#define AW_TAB_GAME_PAGE        5
#else
#define AW_TAB_CONTENT_PAGE     0
#endif



AWBoolean CALLBACK 
AWSettingsDlg::DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
   AWBoolean retVal = FALSE;
   AWSettingsDlg* ss = (AWSettingsDlg*)GetWindowLong(hwnd, GWL_USERDATA);
   if ( !ss && msg != WM_INITDIALOG ) return FALSE;
   switch (msg)
   { 
   case WM_INITDIALOG:
      retVal = TRUE;
      ss = (AWSettingsDlg*)lParam;
      SetWindowLong(hwnd, GWL_USERDATA, (LONG)ss);
      if (ss) 
      {
         ss->OnInitDialog(hwnd, wParam, lParam, retVal);
      }
      break;

   case WM_NOTIFY:   
      if (IDC_SAVERCONFIG_TAB == (int)wParam)
      {
         LPNMHDR pnmh = (LPNMHDR)lParam;
         if (TCN_SELCHANGE == pnmh->code)
         {
            retVal = TRUE;
            ss->onSelChanged();
         }//if (TCN_SELCHANGE == pnmh->code)
      }//if (IDC_SAVERCONFIG_TAB == (int)wParam)
      break; //case WM_NOTIFY:

   case WM_TIMER:
      ss->onTimer((UINT)wParam);
      break;

   case WM_AWGET_UPDATE:
      ss->getUpdate();
      break;
       
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDC_VISIT_ALTEREDWORLDS:
         ss->visitAlteredWorlds();
         break;

      case IDC_ABOUT_ALTEREDSAVER:
         ss->about();
         break;

      case IDHELP:
         AWWinUtils::showHTMLHelp(ss->m_data.m_help);
         break;

      case IDOK:
         ss->OnOK(hwnd, msg, wParam, lParam);
      case IDCANCEL:
         ss->close();
      default:
         break;
	    }//switch (id) WM_COMMAND
	    break;          
    }//switch (msg)
    return retVal;
}//AWBoolean CALLBACK AWSettingsDlg::DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



int
AWSettingsDlg::DoModal()
{
	return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
		m_parent ? m_parent : GetActiveWindow(), (DLGPROC)DialogProc, (LPARAM)this); 

}


void
AWSettingsDlg::getUpdate()
{
   m_exitCode = IDC_UPDATESAVER;
   close();
}//void AWSettingsDlg::getUpdate()


void
AWSettingsDlg::visitAlteredWorlds()
{
   if (m_data.isRegistered())
   {
      AWWinUtils::visitAlteredWorlds();
   }
   else
   {
      if (AWWinUtils::buyNowOrRegister(m_data, m_hInst, m_hDlg, IDC_VISIT_ALTEREDWORLDS))
      {  //we just attempted to register the product
         //so refresh the screen in case it worked.
         m_data.loadUserInfo();
         if (m_data.isRegistered())
         {
            if (m_timerID)
            {
               KillTimer(m_hDlg, m_timerID);
               m_timerID = 0;
            }//if (m_timerID)
            SetDlgItemText(m_hDlg, IDC_VISIT_ALTEREDWORLDS, "Visit AlteredWorlds.com!");
         }//if (!m_data.isRegistered())
      }//if (AWWinUtils::buyNowOrRegister(...)
   }
}//void AWSettingsDlg::visitAlteredWorlds()



AWSettingsDlg::AWSettingsDlg(AWConfigWin& data, HINSTANCE hInst) : 					
               m_data(data),
               m_hDlg(NULL),
               m_hInst(hInst),                          
               m_contentDlg(m_data, hInst),
               m_timerID(0),
               m_buyNowVisible(TRUE),
               m_exitCode(0),
#ifdef _AW_MINIMAL_CONFIG_GUI 
               m_curTabDlg(NULL)
#else
               m_curTabDlg(NULL),
               m_gameDlg(m_data, hInst),
               m_controlsDlg(m_data, hInst),
               m_soundDlg(m_data, hInst),
               m_sceneDlg(m_data, hInst),
               m_displayDlg(m_data, hInst)
#endif
               
{
   m_parent = (HWND)m_data.getWinID();
}


// onSelChanged - processes the TCN_SELCHANGE notification. 
void
AWSettingsDlg::onSelChanged() 	
{ 
   int iSel = TabCtrl_GetCurSel(m_hTab);  
   // Destroy the current child dialog box, if any. 
   if (m_curTabDlg)
   {
      m_curTabDlg->transferData(TRUE);
      m_curTabDlg->hideWindow();
   }
   if (AW_TAB_CONTENT_PAGE == iSel)
   {
      m_curTabDlg = &m_contentDlg;
   }
#ifndef _AW_MINIMAL_CONFIG_GUI
   else if (AW_TAB_SAVER_PAGE == iSel)
   {
      m_curTabDlg = &m_sceneDlg;
   }
   else if (AW_TAB_DISPLAY_PAGE == iSel)
   {
      m_curTabDlg = &m_displayDlg;
   }

   else if (AW_TAB_SOUND_PAGE == iSel)
   {
      m_curTabDlg = &m_soundDlg;
   }
   else if (AW_TAB_GAME_PAGE == iSel)
   {
      m_curTabDlg = &m_gameDlg;
   }
   else if (AW_TAB_CONTROLS_PAGE == iSel)
   {
      m_curTabDlg = &m_controlsDlg;
   }
#endif
   m_curTabDlg->getValidHWnd();
   m_curTabDlg->transferData(FALSE);
   m_curTabDlg->hideWindow(FALSE);
}//void AWSettingsDlg::onSelChanged() 



void 
AWSettingsDlg::onTimer(UINT timerID)
{  //dispose of last one, we're going to change the period
   unsigned int delay = INVISIBLE_TIME;
   KillTimer(m_hDlg, m_timerID);
   if (m_buyNowVisible)
   {
      SetDlgItemText(m_hDlg, IDC_VISIT_ALTEREDWORLDS, "");
   }
   else
   {
      SetDlgItemText(m_hDlg, IDC_VISIT_ALTEREDWORLDS, "BUY NOW!");
      delay = VISIBLE_TIME;
   }
   //for flashing BUY NOW!
   m_timerID = SetTimer(m_hDlg, 1, delay, NULL);
   m_buyNowVisible = !m_buyNowVisible;
}//void AWSettingsDlg::onTimer(UINT timerID)


void
AWSettingsDlg::close()
{
   if (m_contentDlg.isDownloading())
   {
      AWCString msg("You're downloading an AlteredWorld! If you\n");
      msg +=        "really want to close this dialog now,you can\n";
      msg +=        "resume the download later.\n\n";
      msg +=        "Please press \'Ok\' to close or \'Cancel\' to continue\nthe download.";
      if (IDOK != ::MessageBox(m_hDlg, msg.c_str(), "WARNING - Download in progress!", MB_OKCANCEL))
      {
         return;
      }
      else
      {
         m_contentDlg.OnCancelDownload();
         Sleep(200);
      }
   }//if (m_contentPage.isDownloading())
   if (m_timerID)
   {
      KillTimer(m_hDlg, m_timerID);
   }
   ::EndDialog(m_hDlg, m_exitCode);
}//void AWSettingsDlg::close()



void
AWSettingsDlg::transferData(AWBoolean fromControls)
{	
	if (fromControls)
	{
      int iSel = TabCtrl_GetCurSel(m_hTab);  
      m_data.setLastTab(iSel);
#ifndef _AW_MINIMAL_CONFIG_GUI
      m_sceneDlg.transferData(fromControls);
      m_displayDlg.transferData(fromControls);
      m_gameDlg.transferData(fromControls);
      m_soundDlg.transferData(fromControls);  
      m_controlsDlg.transferData(fromControls);
#endif
	}//if (fromControls)
	else //toControls
	{
	}//else //toControls
}//void AWSettingsDlg::transferData(AWBoolean fromControls)



LRESULT     
AWSettingsDlg::OnInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam, AWBoolean& bHandled)
{
   m_buyNowVisible = TRUE;
#ifndef _AW_MINIMAL_CONFIG_GUI
   m_data.loadConfig(); //already loaded
   m_data.buildSceneList(); //may as well do this here.
#endif
   m_hDlg = hwnd;

   DWORD    dwDlgBase  = GetDialogBaseUnits(); 
   int      cxMargin   = LOWORD(dwDlgBase) / 4; 
   int      cyMargin   = HIWORD(dwDlgBase) / 8;     
   TCITEM   tie;     
    // Create the tab control - IDC_SAVERCONFIG_TAB    
   InitCommonControls(); 
   m_hTab = GetDlgItem(m_hDlg, IDC_SAVERCONFIG_TAB); 
   // Add a tab for each of the three child dialog boxes. 
   tie.mask = TCIF_TEXT | TCIF_IMAGE;     
   tie.iImage = -1; 
#ifndef _AW_MINIMAL_CONFIG_GUI
   tie.pszText = "Saver";     
   TabCtrl_InsertItem(m_hTab, AW_TAB_SAVER_PAGE, &tie);
  
   tie.pszText = "Display";     
   TabCtrl_InsertItem(m_hTab, AW_TAB_DISPLAY_PAGE, &tie);
 
   tie.pszText = "Sounds";     
   TabCtrl_InsertItem(m_hTab, AW_TAB_SOUND_PAGE, &tie); 

   tie.pszText = "Controls";     
   TabCtrl_InsertItem(m_hTab, AW_TAB_CONTROLS_PAGE, &tie); 
#endif
   tie.pszText = "Content";     
   TabCtrl_InsertItem(m_hTab, AW_TAB_CONTENT_PAGE, &tie);
  
#ifndef _AW_MINIMAL_CONFIG_GUI
#ifndef DISABLE_GAME_PAGE
   tie.pszText = "Game";     
   TabCtrl_InsertItem(m_hTab, AW_TAB_GAME_PAGE, &tie);
#endif
#endif
   TabCtrl_SetCurSel(m_hTab, m_data.getLastTab());

   //Use hardcoded size for Tab from resource file
   //not the most general solution, but it works.
   SetRectEmpty(&m_rcDisplay); 
   m_rcDisplay.left = 7 * LOWORD(dwDlgBase) / 4;
   m_rcDisplay.right = 195 * LOWORD(dwDlgBase) / 4; 
   m_rcDisplay.top = 7 * HIWORD(dwDlgBase) / 8; 
   m_rcDisplay.bottom = 109 * HIWORD(dwDlgBase) / 8;  
   // allow for the tabs on the top
   TabCtrl_AdjustRect(m_hTab, FALSE, &m_rcDisplay);
   
   m_contentDlg.m_parent  = hwnd;
   m_contentDlg.lockAndLoad();
   
#ifndef _AW_MINIMAL_CONFIG_GUI
   m_soundDlg.m_parent    = hwnd;
   m_controlsDlg.m_parent = hwnd;
   m_gameDlg.m_parent     = hwnd;
   m_sceneDlg.m_parent    = hwnd;
   m_displayDlg.m_parent  = hwnd;
   m_soundDlg.lockAndLoad();
   m_gameDlg.lockAndLoad();
   m_controlsDlg.lockAndLoad();
   m_sceneDlg.lockAndLoad();
   m_displayDlg.lockAndLoad();
#endif
   //create and update all windows - why not   
   // Simulate selection of the first item.     
   onSelChanged();
   //load the icon to display in the task bar, etc.
   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
   //for flashing BUY NOW!
   if (!m_data.isRegistered())
   {
      m_timerID = SetTimer(hwnd, 1, VISIBLE_TIME, NULL);
      SetDlgItemText(hwnd, IDC_VISIT_ALTEREDWORLDS, "BUY NOW!");
   }//if (!m_data.isRegistered())
   return TRUE;
}//LRESULT AWSettingsDlg::OnInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam, AWBoolean& bHandled) 



LRESULT     
AWSettingsDlg::OnOK(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   m_exitCode = (UINT)lParam;
   transferData(TRUE); //get data from the controls
#ifndef _AW_MINIMAL_CONFIG_GUI
   m_data.saveConfig();
#endif
   return FALSE;
}//LRESULT AWSettingsDlg::OnOK(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)


 
void
AWSettingsDlg::about()
{  //produce popup menu at middle RHS of 'About' button
   //menu allows users to select between:
   //about box, help & visiting AW online    
   if (!m_data.isRegistered())
   {
      AWWinUtils::about(m_hDlg, IDC_ABOUT_ALTEREDSAVER, m_data.m_help);
   }
   else
   {
      AWWinUtils::alteredDemoAbout(m_hDlg);
   }
}//void AWSettingsDlg::about()