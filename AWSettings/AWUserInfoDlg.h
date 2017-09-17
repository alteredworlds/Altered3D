#ifndef _AWUSERINFODLG_H_
#define _AWUSERINFODLG_H_

#include "res/AlteredSettingsRes.h"
#include "AWSerialize/AWVerify.h"
/////////////////////////////////////////////////////////////////////////////
// AlteredUserInfoDlg
class AlteredUserInfoDlg
{
public:
	AlteredUserInfoDlg(HINSTANCE hInst, HWND parent=NULL);
	~AlteredUserInfoDlg();

	HWND	      m_hWnd;
   HWND        m_parent;
   HINSTANCE   m_hInst;


	enum  {     IDD = IDD_AWUSERINFO };

	LRESULT     OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT     OnNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT     OnBack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT     OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	int	      DoModal();

	static BOOL CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

   char        m_userName[USER_INFO_FIELD_LEN];
   char        m_email[USER_INFO_FIELD_LEN];
   char        m_delivery[SERIAL_FIELD_LEN];
};//class AlteredUserInfoDlg


#endif //sentinel _AWUSERINFODLG_H_
