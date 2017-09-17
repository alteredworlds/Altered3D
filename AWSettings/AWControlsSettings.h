#ifndef __AW_CONTROLS_SETTINGS_H
#define __AW_CONTROLS_SETTINGS_H

#include "AWSettingsCommon.h"
#include "AWTabPage.h"


class AWControlsSettingsDlg : public AWTabPageDlg
{ 
public:
   AWControlsSettingsDlg(AWConfigWin& cfg, 
                         HINSTANCE hInst=NULL, HWND parent=NULL);


   virtual void	transferData(AWBoolean fromControls);
   void           pickScreenshotFolder();


   virtual AWBoolean OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   virtual AWBoolean OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                          LPARAM lParam);

//MEMBER DATA
   enum { IDD = IDD_AWCONTROLS_TAB};

   AWConfigWin&   m_data;
   static int     m_joystick;
};
#endif //sentinel __AW_CONTROLS_SETTINGS_H