#include "AWSaverGame.h"
#include "res/resource.h"
#include "AWSettings/AWSettingsCommon.h"
#include "AWSettings/AWSettingsDlg.h"
#include "AWSettings/AWNagDlg.h"


AWTRACE_INIT("AlteredSaver.log")



int makeSureDestructorIsCalled(AWConfig& cfg, int nCmdShow, HINSTANCE hInstance)
{
   AWSaverGame saverGame(cfg, hInstance);
   return saverGame.execute(nCmdShow);
}//int makeSureDestructorIsCalled(AWConfig& cfg, int nCmdShow, HINSTANCE hInstance)



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
   HANDLE mutex = CreateMutex( NULL, TRUE,  "AlteredSaver");
   if (!mutex || (ERROR_ALREADY_EXISTS == GetLastError()))
   {
      AWTRACE("\n\nAlteredSaver mutex detected - can't start\n\n");
      if (mutex)
      {
         DWORD dwMilliseconds = 500;
         HANDLE handles[2];
         handles[0] = mutex;
         handles[1] = NULL;
         AWTRACE("Did get AlteredSaver mutex - just don't own it yet...\n");
         AWTRACE1("Wait on AlteredSaver mutex for timeout of %d\n", (int)dwMilliseconds); 
         DWORD waitResult = MsgWaitForMultipleObjects(1,          // number of handles in the handle array
                                         handles,     // pointer to the object-handle array
                                         FALSE,         // wait for all or wait for one
                                         dwMilliseconds,  // time-out interval in milliseconds
                                         QS_ALLEVENTS);         
         if (WAIT_OBJECT_0 != waitResult)
         {
            AWTRACE("Wait on mutex failed - aborting\n");
            CloseHandle(mutex);
            mutex = NULL;
            return 0;
         }//if (WAIT_OBJECT_0 != waitResult)
         else
         {
            AWTRACE("Wait on mutex succeeded - time to execute AlteredSaver!\n");
         }//else
      }//if (mutex)
   }//if (!mutex || (ERROR_ALREADY_EXISTS == GetLastError()))
   
   AWCString   sceneName(lpCmdLine);
   AWConfigWin cfg; 
   AWBoolean   showNagDlg    = FALSE;
   //::MessageBox(NULL, lpCmdLine, "Command Line", MB_OK);
   char        fileName[MAX_PATH];
   GetModuleFileName(hInstance, fileName, sizeof(fileName));
   cfg.setExePath(fileName);
   //::MessageBox(NULL, fileName, "File Name", MB_OK);
   
   //extract scene name from command line, also
   //MODE: SAVER, GAME, PREVIEW (includes windowID)
   cfg.loadConfig();
   //verify user info, date etc - also binary checksum
   cfg.loadUserInfo();
   cfg.parseCommandLine(lpCmdLine, NULL!=strstr(fileName, ".scr") ||
                                   NULL!=strstr(fileName, ".SCR"));
   AWWinUtils::cleanupTmpApplication(); //if any - 
   try
   {  //at this point, can launch config dialog OR 3D content...
      int       retCode       = 0;
      AWBoolean continueDlg   = TRUE;
      AWBoolean show3DContent = !cfg.isConfig();
      while (continueDlg)
      {  //we need to show the config dialog 
         continueDlg   = FALSE;
         if (cfg.isConfig())
         {
            AWTRACE("\nLaunching AlteredSaver configuration dialog\n");
            show3DContent = FALSE;
            AWSettingsDlg lnch(cfg, hInstance);
            int retCode = lnch.DoModal();
            if (IDC_PLAY_NOW == retCode)
            {  //if user hits 'Play Now!' the dialog ends with this
               //return code, and we should now launch the 3D content...
               cfg.parseCommandLine(cfg.getGame());
               show3DContent = TRUE;
               continueDlg   = TRUE;
            }//if (IDC_PLAY_NOW == lnch.DoModal())
            else if (IDC_PREVIEWSAVER == retCode)
            {  //if user hits 'Preview!' the dialog ends with this
               //return code, and we should now launch the 3D content...         
               cfg.setIsSaver(AWConfig::awSaver);
               show3DContent = TRUE;
               continueDlg   = TRUE;
            }//else if (IDC_PREVIEWSAVER == retCode)
            else if (IDC_UPDATESAVER == retCode)
            {
               ReleaseMutex(mutex);
               AWWinUtils::launchUpdate(hInstance, IDR_AWUPDATE);
               return 0;
            }
            else
            {
               showNagDlg = TRUE;
            }
         }//if (cfg.isConfig())
         while (show3DContent)
         {
            retCode = makeSureDestructorIsCalled(cfg, nCmdShow, hInstance);
            switch (retCode)
            {
            case AW_STAT_TERMINATE_ERROR:
               show3DContent = FALSE;
               ::MessageBeep(MB_ICONEXCLAMATION);
               break;
            
            default:
            case AW_STAT_TERMINATE_DONE:
               //now indicate config to be shown again
               showNagDlg    = !cfg.isPreview();
               cfg.setIsSaver(AWConfig::awConfig);
               show3DContent = FALSE;              
               break;
            }//switch (retCode)           
         }//while (continueDlg && cfg.isConfig())
      }//while (continueDlg)
   }//try
   catch (...)
   {
      fprintf(stderr, "EXCEPTION - Terminating AlteredSaver!\n");
   }//catch (...)
#ifdef _AW_SHAREWARE_REGISTRATION
   if (showNagDlg && !cfg.isRegistered())
   {  //now if we're unregistered => need to show NAG SCREEN
      AWNagDlg nagDlg(cfg, AWWinUtils::m_daysLeft, cfg.m_help, hInstance, NULL);
      nagDlg.DoModal();
   }//if (showNagDialog && !cfg.isRegistered())
#endif
   ReleaseMutex(mutex);
   return 0;
}//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)