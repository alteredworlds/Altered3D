#ifndef __AW_SCENE_SETTINGS_H
#define __AW_SCENE_SETTINGS_H

#include "AWSaverGame/AWConfig.h"
#include "AWTabPage.h"

class AWSceneSettingsDlg : public AWTabPageDlg
{ 
public:
   AWSceneSettingsDlg(AWConfig& cfg,  HINSTANCE hInst=NULL, HWND parent=NULL);

   virtual void      transferData(AWBoolean fromControls);
   virtual AWBoolean OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, 
                                                          LPARAM lParam);

   void		         pickScene();
   void              getNewScenes();
   void              setSceneIndex(const AWCString& name);

   AWConfig&         m_data;
};//class AWSceneSettingsDlg

#endif //sentinel __AW_SCENE_SETTINGS_H