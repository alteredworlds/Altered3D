#ifndef _AW_CONTENT_PAGE_H
#define _AW_CONTENT_PAGE_H

#include "AWSettingsCommon.h"
#include "AWTabPage.h"


#define WM_AWDONE           WM_USER + 1
#define ID_AWREDIRECT       1
#define ID_AWSUCCESS        0
#define ID_AWCANCEL         2
#define ID_AWBAD_CONNECT    3
#define ID_AWINVALID_USER   4

class AWDownloadMgr; //forward definition

class AWContentPageDlg : public AWTabPageDlg
{ 
public:
   AWContentPageDlg(AWConfigWin& cfg, 
                    HINSTANCE hInst=NULL, HWND parent=NULL);

   void		            updateControls(int reason=ID_AWSUCCESS);
   void                 showDownloadControls(AWBoolean show);

   virtual AWBoolean    OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean    OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean    OnClose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   virtual AWBoolean    OnDialogMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

   void                 OnCancelDownload();
   void                 OnGetContentList();
   void                 OnGetScene();

   AWBoolean            isDownloading();

   AWCString            getLocalSceneDir();
   void                 transferDone(int reason);
   AWBoolean            sceneNotLoaded(const AWCString& token);
   AWBoolean            extractFileList();
   AWBoolean            versionOK(const AWCString& s);
 
   AWConfigWin&         m_data;
   AWBoolean            m_gotContentList;
   vector<AWCString>    m_newSceneList;
   int                  m_selSceneIndex;

protected:
   AWCString            m_serverDir;
   AWCString            m_listFileName;
   static AWDownloadMgr m_downloadMgr;
};

#endif //sentinel _AW_CONTENT_PAGE_H