// AlteredAutoSerialDlg.cpp : Implementation of AlteredAutoSerialDlg
#include  <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "AWAutoSerialDlg.h" 
#include "AWGetSerialDlg.h"

/////////////////////////////////////////////////////////////////////////////
// AlteredAutoSerialDlg

BOOL CALLBACK AlteredAutoSerialDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	BOOL retVal = FALSE;
	AlteredAutoSerialDlg* dlg = (AlteredAutoSerialDlg*)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) 
   {
		case WM_INITDIALOG:
			dlg = (AlteredAutoSerialDlg*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)dlg );
			dlg->m_hWnd = hDlg;
			dlg->OnInitDialog(WM_INITDIALOG, wParam, lParam, retVal);
			retVal = TRUE;			

		case WM_DESTROY:
			break;

		case WM_MOUSEACTIVATE:
			break;

      case WM_ACTIVATE:
         UpdateWindow(hDlg);
         break;

      case WM_TIMER:
         dlg->OnTimer((int)wParam);
         break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			break;

		case WM_COMMAND:			
			switch( LOWORD(wParam) ) 
			{
			case IDC_AWCANCEL:
				dlg->OnCancel(WM_COMMAND, IDC_AWCANCEL, hDlg, retVal);
				break;

			case IDC_AWNEXT:
            dlg->OnNext(WM_COMMAND, IDC_AWNEXT, hDlg, retVal);
				break;

         case IDC_AWBACK:
				dlg->OnBack(WM_COMMAND, IDC_AWBACK, hDlg, retVal);
				break;

         case IDC_AUTOCHK:
            dlg->OnAutoCheck();
            break;

         case IDC_MANUALCHK:
            dlg->OnManualCheck();
            break;
			}//switch( LOWORD(wParam) ) 

		default:
			break;
	}//switch ( message ) 
   return retVal;
}//BOOL CALLBACK AlteredAutoSerialDlg::SceneMgrDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )



int
AlteredAutoSerialDlg::DoModal()
{
	return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
		m_parent ? m_parent : GetActiveWindow(), (DLGPROC)DialogProc, (LPARAM)this); 

}



AlteredAutoSerialDlg::AlteredAutoSerialDlg(HINSTANCE hInst, HWND parent) : 
                    m_hWnd(0), m_hInst(hInst), m_parent(parent),
                    m_isAuto(true), m_flasherVisible(false)
{
   m_userName[0]=0;
   m_email[0]=0;
   m_serial[0]=0;
   m_delivery[0]=0;
}



AlteredAutoSerialDlg::~AlteredAutoSerialDlg()
{
}



LRESULT AlteredAutoSerialDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

   SetTimer(m_hWnd, 1, 1000, NULL);//for flashing 'make sure you're connected'
   CheckDlgButton(m_hWnd, IDC_AUTOCHK,   m_isAuto ? BST_CHECKED   : BST_UNCHECKED);
   CheckDlgButton(m_hWnd, IDC_MANUALCHK, m_isAuto ? BST_UNCHECKED : BST_CHECKED);
   ShowWindow(GetDlgItem(m_hWnd, IDC_MUSTCONNECT), m_isAuto ? SW_SHOW: SW_HIDE);
	return 1;  // Let the system set the focus
}



void     
AlteredAutoSerialDlg::OnTimer(int timerID)
{
   m_flasherVisible = !m_flasherVisible;
   ShowWindow(GetDlgItem(m_hWnd, IDC_MUSTCONNECT), 
              (m_isAuto && m_flasherVisible) ? SW_SHOW : SW_HIDE);
}//void AlteredAutoSerialDlg::OnTimer(int timerID)



bool
AlteredAutoSerialDlg::getSerialNum()
{
   bool retVal = false; 
   if (IsDlgButtonChecked(m_hWnd, IDC_MANUALCHK)==BST_CHECKED)
   {
      if (GetDlgItemText(m_hWnd, IDC_SERIALTXT, m_serial, sizeof(m_serial)))
      {
         m_serial[sizeof(m_serial)-1]=0;
         retVal = validate(m_userName, m_email, m_delivery, m_serial);
      }
   }//if (IsDlgButtonChecked(m_hWnd, IDC_MANUALCHK)==BST_CHECKED)
   else
   {
      AlteredGetSerialDlg threadDlg(m_hInst, m_hWnd);
      strcpy(threadDlg.m_userName, m_userName);
      strcpy(threadDlg.m_email,    m_email);
      strcpy(threadDlg.m_delivery, m_delivery);
      retVal= (IDCANCEL != threadDlg.DoModal());
      if (retVal)
      {
         strcpy(m_serial, threadDlg.m_serial);
      }
   }
#ifndef _AW_SHAREWARE_REGISTRATION
   #define AWUSERKEY                   "Software\\AlteredWorlds\\AlteredSaver v3.0!\\3.05.000"

   if (retVal)
   {  //now we need to save this value
      HKEY  skey; 
      DWORD disp;
      LONG  res=RegCreateKeyEx(HKEY_LOCAL_MACHINE, AWUSERKEY, 0, NULL,
                         REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &skey, &disp);
      if (res==ERROR_SUCCESS)
      {
         RegSetValueEx(skey, "user", 0, REG_SZ, (CONST BYTE*)m_userName, strlen(m_userName));
         RegSetValueEx(skey, "email", 0, REG_SZ, (CONST BYTE*)m_email, strlen(m_email));
         RegSetValueEx(skey, "delivery", 0, REG_SZ, (CONST BYTE*)m_delivery, strlen(m_delivery));
         RegSetValueEx(skey, "serial", 0, REG_SZ, (CONST BYTE*)m_serial, strlen(m_serial));

         RegCloseKey(skey);
      }
   }//if (retVal)
#endif //_AW_SHAREWARE_REGISTRATION
   return retVal;
}//bool AlteredAutoSerialDlg::getSerialNum()



LRESULT  
AlteredAutoSerialDlg::OnNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   if (strlen(m_userName) && strlen(m_email) && 
       strchr(m_email, '.') && strchr(m_email, '@')  && getSerialNum())
   {
      EndDialog(m_hWnd, wID);
   }
   else
   {
      ::MessageBox(m_hWnd, "Please make sure you have entered your name, email address and delivery number!\n\nPlease check you have all cASeS correct...\n\nContact AlteredWorlds for help - support@alteredworlds.com", "INVALID USER!", MB_OK);
   }
	return 0;
}//LRESULT AlteredAutoSerialDlg::OnPlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)



LRESULT 
AlteredAutoSerialDlg::OnBack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(m_hWnd, wID);
	return 0;
}//LRESULT AlteredAutoSerialDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)



LRESULT 
AlteredAutoSerialDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   EndDialog(m_hWnd, wID);
	return 0;
}//LRESULT AlteredAutoSerialDlg::OnShowConfig(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)


void
AlteredAutoSerialDlg::OnAutoCheck()
{  //if we're setting this, clear the other one
	if (IsDlgButtonChecked(m_hWnd, IDC_AUTOCHK)==BST_CHECKED)
	{
      m_isAuto = true;
		CheckDlgButton(m_hWnd, IDC_MANUALCHK, BST_UNCHECKED);
		EnableWindow(GetDlgItem(m_hWnd, IDC_SERIALTXT), FALSE);
      m_flasherVisible = true;
      ShowWindow(GetDlgItem(m_hWnd, IDC_MUSTCONNECT), SW_SHOW);
	}
}//void AlteredAutoSerialDlg::OnAutoCheck()


void
AlteredAutoSerialDlg::OnManualCheck()
{  //if we're setting this, clear the other one
	if (IsDlgButtonChecked(m_hWnd, IDC_MANUALCHK)==BST_CHECKED)
	{
      m_isAuto = false;
		CheckDlgButton(m_hWnd, IDC_AUTOCHK, BST_UNCHECKED);
		EnableWindow(GetDlgItem(m_hWnd, IDC_SERIALTXT), TRUE);
      m_flasherVisible = false;
      ShowWindow(GetDlgItem(m_hWnd, IDC_MUSTCONNECT), SW_HIDE);
	}
}//void AlteredAutoSerialDlg::OnManualCheck()