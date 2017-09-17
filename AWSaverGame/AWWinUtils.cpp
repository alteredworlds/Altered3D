#include <AW3DTools/AWToolsCommon.h>
#include <AW3DTools/AWFile.h>
#include <AW3DTools/AWCString.h>
#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include <lzexpand.h>
#include <Shlwapi.h>
#include "AWWinUtils.h"

#define AW_LAST_TMPAPP_KEY  "Software\\AlteredWorlds\\DelTmp"


#ifdef _AW_SHAREWARE_REGISTRATION
#include "AWSettings/AWUserInfoDlg.h"
#include "AWSettings/AWAutoSerialDlg.h"
#endif
 
UINT      AWWinUtils::m_cdID     = 0;
int       AWWinUtils::m_refresh  = -1;
int	    AWWinUtils::m_bpp		= 0;
int	    AWWinUtils::m_width		= 0;
int	    AWWinUtils::m_height   = 0;
int       AWWinUtils::m_daysLeft = 0;

double AWWinUtils::m_checkSum = 939607672.343907;

extern void file_uncompress(const char* file);


AWBoolean
AWWinUtils::init()
{ 
   return TRUE;
}//AWBoolean init()



AWBoolean
AWWinUtils::verifyDateInfo(time_t& stored, int& daysLeft)
{  //get the system time via windows call
   //for better security, should verify that this date agrees with
   //a few core files on the system
   AWBoolean retVal = FALSE;
   time_t curTime;
   _tzset();
   time(&curTime);
   m_daysLeft = daysLeft;
   //now compare with values passed in
   double difference = difftime(curTime, stored);
   if (daysLeft)// && (difference > 0.0))
   {  //don't really care if time is till moving forwards
      //just whether or not we had trials left     
      retVal = TRUE;
      if (fabs(difference) >= 86400.0) //24 hours...
      {  
         daysLeft--;
      }//if (difference >= 86400.0)     
   }//if (daysLeft && (difference > 0.0))
   //need to write this back to registry iff succeeded
   stored = curTime;
   return retVal;
}//AWBoolean AWWinUtils::verifyDateInfo(time_t& stored, int& daysLeft)



void
findAndReplaceCHECKSUM(unsigned char* test, size_t len, size_t& bytesRead)
{
   if (!test) return;
   int bytesToEnd = len;
   for (size_t i =0; i<len; i++)
   {
      bytesRead++;
      bytesToEnd--;
      if ((bytesRead >= 256 )&&(bytesRead <= 266))
      {
         test[i]=0; //EXE HEADER CRC
      }
#ifdef _AW_SHAREWARE_REGISTRATION
      if (bytesRead == 439384)
      {
         memset(&test[i], 0, (bytesToEnd >= 8) ? 8: bytesToEnd);
      }//if (bytesRead == 402000)
#else
      if (bytesRead == 385152)
      {
         memset(&test[i], 0, (bytesToEnd >= 8) ? 8: bytesToEnd);
      }//if (bytesRead == 397904)
#endif
   }//for (size_t i =0; i<len; i++)
}//void findAndReplaceCHECKSUM(char* test, size_t len, size_t& bytesRead)



AWBoolean      
AWWinUtils::verifyBinary(const AWCString& exePath)
{  //module name came from the WinMain arguments
#ifndef _AWDISABLECHECKSUM
   double         chkSum = 0.0;
   size_t         bytesRead = 0;
   unsigned char  testChar[199]; 
   size_t         numRead=0;
   size_t         testCharLen = sizeof(testChar);
#ifdef _AW_TESTCHECKSUM
   FILE* t   = fopen("c:\\temp\\test.txt", "w");
   FILE* alt = fopen("aw.exe", "wb");
   fprintf(t, "checking file %s\n", exePath.c_str());
#endif //_AW_TESTCHECKSUM
   FILE* testMe = fopen(exePath.c_str(), "rb");
   if (testMe)
   {  //found self (I should hope so - we're running)
      //now read each char and compute hokey checksum 
#ifdef _AW_TESTCHECKSUM
      fprintf(t, "file opened OK\n");
#endif 
      long lastPos = 0;
      long nextPos=0;
      do
      {
         fread(testChar, testCharLen, 1, testMe);
         nextPos=ftell(testMe);
         numRead = nextPos-lastPos;
         if (numRead)
         {
            lastPos=nextPos;          
            findAndReplaceCHECKSUM(testChar, numRead, bytesRead);
            for (size_t i =0; i<numRead; i++)
            {
               chkSum += ((double)testChar[i]/5.0 + 2.72) * 72.6; 
            }
#ifdef _AW_TESTCHECKSUM
            fwrite(testChar, numRead, 1, alt);
#endif
         }//if (testCharLen)
      }//while (!testMe.Eof())
      while (!feof(testMe));
   }//if (testMe.isValid())
   double diffrnce = fabs(chkSum - m_checkSum);
   AWBoolean retVal = (diffrnce <= 0.000001);
 #ifdef _AW_TESTCHECKSUM
   fprintf(t, "stored Checksum %f  calculated Checksum %f diff %f match %d", m_checkSum, chkSum, diffrnce, retVal);
   fflush(t);
   fclose(t);
   fclose(alt);
 #endif
   return retVal;
#else //#ifndef_AWDISABLECHECKSUM ...
   return TRUE;
#endif //#ifndef_AWDISABLECHECKSUM ...
}//AWBoolean AWWinUtils::verifyBinary(const AWCString& exePath)



void
AWWinUtils::cleanup()
{
	revertToUserMode();
}



int
AWWinUtils::revertToUserMode()
{
   AWTRACE("revertToUserMode\n");
	ChangeDisplaySettings(NULL, 0);
	return 1;
}//int AWWinUtils::revertToUserMode()



int
AWWinUtils::switchToMode(int fullScreen, int bpp, int width, int height)
{
   AWTRACE4("Switching screen mode to full %d, BPP %d,  w %d,  h%d\n",fullScreen, bpp, width, height);
	AWBoolean changeNeeded = FALSE;
	DEVMODE devMode;     
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmFields = 0;
	if (-1 != m_refresh)
	{
      AWTRACE("  Refresh change advised\n");
		devMode.dmFields |= DM_DISPLAYFREQUENCY;
		devMode.dmDisplayFrequency = m_refresh;
	}
	if (bpp != m_bpp)
	{
      AWTRACE("  Bit Depth change required\n");
		devMode.dmFields |= DM_BITSPERPEL;
		devMode.dmBitsPerPel = bpp;
		changeNeeded = TRUE;
	}
	if (fullScreen && ((m_width != width) | (m_height != height)))
	{
      AWTRACE("  Size change required\n");
		devMode.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;
		devMode.dmPelsWidth  = width;     
		devMode.dmPelsHeight = height; 
		changeNeeded = TRUE;
    }
   DWORD flags = (fullScreen) ? CDS_FULLSCREEN : 0;
   if (changeNeeded) ChangeDisplaySettings( &devMode, flags );
	return changeNeeded;
}//int AWWinUtils::switchToMode(int fullScreen, int bpp, int width, int height)



void                 
AWWinUtils::buyNow()
{
   ShellExecute( NULL, 
		"open",  // pointer to string that specifies operation to perform 
		"http://www.alteredworlds.com/cgi-bin/toStore",  // pointer to filename or folder name string 
		NULL, // pointer to string that specifies executable-file parameters 
		NULL,  // pointer to string that specifies default directory 
		SW_SHOW);   // whether file is shown when opened
}//void AWWinUtils::buyNow()


void                 
AWWinUtils::visitAlteredWorlds(const AWCString& page)
{  //now launch their browser and take them to our site! 
   AWCString fullURL("http://www.alteredworlds.com");
   if (page.size())
   {
      fullURL += "/";
      fullURL += page;
   }
   ShellExecute( NULL, 
      "open",  // pointer to string that specifies operation to perform 
      fullURL.c_str(), 
      NULL, // pointer to string that specifies executable-file parameters 
      NULL,  // pointer to string that specifies default directory 
      SW_SHOW);   // whether file is shown when opened
}//void AWWinUtils::visitAlteredWorlds()


void      
AWWinUtils::alteredDemoAbout(HWND hWnd)
{  
   char buf[2048];
   strcpy(buf, "From AlteredWorlds.com - a new version of: \n\n\"AlteredSaver v3\"\n\n");
   strcat(buf, "A new, enhanced version of AlteredSaver - faster than ever,\n");
   strcat(buf, "with new visual effects.\n\n");
   strcat(buf, "Includes a 3D Studio MAX Exporter for 3D content creation.\n");
   strcat(buf, "\nPlease check our web site for news of updates and content:\n\n");
   strcat(buf, "Web:\thttp://www.AlteredWorlds.com\n");
   strcat(buf, "Email:\tinfo@alteredworlds.com\n\n");
   strcat(buf, "3D Studio MAX is a trademark of Autodesk & Discreet Software.\n");
   strcat(buf, "All other Contents Copyright © AlteredWorlds 2001.\n");
   MessageBox(hWnd, buf, "AlteredSaver v3.2!", MB_OK);
}//void  AWWinUtils::alteredDemoAbout(HWND hWnd)



AWBoolean      
AWWinUtils::buyNowOrRegister(AWConfig& cfg, HINSTANCE hInst, HWND hWnd, int ctrlID)
{
   AWBoolean justRegistered = FALSE;
   //produce popup menu at middle RHS of 'About' button
   //menu allows users to select between about box & visiting AW online
   RECT button;   
   const int menuAWBuyNowPage     = 100;
   const int menuAWRegister       = 101;
   GetWindowRect(GetDlgItem(hWnd, ctrlID), &button);
   HMENU hMenu = CreatePopupMenu();
   if (hMenu)
   {
      AppendMenu(hMenu, MF_STRING, menuAWBuyNowPage, "Product Purchase Page...");
      AppendMenu(hMenu, MF_STRING, menuAWRegister,   "Register AlteredSaver v3...");
      RECT dummy;
      int test = TrackPopupMenu(hMenu,  TPM_TOPALIGN | TPM_RIGHTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,     
                           button.left+(button.right-button.left)/2, button.top, 0, hWnd, &dummy);
      DestroyMenu(hMenu);
      if (menuAWBuyNowPage == test)
      {
         buyNow();
      }//if (menuAWBuyNowPage == test)
      else if (menuAWRegister == test)
      {  //now launch their browser and take them to our site! 
         justRegistered = registerAlteredSaver(cfg, hInst, hWnd);
      }//else if (menuAWRegister == test)
   }//if (hMenu)
   return justRegistered;
}//AWBoolean AWWinUtils::buyNowOrRegister(AWConfig& cfg, HINSTANCE hInst, HWND hWnd, int ctrlID)


int
AWWinUtils::about(HWND hWnd, int ctrlID, const AWCString& helpPath)
{
   int retVal = 0;
   //produce popup menu at middle RHS of 'About' button
   //menu allows users to select between about box & visiting AW online
   RECT button;   
   const int menuAWAbout          = 100;
   const int menuAWOnTheWeb       = 101;
   const int menuAlteredSaverHelp = 102;
   GetWindowRect(GetDlgItem(hWnd, ctrlID), &button);
   HMENU hMenu = CreatePopupMenu();
   if (hMenu)
   {
      AppendMenu(hMenu, MF_STRING, menuAWAbout,          "About Altered&Saver v3...");
      AppendMenu(hMenu, MF_STRING, menuAlteredSaverHelp, "AlteredSaver v3 &Help...");
      AppendMenu(hMenu, MF_SEPARATOR, 0, "");
      AppendMenu(hMenu, MF_STRING, menuAWOnTheWeb,       "&www.AlteredWorlds.com...");
      RECT dummy;
      retVal = TrackPopupMenu(hMenu,  TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON,     
                           button.left+(button.right-button.left)/2, button.top, 0, hWnd, &dummy);
      DestroyMenu(hMenu);
      if (menuAWAbout == retVal)
      {
         alteredDemoAbout(hWnd);
      }//if (menuAWAbout == test)
      else if (menuAWOnTheWeb == retVal)
      {  //now launch their browser and take them to our site! 
         visitAlteredWorlds("AlteredSaver3.htm");
      }//else if (menuAWOnTheWeb == test)
      else if (menuAlteredSaverHelp == retVal)
      {
         showHTMLHelp(helpPath);
      }
   }//if (hMenu)
   return retVal;
}//void AWWinUtils::about(HWND hWnd, int ctrlID, const AWCString& helpPath)



AWBoolean 
AWWinUtils::registerAlteredSaver(AWConfig& cfg, HINSTANCE hInst, HWND hWnd)
{
   AWBoolean retVal = FALSE;
#ifdef _AW_SHAREWARE_REGISTRATION
   AlteredUserInfoDlg userInfo(hInst, hWnd);
   if (IDC_AWNEXT == userInfo.DoModal())
   {  //got candidate data.
      try
      {
         AlteredAutoSerialDlg serialDlg(hInst, hWnd);
         strcpy(serialDlg.m_userName, userInfo.m_userName);
         strcpy(serialDlg.m_email,    userInfo.m_email);
         strcpy(serialDlg.m_delivery, userInfo.m_delivery);
         if (IDC_AWNEXT == serialDlg.DoModal())
         {  //whenever we're finally done, copy the values to
            //the config object and save
            cfg.m_user     = serialDlg.m_userName; 
            cfg.m_email    = serialDlg.m_email; 
            cfg.m_delivery = serialDlg.m_delivery;
            cfg.m_serial   = serialDlg.m_serial;
            cfg.saveUserInfo();
            retVal = TRUE;
         }//if (IDC_NEXT == serialDlg.DoModal())
      }//try
      catch (...)
      {
      }//catch (...)
   }//if (IDC_NEXT == userInfo.DoModal())
#endif//#ifdef _AW_SHAREWARE_REGISTRATION
   return retVal;
}//void AWWinUtils::registerAlteredSaver(AWConfig& cfg, HINSTANCE hInst, HWND hWnd)


int
AWWinUtils::getUserMode()
{
	int retVal = 0;
	HDC hdc = CreateCompatibleDC(NULL); 
	if (hdc)
	{
		retVal    = 1;
		m_bpp	  = GetDeviceCaps(hdc, BITSPIXEL);
		m_width   = GetSystemMetrics(SM_CXSCREEN);     
		m_height  = GetSystemMetrics(SM_CYSCREEN); 
		//need to find out what Win9x returns here, as next call is
		//NT only?
		m_refresh = GetDeviceCaps(hdc, VREFRESH); 
      if ((0 == m_refresh) || ( 1 == m_refresh))
      {
         m_refresh = -1; //don't change it at all if default found
      }
		DeleteDC(hdc);
      AWTRACE4("User mode found to be: BPP %d,  w %d,  h%d, refresh %d\n",m_bpp, m_width, m_height, m_refresh);
	}
	return retVal;
}//int AWWinUtils::getUserMode()


int
AWWinUtils::playAudioCD(const AWCString& drive)
{   
    DWORD dwReturn;
    MCI_OPEN_PARMS mciOpenParms;
    MCI_SET_PARMS mciSetParms;
    MCI_PLAY_PARMS mciPlayParms;

    // Open the CD audio device by specifying the device name.
    mciOpenParms.lpstrDeviceType = "cdaudio";
    if (dwReturn = mciSendCommand(NULL, MCI_OPEN,
        MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms))
    {
        // Failed to open device. Don't close it; just return error.
       AWTRACE("cd device failed to open");
       return dwReturn;
    }

    // The device opened successfully; get the device ID.
    m_cdID = mciOpenParms.wDeviceID;

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
    if (dwReturn = mciSendCommand(m_cdID, MCI_SET, 
        MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms))
    {
        mciSendCommand(m_cdID, MCI_CLOSE, 0, NULL);
        AWTRACE("mciSendCommand failed");
        return (dwReturn);
    } 
    // Begin play from the specified track and play to the beginning 
    // of the next track. The window procedure function for the parent 
    // window will be notified with an MM_MCINOTIFY message when 
    // playback is complete. Unless the play command fails, the window 
    // procedure closes the device.

    mciPlayParms.dwFrom = 0L;
    mciPlayParms.dwTo = 0L;
    mciPlayParms.dwFrom = 0L;
    mciPlayParms.dwTo = 0L;
    mciPlayParms.dwCallback = NULL;
    if (dwReturn = mciSendCommand(m_cdID, MCI_PLAY,
        0L,  (DWORD)(LPVOID) &mciPlayParms))
    {
        AWTRACE("playcd failed\n");
        mciSendCommand(m_cdID, MCI_CLOSE, 0, NULL);
        return (dwReturn);
    }
    AWTRACE("playcd suceeded");
    return 0L;
}//int AWWinUtils::playAudioCD(const AWCString& drive)



AWBoolean	
AWWinUtils::launchUpdate(HINSTANCE hInst, int resID, const AWCString& params)
{  //update program is stored as a compressed binary resource
   //in this executable
   //
   //FIRST:    dump the binary resource to disk [suitable temp posn.]  
   AWBoolean retVal = FALSE;
   HRSRC  hrsrc = FindResource(hInst, MAKEINTRESOURCE(resID), 
                                      MAKEINTRESOURCE(300));
   if (hrsrc==NULL) 
   {
      ::MessageBeep(MB_ICONEXCLAMATION);
      return FALSE;
   }
   DWORD  size  = SizeofResource(hInst, hrsrc);
   LPVOID rdata = LockResource(LoadResource(hInst, hrsrc));

   char tmpPath[MAX_PATH];
   if (GetTempPath(MAX_PATH, tmpPath))
   {
      strcat(tmpPath, "~awupd.exe.gz");
      FILE* f = fopen(tmpPath, "wb");
      if (f)
      {
         fwrite(rdata, size, 1, f);
         fclose(f);
         //SECOND:   expand compressed file to get executable
         file_uncompress(tmpPath);
         tmpPath[strlen(tmpPath)-3]=0; //remove the .gz
         //THIRD:    launch this executable as a new process and return
         //indicate this app is TEMPORARY
         launchApplication(tmpPath, params, FALSE, TRUE);
      }//if (f)
   }//if (GetTempPath(MAX_PATH, tmpPath))
   return retVal;
}//AWBoolean AWWinUtils::launchUpdate(HINSTANCE hinst, int resID, const AWCString& params)


int       
AWWinUtils::cleanupTmpApplication()
{
   int retVal = 0;
   LONG res; HKEY skey; DWORD valtype;
  
   res=RegOpenKeyEx(HKEY_CURRENT_USER,AW_LAST_TMPAPP_KEY,0,KEY_ALL_ACCESS,&skey);
   if (res!=ERROR_SUCCESS) return retVal;

   char  strData[MAX_PATH];
   DWORD bufSize = MAX_PATH;
   if ((ERROR_SUCCESS == RegQueryValueEx(skey, "del", NULL, &valtype, (BYTE*)strData, &bufSize))
        && bufSize)
   {	//should be REG_SZ
      const char* fName = (const char*)strData;
      if (AWFile::Exists(fName))
      {
         if (DeleteFile(fName))
         {
            RegSetValueEx(skey, "del", 0, REG_SZ, (BYTE*)"", strlen(""));
            retVal = 1;
         }//if (DeleteFile(fileToDelete))
      }//if (fileToDelete.length() && AWFile::Exists(fileToDelete))
   }//if (ERROR_SUCCESS == RegQueryValueEx(skey, "del", NULL, &valtype, (BYTE*)strData, &bufSize))
   RegCloseKey(skey);
   return retVal;
}//int AWWinUtils::cleanupTmpApplication()


void
AWWinUtils::saveTmpFileName(const AWCString& n)
{
   LONG res; HKEY skey; DWORD disp;
   res=RegCreateKeyEx(HKEY_CURRENT_USER, AW_LAST_TMPAPP_KEY, 0, NULL,
                      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &skey, &disp);
   if (res!=ERROR_SUCCESS) return;
   RegSetValueEx(skey, "del", 0, REG_SZ, (CONST BYTE*)n.c_str(), n.length());
   RegCloseKey(skey);
}//void AWWinUtils::saveTmpFileName(const AWCString& n)



int	     
AWWinUtils::launchApplication(const AWCString& app, const AWCString& params, 
                              AWBoolean showMini, AWBoolean isTmp)
{
   int retVal = 0;
   PROCESS_INFORMATION procInfo;
	STARTUPINFO moreStructs;
	memset(&moreStructs, 0, sizeof(moreStructs));
	moreStructs.cb          = sizeof(STARTUPINFO);
	moreStructs.lpReserved  = NULL;
	moreStructs.lpDesktop   = ""; //takes care of NT invisible, new desktop
	moreStructs.lpTitle     = NULL;
	moreStructs.cbReserved2 = 0;
	moreStructs.lpReserved2 = NULL;
   if (showMini)
   {
      moreStructs.dwFlags     = STARTF_USESHOWWINDOW;
      moreStructs.wShowWindow = SW_SHOWMINNOACTIVE;
   }
	AWCString commandLine(app);
   if (isTmp)
   {  //preserve the file name so we can delete it later
      saveTmpFileName(app);
   }//if (isTmp)
   if (params.length())
   {
	   commandLine += " \"";
	   commandLine += params;
      commandLine += "\"";
   }
	if (CreateProcess( NULL, (LPTSTR)commandLine.c_str(), 
		NULL, NULL, FALSE, DETACHED_PROCESS, NULL,
		NULL, &moreStructs, &procInfo))
	{
		retVal = 1;
	}
   return retVal;
}//int AWWinUtils::launchApplication(const AWCString& app)



int	
AWWinUtils::launchHelper(const AWCString& file)
{
	//on winnt4 midi player just opens rather than plays -
	//so for midi files supply "play"
	//note that mp3 player from real audio doesn't define a play?
	int retVal = 0;
	char appPath[MAX_PATH];
	HINSTANCE test = FindExecutable( (LPCTSTR)file.c_str(), NULL, appPath);
	if (32< (long)test)
	{	//we found the helper app, so now need to launch it in a
		//windows 9x and NT friendly manner
      retVal = launchApplication(appPath, file, TRUE);
	}
	return retVal;
}//int	AWWinUtils::launchHelper(const AWCString& file)



int  
AWWinUtils::stopAudioCD(const AWCString& drive)
{
   int retVal = 0;
   if (m_cdID)
   {
      MCI_GENERIC_PARMS junk;
      junk.dwCallback = NULL;
 
      mciSendCommand(m_cdID, MCI_STOP, 
                             0L,(DWORD) (LPMCI_GENERIC_PARMS)&junk);
 
      mciSendCommand(m_cdID, MCI_CLOSE, 0, NULL);
      AWTRACE("Audio CD Device Closed\n");
      retVal = 1;
   } 
	return retVal;
}//int  AWWinUtils::stopAudioCD(const AWCString& drive)



int  
AWWinUtils::showHTMLHelp(const AWCString& file, const AWCString& tag)
{	//test to see if we have a readable file before proceeding
    int retVal = 1; //failure
	if (AWFile::Exists(file) && 
		(file.contains(".htm")  || file.contains(".html"))  )
	{
        //find system default browser and execute via create process so that
        //we can supply an embedded marker within a page
	    char appPath[MAX_PATH];
	    HINSTANCE test = FindExecutable( (LPCTSTR)file.c_str(), NULL, appPath);
	    if (32< (long)test)
	    {	//we found the helper app, so now need to launch it in a
		    //windows 9x and NT friendly manner
		    PROCESS_INFORMATION procInfo;
		    STARTUPINFO moreStructs;
		    memset(&moreStructs, 0, sizeof(moreStructs));
		    moreStructs.cb = sizeof(STARTUPINFO);
		    moreStructs.lpReserved=NULL;
		    moreStructs.lpDesktop=""; //takes care of NT invisible, new desktop
		    moreStructs.lpTitle=NULL;
		    moreStructs.cbReserved2=0;
		    moreStructs.lpReserved2=NULL;

		    AWCString commandLine(appPath);
		    commandLine += " ";
		    commandLine += file;
            commandLine += tag;

		    if (CreateProcess( NULL, (LPTSTR)commandLine.c_str(), 
			    NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL,
			    NULL, &moreStructs, &procInfo))
		    {
			    retVal = 0;//SUCCESS
		    }
	    }//if (32< (long)test)
		return retVal;
	}//if valid help file
	else 
   {
      ::MessageBox(NULL, "AlteredSaver doesn\'t seem to be correctly installed.\n\nPlease re-install AlteredSaver!\n\nAll help may be viewed online at AlteredWorlds.com!", "AlteredSaver Help not found!", MB_OK);
      return -1;
   }
}//int AWWinUtils::showHTMLHelp(const AWCString& file, const AWCString& tag)



int
AWWinUtils::expandLZCompressedTNT(char* fullName)
{
    //test to see if compressed, and if so expand it. In either case
    //return the usable file name in newName [must be big enough].
    //we assume that a compressed file has extension TNX
    //expand to a file with extension TNT in the same directory
    int retVal = 0;
    //test to see if (by naming convention) this file is compressed
    int index = strlen(fullName)-1;
    //move past any trailing white space (shouldn't be any, really)
    while ((index >= 0) && (fullName[index] == ' '))
    {
        index--;
    }//while ((index >= 0) && (fullName[index] == ' '))
    if (index && ((fullName[index] == 'x') || (fullName[index] == 'X')))
    {//we have a compressed file
        INT      sourceHandle, destHandle;
        OFSTRUCT sourceInfo,   destInfo;
        sourceHandle = LZOpenFile(fullName, &sourceInfo, OF_READ);
        if (sourceHandle<0) return -998;
        //expanded file name replaces X|x with t
        fullName[index] = 't';
        destHandle = LZOpenFile(fullName, &destInfo, OF_CREATE);
        if (destHandle<0)   
        {
            LZClose(sourceHandle);
            return -999;
        }
        retVal = LZCopy(sourceHandle, destHandle);
        LZClose(sourceHandle);
        LZClose(destHandle);
    }//if (index && ((fullName[index] == 'x') || (fullName[index] == 'X'))
    return retVal;
}//int AWWinUtils::expandLZCompressedTNT(char* fullName)



AWCString 
AWWinUtils::getLongFilename( AWCString& shortname)
{
   AWCString retVal(shortname);
   //Just check first of all that we don't convert a network 
   //sharename - they can't be convereted
   if ((shortname.length() > 2) && 
       (shortname.at(0) == '\\') && 
       (shortname.at(1) == '\\') )
   {
      return shortname;
   }
   WIN32_FIND_DATA pFindFileData;
   //Lob off the last element of the passed name. 
   //If we received only a root name, e.g. c:\, 
   //ExtractFileDir returns the path unchanged.
   string::size_type posn = shortname.rfind('\\');
   if (string::npos != posn)
   {
      retVal = shortname;
      retVal.replace(posn, retVal.length()-posn, "");
   }
   if (retVal != shortname)
   {  //We still have an unconverted path element. So convert
      //the last one in the current shortname and combine the
      //resulting long name with what we get by calling ourselves
      //recursively with the rest of the path. 
      HANDLE fileHandle = FindFirstFile(shortname.c_str(), &pFindFileData);
      if (INVALID_HANDLE_VALUE != fileHandle)
      {
         retVal = AWWinUtils::getLongFilename(retVal);
         retVal += "\\";
         retVal += pFindFileData.cFileName;
         FindClose( fileHandle );
      }     
   }//if (Result <> shortname)
   else if (retVal.contains("\\"))
   {  //Only the root remains. Remove the backslash since the
      //caller will add it back anyway. } 
      retVal.at(retVal.length()) = 0; //- THIS CALL DOES NOTHING?
   }
   return retVal;
}//AWCString AWWinUtils::getLongFilename( AWCString& shortname)


//DEBUGGING SUPPORT for Windows messages
AWCString
AWWinUtils::getMsgName(int msg)
{
   AWCString msgName;
#ifdef __AWTRACE
   switch(msg)
   {
      case 0x0000:
      msgName = "WM_NULL";break;                 
      case 0x0001:        
      msgName = "WM_CREATE";break;
      case 0x0002:                       
      msgName = "WM_DESTROY";break;                      
      case 0x0003:
      msgName = "WM_MOVE";break;                         
      case 0x0005:
      msgName = "WM_SIZE";break;                         
      case 0x0006:
      msgName = "WM_ACTIVATE";break;                     
      case 0x0007:
      msgName = "WM_SETFOCUS";break;                     
      case 0x0008:
      msgName = "WM_KILLFOCUS";break;                    
      case 0x000A:
      msgName = "WM_ENABLE";break;                       
      case 0x000B:
      msgName = "WM_SETREDRAW";break;                    
      case 0x000C:
      msgName = "WM_SETTEXT";break; 
      case 0x000D:                     
      msgName = "WM_GETTEXT";break;
      case 0x000E:                      
      msgName = "WM_GETTEXTLENGTH";break;                
      case 0x000F:
      msgName = "WM_PAINT";break;                        
      case 0x0010:
      msgName = "WM_CLOSE";break;                        
      case 0x0011:
      msgName = "WM_QUERYENDSESSION";break;              
      case 0x0012:
      msgName = "WM_QUIT";break;                  
      case 0x0013:       
      msgName = "WM_QUERYOPEN";break;               
      case 0x0014:     
      msgName = "WM_ERASEBKGND";break;                   
      case 0x0015:
      msgName = "WM_SYSCOLORCHANGE";break;               
      case 0x0016:
      msgName = "WM_ENDSESSION";break;                   
      case 0x0018:
      msgName = "WM_SHOWWINDOW";break;                   
      case 0x001A:
      msgName = "WM_WININICHANGE";break; 
      case 0x001B:                
      msgName = "WM_DEVMODECHANGE";break;                
      case 0x001C:
      msgName = "WM_ACTIVATEAPP";break;                  
      case 0x001D:
      msgName = "WM_FONTCHANGE";break;                  
      case 0x001E: 
      msgName = "WM_TIMECHANGE";break;                  
      case 0x001F:
      msgName = "WM_CANCELMODE";break;                   
      case 0x0020:
      msgName = "WM_SETCURSOR";break;                    
      case 0x0021:
      msgName = "WM_MOUSEACTIVATE";break;                
      case 0x0022:
      msgName = "WM_CHILDACTIVATE";break;                
      case 0x0023:
      msgName = "WM_QUEUESYNC";break;                    
      case 0x0024:
      msgName = "WM_GETMINMAXINFO";break;                
      case 0x0026:
      msgName = "WM_PAINTICON";break;                    
      case 0x0027:
      msgName = "WM_ICONERASEBKGND";break;               
      case 0x0028:
      msgName = "WM_NEXTDLGCTL";break;                   
      case 0x002A:
      msgName = "WM_SPOOLERSTATUS";break;                
      case 0x002B:
      msgName = "WM_DRAWITEM";break;                     
      case 0x002C:
      msgName = "WM_MEASUREITEM";break;                  
      case 0x002D:
      msgName = "WM_DELETEITEM";break;                   
      case 0x002E:
      msgName = "WM_VKEYTOITEM";break;                   
      case 0x002F:
      msgName = "WM_CHARTOITEM";break;                   
      case 0x0030:
      msgName = "WM_SETFONT";break;                      
      case 0x0031:
      msgName = "WM_GETFONT";break;                      
      case 0x0032:
      msgName = "WM_SETHOTKEY";break;                    
      case 0x0033:
      msgName = "WM_GETHOTKEY";break;                    
      case 0x0037:
      msgName = "WM_QUERYDRAGICON";break;                
      case 0x0039:
      msgName = "WM_COMPAREITEM";break;                  
      case 0x0041:
      msgName = "WM_COMPACTING";break;                   
      case 0x0044:
      msgName = "WM_COMMNOTIFY";break;                   
      case 0x0046:  
      msgName = "WM_WINDOWPOSCHANGING";break;            
      case 0x0047:
      msgName = "WM_WINDOWPOSCHANGED";break;             
      case 0x0048:
      msgName = "WM_POWER";break;                        
      case 0x004A:
      msgName = "WM_COPYDATA";break;                     
      case 0x004B:
      msgName = "WM_CANCELJOURNAL";break;                
      case 0x004E:
      msgName = "WM_NOTIFY";break;                       
      case 0x0050:
      msgName = "WM_INPUTLANGCHANGEREQUEST";break;       
      case 0x0051:
      msgName = "WM_INPUTLANGCHANGE";break;              
      case 0x0052:
      msgName = "WM_TCARD";break;                        
      case 0x0053:
      msgName = "WM_HELP";break;                         
      case 0x0054:
      msgName = "WM_USERCHANGED";break;                 
      case 0x0055: 
      msgName = "WM_NOTIFYFORMAT";break;                
      case 0x007B:
      msgName = "WM_CONTEXTMENU";break;                  
      case 0x007C:
      msgName = "WM_STYLECHANGING";break;                
      case 0x007D:
      msgName = "WM_STYLECHANGED";break;                 
      case 0x007E:
      msgName = "WM_DISPLAYCHANGE";break;                
      case 0x007F:
      msgName = "WM_GETICON | WM_SETICON";break;                                           
      case 0x0081:
      msgName = "WM_NCCREATE";break;                     
      case 0x0082:
      msgName = "WM_NCDESTROY";break;                    
      case 0x0083:
      msgName = "WM_NCCALCSIZE";break;                   
      case 0x0084:
      msgName = "WM_NCHITTEST";break;                    
      case 0x0085:
      msgName = "WM_NCPAINT";break;                      
      case 0x0086:
      msgName = "WM_NCACTIVATE";break;
      case 0x0087:
      msgName = "WM_GETDLGCODE";break;                   
      case 0x00A0:
      msgName = "WM_NCMOUSEMOVE";break;                  
      case 0x00A1:
      msgName = "WM_NCLBUTTONDOWN";break;                
      case 0x00A2:
      msgName = "WM_NCLBUTTONUP";break;
      case 0x00A3:                  
      msgName = "WM_NCLBUTTONDBLCLK";break;              
      case 0x00A4:
      msgName = "WM_NCRBUTTONDOWN";break;                
      case 0x00A5:
      msgName = "WM_NCRBUTTONUP";break;                  
      case 0x00A6:
      msgName = "WM_NCRBUTTONDBLCLK";break;              
      case 0x00A7:
      msgName = "WM_NCMBUTTONDOWN";break;                
      case 0x00A8:
      msgName = "WM_NCMBUTTONUP";break;                  
      case 0x00A9:
      msgName = "WM_NCMBUTTONDBLCLK";break;              
      case 0x0100:
      msgName = "WM_KEYFIRST | WM_KEYDOWN";break;                                          
      case 0x0101:
      msgName = "WM_KEYUP";break;                        
      case 0x0102:
      msgName = "WM_CHAR";break;                         
      case 0x0103:
      msgName = "WM_DEADCHAR";break;                     
      case 0x0104:
      msgName = "WM_SYSKEYDOWN";break;                   
      case 0x0105:
      msgName = "WM_SYSKEYUP";break;                     
      case 0x0106:
      msgName = "WM_SYSCHAR";break;                      
      case 0x0107:
      msgName = "WM_SYSDEADCHAR";break;                  
      case 0x0108:
      msgName = "WM_KEYLAST";break;                      
      case 0x010D:
      msgName = "WM_IME_STARTCOMPOSITION";break;         
      case 0x010E:
      msgName = "WM_IME_ENDCOMPOSITION";break;           
      case 0x010F:
      msgName = "WM_IME_COMPOSITION | WM_IME_KEYLAST";break;                               
      case 0x0110:
      msgName = "WM_INITDIALOG";break;                   
      case 0x0111:
      msgName = "WM_COMMAND";break;                      
      case 0x0112:
      msgName = "WM_SYSCOMMAND";break;                   
      case 0x0113:
      msgName = "WM_TIMER";break;                        
      case 0x0114:
      msgName = "WM_HSCROLL";break;                      
      case 0x0115:
      msgName = "WM_VSCROLL";break;                      
      case 0x0116:
      msgName = "WM_INITMENU";break;                     
      case 0x0117:
      msgName = "WM_INITMENUPOPUP";break;                
      case 0x011F:
      msgName = "WM_MENUSELECT";break;                   
      case 0x0120:
      msgName = "WM_MENUCHAR";break;                     
      case 0x0121:
      msgName = "WM_ENTERIDLE";break;                    
      case 0x0132:
      msgName = "WM_CTLCOLORMSGBOX";break;               
      case 0x0133:
      msgName = "WM_CTLCOLOREDIT";break;                 
      case 0x0134:
      msgName = "WM_CTLCOLORLISTBOX";break;              
      case 0x0135:
      msgName = "WM_CTLCOLORBTN";break;                  
      case 0x0136:
      msgName = "WM_CTLCOLORDLG";break;                  
      case 0x0137:
      msgName = "WM_CTLCOLORSCROLLBAR";break;            
      case 0x0138:
      msgName = "WM_CTLCOLORSTATIC";break;               
      case 0x0200:
      msgName = "WM_MOUSEMOVE";break;                    
      case 0x0201:
      msgName = "WM_LBUTTONDOWN";break; 
   }//switch
#endif  //sentinel __AWTRACEHI
   return msgName;
}//AWCString AWWinUtils::getMsgName(int msg);
                  
 
/*                 
msgName = "WM_LBUTTONUP";break;                    case 0x0202
msgName = "WM_LBUTTONDBLCLK";break;                case 0x0203
msgName = "WM_RBUTTONDOWN";break;                  case 0x0204
msgName = "WM_RBUTTONUP";break;                    case 0x0205
msgName = "WM_RBUTTONDBLCLK";break;                case 0x0206
msgName = "WM_MBUTTONDOWN";break;                  case 0x0207
msgName = "WM_MBUTTONUP";break;                    case 0x0208
msgName = "WM_MBUTTONDBLCLK";break;                case 0x0209
msgName = "WM_MOUSEWHEEL";break;                   case 0x020A
msgName = "WM_PARENTNOTIFY"                 case 0x0210
msgName = "WM_ENTERMENULOOP"                case 0x0211
msgName = "WM_EXITMENULOOP"                 case 0x0212
msgName = "WM_NEXTMENU"                     case 0x0213
msgName = "WM_SIZING"                       case 0x0214
msgName = "WM_CAPTURECHANGED"               case 0x0215
msgName = "WM_MOVING"                       case 0x0216
msgName = "WM_POWERBROADCAST"               case 0x0218
msgName = "WM_DEVICECHANGE"                 case 0x0219
msgName = "WM_IME_SETCONTEXT"               case 0x0281
msgName = "WM_IME_NOTIFY"                   case 0x0282
msgName = "WM_IME_CONTROL"                  case 0x0283
msgName = "WM_IME_COMPOSITIONFULL"          case 0x0284
msgName = "WM_IME_SELECT"                   case 0x0285
msgName = "WM_IME_CHAR"                     case 0x0286
msgName = "WM_IME_KEYDOWN"                  case 0x0290
msgName = "WM_IME_KEYUP"                    case 0x0291
msgName = "WM_MDICREATE"                    case 0x0220
msgName = "WM_MDIDESTROY"                   case 0x0221
msgName = "WM_MDIACTIVATE"                  case 0x0222
msgName = "WM_MDIRESTORE"                   case 0x0223
msgName = "WM_MDINEXT"                      case 0x0224
msgName = "WM_MDIMAXIMIZE"                  case 0x0225
msgName = "WM_MDITILE"                      case 0x0226
msgName = "WM_MDICASCADE"                   case 0x0227
msgName = "WM_MDIICONARRANGE"               case 0x0228
msgName = "WM_MDIGETACTIVE"                 case 0x0229
msgName = "WM_MDISETMENU"                   case 0x0230
msgName = "WM_ENTERSIZEMOVE"                case 0x0231
msgName = "WM_EXITSIZEMOVE"                 case 0x0232
msgName = "WM_DROPFILES"                    case 0x0233
msgName = "WM_MDIREFRESHMENU"               case 0x0234
msgName = "WM_MOUSEHOVER"                   case 0x02A1
msgName = "WM_MOUSELEAVE"                   case 0x02A3
msgName = "WM_CUT";break;                         case 0x0300
msgName = "WM_COPY";break;                        case 0x0301
msgName = "WM_PASTE";break;                       case 0x0302
msgName = "WM_CLEAR";break;                       case 0x0303
msgName = "WM_UNDO";break;                        case 0x0304
msgName = "WM_RENDERFORMAT";break;                case 0x0305
msgName = "WM_RENDERALLFORMATS";break;            case 0x0306
msgName = "WM_DESTROYCLIPBOARD";break;            case 0x0307
msgName = "WM_DRAWCLIPBOARD";break;               case 0x0308
msgName = "WM_PAINTCLIPBOARD";break;              case 0x0309
msgName = "WM_VSCROLLCLIPBOARD";break;            case 0x030A
msgName = "WM_SIZECLIPBOARD";break;               case 0x030B
msgName = "WM_ASKCBFORMATNAME";break;             case 0x030C
msgName = "WM_CHANGECBCHAIN";break;               case 0x030D
msgName = "WM_HSCROLLCLIPBOARD";                  case 0x030E
msgName = "WM_QUERYNEWPALETTE";                    case 0x030F
msgName = "WM_PALETTEISCHANGING";break;            case 0x0310
msgName = "WM_PALETTECHANGED";break;               case 0x0311
msgName = "WM_HOTKEY";break;                       case 0x0312
msgName = "WM_PRINT";break;                        case 0x0317
msgName = "WM_PRINTCLIENT";break;                  case 0x0318
msgName = "WM_HANDHELDFIRST";break;                case 0x0358
msgName = "WM_HANDHELDLAST";break;                 case 0x035F
msgName = "WM_AFXFIRST";break;                     case 0x0360
msgName = "WM_AFXLAST";break;                      case 0x037F
msgName = "WM_PENWINFIRST";break;                  case 0x0380
msgName = "WM_PENWINLAST";break;                   case 0x038F
msgName = "WM_APP";break;                          case 0x8000
*/