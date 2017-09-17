#include "AWFlock.h"
#include "AWBoidController.h"
#include "AWGLRenderer.h"
#include "time.h"
#ifndef _WIN32
#include <stdlib.h>
#endif

#define VER_AWFlock_1	100
#define VER_AWFlock_2	101
#define VER_AWFlock_3   102
#define VER_AWFlock_4   103
#define VER_AWFlock_5   104

AWDEFINE_VERSIONABLE_PERSISTABLE(AWFlock, CLSID_AWFLOCK, VER_AWFlock_5);


#ifdef _WIN32
float rnd() {return (float)rand()/(float)RAND_MAX;}
#else
double rnd() {return (double)random()/(double)RAND_MAX;}
#endif



AWFlock::AWFlock(const AWCString& name, AWNode* parent)
				: AWNode(name, parent), 
				  m_boidDistances(NULL),
				  m_srceObj(NULL),
				  m_ownsSrceObj(FALSE),
				  m_numBoids(0),
				  m_minY(0.0f),
				  m_maxY(0.0f),
                  m_enabled(TRUE),
                  m_timeout(0.0),
                  m_renderer(NULL)
{
	//should be TRUE by default, but just make sure...
	//this changed when scene graph became owned by m_root
	setOwnsChildren(TRUE);
}


AWFlock::~AWFlock()
{
	delete [] m_boidDistances;
   m_boidDistances = NULL;
	if (m_ownsSrceObj)
   {
		delete m_srceObj;
   }
   m_srceObj = NULL;
   m_renderer=NULL;
}//AWFlock::~AWFlock()


float&
AWFlock::getDistance(int row, int col)
{	//this function WILL TRAP if init() not called yet
	//with a non-zero number of kids
	return m_boidDistances[col * getNumChildren() + row];
}


void
AWFlock::scale(const AWPoint& scale)
{
	AWNode::scale(scale);
	m_maxY *= scale.y;
	m_minY *= scale.y;
}//void AWFlock::scale(const AWPoint& scl)


void
AWFlock::reset(double t)
{
   AWTRACE1("\nFlock %s reset\n", getName().c_str());
   delete m_boidDistances;
   m_boidDistances = NULL;
   m_children.destroy();
}//void AWFlock::reset(float t)


int 
AWFlock::populate(double t, AWGLRenderer& renderer)
{
    //due to new flock reset code, we may have children already
    //and thus may have executed this fn. once already
	//populate the flock with m_numBoids clones of m_srceObj
	//and assign a boid controller to each
    reset(t);
    if (!m_srceObj) return 0;
    m_srceObj->init(t, renderer);
	if (m_srceObj && m_numBoids)
	{	//array will hold the relative boid distances
		if (!m_boidDistances)
            m_boidDistances  = new float[m_numBoids * m_numBoids];
		//now go through and add a clone of m_srceObj 
        //until we have m_numBoids children
		AWObject*		  nextBoid    = NULL;
		for (int i = 0; i < m_numBoids; i++)
		{	//create the AWObject and clone the source for the boid
			nextBoid = (AWObject*)m_srceObj->clone();       
			//now assign an AWBoidController
			nextBoid->setController(new AWBoidController(t));
			addChild(nextBoid);
		}//for (int i = 0; i < m_numBoids; i++)
      AWTRACE3("\nFlock %s populated with %i clones of %s\n", getName().c_str(), m_numBoids, m_srceObj->getName().c_str());
	}//if (m_srceObj && m_numBoids)
    return 0;
}//int AWFlock::populate(float t, GLRenderer& renderer)



void
AWFlock::init(double time, AWGLRenderer& renderer)
{
   m_renderer = &renderer;
   //need to know number of boids, and object to be used as clone source
   //for now, assume that m_numBoids is valid
   setAnimStartTime(time, FALSE);
   setLastUpdate(0.0);
   if (m_srceObj &&m_boidDistances && m_numBoids && (m_numBoids == getNumChildren()))
   {	
      //fill the global data for the flock
      AWTRACE2("Flock srce %s has radius %f\n", m_srceObj->getName().c_str(), (double)m_srceObj->getRadius());
      float boidRadius = m_srceObj->getRadius();
      float scaleFactor = boidRadius / 0.707107f;
      m_influenceRadiusSquared = m_influenceRadius*m_influenceRadius*boidRadius*boidRadius;	
      if (m_collisionFraction==1.0f)
      {	//prevent divide by zero error
         m_collisionFraction=0.99f;
      }
      m_invCollisionFraction	 = 1.0f/(1.0f - m_collisionFraction);
      //setup initial data on each boid
      AWObject*			nextBoid	= NULL;
      AWBoidController*	boidControl = NULL;
      AWPoint				nextPosn;
      srand( 100);//(unsigned)time( NULL )  );
      if (m_startPosn == AWPoint(0.0f, 0.0f, 0.0f))
      {
         getPosn(m_startPosn);
      }
      AWTRACE3("Flock initial posn (%f.2, %f.2, %f.2)\n", (double)m_startPosn.x, (double)m_startPosn.y, (double)m_startPosn.z);
      for (int i = 0; i < getNumChildren(); i++)
      {
         //get the next boid
         nextBoid = (AWObject*)getChild(i);
         //get the AWBoidController
         boidControl = (AWBoidController*)nextBoid->getController();
        
         nextPosn = m_startPosn;
         nextPosn += AWPoint(scaleFactor*(float)m_numBoids*(rnd()-rnd()), 
            2.0f * scaleFactor * rnd(), 
            scaleFactor * -(float)m_numBoids*(rnd()-rnd()));
         boidControl->setPosn( nextPosn );
         AWPoint dir(rnd()-rnd(), rnd()-rnd(), rnd()-rnd());
         dir.normalize();
         boidControl->m_dir = dir;
         boidControl->m_speed			 = m_normalSpeed * boidRadius;
         boidControl->m_normalSpeed		 = m_normalSpeed * boidRadius;
         boidControl->m_pitchToSpeedRatio = 0.001f * boidRadius;
         boidControl->m_turnSpeedup		 = 0.1f * boidRadius;
         //following may not be necessary
         boidControl->m_dPos				 = AWPoint(0.0f, 0.0f, 0.0f);
         boidControl->m_dDir				 = AWPoint(0.0f, 0.0f, 0.0f);
         boidControl->m_euler			 = AWPoint(0.0f, 0.0f, 0.0f);
         boidControl->m_dCount			 = 0;
         boidControl->m_dYaw				 = 0.0f;
         // for effect, we can set anim start times of
         // each boid to random val in range [-0.5..0.5] 
         nextBoid->setAnimStartTime(rnd() - 0.5, FALSE);
         AWTRACE4("  child %2i pos (%.2f,%.2f,%.2f)", i, (double)nextPosn.x, (double)nextPosn.y, (double)nextPosn.z);
         AWTRACE4("  dir (%.2f, %.2f, %.2f)   speed %.3f\n",(double)dir.x, (double)dir.y, (double)dir.z, (double)boidControl->m_speed);
      }//for (int i = 0; i < m_numBoids; i++)
      AWTRACE("\n");
   }//if (m_srceObj && m_numBoids)
}//void AWFlock::init(float t, GLRenderer& renderer)



int		
AWFlock::calcTransform(double time, const AWMatrix4& parentTM)
{ 
    double useTime = time - m_animStart;
	if (useTime < 0) useTime = 0;
    if (getEnabled())
    {    
        if(0 == getNumChildren())
        {   //iff ENABLED and no children, => all destroyed last frame.
            setEnabled(FALSE);
            //if (m_renderer) m_renderer->setSwitchLevel();
            return 1;
        }
        else if (m_timeout && (useTime > m_timeout))
        {   //user has exceeded level timeout, so need to take appropriate action
            //if (m_renderer) m_renderer->setLevelFailed();
            return 1;
        }
    }//if (getEnabled())
	if (m_boidDistances)
	{
		AWPoint globalGoal(0.0f, 0.0f, 0.0f);
		float fDist;
		AWBoidController* boidI = NULL;
		AWBoidController* boidJ = NULL;
		int numKids = getNumChildren();
		if (m_transformer)
		{
			m_transformer->calcTransform(useTime, parentTM);
			m_transformer->getPosn(globalGoal);
		}
		int i =0;
		// first update the dist array 0.0..1.0 with 0.0 being furthest away
		for( i=0; i < numKids; i++ )
		{
			boidI = (AWBoidController*)getChild(i)->getController();
			for( int j=i+1; j < numKids; j++ )
			{
				boidJ = (AWBoidController*)getChild(j)->getController();
				fDist = ( boidI->getPosn() - boidJ->getPosn() ).sqrMagnitude();
				fDist = m_influenceRadiusSquared - fDist;
				if( fDist < 0.0f )
					fDist = 0.0f;
				else
					fDist /= m_influenceRadiusSquared;
				getDistance(i, j) = getDistance(j, i) = fDist;
			}//for( int j=i+1; j < m_numBoids; j++ )
			getDistance(i, i) = 0.0f;
			boidI->m_dDir   = AWPoint( 0.0f, 0.0f, 0.0f );
			boidI->m_dPos   = AWPoint( 0.0f, 0.0f, 0.0f );
			boidI->m_dCount = 0;
			boidI->m_globalGoal = globalGoal;
		}//for( int i=0; i < m_numBoids; i++ )
		AWPoint boidIPos;
		for( i=0; i < numKids; i++ )
		{
			boidI = (AWBoidController*)getChild(i)->getController();
			boidI->getPosn(boidIPos);
			//values of 0 for minY and maxY mean DO NOT USE
			if (m_maxY && (boidIPos.y > m_maxY))
			{
				boidI->m_dPos.y -= boidIPos.y - m_maxY;
			}
			else if (m_minY && (boidIPos.y < m_minY))
			{
				boidI->m_dPos.y += m_minY - boidIPos.y;
			}
			for( int j=i+1; j < numKids; j++ )
			{	// if i is near j have them influence each other
				if (getDistance(i,j) > 0.0f)
				{
					boidJ = (AWBoidController*)getChild(j)->getController();
					AWPoint vDiff(boidI->getPosn() - boidJ->getPosn());
					vDiff.normalize();

					AWPoint vDelta;
					float   fCollWeight = 0.0f;     // collision weighting

					// only do collision testing against the nearest ones
					if( getDistance(i,j) - m_collisionFraction > 0.0f )
						fCollWeight = (getDistance(i,j) - m_collisionFraction) * m_invCollisionFraction;

					// add in a little flock centering
					if( getDistance(i,j) - (1.0f-m_collisionFraction) > 0.0f )
						fCollWeight -= getDistance(i,j) * (1.0f-fCollWeight);

					vDelta = fCollWeight * vDiff;

					// add in the collision avoidance
					boidI->m_dPos += vDelta;
					boidJ->m_dPos -= vDelta;

					// add in the velocity influences
					boidI->m_dDir += getDistance(i,j) * boidJ->m_dir;
					boidJ->m_dDir += getDistance(i,j) * boidI->m_dir;
					boidI->m_dCount++;
					boidJ->m_dCount++;
				}//if (getDistance(i,j) > 0.0f)
			}//for( int j=i+1; j < numKids; j++ )
		}//for( i=0; i < numKids; i++ )
	}//if (m_boidDistances)
	//now that all data is prepared, calc. the usual transform for this node
	//and then call calc transform for each child. i.e. call the base class...
	//now iterate through children and calculate their transforms
	for (int i = 0; i < (int)m_children.getNum(); i++)
	{
		getChild(i)->calcTransform(useTime, parentTM);
	}//for (int i = 0; i < m_children.entries(); i++)
	return 1;
}//int AWFlock::calcTransform(double t, const AWMatrix4& parentTM)




void        
AWFlock::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
    if (VER_AWFlock_4 >= version)
    {
	    AWNode::restoreGuts(file);
	    file >> m_srceObj;
	    file >> m_ownsSrceObj;
	    file >> m_numBoids;
	    file >> m_collisionFraction;
	    file >> m_influenceRadius;	//as multiple of boid radius
	    file >> m_normalSpeed;
	    if (VER_AWFlock_1 != version)
	    {
		    file >> m_maxY;
		    file >> m_minY;
	    }
        if (version >= VER_AWFlock_3)
        {
            file >> m_enabled;
        }
        else m_enabled = TRUE;
        if (version >= VER_AWFlock_4)
        {
            if (version < VER_AWFlock_5)
            {
                float tmp;
                file >> tmp;
                m_timeout = tmp;
            }
            else
                file >> m_timeout;
        }
        else m_timeout = 0.0;
    }
}//void AWFlock::restoreGuts(AWFile& file) const



void        
AWFlock::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWNode::saveGuts(file);
	file << m_srceObj;
	file << m_ownsSrceObj;
	file << m_numBoids;
	file << m_collisionFraction;
	file << m_influenceRadius;	//as multiple of boid radius
	file << m_normalSpeed;
	file << m_maxY;
	file << m_minY;
    file << m_enabled;
    file << m_timeout;
}//void AWFlock::saveGuts(AWFile& file) const




AWspace     
AWFlock::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWNode::binaryStoreSize() + 
		AWPERSISTABLE_POINTER_SIZE(m_srceObj) +
		sizeof(m_ownsSrceObj) +
		sizeof(m_numBoids) +
		sizeof(m_collisionFraction) +
		sizeof(m_influenceRadius) +
		sizeof(m_normalSpeed) +
		sizeof(m_maxY) +
		sizeof(m_minY) +
        sizeof(m_enabled) +
        sizeof(m_timeout);
	return retVal;
}//AWspace  AWFlock::binaryStoreSize() const
