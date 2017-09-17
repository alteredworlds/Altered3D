// AlteredUserInfoDlg.cpp : Implementation of AlteredUserInfoDlg
#include  <windows.h>
#include "AWUserInfoDlg.h" 
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

/////////////////////////////////////////////////////////////////////////////
// AlteredUserInfoDlg

BOOL CALLBACK AlteredUserInfoDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	BOOL retVal = FALSE;
	AlteredUserInfoDlg* dlg = (AlteredUserInfoDlg*)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) 
   {
		case WM_INITDIALOG:
			dlg = (AlteredUserInfoDlg*)lParam;
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
         //::MessageBeep(MB_OK);
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
			}//switch( LOWORD(wParam) ) 

		default:
			break;
	}//switch ( message ) 
   return retVal;
}//BOOL CALLBACK AlteredUserInfoDlg::SceneMgrDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )




int
AlteredUserInfoDlg::DoModal()
{
	return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
		m_parent ? m_parent : GetActiveWindow(), (DLGPROC)DialogProc, (LPARAM)this); 

}

AlteredUserInfoDlg::AlteredUserInfoDlg(HINSTANCE hInst, HWND parent) : 
                    m_hWnd(0), m_hInst(hInst), m_parent(parent)
{
   m_userName[0]=0;
   m_email[0]=0;
   m_delivery[0]=0;
}


AlteredUserInfoDlg::~AlteredUserInfoDlg()
{
}



LRESULT AlteredUserInfoDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   SetDlgItemText(m_hWnd, IDC_NAMESTR,     m_userName);
   SetDlgItemText(m_hWnd, IDC_DELIVERYSTR, m_delivery);
   SetDlgItemText(m_hWnd, IDC_EMAILSTR,    m_email);

   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	return 1;  // Let the system set the focus
}



LRESULT  
AlteredUserInfoDlg::OnNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   GetDlgItemText(m_hWnd, IDC_NAMESTR,     m_userName, sizeof(m_userName));
   m_userName[sizeof(m_userName)-1]=0;
   GetDlgItemText(m_hWnd, IDC_DELIVERYSTR, m_delivery, sizeof(m_delivery));
   m_delivery[sizeof(m_delivery)-1]=0;
   GetDlgItemText(m_hWnd, IDC_EMAILSTR,    m_email,    sizeof(m_email));
   m_email[sizeof(m_email)-1]=0;
   if (strlen(m_userName) && strlen(m_email) && 
       strchr(m_email, '.') && strchr(m_email, '@') && 
       strlen(m_delivery))
   {
      EndDialog(m_hWnd, wID);
   }
   else
   {
      ::MessageBox(m_hWnd, "Please make sure you have entered your name, email address and delivery number!\n\nPlease check you have all cASeS correct...\n\nContact AlteredWorlds for help - support@alteredworlds.com", "INVALID USER!", MB_OK);
   }
	return 0;
}//LRESULT AlteredUserInfoDlg::OnPlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)



LRESULT 
AlteredUserInfoDlg::OnBack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(m_hWnd, wID);
	return 0;
}//LRESULT AlteredUserInfoDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)



LRESULT 
AlteredUserInfoDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   EndDialog(m_hWnd, wID);
	return 0;
}//LRESULT AlteredUserInfoDlg::OnShowConfig(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)