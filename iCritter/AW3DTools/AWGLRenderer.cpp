#include "AWToolsCommon.h"
#include <math.h>
#include <time.h>
#include "AWGLRenderer.h"


void* AWGLRenderer::m_libHandle = NULL;

#define FRAME_STEP (1.0 / 24.0)


AWGLRenderer::AWGLRenderer()
	: m_scene(NULL)
{
    resetData();
}


AWGLRenderer::~AWGLRenderer()
{
}


//used to reset data in saver when new scene is loaded
//we keep the same HWND & Window, but kill the old GL Context
//and re-init a new one
void
AWGLRenderer::resetData()
{ 
    m_scene = NULL;
    m_glErr = 0;
	m_captureBuf = NULL;
	m_time = 0.0;
    m_pauseTime = 0.0;
	m_loop = FALSE;	
	m_paused = FALSE;
    m_libHandle = NULL;
    m_trisThisFrame = 0;
    // NOTE: m_background init???
    setUsingLighting(TRUE);
    setUserAllowsLighting(TRUE);
    setUsingFog(TRUE);
    setUserAllowsFog(TRUE);
    setUserAllowsVertexColours(TRUE);
}//void AWGLRenderer::resetData()


void
AWGLRenderer::initialize(int width, int height)
{
	m_width = width;
	m_height= height;
	initializeGL();
	initializeObjects();
	m_perfCntr.zeroTime();
	getScene().m_nodeList.resetTimeBar(0.0);
	m_pauseTime=0.0;	
	m_loop = (AWBoolean)getScene().getLoopTime(); //zero == FALSE for no loopign
	m_time = 0.0;
}//void AWGLRenderer::initialize(int width, int height)


void
AWGLRenderer::captureScreen()
{
}

void
AWGLRenderer::toggleFog()
{
	//AWBoolean bVal = !getScene().getUseFog();
	//getScene().setUseFog(bVal);
	//setFogParams();
}


void
AWGLRenderer::useFog(int fogVal)
{	//make sure we really need to change state
	if (fogVal != getScene().getUseFog())
	{	//desired state is different to the current state, so change
		getScene().setUseFog(fogVal);
		setFogParams();
	}
}//void AWGLRenderer::useFog(int fogVal)


void
AWGLRenderer::setFogParams()
{
}


void
AWGLRenderer::backCamera()
{
	if (getScene().m_nodeList.getNumCameras())
	{
		int camIndex = getScene().m_nodeList.findCamera(getScene().m_nodeList.getCurCamera());
		camIndex--;
		camIndex = abs(camIndex % getScene().m_nodeList.getNumCameras());
		getScene().m_nodeList.setCurCamera(getScene().m_nodeList.getCamera(camIndex));
		selectCamera();
	}
}//void AWGLRenderer::backCamera()


void
AWGLRenderer::forwardCamera()
{
	if (getScene().m_nodeList.getNumCameras())
	{
		int camIndex = getScene().m_nodeList.findCamera(getScene().m_nodeList.getCurCamera());
		camIndex++;
		camIndex %= getScene().m_nodeList.getNumCameras();
		getScene().m_nodeList.setCurCamera(getScene().m_nodeList.getCamera(camIndex));
		selectCamera();
	}
}//void AWGLRenderer::forwardCamera()


AWBoolean
AWGLRenderer::initCamera()
{
	if (m_width && m_height)
		m_aspect = (float)m_width/(float)m_height;
	return selectCamera();
}//AWBoolean AWGLRenderer::initCamera()


void
AWGLRenderer::initializeObjects()
{
	//init object data e.g. bounding spheres
	getScene().m_nodeList.init(0.0, *this);
	
	//zero time here so that any time sensitive settings use t=0
	m_perfCntr.zeroTime();
	
	getScene().m_nodeList.startup(0.0, *this);
}//void AWGLRenderer::initializeObjects()


void
AWGLRenderer::shutdown()
{
	shutdownGL();
	if (m_scene)
	{
		getScene().clearAndDestroy();
		m_scene = NULL;
	}
	resetData();
}//void AWGLRenderer::shutdown()


void
AWGLRenderer::resizeGL(int width, int height)
{
	if (width && height)
	{
		m_height = height;
		m_width  = width;
		initCamera();
	}//if (width && height)
}//void AWGLRenderer::resizeGL(int width, int height)


const double
AWGLRenderer::evalTime()
{
	if (m_paused) return m_time;
	if (!m_captureBuf)
	{
		m_time = m_perfCntr.getTime() - m_pauseTime;
		//AWTRACEHI2("Eval time as: %f (pause time is: %f)\n", (double)m_time, (double)m_pauseTime);
	}
	else
	{  //if capturing the screen advance the time by one frame
		m_time += FRAME_STEP;
	}
	if (m_loop && (m_time >= getScene().getLoopTime()))
	{
		AWTRACEHI2("Loop enabled: resetting time since %f greater than loop time %f\n", m_time, getScene().getLoopTime());
		m_time=0.0;
		m_perfCntr.zeroTime();
		m_pauseTime=0.0;
	}
	return m_time;
}//const float AWGLRenderer::evalTime()


void
AWGLRenderer::pause()
{
    m_pauseTime = m_perfCntr.getTime() - m_pauseTime;
}//void AWGLRenderer::pause(float t)


void
AWGLRenderer::togglePause()
{
    pause();
    m_paused = !m_paused;
}//void AWGLRenderer::togglePause()


void
AWGLRenderer::display(float viewZoom)
{  
	m_perfCntr.startTransform(); //start transform performance counter
	//update the Viewport TM based on user input for this frame
	getScene().update(viewZoom, m_time);	
	m_perfCntr.stopTransform();  //stop the transform performance counter
	m_perfCntr.startRender();    //start render performance counter
	//now iterate through the textures  
	//set the background color ONCE at this top level
	if (getScene().isBackgroundAnimated() && (-1 == getScene().getUseFog()))
	{
		getScene().getBackground(*(AWPoint*)m_background);
		AWTRACEHI4("Setting animated clear colour: (%5f, %5f, %5f, %5f)\n", (double)m_background[0], (double)m_background[1], (double)m_background[2], (double)m_background[3]);
		glClearColor(m_background[0], m_background[1], m_background[2], m_background[3]);
	}//if (getScene().isAmbientAnimated())
	displayFrame();  
	m_perfCntr.stopRender();
	//render sequential files when in render mode - very slow!
	if (m_captureBuf) 
        captureScreen();
	
	m_perfCntr.startCollide();
	getScene().m_nodeList.collideAllObjects(m_time);
	m_perfCntr.stopCollide();
	m_perfCntr.update(m_trisThisFrame);
}//void AWGLRenderer::display(float speed, float roll, float yaw, float pitch)


int
AWGLRenderer::getNumPasses(AWObject* o)
{
	return 0;
}


void
AWGLRenderer::showAllAlphaTestObjects()
{	//list is sorted in ascending order of depth, so need to show it the
	//other way round (furtherst away FIRST)
	AWTRACE1("\nGLRenderer::showAllAlphaTestObjects for %i objects\n", 
			 (int)getScene().m_nodeList.getNumAlphaTestObjects());
	int curObjIndex = getScene().m_nodeList.getNumAlphaTestObjects()-1;
	while (0 <= curObjIndex)
	{
		AWObject* curObj = getScene().m_nodeList.getAlphaTestObject(curObjIndex);
		drawObject(curObj);
		curObjIndex--;
	}//while (curObjIndex < (int)m_objects.entries())
}//void	AWGLRenderer::showAllAlphaTestObjects()


void
AWGLRenderer::showAllTransparentObjects()
{	//list is sorted in ascending order of depth, so need to show it the
	//other way round (furtherst away FIRST)
	AWTRACE1("\nGLRenderer::showAllTransparentObjects for %i objects\n", 
			 (int)getScene().m_nodeList.getNumTransparentObjects());
	int curObjIndex = getScene().m_nodeList.getNumTransparentObjects()-1;
	while (0 <= curObjIndex)
	{
		AWObject* curObj = getScene().m_nodeList.getTransparentObject(curObjIndex);
		drawObject(curObj);
		curObjIndex--;
	}//while (curObjIndex < (int)m_objects.entries())
}//void	AWGLRenderer::showAllTransparentObjects()


void
AWGLRenderer::showAllOpaqueObjects()
{
	AWTRACE1("\nGLRenderer::showAllOpaqueObjects for %i objects\n", 
			 (int)getScene().m_nodeList.getNumOpaqueObjects());
    AWCString envMarker("Environment");
	int curObjIndex = 0;
	while (curObjIndex < getScene().m_nodeList.getNumOpaqueObjects())
	{
		AWObject* curObj = getScene().m_nodeList.getOpaqueObject(curObjIndex);
		drawObject(curObj);
		curObjIndex++;
	}//while (curObjIndex < (int)m_objects.entries())
}//void	AWGLRenderer::showAllOpaqueObjects()