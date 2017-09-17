#ifndef __AW_CONFIG_H
#define __AW_CONFIG_H
//single interface to configuration for SpaceGnats & AlteredSaver
//to be implemented as derived class per-platform

#include "AW3DTools/AWCString.h"

class AWConfig
{
public:
   enum AWMode       {awGame=0, awConfig, awPreview, awSaver};
   enum AWSounds     {awNone=0, awNotUsed, awPlayFile, awPlayCD, awPlayGameMusic};

   AWConfig(AWMode isSaver=awGame);
   virtual           ~AWConfig() {;}

   virtual void      parseCommandLine(const AWCString& cmdLine, AWBoolean forceSaver=FALSE)=0;
     
   virtual void      loadSharedInfo(){;}
   virtual void      saveSharedInfo(){;}
   virtual void      loadUserInfo()=0;
   virtual void      saveUserInfo()=0;

   virtual void      loadConfig()=0;
   virtual void      saveConfig()=0;
   
   virtual void      saveLevel(){;}

   virtual AWBoolean isRegistered()             {return m_registered;}
   virtual void      setIsRegistered(AWBoolean v) {m_registered=v;}

   const AWCString&  getCommandLineSceneName()  {return m_cmdLineSceneName;}
   long              getWinID()                 {return m_winID;}

   virtual void      updateDisplayInfo()=0;

   void              setIsSaver(AWMode val=awSaver)  {m_isSaver=val;}
   AWBoolean         isSaver()                  {return awSaver  ==m_isSaver;}
   AWBoolean         isGame()                   {return awGame   ==m_isSaver;}
   AWBoolean         isPreview()                {return awPreview==m_isSaver;}
   AWBoolean         isConfig()                 {return awConfig ==m_isSaver;}

   AWBoolean         isValidUser()              {return m_validUser;}

   virtual AWBoolean isFullScreen()=0;

   int               getLastTab()               {return m_lastTabViewed;}
   void              setLastTab(int v)          {m_lastTabViewed=v;}

   AWBoolean         getPlayCD()                {return awPlayCD        == m_playSound;}
   AWBoolean         getNoMusic()               {return awNone          == m_playSound;}
   AWBoolean         getPlayFile()              {return awPlayFile      == m_playSound;}
   AWBoolean         getPlayGameMusic()         {return awPlayGameMusic == m_playSound;}
   AWBoolean         getPlayEffects()           {return m_playEffects;}
   AWBoolean         getUseBump()               {return m_useBump;}
   AWBoolean         getUseCubeEnv()            {return m_useCubeEnv;}
   AWBoolean         getUseBumpSpecular()       {return m_useBumpSpecular;}
   AWBoolean         getUseTexCompress()        {return m_useTexCompress;}

   AWBoolean         getBumpAvailable()         {return m_bumpAvailable;}
   AWBoolean         getCubeEnvAvailable()      {return m_cubeEnvAvailable;}
   AWBoolean         getTexCompressAvailable()  {return m_texCompressAvailable;}

   void              setPlayCD()                {m_playSound = awPlayCD;}
   void              setNoMusic()               {m_playSound = awNone;}
   void              setPlayFile(const AWCString& f="") {m_playSound = awPlayFile; if (f.length()) m_waveFile=f;}
   void              setPlayGameMusic()         {m_playSound = awPlayGameMusic;}
   void              setPlayEffects(AWBoolean v)     {m_playEffects = (int) v;}
   int				   getSceneSwitchSecs()			{return 60 * m_sceneSwitchMinutes;}
   void              setUseBump(AWBoolean v)    {m_useBump=v;}
   void              setUseCubeEnv(AWBoolean v) {m_useCubeEnv=v;}
   void              setUseBumpSpecular(AWBoolean v)    {m_useBumpSpecular=v;}

   void              setUseTexCompress(AWBoolean v)    {m_useTexCompress=v;}

   void              setBumpAvailable(AWBoolean v)          {m_bumpAvailable=v;}
   void              setCubeEnvAvailable(AWBoolean v)       {m_cubeEnvAvailable=v;}
   void              setTexCompressAvailable(AWBoolean v)   {m_texCompressAvailable=v;}
   

   virtual AWBoolean getNextScene(AWCString& sceneName);
   virtual int       getNumScenes()=0;
   virtual AWCString getScene(int index)=0;
   virtual void      buildSceneList(const AWCString& scene = "")=0;
   virtual void      setScene(const AWCString& name)=0;
   virtual AWCString getScenePath(const AWCString& shortName);

   virtual AWCString getGame(int i=0)           {return m_game;}

public:
   AWMode            m_isSaver;  
   AWBoolean         m_validUser; //derived at run time from user info below
   AWBoolean         m_registered;//derived at run time from user info below
   int               m_sceneIndex;
   int               m_lastTabViewed;

   long              m_winID;
   AWCString         m_cmdLineSceneName;


//PERSISTED SHARED CONFIGURATION
   AWCString         m_user; 
   AWCString         m_email; 
   AWCString         m_delivery;
   AWCString         m_serial;
   AWCString	      m_scene;
   AWCString         m_snapshots;
   AWCString	      m_help;
   int               m_level;                   //game level reached
   int               m_width, m_height, m_bpp;  //desired user settings 
   AWBoolean         m_flipY;

//PERSISTED SAVER CONFIGURATION (or from command line)
   //use current screen settings or switch to desired user settings
   AWBoolean         m_specifyMode; 
   //switch to next scene automatically       
   AWBoolean         m_autoSwitchScenes;
   //after this number of minutes
   int               m_sceneSwitchMinutes; 
   AWCString         m_cdrom;                //windows needs this to play a cd
   //original meaning of 'playWave' registry key extended
   // 0 => no sounds
   // 1 => play looped wave
   // 2 => launch helper app
   // 3 => play audio CD
   int			      m_playSound;
   AWCString	      m_waveFile;             //the sound file name, if we're playing one

//PERSISTED GAME CONFIGURATION
   AWBoolean         m_fullScreen;              //switch to fullscreen mode
   int               m_useLevel;                
   int               m_playMusic;
   int               m_playEffects;  
   AWCString         m_game;
   int               m_userInputMethod;

//OPENGL SETTINGS
   AWBoolean         m_useDisplayLists; 
   AWBoolean         m_useVertexArrays;
   AWBoolean         m_useCVAs;
   AWBoolean         m_useCubeEnv;
   AWBoolean         m_useBump;
   AWBoolean         m_useBumpSpecular;
   AWBoolean         m_useTexCompress;
   AWBoolean         m_texCompressAvailable;
   AWBoolean         m_bumpAvailable;
   AWBoolean         m_cubeEnvAvailable;
};//class AWConfig


inline 
AWConfig::AWConfig(AWMode isSaver) : m_isSaver(isSaver) 
{  //assign default values to all member data
//RUNTIME DATA
   m_validUser     = FALSE;
   m_registered    = FALSE;
   m_lastTabViewed = 0;
//PERSISTED SHARED CONFIGURATION
   m_level = 0;
   m_bpp   = 16;
   m_width = 800;
   m_height= 600;
   m_flipY = FALSE;
//PERSISTED GAME CONFIGURATION
   m_useLevel        =  0;
   m_playMusic = awNone;
   m_playEffects = 1;
   m_userInputMethod = 0;  //keboard - should use enum
   m_fullScreen      = FALSE;

//PERSISTED SAVER CONFIGURATION
   m_specifyMode = FALSE;
   m_autoSwitchScenes = TRUE;
   m_sceneSwitchMinutes = 1;
   m_playSound = 0;
   m_useDisplayLists = TRUE; 
   m_useTexCompress  = FALSE;
   m_useVertexArrays = FALSE; //should be TRUE when everything works properly
   m_useCVAs         = FALSE;
   m_useCubeEnv      = TRUE;
   m_useBump         = TRUE;
   m_useBumpSpecular = TRUE;
   m_bumpAvailable         = TRUE;
   m_cubeEnvAvailable      = TRUE;
   m_texCompressAvailable  = TRUE;
}//inline AWConfig::AWConfig(AWBoolean isSaver) : m_isSaver(isSaver) 



inline AWBoolean
AWConfig::getNextScene(AWCString& nextSceneName)
{
   AWBoolean retVal   = FALSE;
   int oldIndex  = m_sceneIndex;
   int numScenes = getNumScenes();
   AWTRACE1("entered getNextScene with %d scenes available\n", numScenes);
   if (numScenes > 1)
   {  //we should be using the scene indicated by m_sceneIndex
      //at this time. So, increment the index, mod by num entries,
      //and don't bother to change if same as before!
      AWTRACE2("\nAWConfig::getNextScene - current scene index % i, num scenes %i\n",m_sceneIndex, numScenes);
      m_sceneIndex=(m_sceneIndex+1)%numScenes;
      if (m_sceneIndex != oldIndex)
      {
      	retVal = TRUE;
         nextSceneName = getScene(m_sceneIndex);
         AWTRACE2("Scene index set to %i - loading new scene %s\n", (int)m_sceneIndex, nextSceneName.c_str());
      }//if (m_sceneIndex != oldIndex)
   }//if (numScenes > 1)
   return retVal;
}//AWBoolean AWConfig::getNextScene(AWCString& nextSceneName)

#endif //sentinel __AW_CONFIG_H