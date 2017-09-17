#ifndef _AlteredGetSerialDlg_H_
#define _AlteredGetSerialDlg_H_

#include "res/AlteredSettingsRes.h"
#include "AWSerialize/AWVerify.h"
/////////////////////////////////////////////////////////////////////////////
// AlteredGetSerialDlg
class AlteredGetSerialDlg
{
public:
	AlteredGetSerialDlg(HINSTANCE hInst, HWND parent=NULL);
	~AlteredGetSerialDlg();

	static HWND	   m_hWnd;
   HWND           m_parent;
   HINSTANCE      m_hInst;


	enum { IDD = IDD_AWGETSERIAL };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	int	         DoModal();
   bool           NetThreadStillRunning();

	static BOOL CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

   static char           m_userName[USER_INFO_FIELD_LEN];
   static char           m_useName[USER_INFO_FIELD_LEN];
   static char           m_email[USER_INFO_FIELD_LEN];
   static char           m_serial[ASCII_SERIAL_LEN];
   static char           m_delivery[SERIAL_FIELD_LEN];
   static char*          m_pProxy;
   static int            m_proxyPort;

protected:
   static bool           getProxyInfo();
   static void           getHTTP(char* lpServerName, char* lpFileName);
   static  unsigned int  __stdcall getSerialNum(void* );
   static unsigned long  m_netThread;
   static unsigned int   m_threadID;
};//class AlteredGetSerialDlg


#endif //sentinel _AlteredGetSerialDlg_H_
