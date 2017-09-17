#include "AWFlightController.h"


#define VER_AWFlightController_1 100
#define VER_AWFlightController_2 101
#define VER_AWFlightController_3 102
#define VER_AWFlightController_4 103


AWDEFINE_VERSIONABLE_PERSISTABLE(AWFlightController, CLSID_AWFLIGHTCONTROLLER, VER_AWFlightController_4);

AWMatrix4 AWFlightController::m_inputTransform;


AWFlightController::AWFlightController() 
	: m_mode(usual), 
	  m_firstTime(TRUE), m_lock(1), m_speed(0.0f), m_pitchFactor(1.0f), 
     m_yawFactor(1.0f), m_zoomFactor(1.0f), m_maxZoom(0.0f)
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
AWFlightController::updateTransform(double time)
{	//updates LOCAL transform
    double deltaT = time - getLastUpdate();
	setLastUpdate(time);
	return deltaT;
}//float AWFlightController::updateTransform(const AWPoint& euler, float accn, float t)


int						
AWFlightController::calcTransform(double time, const AWMatrix4& parentTM)
{	//the member data for PRS will have been updated by the draw thread
	//this fn. should perform any calculations required of this data
	//calculate the local transform data for this frame
	float useZoom = m_zoomFactor * m_zoom;
	switch (m_mode)
	{
	case usual:
		break;
	case record:
		addRotation(getRotn(), time);
		addTranslation(getPosn(), time);
		break;
	case playBack:
		m_rotn.calcQuat(time, m_loop);
		m_posn.calc(time, m_loop);
		break;
	default:;
	}//switch
	//now update the wsTM - note that a Camera will be used to supply the
	m_wsTM=parentTM;
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
#ifdef __AWTRACEHI
	//dump World Space Transform before further rotation etc.
	m_wsTM.dump();
#endif
	// transformations applied AFTER camera wsTM...
	if (0.0 != m_zoom)
	{	// ZOOM...
		if ((m_maxZoom > 0.0f) && (fabs(useZoom) > m_maxZoom))
			useZoom = useZoom > 0.0f ? m_maxZoom : -m_maxZoom;
		m_inputTransform.awTranslate(AWPoint(0.0, 0.0, useZoom));
	}
	if (!m_inputTransform.isIdentity())
		m_wsTM = m_wsTM * m_inputTransform;
	return 1;
}//int AWFlightController::calcTransform(double time, const AWMatrix4& parentTM)


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
		if (version > VER_AWFlightController_2)
		{
			file.Read(m_pitchFactor);
			file.Read(m_yawFactor);
			file.Read(m_zoomFactor);
			if (version > VER_AWFlightController_3)
			{
				file.Read(m_maxZoom);
			}				
            // NOTE: DEMO CODE FOR SPENCE FISHY ONLY...
			if (m_maxZoom <= 1.0)
				m_maxZoom = 100.0;
			if (0.0 == m_zoomFactor)
				m_zoomFactor = 5.0;
		}
	}
	//
#ifdef __AWTRACEHI
	dumpKeys();
#endif
}//void AWFlightController::restoreGuts(AWFile& file)


void        		
AWFlightController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
	file.Write((int)m_mode);
	file.Write(m_lock);
	file.Write(m_pitchFactor);
	file.Write(m_yawFactor);
	file.Write(m_zoomFactor);
	file.Write(m_maxZoom);
}//void AWFlightController::saveGuts(AWFile& file) const


AWspace     		
AWFlightController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
					sizeof(int)   +
					sizeof(m_lock) +
					sizeof(m_pitchFactor) +
					sizeof(m_yawFactor) +
					sizeof(m_zoomFactor) +
					sizeof(m_maxZoom) +
					 AWTransformController::binaryStoreSize();
	return retVal;
}//AWspace AWFlightController::binaryStoreSize() const
