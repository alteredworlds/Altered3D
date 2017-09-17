#ifndef __AW_DISPLAY_SETTINGS_H
#define __AW_DISPLAY_SETTINGS_H
 

#include "AWSaverGame/AWConfig.h"
#include "AWTabPage.h"


class AWDisplaySettingsDlg : public AWTabPageDlg
{ 
public:
   AWDisplaySettingsDlg(AWConfig& cfg, HINSTANCE hInst=NULL, HWND parent=NULL);

   virtual void      transferData(AWBoolean fromControls);
   virtual AWBoolean OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                          LPARAM lParam);

   void		         fillDisplayModeList();  
   void		         pickScene();
   void              configure3D();
   void              currentSettingsCheck();
   void              switchSettingsCheck();
   void              setSceneIndex(const AWCString& name);

//MEMBER DATA
   AWConfig&      m_data;
};//class AWDisplaySettingsDlg : public AWTabPageDlg

#endif //sentinel __AW_DISPLAY_SETTINGS_H