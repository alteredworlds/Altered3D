#ifndef __AW_SAVER_SETTINGS_H
#define __AW_SAVER_SETTINGS_H

#include "AWSaverGame/AWConfig.h"
#include "AWContentPage.h"
#ifndef _AW_MINIMAL_CONFIG_GUI
#include "AWSceneSettings.h"
#include "AWDisplaySettings.h"
#include "AWSoundSettings.h"
#include "AWControlsSettings.h"
#include "AWGamePage.h"
#endif

#define SND_OFF				0
#define SND_LOOPED_WAVE		1
#define SND_LAUNCH_HELPER	2
#define SND_PLAY_CD			3

#define IDC_UPDATESAVER -72


class AWSettingsDlg
{ 
public:
   AWSettingsDlg(AWConfigWin& data, HINSTANCE hInst = NULL);

   void                 setParent(HWND parent) {m_parent=parent;}
   HWND                 getParent()            {return m_parent;}

   void                 onSelChanged();
   void                 onTimer(UINT timerID);

	void		            transferData(AWBoolean fromControls);
	
   void                 about();
   void                 visitAlteredWorlds();
   void                 close();
   void                 getUpdate();

   int		            DoModal();
   static AWBoolean CALLBACK DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

   LRESULT              OnInitDialog(HWND hwnd, WPARAM wParam, LPARAM lParam, AWBoolean& bHandled);
   LRESULT              OnOK(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

   void                 onEraseBkGnd(HDC hdc);

//MEMBER DATA
   enum { IDD = IDD_AWSAVERCONFIG};

   HINSTANCE            m_hInst;
   HWND		            m_hDlg, m_parent;
   UINT                 m_timerID;
   UINT                 m_exitCode;
   AWConfigWin&         m_data;
   HWND                 m_hTab;    // tab control 
   RECT                 m_rcDisplay;  // display rectangle for the tab control 
   AWBoolean            m_buyNowVisible;
   HANDLE               m_backgroundBitmap;
   AWTabPageDlg*        m_curTabDlg;
   
//child dialogs
   AWContentPageDlg      m_contentDlg;
#ifndef _AW_MINIMAL_CONFIG_GUI
   AWSceneSettingsDlg    m_sceneDlg;
   AWDisplaySettingsDlg  m_displayDlg;
   AWControlsSettingsDlg m_controlsDlg;
   AWGamePageDlg         m_gameDlg;
   AWSoundSettingsDlg    m_soundDlg;  
#endif
};//class AWSettingsDlg
#endif //sentinel __AW_SAVER_SETTINGS_H