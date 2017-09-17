
#ifndef __AWSPACEGNATS_H
#define __AWSPACEGNATS_H

#include <AW3DTools/AWGLRenderer.h>
#include "AWUserInput.h"
#include "AWConfig.h"

#define AUTO_FIRE_SECONDS        0.5f
#define RELOAD_TIME              0.16f
#define INACTIVE_SECONDS         6.0f
#define CAMERA_SWITCH_SECONDS    30.0f
#define TIMING_INFO_OUT          5.0f
#define DEMO_TIMEOUT_SECONDS     20.0f



class AWSpaceGnats
{
public:
   AWSpaceGnats(AWConfig& cfg);
   ~AWSpaceGnats(); 

   AWBoolean         loadScene(const AWCString& scene="");
   AWBoolean         loadScene(AWFile& f);
   void	            useGun(const AWCString& name);
   AWBoolean         checkIsSpaceGnats(const AWCString& name);

   AWBoolean         getSwitchToNextScene()  {return m_switchToNextScene;}
   void              setSwitchToNextScene(AWBoolean v) {m_switchToNextScene=v;}
   
   AWBoolean         getSwitchToNextSection()  {return m_switchToNextSection;}
   void              setSwitchToNextSection(AWBoolean v) {m_switchToNextSection=v;}

   void              init(int width, int height);

   void              resetData();
   void              resetTimers();

   void              display();

   
   AWBoolean         getUserActive()                  {return m_userActiveRecently;}
   void              setUserActive(AWBoolean v=TRUE);

   AWBoolean         getUseJoystick()  {return USE_JOYSTICK == m_userInputMethod;}
   AWBoolean         getUseMouse()     {return USE_MOUSE    == m_userInputMethod;}
   AWBoolean         getUseKeyboard()  {return USE_KEYBOARD == m_userInputMethod;}

   void              setUseJoystick()  {m_userInputMethod = USE_JOYSTICK; m_userInput.setUseJoystick();}
   void              setUseMouse()     {m_userInputMethod = USE_MOUSE; m_userInput.setUseMouse();}
   void              setUseKeyboard()  {m_userInputMethod = USE_KEYBOARD; m_userInput.setUseKeyboard();}

   AWBoolean         getWin()          {return 2 == m_levelChange;}
   AWBoolean         getFail()	      {return 1 == m_levelChange;}
   AWBoolean         justFired()	      {return m_justFired;}
   AWBoolean         timeToExplode()   {return m_timeToExplode;}

   void              resetEvents();
   AWBoolean         getPlayTransition();
   void              detectBonusLevel();

   void              showRegisterNow(AWBoolean val);
   void              showRegDisplay(AWBoolean showIt);
   void              setCamPlayback(AWBoolean play=FALSE);
   void              setSceneSwitchTime(float v)      {AWTRACE1("\n  set AUTOSWITCH TIME %f\n", (double)v);m_sceneSwitchTime=v;}
   float             getSceneSwitchTime()             {return m_sceneSwitchTime;}

   int 			      getLevel();
   void              recordLevel();

   AWBoolean         getValidUser()                   {return m_validUser;}

   void              keyEvent(unsigned char code, AWBoolean down);

   void              fire();
   void              left();
   void              right();
   void              up();
   void              down();
   void              backCamera();
   void              forwardCamera();
   void              captureFrame(const AWCString& path);
   void              toggleMultiFrameCapture(const AWCString& path);

   void              setReloadTimeout(float t)       {m_reloadSecs=t;}

   int               getFlock();

   void              copyToConfig();
   void              copyFromConfig();
   
protected: 
   void              processEvents();
   void              processTimers();

   void              processGameEvents();
   void              processSaverTimers();
   void              processSaverEvents();

   void              switchToNextLevel(float t);
   void              playTransition(float t, int i=0);

   void              outputTimingInfo();
   void              setSaverTimeouts();
   void              checkForGameCamera();

   void              setSectionTimeout(float t)      {AWTRACE1("\nset section timeout to %f\n", (double)t);m_sectionTimeout=t;}
   float             getSectionTimeout()             {return m_sectionTimeout;}
   
   void              setJustFired(AWBoolean v =TRUE)      {m_justFired=v;}
   void              setTimeToExplode(AWBoolean v=TRUE)   {m_timeToExplode=v;}
   AWBoolean         isReloading()                   {return m_reloading;}
   void              setIsReloading(AWBoolean v =TRUE)    {m_reloading=v;}

//member data
public:
   GLRenderer        m_renderer;
   AWScene           m_scene;
   AWUserInput       m_userInput;
   AWConfig&         m_cfg;

   int               m_currentLevel;
   int               m_numFailures;
   float             m_curTime;

   int               m_levelChange;  //0 => no change   1 => fail   2 => win 
   AWBoolean         m_gameCam;
   AWBoolean         m_userActiveRecently;
   AWBoolean         m_allowCameraSwitch;
   AWBoolean         m_outputTimingInfo;
   AWBoolean         m_justFired;
   AWBoolean         m_timeToExplode;
   AWBoolean         m_useGun;
   AWBoolean         m_reloading;
   AWBoolean         m_switchToNextScene;
   AWBoolean         m_switchToNextSection;
   AWBoolean         m_isSpaceGnats;
   AWBoolean         m_validUser;
   AWBoolean         m_flipY;
   int               m_userInputMethod;
   
//RUNTIME TIMEOUT values
   float             m_sceneSwitchTime;
   float             m_autoFireTime;
   float             m_inactiveTimeout;
   float             m_cameraSwitchTime;
   float             m_nextTimingInfoOut;
   float             m_sectionTimeout;
   float             m_reloadTime;
   float             m_reloadSecs;
   
//arrow ken angular rotation
   float             m_x, m_y;
   float             m_viewPitch, m_viewYaw;
};//class AWSpaceGnats



inline void        
AWSpaceGnats::resetEvents()     
{
   m_levelChange=0;
   m_justFired=FALSE;
   m_timeToExplode=FALSE;
}//inline void AWSpaceGnats::resetEvents()  



inline void        
AWSpaceGnats::setUserActive(AWBoolean v)       
{
   if (v != m_userActiveRecently)
   {
      m_userActiveRecently = v;
      AWTRACE2("\nSaver - User Active set %i  at: %f s\n", (int)v, (double)m_curTime);
      setCamPlayback(!m_userActiveRecently);
   }//if (v != m_userActiveRecently)
   if (v) 
   {
      m_inactiveTimeout = m_curTime + INACTIVE_SECONDS;
   }//if (v)
}//void AWSpaceGnats::setUserActive(AWBoolean v)



inline void
AWSpaceGnats::setCamPlayback(AWBoolean play)
{
   AWFlightController* usr = m_scene.m_nodeList.getTransformer();
   if (usr && m_isSpaceGnats)
   {//we have just changed. switch off camrea playback for game cam
      usr->setMode(play ? 
         AWFlightController::playBack : AWFlightController::usual);
      AWTRACE1("Setting gameCam playback mode to %i\n", (int)play);
   }
}//void AWSpaceGnats::setCamPlayback(AWBoolean play)


inline AWBoolean        
AWSpaceGnats::getPlayTransition()
{
   AWBoolean retVal = ((m_currentLevel == 5) && (m_cfg.m_level >= 6)) ||
                                  ((m_currentLevel == 2) && (m_cfg.m_level >= 3));
   AWTRACE3("Play Transition: %i [curLevel %i  recorded Level %i]\n", (int)retVal, (int)m_currentLevel, (int)m_cfg.m_level);
   return retVal;
}//inline AWBoolean AWSpaceGnats::getPlayTransition()



inline void        
AWSpaceGnats::keyEvent(unsigned char code, AWBoolean down)
{
    m_userInput.keyEvent(code, down);
}//inline void AWSpaceGnats::keyEvent(unsigned char code, AWBoolean down)

inline int
AWSpaceGnats::getLevel()
{   //returns zero based level in range [0..5]
    return m_cfg.m_level % 6;
}//inline int AWSpaceGnats::getLevel()

inline int       
AWSpaceGnats::getFlock()
{   //returns the zero based flock index in range [0..2]
    return m_cfg.m_level%3;
}//inline int AWSpaceGnats::getFlock()

inline void 
AWSpaceGnats::fire()
{
   if (m_gameCam)
   {
      setUserActive();
      m_useGun=TRUE;
   } 
}//void AWSpaceGnats::fire()



inline void 
AWSpaceGnats::up()
{
   setUserActive();
	if (m_gameCam)
   {
      AWTRACE("AWSpaceGnats::up() - gameCam\n");
      m_y = m_flipY ? m_y+ANGLAR_ROTN : m_y-ANGLAR_ROTN;
   }
   else 
   {
      AWTRACE("AWSpaceGnats::up() - saverCam\n");
      m_viewPitch += 1.0f;
   }
}//void AWSpaceGnats::up()



inline void 
AWSpaceGnats::down()
{
   setUserActive();
	if (m_gameCam)
   {
      m_y = m_flipY ? m_y-ANGLAR_ROTN : m_y+ANGLAR_ROTN;
   }
   else 
   {
      m_viewPitch -= 1.0f;
   }
}//void AWSpaceGnats::down()


inline void 
AWSpaceGnats::left()
{
   setUserActive();
   if (m_gameCam)
   {
      m_x += ANGLAR_ROTN;
   }
   else 
   {
      m_viewYaw -= 1.0f;
   }
}//void AWSpaceGnats::left()



inline void 
AWSpaceGnats::right()
{
   setUserActive();
   if (m_gameCam)
   {
      m_x -= ANGLAR_ROTN;
   }
   else 
   {
      m_viewYaw += 1.0f;
   }
}//void AWSpaceGnats::right()


#endif //__AWSPACEGNATS_H
