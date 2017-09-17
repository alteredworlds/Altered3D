#include "AlteredExport.h"


//Camera output
AWNode* 
TnTExprt::ExportCameraObject(INode* node, AWNode* parent)
{
	CameraState cs;
	TimeValue t = GetStaticFrame();
	Interval valid = FOREVER;
	// Get animation range
	Interval animRange = ip->GetAnimRange();
	
	ObjectState os = node->EvalWorldState(t);
	CameraObject *cam = (CameraObject *)os.obj;	
	cam->EvalCameraState(t,valid,&cs);

	AWCamera* awCam  = new AWCamera(node->GetName(), parent);
	//extract the node transformation
	assignTransformController(node, awCam);
	ExportCameraSettings(*awCam, &cs, cam, t);

	// Export animation keys for the camera node
	if (GetIncludeAnim() && GetIncludeCamLightAnim()) 
	{
		ExportAnimKeys(node, awCam, TRUE);
		if (awCam->isAnimated())
		{	//if we have an animation to play, ensure that the scene is
			//loaded with the camera in playback mode already
			((AWFlightController*)awCam->getController())->switchPlaybackMode();
		}
	}//if (GetIncludeAnim())
   //check for user motion lock and set accordingly
   AWBoolean isJunkParam = FALSE;
   if (node->GetUserPropBool(_T("LOCK"), isJunkParam))
   {	//make this typesafe ASAP
      AWTransformController* ctl = awCam->getController();
      if (ctl && (CLSID_AWFLIGHTCONTROLLER==ctl->isA()))
      {
         ((AWFlightController*)ctl)->setLock();
      }//if (ctl && (CLSID_AWFLIGHTCONTROLLER==ctl->isA()))
   }//if (node->GetUserPropBool(_T("LOCK"), isJunkParam))   
	return awCam;
}//void TnTExprt::ExportCameraObject(INode* node)




void 
TnTExprt::ExportCameraSettings(AWCamera& awCam, CameraState* cs, 
							   CameraObject* cam, TimeValue t)
{
	if (cs->manualClip) 
	{
		awCam.m_nearClip = cs->hither;
		awCam.m_farClip  = cs->yon;
	}
	else
	{
		awCam.m_nearClip = cs->nearRange;
		awCam.m_farClip  = cs->farRange;
	}
	awCam.m_fov = cs->fov*fRadToDeg;
	//ID_CAMERA_TDIST, cam->GetTDist(t)));
}//void TnTExprt::ExportCameraSettings(AWCamera& awCam, CameraState* cs, CameraObject* cam, TimeValue t)



void
TnTExprt::assignCameraForActiveViewport(Interface* i)
{	//DOES NOT YET WORK
	if (i)
	{
		ViewExp* viewport = i->GetActiveViewport();
		Matrix3 aTM, coordSysTM;
		viewport->GetAffineTM(aTM);
		// The affine TM transforms from world coords to view coords
		// so we need the inverse of this matrix?
		coordSysTM = Inverse(aTM);
		//coordSysTM = aTM;
		//get the position and orientation of the Viewport
		//what else do I need for a Camera?
		//Clipping Planes, FOV
		
		float fov = 45.0f;
      if ((VIEW_PERSP_USER == viewport->GetViewType()) ||
          (VIEW_CAMERA == viewport->GetViewType()))
      {
         fov = RadToDeg(viewport->GetFOV());
      }
		float nearClip = 1.0f;
		float farClip = 3000.0f; 
		GraphicsWindow* graphicsWindow = viewport->getGW();
		if (graphicsWindow)
		{
			//This method returns the largest device Z value.
		//nearClip = graphicsWindow->getHitherCoord();
			//This method returns the smallest device Z value.
			//farClip = graphicsWindow->getYonCoord();
		}
      AWPoint maxP(theScene.m_nodeList.getMax());
      AWPoint minP(theScene.m_nodeList.getMin());		
      maxP = maxP-minP;
      farClip = maxP.magnitude();
      if (farClip < 1000.0f) farClip = 1000.0f;

		AWPoint posn, scale(1.0f, 1.0f, 1.0f);
		AWQuat  rotn;
		AWCamera* awCam = new AWCamera("Active View", NULL, 45.0f, nearClip, farClip);

		getTransformFromMatrix3(coordSysTM, posn, scale, rotn, TRUE);

		AWFlightController* trans = new AWFlightController;
		trans->setTransform(posn, scale, rotn);
		awCam->setController(trans);
		theScene.m_nodeList.addNode( awCam );
		theScene.setCamera(awCam);
		//when done, must release the ActiveViewport pointer with
		i->ReleaseViewport(viewport);
	}//if (i)
}//void TnTExprt::assignCameraForActiveViewport(Interface* i)




AWCamera*
TnTExprt::initCamera(Interface* ip)
{	//we assume on entry that all AWCameras in the scene are
	//contained in the camera list, but are owned by m_root
	AWCamera* cam=NULL;
	if (NULL == GetCamera())
	{	//if the Camera node is NULL at this point, we need to create a Camera
		//for the active viewport, extract all the information required,
		//add it to the AWScene and set it as the current camera
		//assignCameraForActiveViewport(ip);
      ;
	}//if (NULL == GetCamera())
	else
	{//make sure that a camera is selected in our altered world...
		AWCString selCameraName(GetCamera()->GetName());
		//find this in the aw camera list
		AWCamera* nextCam=NULL;
		for (int cam=0; cam < theScene.m_nodeList.getNumCameras(); cam++)
		{
			nextCam = theScene.m_nodeList.getCamera(cam);
			if (nextCam && (selCameraName == nextCam->getName()))
			{
				theScene.m_nodeList.setCurCamera(nextCam);
				break;
			}//if (nextcam && (selCameraName == nextcam.getName()))
		}//for (int cam=0; cam < theScene.m_nodeList.getNumCameras(); cam++)
	}//else
	//if no object selected to MOVE, use this for the user transform
	//IFF the name starts with gameCam...
	if (!theScene.m_nodeList.getTransformer())
	{
		AWCamera* awCam = theScene.m_nodeList.getCurCamera();
		if (awCam && awCam->getName().contains("gameCam"))
		{
			AWTransformController* tran = awCam->getController();
			if ( tran && (CLSID_AWFLIGHTCONTROLLER == tran->isA()) )
			{
				theScene.m_nodeList.setTransformer((AWFlightController*)tran);
			}
		}
	}//if (theScene.m_nodeList.getViewTransformer())
	//set the parent of the background node (if any) to be the current camera
	AWNode* environment = theScene.m_nodeList.getEnvironment();
	//need to remove the environment from the node list
	theScene.m_nodeList.removeNode(environment);
	cam = theScene.m_nodeList.getCurCamera();
	if (environment && cam && 
		environment->getController() && cam->getController())
	{		
		//need to express the translational component of the environment transform
		//relative to the camera, its new parent
		AWPoint camPos, environPosn;
		//evaluate the camera ws position
		AWMatrix4 iden;
		iden.identity();
		cam->getRoot()->calcTransform(0, iden);
		cam->getController()->getWSPosition(camPos);
		environment->getPosn(environPosn);
		environment->getController()->setPosn(environPosn - camPos);
		//environment->setParent(curCam);
		cam->addChild(environment);
	}
	return cam;
}//AWCamera* TnTExprt::initCamera(Interface* ip)