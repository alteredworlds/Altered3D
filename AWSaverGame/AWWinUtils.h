#ifndef __AW_WINUTILS_H
#define __AW_WINUTILS_H
 
#define AW_SYSTEMGL		1
#define AW_VOODOOGL		2
#define AW_DIRECTXGL	3

#define AW_ABOUT_MENU  100
#define AW_ONWEB_MENU  101

#include <time.h>
#include "AWConfig.h"

class AWWinUtils
{
public:
   static int	     init();
   static void      cleanup();

   static int       showHTMLHelp(const AWCString& file, const AWCString& tag="");
   static int	     launchHelper(const AWCString& file);
   static int       playAudioCD(const AWCString& drive);
   static int       stopAudioCD(const AWCString& drive);
   static AWCString getLongFilename( AWCString& shortname);
   static int       about(HWND hWnd, int ctrlID, const AWCString& helpPath);
   static void      alteredDemoAbout(HWND hWnd=NULL);
   static void      buyNow();
   static AWBoolean buyNowOrRegister(AWConfig& cfg, HINSTANCE hInst, HWND hWnd, int ctrlID);
   static AWBoolean registerAlteredSaver(AWConfig& cfg, HINSTANCE hInst, HWND hWnd);
   static void      visitAlteredWorlds(const AWCString& page="");
   static AWBoolean launchUpdate(HINSTANCE hInst, int resID, const AWCString& params="");
   static int	     launchApplication(const AWCString& app, 
                           const AWCString& params="", AWBoolean showMini=FALSE, AWBoolean isTmp=FALSE);

   static int       cleanupTmpApplication();
   static void      saveTmpFileName(const AWCString& n);

   static AWBoolean verifyDateInfo(time_t& stored, int& daysLeft);

   static AWBoolean verifyBinary(const AWCString& exePath);

   static int       expandLZCompressedTNT(char* fullName);

   static int       revertToUserMode();
   static int       getUserMode();
   static int       switchToMode(int fullScreen, int bpp, int width, int height);

   static AWCString getMsgName(int msg);
//these will be filled with the user's initial screen mode
   static int	     m_bpp;
   static int	     m_width;
   static int	     m_height;
   static int       m_refresh;;
   static UINT      m_cdID;
   static int       m_daysLeft;
   static double    m_checkSum;
};//class AWWinUtils

#endif