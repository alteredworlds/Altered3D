#ifndef _AW_SAVER_GAME_H
#define _AW_SAVER_GAME_H

#include <AW3DTools/AWToolsCommon.h>
#include "AWSpaceGnats.h"
#include "AWUserInput.h"
#include "AWConfigWin.h"
#include "AWSndProviderWin.h"


#define AW_STAT_CONTINUE          0
#define AW_STAT_TERMINATE_DONE    5
#define AW_STAT_TERMINATE_ERROR   7


class AWSaverGame
{
public:
   AWSaverGame(AWConfig& cfg, HINSTANCE hinst = NULL);
   virtual           ~AWSaverGame();

   void              resetData();

   AWBoolean         registerWindowClass();
   AWBoolean         unRegisterWindowClass();
   AWBoolean         createWindow();

   AWBoolean         showConfig()   {return m_cfg.isConfig();}

   int               execute(int nCmdShow);

   AWBoolean         loadSceneFromResource(WORD resID);
   void              setCmdLine(const AWCString& cmdLine);
  
   int               runMessageLoop();
   void              closeSaverWindow();

   LRESULT           OnCreate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnTimer(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnActivate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnSetFocus(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnKillFocus(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnSetCursor(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnMouseButtonDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnMouseButtonUp(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnMouseMove(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnSysCommand(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnKeyDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnClose(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnDestroy(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnPowerActivate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnNcPaint(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnPaint(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnCaptureChanged(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnSize(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnMove(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnJoy1Move(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnJoy1ButtonDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnJoy1ButtonUp(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnWomDone(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
   LRESULT           OnSwitchScenes(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

   static LRESULT CALLBACK WindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

   static AWSaverGame* m_this;

//member data
public:
   HINSTANCE            m_hInst;
   HWND		            m_hWnd;
   HWND                 m_parent;
   HDC			         m_ghDC;
   HGLRC		            m_ghRC;
   HCURSOR              m_oldCur;             

   AWSpaceGnats         m_gnats;
   AWConfig&            m_cfg;
   AWSoundProviderWin   m_snd;

   POINT		            m_initCursorPos;
   UINT		            m_idTimer;   
   DWORD                m_initTime;
   AWBoolean            m_isActive;
   AWBoolean		      m_reallyClose; // for NT, so we know if a WM_CLOSE came from us or it.
   AWBoolean            m_monitorPowerDown;
   AWBoolean            m_launchAsSaver; //rubbish for ATI horrble shutdown hack
   AWBoolean            m_hasFocus;
   AWBoolean            m_firstTime;
   AWBoolean            m_displayModeSwitched;

   int                  m_status;

   static char*         m_szAppName;

protected:
   void                 initSound();
   void                 shutdownSound();
   void                 adjustClientRect(RECT& screenPosn);

   int		            m_captionHeight;
   int                  m_sizeBarX;
   int                  m_sizeBarY;
}; //class AWSaverGame


#endif //sentinel