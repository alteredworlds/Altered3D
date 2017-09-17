#include <stdio.h>
#include "AWConfigLin.h"
#include <stdlib.h>
//#include <fstream.h>
#include <unistd.h>
#include <signal.h>
#include <sys/file.h>
//#include <rw/rwfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <fstream>

extern int errno;

#define AW_APPNAME             "AlteredSaver"

#define AW_SCENENAME_TAG       "scene"
#define AW_SNAPSHOT_TAG        "snapshots"
#define AW_HELP_TAG            "help"
#define AW_LEVEL_TAG           "level"
#define AW_FULLSCREEN_TAG      "fullScreen"
#define AW_USELEVEL_TAG        "useLevel"
#define AW_HEIGHT_TAG          "height"
#define AW_WIDTH_TAG           "width"
#define AW_BPP_TAG             "bpp"
#define AW_FLIPY_TAG           "flipY"
#define AW_AUTOSWITCH_TAG      "autoSwitchScenes"
#define AW_SWITCHMINS_TAG      "switchMinutes"
#define AW_USERINPUTMETHOD_TAG "userInputMethod"
#define AW_PLAYSOUNDTYPE_TAG   "playSound"
#define AW_PLAYGAMEEFFECTS_TAG "playEffects"
#define AW_CDROMDRIVE_TAG      "audiocd"
#define AW_LASTTABVIEWED_TAG   "lasttab"
#define AW_SOUNDFILE_TAG       "soundFile"
#define AW_GAME_TAG            "game"
#define AW_HTTPPROXY_TAG       "httpProxy"
#define AW_DISPLAYLISTS_TAG    "displayLists"
#define AW_CVA_TAG             "cVertArrays"


#define AW_CONFIG_HOME         "/.AlteredSaver2/"
#define AW_CONFIG_FILENAME     ".AlteredSaver2.cfg"

//not persisted - internal use
#define AW_REMOVE_EXT_TAG      "AWREMOVE"

//need replacing
// Parse a command line buffer into arguments
int
AWConfigLin::parseCommandLine(char* cmdline, char** argv)
{
	char* bufp;
	int argc;

	argc = 0;
	for ( bufp = cmdline; *bufp; ) {
		// Skip leading whitespace
		while ( isspace(*bufp) ) {
			++bufp;
		}
		// Skip over argument
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			// Skip over word
			while ( *bufp && (*bufp != '"') ) {
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			//Skip over word
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return argc;
}//int AWConfgLin::parseCommandLine(char *cmdline, char **argv)



AWConfigLin::AWConfigLin(AWMode isSaver) : AWConfig(isSaver)
{  //Linux specific default values should be set here
   m_home          = AWCString(getenv("HOME"));
   m_home          +=  AWCString(AW_CONFIG_HOME);
   m_cfgFileName   = m_home;
   m_cfgFileName   += AWCString(AW_CONFIG_FILENAME);
   m_validUser     = TRUE;
   m_lastTabViewed = 0;
   m_appLock       = NULL;
   m_playerPID     = 0;
   m_viewerPID     = 0;
   memset(&m_globBuf, 0, sizeof(m_globBuf));  //make sure it's zeroed
}//AWConfigLin::AWConfigLin(AWBoolean isSaver) : AWConfig(isSaver)


AWConfigLin::~AWConfigLin()
{
   if (0 < m_globBuf.gl_pathc)
   {  //this memory was allocated via 'glob' and should be freed in same way
      globfree(&m_globBuf);
   }//if (0 < m_globBuf.gl_pathc)
}//AWConfigLin::~AWConfigLin()


AWBoolean
AWConfigLin::getValueForName(const AWCString& name, const AWCString& val)
{
/*
   AWBoolean           m_specifyMode;AWCString      m_cdrom;
   //original meaning of 'playWave' registry key extended
   // 0 => no sounds
   // 1 => play looped wave   DOES NOT CURRENTLY APPLY FOR LINUX VERSION  1 => 2
   // 2 => launch helper app
   // 3 => play audio CD
   int			   m_playSound;AWCString	   m_waveFile;int            m_playMusic;
   int            m_playEffects;AWCString      m_runApp;
   */
   AWBoolean retVal = TRUE;
   //AWTRACE2("entered getValueForName(%s, %s)\n", name.c_str(), val.c_str());
   if (AW_SCENENAME_TAG == name)
   {
      m_scene = val.c_str();
   }
   else if (AW_GAME_TAG == name)
   {
      m_game = val.c_str();
   }
   else if (AW_SNAPSHOT_TAG == name)
   {
      m_snapshots = val.c_str();
   }
   else if (AW_HELP_TAG == name)
   {
      m_help = val.c_str();
   }
   else if (AW_LEVEL_TAG == name)
   {
      m_level = atoi(val.c_str());
   }
   else if (AW_FULLSCREEN_TAG == name)
   {
      m_fullScreen = (AWBoolean)atoi(val.c_str());
   }
   else if (AW_USELEVEL_TAG == name)
   {
      m_useLevel = atoi(val.c_str());
   }
   else if (AW_WIDTH_TAG == name)
   {
      m_width = atoi(val.c_str());
   }
   else if (AW_HEIGHT_TAG == name)
   {
      m_height = atoi(val.c_str());
   }
   else if (AW_BPP_TAG == name)
   {
      m_bpp = atoi(val.c_str());
   }
   else if (AW_FLIPY_TAG == name)
   {
      m_flipY = (AWBoolean)atoi(val.c_str());
   }
   else if (AW_AUTOSWITCH_TAG == name)
   {
      m_autoSwitchScenes = (AWBoolean)atoi(val.c_str());
   }
   else if (AW_SWITCHMINS_TAG == name)
   {
      m_sceneSwitchMinutes = atoi(val.c_str());
   }
   else if (AW_USERINPUTMETHOD_TAG == name)
   {
      m_userInputMethod = atoi(val.c_str());
   }
   else if (AW_PLAYSOUNDTYPE_TAG == name)
   {
      m_playSound = atoi(val.c_str());
   }
   else if (AW_PLAYGAMEEFFECTS_TAG == name)
   {
      m_playEffects = atoi(val.c_str());
   }
   else if (AW_CDROMDRIVE_TAG == name)
   {
      m_cdrom = val;
   }
   else if (AW_LASTTABVIEWED_TAG == name)
   {
      setLastTab(atoi(val.c_str()));
   }
   else if (AW_SOUNDFILE_TAG == name)
   {
      m_waveFile = val;
   }
   else if (AW_HTTPPROXY_TAG == name)
   {
      m_httpProxy = val;
   }
   else if (AW_CVA_TAG == name)
   {
      m_useVertexArrays = (AWBoolean)atoi(val.c_str());
   }
   else if (AW_DISPLAYLISTS_TAG == name)
   {
      m_useDisplayLists = (AWBoolean)atoi(val.c_str());
   }
   else retVal = FALSE;
   return retVal;
}//void AWConfigLin::getValueForName()



AWBoolean
AWConfigLin::parseLineFor(const AWCString& line, const AWCString& name)
{  //looks for name in line, and if found grabs the value
   //format should be
   //variable_name=value
   AWBoolean retVal = FALSE;

   AWTRACE2("entered parseLineFor(%s, %s)\n", line.c_str(), name.c_str());
   string::size_type namePos = line.find(name);
   string::size_type eqPos   = line.find("=");
   if ((string::npos != namePos) && (string::npos != eqPos) && (namePos < eqPos))
   {  //there should be an equals sign after the variable name
      //recognize only one variable per line
      //fprintf(stderr, "\t%s: ", name.c_str());
      retVal = TRUE;
      if (++eqPos < line.length())
      { //we should now have a value accessible
         AWCString val;
         val = line;
         val.replace(0, eqPos, "");
         getValueForName(name, val);
         AWTRACE2("loaded cfg name: %s  data: %s\n", name.c_str(), val.c_str());
      }
      //fprintf(stderr, "\n");
   }//if (string::npos != pos)
   return retVal;
}//AWBoolean  AWConfigLin::parseLineFor(const AWCString& name)



void
AWConfigLin::setHelperForExtension(const AWCString& ext, AWCString& helper)
{
   AWCString oldHelper;
   //fprintf(stderr, "extensions was: %s\n   ext is: %s  helper is %s\n", m_extensions.c_str(), ext.c_str(), helper.c_str());
   if (!helper.length()) helper = AW_REMOVE_EXT_TAG;
   if (!getHelperForExtension(ext, oldHelper, helper))
   {  //we haven't replaced, so do it now
      m_extensions += ext + AWCString("=") + helper + AWCString(";");
   }
   //fprintf(stderr, "extensions is now: %s\n", m_extensions.c_str());
}//void AWConfigLin::setHelperForExtension(const AWCString& ext, AWCString& helper)



AWBoolean
AWConfigLin::parseExtension(const AWCString& line)
{  //looks to see if this line contains an extension
   //and associated helper application
   //format example:
   //.mp3=plaympeg      no leading spaces here
   AWBoolean retVal = FALSE;
   AWCString newExtStr;
   //fprintf(stderr, "parse extension from: %s\n", line.c_str());
   if (line.length() && ('.' == line[0]))
   {  //equals sign delimits the extension (no whitespace)
      string::size_type pos = line.find("=");
      if ((string::npos != pos) && (++pos < line.length()))
      { //we should now have an extension AND value accessible
         m_extensions += line;
         m_extensions += ";";
      }
      //fprintf(stderr, "ext string: %s\n", m_extensions.c_str());
   }//if (string::npos != pos)
   return retVal;
}//AWBoolean  AWConfigLin::parseExtension(const AWCString& name)

AWBoolean
AWConfigLin::getHelperForExtension(const AWCString& ext, AWCString& helperStr, const AWCString& replaceStr)
{  //see if this extension is already handled
   //extension should be in form
   //.xxxxx
   //where there is at least one x (and less than 10, arbitrarily)
   AWBoolean retVal = FALSE;
   int len = ext.length();
   if ((len > 1) && (len < 12))
   {
      string::size_type endPosHelper = 0;
      string::size_type extPosStart  = m_extensions.find(ext);
      string::size_type extPosEnd    = extPosStart;
      if ( string::npos != extPosStart )
      {  //found the start of the extension. make sure it's a full match
         extPosEnd = m_extensions.find("=", extPosStart); //move past extension and =
         if (string::npos == extPosEnd) return retVal;
         AWCString foundExt;
         foundExt.eqSubString(m_extensions, extPosStart, extPosEnd);
         if (foundExt != ext) return retVal; //not an exact match
         if (string::npos != (endPosHelper = m_extensions.find(";", ++extPosEnd)) )
         {
            helperStr.eqSubString(m_extensions, extPosEnd, endPosHelper);
            if (replaceStr.length())
            {
               if (AW_REMOVE_EXT_TAG == replaceStr)
               { //remove this extension from the set
                  m_extensions.replace(extPosEnd, endPosHelper-extPosEnd, "");
               }
               else m_extensions.replace(extPosEnd, endPosHelper-extPosEnd, replaceStr);
            }
            if (helperStr.length()) retVal = TRUE;
         }//if (string::npos != (endPos = m_extensions.index(';', pos)) )
      }//if ( string::npos != pos )
   }//if ((len > 1) && (len < 12))
   return retVal;
}//AWBoolean AWConfigLin::isValidExtension(const AWCString& ext, AWCString& helper, const AWCString& replaceStr)


AWCString
AWConfigLin::getExtFromFile(const AWCString& fileName)
{
   AWCString ext(fileName);
   string::size_type pos = ext.rfind(".");
   if (string::npos != pos)
   {
      ext.replace(0, pos, "");
   }
   else ext = "";
   return ext;
}//AWCString AWConfigLin::getExtFromFile(const AWCString& fileName)


AWBoolean
AWConfigLin::getHelperForFile(const AWCString& fileName, AWCString& helperStr)
{
   AWBoolean retVal = FALSE;
   AWCString ext(getExtFromFile(fileName));
   if (ext.length())
   {
      retVal = getHelperForExtension(ext, helperStr);
   }
   return retVal;
}//AWBoolean AWConfigLin::getHelperForFile(const AWCString& fileName)





AWBoolean fileExists(const char* fName)
{
   AWBoolean retVal = FALSE;
   if (fName)
   {
      struct stat junkblah;
      retVal = 0 == lstat(fName, &junkblah);
   }
   return retVal;
}//AWBoolean fileExists(char* fName)


void
AWConfigLin::openViewer(const AWCString& htmlFile, const AWCString& ext)
{
   //playFileStart(m_viewerPID, htmlFile, ext);

   AWCString helper, file(htmlFile);
   if (ext.length())
   {
      getHelperForExtension(ext, helper);
   }
   else if (file.length())
   {
      getHelperForFile(file, helper);
   }
   if (!helper.length() || !file.length()) return;

   if (!(m_viewerPID = fork()))
   {  //this is the young child...
      const int len = 1024;
      char  temp[len];
      char* argv[5];
      //special case netscape to allow for remote opening
      AWCString lockName(getenv("HOME"));
      lockName += "/.netscape/lock";
      AWBoolean netscapeHack = helper.contains("netscape") &&
                              fileExists(lockName);
      if (netscapeHack)
      {
         snprintf(temp, len-1, "openURL(%s)", file.c_str());
         temp[len-1]=0; //just in case above call copied 499 w/o terminating
         argv[0] = (char*)helper.c_str();
         argv[1] = "-remote";
         argv[2] = temp;
         argv[3] = NULL;
      }
      else
      {
         argv[0] = (char*)helper.c_str();
         argv[1] = (char*)file.c_str();
         argv[2] = NULL;
      }
      execvp(helper.c_str(), argv);
   }//if (!(child = fork()))
}

void
AWConfigLin::closeViewer()
{
   playFileStop(m_viewerPID);
}


void
AWConfigLin::playSoundFileStart()
{
   playFileStart(m_playerPID, m_waveFile);
}

void
AWConfigLin::playSoundFileStop()
{
   playFileStop(m_playerPID);
}

AWBoolean
AWConfigLin::isSoundFilePlaying()
{
	AWBoolean retVal = false;
   int status;
	if ( m_playerPID > 0 )
	{
		waitpid(m_playerPID, &status, WNOHANG);
		retVal =  0 == kill(m_playerPID, 0);
	}
	return retVal;
}//AWBoolean AWConfigLin::isSoundFilePlaying()



void
AWConfigLin::playFileStart(int& PID, const AWCString& file, const AWCString& ext)
{
   AWCString helper;
   if (ext.length())
   {
      getHelperForExtension(ext, helper);
   }
   else getHelperForFile(file, helper);
   if (helper.length() && file.length())
   {
      if (!(PID = fork()))
      {  //this is the young child...
      	//Parse the command line - first find num arguments
      	//note we require program itself as 1st  + file as last argument
      	char* shite = strdup(helper.c_str());
      	int argc = parseCommandLine(shite, NULL);
			char** argv = new char*[argc+2];
			parseCommandLine(shite, argv);
			argv[argc++] = (char*)file.c_str();
         argv[argc] = NULL;
         execvp(argv[0], argv);
         //error
		   perror(argv[0]);
         _exit(-1);
      }//if (!(child = fork()))
   }//if (helper.length() && file.length())
}//void AWConfigLin::playFileStart(int& PID, const AWCString& file)


void
AWConfigLin::playFileStop(int& PID)
{
   int status;
	if ( PID > 0 )
	{
		if ( kill(PID, 0) == 0 )
		{
			kill(PID, SIGTERM);
			sleep(1);
			waitpid(PID, &status, WNOHANG|WUNTRACED);
		}
		PID = 0;
	}//if ( PID > 0 )
}//void AWConfigLin::playFileStop(int& PID)


void
AWConfigLin::saveExtensions(ostream& cfg)
{
   if (!m_extensions.length()) return;
   string::size_type pos;
   string::size_type from = 0;
   AWCString outLine;
   char      line[300];
   while (string::npos != (pos = m_extensions.find(";", from)))
   {  //each extension record is terminated with ; even if only one of them
      outLine.eqSubString(m_extensions, from, pos);
      //outLine = m_extensions(from, pos-from);
      from = pos+1;
      snprintf(line, 299, "%s\n", outLine.c_str());
      if (cfg.good())
      {
         cfg << line;
      }
   }//while (string::npos != (pos = m_extensions.index(";", from)))
}//void AWConfigLin::saveExtensions()



AWBoolean
AWConfigLin::isFullScreen()
{
   return m_fullScreen;
}



void
AWConfigLin::loadConfig()
{
   AWCString line;
   //fprintf(stderr, "Reading AWConfig file at: %s\n", m_cfgFileName.c_str());
   ifstream cfg(m_cfgFileName);
   while (cfg.good())
   {
      line.erase();
      getline(cfg, line, '\n');
      if (parseLineFor(line, AW_SCENENAME_TAG)) {;}
      else if (parseLineFor(line, AW_SNAPSHOT_TAG)) {;}
      else if (parseLineFor(line, AW_GAME_TAG)) {;}
      else if (parseLineFor(line, AW_LEVEL_TAG)) {;}
      else if (parseLineFor(line, AW_FULLSCREEN_TAG)) {;}
      else if (parseLineFor(line, AW_USELEVEL_TAG)) {;}
      else if (parseLineFor(line, AW_HEIGHT_TAG)) {;}
      else if (parseLineFor(line, AW_WIDTH_TAG)) {;}
      else if (parseLineFor(line, AW_BPP_TAG)) {;}
      else if (parseLineFor(line, AW_FLIPY_TAG)) {;}
      else if (parseLineFor(line, AW_AUTOSWITCH_TAG)) {;}
      else if (parseLineFor(line, AW_SWITCHMINS_TAG)) {;}
      else if (parseLineFor(line, AW_USERINPUTMETHOD_TAG)) {;}
      else if (parseLineFor(line, AW_PLAYSOUNDTYPE_TAG)) {;}
      else if (parseLineFor(line, AW_PLAYGAMEEFFECTS_TAG)) {;}
      else if (parseLineFor(line, AW_CDROMDRIVE_TAG)) {;}
      else if (parseLineFor(line, AW_LASTTABVIEWED_TAG)) {;}
      else if (parseLineFor(line, AW_SOUNDFILE_TAG)) {;}
      else if (parseLineFor(line, AW_HTTPPROXY_TAG)) {;}
      else if (parseLineFor(line, AW_DISPLAYLISTS_TAG)) {;}
      else if (parseLineFor(line, AW_CVA_TAG)) {;}
      else parseExtension(line);
   }
}//void AWConfigLin::loadConfig()


void
AWConfigLin::saveConfig()
{
   //fprintf(stderr, "Writing AWConfig file at: %s\n", m_cfgFileName.c_str());
   ofstream cfg(m_cfgFileName);
   if (cfg.good())
   {
      writeString(cfg, "AlteredSaver v2.3\n", "AWVERSION");
      writeString(cfg, m_scene, AW_SCENENAME_TAG);
      writeString(cfg, m_snapshots, AW_SNAPSHOT_TAG);
      writeString(cfg, m_game, AW_GAME_TAG);
      writeInt(cfg, m_level, AW_LEVEL_TAG);
      writeInt(cfg, m_fullScreen, AW_FULLSCREEN_TAG);
      writeInt(cfg, m_useLevel, AW_USELEVEL_TAG);
      writeInt(cfg, m_height, AW_HEIGHT_TAG);
      writeInt(cfg, m_width, AW_WIDTH_TAG);
      writeInt(cfg, m_bpp, AW_BPP_TAG);
      writeInt(cfg, (int)m_flipY, AW_FLIPY_TAG);
      writeInt(cfg, (int)m_autoSwitchScenes, AW_AUTOSWITCH_TAG);
      writeInt(cfg, m_sceneSwitchMinutes, AW_SWITCHMINS_TAG);
      writeInt(cfg, m_userInputMethod, AW_USERINPUTMETHOD_TAG);
      writeInt(cfg, m_playSound, AW_PLAYSOUNDTYPE_TAG);
      writeString(cfg, m_waveFile, AW_SOUNDFILE_TAG);
      writeInt(cfg, m_playEffects, AW_PLAYGAMEEFFECTS_TAG);
      writeString(cfg, m_cdrom, AW_CDROMDRIVE_TAG);
      writeInt(cfg, m_lastTabViewed, AW_LASTTABVIEWED_TAG);
      writeString(cfg, m_httpProxy, AW_HTTPPROXY_TAG);
      writeInt(cfg, (int)m_useDisplayLists, AW_DISPLAYLISTS_TAG);
      writeInt(cfg, (int)m_useVertexArrays, AW_CVA_TAG);
      saveExtensions(cfg);
   }//if (cfg.good())
}//void AWConfigLin::saveConfig()


void
AWConfigLin::writeInt(ostream& os, int val, char* name)
{
   char line[300];
   snprintf(line, 299, "%s=%d\n", name, val);
   if (os.good())
   {
      os << line;
   }
}//void AWConfigLin::writeInt(ostream& os, int val, char* name)


void
AWConfigLin::writeString(ostream& os, const AWCString& val, char* name)
{
   char line[300];
   snprintf(line, 299, "%s=%s\n", name, val.c_str());
   if (os.good())
   {
      os << line;
   }
}//void AWConfigLin::writeString(ostream& os, int val, char* name)



void
AWConfigLin::loadGameConfig()
{
}


void
AWConfigLin::loadSaverConfig()
{

}

void
AWConfigLin::saveGameConfig()
{

}

void
AWConfigLin::saveSaverConfig()
{

}


void
AWConfigLin::loadUserInfo()
{

}

//LOCKING
//Helper functions to allow only a single AlteredSaver at a time
//--------------------------------------------------------------
int AWConfigLin::getLock()
{
   int retVal = 0;
	AWCString lockFile(m_home);
        lockFile += AW_APPNAME;
        lockFile += ".lk";
	if ( m_appLock || (m_appLock = fopen(lockFile, "w")) )
	{  //file opened - now try for an exclusive lock
      retVal = 0 == flock(fileno(m_appLock), LOCK_EX|LOCK_NB);
      AWTRACE1("   flock returned %d\n", retVal);
	}
	return retVal;
}


void AWConfigLin::releaseLock()
{
   AWTRACE1("release lock with m_appLock %d\n", (int)m_appLock);
   if (m_appLock)
   {
      flock(fileno(m_appLock), LOCK_UN);
      fclose(m_appLock);
      m_appLock = NULL;
   }
}


//scene list management functions
int
AWConfigLin::getNumScenes()
{
   return m_globBuf.gl_pathc;
}//int AWConfigLin::getNumScenes()


AWCString
AWConfigLin::getScene(int index)
{
   AWCString scene;
   if ((index >= 0) && (index < getNumScenes()))
   {
      scene = m_globBuf.gl_pathv[index];
   }
   return scene;
}//AWCString AWConfigLin::getScene(int index)


void
AWConfigLin::buildSceneList(const AWCString& scene)
{
   //idea is to extract the scene directory from this parameter
   //parameter can be either:
   //(i) full path to a given scene file which user wants to show first  - other scenes in same dir
   //(ii)scenes directory where all scenes can be found
   AWCString useScene(scene);
   if (!useScene.length()) useScene = m_scene;
   AWCString dir(useScene);

   if (string::npos != dir.find(".tnt"))
   {	//need to remove the filename for specification of initial directory
	   string::size_type index = dir.rfind("/");
      if (string::npos == index)
           index = dir.rfind("\\");
	   if (string::npos != index)
	   {
	      if (index+1 < dir.length())
              {
                 dir.replace(index+1, dir.length()-index-1, "");
              }
	   }
   }//if (dir.contains(".tnt", AWCString::ignoreCase))
	//now we should have the directory
	AWCString searchFor(dir);
        searchFor += "*.tnt";
	AWTRACE1("searching directory %s\n", searchFor.c_str());

	if (0 == glob(searchFor.c_str(), 0, NULL, &m_globBuf))
	{  //we've found some AlteredWorlds - keep the paths in this 'glob' data structure
	   //now just need to keep a record of which scene we're on currently, if any
	   //go through all matching scenes - see if match 'scene' - if so, keep index
	   //if we don't find a match, just set scene to first in list by leaving m_sceneIndex=0 ;
      AWTRACE("Found at least one AlteredWorld\n");
	   m_sceneIndex=0;
	   for (int item = 0; item < m_globBuf.gl_pathc; item++)
	   {
	      if (0 == strcmp(m_globBuf.gl_pathv[item], useScene.c_str()))
	      { //we've found the supplied scene - just set the index and bail
	         m_sceneIndex = item;
            AWTRACE2("  selecting scene: %s index %d\n", useScene.c_str(), m_sceneIndex);
	         break;
	      }
	   }//for (int item = 0; item < m_globBuf.gl_pathc; item++)
	}//if (0 == glob(searchFor.c_str(), 0, NULL, &m_globBuf))
	m_autoSwitchScenes &= (getNumScenes() > 1);
}//void AWConfigLin::buildSceneList(const AWCString& scene)


void
AWConfigLin::setScene(const AWCString& name)
{
	 m_scene = name;
    m_sceneIndex = 0;
    for (int i=0; i < m_globBuf.gl_pathc; i++)
    {
        if (name == m_globBuf.gl_pathv[i])
        {
            m_sceneIndex = i;
            break;
        }//if (name == m_globBuf.gl_pathv[i])
    }//for (int i=0; i < m_globBuf.gl_pathc; i++)
}//void AWConfigLin::setSceneIndex(const AWCString& name)



