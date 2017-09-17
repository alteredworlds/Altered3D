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

   void           setLock(int l=1)  {m_lock=l;}
   int            getLock()         {return m_lock;}
	float				updateTransform(const AWPoint& euler, float speed, float t);
	virtual int			calcTransform(float t, const AWMatrix4& parentTM);
	//temp?
	void				setViewRot(float pitch, float yaw)	
					{m_viewPitch=pitch; m_viewYaw=yaw;}

protected:
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

protected:
	//not persisted
	float				m_viewYaw, m_viewPitch, m_speed;	
	transformMode		m_mode;	//flight controller operation mode
	AWBoolean				m_firstTime;
	AWPoint				m_initialY;
   int               m_lock;
};//class AWTransformController : public AWTransformController

#endif //sentinel __FLIGHT_CONTROLLER_H