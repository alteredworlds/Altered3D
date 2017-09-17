#include "AWSettingsCommon.h"
#include "res/AlteredSettingsRes.h"
#include "AWNagDlg.h"

#include <regstr.h>

//#define AW_NEWWORLDS_HELP "nwindex.htm"
#define DLG_VANISH_TIME 60000
static HWND testHWND = NULL;

AWBoolean        AWNagDlg::m_noExtraBS = FALSE;

/////////////////////////////////////////////////////////////////////////////
// AWNagDlg

AWBoolean CALLBACK AWNagDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	AWBoolean retVal = FALSE;
	AWNagDlg* dlg = (AWNagDlg*)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) 
    {
		case WM_INITDIALOG:
			dlg = (AWNagDlg*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)dlg );
			dlg->initDialog(hDlg);
         retVal = TRUE;
         break;

		case WM_MOUSEACTIVATE:
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			break;

      case WM_TIMER:
         dlg->onTimer((UINT)wParam);
         break;

      case WM_CLOSE:
         dlg->close();
         break;

		case WM_COMMAND:			
			switch( LOWORD(wParam) ) 
			{
         case IDHELP:
            dlg->help();
            break;
			case IDOK:
            dlg->close();
            break;
         case IDC_BUYNOW:
            dlg->buyNow();
            break;
         case IDC_ABOUT_ALTEREDSAVER:
            dlg->about();
            break;
			}//switch( LOWORD(wParam) ) 
			break;

		default:
			break;
	}//switch ( message )
   return retVal;
}//AWBoolean CALLBACK AWNagDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )



int
AWNagDlg::DoModal()
{
   return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
                              m_parent, (DLGPROC)DialogProc, 
                              (LPARAM)this);
}


HWND
AWNagDlg::DoModeless()
{
   m_hWnd = CreateDialogParam(m_hInst,  // handle to module
                 MAKEINTRESOURCE(IDD),   // dialog box template name
                 m_parent ? m_parent : GetActiveWindow(),      // handle to owner window
                 (DLGPROC)DialogProc,  // dialog box procedure
                 (LPARAM)this);
   ShowWindow(m_hWnd, SW_SHOW);
   return m_hWnd;
}//HWND AWNagDlg::DoModeless()


AWNagDlg::AWNagDlg(AWConfig& cfg, int daysLeft, const AWCString& helpPath, 
                   HINSTANCE hInst, HWND parent) : 
                      m_cfg(cfg),
                      m_hWnd(NULL),   m_parent(parent), 
							 m_hInst(hInst), m_helpPath(helpPath),
                      m_buyNowVisible(TRUE), 
                      m_goAwayTimerID(0), m_timerID(0),
                      m_daysLeft(daysLeft)
{
   string::size_type pos = m_helpPath.rfind("\\");
   if ((string::npos != pos) && (++pos < m_helpPath.length()))
   {
      m_helpPath.remove(pos);
   }
   m_helpPath += "index.htm";
}


AWBoolean
AWNagDlg::realSaverNT()
{
   AWBoolean retVal=FALSE;
	HDESK hDesktop;      
	// try to open the desktop that the screen saver runs on. This
	// desktop is created on the fly by winlogon, so it only exists
	// when a screen saver is invoked.
	hDesktop = OpenDesktop("Screen-Saver", 0, FALSE, MAXIMUM_ALLOWED);
	if(hDesktop == NULL)
   {  // if the call fails due to access denied, the screen saver
      // is running because the specified desktop exists - we just
      // don't have any access.     
      retVal = GetLastError() == ERROR_ACCESS_DENIED;
   } 
	else
   {
      HDESK ourDesktop = GetThreadDesktop(GetCurrentThreadId());
      if (ourDesktop && (0==SwitchDesktop(ourDesktop)))
      {
         retVal = TRUE;
      }
      CloseDesktop(hDesktop);	
   }  
	return retVal;
}//AWBoolean AWNagDlg::realSaverNT()



void 
AWNagDlg::initDialog(HWND hwnd)
{
   m_hWnd = hwnd;
   //set the days left stuff...
   char temp[10];
   sprintf(temp, "%2d", m_daysLeft);
   SetDlgItemText(m_hWnd, IDC_DAYSLEFTTEXT, temp);
   //load the icon to display in the task bar, etc.
   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
   //load the background bitmap
   //m_backgroundBitmap = LoadImage(m_hInst, MAKEINTRESOURCE(IDB_BITMAP1),  
   //                                IMAGE_BITMAP, 0,  0, LR_DEFAULTCOLOR);
   //for freaky NT shit
   UINT dlgVisibleTime = DLG_VANISH_TIME;
   if (realSaverNT())
   {
      EnableWindow(GetDlgItem(hwnd, IDC_BUYNOW), FALSE);
      EnableWindow(GetDlgItem(hwnd, IDC_ABOUT_ALTEREDSAVER), FALSE);
      dlgVisibleTime = 3000; //three seconds
   }
   //for flashing BUY NOW!
   m_timerID = SetTimer(hwnd, 1, VISIBLE_TIME, NULL);
   m_goAwayTimerID = SetTimer(hwnd, 2, dlgVisibleTime, NULL);
   if (!m_noExtraBS)
   {
      //make sure we're the topmost window
      ShowWindow(m_hWnd, SW_MINIMIZE);
      ShowWindow(m_hWnd, SW_RESTORE);
      SetWindowPos(m_hWnd, HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
      SetFocus(m_hWnd);
   }
}//void AWNagDlg::initDialog(HWND hwnd)



void
AWNagDlg::help()
{
    AWWinUtils::showHTMLHelp(m_helpPath);
}//void AWNagDlg::help()



void
AWNagDlg::buyNow()
{
   AWWinUtils::buyNowOrRegister(m_cfg, m_hInst, m_hWnd, IDC_BUYNOW);
}


void 
AWNagDlg::onTimer(UINT timerID)
{  //dispose of last one, we're going to change the period
   if (m_goAwayTimerID == timerID)
   {
      PostMessage(m_hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
   }
   else if (m_timerID == timerID)
   {
      unsigned int delay = INVISIBLE_TIME;
      KillTimer(m_hWnd, m_timerID);
      if (m_buyNowVisible)
      {
         SetDlgItemText(m_hWnd, IDC_BUYNOW, "");
      }
      else
      {
         SetDlgItemText(m_hWnd, IDC_BUYNOW, "&BUY NOW!");
         delay = VISIBLE_TIME;
      }
      //for flashing BUY NOW!
      m_timerID = SetTimer(m_hWnd, 1, delay, NULL);
      m_buyNowVisible = !m_buyNowVisible;
   }
}//void AWNagDlg::onTimer(UINT timerID)



void
AWNagDlg::close()
{
   if (m_timerID)
   {
      KillTimer(m_hWnd, m_timerID);
      m_timerID = 0;
   }
   if (m_goAwayTimerID)
   {
      KillTimer(m_hWnd, m_goAwayTimerID);
      m_goAwayTimerID = 0;
   }
   EndDialog(m_hWnd, 0);
}//void AWNagDlg::close()



void
AWNagDlg::about()
{
   //produce popup menu at middle RHS of 'About' button
   //menu allows users to select between about box & visiting AW online
   RECT button;   
   const int menuAWAbout          = 100;
   const int menuAWOnTheWeb       = 101;
   const int menuAlteredSaverHelp = 102;
   GetWindowRect(GetDlgItem(m_hWnd, IDC_ABOUT_ALTEREDSAVER), &button);
   HMENU hMenu = CreatePopupMenu();
   if (hMenu)
   {
      AppendMenu(hMenu, MF_STRING, menuAWAbout,          "About Altered&Saver v3...");
      AppendMenu(hMenu, MF_STRING, menuAlteredSaverHelp, "AlteredSaver v3 &Help...");
      AppendMenu(hMenu, MF_SEPARATOR, 0, "");
      AppendMenu(hMenu, MF_STRING, menuAWOnTheWeb,       "&www.AlteredWorlds.com...");
      RECT dummy;
      int test = TrackPopupMenu(hMenu,  TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,     
                           button.left+(button.right-button.left)/2, button.top, 0, m_hWnd, &dummy);
      DestroyMenu(hMenu);
      if (menuAWAbout == test)
      {
         AWWinUtils::alteredDemoAbout(m_hWnd);
      }//if (menuAWAbout == test)
      else if (menuAWOnTheWeb == test)
      {  //now launch their browser and take them to our site! 
         AWWinUtils::visitAlteredWorlds("AlteredSaver3.htm");
      }//else if (menuAWOnTheWeb == test)
      else if (menuAlteredSaverHelp == test)
      {
         AWWinUtils::showHTMLHelp(m_helpPath);
      }
   }//if (hMenu)
}//void AWNagDlg::about()