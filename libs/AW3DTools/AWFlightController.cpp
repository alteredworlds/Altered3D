#include "AWFlightController.h"


#define VER_AWFlightController_1 100
#define VER_AWFlightController_2 101


AWDEFINE_VERSIONABLE_PERSISTABLE(AWFlightController, CLSID_AWFLIGHTCONTROLLER, VER_AWFlightController_2);


AWFlightController::AWFlightController() 
	: m_mode(usual), m_viewYaw(0.0f), m_viewPitch(0.0f),
	  m_firstTime(TRUE), m_lock(0), m_speed(0.0f)
{
}


AWFlightController::~AWFlightController()
{
}


void							
AWFlightController::switchRecordMode()
{
	if (getMode() == AWFlightController::record)
	{
		setMode(AWFlightController::usual);
	}
	else
	{
		clearAnimation();
		setMode(AWFlightController::record);
	}
}//void	AWFlightController::switchRecordMode()


void											
AWFlightController::switchPlaybackMode()
{
	if (getMode() == AWFlightController::playBack)
	{
		setMode(AWFlightController::usual);
	}
	else
	{
		setMode(AWFlightController::playBack);
	}
}//void	AWFlightController::switchPlaybackMode()




float
AWFlightController::updateTransform(const AWPoint& euler, float accn, float t)
{	//updates LOCAL transform
    float deltaT = t - getLastUpdate();
	setLastUpdate(t);
	if (!m_firstTime && ((m_mode == usual) || (m_mode == record)))
	{
		AWQuat		curRot(getRotn());
		AWQuat		deltaRot;
		AWMatrix4	rotTM;
		AWPoint		curX;
		//note we're now using ANGULAR VELOCITY here
		deltaRot.fromEuler(deltaT*euler);
		curRot = curRot * deltaRot;
		
		curRot.toMatrix(rotTM);
		curX.x=rotTM[0];
		curX.y=rotTM[1];
		curX.z=rotTM[2];

		float dotVal = dot(m_initialY, curX);
		float thetaRad = (float)acos(dotVal);
		double theta = thetaRad * 360.0/(2.0*M_PI)-90;
		/*
		char msg[200];
		sprintf(msg, "q: (w %f, %f, %f, %f) X' (%f, %f, %f)  Dot: %f Deg: %f\n", 
			curRot.w, curRot.x, curRot.y, curRot.z, 
			curX.x, curX.y, curX.z, dotVal, theta);
		OutputDebugString(msg);*/
		if ((theta > 0.02) || (theta < -0.02))
		{
			AWQuat correction(AWPoint(0.0f, 0.0f, 1.0f), (float)theta);
			curRot = curRot * correction;
		}
		setRotn(curRot);
      m_speed = m_speed + accn * deltaT;
		if (m_speed !=  0.0f)
		{
			AWPoint		look;			
			rotTM.getDirection(look);
			setPosn(getPosn() - m_speed * look);
		}//if (speed >  0.0f)		
	}//if (m_mode == usual)
   return deltaT;
}//float AWFlightController::updateTransform(const AWPoint& euler, float accn, float t)




int						
AWFlightController::calcTransform(float t, const AWMatrix4& parentTM)
{	//the member data for PRS will have been updated by the draw thread
	//this fn. should perform any calculations required of this data
	//calculate the local transform data for this frame
	switch (m_mode)
	{
	case usual:
		break;

	case record:
		addRotation(getRotn(), t);
		addTranslation(getPosn(), t);
		break;

	case playBack:
		m_rotn.calcQuat(t, m_loop);
		m_posn.calc(t, m_loop);      
		break;
	default:;
	}//switch
	//now update the wsTM - note that a Camera will be used to supply the
	m_wsTM=parentTM;
	if (!m_lock && (m_viewYaw || m_viewPitch))
	{
		AWQuat rot;
		rot.fromEuler(AWPoint(m_viewPitch, m_viewYaw, 0.0f));
		m_wsTM.awRotate(rot);
	}
	//now update with local TM information
	m_wsTM.awTranslate(m_posn.getVal());
	m_wsTM.awRotate(m_rotn.getQuat());
	m_wsTM.awScale(m_scale.getVal());
	if (m_firstTime)
	{
		m_initialY.x=m_wsTM[4];
		m_initialY.y=m_wsTM[5];
		m_initialY.z=m_wsTM[6];
		/*
		char msg[200];
		sprintf(msg, "Y (%f, %f, %f) \n", m_initialY.x, m_initialY.y, m_initialY.z);
		OutputDebugString(msg);*/
		m_firstTime=FALSE;
	}
	return 1;
}//int AWFlightController::calcTransform(float t, const AWMatrix4& parentTM)



void				
AWFlightController::restoreGuts(AWFile& file)
{
	unsigned int version;
	int mode;

	restoreVersion(file, &version);   // Get the saved version #
	AWTransformController::restoreGuts(file);	
	file.Read(mode);
	m_mode = (transformMode)mode;
   if (version > VER_AWFlightController_1)
   {
      file.Read(m_lock);
   }//if (version > VER_AWFlightController_1)
}//void AWFlightController::restoreGuts(AWFile& file)



void        		
AWFlightController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
	file.Write((int)m_mode);
   file.Write(m_lock);
}//void AWFlightController::saveGuts(AWFile& file) const



AWspace     		
AWFlightController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
					 sizeof(int)   +
                sizeof(m_lock) +
					 AWTransformController::binaryStoreSize();
	return retVal;
}//AWspace AWFlightController::binaryStoreSize() const

