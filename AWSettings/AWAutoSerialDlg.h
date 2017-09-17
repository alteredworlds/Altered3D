#ifndef _AWAUTOSERIALDLG_H
#define _AWAUTOSERIALDLG_H

#include "res/AlteredSettingsRes.h"
#include "AWSerialize/AWVerify.h"
/////////////////////////////////////////////////////////////////////////////
// AlteredAutoSerialDlg
class AlteredAutoSerialDlg
{
public:
	AlteredAutoSerialDlg(HINSTANCE hInst, HWND parent=NULL);
	~AlteredAutoSerialDlg();

	HWND	      m_hWnd, m_parent;
   HINSTANCE   m_hInst;


	enum {   IDD = IDD_AWAUTOSERIAL };

	LRESULT  OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  OnNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT  OnBack(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT  OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   void     OnTimer(int timerID);	
   void     OnManualCheck();
   void     OnAutoCheck();
	int	   DoModal();

	static BOOL CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

   char     m_userName[USER_INFO_FIELD_LEN];
   char     m_email[USER_INFO_FIELD_LEN];
   char     m_serial[ASCII_SERIAL_LEN];
   char     m_delivery[SERIAL_FIELD_LEN];
   bool     m_isAuto;
   bool     m_flasherVisible;

protected:
   bool     getSerialNum();
};//class AlteredAutoSerialDlg


#endif //sentinel _AWAUTOSERIALDLG_H
