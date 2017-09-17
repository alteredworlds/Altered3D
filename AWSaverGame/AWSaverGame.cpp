#include "AWSaverGame.h"
#include <AW3DTools/AWGLRenderer.h>
#include <AW3DTools/AWRocket.h>
#include <AW3DTools/AWImageProcessor.h>
#include "AWSndProviderWin.h"

#include "AWWinUtils.h"
#include "AWSettings/res/AlteredSettingsRes.h"


#include <pbt.h>

#define WM_AWSWITCHSCENE WM_USER + 22

char* AWSaverGame::m_szAppName = "AlteredWorlds";


//static const double SAVER_CHECKSUM     =      962626710.408973;
#define MOUSE_THRESHOLD          50
#define SAVER_IGNORE_TIME        1.5f

AWSaverGame* AWSaverGame::m_this = NULL;


LRESULT CALLBACK 
AWSaverGame::WindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ 
   LRESULT lRet = 0;
   if (!m_this) 
   {
      AWTRACE("\nInvalid THIS pointer in WindowProc\n");
      return 0;
   }
   AWTRACE4("WindowProc: %5x : %-23s  wParam %10x  lParam %10x\n", msg, AWWinUtils::getMsgName((int)msg).data(), (int)wParam, (int)lParam);
   switch (msg)
   { 
   case WM_CREATE:
      lRet = m_this->OnCreate(hwnd, msg, wParam, lParam);
      break;

   case WM_NCPAINT:
      lRet = m_this->OnNcPaint(hwnd, msg, wParam, lParam);
      break;

   case WM_PAINT:
      lRet = m_this->OnPaint(hwnd, msg, wParam, lParam);
      break;

   case WM_AWSWITCHSCENE:
      lRet = m_this->OnSwitchScenes(hwnd, msg, wParam, lParam);
      break;

   case WM_SETCURSOR:
      lRet = m_this->OnSetCursor(hwnd, msg, wParam, lParam);
      break;

   case WM_CAPTURECHANGED:
      //we're losing mouse capture
      lRet = m_this->OnCaptureChanged(hwnd, msg, wParam, lParam);
      break;

   case WM_SIZE:
      lRet = m_this->OnSize(hwnd, msg, wParam, lParam);
      break;

   case WM_ACTIVATE: 
   //case WM_ACTIVATEAPP: 
   //case WM_NCACTIVATE:
      lRet = m_this->OnActivate(hwnd, msg, wParam, lParam);
      break;

   case WM_MOVE:
      lRet = m_this->OnMove(hwnd, msg, wParam, lParam);
      break;

   case WM_SYSCOMMAND:
      lRet = m_this->OnSysCommand(hwnd, msg, wParam, lParam);
      break;

   case WM_POWERBROADCAST:
      lRet = m_this->OnPowerActivate(hwnd, msg, wParam, lParam);
      break;

   case WM_SETFOCUS:
      lRet = m_this->OnSetFocus(hwnd, msg, wParam, lParam);
      break;

   case WM_KILLFOCUS:
      lRet = m_this->OnKillFocus(hwnd, msg, wParam, lParam);
      break;

      //case WM_WINDOWPOSCHANGED:
      //The WM_SIZE and WM_MOVE messages are not sent if an application 
      //handles the WM_WINDOWPOSCHANGED message without calling DefWindowProc. 
      //It is more efficient to perform any move or size change processing 
      //during the WM_WINDOWPOSCHANGED message without calling DefWindowProc. 
      //zero retval indicates we handled this message
      //case WM_MOVE:
      //case WM_SIZE:
      //  break;
   case WM_ERASEBKGND:
      //An application should return nonzero in response to WM_ERASEBKGND 
      //if it processes the message and erases the background; *this indicates 
      //that no further erasing is required*. If the application returns zero, 
      //the window will remain marked for erasing. (Typically, this indicates 
      //that the fErase member of the PAINTSTRUCT structure will be TRUE.) 
      lRet = 1;
      break;

   case WM_TIMER:
      lRet = m_this->OnTimer(hwnd, msg, wParam, lParam);
      break;

   case MM_JOY1MOVE:
      lRet = m_this->OnJoy1Move(hwnd, msg, wParam, lParam);
      break;

   case MM_JOY1BUTTONDOWN:
      lRet = m_this->OnJoy1ButtonDown(hwnd, msg, wParam, lParam);
      break;

   case MM_JOY1BUTTONUP:
      lRet = m_this->OnJoy1ButtonUp(hwnd, msg, wParam, lParam);
      break;

   case WM_LBUTTONDOWN: 
   case WM_MBUTTONDOWN: 
   case WM_RBUTTONDOWN: 
      lRet = m_this->OnMouseButtonDown(hwnd, msg, wParam, lParam);
      break;

   case WM_LBUTTONUP:
   case WM_MBUTTONUP: 
   case WM_RBUTTONUP: 
      lRet = m_this->OnMouseButtonUp(hwnd, msg, wParam, lParam);
      break;

   case WM_MOUSEMOVE:
      lRet = m_this->OnMouseMove(hwnd, msg, wParam, lParam);
      break;

   case WM_KEYDOWN:
      lRet = m_this->OnKeyDown(hwnd, msg, wParam, lParam); 
      break;

   case WM_CLOSE:
      lRet = m_this->OnClose(hwnd, msg, wParam, lParam);
      break;

   case WM_DESTROY:
      lRet = m_this->OnDestroy(hwnd, msg, wParam, lParam);
      break;

   case MM_WOM_DONE:
      lRet = m_this->OnWomDone(hwnd, msg, wParam, lParam);
      break;
      
   default:;
      lRet = DefWindowProc(hwnd, msg, wParam, lParam);
   }//switch (msg)
   return lRet;
}//LRESULT CALLBACK AWSaverGame::WindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



void
AWSaverGame::resetData()
{
   m_hWnd = NULL;
   m_ghDC = NULL;
   m_ghRC = NULL; 
   m_reallyClose = FALSE;
   m_isActive = FALSE;
   m_oldCur = NULL;
   m_monitorPowerDown = FALSE;
   m_launchAsSaver =TRUE;
   m_captionHeight =0;
   m_sizeBarX = 0;
   m_sizeBarY = 0;
   m_hasFocus = FALSE;
   m_firstTime= TRUE; 
   m_displayModeSwitched = FALSE;
}//void AWSaverGame:;resetData()



AWSaverGame::AWSaverGame(AWConfig& cfg, HINSTANCE hinst) :
        m_cfg(cfg), m_hInst(hinst), m_gnats(cfg), m_idTimer(0) 
{
    m_this = this;
    resetData();
}



AWSaverGame::~AWSaverGame()
{
    m_this = NULL;
}


AWBoolean 
AWSaverGame::registerWindowClass()
{
   AWBoolean retVal = TRUE;
   WNDCLASS wc;
   // Register the frame class
   wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = (WNDPROC)WindowProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = m_hInst;
   wc.hIcon         = LoadIcon (m_hInst, MAKEINTRESOURCE(IDI_AWICON));
   wc.hCursor       = m_cfg.isPreview() ? LoadCursor(NULL,IDC_ARROW):NULL;
   
   if (!m_cfg.isGame())
   {
      m_szAppName = "ScrClass";
   }
   if (m_cfg.isGame())
   {
      wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
      wc.lpszMenuName  = m_szAppName;
   }//if (m_cfg.isGame())
   else
   {
      wc.hbrBackground = NULL;
      wc.lpszMenuName  = NULL;
   }
   wc.lpszClassName = m_szAppName;
   if (!RegisterClass(&wc))
   {
      retVal = FALSE;
   }
   return retVal;
}//AWBoolean AWSaverGame::registerWindowClass()


AWBoolean AWSaverGame::unRegisterWindowClass()
{
   return (AWBoolean)UnregisterClass(m_szAppName, m_hInst);
}//AWBoolean AWSaverGame::unRegisterWindowClass()


AWBoolean       
AWSaverGame::createWindow()
{
   m_hWnd = NULL;
   registerWindowClass();
   if (m_cfg.isPreview())
   { 
      m_parent = (HWND)m_cfg.getWinID();
      RECT rc; 
      GetWindowRect(m_parent, &rc);
      int cx=rc.right-rc.left, cy=rc.bottom-rc.top;
      m_hWnd = CreateWindowEx(0,"ScrClass","AlteredSaverPreview",
                             WS_CHILD|WS_VISIBLE,0,0,cx,cy, m_parent, 
                             NULL, m_hInst, NULL);
   }//if (m_cfg.isPreview())
   else if (m_cfg.isSaver())
   { 
      DWORD exstyle   = WS_EX_TOPMOST;// | WS_EX_TOOLWINDOW;
      int viewWidth   = m_cfg.m_specifyMode ? m_cfg.m_width : GetSystemMetrics(SM_CXSCREEN);
      int viewHeight  = m_cfg.m_specifyMode ? m_cfg.m_height : GetSystemMetrics(SM_CYSCREEN);
      m_hWnd          = CreateWindowEx(exstyle,"ScrClass",
                                     "AlteredSaverWindow",
                                     WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,
                                     0, 0, viewWidth, viewHeight, 
                                     NULL, NULL, m_hInst, NULL);
   }//else if (m_cfg.isSaver())
   else 
   {  // GAME - Create the frame - WS_POPUP window style
      int captionBarHeightInPixels = 0;
      DWORD stylE = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      int viewWidth   = m_cfg.m_specifyMode ? m_cfg.m_width : GetSystemMetrics(SM_CXSCREEN);
      int viewHeight  = m_cfg.m_specifyMode ? m_cfg.m_height : GetSystemMetrics(SM_CYSCREEN);
      if (!m_cfg.isFullScreen())
      {  //in a window... allow for frame, set correct window styles.
         stylE |= WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
         captionBarHeightInPixels = GetSystemMetrics(SM_CYCAPTION);
      }
      
      char* winTitle = "AlteredSaver v3!";
      m_hWnd = CreateWindow(m_szAppName, winTitle, stylE, 0, 0, viewWidth, 
                            viewHeight + captionBarHeightInPixels,
                            NULL, NULL, m_hInst, NULL);
   }//else 
   return NULL != m_hWnd;
}//AWBoolean AWSaverGame::createWindow()



LRESULT           
AWSaverGame::OnSwitchScenes(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{  //LPARAM 1 IFF switching to next section in composite scene
   //  in which case wParam is number of seconds remaining before 
   //  next switch due to saver timeout
   //LPARAM 0 IFF switching to new scene due to saver timeout
   LRESULT retVal = 0;
   int     switchTime = (int)wParam;
   AWTRACE2("OnSwitchScenes() with wParam %i cmdLineSceneName %s\n", (int)wParam, m_cfg.m_cmdLineSceneName.c_str());
 
   if (!(int)lParam)
   {  //switching to new scene due to saver timeout
      AWTRACE("  lParam is 0 - moving to next scene on disk\n");
      m_cfg.getNextScene(m_cfg.m_cmdLineSceneName);
   }
   //shut down GL context for last scene, cleaning up all data
   m_gnats.m_renderer.shutdown();
   //load the new scene
   m_gnats.loadScene(m_cfg.m_cmdLineSceneName);
   m_gnats.resetEvents();
   m_ghRC = (HGLRC)GLRenderer::destroyContext(m_ghRC);   
   //re-init 3D Engine with new scene
   if (GLRenderer::setupPixelFormat(m_ghDC, m_cfg.m_bpp))
   {
      m_ghRC = (HGLRC)GLRenderer::createContextAndSelect(m_ghDC);
      RECT rect;
      GetClientRect(hwnd, &rect);
      m_gnats.init(m_cfg.m_width, m_cfg.m_height);
   }//if (GLRenderer::setupPixelFormat(m_ghDC, m_cfg.m_bpp))
   if (switchTime)
   {  //internalSwitch gives number of seconds before switch to new SCENE      
      m_gnats.setSceneSwitchTime(switchTime>0 ? switchTime : 0);
	   AWTRACE1("  after switching internal section num, setSceneSwitchTime(%i)\n", (int)m_gnats.getSceneSwitchTime());
   }//if (internalSwitch)
   if ((int)lParam)
   {  //we MAY have switched to bonus level, in which case
      //extra processing will be required. check and see.
      m_gnats.detectBonusLevel();
   }//if ((int)lParam)
   //before continuing with Windows message loop processing...
   return retVal;
}//LRESULT AWSaverGame::OnSwitchScenes(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
 


void 
AWSaverGame::adjustClientRect(RECT& screenPosn)
{
    screenPosn.top      += m_captionHeight;
    screenPosn.left     += m_sizeBarX;
    screenPosn.right    -= m_sizeBarX;
    screenPosn.bottom   -= m_sizeBarY;
}//void AWSaverGame::adjustClientRect(RECT& screenPosn)



LRESULT
AWSaverGame::OnCreate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   RECT rect;
   m_hWnd = hwnd;
   m_ghDC = GetDC(hwnd);

   GetCursorPos(&m_initCursorPos); 
   m_initTime = GetTickCount();    
   
   if (GLRenderer::setupPixelFormat(m_ghDC, m_cfg.m_bpp))
   {
      m_ghRC = (HGLRC)GLRenderer::createContextAndSelect(m_ghDC);
      
      GetClientRect(hwnd, &rect);
      m_gnats.m_renderer.initialize(rect.right, rect.bottom);

      GetWindowRect(m_hWnd, &rect);
      if (!m_cfg.isFullScreen())
      {
         m_captionHeight = GetSystemMetrics(SM_CYMENU);
         m_sizeBarX      = GetSystemMetrics(SM_CXSIZEFRAME);
         m_sizeBarY      = GetSystemMetrics(SM_CYSIZEFRAME); 
      }
      adjustClientRect(rect);
      m_gnats.m_userInput.setClientArea(rect);
      if (m_cfg.isGame())
      {
         m_gnats.m_userInput.setCursorHome();
      }
   }//if (GLRenderer::setupPixelFormat(m_ghDC, m_gnats.m_bpp))
   
   
   if (m_gnats.getUseJoystick() && 
            (JOYERR_NOERROR == joySetCapture(m_hWnd, JOYSTICKID1, 30, TRUE)))
   {
      joySetThreshold(JOYSTICKID1, 999);
      m_gnats.m_userInput.setUseJoystick();
   }//game.getUseJoystick() &&
   else if (m_gnats.getUseMouse())
   {
      m_gnats.m_userInput.setUseMouse();
   }
      
   if (m_cfg.isPreview()) 
   {   
      m_idTimer = SetTimer(hwnd,0,1,NULL);
   }//if (m_cfg.isPreview()) 
   return 0;
}//LRESULT AWSaverGame::OnCreate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



void 
AWSaverGame::initSound()
{
   if (m_cfg.isPreview()||m_cfg.isConfig()) return;
   if (m_cfg.getPlayGameMusic())
   {
      m_snd.init();
      if (m_cfg.getPlayGameMusic())
      {
         m_snd.loadWave("sounds\\gnats.wav",   gameLoopSndID);
         m_snd.play(gameLoopSndID, -1);
      }//if (game.m_playMusic)
      if (m_cfg.getPlayEffects()) 
      {
         m_snd.loadWave("sounds\\Bgun.wav",    gunSndID);
         m_snd.loadWave("sounds\\fail.wav",    failLevelSndID);
         m_snd.loadWave("sounds\\winner.wav",  doneLevelSndID);
         m_snd.loadWave("sounds\\explode.wav", explodeSndID);
      }//if (m_cfg.m_playEffects)
   }//if (m_cfg.getPlayGameMusic())

   if (m_cfg.getPlayCD())
   {
      AWWinUtils::playAudioCD(m_cfg.m_cdrom);
   }//if (m_cfg.getPlayCD())
   else if (m_cfg.getPlayFile() && m_cfg.m_waveFile.length() && AWFile::Exists(m_cfg.m_waveFile))
   {
      //is it a WAV file?
      if (m_cfg.m_waveFile.contains(".wav"))
      {
         ::PlaySound(m_cfg.m_waveFile.c_str(), NULL, SND_ASYNC | SND_LOOP);
      }
      else
      {  //launch helper application
         AWWinUtils::launchHelper(m_cfg.m_waveFile);
      }
   }//else if (m_cfg.getPlayFile() && waveFile.length() && AWFile::Exists(waveFile))
}//void AWSaverGame::initSound()



void
AWSaverGame::shutdownSound()
{ 
   if (m_cfg.isPreview()||m_cfg.isConfig()) return;
   if (m_cfg.getPlayCD()) 
   { //start playing sound if desired
      AWWinUtils::stopAudioCD(m_cfg.m_cdrom);
   }//if (m_cfg.getPlayCD()) 
   else 
   {
      ::PlaySound(NULL, NULL, SND_PURGE);
   }
}//void AWSaverGame::shutdownSound()



LRESULT         
AWSaverGame::OnNcPaint(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT lRet = 0;
   if (!m_cfg.isFullScreen())
   {
      lRet = DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return lRet;
}//LRESULT AWSaverGame::OnNcPaint(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT         
AWSaverGame::OnPaint(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT lRet=0;
   PAINTSTRUCT ps;
   BeginPaint(hwnd, &ps);
   if (!m_cfg.isFullScreen() && m_gnats.m_renderer.getPaused()) 
   {
      m_gnats.m_renderer.displayFrame();
   }
   EndPaint(hwnd, &ps);
   return lRet;
}//LRESULT AWSaverGame::OnPaint(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnActivate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   AWTRACE1("OnActivate %d\n", (int)msg);
   if (m_cfg.isSaver())
   {
      if (WM_NCACTIVATE == msg)
      {
         //When the fActive parameter is FALSE, an application should return TRUE 
         //to indicate that Windows should proceed with the default processing, 
         //or it should return FALSE to prevent the title bar or icon from being 
         //deactivated. When fActive is TRUE, the return value is ignored.
         ;//return 1;
      }
      if (LOWORD(wParam)==WA_INACTIVE)
      {
         //doesn't seem to need any action?
         //MessageBeep(MB_ICONEXCLAMATION);
      }
   }//if (m_cfg.isSaver())
   else if (m_isActive && (LOWORD(wParam)==WA_INACTIVE)) 
   {//we should de-activate
      m_isActive = FALSE;
      if (m_oldCur) SetCursor(m_oldCur);
      if (!m_gnats.m_renderer.getPaused()) 
      {
         m_gnats.m_renderer.togglePause();
      }
      if (m_cfg.isFullScreen() && m_cfg.isGame()) 
      {
         ShowWindow(m_hWnd, SW_MINIMIZE);
         m_gnats.m_renderer.makeCurrent(NULL, NULL);           
         if (m_displayModeSwitched)
         {
            AWWinUtils::revertToUserMode();
            m_displayModeSwitched = FALSE;
         }//if (m_displayModeSwitched)           
      }//if (cfg.isFullScreen()) 
   }//if (isActive && (LOWORD(wParam)==WA_INACTIVE)) 
   else if (!m_isActive)
   {//time to motivate...
      m_isActive = TRUE;
      if (m_cfg.isFullScreen() && m_cfg.isGame()) 
      {
         if (!m_displayModeSwitched)
         {
            m_displayModeSwitched = AWWinUtils::switchToMode(m_cfg.isFullScreen(), m_cfg.m_bpp, 
                   m_cfg.m_width, m_cfg.m_height);
         }           
         m_gnats.m_renderer.makeCurrent(m_ghDC, m_ghRC);
         ShowWindow(m_hWnd, SW_RESTORE);
      }//if (!m_displayModeSwitched && cfg.isFullScreen()) 
      if (m_gnats.m_renderer.getPaused()) 
      {
         m_gnats.m_renderer.togglePause();
      }//if (m_gnats.m_renderer.getPaused()) 
      if (!m_cfg.isPreview())
      {
         m_oldCur = SetCursor(NULL);
      }
   }//else if (!isActive)
   return 1;
}//LRESULT AWSaverGame::OnActivate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnSize(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT retVal = 0;
   m_cfg.m_width  = LOWORD(lParam);
   m_cfg.m_height = HIWORD(lParam);
   m_gnats.m_renderer.resizeGL(m_cfg.m_width, m_cfg.m_height);
   if (hwnd)
   {
      RECT rect;
      GetWindowRect(hwnd, &rect);
      adjustClientRect(rect);
      m_gnats.m_userInput.setClientArea(rect);
   }//if (hwnd)
   return retVal;
}//LRESULT AWSaverGame::OnSize(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   if (hwnd)
   {
      RECT rect;
      GetWindowRect(hwnd, &rect);
      adjustClientRect(rect);
      m_gnats.m_userInput.setClientArea(rect);
   }//if (hWnd)
   return 1;
}//LRESULT AWSaverGame::OnMove(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnSetFocus(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   if (m_cfg.isGame())
   {
      m_hasFocus = TRUE;
      m_gnats.m_userInput.setFocus();
      m_snd.resume();
      if (!m_snd.isActive(gameLoopSndID))
      {
         m_snd.play(gameLoopSndID, -1);
      }
      if (m_gnats.m_renderer.getPaused()) 
      {
         m_gnats.m_renderer.togglePause();
      }
   }//if (m_cfg.isGame())
   return 0;
}//LRESULT AWSaverGame::OnSetFocus(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



LRESULT 
AWSaverGame::OnKillFocus(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   if (m_cfg.isGame())
   {
      m_gnats.m_userInput.killFocus();
      m_hasFocus=FALSE;
      m_snd.pause();
      if (!m_gnats.m_renderer.getPaused()) 
      {
         m_gnats.m_renderer.togglePause();           
      }//if (!m_gnats.m_renderer.getPaused()) 
   }//if (m_cfg.isGame())
   return 0;
}//LRESULT AWSaverGame::OnKillFocus(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



LRESULT 
AWSaverGame::OnTimer(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   if (m_monitorPowerDown && m_cfg.isSaver())
   {
      KillTimer(m_hWnd, m_idTimer);
      m_idTimer = 0;
      m_monitorPowerDown = FALSE;
   }//if (m_monitorPowerDown && m_cfg.isSaver())
   return 0;
}//LRESULT AWSaverGame::OnTimer(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT         
AWSaverGame::OnPowerActivate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT retVal = 1;
   AWTRACE2("Received WM_POWERBROADCAST with wParam %d at time %f\n", (int)wParam, (double)m_gnats.m_curTime);
   if ((PBT_APMPOWERSTATUSCHANGE != (DWORD)wParam) && 
       !m_gnats.m_renderer.getPaused()) 
   {
      AWTRACE(" PAUSE SAVER...\n");
      m_gnats.m_renderer.togglePause();
   }
   switch ((DWORD)wParam)
   {
   case PBT_APMQUERYSUSPEND:
      AWTRACE("  Granting permission to suspend\n");
      break;
   case PBT_APMSUSPEND:
      AWTRACE("  Suspend warning - close open files - saver paused already\n");
   #ifdef __AWTRACE
      //fclose( stream );
      //stream = NULL;
   #endif //__AWTRACE
      ::PlaySound(NULL, NULL, SND_PURGE);
      break;
   case PBT_APMPOWERSTATUSCHANGE:
      {//check to see what's up here
         SYSTEM_POWER_STATUS powerStatus;
         if (GetSystemPowerStatus(&powerStatus))
         {
#ifdef __AWTRACE
            fprintf(stderr, "  SystemPowerStatus Line Stat %i  BatFlag %i  LifePercent %i  Reserved1 %i  LifeTime %i  FullTime %i", 
            (int) powerStatus.ACLineStatus,
            (int)  powerStatus.BatteryFlag,
            (int)  powerStatus.BatteryLifePercent,
            (int)  powerStatus.Reserved1,
            (int)  powerStatus.BatteryLifeTime,
            (int)  powerStatus.BatteryFullLifeTime);  
            fflush(stderr);
#endif
         }//if (GetSystemPowerStatus(&powerStatus))
         //retVal = DefWindowProc(hwnd, msg, wParam, lParam);
      }
      break;

   case PBT_APMOEMEVENT:
      AWTRACE("OEM Power event - default processing - paused already\n");
      retVal = DefWindowProc(hwnd, msg, wParam, lParam);
      break;

   case PBT_APMRESUMESUSPEND:
   case PBT_APMRESUMECRITICAL:
      //pause remains in effect - now close saver.
#ifdef __AWTRACE
    //stream = freopen( "AlteredSaver.log", "at", stderr );   
    //if( stream == NULL )
    //{
    //   fprintf( stdout, "error on freopen\n" );   
    //}
    //else   
    //{
    //   fprintf( stream, "\nAlteredsaver log file reopened\n" );  
    //}
#endif
#ifdef _AW_SPACEGNATS_DEMO
      AWNagDlg::m_noExtraBS = TRUE;
#endif //_AW_SPACEGNATS_DEMO
      closeSaverWindow();
      break;
   default:
      retVal = DefWindowProc(hwnd, msg, wParam, lParam);
   }//switch
   return retVal;
}//LRESULT AWSaverGame::OnPowerActivate(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnSetCursor(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT lRet = 1;
   if (m_cfg.isSaver())
   {
      SetCursor(NULL);
   }
   else if (!m_cfg.isFullScreen())
   {
      lRet = DefWindowProc(m_hWnd, msg, wParam, lParam);
   }
   return lRet;
}//LRESULT AWSaverGame::OnSetCursor(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnCaptureChanged(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{  //we're losing mouse capture
   return 0;
}//LRESULT AWSaverGame::OnCaptureChanged(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnJoy1Move(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   WORD joyposX = LOWORD(lParam);
   WORD joyposY = HIWORD(lParam);
   m_gnats.m_userInput.joyMoveEvent(joyposX, joyposY);
   return 1;
}//LRESULT AWSaverGame::OnJoy1Move(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnJoy1ButtonDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   if (!((int)JOY_BUTTON1CHG == (int)wParam))
   {
      m_gnats.m_userInput.joyBtnEvent((int)wParam);
   }
   m_gnats.m_userInput.joyBtnEvent((int)wParam, FALSE);
   return 1;
}//LRESULT AWSaverGame::OnJoy1ButtonDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnJoy1ButtonUp(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   m_gnats.m_userInput.joyBtnEvent((int)wParam, FALSE);
   return 1;
}//LRESULT AWSaverGame::OnJoy1ButtonUp(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnMouseButtonDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = 0;
   AWTRACE("OnMouseButtonDown\n" );
   if (m_cfg.isSaver()) 
   {
      if (m_monitorPowerDown && (0 == m_idTimer))
      {  //this timer will go off after it's safe to quit
         //windows has settled down again
         if (m_gnats.m_renderer.getPaused()) 
         {
            m_gnats.m_renderer.togglePause();
         }
         m_idTimer = SetTimer(hwnd, 1, 2500, NULL);
         retVal = 0;         
      }//if (m_monitorPowerDown && (0 == m_idTimer))
      else
      {
         closeSaverWindow();
      }
   }//if (m_cfg.isSaver()) 
   else if (m_cfg.isGame())
   {
      m_gnats.m_userInput.mouseBtnEvent((int)wParam);
   }
   return retVal;
}//LRESULT AWSaverGame::OnMouseButtonDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnMouseButtonUp(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = 0;
   AWTRACE("OnMouseButtonUp\n" );
   if (m_cfg.isGame())
   {
      m_gnats.m_userInput.mouseBtnEvent((int)wParam, FALSE);
   }//if (m_cfg.isGame())
   return retVal;
}//LRESULT AWSaverGame::OnMouseButtonUp(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnMouseMove(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   LRESULT retVal = 0;
   AWTRACE("OnMouseMove\n");
   if (m_cfg.isGame())
   {
      WORD posX = LOWORD(lParam);
      WORD posY = HIWORD(lParam);
      if (m_firstTime)
      {
         m_gnats.m_userInput.setLastPos(posX, posY);
         m_firstTime = FALSE;
      }
      else m_gnats.m_userInput.mouseMoveEvent(posX, posY);
   }
   else if (m_monitorPowerDown && (m_cfg.isSaver()) && (0 == m_idTimer))
   {  //this timer will go off after it's safe to quit
      //windows has settled down again
      if (m_gnats.m_renderer.getPaused()) 
      {
         m_gnats.m_renderer.togglePause();
      }
      m_idTimer = SetTimer(hwnd, 1, 2500, NULL);
      AWTRACE("  Setting delay and posting SC_MONITORPOWER, -1\n");
      PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1); 
   }//if (m_monitorPowerDown && (m_cfg.isSaver()) && (0 == m_idTimer))
   else if (m_cfg.isSaver())
   { 
      POINT pt; 
      GetCursorPos(&pt);
      int dx = pt.x - m_initCursorPos.x; 
      if (dx < 0) 
         dx = -dx; 
      int dy = pt.y - m_initCursorPos.y; 
      if (dy < 0) 
         dy = -dy;
      if ((dx > MOUSE_THRESHOLD || dy > MOUSE_THRESHOLD) && 
        (m_gnats.m_curTime > SAVER_IGNORE_TIME))
      { 
         closeSaverWindow();
      }
   }//if (m_cfg.isSaver())
   return retVal;
}//LRESULT AWSaverGame::OnMouseMove(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnSysCommand(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   AWTRACE("WM_SYSCOMMAND received\n");
   LRESULT retVal = 1;
   int     cmd    = wParam & 0xFFF0;
   if (SC_SCREENSAVE==cmd)
   {  //WM_SYSCOMMAND: gobbling up a SC_SCREENSAVE to stop a new saver from running
      AWTRACE(" SC_SCREENSAVE - return 0 to indicate we've handled message: saver won't be re-launched\n");
      retVal = 0;
   }//if (SC_SCREENSAVE == cmd)
   else if (m_cfg.isGame())
   {
      AWTRACE(" in Game mode - just call DefWindowProc & pass back its return value\n");
      retVal = DefWindowProc (m_hWnd, msg, wParam, lParam);
   }//if (m_cfg.isGame())
   else
   {    
      if (m_cfg.isSaver() && (SC_MONITORPOWER==cmd))
      {  //lParam == 1 => LOW POWER   lParam == 2 => SHUTDOWN
         //in either case make sure we tell system it's already taken care of
         AWTRACE1("  Monitor Power message Processed , lParam %d\n", (int)lParam);     
         m_monitorPowerDown = TRUE;
         if (!m_gnats.m_renderer.getPaused()) 
         {
            m_gnats.m_renderer.togglePause();
            ::PlaySound(NULL, NULL, SND_PURGE);
         }
      }//if (m_cfg.isSaver() && (SC_MONITORPOWER==cmd))
      if (m_cfg.isSaver() && (SC_CLOSE==cmd))
      {  //WM_SYSCOMMAND: gobbling up a SC_CLOSE;
         AWTRACE("  SC_SCREENSAVE|SC_CLOSE ignored\n");
         retVal = 0;
      }//if (m_cfg.isSaver() && (SC_CLOSE==cmd))
      else
      {
         AWTRACE("  call DefWindowProc & pass back its return value\n");
         retVal = DefWindowProc(hwnd, msg, wParam, lParam);
      }
   }//else
   return retVal;
}//LRESULT AWSaverGame::OnSysCommand(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnWomDone(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   if (!m_gnats.m_renderer.getPaused())
   {
      if (m_snd.isActive(gameLoopSndID))
      {//active, but paused - so resume
         m_snd.resume(gameLoopSndID);   
      }
      else
      {//inactive, so need to issue a new play command
         m_snd.play(gameLoopSndID, -1);
      }
   }//if (!m_gnats.m_renderer.getPaused())
   return 1;
}//LRESULT AWSaverGame::OnWomDone(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



LRESULT 
AWSaverGame::OnKeyDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   AWTRACE1("WM_KEYDOWN received %d\n", (int)wParam);
   if (m_monitorPowerDown && m_cfg.isSaver() && (0 == m_idTimer))
   {  //this timer will go off after it's safe to quit
      //windows has settled down again
      if (m_gnats.m_renderer.getPaused()) 
      {
         m_gnats.m_renderer.togglePause();
      }
      m_idTimer = SetTimer(hwnd, 1, 2500, NULL);
      AWTRACE("  Setting mode change delay timer\n");
      return 0;
   }//if (m_monitorPowerDown)
   switch (wParam) 
   {
   case 27: //ESC
      if (m_cfg.isGame())
      {
         m_gnats.recordLevel();
      }//if (m_cfg.isGame())
      closeSaverWindow();
      break;
   case 112://f1 - back thru cam list
      m_gnats.backCamera();
      break;
   case 113://f2 - forward thru cam list
      m_gnats.forwardCamera();
      break;
   case 114://f3 - screen capture (single frame)
      m_gnats.captureFrame(m_cfg.m_snapshots);
      break;
   case 115: //F4 screen capture, frame sequence toggle
      m_gnats.toggleMultiFrameCapture(m_cfg.m_snapshots);
      break;
   case 116://F5
      m_gnats.m_renderer.setShowStatistics(!m_gnats.m_renderer.getShowStatistics());
      break;
   case 17: //CTRL
   case 32: case 38: case 37: case 39: case 40: case 'A': case 'Z':
      break;

   case 'V': //DEBUGGING CODE FOR NVIDIA TEST VERSION
      //state is initially set to 
      //YES, TRANSFORM REFLECTION TEXTURE MATRIX so as to
      //rotate eye-space reflection vectors generated by 
      //GL_REFLECTION_MAP_EXT  back into world space
      //m_gnats.m_renderer.toggleTransformReflections();
      break;

   case 'N':
      m_gnats.m_renderer.toggleDrawNormals();
      break;

   case 'M':
      m_gnats.m_renderer.togglePolygonMode();
      break;

   case 'T':
      m_gnats.m_renderer.toggleShowTextures();
      break;
        
   default:
      DefWindowProc(hwnd, msg, wParam, lParam);
      if (m_cfg.isSaver() && !m_monitorPowerDown)
      {
         closeSaverWindow();
      }
      break;
   }//switch (wParam) 
   return 0;
}//LRESULT AWSaverGame::OnKeyDown(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)




LRESULT 
AWSaverGame::OnClose(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   AWTRACE("OnClose\n");
    LRESULT retVal = m_cfg.isPreview();
    if (m_cfg.isSaver()) retVal=FALSE;
    if (!m_cfg.isSaver() || (m_cfg.isSaver() && m_reallyClose))
    { 
        retVal = TRUE; 
    }//if (m_cfg.isSaver() && m_reallyClose)
    if (retVal && m_hWnd)
    {   //next two ShowWindow calls necessary to prevent
        //the occasional Ghost task bar entry on Win98/TNT
       if (m_launchAsSaver && m_cfg.isSaver())
       {
        ShowWindow(m_hWnd, SW_HIDE);
        ShowWindow(m_hWnd, SW_MINIMIZE);
       }
        if (m_idTimer!=0)
        {
            KillTimer(hwnd, m_idTimer); 
        }
        m_idTimer=0;
        m_gnats.m_renderer.shutdown();
        m_ghRC = (HGLRC)GLRenderer::destroyContext(m_ghRC);
        if (m_ghDC)
        {
          ReleaseDC(m_hWnd, m_ghDC);
        }
        m_ghDC = NULL;             
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;       
    }//if (retVal && m_hWnd)
    return retVal;
}//LRESULT AWSaverGame::OnClose(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)




LRESULT AWSaverGame::OnDestroy(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   AWTRACE("On Destroy - PostQuitMessage(0)\n");
   PostQuitMessage(0);
   return 0;
}//LRESULT AWSaverGame::OnDestroy(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)



void 
AWSaverGame::closeSaverWindow()
{
   AWTRACE("closeSaverWindow - Posting WM_CLOSE\n");
   m_reallyClose = TRUE; 
   PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}//void AWSaverGame::closeSaverWindow()

 

AWBoolean
AWSaverGame::loadSceneFromResource(WORD resID)
{  //load from binary resource
   AWBoolean retVal = FALSE;
   HRSRC  hrsrc = FindResource(m_hInst, MAKEINTRESOURCE(resID), 
                                     MAKEINTRESOURCE(300));
   if (hrsrc==NULL) 
   {
      return FALSE;
   }
   DWORD  size  = SizeofResource(m_hInst, hrsrc);
   LPVOID rdata = LockResource(LoadResource(m_hInst, hrsrc));

   AWFile memFile((char*)rdata, size);
   try
   {
      m_gnats.loadScene(memFile);
      retVal = TRUE;
   }
   catch (...)
   {
      MessageBeep(MB_ICONEXCLAMATION);
   }
   return retVal;
}//AWBoolean AWSaverGame::loadSceneFromResource(WORD resID)


int 
AWSaverGame::runMessageLoop()
{
   m_status = AW_STAT_TERMINATE_DONE;
   AWBoolean cont = TRUE;
   MSG msg;
   m_gnats.m_userInput.start();
   // Animation loop 
   while (cont) 
   {  // Process all pending messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
      {
         if (GetMessage(&msg, NULL, 0, 0)) 
         {
            AWTRACE4("GameLoop  : %5x : %-23s  wParam %10x  lParam %10x\n", msg.message, 
                        AWWinUtils::getMsgName((int)msg.message).data(), 
                        (int)msg.wParam, (int)msg.lParam);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }//if (GetMessage(&msg, NULL, 0, 0)) 
         else 
         {
            AWTRACE("WM_QUIT - Ending Message Loop");
            cont = FALSE;
         }
      }//while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
      //now process the game engine
      try
      {
         if (m_gnats.getSwitchToNextScene())
         {
            m_gnats.setSwitchToNextScene(FALSE);
            PostMessage(m_hWnd, WM_AWSWITCHSCENE, (WPARAM)0, (LPARAM)0);
         }//if (m_gnats.getSwitchToNextScene())
         else if (m_gnats.getSwitchToNextSection())
         {
            m_gnats.setSwitchToNextSection(FALSE);
            PostMessage(m_hWnd, WM_AWSWITCHSCENE, (WPARAM)(int)m_gnats.getSceneSwitchTime(), (LPARAM)1);
         }
         else if (cont && !m_gnats.m_renderer.getPaused())
         {
            m_gnats.display();       
            if (m_gnats.getFail())
            {  
               m_snd.pause(gameLoopSndID);
               m_snd.stop(gunSndID);
               m_snd.play(failLevelSndID, 0, m_hWnd);
            }//if (m_gnats.m_renderer.getLevelFailed())
            else if (m_gnats.getWin())
            {
               m_snd.pause(gameLoopSndID);
               m_snd.stop(gunSndID);
               m_snd.play(doneLevelSndID, 0, m_hWnd);
            }//else if (m_gnats.m_renderer.getSwitchLevel())
            else if (m_gnats.justFired() && m_gnats.getUserActive())
            {
               m_snd.play(gunSndID);
            }//else if (m_gnats.justFired())
            if (m_gnats.timeToExplode())
            {
               m_snd.play(explodeSndID);
            }//else if (m_gnats.timeToExplode())
            m_gnats.resetEvents();
         }//if (!game.m_scene.getPaused())
         else if (cont)
         {
            Sleep(100);
         }
      }//try
      catch(...)
      {
         AWTRACE("AlteredSaver - EXCEPTION during render - terminating...!\n");
         m_status = AW_STAT_TERMINATE_ERROR;
         cont     = FALSE;
      }//catch(...)
   }//while (cont) 
   return m_status;
}//int AWSaverGame::runMessageLoop()



int         
AWSaverGame::execute(int nCmdShow)
{
   m_status  = AW_STAT_CONTINUE;
   UINT                 oldval;
   resetData();
   AWWinUtils::init(); //get winsysdir for later verification
   //read stored config info
   m_gnats.copyFromConfig();
   m_cfg.updateDisplayInfo();
   m_cfg.buildSceneList(m_cfg.m_cmdLineSceneName);
   initSound();
   AWCString useScene(m_cfg.m_cmdLineSceneName);
   if (!m_cfg.m_cmdLineSceneName.length() && m_cfg.isSaver())
   {
      useScene = m_cfg.m_scene;
   }
   AWTRACE("Checking user input method - ");
   if (!m_cfg.isGame())
   {
      AWTRACE(" not game, so setting to KEYBOARD!");
      m_gnats.setUseKeyboard();
   }//if (!m_cfg.isGame())
   AWTRACE("\n");
   if (useScene.length() && m_gnats.loadScene(useScene))
   {
      ;
   }
   else if (loadSceneFromResource(IDR_AWLOGO))
   {  //load default scene from binary resource
      ;
   }
   else
   {
      AWTRACE("EXIT - No valid scene could be loaded\n");
      return m_status=AW_STAT_TERMINATE_ERROR;
   }
   if (!m_cfg.isPreview())
   {  //don't change screen res etc. for preview mode     
      m_displayModeSwitched = AWWinUtils::switchToMode(m_cfg.isFullScreen(), m_cfg.m_bpp, 
                                                        m_cfg.m_width, m_cfg.m_height);
      m_oldCur = SetCursor(NULL);
   }//switch (m_mode)
   else
   {  //for preview mode, it seems to be necessary to
      //turn OFF CVAs and DLists. Not sure why.
      m_gnats.m_renderer.setUseDisplayLists(FALSE);
      m_gnats.m_renderer.setUseVertArrays(TRUE);
      m_gnats.m_renderer.setCompileVertArrays(FALSE);
   }
   //TEMPORARY BUG TEST CODE FOR NVIDIA
   //m_gnats.m_renderer.toggleTransformReflections();
   createWindow();
   // Show and update main window 
   ShowWindow(m_hWnd, nCmdShow);
   UpdateWindow(m_hWnd);
   SetForegroundWindow(m_hWnd);   
   if (m_cfg.isSaver())
   {
      SystemParametersInfo(SPI_SCREENSAVERRUNNING, 1, &oldval, 0);
   }
   
   try
   {
      runMessageLoop();
   } 
   catch (...)
   {
      OutputDebugString("EXCEPTION - terminating...");
      return m_status=AW_STAT_TERMINATE_ERROR;
   }
   //copy over discovered settings for hasCubeMap & hasBump
   AWTRACE("\nGrabbing detected graphics capabilites for AWConfig:\n");
   m_cfg.setBumpAvailable(m_gnats.m_renderer.supportsNVRegisterCombiners()||m_gnats.m_renderer.supportsDot3Combiners());
   AWTRACE1("  Bump mapping support: %i\n", m_cfg.getBumpAvailable());
   m_cfg.setCubeEnvAvailable(m_gnats.m_renderer.supportsCubeMap()); 
   AWTRACE1("  Cube Environment support: %i\n", m_cfg.getCubeEnvAvailable());
   m_cfg.setTexCompressAvailable(m_gnats.m_renderer.supportsS3Compression());
   AWTRACE1("  S3 Texture Compression support: %i\n", m_cfg.getTexCompressAvailable());
   m_cfg.saveSharedInfo();
   if (m_displayModeSwitched)
   {
      AWWinUtils::revertToUserMode();
      m_displayModeSwitched = FALSE;
   }//if (m_displayModeSwitched)
	if (m_oldCur)
   {  
      SetCursor(m_oldCur);
      m_oldCur = NULL;
   }
   shutdownSound();
   if (m_cfg.isSaver())
   {
      SystemParametersInfo(SPI_SCREENSAVERRUNNING, 0, &oldval, 0);
   }//if (m_cfg.isSaver()) 
   unRegisterWindowClass();
   AWTRACE("Terminating AlteredSaver!\n");
   return m_status;
}//int AWSaverGame::execute(int nCmdShow)


