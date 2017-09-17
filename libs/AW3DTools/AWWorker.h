#ifndef __AW_WORKER_H
#define __AW_WORKER_H


//#include "AWLock.h"

class AWExport AWObject; //forward definition
class AWExport AWScene; //forward definition
class AWExport AWCamera; //forward definition
class AWExport AWLight; //forward definition
class AWExport AWFlock; //forward
//class AWExport AWNode; //forward
class AWExport AWRocket; //forward

#include "AWPoint.h"
#include "AWMatrix4.h"
#include "AWFlightController.h"
#include "AWSphrCone.h"
#include "AWNode.h"
#include "AWTimerBarCtl.h"

class AWExport GLRenderer; //forward

class AWExport AWWorker : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWWorker);

	AWWorker();
	~AWWorker()			{clearAndDestroy();}

	void				clearAndDestroy();
	virtual void		compact();

   void           setUseAlphaTest(AWBoolean useAlpha = TRUE);

	void				startup(float t, GLRenderer& renderer);
	void				init(float time, GLRenderer& renderer);
   void           initProjectiles();
   void           initBillboardedNodes(AWCamera* cam);

	void				setViewRot(float viewPitch, float viewYaw)
						{if (getViewTransformer()) 
						getViewTransformer()->setViewRot(viewPitch, viewYaw);}

	void				waitForObject(int i);
	void				releaseObject(int i);

	void				transformAllNodes(float frame);
	void				transformNodes(AWOrdered& colln, float frame);
   void           calcVisibility(float t);
   void           calcVisibility(float t, AWSolidCone& viewCone);

	void				addProjectile(AWRocket* rocket);
	AWRocket*			removeProjectile(AWRocket* rocket);
    int                 getNumProjectiles()     {return (int)m_projectiles.getNum();}
    AWRocket*           getProjectile(int i)    {return (AWRocket*)m_projectiles[i];}

	void				addNode(AWNode* node, AWNode* parent=NULL);
	void				addNodeToFlatList(AWNode* node);
	void				addNodeAndChildrenToFlatList(AWNode* node);

	AWNode*				removeNode(AWNode* obj);
	AWNode*				removeNodeFromFlatList(AWNode* obj);
	AWNode*				removeNodeAndChildrenFromFlatList(AWNode* obj);

    AWNode&             getRoot()   {return m_root;}

	void				processEvents(AWNode& n, float t);

    AWFlock*            clearCurrentFlock();
    //void                switchToNextLevel(float t);
    void                resetLevel(float t, GLRenderer renderer);
    float               playTransition(float t, int i = 0);

    void                resetTimeBar(float timeNow);
    void                initTimeBar(float timeout, float timeNow);
    void                setTimerBarCtl(AWTimerBarController* c){m_timerBarCtl=c;}
    AWTimerBarController* getTimerBarCtl()     {return m_timerBarCtl;}

    void                setLevelEnd(AWNode* n) {m_levelEnd=n;}
    AWNode*             getLevelEnd()          {return m_levelEnd;}
    void                setTargetFlock(int flk);


//AWPersistable virtuals
	virtual void		restoreGuts(AWFile& file);
    virtual void		saveGuts(AWFile& file) const;

	virtual AWspace		binaryStoreSize() const;
	//AWBoolean		isEqual(const AWPersistable* o) const{return m_name == ((AWCamera*)o)->m_name;}
	//int				compareTo(const AWPersistable* o) const{return isEqual(o) ? 0 : -1;}
//GENERAL NODE MANIPULATION
	//void				scaleNodes(AWOrdered& colln, const AWPoint& scale);
	AWPoint				fitToRange(const AWPoint& range);
	//void				center(AWOrdered& colln, const AWPoint& origin);
	void				center(const AWPoint& origin);
	void				updateMaxValues(const AWPoint& val);
	const AWPoint&		getMax() {return m_max;}
	const AWPoint&		getMin() {return m_min;}

//USER ACTIONS
	void				useGun(const AWCString& name);
	void				fireGun(float time);
	void				updateTransformer(const AWPoint& euler, float speed, float t);
				
//COLLISION DETECTION
	AWBoolean			collideObjectWithWorld(AWObject* srce, float time);
	void				collideAllObjects(float time);

public:
//TOP LEVEL ACCESS FUNCTIONS - look at all 3 object helper collections
   int               getNumObjects();  
   AWObject*			getObject(int i);
//ACCESS functions for helper collections
	int					getNumOpaqueObjects()		{return (int)m_objectList.getNum();}
   int               getNumAlphaTestObjects()	{return (int)m_alphaTestObjects.getNum();}
	int					getNumTransparentObjects()	{return (int)m_transparentObjects.getNum();}
	int					getNumCameras()		      {return (int)m_cameraList.getNum();}
	int					getNumLights()		         {return (int)m_lightList.getNum();}
	int					getNumFlocks()		         {return (int)m_flockList.getNum();}
   int               getFlockIndex()            {return m_flockIndex;}
   void              setFlockIndex(int v)       {m_flockIndex=v;}

	AWObject*			getOpaqueObject(int i);
	AWObject*			getTransparentObject(int i);
   AWObject*         getAlphaTestObject(int i);
	AWObject*			getObjectByName(const AWCString& name);
	AWFlock*			   getFlock(int i);
	AWCamera*			getCamera(int i);
	int					findCamera(const AWCamera* cam)	{return m_cameraList.find((AWPersistable*)cam);}
	AWLight*			   getLight(int i);
   AWLight*			   getLightByName(const AWCString& name);

	void				setCurCamera(AWCamera* obj);
	AWCamera*			getCurCamera() {return m_curCamera;}
	AWFlightController* getTransformer(){return m_userTransform;}
	void				setTransformer(AWFlightController* f);
	AWFlightController* getViewTransformer();
	void				setEnvironment(AWObject* env);
	AWObject*			getEnvironment() {return m_environment;}


protected:
	void				compactNodes(AWOrdered& list);
	void				initFlock(AWFlock* flock, GLRenderer renderer);
	//QSORT helpers
	void				sortTransparentObjects();
    void                sortObjectsByTexture();
	void				reArrange( AWOrdered& objs, int  lb, int  ub, int& splitPoint);
    void                reArrangeT( AWOrdered& objs, int  lb, int  ub, int& splitPoint);
	void				qSort( AWOrdered& objs, int lb, int ub);
	void				swap(AWOrdered& objs, int first, int second);
    int                 m_sortField; //0=> m_isVisibleThisFrame
                                     //1=> m_textureList(0)

	//current camera - provides PROJECTION MATRIX INFO
	AWCamera*			m_curCamera;
	//sink for JOYSTICK UPDATES
	AWFlightController* m_userTransform;

	//helper object for current demo - the one and only projectile type
	//cloned for each bullet (i.e. display list reused)
	AWRocket*			m_curProjectile;
	AWObject*			m_environment;
    AWTimerBarController* m_timerBarCtl;

	AWSolidCone			m_viewCone;			//approximates Veiw Frustrum for culling

	AWNode				m_root;			 //root node for Active Nodes collection
//collections grouping the nodes by type
	AWOrdered			m_objectList;
	AWOrdered			m_cameraList;
	AWOrdered			m_lightList;
	AWOrdered			m_flockList;
   AWOrdered         m_alphaTestObjects;
	AWOrdered			m_transparentObjects;
	AWOrdered			m_projectiles;
	//AWMatrix4			m_viewTM;

	AWPoint				m_min;
	AWPoint				m_max;

	AWBoolean			m_firstTime;

    int                 m_flockIndex;
    AWBoolean           m_levelEndAdded;
    AWNode*             m_levelEnd;
 
private:
	//HELPERS for dealing with file format versions 1 & 2
	void				constructRootNode();
	void				getRootNodes(AWOrdered& colln);
	void				ensureOwnsChildren(AWNode* next);
};//class AWWorker


inline int
AWWorker::getNumObjects()
{
   return getNumOpaqueObjects() + getNumAlphaTestObjects() + getNumTransparentObjects();
}//inline int AWWorker::getNumObjects()


inline AWObject*			
AWWorker::getObject(int i)
{
   AWObject* retVal = NULL;
   int numOpaque      = getNumOpaqueObjects();
   int numTransparent = getNumTransparentObjects();
   int numAlphaT      = getNumAlphaTestObjects();
   if (i < (numOpaque + numTransparent + numAlphaT))
	{
      if (i < numOpaque)
      {
         retVal = getOpaqueObject(i);
      }
      else if (i < numOpaque + numAlphaT)
      {
         retVal = getAlphaTestObject(i-numOpaque);	
      }
      else
      {
         retVal = getTransparentObject(i-numOpaque-numAlphaT);
      }
	}//if (i < (numOpaque + numTransparent + numAlphaT))
   return retVal;
}//inline AWObject* AWWorker::getObject(int i)


inline AWLight*															 
AWWorker::getLight(int i)
{
	return (AWLight*)m_lightList[i];
}

inline AWCamera*															 
AWWorker::getCamera(int i)
{
	return (AWCamera*)m_cameraList[i];
}

inline AWObject*			
AWWorker::getOpaqueObject(int i)
{
	return (AWObject*)m_objectList[i];
}//AWObject* AWWorker::getOpaqueObject(int i)


inline AWObject*			
AWWorker::getTransparentObject(int i)
{
	return (AWObject*)m_transparentObjects[i];
}//AWObject* AWWorker::getTransparentObject(int i)

inline AWObject*			
AWWorker::getAlphaTestObject(int i)
{
	return (AWObject*)m_alphaTestObjects[i];
}//AWObject* AWWorker::getAlphaTestObject(int i)


inline AWFlock*			
AWWorker::getFlock(int i)
{
	return (AWFlock*)m_flockList[i];
}//AWFlock* AWWorker::getFlock(int i)


#endif //sentinel __AW_WORKER_H