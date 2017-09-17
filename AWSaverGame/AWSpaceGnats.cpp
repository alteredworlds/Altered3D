#include <AW3DTools/AWFlock.h>
#include <AWSerialize/AWVerify.h>

#include "AWSpaceGnats.h"


AWSpaceGnats::AWSpaceGnats(AWConfig& cfg) : m_cfg(cfg), 
   m_isSpaceGnats(FALSE), m_validUser(FALSE)
{
   AWTRACE("AWSpaceGnats constructor\n");
   
   m_userInputMethod = USE_KEYBOARD;
   setSwitchToNextSection(FALSE);
   setSwitchToNextScene(FALSE);
   m_scene.m_targetSection = 1;
   m_renderer.setScene(m_scene);
   // Reassign stderr

   resetData();
#ifndef _WIN32
   m_validUser = true; //for demo version only!
#endif
}


AWSpaceGnats::~AWSpaceGnats()
{
   AWTRACE("AWSpaceGnats Destructor\n");
   m_userInputMethod = USE_KEYBOARD;
   //GLRenderer::destroyRenderer();
}//AWSpaceGnats::~AWSpaceGnats()



void
AWSpaceGnats::resetData()
{
    AWTRACE("\n*resetData*\n");
    m_numFailures        = 0;
    m_currentLevel       = 0;

    m_flipY              = FALSE;
    m_gameCam            = FALSE;
    m_userActiveRecently = FALSE;
    //m_autoSwitchScenes   = FALSE;
    m_allowCameraSwitch  = TRUE;
    m_outputTimingInfo   = FALSE;
    //m_useLevel	         = FALSE;
    m_justFired          = FALSE;
    m_timeToExplode      = FALSE;
    m_useGun             = FALSE;
    m_reloading          = FALSE;

    setSwitchToNextScene(FALSE);
    setSwitchToNextSection(FALSE);

    m_levelChange        = 0;

    setSceneSwitchTime(60.0f);
    m_reloadTime         = 0.0f;
    m_autoFireTime       = AUTO_FIRE_SECONDS;
    m_cameraSwitchTime   = CAMERA_SWITCH_SECONDS;
    m_inactiveTimeout    = INACTIVE_SECONDS;
    m_nextTimingInfoOut  = TIMING_INFO_OUT;
    setSectionTimeout(0.0f);
    m_reloadSecs         = RELOAD_TIME;
    m_curTime            = 0.0f;
  
    m_viewPitch = m_viewYaw = 0.0f;  
    m_x = m_y = 0.0f;
}//void AWSpaceGnats::resetData()



void
AWSpaceGnats::resetTimers()
{
    AWTRACE("\n*resetTimers*\n");
    setSceneSwitchTime(m_cfg.getSceneSwitchSecs());
    m_reloadTime         = 0.0f;
    m_autoFireTime       = AUTO_FIRE_SECONDS;
    m_cameraSwitchTime   = CAMERA_SWITCH_SECONDS;
    m_inactiveTimeout    = INACTIVE_SECONDS;
    m_nextTimingInfoOut  = TIMING_INFO_OUT;
    setSectionTimeout(0.0f);
    m_reloadSecs         = RELOAD_TIME;
    m_curTime            = 0.0f;  
    if (m_cfg.isSaver() && m_isSpaceGnats)
    {
        setSaverTimeouts();
        m_renderer.initTimeBar(60.0f);
    }
    m_viewPitch = m_viewYaw = 0.0f;
    m_x = m_y = 0.0f;
}//void AWSpaceGnats::resetTimers()



void        
AWSpaceGnats::init(int width, int height)
{
   m_renderer.initialize(m_cfg.m_width, m_cfg.m_height);
   checkForGameCamera();
   resetTimers();
   m_curTime = 0.0f;
   AWTRACE2("\nCurrent time is: %f   next auto-switch due at: %f\n", (double)m_curTime, (double)getSceneSwitchTime());
}//void AWSpaceGnats::init(int width, int height)



void
AWSpaceGnats::display()
{  //now is a suitable time to poll the input devices
   //and calculate params to be sent to display(...)
   m_userInput.determineFocus();
   float speed = 0.0f;
   if (m_userInput.gotFocus())
   {
      //AWTRACEHI("AWSpaceGnats::display() - m_userInput.gotFocus() - TRUE\n");
      m_userInput.getKeyData(m_renderer.getTime());
      if (m_userInput.fire())
      {
			fire();
		}
		if (m_userInput.up()) //up arrow
		{
			up();
		}
		if (m_userInput.down()) //down arrow
		{
			down();
		}
		if (m_userInput.left()) //left arrow
		{
			left();
		}
		if (m_userInput.right()) //right arrow
		{
			right();
		}
      if (!m_isSpaceGnats && m_userInput.speedUp())
      {
         speed = 1.0f;
      }//if (m_userInput.speedUp())
	   if (!m_isSpaceGnats && m_userInput.speedDown())
      {
         speed = -1.0f;
      }//if (m_userInput.speedDown())
      if (m_userInput.forwardCam())
      {
         forwardCamera();
      }//if (m_userInput.forwardCam())
      if (m_userInput.backCam())
      {
         backCamera();
      }//if (m_userInput.backCam())
      if (m_userInput.toggleMesh())
      {
         m_renderer.togglePolygonMode();
      }//if (m_userInput.toggleMesh())
      if (m_userInput.toggleNormals())
      {
         m_renderer.toggleDrawNormals();
      }//if (m_userInput.toggleNormals())
      if (FALSE)//m_userInput.captureFrame())
      {
         captureFrame(m_cfg.m_snapshots);
      }//if (m_userInput.captureFrame())
   }//if (m_userInput.gotFocus())
   if (m_renderer.getPaused()) return;
   processTimers();
   m_renderer.display(speed, m_y, m_x, 0.0f, m_viewPitch, m_viewYaw);
   m_y = m_x = 0.0f;
   processEvents();
   m_userInput.clearKeys();
}//void AWSpaceGnats::display()



void
AWSpaceGnats::processTimers()
{  
//TIMER EVENTS SHARED BY SAVER AND GAME
//CALLED *BEFORE* calcTransform & display THIS FRAME
   //AWTRACEHI("\n\nAWSpaceGnats::processTimers\n");
   m_curTime = m_renderer.evalTime();
//OUTPUT TIMING INFO
   if (m_outputTimingInfo && (m_curTime > m_nextTimingInfoOut))
   {
	   m_nextTimingInfoOut = m_curTime + TIMING_INFO_OUT;
	   outputTimingInfo();
   }//if (m_outputTimingInfo && (curTime > m_nextTimingInfoOut)
//HANDLE FIRING &  RELOADING TIMER
   if (m_useGun && !m_reloading) 
   {
      m_scene.m_nodeList.fireGun(m_curTime);
      m_useGun     = FALSE;
      m_reloading  = TRUE;
      m_reloadTime = m_curTime + m_reloadSecs;
      m_justFired  = TRUE;
   }
   else if (m_reloading && (m_curTime >= m_reloadTime ))
   {
      m_reloading  = FALSE;
   }
//FLAG MOVE TO NEXT SCENE IN COMPOSITE TNT FILE
   if (getSectionTimeout() && (m_curTime >= getSectionTimeout()))
   {  //flag that need to move to next scene in composite file 
      AWTRACE("Section timeout detected\n");
      setSectionTimeout(0.0f);
      if ((m_scene.m_numSections > 1) && 
          (m_scene.m_targetSection <= m_scene.m_numSections))
      {
         AWTRACE("\nsignal EVENT - switchToNextSection of composite TNT file\n");
         setSwitchToNextSection(TRUE);
      }//if (m_renderer.m_targetSection < m_renderer.m_numSections) 
   }//if (m_sectionTimeout && (m_curTime >= m_sectionTimeout))
//PROCESS SAVER TIMERS
   if (!m_cfg.isGame())
   {
      processSaverTimers();
   }
}//void AWSpaceGnats::processTimers()



void
AWSpaceGnats::processSaverTimers()
{
//CALLED *BEFORE* calcTransform & display THIS FRAME 
//INACTIVITY TIMEOUT
   AWTRACEHI("AWSpaceGnats::processSaverTimers\n");
   if (getUserActive())
   {
      m_autoFireTime     = m_curTime + AUTO_FIRE_SECONDS;
      if (m_curTime >= m_inactiveTimeout)
      {
         AWTRACE("\nSaver - Inactive timeout detected - setting user active FALSE\n");        
         setUserActive(FALSE);
         m_inactiveTimeout = m_curTime + INACTIVE_SECONDS;
      }//if (m_curTime >= m_inactiveTimeout) 
      return;
   }//if (getUserActive())
   else
   {
//AUTO_SWITCH SCENES
      if ( m_cfg.m_autoSwitchScenes && (m_curTime >= getSceneSwitchTime()))
      { //move to a new TNT file...
         AWTRACE1("\nSaver scene switch timeout at time %f - try and load next scene, if any\n", (double)m_curTime);
         setSceneSwitchTime(m_curTime + m_cfg.getSceneSwitchSecs());
         if (!getUserActive())
         {
            setSwitchToNextScene(TRUE);
            m_curTime = 0.0f;
         }//if (!getUserActive())
      }//if ( m_autoSwitchScenes && (curTime >= m_sceneSwitchTime))
//ARE WE ABOUT TO MOVE TO NEXT SCENE IN COMPOSITE FILE?
      if (getSwitchToNextSection())
      {  //for AlteredSaver this multi-scene file counts as ONE SAVER
         //make sure to remove play time so far from the total play time
         //for each saver...
         AWTRACE("\nSaver EVENT - switchToNextSection detected\n");
         if (getSceneSwitchTime())
         {
            AWTRACE2("\n  Saver AUTO SWITCH was: %f  at: %f ", (double)getSceneSwitchTime(), (double)m_curTime);
            setSceneSwitchTime(getSceneSwitchTime() - m_curTime);
            AWTRACE1(" is now: %f\n", (double)getSceneSwitchTime());
         }
      }//if (getSwitchToNextSection())
//SWITCH CAMERA TIMER
      else if (m_cameraSwitchTime && (m_curTime > m_cameraSwitchTime))
      {
         m_cameraSwitchTime = m_curTime + CAMERA_SWITCH_SECONDS;
         AWTRACE2("\nSaver camera switch timeout at: %f   - next auto-switch due at: %f\n", (double)m_curTime, (double)m_cameraSwitchTime);
         if (m_scene.m_nodeList.getNumCameras()>1)
         {
            AWTRACE("   Switched Camera\n");
            forwardCamera();
         }//else if (m_scene.m_nodeList.getNumCameras()>1)
      }//if (curTime > m_cameraSwitchTime)
//AUTO_FIRE TIMER
      if (m_curTime >= m_autoFireTime)
      {
         m_autoFireTime = m_curTime + AUTO_FIRE_SECONDS;
         m_useGun=TRUE;
      }//if (!setUserActive() && (curTime >= m_autoFireTime))
      if ((m_autoFireTime != 999999999.0f) && 
          ((m_autoFireTime - m_curTime) > AUTO_FIRE_SECONDS))
      {
         m_autoFireTime = 0.0f;
      }
   }//if (getUserActive()) {} else {
}//void AWSpaceGnats::processSaverTimers()



void
AWSpaceGnats::processEvents()
{  //the following events may have been generated during GLRenderer::display
   // *levelFailed*
   // *switchLevel* 
   // *switchToNextScene*
   // SHARED EVENTS between GAME and SAVER
   ;
   if (getSwitchToNextSection())
   {  //we must switch to a new scene within composite TNT file
      AWTRACE1("\nShared EVENT - switchToNextSection detected at: %f\n", (double)m_curTime);     
      m_renderer.setSwitchLevel(FALSE);
      m_renderer.setLevelFailed(FALSE);
      m_userActiveRecently = FALSE;
      //for game, the target section is determined as part of the load logic
      m_scene.m_targetSection++;
   }//if (getSwitchToNextSection())
   if (!m_cfg.isGame()) processSaverEvents();
   processGameEvents();
}//void AWSpaceGnats::processEvents()


void
AWSpaceGnats::detectBonusLevel()
{
   if (!m_cfg.isGame() && (3 == m_scene.m_targetSection))
   {
      AWTRACE("\n   switching to bonus level\n");
      setSectionTimeout(90.0f);
      m_scene.m_targetSection = 0;
      m_currentLevel = 0;
      m_scene.m_nodeList.setFlockIndex(0);
   }//if (!m_cfg.isGame() && (3 == m_scene.m_targetSection))
}//void AWSpaceGnats::detectBonusLevel()


void
AWSpaceGnats::showRegDisplay(AWBoolean showIt)
{
   AWWorker& worker = m_scene.m_nodeList;
   AWObject* regD = worker.getObjectByName("RegDisplay");
   if (regD) regD->setIsVisible(showIt, TRUE);
}//void AWSpaceGnats::showRegDisplay(AWBoolean showIt)


void
AWSpaceGnats::showRegisterNow(AWBoolean val)
{  
   showRegDisplay(TRUE);
   forwardCamera();
}//void AWSpaceGnats::showRegisterNow(AWBoolean val)



void
AWSpaceGnats::processGameEvents()
{
   if (m_renderer.getTimeToExplode())
   {  //lame stuff - but time to release this product...
      m_renderer.setTimeToExplode(FALSE);
      setTimeToExplode();
   }
//GAME - HANDLE LEVEL SUCCESS
   if (m_renderer.getSwitchLevel())
   {
      m_renderer.setSwitchLevel(FALSE);
      m_levelChange        = 2;         
      m_currentLevel++;
      switchToNextLevel(m_curTime);
      AWTRACE1("\nSwitched to new level: %i\n", m_currentLevel);
   }//if (m_renderer.getSwitchLevel())
//GAME - HANDLE LEVEL FAILED
   if (m_renderer.getLevelFailed())
   {
      m_levelChange = m_levelChange ? m_levelChange : 1;//fail
      m_renderer.setLevelFailed(FALSE);
      m_scene.m_nodeList.resetLevel(m_curTime, m_renderer);
      AWTRACE1("\nReset level: %i\n", m_currentLevel);
   }//if (m_renderer.getLevelFailed())
}//void AWSpaceGnats::processGameEvents()



void        
AWSpaceGnats::processSaverEvents()
{
//DON'T BOTHER WITH THIS FOR NON-COMPOSITE TNT FILES
   if (1 == m_scene.m_numSections) return;   
//FAILED LEVEL LOGIC FOR SAVER ONLY
   if (m_renderer.getLevelFailed())
   {
      AWTRACE("\nSaver level failed - \n");
      if (!getUserActive() && m_cfg.m_level)
      {  //if user not active recently, saver switches levels on timeout
         //only need to do this if we've made it beyond 1st level
         m_renderer.setSwitchLevel(TRUE);
         m_renderer.setLevelFailed(FALSE);
         m_scene.m_nodeList.clearCurrentFlock();
         AWTRACE1("\n  Saver should move to NEXT LEVEL as user has been to level %i\n", m_cfg.m_level);
      }//if (!getUserActive() && m_level)
   }//if (m_renderer.getLevelFailed())
//SWITCH LEVEL LOGIC FOR SAVER ONLY
    if (m_renderer.getSwitchLevel())
    {
       AWTRACE2("\nSaver switching levels - User at level: %i  has visited % i \n", m_currentLevel, m_cfg.m_level);
       //note that m_currentLevel should be in range [0..6]
       //we're going to play a transition IFF either
       //(i) we are in level 5 and we've been to 6 in the game
       //(ii)we are in level 2 and we've been to 3 in the game
        AWBoolean playT = getPlayTransition();
        //have we gone further than ever before, captain...?
        if (m_currentLevel >= m_cfg.m_level)
        {
            AWTRACE("\nProgressing further than ever before?\n");
            if (getUserActive())
            {//recent activity implies let them move onward...
               m_cfg.m_level++; //don't save this, mind...  <==WARNING
               AWTRACE1("User activity detected recently - move to (non-recorded) level %i\n", (int)m_cfg.m_level);
               playT = getPlayTransition();         
            }//if (getUserActive())
            else
            {   //we are about to go a level too far...
                //so take us right back to the start via a section timeout
                //note that if we're about to play a transition, we don't
                //set a section timeout here as the playTransition call will
                AWTRACE("\n  user INACTIVE - back to the start...\n");
                m_renderer.setSwitchLevel(FALSE);
                if ( !playT )
                {   //we were NOT about to play a transition
                    setSwitchToNextSection(TRUE);
                    m_scene.m_targetSection=1;
                    AWTRACE("\n     no transition - switchToNextSection = TRUE - back to the start...\n");
                }//if ((m_currentLevel != 3) && (m_currentLevel != 6))
                //make sure we load the first 'scene' - this value is
                //incremented in the section timeout event handler
                if (m_currentLevel != 5) 
                {
                    AWTRACE("\n     m_currentLevel != 5 - back to the start...\n");
                    m_scene.m_targetSection = 1;
                    m_currentLevel = 0;
                }//if (m_currentLevel != 6)
            }//else
        }//if (m_currentLevel > AWWinUtils::m_level)
        if (playT)
        {
           //don't want to fire during a transition, or switch cameras
            m_cameraSwitchTime = 0.0f;
            m_autoFireTime = 999999999.0f;
            AWTRACE("\n-about to play a transition-make sure no cam switch or autofire\n");
        }//if (playT)
    }//if (m_renderer.getSwitchLevel())
}//void AWSpaceGnats::processSaverEvents()



void                
AWSpaceGnats::playTransition(float t, int i)
{
   AWTRACE2("AWSpaceGnats::playTransition(%4.2f, %i)", (double)t, i);
   float transitionDuration = m_scene.m_nodeList.playTransition(t, i);    
   setSectionTimeout(t + transitionDuration);
   m_renderer.selectCamera();
}//void AWSpaceGnats::playTransition(float t, int i)*/



void
AWSpaceGnats::switchToNextLevel(float t)
{  //a flock has been destroyed - time to move on to the next one
   //the currently selected flock can be set to disabled
   //and the next flock in the list initialized
   AWWorker& work = m_scene.m_nodeList;
   int numFlocks  = work.getNumFlocks();
   int flockIndex = work.getFlockIndex();
   AWTRACE1("\nswitchToNextLevel - current flock index is %i\n", flockIndex);
   if (flockIndex < numFlocks)
   {
      AWTRACE1("\n   disabling flock %i\n", flockIndex);
      work.getFlock(flockIndex)->setEnabled(FALSE);
      flockIndex++;
      if (flockIndex < numFlocks)
      { 
         AWTRACE2("\n   switching to flock index %i  at: %f  ", flockIndex, (double)m_curTime);
         work.setFlockIndex(flockIndex);
         AWFlock* nextFlock = work.getFlock(flockIndex);
         if (nextFlock)
         {   //populate the flock from the clone source
            nextFlock->setEnabled(TRUE);
            if (!nextFlock->getNumChildren()) nextFlock->populate(t, m_renderer);
            nextFlock->init(t, m_renderer);
            //and add all the generated child (flock member) objects to the display list
            for (int child=0; child<nextFlock->getNumChildren(); child++)
            {
               work.addNodeAndChildrenToFlatList(nextFlock->getChild(child));
            }//for (int child=0; child<curFlock->getNumChildren(); child++)
         }//if (nextFlock)
         AWTRACE1(" added %i children\n", nextFlock->getNumChildren());
      }//if (m_flockIndex < numFlocks)
      else
      {
         AWTRACE1("\n   playing transition at: %f\n", (double)m_curTime);
         playTransition(t);
      }//if (m_flockIndex < (int)m_flockList.entries())
   }//if (work.m_flockIndex < numFlocks)
}//void AWSpaceGnats::switchToNextLevel(float t)



void
AWSpaceGnats::setSaverTimeouts()
{
    AWTRACE("\nSetting saver timeouts on all flocks\n");
    AWWorker& work = m_scene.m_nodeList;
    for (int w=0; w< work.getNumFlocks(); w++)
    {
        AWFlock* flk = work.getFlock(w);
        AWTRACE1("Setting 60s timer on flock: %s\n", flk->getName().c_str());
        if (flk->getTimeout())
        {
            flk->setTimeout(60.0f);
        }//if (flk->getTimeout())
    }//for (int w=0; w< work.getNumFlocks(); w++)
    AWTRACE("\n");
}//void AWSpaceGnats::setSaverTimeouts()



void
AWSpaceGnats::useGun(const AWCString& name)
{
	m_scene.m_nodeList.useGun(name);
}//void	AWSpaceGnats::useGun(const AWCString& name)



void        
AWSpaceGnats::recordLevel()
{
   if (m_currentLevel > getLevel())
   {
       m_cfg.m_level += m_currentLevel - getLevel();
       //now save it
       m_cfg.saveLevel();
   }//if (m_currentLevel > getLevel())
}//void AWSpaceGnats::recordLevel()



void        
AWSpaceGnats::backCamera()
{
    AWTRACE("\n*backCamera*\n");
    m_renderer.backCamera();
    m_viewPitch = m_viewYaw = 0.0f;
    checkForGameCamera();
}//void AWSpaceGnats::backCamera()



void        
AWSpaceGnats::forwardCamera()
{
    AWTRACE("\n*forwardCamera*\n");
    m_viewPitch = m_viewYaw = 0.0f;
    m_renderer.forwardCamera();
    checkForGameCamera();
}//void AWSpaceGnats::forwardCamera()



void        
AWSpaceGnats::captureFrame(const AWCString& path)
{
   AWTRACE("Capture a frame\n");
   m_renderer.pause();
   m_renderer.startCapture(path, IMG_FMT_TGA);
	m_renderer.captureScreen();
	m_renderer.stopCapture();
   m_renderer.pause();
}//void AWSpaceGnats::captureFrame(const AWCString& path)



void        
AWSpaceGnats::toggleMultiFrameCapture(const AWCString& path)
{
    if (m_renderer.captureEnabled())
    {
	    m_renderer.stopCapture();
	    m_renderer.pause();
    }
    else
    {
       m_renderer.pause();
	    m_renderer.startCapture(path, IMG_FMT_TGA);
    }
}//void AWSpaceGnats::toggleMultiFrameCapture(const AWCString& path)



AWBoolean        
AWSpaceGnats::checkIsSpaceGnats(const AWCString& name)
{
   m_isSpaceGnats = (m_scene.m_numSections > 1) && name.contains("goSpaceGnats");
   return m_isSpaceGnats;
}//AWBoolean  AWSpaceGnats::checkIsSpaceGnats(const AWCString& name)



void
AWSpaceGnats::checkForGameCamera()
{
   AWTRACE("Check to see if current camera is a \'Game Camera\'\n");
   AWWorker& worker = m_scene.m_nodeList;
   //always gameCam for game usage or when someone used 'MOVE=T'
   m_gameCam = m_cfg.isGame() || worker.getTransformer(); 
   if (m_gameCam) return;
   AWTransformController* flight = NULL;   
   AWCamera* curCam = worker.getCurCamera();
   if (curCam)
   {
      m_gameCam = (AWBoolean)curCam->getName().contains("gameCam");
      if (m_gameCam)
      {
         flight = curCam->getController();
         if (flight && (CLSID_AWFLIGHTCONTROLLER == flight->isA()))
         {
            worker.setTransformer((AWFlightController*)flight);
         }//if (flight && (CLSID_AWFLIGHTCONTROLLER == flight->isA()))
      }//if (m_gameCam)
   }//if (curCam)
}//void AWSpaceGnats::checkForGameCamera()



AWBoolean   
AWSpaceGnats::loadScene(AWFile& f)
{
   AWTRACE("Load scene from AWFile\n");
   AWBoolean retVal = FALSE;
   if (f.isValid())
   {
      retVal = TRUE;
      try
      {
         m_scene.m_targetSection = 1;  
         f >> m_scene;
         resetTimers();
         resetData();
         checkForGameCamera();
         return TRUE;
      }//try
      catch (...)
      {
      retVal = FALSE;
      }             
   }//if (scene.length())
   return retVal;
}//AWBoolean  AWSpaceGnats::loadScene(AWFile& f)



AWBoolean        
AWSpaceGnats::loadScene(const AWCString& scene)
{
   AWBoolean retVal = FALSE;
   AWCString sceneName(scene);
   if (!sceneName.length()) sceneName = m_cfg.m_scene;
   AWTRACE1("Load scene from AWFile %s\n", sceneName.c_str());
   //fprintf(stdout, "valid %d: loading scene named: %s\n",(int)getValidUser(), m_sceneName.c_str());
   if (getValidUser() && sceneName.length() && AWFile::Exists(sceneName))
   {
        retVal = TRUE;
        if (m_outputTimingInfo)
        {
            fprintf( stdout, "Loading AlteredWorld - ");
            fprintf( stdout, sceneName.c_str());
            fprintf( stdout, "\n");
            fflush(stdout);
        }//if (m_outputTimingInfo)
        int goFlock    = 0;
        if (m_cfg.isGame())
        {   //m_currentLevel should be in range [0..5]
            if (0 == m_currentLevel) 
            {
               goFlock        = m_cfg.m_useLevel ? getFlock() : 0;
               m_currentLevel = m_cfg.m_useLevel ? getLevel() : 0;
            }
            m_scene.m_targetSection = (m_currentLevel > 2) ? 2 : 1;
            if (6 == m_currentLevel)
            {
               m_scene.m_targetSection = 3;
            }
            AWTRACE2("Composite TNT file - target section set to: %i  curLevel %i\n", (int)m_scene.m_targetSection, (int)m_currentLevel);
        }//if (!m_cfg.m_isSaver)
        else if (!m_isSpaceGnats)
        {
            m_scene.m_targetSection =1;
        }
        try
        {
            AWTRACE2("\nLoading AlteredWorld %s section %i \n", sceneName.c_str(), m_scene.m_targetSection);
            m_scene.load(sceneName);
            m_cfg.setScene(sceneName);
            useGun("Bouncer");
            if (checkIsSpaceGnats(sceneName))
            {
               m_scene.m_nodeList.setTargetFlock(goFlock);
            }
            checkForGameCamera();
            if (m_cfg.isGame())
            {
               setCamPlayback(FALSE);                 
            }//if (isGame())
            else
            {
               setUserActive(FALSE);
               resetTimers();
            }//else
        }//try
        catch (...)
        {
            retVal = FALSE;
        }             
    }//if (scene.length())
    return retVal;
}//AWBoolean AWSpaceGnats::loadScene(const AWCString& scene)



void
AWSpaceGnats::outputTimingInfo()
{
    char msg[300];
    double test   = m_renderer.m_perfCntr.getFrequency();
    double render = 1000.0f * m_renderer.m_perfCntr.m_renderCnt.m_diff;
    double trans  = 1000.0f * m_renderer.m_perfCntr.m_transformCnt.m_diff;
    //double coll =  1000.0f * m_renderer.m_perfCntr.m_collideCnt.m_diff;
    sprintf(msg, "FPS: %5.2f for %i triangles - (ms): render %2.2f, transform %2.2f",
    test,
    //show number of triangles sent to OpenGL
    m_renderer.m_perfCntr.getTrisThisFrame(),
    //get ms data for render/transform/collide
    render,
    trans);
    //cout << msg << endl << flush;
    fprintf(stdout, msg);
    fprintf(stdout, "\n");
    fflush(stdout);
}//void AWSpaceGnats::outputTimingInfo()



void        
AWSpaceGnats::copyToConfig()
{
   //transfer all values from config object to self
   m_cfg.m_validUser       = m_validUser;
   m_cfg.m_flipY           = m_flipY;
   m_cfg.m_userInputMethod = m_userInputMethod;
}//void  AWSpaceGnats::copyToConfig()



void        
AWSpaceGnats::copyFromConfig()
{  //transfer all values from config object to this AWSpaceGnats instance
   m_validUser = m_cfg.m_validUser;
   m_flipY = m_cfg.m_flipY;
   m_userInputMethod = m_cfg.m_userInputMethod;
//YUK YUK YUK
   m_userInput.setInputMethod(m_userInputMethod);
   m_renderer.setUseDisplayLists(m_cfg.m_useDisplayLists);
   m_renderer.setUseVertArrays(m_cfg.m_useVertexArrays);
   m_renderer.setCompileVertArrays(m_cfg.m_useCVAs);
   m_renderer.setUseCubeEnv(m_cfg.getUseCubeEnv());
   m_renderer.setUseBump(m_cfg.getUseBump());
   m_renderer.setUseBumpSpecular(m_cfg.getUseBumpSpecular());
   m_renderer.setUseS3Compression(m_cfg.getUseTexCompress());
}//void AWSpaceGnats::copyFromConfig()
