#ifndef __AW_BOID_CONTROLLER_H
#define __AW_BOID_CONTROLLER_H

#include "AWTransformController.h"


class AWExport AWBoidController : public AWTransformController
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWBoidController);

	AWBoidController(float t = 0.0f);
	virtual				~AWBoidController();

	//calculate the transformation data for a given time
	virtual int			calcTransform(float t, const AWMatrix4& parentTM);

//AWPersistable virtuals
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

public:
	//state data specific to Boid controller
	AWPoint				m_dir;		//current direction
	AWPoint				m_dPos;		//delta posn
	AWPoint				m_dDir;		//delta direction
	int					m_dCount;	//number of influencing boids
	float				m_speed;	//current speed
	AWPoint				m_euler;	//euler orientation
	float				m_dYaw;		//delta yaw
	//these should come from the parent AWFlock, methinks
	float				m_angleTweak;
	float				m_normalSpeed;
	float				m_pitchToSpeedRatio;
	float				m_turnSpeedup;
	AWPoint				m_globalGoal;
};//class AWBoidController : public AWPersistable


#endif //sentinel __AW_BOID_CONTROLLER_H


