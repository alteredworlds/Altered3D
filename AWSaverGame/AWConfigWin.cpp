#include "AW3DTools/AWToolsCommon.h"
#include "AW3DTools/AWScene.h"
#include "AWConfigWin.h"
#include "AWWinUtils.h"
#include "AWSerialize/AWVerify.h"
#include <algorithm>

#define DISPLAYKEYNAME              "Display"
#define DATE_KEY_LOCATION           "AlteredWorld.1\\CLSID"

#define EXPORTERKEYNAME             "Software\\AlteredWorlds\\Exporter"

#define SHAREDKEYNAME               "Software\\AlteredWorlds\\Shared"

#define REGSTR_PATH_CONFIG          "Software\\AlteredWorlds\\Saver"
#define AWUSERKEY                   "Software\\AlteredWorlds\\AlteredSaver v3.2!\\3.20.000"



AWConfigWin::AWConfigWin(AWMode isSaver) : AWConfig(isSaver), m_showMakeExe(FALSE)
{
}


AWConfigWin::~AWConfigWin()
{
}



void      
AWConfigWin::parseCommandLine(const AWCString& cmdLine, AWBoolean forceConfig)
{
   //parse command line to determine:
   //  mode of operation, which can be:
   //  (a) GAME - scene name will be supplied as sole param
   //  (b) SAVER - regular, full screen mode ' /s' param
   //  (c) SAVER - PREVIEW mode - '/p windowID'
   //::MessageBox(NULL, cmdLine.c_str(), "Command Line is:", MB_OK);
   m_cmdLineSceneName = "";
   string::size_type saverPos   = cmdLine.find("/s");
   if (string::npos == saverPos)
   {
      saverPos   = cmdLine.find("/S");
   }
   string::size_type cfgPos     = cmdLine.find("/c");
   if (string::npos == cfgPos)
   {
      cfgPos   = cmdLine.find("/C");
   }
   string::size_type previewPos = cmdLine.find("/p");
   if (string::npos == previewPos)
   {
      previewPos   = cmdLine.find("/P");
   }
   if (string::npos != saverPos)
   {  //executing as SCREEN SAVER, FULL SCREEN
      setIsSaver(awSaver);
   }
   else
   {  //could be game, config or saver preview
      AWCString windowIDStr;
      if (string::npos != cfgPos)
      {  //need to grab the parent WindowID, if given       
         windowIDStr.eqSubString(cmdLine, cfgPos+2);
         m_winID = (long)(HWND)atoi(windowIDStr.c_str());
         setIsSaver(awConfig);
      }
      else if (string::npos != previewPos)
      {  //executing as SCREEN SAVER, PREVIEW mode
         //need to grab the parent WindowID, if given
         AWCString windowIDStr;
         windowIDStr.eqSubString(cmdLine, previewPos+2);
         m_winID = (long)(HWND)atoi(windowIDStr.c_str());
         setIsSaver(awPreview);
      }//if (string::npos != pos)
      else if ((string::npos == cfgPos) && cmdLine.length())
      {  //GAME mode - we should hopefully have a scene
         //name supplied as the sole parameter
         setIsSaver(awGame);
         m_cmdLineSceneName = cmdLine;
      }// else if ((string::npos == cfgPos) && cmdLine.length())
      else 
      {  //no recogniseable command line.
         //for a SAVER this implies we must show the config dialog
         //otherwise run as a saver (!)   
         if (!forceConfig)
         {
            m_cmdLineSceneName = "rubbish";
         }
         setIsSaver(forceConfig ? awConfig : awGame);         
      }//else
   }//else
}//void AWConfigWin::parseCommandLine(const AWCString& cmdLine, AWBoolean forceSaver)



void 
AWConfigWin::loadConfig()
{
   m_cmdLineSceneName = "";
   loadSharedInfo();
   //now load main body of config info
   char  strData[300];
   DWORD bufSize;
   LONG res; HKEY skey; DWORD valtype, val;

   m_level        = 0;

   res=RegOpenKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_CONFIG,0,KEY_ALL_ACCESS,&skey);
   if (res!=ERROR_SUCCESS) return;

   bufSize=299;
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "scene", NULL, &valtype, (BYTE*)strData, &bufSize))
   {	//should be REG_SZ
      m_scene = (const char*)strData;
   }

   strData[0]=0;
   bufSize=299;
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "waveFile", NULL, &valtype, (BYTE*)strData, &bufSize))
   {
      m_waveFile = (const char*)strData;
   }

   strData[0]=0;
   bufSize=299;
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "help", NULL, &valtype, (BYTE*)strData, &bufSize))
   {
      m_help = (const char*)strData;
   }

   strData[0]=0;
   bufSize=299;
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "cdrom", NULL, &valtype, (BYTE*)&strData, &bufSize))
   {
      m_cdrom = (const char*)strData;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "switch", NULL, &valtype, (BYTE*)&val, &bufSize))
   {	//should be REG_DWORD
      m_specifyMode = (AWBoolean)val;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "playWave", NULL, &valtype, (BYTE*)&val, &bufSize))
   {
      m_playSound = val;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "autoSwitchScenes", NULL, &valtype, (BYTE*)&val, &bufSize))
   {
      m_autoSwitchScenes = (AWBoolean)val;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "switchScenesMinutes", NULL, &valtype, (BYTE*)&val, &bufSize))
   {
      m_sceneSwitchMinutes = (int)val;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "curTab", NULL, &valtype, (BYTE*)&val, &bufSize ))
   {
      m_lastTabViewed = (int)val;
   }

   //get level options
   bufSize=sizeof(strData);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "game", NULL, &valtype, (BYTE*)strData, &bufSize))
   {	//should be REG_SZ
      m_game = (const char*)strData;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "level", NULL, &valtype, 
                   (BYTE*)&val, &bufSize ))
   {//should be a DWORD value
      m_level = (int)val;
   }

   if (ERROR_SUCCESS == RegQueryValueEx(skey, "useLevel", NULL, &valtype, 
                   (BYTE*)&val, &bufSize ))
   {//should be a DWORD value
      m_useLevel = (int)val;
   }

   if (ERROR_SUCCESS == RegQueryValueEx(skey, "playMusic", NULL, &valtype, 
                   (BYTE*)&val, &bufSize ))
   {//should be a DWORD value
      m_playMusic = (int)val;
   }

   if (ERROR_SUCCESS == RegQueryValueEx(skey, "playEffects", NULL, &valtype, 
                   (BYTE*)&val, &bufSize ))
   {//should be a DWORD value
      m_playEffects = (int)val;
   }

   if (ERROR_SUCCESS == RegQueryValueEx(skey, "joyStick", NULL, &valtype, 
                   (BYTE*)&val, &bufSize ))
   {//should be a DWORD value
      m_userInputMethod = (int)val;
   }

   loadDisplayConfig(skey);
   RegCloseKey(skey);
   
   if (m_sceneSwitchMinutes < 1) //make sure we have minimum value of 1
   {  
      m_sceneSwitchMinutes = 1;
   }
}//void AWConfigWin::loadConfig()




void 
AWConfigWin::saveConfig()
{
   LONG res; HKEY skey; DWORD val, disp;
   res=RegCreateKeyEx(HKEY_CURRENT_USER, REGSTR_PATH_CONFIG, 0, NULL,
                      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &skey, &disp);
   if (res!=ERROR_SUCCESS) return;

   RegSetValueEx(skey, "scene", 0, REG_SZ, (CONST BYTE*)m_scene.c_str(), m_scene.length());
   RegSetValueEx(skey, "waveFile", 0, REG_SZ, (CONST BYTE*)m_waveFile.c_str(), m_waveFile.length());
   RegSetValueEx(skey, "help", 0, REG_SZ, (CONST BYTE*)m_help.c_str(), m_help.length());
   RegSetValueEx(skey, "cdrom", 0, REG_SZ, (CONST BYTE*)m_cdrom.c_str(), m_cdrom.length());

   val=m_specifyMode;
   RegSetValueEx(skey, "switch", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

   val=m_playSound;
   RegSetValueEx(skey, "playWave", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

   val=m_autoSwitchScenes;
   RegSetValueEx(skey, "autoSwitchScenes", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

   val=(int)m_sceneSwitchMinutes;
   RegSetValueEx(skey, "switchScenesMinutes", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

   val=m_lastTabViewed;
   RegSetValueEx(skey, "curTab", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   val=m_level;
   RegSetValueEx( skey, "level", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   val=m_useLevel;
   RegSetValueEx( skey, "useLevel", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   val=m_playMusic;
   RegSetValueEx( skey, "playMusic", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   val=m_playEffects;
   RegSetValueEx( skey, "playEffects", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   val=m_userInputMethod;
   RegSetValueEx( skey, "joyStick", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   saveDisplayConfig(skey);
   RegCloseKey(skey);
   saveSharedInfo();
}//void AWConfigWin::saveConfig()




AWBoolean 
AWConfigWin::isFullScreen()
{  //we're fullscreen iff:
   //(i)  fullscreen explicity specified, OR 
   //(ii) we're told to use current screen settings (m_specifyMode == FALSE)
   //(iii)the settings we're told to use match full screen 
   AWBoolean retVal = isSaver() || m_fullScreen || !m_specifyMode ||
                     (m_specifyMode && ( (m_width == AWWinUtils::m_width) && 
                                               (m_height == AWWinUtils::m_height) ) );
   return retVal;
}//AWBoolean AWConfigWin::isFullScreen()



void
AWConfigWin::updateDisplayInfo()
{  //make sure we've queried the system's current settings
   AWWinUtils::getUserMode();
   if (isFullScreen() && !m_specifyMode)
   {  //we're using user's current display settings.
      //copy them into config values.
      m_width  = AWWinUtils::m_width; 
      m_height = AWWinUtils::m_height;
      m_bpp    = AWWinUtils::m_bpp;
   }//if (isFullScreen() && !m_specifyMode)
}//void AWConfigWin::updateDisplayInfo()



int
AWConfigWin::valiDate()
{  //get stored date & number of runs remaining - compare with now
   AWTRACE("Validating Date\n");
   int      retVal = 0;
   time_t   storedTime=0;;
   int      daysLeft=0;
   char     strData[300];
   DWORD    valtype, bufSize; 
   HKEY     skey;
   //GET date & use info from primary location
   LONG     res = RegOpenKeyEx(HKEY_CLASSES_ROOT, DATE_KEY_LOCATION, 0, 
                               KEY_QUERY_VALUE|KEY_SET_VALUE, &skey);
   if (res==ERROR_SUCCESS)
   {  //found the registry key - now get the data
      AWBoolean carryOn = FALSE;
      bufSize = 299;
      if (ERROR_SUCCESS == RegQueryValueEx(skey, "", 
                                           NULL, &valtype, (BYTE*)strData, 
                                           &bufSize) && (bufSize==39))
      {	//should be CLSID format string 38 chars long + term, for example
         //{98A797B7-4CEC-061c-00*1e-0010*A4FF1FE7}
         //our stupidly simple system puts the unencrypted info here
         //time_t is the last 8 chars [a long => 4 bytes => 8 nibbles]
         //daysLeft = 23, 24 th chars
         char* stopScan  = NULL;
         char* dataStart = (char*)strData;
         dataStart += 22;
         daysLeft   = (int)strtol(dataStart, &stopScan, 16);
         dataStart += 7;
         storedTime = strtol(dataStart, &stopScan, 16);
         carryOn = TRUE;
         AWTRACE2(" Stored values: %d days left, storeTime %ld\n", daysLeft, storedTime);
      }
      //we have the info stored in the registry. compare with
      //current system time for valid PASS/FAIL
      if (carryOn && (carryOn = AWWinUtils::verifyDateInfo(storedTime, daysLeft)))
      {  //PASSED THE TEST - perform yuk conversion to fake CLSID format
         //need to store this again...insert into strData
         //so we want storedTime as an 8 char (hex) string
         //and daysLeft as a 2 char (hex) string
         char temp[10];
         char* dest = ((char*)strData) + 22;

         sprintf(temp, "%2x", daysLeft);
         *dest++ = (temp[0]==' ') ? '0' : temp[0];
         *dest   = (temp[1]==' ') ? '0' : temp[1];

         dest += 6;
         sprintf(temp, "%8lx", storedTime);
         int i =0;        
         while (i<8)
         {
            if (temp[i] == ' ')
            {
               dest[i] = '0';
            }
            else
            {
               dest[i] = temp[i];
            }
            i++;
         }//while (i<8) 
         //STORE NEWLY ADJUSTED TIMEOUT DETAILS
         if (ERROR_SUCCESS == RegSetValueEx(skey, "", 0, REG_SZ, 
                                            (CONST BYTE*)strData,  bufSize) )
         {
            AWTRACE2(" Saved values: %d days left, storeTime %ld\n", daysLeft, storedTime);
            retVal = daysLeft;
         }
#define AWALTERNATE "CLSID\\{5BE0F8C1-4DA5-11d4-8CE0-0010A4FF1FE7}"
         //also write this value to alternate location which will not be
         //removed by the uninstall program. Allows for prevention of
         //install, or retention of previous timeout value on install
         HKEY  alternateLocation;
         DWORD disp;
         if (ERROR_SUCCESS==RegCreateKeyEx(HKEY_CLASSES_ROOT, AWALTERNATE, 0, NULL,
                                          REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 
                                          NULL, &alternateLocation, &disp))
         {
            if (ERROR_SUCCESS == RegSetValueEx(alternateLocation, "", 0, REG_SZ, 
                                            (CONST BYTE*)strData,  bufSize) )
            {
               AWTRACE2(" Saved Alternates: %d days left, storeTime %ld\n", daysLeft, storedTime);
            }
            RegCloseKey(alternateLocation);
         }
      }
      RegCloseKey(skey);
   }//if (res==ERROR_SUCCESS)
   return retVal;
}//int AWConfigWin::valiDate()



void      
AWConfigWin::saveUserInfo()
{  //SAVE USER INFO TO REGISTRY
   HKEY  skey; 
   DWORD disp;
   LONG  res=RegCreateKeyEx(HKEY_LOCAL_MACHINE, AWUSERKEY, 0, NULL,
                      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &skey, &disp);
   if (res!=ERROR_SUCCESS) return;

   RegSetValueEx(skey, "user", 0, REG_SZ, (CONST BYTE*)m_user.c_str(), m_user.length());
   RegSetValueEx(skey, "email", 0, REG_SZ, (CONST BYTE*)m_email.c_str(), m_email.length());
   RegSetValueEx(skey, "delivery", 0, REG_SZ, (CONST BYTE*)m_delivery.c_str(), m_delivery.length());
   RegSetValueEx(skey, "serial", 0, REG_SZ, (CONST BYTE*)m_serial.c_str(), m_serial.length());

   RegCloseKey(skey);
}//void AWConfigWin::saveUserInfo()



void 
AWConfigWin::loadUserInfo()
{ 
//initialise values
   m_validUser  = FALSE;
   m_registered = FALSE;
//FIRST STAGE - validate the executable via (homebrew) checksum
   m_validUser = AWWinUtils::verifyBinary(m_exePath); 
   AWTRACE1("CRC Validation %d\n", (int)m_validUser);
   if(!m_validUser) return;
//NOW - validate stored user info  => matching IFF
//(i)  FULL VERSION
//(ii) REGISTERED SHAREWARE VERSION
//
//LOAD FROM REGISTRY
   char  strData[300];
   DWORD bufSize;
	LONG  res; HKEY skey; DWORD valtype;
   res=RegOpenKeyEx(HKEY_LOCAL_MACHINE, AWUSERKEY, 0, KEY_QUERY_VALUE, &skey);
   if (ERROR_SUCCESS==res)
   {
      bufSize=299;
      if (ERROR_SUCCESS == RegQueryValueEx(skey, "user", NULL, &valtype, (BYTE*)strData, &bufSize))
      {	//should be REG_SZ
         m_user = (const char*)strData;
      }
      if (res!=ERROR_SUCCESS) return;
      bufSize=299;
      if (ERROR_SUCCESS == RegQueryValueEx(skey, "email", NULL, &valtype, (BYTE*)strData, &bufSize))
      {	//should be REG_SZ
         m_email = (const char*)strData;
      }
      if (res!=ERROR_SUCCESS) return;
      bufSize=299;
      if (ERROR_SUCCESS == RegQueryValueEx(skey, "delivery", NULL, &valtype, (BYTE*)strData, &bufSize))
      {	//should be REG_SZ
         m_delivery = (const char*)strData;
      }
      if (res!=ERROR_SUCCESS) return;
      bufSize=299;
      if (ERROR_SUCCESS == RegQueryValueEx(skey, "serial", NULL, &valtype, (BYTE*)strData, &bufSize))
      {	//should be REG_SZ
         m_serial = (const char*)strData;
      }
      RegCloseKey(skey);
   }//if (ERROR_SUCCESS==res)
//NOW VALIDATE THE RETRIEVED INFORMATION
#ifdef _AWDISABLE_USER_VALIDATION
   m_registered = TRUE;
#else
   m_registered = validate(m_user.c_str(), m_email.c_str(), m_delivery.c_str(), m_serial.c_str());
#endif

   if (!m_registered)
   {  //if user still invalid, check for n day trial
      m_validUser = valiDate();
      AWTRACE1("Not registered - validate by DATE, giving %i\n", (int)m_validUser);     
   }//if (!m_validUser)
}//void  AWConfigWin::loadUserInfo()



void      
AWConfigWin::loadSharedInfo()
{
   DWORD bufSize;
   DWORD valType;
   DWORD val;
   LONG  res; 
   HKEY  skey; 
   
   res=RegOpenKeyEx(HKEY_CURRENT_USER, SHAREDKEYNAME, 0, KEY_ALL_ACCESS, &skey);
   if (res!=ERROR_SUCCESS) return;

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "hasBump", NULL, &valType, 
                               (BYTE*)&val, &bufSize ))
   {  //should be a DWORD value
      m_bumpAvailable = (int)val;
   }

   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "hasCubeEnv", NULL, &valType, 
                               (BYTE*)&val, &bufSize ))
   {  //should be a DWORD value
      m_cubeEnvAvailable = (int)val;
   }
   bufSize = sizeof(val);
   if (ERROR_SUCCESS == RegQueryValueEx(skey, "hasS3Compress", NULL, &valType, 
                               (BYTE*)&val, &bufSize ))
   {  //should be a DWORD value
      m_texCompressAvailable = (int)val;
   }
   RegCloseKey(skey);
}//void AWConfigWin::loadSharedInfo()



void      
AWConfigWin::saveSharedInfo()
{
   HKEY skey; 
   DWORD val, disp;
   LONG res=RegCreateKeyEx(HKEY_CURRENT_USER, SHAREDKEYNAME, 0, NULL,
                      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &skey, &disp);
   if (res!=ERROR_SUCCESS) return;

   val=m_bumpAvailable;
   RegSetValueEx(skey, "hasBump", 0, REG_DWORD, (const BYTE*)&val, 
                  sizeof(val));

   val=m_cubeEnvAvailable;
   RegSetValueEx(skey, "hasCubeEnv", 0, REG_DWORD, (const BYTE*)&val, 
                  sizeof(val));

   val=m_texCompressAvailable;
   RegSetValueEx(skey, "hasS3Compress", 0, REG_DWORD, (const BYTE*)&val, 
                  sizeof(val));
   RegCloseKey(skey);
}//void AWConfigWin::saveSharedInfo()



void      
AWConfigWin::saveLevel()
{
   LONG res; HKEY skey; DWORD val, disp;
   res=RegCreateKeyEx(HKEY_CURRENT_USER, REGSTR_PATH_CONFIG, 0, NULL,
                      REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &skey, &disp);
   if (res!=ERROR_SUCCESS) return;

   val=m_level;
   RegSetValueEx( skey, "level", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));

   RegCloseKey(skey);
}//void AWConfigWin::saveLevel()



void 
AWConfigWin::loadExporterInfo()
{
   HRESULT keyOpenVal;
   HKEY    exporter;
   m_showMakeExe = FALSE;
   keyOpenVal = RegOpenKeyEx(HKEY_CURRENT_USER, EXPORTERKEYNAME, 0, KEY_ALL_ACCESS, &exporter);
	if (ERROR_SUCCESS == keyOpenVal)
	{
      DWORD valtype, val;
      char  strData[300];
      DWORD bufSize=299;
      
      strData[0]=0;
      if (ERROR_SUCCESS == RegQueryValueEx(exporter, "help", NULL, &valtype, (BYTE*)strData, &bufSize))
      {
         m_exportHelp = (const char*)strData;
      }

      bufSize = sizeof(val);
      if (ERROR_SUCCESS == RegQueryValueEx(exporter, "makeEXE", NULL, &valtype, 
                      (BYTE*)&val, &bufSize ))
      {  
         m_showMakeExe = (int)val;
      }
   }//if (ERROR_SUCCESS == keyOpenVal)
}//void AWConfigWin::loadExporterInfo()



void 
AWConfigWin::saveExporterInfo()
{
   DWORD   disp;
	HKEY    exporter;
   HRESULT keyOpenVal;
   keyOpenVal = RegCreateKeyEx(HKEY_CURRENT_USER, EXPORTERKEYNAME, 0, "REG_DWORD", REG_OPTION_NON_VOLATILE,
									    KEY_ALL_ACCESS, NULL, &exporter, &disp);
	if (ERROR_SUCCESS == keyOpenVal)
   {  //Exporter options
      RegSetValueEx(exporter, "help", 0, REG_SZ, (CONST BYTE*)m_exportHelp.c_str(), m_exportHelp.length());
   }//if (ERROR_SUCCESS == keyOpenVal)
}//void AWConfigWin::saveExporterInfo()



void
AWConfigWin::loadDisplayConfig(HKEY base)
{  //gameWidth, gameHeight, gamePixelDepth, fullScreen
	//if registry key is present, load the contents
	//otherwise assign default values
	HKEY display;
	BYTE data[200];
   HRESULT keyOpenVal;
   keyOpenVal = RegOpenKeyEx(base, DISPLAYKEYNAME, 0, KEY_ALL_ACCESS, &display);
	if (ERROR_SUCCESS == keyOpenVal)
	{
	//get display options
		DWORD valueType;
		DWORD bufSize = 200;
		//lazy code assumes all keys are correctly of type REG_DWORD
		if (ERROR_SUCCESS == RegQueryValueEx(display, "fullScreen",  NULL, 
							&valueType, data, &bufSize ))
		{//we've got the fullscreen setting
			m_fullScreen = (int)(*(DWORD*)data);
		}
      bufSize = 200;
		if (ERROR_SUCCESS == RegQueryValueEx(display, "bitsPerPixel",  NULL, 
							&valueType, data, &bufSize ))
		{//we've got the fullscreen setting
			m_bpp = (int)(*(DWORD*)data);
		}
      bufSize = 200;
		if (ERROR_SUCCESS == RegQueryValueEx(display, "width",  NULL, 
							&valueType, data, &bufSize ))
		{//we've got the fullscreen setting
			m_width = (int)(*(DWORD*)data);
		}
      bufSize = 200;
		if (ERROR_SUCCESS == RegQueryValueEx(display, "height",  NULL, 
							&valueType, data, &bufSize ))
		{//we've got the fullscreen setting
			m_height = (int)(*(DWORD*)data);
		}
		bufSize = 199;
		if (ERROR_SUCCESS == RegQueryValueEx(display, "snapshotDir",  NULL, 
							&valueType, data, &bufSize ))
		{	//ANSI chars - CAREFUL should really be using UNICODE throughout
			m_snapshots = (const char*)data;
		}
      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "flipY", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_flipY = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useDisplayLists", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useDisplayLists = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useVertexArrays", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useVertexArrays = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useCVAEXT", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useCVAs = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useBump", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useBump = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useCubeEnv", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useCubeEnv = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useBumpSpec", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useBumpSpecular = (int)(*(DWORD*)data);
      }

      bufSize = 200;
      if (ERROR_SUCCESS == RegQueryValueEx(display, "useTexCompress", NULL, &valueType, 
                                  (BYTE*)&data, &bufSize ))
      {//should be a DWORD value
         m_useTexCompress = (int)(*(DWORD*)data);
      }
		//close display settings key
		RegCloseKey(display);
	}//if (ERROR_SUCCESS == keyOpenVal)
}//void AWConfigWin::loadDisplayConfig(HKEY base)



void
AWConfigWin::saveDisplayConfig(HKEY base)
{
	DWORD   disp;
	HKEY    display;
   HRESULT keyOpenVal;
   keyOpenVal = RegCreateKeyEx(base, DISPLAYKEYNAME, 0, "REG_DWORD", REG_OPTION_NON_VOLATILE,
									    KEY_ALL_ACCESS, NULL, &display, &disp);
	if (ERROR_SUCCESS == keyOpenVal)
   {  //get display options
      DWORD value;
      value=m_fullScreen;
      RegSetValueEx( display, "fullScreen", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));

      value=m_bpp;
      RegSetValueEx( display, "bitsPerPixel", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));

      value=m_width;
      RegSetValueEx( display, "width", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));

      value=m_height;
      RegSetValueEx( display, "height", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));

      //ANSI chars - CAREFUL should really be using UNICODE throughout
      RegSetValueEx(display, "snapshotDir", 0, REG_SZ, 
							      (const BYTE*)m_snapshots.c_str(), m_snapshots.length());
      value=m_flipY;
      RegSetValueEx(display, "flipY", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value));

      value=m_useDisplayLists; 
      RegSetValueEx(display, "useDisplayLists", 0, REG_DWORD, (const BYTE*)&value,
                     sizeof(value));

      value=m_useVertexArrays; 
      RegSetValueEx(display, "useVertexArrays", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value));

      value=m_useCVAs; 
      RegSetValueEx(display, "useCVAEXT", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value));

      value=m_useBump;
      RegSetValueEx(display, "useBump", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value));

      value=m_useCubeEnv;
      RegSetValueEx(display, "useCubeEnv", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value));

      value=m_useBumpSpecular;
      RegSetValueEx(display, "useBumpSpec", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value)); 
      
      value=m_useTexCompress;
      RegSetValueEx(display, "useTexCompress", 0, REG_DWORD, (const BYTE*)&value, 
                     sizeof(value)); 
      
      //close display settings key
      RegCloseKey(display);
	}//if (ERROR_SUCCESS == keyOpenVal)
}//AWBoolean AWConfigWin::saveDisplayConfig(HKEY base)



//scene list management functions
int
AWConfigWin::getNumScenes()
{
   return m_sceneList.size();
}//int AWConfigWin::getNumScenes()



AWCString   	
AWConfigWin::getScene(int index)
{
   AWCString scene;
   if ((index >= 0) && (index < getNumScenes()))
   {
      scene = m_sceneList.at(index);
   }
   return scene;
}//AWCString AWConfigWin::getScene(int index)



void
AWConfigWin::buildSceneList(const AWCString& scene)
{  //idea is to extract the scene directory from this parameter
   //parameter can be either:
   //(i) full path to a given scene file which user wants to show first  - other scenes in same dir
   //(ii)scenes directory where all scenes can be found
   AWCString useScene(scene);
   if (!useScene.length()) useScene = m_scene;
   AWCString dir(AWScene::getSceneDirectory(useScene));

	//now we should have the directory
	AWCString searchFor(dir);   
   searchFor += "*.tnt";
   AWTRACE1("searching directory %s\n", searchFor.c_str());

   WIN32_FIND_DATA retData;
	HANDLE			fHandle;
   AWCString      listName;

   m_sceneList.clear();
	fHandle = FindFirstFile(searchFor.c_str(), &retData);	
	if (INVALID_HANDLE_VALUE != fHandle)
	{
      int ind        = 0;
      m_sceneIndex   = 0;
      do
      {
         listName = dir;
         listName += retData.cFileName;
         m_sceneList.push_back(listName);	
         if (listName.contains("goSpaceGnats.tnt")||listName.contains("GOSPACEGNATS.TNT"))
         {  //this is our single saver game...
            //grab the info IFF we don't already have a valid game
            //path loaded from the registry
            if (!m_game.length() || !AWFile::Exists(m_game))
            {
               AWTRACE1("Setting current SaverGame to %s\n", listName.c_str());
               m_game = listName;
            }//if (!m_game.length() || !AWFile::Exists(m_game))
         }//if (listName.contains("goSpaceGnats.tnt")||listName.contains("GOSPACEGNATS.TNT"))
         ind++;
      }
      while(FindNextFile(fHandle, &retData));
		FindClose(fHandle);

      AWTRACE1("Found %i scenes\n", (int)m_sceneList.size());
      //we should now SORT THE LIST OF SCENES in alphabetical order
      sort(m_sceneList.begin(), m_sceneList.end());
      //and find the selected scene index
      AWCString matchName(AWScene::getSceneName(useScene));
      for (ind = 0; ind < m_sceneList.size(); ind++)
      {
         listName = m_sceneList.at(ind);
         AWTRACE1(" adding AlteredWorld: %s\n", listName.c_str());
         if (matchName == AWScene::getSceneName(listName))
         {
            AWTRACE2(" selected scene %s at index: %i\n", listName.c_str(), ind);
            m_sceneIndex=ind;
         }//if (matchName == AWScene::getSceneName(listName))
      }//for (indx = 0; indx < m_sceneList.size(); indx++)      
	}//if (INVALID_HANDLE_VALUE != fHandle)
   m_autoSwitchScenes &= (getNumScenes() > 1);
}//void AWConfigWin::buildSceneList(const AWCString& scene)



void
AWConfigWin::setScene(const AWCString& name)
{
   m_scene = name;
   m_sceneIndex = 0;
   for (int i=0; i < getNumScenes(); i++)
   {
      //NOTE - this is not right. we want to make sure it's an
      //exact match, rather than just CONTAINING the file name.
      string::size_type pos = m_sceneList.at(i).find(name);
      if (string::npos != pos) 
      {
         m_sceneIndex=i;
         break;
      }//if (string::npos != pos) 
   }//for (int i=0; i < m_globBuf.gl_pathc; i++)
}//void AWConfigWin::setSceneIndex(const AWCString& name)