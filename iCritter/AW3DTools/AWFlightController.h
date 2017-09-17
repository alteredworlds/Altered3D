#ifndef __FLIGHT_CONTROLLER_H
#define __FLIGHT_CONTROLLER_H

#include "AWTransformController.h"



class AWExport AWFlightController : public AWTransformController
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWFlightController);

	enum transformMode  {usual, record, playBack};
	AWFlightController();
	virtual				~AWFlightController();
	void				setMode(transformMode mode)		{m_mode=mode;}
	transformMode		getMode()						{return m_mode;}
	void				switchRecordMode();
	void				switchPlaybackMode();

	void				setLock(int l=1)  {m_lock=l;}
	int					getLock()         {return m_lock;}
	float				updateTransform(double t);
	virtual int			calcTransform(double t, const AWMatrix4& parentTM);
	void				setZoom(float zoom) {m_zoom=zoom;}
	
	static AWMatrix4    m_inputTransform;

protected:
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

protected:
	//not persisted
	float				m_zoom;
	float				m_speed;	
	transformMode		m_mode;	//flight controller operation mode
	AWBoolean			m_firstTime;
	AWPoint				m_initialY;
	
	// persisted...
	int					m_lock;
	// new in latest version
	float				m_pitchFactor, m_yawFactor, m_zoomFactor;
	float				m_maxZoom;
};//class AWTransformController : public AWTransformController

#endif //sentinel __FLIGHT_CONTROLLER_H