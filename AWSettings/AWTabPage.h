#ifndef _AW_TAB_DIALOG_PAGE_H
#define _AW_TAB_DIALOG_PAGE_H

#include "AWSettingsCommon.h"


class AWTabPageDlg
{ 
public:
   AWTabPageDlg(HINSTANCE hInst=NULL, HWND parent=NULL);

   void              setParent(HWND parent) {m_parent=parent;}
   HWND              getParent()            {return m_parent;}

   void              hideWindow(AWBoolean yes=TRUE);

   void              lockAndLoad();
   HWND              getValidHWnd();
   virtual void      transferData(AWBoolean fromControls){;}

   HWND              DoModeless();

   virtual AWBoolean OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean OnDialogMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   virtual AWBoolean OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean OnClose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   static AWBoolean CALLBACK DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

//MEMBER DATA
   unsigned int      m_IDD;

   HINSTANCE         m_hInst;
   HWND		         m_hDlg;
   HWND              m_parent;
   DLGTEMPLATE*      m_res; 
   AWBoolean         m_hasInit;
};//class AWTabPageDlg



inline AWBoolean 
AWTabPageDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{  
   return FALSE; //default processing will be used
}


inline AWBoolean 
AWTabPageDlg::OnDialogMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   return FALSE; //default processing will be used
}


#endif //sentinel _AW_TAB_DIALOG_PAGE_H