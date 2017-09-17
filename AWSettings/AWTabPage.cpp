#include "AW3DTools/AWToolsCommon.h"
#include "AWTabPage.h"
#include "AWSettingsDlg.h"


AWBoolean CALLBACK 
AWTabPageDlg::DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   //Typically, the dialog box procedure should return TRUE 
   //if it processed the message, and FALSE if it did not. 
   //If the dialog box procedure returns FALSE, the dialog 
   //manager performs the default dialog operation for the message.
   AWBoolean retVal = FALSE;
   AWTabPageDlg* dlg = (AWTabPageDlg*)GetWindowLong(hwnd, GWL_USERDATA);
   if ( !dlg && msg != WM_INITDIALOG ) return FALSE;
   switch (msg)
   { 
   case WM_INITDIALOG:
      dlg = (AWTabPageDlg*)lParam;
      SetWindowLong(hwnd, GWL_USERDATA, (LONG)dlg);
      if (dlg)
      {
         retVal = dlg->OnInitDialog(hwnd, msg, wParam, lParam);
      }
      break;
      
   case WM_DESTROY:
      retVal = dlg->OnClose(hwnd, msg, wParam, lParam);
      break;

   case WM_COMMAND:
      retVal = dlg->OnDialogCommand(hwnd, msg, wParam, lParam);
      break;  

   default:
      retVal = dlg->OnDialogMessage(hwnd, msg, wParam, lParam);
      //pass any other messages on to the dialog class for 
      //further processing (optional)
    }//switch (msg)
    return retVal;
}//AWBoolean CALLBACK AWTabPageDlg::DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



HWND
AWTabPageDlg::DoModeless()
{
   lockAndLoad(); //won't do it again if already done
   m_hDlg = CreateDialogIndirectParam(m_hInst,  // handle to module
                 m_res,   // dialog box template name
                 m_parent ? m_parent : GetActiveWindow(),      // handle to owner window
                 (DLGPROC)DialogProc,  // dialog box procedure
                 (LPARAM)this);
   if (NULL == m_hDlg)
   {
      DWORD errNum = GetLastError();
   }
   ShowWindow(m_hDlg, SW_SHOW);
   return m_hDlg;
}//HWND AWTabPageDlg::DoModeless(HINSTANCE hinst)



AWTabPageDlg::AWTabPageDlg(HINSTANCE hInst, HWND parent)
            :	m_parent(parent),
               m_hDlg(NULL),
               m_res(NULL),
               m_hInst(hInst)
{	
}



void
AWTabPageDlg::hideWindow(AWBoolean yes)
{
   ShowWindow(m_hDlg, yes?SW_HIDE:SW_SHOW);
}//void AWTabPageDlg::hideWindow(AWBoolean yes)



HWND           
AWTabPageDlg::getValidHWnd()
{  //if already created, return m_hDlg 
   //otherwise create the modeless dialog (Tab Page)
   return m_hDlg ? m_hDlg :  DoModeless();
}//HWND AWTabPageDlg::getValidHWnd()


               
AWBoolean     
AWTabPageDlg::OnClose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    transferData(TRUE); //get data from the controls
    return FALSE;
}//LRESULT AWTabPageDlg::OnClose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



void        
AWTabPageDlg::lockAndLoad()
{  //loads and locks a dialog template resource. 
   if (!m_res)
   {
      HRSRC   hrsrc = FindResource(m_hInst, MAKEINTRESOURCE(m_IDD), RT_DIALOG); 
      HGLOBAL hglb  = LoadResource(m_hInst, hrsrc); 
      m_res = (DLGTEMPLATE*)LockResource(hglb); 
   }//if (m_res)
}//void AWTabPageDlg::lockAndLoad()



AWBoolean     
AWTabPageDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    m_hDlg = hwnd;
    AWSettingsDlg* pSettingsDlg = (AWSettingsDlg*)GetWindowLong(GetParent(m_hDlg), GWL_USERDATA); 
    if (pSettingsDlg)
    {
       SetWindowPos(m_hDlg, HWND_TOP, pSettingsDlg->m_rcDisplay.left, 
                    pSettingsDlg->m_rcDisplay.top, 0, 0, SWP_NOSIZE);
    }
    transferData(FALSE); //transfer data to the controls
    return TRUE;
}//LRESULT AWTabPageDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
