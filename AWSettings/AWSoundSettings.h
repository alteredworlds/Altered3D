#ifndef __AW_SOUND_SETTINGS_H
#define __AW_SOUND_SETTINGS_H


#include "AWSaverGame/AWConfig.h"
#include "AWTabPage.h"


class AWSoundSettingsDlg : public AWTabPageDlg
{ 
public:
   AWSoundSettingsDlg(AWConfig& cfg, HINSTANCE hInst=NULL, HWND parent=NULL);

   AWCString         getDisplayName(const AWCString& fullName, int maxLen);

   virtual void		transferData(AWBoolean fromControls);
   //virtual AWBoolean OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   void		         fillCDROMList();  
   void		         pickWave();
   void              close();
   void              noSoundsCheck();
   void              playWaveCheck();
   void              launchHelperCheck();
   void              playAudioCDCheck();
   void              gameMusicCheck();

   AWConfig&         m_data;
};//class AWSoundSettingsDlg : public AWTabPageDlg

#endif //sentinel __AW_SOUND_SETTINGS_H