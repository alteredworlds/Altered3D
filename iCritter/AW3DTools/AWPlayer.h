
#ifndef __AWPLAYER_H
#define __AWPLAYER_H

#include "AWGL1Renderer.h"
#include "AWUserInput.h"

#define CAMERA_SWITCH_SECONDS    20.0

class AWPlayer
{
public:
   AWPlayer(AWBoolean openGL2=FALSE,
            AWBoolean useLights=TRUE,
            AWBoolean useFog=TRUE,
            AWBoolean useVertexColours=TRUE);
   ~AWPlayer(); 
	
	enum DeviceOrientation {
        Portrait,
        PortraitUpsideDown,
        LandscapeLeft,
        LandscapeRight
    };

	AWBoolean		loadScene(const AWCString& scene="");
	AWBoolean       loadScene(AWFile& f);
  
	void            init(int width, int height);
	void			shutdown();

	void            resetData();
	void            resetTimers();
	void			pause();
	void			resume();

	void            display();

   	void			zoomOut();
	void			zoomIn();
	void			nextCamera();
	void			setXYZRotation(float x, float y, float z, bool degrees=true);

protected: 
	void			assignTransformController();
	void			backCamera();
	void			forwardCamera();
   
//member data
public:
	AWGLRenderer*	m_renderer;
	AWScene			m_scene;

	double			m_curTime;
	AWBoolean		m_initCalled;
	AWBoolean		m_nextCamera;
   
//RUNTIME TIMEOUT values
    double			m_cameraSwitchTime;
   
//arrow ken angular rotation
	float			m_zoom;
	AWPoint			m_rotation;
	bool			m_degrees;
	
private:
	DeviceOrientation m_orientation;
};//class AWPlayer

#endif //__AWPLAYER_H
