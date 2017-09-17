#ifndef __AW_CONFIG_LIN_H
#define __AW_CONFIG_LIN_H

#include <stdio.h>
#include <glob.h>

#include "AW3DTools/AWToolDefs.h"
#include "AWConfig.h"


class AWConfigLin : public AWConfig
{
public:
   AWConfigLin(AWMode isSaver=awGame);
   virtual        ~AWConfigLin();

   virtual AWBoolean   isFullScreen();
   virtual void   loadConfig();
   virtual void   saveConfig();

   virtual void   loadGameConfig();
   virtual void   loadSaverConfig();

   virtual void   saveGameConfig();
   virtual void   saveSaverConfig();

   virtual void   loadUserInfo();

   virtual int       getNumScenes();
   virtual AWCString getScene(int index);
   virtual void      buildSceneList(const AWCString& scene = "");
   virtual void      setScene(const AWCString& name);

   void           playFileStart(int& PID, const AWCString& file, const AWCString& ext="");
   void           playFileStop(int& PID);

   void           playSoundFileStart();
   void           playSoundFileStop();
   AWBoolean				isSoundFilePlaying();

   void           openViewer(const AWCString& htmlFile, const AWCString& ext = "");
   void           closeViewer();

   AWCString      getExtFromFile(const AWCString& fileName);
   AWBoolean           getHelperForFile(const AWCString& fileName, AWCString& helper);
   AWBoolean           getHelperForExtension(const AWCString& ext, AWCString& helper, const AWCString& replaceStr="");

   void           setHelperForExtension(const AWCString& ext, AWCString& helper);

   const AWCString& getHome()    {return m_home;}

   int            getLock();
   void           releaseLock();

//persisted new data members
   AWCString      m_extensions;
   AWCString      m_game;
   //HTTP proxy support for scene downloads. stored as: server:port
   AWCString      m_httpProxy;

protected:
   int				parseCommandLine(char* cmdline, char** argv);
   AWBoolean           parseExtension(const AWCString& line);
   AWBoolean           parseLineFor(const AWCString& line, const AWCString& name);
   AWBoolean           getValueForName(const AWCString& name, const AWCString& val);
   void           writeInt(ostream& os, int val, char* name);
   void           writeString(ostream& os, const AWCString& val, char* name);
   void           saveExtensions(ostream& cfg);

//not persistent
   AWCString      m_cfgFileName;
   AWCString      m_home;
   FILE*          m_appLock;
   int            m_playerPID;
   int            m_viewerPID;

//holds all scenes found in the same directory as the one
//supplied in buildSceneList(AWCString& scene);
   glob_t      	m_globBuf;
};//class AWConfigLin : public AWConfig

#endif //sentinel __AW_CONFIG_LIN