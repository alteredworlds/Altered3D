#ifndef __AWFLOCK_H
#define __AWFLOCK_H

#include "AWToolsCommon.h"
#include "AWNode.h"
#include "AWObject.h"

class AWExport GLRenderer; //forward


class AWExport AWFlock : public AWNode
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWFlock);

	AWFlock(const AWCString& name = "", AWNode* parent=NULL);
	virtual		~AWFlock();

	virtual void compact();
	int		     populate(float t, GLRenderer& renderer);	//populate flock with clones, returns num added
	virtual void init(float t, GLRenderer& renderer);		//initialise the clone values
    virtual void reset(float t);    //put back to state before populate

	virtual void scale(const AWPoint& scale);

	void		setCloneSrce(AWObject* srce)	{m_srceObj=srce;}
	void		setOwnsSrce(AWBoolean val)		{m_ownsSrceObj=val;}
	void		setNumBoids(int num)			{m_numBoids=num;}
	void		setCollideWeight(float v)		{m_collisionFraction=v;}
	void		setInfluenceRadius(float v)		{m_influenceRadius=v;}
	void		setNormalSpeed(float v)			{m_normalSpeed=v;}
	void		setMaxY(float v)				{m_maxY=v;}
	void		setMinY(float v)				{m_minY=v;}
    void        setEnabled(AWBoolean val)       {m_enabled=val;}
    void        setTimeout(float v)             {m_timeout=v;}

	int			getNumBoids()			{return m_numBoids;}
	AWBoolean	getOwnsSrce()			{return m_ownsSrceObj;}
	float		getCollideWeight()		{return m_collisionFraction;}
	float		getInfluenceRadius()	{return m_influenceRadius;}
	float		getNormalSpeed()		{return m_normalSpeed;}
	float		getMinY()				{return m_minY;}
	float		getMaxY()				{return m_maxY;}
    AWBoolean   getEnabled()            {return m_enabled;}
    float       getTimeout()            {return m_timeout;}

	//void		scale(const AWPoint& scale);
	//void		updateTransform(const AWPoint& euler, float speed);
	//update the object state with the values for the specified time
	virtual int	calcTransform(float t, const AWMatrix4& parentTM);

	virtual void restoreGuts(AWFile& file);
    virtual void saveGuts(AWFile& file) const;
	virtual AWspace binaryStoreSize() const;
	//AWBoolean		isEqual(const AWPersistable* o) const{return m_name == ((AWFlock*)o)->m_name;}
	//int			compareTo(const AWPersistable* o) const{return isEqual(o) ? 0 : -1;}

protected:
	//MEMBER FUNCTIONS (HELPERS)
	float&	getDistance(int row, int col);
	//MEMBER DATA
	//number of boids in the flock is the number of children
	float*	m_boidDistances; //will be float[numBoids*numBoids]
	//global goal (as a point) is given by m_posn
	float	m_influenceRadiusSquared;
	float	m_invCollisionFraction;
	//the following data is persisted, & transferred from MAX
	//SOURCE OBJECT to be cloned for the flock
	AWObject*	m_srceObj;
	AWBoolean	m_ownsSrceObj;
    AWBoolean   m_enabled;
	int			m_numBoids;
	float		m_collisionFraction;
	float		m_influenceRadius;	//as multiple of boid radius
	float		m_normalSpeed;		//as multiple of boid radius per second
	float		m_minY;
	float		m_maxY;
    float       m_timeout;      //really just for savergames...
    AWPoint     m_startPosn;
    GLRenderer* m_renderer;
};//class AWExport AWFlock : public AWNode


inline void
AWFlock::compact()
{	//prevent happening twice for m_srceObj (which doesn't really matter)
	if (m_ownsSrceObj && m_srceObj)
		m_srceObj->compact();
}

#endif //__AWFLOCK_H