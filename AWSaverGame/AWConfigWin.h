#ifndef __AW_CONFIGWIN_H
#define __AW_CONFIGWIN_H

#include "AWConfig.h"
#include <windows.h>
#include <vector>
 

class AWConfigWin : public AWConfig
{
public:
   AWConfigWin(AWMode isSaver=awGame);
   virtual           ~AWConfigWin(); 

   virtual void      parseCommandLine(const AWCString& cmdLine, AWBoolean forceSaver=FALSE);

   virtual void      loadUserInfo();
   virtual void      saveUserInfo();

   virtual void      saveLevel();

   virtual void      loadSharedInfo();
   virtual void      saveSharedInfo();

   virtual void      loadExporterInfo();
   virtual void      saveExporterInfo();

   virtual void      loadConfig();
   virtual void      saveConfig();

   virtual void      updateDisplayInfo();

   virtual AWBoolean isFullScreen();

   virtual int       valiDate();

   virtual int       getNumScenes();
   virtual AWCString getScene(int index);
   virtual void      buildSceneList(const AWCString& scene = "");
   virtual void      setScene(const AWCString& name);

   const AWCString&  getExportHelp()                   {return m_exportHelp;}
   const AWCString&  getExePath()                     {return m_exePath;}

   void              setExportHelp(const AWCString& s){m_exportHelp=s;}
   void              setExePath(const AWCString& s)   {m_exePath=s;}

   AWBoolean         getShowMakeExe()                 {return m_showMakeExe;}

protected:   
   virtual void      loadDisplayConfig(HKEY base);
   virtual void      saveDisplayConfig(HKEY base);

   AWCString         m_exePath;
   AWCString         m_exportHelp;
   AWBoolean         m_showMakeExe;

   vector<AWCString> m_sceneList;
};//class AWConfigWin : public AWConfig


#endif //sentiel __AW_CONFIGWIN_H