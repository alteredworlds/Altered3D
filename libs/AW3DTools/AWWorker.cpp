#include "AWQuat.h"
#include "AWGLRenderer.h"
#include "AWWorker.h"
#include "AWFlightController.h"
#include "AWFlock.h"
#include "AWRocket.h"
//next two needed by initbillboardednodes only - should be 
//moved ASAP
#include "AWFog.h"
#include "AWSea.h"
 

#define VER_AWWORKER_1 102
#define VER_AWWORKER_2 103
#define VER_AWWORKER_3 104
#define VER_AWWORKER_4 105
#define VER_AWWORKER_5 106
#define VER_AWWORKER_6 107
#define VER_AWWORKER_7 108


AWDEFINE_VERSIONABLE_PERSISTABLE(AWWorker, CLSID_AWWORKER, VER_AWWORKER_7);


AWWorker::AWWorker() : m_firstTime(TRUE), m_curProjectile(NULL), 
                        m_curCamera(NULL), m_userTransform(NULL),
                        m_environment(NULL),
                        m_transparentObjects(5),
                        m_alphaTestObjects(5),
                        m_projectiles(1), m_flockIndex(0),
                        m_levelEndAdded(FALSE), m_levelEnd(NULL),
                        m_timerBarCtl(NULL)
{
   m_objectList.setOwnsObjects(FALSE);
	m_lightList.setOwnsObjects(FALSE);
	m_cameraList.setOwnsObjects(FALSE);
   m_alphaTestObjects.setOwnsObjects(FALSE);
	m_transparentObjects.setOwnsObjects(FALSE);
	m_flockList.setOwnsObjects(FALSE);
}



void
AWWorker::compactNodes(AWOrdered& list)
{
	list.pack();
	for (int i=0; i<(int)list.getNum();i++)
	{
		((AWNode*)list[i])->compact();
	}
}//void AWWorker::compactNodes(AWOrdered& list)



void
AWWorker::compact()
{
	m_root.compact();
	compactNodes(m_projectiles); //current projectile will now be part of m_projectiles list	
	if (m_environment)
   {
		m_environment->compact();
   }
}

void								 
AWWorker::clearAndDestroy()
{
	m_firstTime = TRUE;
	if (m_environment && !m_environment->getParent())
	{	//environment, if  present, should be parented by the
		//curent camera and deleted along with it (below)
		//but just in case we're in an anomolous state, get it here
		//if it doesn't have a parent
		delete m_environment;
	}//if (m_environment && !m_environment->getParent())
	m_environment = NULL; 
	m_root.clearAndDestroy(); //owns all (active) nodes - deletes them all
   //the following collections are of type AWOrdered and therefore
   //always write their contents, which is the desired behavior.
   //however, they are set 
	m_objectList.destroy();
	m_lightList.destroy();
	m_cameraList.destroy();
   m_alphaTestObjects.destroy();
	m_transparentObjects.destroy();
	m_flockList.destroy();
	//now deal with non-transformed / inactive / resource nodes
	m_projectiles.destroy(); //clone sources for projectiles

	m_max		= AWPoint(0.0f, 0.0f, 0.0f);
	m_min		= AWPoint(0.0f, 0.0f, 0.0f);
	//note that m_curProjectile is in m_projectiles and mustn't be double deleted
	m_curProjectile = NULL;
	m_curCamera = NULL; //owned by the camera list
	m_userTransform = NULL; //owned by camera
    if (!m_levelEndAdded)
    {
        delete m_levelEnd;
    }
    m_levelEndAdded = FALSE;
    m_levelEnd      = NULL;
    m_timerBarCtl   = NULL; //always owned by AWNode
}//void AWWorker::clearAndDestroy()



void           
AWWorker::setUseAlphaTest(AWBoolean useAlpha)
{  //TRUE branch of this still needs to be written -
   //for now we only care about turning it OFF
   if (!useAlpha)
   {  //go through all the alpha test list 
      //remove each item and 
      //add to the alpha blended list
      for (int i=0; i<(int)m_alphaTestObjects.getNum(); i++)
      {
         m_transparentObjects.add(m_alphaTestObjects[i]);
      }
      //re: ownership - should be the case anyway, but make sure...
      m_alphaTestObjects.setOwnsObjects(FALSE); 
      m_alphaTestObjects.destroy();
   }//if (!useAlpha)
}//void AWWorker::setUseAlphaTest(AWBoolean useAlpha)


AWLight*			   
AWWorker::getLightByName(const AWCString& name)
{
   AWLight* retVal = NULL;
   for (int l=0; l<getNumLights(); l++)
   {
      retVal =  getLight(l);
      if ( retVal && (name == retVal->getName()))
		{
			break;		
		}//if ( retVal && (name == retVal->getName()))
		else retVal = NULL;
   }//for (int l=0; l<getNumLights(); l++)
   return retVal;
}//AWLight*  AWWorker::getLightByName(const AWCString& name)


AWObject*
AWWorker::getObjectByName(const AWCString& name)
{
	AWObject* retVal = NULL;
	//now find the AWObject with this name, if any
   //looks in Opaque, Alpha Test & Transparent object collections
   for (int i=0; i<getNumObjects(); i++)
   {
      retVal =  getObject(i);
      if ( retVal && (name == retVal->getName()))
		{
			break;		
		}//if ( pBoid && (testName == boidName))
		else retVal = NULL;
   }//for (int i=0; i<getNumObjects(); i++)
	return retVal;
}//AWObject* AWWorker::getObjectByName(const AWCString& name)



void				
AWWorker::setEnvironment(AWObject* env) 
{
	m_environment=env;
}//void	AWWorker::setEnvironment(AWObject* env)



void
AWWorker::init(float time, GLRenderer& renderer)
{	//reset all evens in the scene to time    
   m_root.init(time, renderer);

   AWRocket* nextRocket = NULL;
   for (int i=0; i<(int)m_projectiles.getNum(); i++)
   {
      if (nextRocket = getProjectile(i))
      {
         nextRocket->init(time, renderer);
         nextRocket->setRenderer(&renderer);
      }//if (nextRocket = getProjectile(i))
   }//for (int i=0; i<(int)m_projectiles.getNum(); i++)
   if (m_levelEnd) m_levelEnd->init(time, renderer);
}//void AWWorker::init(float time, GLRenderer& renderer)




void				 
AWWorker::startup(float t, GLRenderer& renderer)
{
   //calculate object face, radius data
   init(0.0f, renderer);	//int all scene objects
   //must initialize all AWFlock objects
   for (int flock=0; flock<(int)m_flockList.getNum(); flock++)
   {
      AWFlock* curFlock = (AWFlock*)m_flockList[flock];
      if (!curFlock->getNumChildren())
      {   
         //a saved scene may have the flock already populated and so on -
         //test for this by seeing if it has any children.
         //NOTE - a Flock object in MAX should NOT have any children
         //first need to find the correct AWObject which is the clone source for
         //the flock, and extract from m_objectList if desired
         initFlock(curFlock, renderer);
         if (curFlock->getEnabled())
         {  //if this flock is enabled, populate the flock from 
            //the clone source
            curFlock->populate(t, renderer);
            AWTRACE2("\nstartup populated flock %15s with %i members\n", curFlock->getName().c_str(), curFlock->getNumChildren());
            curFlock->init(t, renderer);
            //and add all the generated child (flock member) objects to the display list
            for (int child=0; child<curFlock->getNumChildren(); child++)
            {
               addNodeAndChildrenToFlatList(curFlock->getChild(child));
            }//for (int child=0; child<curFlock->getNumChildren(); child++)
         }//if (curFlock->getEnabled())
      }//if (!curFlock->getNumChildren())
   }//for (int flock=0; flock<m_flockList.getNum(); flock++)
   sortObjectsByTexture(); //do ONCE for now
}//void	AWWorker::startup(float t, GLRenderer& renderer)



void                
AWWorker::setTargetFlock(int flk)
{   //using a zero based index, enable the desired flock
    if (flk < (int)m_flockList.getNum())
    {
        m_flockIndex = flk;
        for (int flock=0; flock<(int)m_flockList.getNum(); flock++)
	    {
		    AWFlock* curFlock = (AWFlock*)m_flockList[flock];
            curFlock->setEnabled( flk == flock );
        }//for (int flock=0; flock<m_flockList.getNum(); flock++)
    }//if (flk < m_flockList.getNum())
}//void AWWorker::setTargetFlock(int flk)



AWFlock* 
AWWorker::clearCurrentFlock()
{
    AWFlock* curFlock = (AWFlock*)m_flockList[m_flockIndex];
    AWTRACE2("\n   clearing %i members from flock %i\n", curFlock->getNumChildren(), m_flockIndex);
    for (int child=0; child<curFlock->getNumChildren(); child++)
	{
		removeNodeAndChildrenFromFlatList(curFlock->getChild(child));
      delete curFlock->removeChildAt(child--);
	}//for (int child=0; child<curFlock->getNumChildren(); child++)
    return curFlock;
}//AWFlock* AWWorker::clearCurrentFlock()



float                
AWWorker::playTransition(float t, int i)
{
   float retVal = 0.0f;
   if (!m_levelEnd) return retVal;
   m_levelEnd->setAnimStartTime(t, FALSE);
   if (!m_levelEndAdded)
   {  //if not yet added to the scene, add now.
      addNode(m_levelEnd);
      m_levelEndAdded = TRUE;
   }//if (!m_levelEndAdded)
   //now find the camera for this transition, if any
   //and select it. Note that this lousy code doesn't recurse into
   //children - just looks at the direct children of this node
   AWNode* obj = NULL;
   for (int index=0; index < m_levelEnd->getNumChildren(); index++)
   {
      obj = m_levelEnd->getChild(index);
      if (obj && (CLSID_AWCAMERA == obj->isA()))
      {
         setCurCamera((AWCamera*)obj);
         AWTransformController* cntrl = obj->getController();
         if (cntrl)
         {
            cntrl->setIsLooped(FALSE);
            retVal = cntrl->getAnimationDuration();
         }
      }//if (obj && (CLSID_AWCAMERA == obj->isA()))
   }//for (int index=0; index<m_levelEnd->getNumChildren(); index++)  
   return retVal;
}//void AWWorker::playTransition(float t, int i)



void 
AWWorker::resetLevel(float t, GLRenderer renderer)
{
    int numFlocks = (int)m_flockList.getNum();
    if (numFlocks && (m_flockIndex < numFlocks) && (m_flockIndex >= 0))
    {
        int child=0;
        AWFlock* useFlock    = (AWFlock*)m_flockList[m_flockIndex];
        for (child=0; child<useFlock->getNumChildren(); child++)
	    {
		    removeNodeAndChildrenFromFlatList(useFlock->getChild(child));
	    }//for (int child=0; child<curFlock->getNumChildren(); child++) 
        useFlock->populate(t, renderer);  //also removes existing children
        useFlock->init(t, renderer);
	    //and add all the generated child (flock member) objects to the display list
	    for (child=0; child<useFlock->getNumChildren(); child++)
	    {
		    addNodeAndChildrenToFlatList(useFlock->getChild(child));
	    }//for (int child=0; child<curFlock->getNumChildren(); child++)   
    }//if (numFlocks && (m_flockIndex < numFlocks) && (m_flockIndex >= 0))
    else m_flockIndex++; //if this path occured, put things back the way they were
}//void AWWorker::resetLevel(float t, GLRenderer renderer)



void
AWWorker::initFlock(AWFlock* flock, GLRenderer renderer)
{
	if (flock)
	{
		AWCString flockName(flock->getName());
		//get the boid name, if any
		AWObject* pBoid = NULL;
		int index = flockName.find("*");
      if (string::npos != index)
		{
         AWCString boidName(flockName);
         boidName.replace(0, index+1, "");
         //flockName.substr(index+1, flockName.length()-index-1));
			flockName.replace(index, flockName.length()-index, "");
			flock->setName(flockName);
			pBoid = getObjectByName(boidName);
		}//if (AW_NPOS != index)		
		//and once found, set the AWObject pointer in the AWFlock
		if (flock->getOwnsSrce())
		{
			removeNode(pBoid);//watch out here with new scene design - careful
			flock->setCloneSrce(pBoid);
		}//if (curFlock->getOwnsSrce())
		else
		{//need to make a FULL COPY
			AWObject* temp = (AWObject*)pBoid->copy();
			temp->init(0.0f, renderer);
			flock->setCloneSrce(temp);
			//and now transfer ownership of this copy
			//to the flock. NOTE - this ensures that
			//should the original object vanish from
			//the scene due to (for example) being
			//destroyed, the flock can still function.
			//To avoid a memory leak, the flock must delete
			//this new clone srce on destruction
			flock->setOwnsSrce(TRUE);
		}
	}//if (flock)
}//void AWWorker::initFlock(AWFlock* flock, GLRenderer renderer)



void
AWWorker::initTimeBar(float timeout, float timeNow)
{
    if (m_timerBarCtl)
    {
        m_timerBarCtl->setInterval(timeout);
        m_timerBarCtl->init(timeNow);
    }//if (m_timerBarCtl)
}//void AWWorker::initTimeBar(float timeout, float timeNow)


void
AWWorker::resetTimeBar(float timeNow)
{
    if (m_timerBarCtl)
    {
        m_timerBarCtl->init(timeNow);
    }//if (m_timerBarCtl)
}//void AWWorker::resetTimeBar(float timeNow)



void							 
AWWorker::waitForObject(int i)
{
#ifndef AW_SINGLE_THREADED_ENGINE
	//(*(AWObject*)m_objectList.at(i)).acquire();
#endif
}//void AWWorker::waitForObject(int i)


void			
AWWorker::releaseObject(int i)
{
#ifndef AW_SINGLE_THREADED_ENGINE
	//(*(AWObject*)m_objectList.at(i)).release();
#endif
}//void AWWorker::releaseObject(int i)



void				
AWWorker::addProjectile(AWRocket* rocket)
{
	if (rocket)
	{
		m_projectiles.add(rocket);
	}
}




AWRocket*			
AWWorker::removeProjectile(AWRocket* rocket)
{
	return (AWRocket*)m_projectiles.remove(rocket);       
}//AWRocket* AWWorker::removeProjectile(AWRocket* rocket)



void
AWWorker::addNodeToFlatList(AWNode* node)
{
	//WATCH OUT!!!! THIS NEEDS TO BE CLEANER
	//in particular, what call for 'isDerivedFrom'?
	//testing purposes -
	//AWObject* testCasting = dynamic_cast<AWObject*>(node);
	if (!node) return;
	switch (node->isA())
	{
	case CLSID_AWOBJECT:
	case CLSID_AWLODOBJECT:
	case CLSID_AWSEAOBJECT:
	case CLSID_AWROCKET:
		if (AWTexture::B_NONE == ((AWObject*)node)->getBlendType())
		{
			m_objectList.add(node);
		}
        else if (AWTexture::ALPHA_TEST == ((AWObject*)node)->getBlendType())
        {
            m_alphaTestObjects.add(node);
        }
		else
		{
			m_transparentObjects.add(node);
		}
		break;
	case CLSID_AWCAMERA:
		m_cameraList.add(node);
		break;
	case CLSID_AWLIGHT:
		m_lightList.add(node);
		break;
	case CLSID_AWFLOCK:
		m_flockList.add(node);
		break;
	default:;
	}//switch (node->isA())
}//void AWWorker::addNodeToFlatList(AWNode* node)



void 
AWWorker::addNodeAndChildrenToFlatList(AWNode* node)
{ 
	if (!node) return;
	addNodeToFlatList(node);
	for (int child = 0; child < node->getNumChildren(); child++)
	{
		addNodeAndChildrenToFlatList(node->getChild(child));
	}//for (int child = 0; child < node->getNumChildren(); child++)
}//void AWWorker::addNodeAndChildrenToFlatList(AWNode* obj)



void
AWWorker::addNode(AWNode* node, AWNode* parent)
{	//should only call this for ROOT LEVEL NODES
	if (node)
	{	//first, insert as a child into the root node or parent node if given
		if (parent)
		{
			parent->addChild(node);
		}
		else
		{//usual case of inserting into root node.
			m_root.addChild(node); //also sets node->m_parent = &m_root	
		}
		//now insert node and all of its children (if any) into the
		//relevant flat list(s)
		addNodeAndChildrenToFlatList(node);
	}//if (node)
}//void AWWorker::addNode(AWNode* node)



AWNode*				
AWWorker::removeNode(AWNode* node)
{	//detatch from parent, and remove self and all children from the
	//flat lists. Note that in new structure we should always have a parent
	if (node && node->getParent())
	{
        if (node->getParent())
        {
		    node->getParent()->removeChild(node);
            node->setParent(NULL);
        }
        removeNodeAndChildrenFromFlatList(node);
	}
	return node;
}//AWNode* AWWorker::removeNodeFromFlatList(AWNode* node)




AWNode*				
AWWorker::removeNodeFromFlatList(AWNode* node)
{
	AWNode* retVal = NULL;
	if (node)
	{	//remove from the relevant flat list
		switch (node->isA())
		{
		case CLSID_AWOBJECT:
		case CLSID_AWLODOBJECT:
		case CLSID_AWSEAOBJECT:
		case CLSID_AWROCKET:
			if (AWTexture::B_NONE == ((AWObject*)node)->getBlendType())
			{
				retVal = (AWNode*)m_objectList.remove(node);
			}
         else if (AWTexture::ALPHA_TEST == ((AWObject*)node)->getBlendType())
         {
             retVal = (AWNode*)m_alphaTestObjects.remove(node);
         }
			else
			{
				retVal = (AWNode*)m_transparentObjects.remove(node);
			}
			break;
		case CLSID_AWCAMERA:
			retVal = (AWNode*)m_cameraList.remove(node);
			break;
		case CLSID_AWLIGHT:
			retVal = (AWNode*)m_lightList.remove(node);
			break;
		case CLSID_AWFLOCK:
			retVal = (AWNode*)m_flockList.remove(node);
			break;
		default:;
		}//switch (node->isA())
	}//if (node)
	return retVal;
}//AWNode*	AWWorker::removeNodeFromFlatList(AWNode* obj)



AWNode*				
AWWorker::removeNodeAndChildrenFromFlatList(AWNode* node)
{
	AWNode* retVal = NULL;
    if (node)
    {
        removeNodeFromFlatList(node);

	    for (int child = 0; child < node->getNumChildren(); child++)
	    {	
		    removeNodeAndChildrenFromFlatList(node->getChild(child));
	    }
    }//if (node)
	return retVal;
}//AWNode* AWWorker::removeNodeAndChildrenFromFlatList(AWNode* obj)




AWFlightController* 		
AWWorker::getViewTransformer()
{
	if (m_curCamera)
		return (AWFlightController*)m_curCamera->getController();
	else return NULL;
}



void		
AWWorker::updateTransformer(const AWPoint& euler, float speed, float t)
{
   AWTRACEHI("AWWorker::updateTransformer");
	if(m_userTransform) 
   {
      AWTRACEHI4(" m_userTransform assigned - update with (%f, %f, %f) at %f",(double)euler.x, (double)euler.y, (double)euler.z, (double)t);
      m_userTransform->updateTransform(euler, speed, t);	
   }
   AWTRACEHI("\n");
}


void						
AWWorker::setTransformer(AWFlightController* f)
{
   AWTRACE1("AWWorker::setTransformer %i\n", (int)f);
	m_userTransform = f;
}//void	AWWorker::setViewTransformer(AWFlightController* f)



void						
AWWorker::setCurCamera(AWCamera* obj) 
{
	if (!obj) return;
	if (m_curCamera && (m_curCamera != obj))
	{	//re-parent any billboarded nodes to the new camera.
		//must also take care of environment, if any
		//for each child of the current camera
		//if it's a billboarded node, re-parent it (no localTM change reqd.)
		AWNode* nextNode=NULL;
		int child =0;
		while (child < m_curCamera->getNumChildren())
		{
			nextNode = m_curCamera->getChild(child);
			if (nextNode && nextNode->getController()) 
			{
				switch (nextNode->getController()->isA())
				{
				case CLSID_AWENVIRONCONTROLLER:
					//need to adjust the relative transform of environment
					//object and the (new) camera
				case CLSID_AWBILLBOARDCONTROLLER:
					//do specific stuff for this type
					//set the node's parent to be the camera
					m_curCamera->removeChildAt(child);
					child--;
					obj->addChild(nextNode);
					break;
				default:
					//most nodes remain unaffected
					break;
				}//switch
			}//if (nextNode && ...)
			child++;
		}
	}//if (m_curCamera && (m_curCamera != obj))
	m_curCamera = obj;
	if (m_curCamera)
	{
		//now initialise the angle components of the view frustrum cone from the 
		//camera FOV
		m_viewCone.setAngle(m_curCamera->m_fov);
		m_viewCone.m_farClip = m_curCamera->m_farClip;
	}
}//void	AWWorker::setCurCamera(AWCamera* obj)



AWBoolean
AWWorker::collideObjectWithWorld(AWObject* srce, float time)
{	//collides given object against known world
	AWBoolean retVal = FALSE;
	if (srce)
	{
      AWObject*	curObject = NULL;
      for (int i=0; i<getNumObjects(); i++)
      {
         curObject = getObject(i);
         if (curObject && curObject->isCollisionTarget())
			{
				srce->collideWith(curObject, time);	
			}//if (curObject && curObject->isCollisionTarget())
      }//for (int i=0; i<getNumObjects(); i++)
	}//if (srce)
	return retVal;
}//AWBoolean AWWorker::collideObjectWithWorld(AWObject* srce, float time)



void							
AWWorker::collideAllObjects(float time)
{	//now run collision detection
	//note that object traversal would be better done using
	//an iterator that would
	//(i)  hide implementation of collection(s?)
	//(ii) multithread-safe object access [allows death on another thread!]
   AWObject*	curObject = NULL;
   for (int i=0; i<getNumObjects(); i++)
   {
      curObject = getObject(i);
      if (curObject && curObject->isCollisionSource())
		{	//include only objects marked as
			//MOVING, COLLIDE WITH OTHER THINGS
			//
			//collide against rest of the world (COLLISION TARGETS)
			collideObjectWithWorld(curObject, time);
		}//if (obj && obj->isCollisionSource())
   }//for (int i=0; i<getNumObjects(); i++)
}//void	AWWorker::collideAllObjects(float time)



void
AWWorker::updateMaxValues(const AWPoint& val)
{
	if (m_firstTime) 
	{
		m_min = val;
		m_firstTime = FALSE;
	}
	else
	{
		m_max.getMax(val);
		m_min.getMin(val);
	}
}//void AWWorker::updateMaxValues(const AWPoint& val)




void							
AWWorker::center(const AWPoint& origin)
{
	AWPoint translate = origin - (m_min + (m_max - m_min) / 2.0f);
	m_min += translate;
	m_max += translate;
	m_root.offset(translate);
}




AWPoint							
AWWorker::fitToRange(const AWPoint& range)
{
	AWPoint scale;
	scale.x = range.x / (float)fabs(m_max.x-m_min.x);
	scale.y = (range.y == -1) ? scale.x : range.y / (float)fabs(m_max.y - m_min.y);
	scale.z = (range.z == -1) ? scale.x : range.z / (float)fabs(m_max.z - m_min.z);
	
	m_root.scale(scale);
	for (int nodeIndex = 0; nodeIndex < (int)m_projectiles.getNum(); nodeIndex++)
	{
		((AWNode*)m_projectiles[nodeIndex])->scale(scale);
		
	}//for nodeIndex
	if (m_levelEnd) m_levelEnd->scale(scale);
	m_min *= scale;
	m_max *= scale;
	return scale;
}//AWPoint AWWorker::fitToRange(const AWPoint& range)



void
AWWorker::processEvents(AWNode& n, float t)
{//at the moment just kills a node if its time is up
	AWNode* nextNode = NULL;
	for (int child =0; child < n.getNumChildren(); child++)
	{
		nextNode = n.getChild(child);
		if (nextNode)
		{
			if (nextNode->isAlive(t))
			{
				processEvents(*nextNode, t);
			}
			else
			{
				//time to die, node...
				//and now children - how cruel
				delete removeNode(nextNode);
				//do we need to decrement the loop counter now?
				child--;
			}
		}//if (nextNode)
	}//for (int child =0; child < n.getNumChildren(); child++)
}//void AWWorker::processEvents(AWNode& n, float t)



void							
AWWorker::transformAllNodes(float t)
{
   AWMatrix4 tmp;
	tmp.identity();
	//call virtual void AWNode::calcTransform(float t, const AWMatrix4& parentTM)
	//for all children of the root node
   AWTRACE1("\n\nCALC TRANSFORM %fs\n", (double)t);
	m_root.calcTransform(t, tmp);
	processEvents(m_root, t);
	
	//we are finally guarenteed that the viewing transform will have been calculated
	//at this point should also update the cone that approximates
	//the camera view volume, to be used in View Frustrum Culling	
	if (m_curCamera) m_curCamera->getViewCone(m_viewCone);
	//and so at this point can now process any calculations requiring
	//the view matrix (ie. VISIBILITY,   LOD)
    m_root.calcViewDependentData(t, m_viewCone);
    calcVisibility(t, m_viewCone);
	 sortTransparentObjects();
}//void	AWWorker::transformAllNodes(float frame)


void 
AWWorker::calcVisibility(float t)
{
   if (m_curCamera) m_curCamera->getViewCone(m_viewCone);
   m_root.calcVisibility(t, m_viewCone);
}//void AWWorker::calcVisibility(float t)



void 
AWWorker::calcVisibility(float t, AWSolidCone& viewCone)
{
   m_root.calcVisibility(t, viewCone);
}//void AWWorker::calcVisibility(float t, AWSolidCone& viewCone)



void 
AWWorker::swap(AWOrdered& objs, int first, int second) 
{
   AWPersistable* temp	= objs[first];
   objs[first]			= objs[second];
   objs[second]			= temp;
}//void AWWorker::swap(AWOrdered& objs, int first, int second)


/*pick one element in the array, which will be the pivot. 
make one pass through the array, called a partition step, 
re-arranging the entries so that: 
the pivot is in its proper place. 
entries smaller than the pivot are 
to the left of the pivot. 
entries larger than the pivot are to its right. 
recursively apply quicksort to the part of the array 
that is to the left of the pivot, and to the part on its right*/
void 
AWWorker::qSort( AWOrdered& objs, int lb, int ub)
{
   int splitPoint;

   if ( lb < ub) 
   {
       if (m_sortField)
       {
           reArrangeT(objs, lb, ub, splitPoint);
       }
       else
       {
           reArrange(objs, lb, ub, splitPoint);
       }
       qSort(objs, lb, splitPoint - 1);
       qSort(objs, splitPoint + 1, ub);
   }//if ( lb < ub)
}//void AWWorker::qSort( AWOrdered& objs, int lb, int ub)




void 
AWWorker::reArrange( AWOrdered& objs, int  lb, int  ub, int& splitPoint) 
{
    int i;
    int loc;

    AWObject* pivot = (AWObject*)objs[lb];
    float pivotTest = pivot->m_isVisibleThisFrame;
    float compareWith = 0.0f;

    i = lb;
    loc = ub + 1;

    while ( i < loc) 
	{
		do 
		{
			i = i + 1;
            compareWith = ((AWObject*)objs[i])->m_isVisibleThisFrame;
		}
		while ( compareWith <= pivotTest && i < ub);

		do 
		{
			loc = loc - 1;
            compareWith = ((AWObject*)objs[loc])->m_isVisibleThisFrame;
		}
		while (compareWith > pivotTest);

		if (i < loc) 
		{
			swap(objs, i, loc);
		}//if (i < loc)    
	} // while ( i < loc) 

    swap(objs, lb, loc);
    splitPoint = loc;
}//void AWWorker::reArrange( int  nums[], int  lb, int  ub, int& splitPoint)


void 
AWWorker::reArrangeT( AWOrdered& objs, int  lb, int  ub, int& splitPoint) 
{
    int i;
    int loc;

    AWObject* pivot       = (AWObject*)objs[lb];
    void*     compareWith = NULL;
    void*     pivotTest   = (void*)pivot->getTexture(0);
    i = lb;
    loc = ub + 1;

    while ( i < loc) 
	{
		do 
		{
			i = i + 1;         
            compareWith = ((AWObject*)objs[i])->getTexture(0);
	    }
		while ( compareWith <= pivotTest && i < ub);

		do 
		{
			loc = loc - 1;
            compareWith = ((AWObject*)objs[loc])->getTexture(0);
		}
		while (compareWith > pivotTest);

		if (i < loc) 
		{
			swap(objs, i, loc);
		}//if (i < loc)    
	} // while ( i < loc) 

    swap(objs, lb, loc);
    splitPoint = loc;
}//void AWWorker::reArrange( int  nums[], int  lb, int  ub, int& splitPoint)



void
AWWorker::sortTransparentObjects()
{	//sorts the AWOrdered m_transparentObjects using QSORT
	//using field m_isVisibleThisFrame
	//usage is as follows:
	//NOTE that list is sorted in INCREASING order on m_isVisibleThisFrame
    m_sortField = 0;
	qSort(m_transparentObjects, 0, m_transparentObjects.getNum() - 1);
}//void AWWorker::sortTransparentObjects()



void
AWWorker::sortObjectsByTexture()
{	
    m_sortField = 1;
	qSort(m_alphaTestObjects, 0, m_alphaTestObjects.getNum() - 1);
    qSort(m_objectList,       0, m_objectList.getNum() - 1);
}//void AWWorker::sortObjectsByTexture()



void
AWWorker::initBillboardedNodes(AWCamera* cam)
{	//now go through and set the parent of any BILLBOARD nodes to be the
	//current camera also
	//NOTE - based on assumption that they have no parent initially
	//so they are the direct children of theScene.m_nodeList.m_root
	AWNode*		nextNode		= NULL;
	for (int child =0; child < m_root.getNumChildren(); child++)
	{
		nextNode = m_root.getChild(child);
		if (nextNode)
		{	//Process BILLBOARDED nodes
			if (nextNode && nextNode->getController() && 
				(nextNode->getController()->isA() == CLSID_AWBILLBOARDCONTROLLER))
			{	//set the node's parent to be the camera
				m_root.removeChild(nextNode); 
				child--;
				cam->addChild(nextNode);
			}
		}//if (nextNode)
	}//for (int child =0; child < root.getNumChildren(); child++)
}//void AWWorker::initBillboardedNodes(AWCamera* cam)



void
AWWorker::initProjectiles()
{  //for each projectile, 
   //go through object and transparent object lists
   //see if there's an explosion for the projectile
   //
   //if there is, remove it from the flat list and store
   //via pointer on the projectile object itself
   AWCString explosionName;
   for (int i=0; i< (int)getNumProjectiles(); i++)
   {
      AWRocket* nextProj = getProjectile(i);
      //the projectile needs to be removed from the node list where
      //the nodeEnum has left it as well as here in the projectiles list
      removeNode(nextProj);
      //from the projectile NAME we can determine explosion name, if present -
      explosionName = nextProj->getName();
      explosionName += ":explode";
      AWObject* foundIt = getObjectByName(explosionName);
      if (foundIt)
      {	//remove from its current position in the scene
         //which also removes it from any flat lists it belongs to
         removeNode(foundIt);
         //assign it as the explosion helper for the projectile
         nextProj->setExplosion(foundIt);
      }//if (foundIt)
   }//for (int i=0; i< (int)theScene.m_nodeList.getNumProjectiles(); i++)
}//void AWWorker::initProjectiles()



void
AWWorker::useGun(const AWCString& name)
{
	AWRocket* cur = NULL;
	//search by name in the projectiles list
	for (int obj = 0; obj < (int)m_projectiles.getNum(); obj++)
	{
		cur = (AWRocket*)m_projectiles[obj];
		if (name == cur->getName())
		{   //we have found the correct projectile object
			//so copy the pointer value to m_curProjectile, the current projectile
			m_curProjectile = cur;
			//and terminate the search
			break;
		}//if (name == cur->getName())
	}//for (int obj = 0; obj < m_scene.m_objectList.getNum(); obj++)
}//void AWWorker::useGun(const AWCString& name)




void
AWWorker::fireGun(float time)
{
	if (m_curProjectile) 
	{
		//create a shallow copy of the current projectile
		//shares vertex and face data
		AWRocket* rocket = (AWRocket*)m_curProjectile->clone();
		//insert into scene for display - insert at root
		addNode(rocket);
		rocket->fire(getTransformer(), time);
	}//if (m_curProjectile) 
}//void AWWorker::fireGun(float time)



///////////////////////////
// old scene format helpers
//
void
AWWorker::ensureOwnsChildren(AWNode* next)
{
	if (next)
	{
		AWNode* nextKid=NULL;
		next->setOwnsChildren(TRUE);
		for (int i=0; i<next->getNumChildren(); i++)
		{
			nextKid = (AWNode*)next->getChild(i);
			if (nextKid) ensureOwnsChildren(nextKid);
		}
	}//if (next)
}//void AWWorker::ensureOwnsChildren()



void
AWWorker::getRootNodes(AWOrdered& colln)
{	//for this collection, examine each contained AWNode derivative

	//if it is a root level node, add to m_root.
	//ensure that all AWNodes are set to OWN THEIR CHILDREN
	AWNode* nextNode = NULL;
	for (int i=0; i<(int)colln.getNum(); i++)
	{
		nextNode = (AWNode*)colln[i];
		if (nextNode && !nextNode->getParent())
		{
			m_root.addChild(nextNode);
			ensureOwnsChildren(nextNode);
		}//if (nextNode && !nextNode->getParent())
	}//for (int i=0; i<colln.getNum(); i++)
}//void AWWorker::getRootNodes(AWOrdered& colln)


void
AWWorker::constructRootNode()
{	//helper fn. to construct the new scene graph in case of older
	//file formats. To be called after all nodes have been read from 
	//the stream.
    //NOTE - does not therefore have to read the m_alphaTestObjects list
    //as this didn't exist for the older versions
	getRootNodes(m_objectList);
	getRootNodes(m_transparentObjects);
	getRootNodes(m_cameraList);
	getRootNodes(m_lightList);
	getRootNodes(m_flockList);
}//void AWWorker::constructRootNode()



//*****************************************************
// AWPersistable Persistence virtual function overrides
//
//****************************************************/



void 
AWWorker::restoreGuts(AWFile& file)
{
   unsigned int version;
   restoreVersion(file, &version);	// version number
   if (version <= VER_AWWORKER_7)
   {
      AWPersistable::restoreGuts(file);
      if (version > VER_AWWORKER_2)
      {
         file >> m_root;
      }
      m_objectList.read(file, version < VER_AWWORKER_7);
      if (version >= VER_AWWORKER_4)
      {
         m_alphaTestObjects.read(file, version < VER_AWWORKER_7);
      }
      m_transparentObjects.read(file, version < VER_AWWORKER_7);
      m_cameraList.read(file, version < VER_AWWORKER_7);
      m_lightList.read(file, version < VER_AWWORKER_7);;
      m_flockList.read(file, version < VER_AWWORKER_7);;
      //following fns rely on if at top of fn. for versions
      if (version > VER_AWWORKER_1)
      {
         m_projectiles.read(file, version < VER_AWWORKER_7);
      }
      if (version <= VER_AWWORKER_2)
      {
         constructRootNode();
      }
      file >> m_curCamera;
      file >> m_userTransform;
      if (version <= VER_AWWORKER_5)
      {  //not ever used by this stage
         //was set in error. Owned by a camera.
         m_userTransform = NULL;
      }
      m_max.restoreGuts(file);
      m_min.restoreGuts(file); 
      file >> m_curProjectile;
      file >> m_environment;
      m_levelEnd = NULL;
      if (version >= VER_AWWORKER_5)
      {
         file >> m_levelEnd;
      }
      m_timerBarCtl = NULL;
      if (version >= VER_AWWORKER_6)
      {
         file >> m_timerBarCtl;
      }
      //init the viewcone fov and farclip
		setCurCamera(m_curCamera);
	}//if (version <= VER_AWWORKER_7)
}//void AWWorker::restoreGuts(AWFile& file)



void 
AWWorker::saveGuts(AWFile& file) const
{
   saveVersion(file, uVersion_ );	// version number
   AWPersistable::saveGuts(file);
   file << m_root;
   file << m_objectList;
   file << m_alphaTestObjects;
   file << m_transparentObjects;
   file << m_cameraList;
   file << m_lightList;
   file << m_flockList;
   file << m_projectiles;
   file << m_curCamera;
   file << m_userTransform;
   m_max.saveGuts(file);
   m_min.saveGuts(file);
   file << m_curProjectile;
   file << m_environment;
   file << m_levelEnd;
   file << m_timerBarCtl;
}//void AWWorker::saveGuts(AWFile& file) const




AWspace 
AWWorker::binaryStoreSize() const
{
	return sizeof uVersion_ + 
		AWPersistable::binaryStoreSize() +
		m_root.recursiveStoreSize() +
		m_objectList.getStoreSize() +
		m_cameraList.getStoreSize() +
		m_flockList.getStoreSize() +
      m_alphaTestObjects.getStoreSize() +
		m_transparentObjects.getStoreSize() +
		m_projectiles.getStoreSize() + 
		AWPERSISTABLE_POINTER_SIZE(m_curCamera) +
		AWPERSISTABLE_POINTER_SIZE(m_userTransform) +
		AWPERSISTABLE_POINTER_SIZE(m_curProjectile) +
		AWPERSISTABLE_POINTER_SIZE(m_environment) +
      AWPERSISTABLE_POINTER_SIZE(m_levelEnd) +
      AWPERSISTABLE_POINTER_SIZE(m_timerBarCtl) +
		sizeof (m_max) +
		sizeof (m_min) +
		m_lightList.getStoreSize();
}//AWspace AWWorker::binaryStoreSize() const
