#include "AWFlock.h"
#include "AWSpaceGnats.h"
#include "AWGL2Renderer.h"



AWPlayer::AWPlayer(AWBoolean openGL2) 
    : m_initCalled(FALSE)
{
	AWTRACE("AWPlayer constructor\n");
    if (openGL2)
        m_renderer = new AWGL2Renderer();
    else
        m_renderer = new AWGL1Renderer();
	m_renderer->setScene(m_scene);
	resetData();
}


AWPlayer::~AWPlayer()
{
	AWTRACE("AWPlayer Destructor\n");
    delete m_renderer;
}//AWPlayer::~AWPlayer()


void
AWPlayer::resetData()
{
    AWTRACE("\n*resetData*\n");
	m_nextCamera = FALSE;
    m_cameraSwitchTime   = CAMERA_SWITCH_SECONDS;
    m_curTime            = 0.0f;
	m_zoom = 0.0f;
	m_degrees = false;
	m_rotation.setZero();
}//void AWPlayer::resetData()


void
AWPlayer::resetTimers()
{
    AWTRACE("\n*resetTimers*\n");
	m_nextCamera = FALSE;
    m_cameraSwitchTime   = CAMERA_SWITCH_SECONDS;
	m_curTime            = 0.0f;  
	m_zoom = 0.0f;
}//void AWPlayer::resetTimers()


void        
AWPlayer::init(int width, int height)
{
	m_renderer->initialize(width, height);
	resetTimers();
	m_curTime = 0.0f;
	AWTRACE1("\nCurrent time is: %f\n", (double)m_curTime);
	RECT r;
	r.left = r.top = 0;
	r.right = width; r.bottom = height;
	m_initCalled = TRUE;
}//void AWPlayer::init(int width, int height)


void					
AWPlayer::shutdown()
{
	if (m_initCalled)
	{
		m_renderer->shutdown();
		m_renderer->setScene(m_scene);
		resetData();
		m_initCalled = FALSE;
	}
}


void			 
AWPlayer::pause()
{
	if (!m_renderer->getPaused())
		m_renderer->togglePause();
}


void			 
AWPlayer::resume()
{
	if (m_renderer->getPaused())
		m_renderer->togglePause();
}


void
AWPlayer::display()
{  
   if (m_renderer->getPaused()) 
		return;
	m_curTime = m_renderer->evalTime();
	//SWITCH CAMERA TIMER
	if (m_nextCamera || (m_cameraSwitchTime && (m_curTime > m_cameraSwitchTime)))
	{
		m_cameraSwitchTime = m_curTime + CAMERA_SWITCH_SECONDS;
		AWTRACE2("\nSaver camera switch timeout at: %f   - next auto-switch due at: %f\n", (double)m_curTime, (double)m_cameraSwitchTime);
		if (m_scene.m_nodeList.getNumCameras()>1)
		{
			AWTRACE("   Switched Camera\n");
			forwardCamera();
		}
		m_nextCamera = FALSE;
	}
	//NOTES: this sucks, but works until decent O-O design sorted out.
	// rotation is applied to the m_inputTransform first, then
	// zoom is applied within the body of the flightcontroller as that is
	// the location of the max/min zoom values.
	AWFlightController::m_inputTransform.identity();
	if (0.0f != m_rotation.z)
		AWFlightController::m_inputTransform.awRotateZ(m_rotation.z, m_degrees);
	if (0.0f != m_rotation.y)
		AWFlightController::m_inputTransform.awRotateY(m_rotation.y, m_degrees);
	if (0.0f != m_rotation.x)
		AWFlightController::m_inputTransform.awRotateX(m_rotation.x, m_degrees);
	m_renderer->display(m_zoom);
}//void AWPlayer::display()


void			
AWPlayer::setXYZRotation(float x, float y, float z, bool degrees)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
	m_degrees = degrees;
}

void
AWPlayer::assignTransformController()
{	
	AWTRACE("Check to see if current camera is a \'Game Camera\'\n");
	AWWorker& worker = m_scene.m_nodeList;
	AWTransformController* flight = NULL;   
	AWCamera* curCam = worker.getCurCamera();
	if (curCam)
	{			
		flight = curCam->getController();
		if (flight && (CLSID_AWFLIGHTCONTROLLER == flight->isA()))
		{
			worker.setTransformer((AWFlightController*)flight);
		}//if (flight && (CLSID_AWFLIGHTCONTROLLER == flight->isA()))			
	}//if (curCam)
}


void        
AWPlayer::backCamera()
{
    AWTRACE("\n*backCamera*\n");
	m_zoom = 0.0f;
	m_renderer->backCamera();
}


void        
AWPlayer::forwardCamera()
{
    AWTRACE("\n*forwardCamera*\n");
    m_zoom = 0.0f;
    m_renderer->forwardCamera();
}


void					
AWPlayer::zoomOut()
{
	AWTRACEHI("AWPlayer::zoomOut()\n");
	m_zoom += 1.0f;
}


void					
AWPlayer::zoomIn()
{
	AWTRACEHI("AWPlayer::zoomIn()\n");
	m_zoom -= 1.0f;
}


void
AWPlayer::nextCamera()
{
	AWTRACEHI("AWPlayer::nextCamera()\n");
	m_nextCamera = TRUE;
}


AWBoolean   
AWPlayer::loadScene(AWFile& f)
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
      }//try
      catch (...)
      {
			retVal = FALSE;
      }             
   }//if (scene.length())
   return retVal;
}//AWBoolean  AWPlayer::loadScene(AWFile& f)


AWBoolean        
AWPlayer::loadScene(const AWCString& scene)
{
	AWBoolean retVal = FALSE;
	AWCString sceneName(scene);
	AWTRACE1("Load scene from AWFile %s\n", sceneName.c_str());
	if (sceneName.length() && AWFile::Exists(sceneName))
	{
		retVal = TRUE;
		try
		{
			m_scene.m_targetSection = 1; 
			AWTRACE2("\nLoading AlteredWorld %s section %i \n", sceneName.c_str(), m_scene.m_targetSection);
			m_scene.load(sceneName);
			//m_scene.save("/Users/twcgilbert/Development/test.tnt");
			resetTimers();
		}//try
		catch (...)
		{
			retVal = FALSE;
		}             
	}//if (scene.length())
	return retVal;
}//AWBoolean AWPlayer::loadScene(const AWCString& scene)
