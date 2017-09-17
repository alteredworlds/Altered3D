#include "AWBoidController.h"


#define VER_AWBoidController_1	100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWBoidController, CLSID_AWBOIDCONTROLLER, VER_AWBoidController_1);


AWBoidController::AWBoidController(float t)
{
	m_dCount = 0;	//number of influencing boids
	m_speed  = 0.0f;	//current speed
	m_dYaw   = 0.0f;		//delta yaw
	//these should come from the parent AWFlock, methinks
	m_angleTweak		= 0.02f;
	setLastUpdate(t);
	
}


AWBoidController::~AWBoidController()
{
}




int						
AWBoidController::calcTransform(float t, const AWMatrix4& parentTM)
{	//the member data for PRS will have been updated by the draw thread
	//this fn. should perform any calculations required of this data
	//calculate the local transform data for this frame 
	float deltaT = t - getLastUpdate();
	setLastUpdate(t);
    if( m_dCount )
    {
        m_dDir /= (float)m_dCount;	//average accorss all contributing boids
        m_dDir -= m_dir;			//find displacement: cur dir to new dir
        m_dDir *= 1.5f;				//weight relative to collision & centering
    }
    AWPoint vDelta = m_dDir + m_dPos;
    AWPoint vOffset;

    // add in the influence of the global goal
    AWPoint vGoal = 0.5 * (m_globalGoal-getPosn()).normalize();
    vDelta += vGoal;

	/*no obstacles yet
    // add in any obstacles
    for( int j=0; j<NUM_OBSTACLES; j++ )
    {
        AWPoint vOb = m_vLoc - g_vObstacleLocations[j];
        float     fDist = Magnitude(vOb);

        if( fDist > 2*OBSTACLE_RADIUS )
            continue;

        vOb /= fDist;   // normalize
        fDist = 1.0f - fDist/(2*OBSTACLE_RADIUS);
        vDelta += fDist * vOb * 5.0f;
    }*/

    // first deal with pitch changes
    if( vDelta.y > 1.8f )
    {           // we're too low
        m_euler.x += m_angleTweak;
        if( m_euler.x > 0.8f )
            m_euler.x = 0.8f;
    }
    else if( vDelta.y < -1.8f )
    {   // we're too high
        m_euler.x -= m_angleTweak;
        if( m_euler.x < -0.8f )
            m_euler.x = -0.8f;
    } 
    else
    {
        // add damping
        m_euler.x *= 0.98f;
    }

    // speed up or slow down depending on angle of attack
    m_speed -= m_euler.x * m_pitchToSpeedRatio;
    // damp back to normal
    m_speed = (m_speed-m_normalSpeed)*0.99f + m_normalSpeed;

    if( m_speed < m_normalSpeed/2 )
        m_speed = m_normalSpeed/2;
    if( m_speed > m_normalSpeed*5 )
        m_speed = m_normalSpeed*5;

    // now figure out yaw changes
    vOffset    = vDelta;
    vOffset.y  = 0.0f;
    vDelta     = m_dir;
    vOffset.normalize();
    float fDot = dot( vOffset, vDelta );
    
    // speed up slightly if not turning much
    if( fDot > 0.7f )
    {
        fDot -= 0.7f;
		//how is next line affected by the scale of my scene (ie m_speed)
        m_speed += fDot * m_turnSpeedup;
    }
    vOffset = cross( vOffset, vDelta );
	//what are these magic numbers?
    fDot = (1.0f-fDot)/2.0f * 0.07f;
    if( vOffset.y > 0.05f )
        m_dYaw = (m_dYaw*19.0f - fDot) * 0.05f;
    else if( vOffset.y < -0.05f )
        m_dYaw = (m_dYaw*19.0f + fDot) * 0.05f;
    else
        m_dYaw *= 0.98f; // damp it

    m_euler.y += (float)(m_dYaw/5.0);
    m_euler.z = (float)(-m_dYaw*2.0);

	//now update the parentTM with localTM information to get wsTM
	AWQuat rotn;
	rotn.fromEuler((float)(360.0 / 2.0 * M_PI) * m_euler);
	m_wsTM.identity();	
	m_wsTM.awTranslate(m_posn.getVal());
	m_wsTM.awRotate(rotn);
	m_wsTM.awScale(m_scale.getVal());
    m_wsTM.getDirection(m_dir);
	m_dir.normalize();
	m_dir = -m_dir;
    setPosn(getPosn() + deltaT * m_speed * m_dir);
	return 1;
}//int AWBoidController::calcTransform(float t, const AWMatrix4& parentTM)



//AWPersistable virtuals
void				
AWBoidController::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWBoidController_1 == version)
	{
		AWTransformController::restoreGuts(file);
		m_dir.restoreGuts(file);		//current direction
		m_dPos.restoreGuts(file);		//delta posn
		m_dDir.restoreGuts(file);	//delta direction
		file.Read(m_dCount);	//number of influencing boids
		file.Read(m_speed);	//current speed
		m_euler.restoreGuts(file);	//euler orientation
		file.Read(m_dYaw);		//delta yaw
		file.Read(m_angleTweak);
		file.Read(m_normalSpeed);
		file.Read(m_pitchToSpeedRatio);
		file.Read(m_turnSpeedup);
		m_globalGoal.restoreGuts(file);
	}
}//void AWBoidController::restoreGuts(AWFile& file)




void        		
AWBoidController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
	m_dir.saveGuts(file);		//current direction
	m_dPos.saveGuts(file);		//delta posn
	m_dDir.saveGuts(file);	//delta direction
	file.Write(m_dCount);	//number of influencing boids
	file.Write(m_speed);	//current speed
	m_euler.saveGuts(file);	//euler orientation
	file.Write(m_dYaw);		//delta yaw
	file.Write(m_angleTweak);
	file.Write(m_normalSpeed);
	file.Write(m_pitchToSpeedRatio);
	file.Write(m_turnSpeedup);
	m_globalGoal.saveGuts(file);
}//void AWBoidController::saveGuts(AWFile& file) const




AWspace     		
AWBoidController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
					 AWTransformController::binaryStoreSize();
	retVal += sizeof(m_dir) +		//current direction
				sizeof(m_dPos) +		//delta posn
				sizeof(m_dDir) +		//delta direction
				sizeof(m_dCount) +	//number of influencing boids
				sizeof(m_speed) +	//current speed
				sizeof(m_euler) +	//euler orientation
				sizeof(m_dYaw) +		//delta yaw
				//these should come from the parent AWFlock, methinks
				sizeof(m_angleTweak) +
				sizeof(m_normalSpeed) +
				sizeof(m_pitchToSpeedRatio) +
				sizeof(m_turnSpeedup) +
				sizeof(m_globalGoal);
	return retVal;
}//AWspace AWBoidController::binaryStoreSize() const

