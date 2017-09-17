#ifndef _AW_GAME_PAGE_H
#define _AW_GAME_PAGE_H

#include "AWSettingsCommon.h"
#include "AWTabPage.h"


class AWGamePageDlg : public AWTabPageDlg
{ 
public:
   AWGamePageDlg(AWConfigWin& cfg, HINSTANCE hInst=NULL, HWND parent=NULL);

   virtual void	   transferData(AWBoolean fromControls);

   virtual AWBoolean OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                          LPARAM lParam);

   void              OnPlayNow();
   AWConfigWin&      m_data;
};
#endif //sentinel _AW_GAME_PAGE_H