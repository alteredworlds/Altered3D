
#include "AWObject.h"
#include "AWRocket.h"
#include "AWBoidController.h"
#include "AWGLRenderer.h"
#include "AWBillboardController.H"
#include "AWAnimatedTexture.h"
#include "AWMaterial.h"



//need to have object providing faces / verts / tverts / texture
//derived from AWObject and having a AWRocketController

#define VER_AWROCKET_1			 100
#define VER_AWROCKET_2			 101
#define VER_AWROCKET_3			 102
#define VER_AWROCKETCONTROLLER_1 100

AWDEFINE_VERSIONABLE_PERSISTABLE(AWRocket, CLSID_AWROCKET, VER_AWROCKET_3);
AWDEFINE_VERSIONABLE_PERSISTABLE(AWRocketController, CLSID_AWROCKETCONTROLLER, VER_AWROCKETCONTROLLER_1);


AWRocket::AWRocket(const AWCString& name, AWNode* parent,
				   float speed, float life)
		: AWObject(name, parent, life), 
          m_speed(speed),
          m_explosion(NULL),
          m_renderer(NULL) 
{
}


AWRocket::~AWRocket()
{
    if (m_ownsVertexData)
    {   //assume that if this rocket owns it's vertex data, then
        //it must be the clone source for this projectile class
        //and therefore also owns the explosion effect object
        //
        //so delete it
        delete m_explosion;
    }//if (m_ownsVertexData)
    m_explosion = NULL;
    m_renderer  = NULL;
}//AWRocket::~AWRocket()



AWRocket&
AWRocket::clone(const AWRocket& other)
{
	//copy contents of other into this and return self
	//
	//copy base class
	AWObject::clone((const AWObject&)other);
	//and now own data
	m_speed = other.m_speed;
    //copy over the pointer value for the explosion object
    m_explosion = other.m_explosion;
    m_renderer  = other.m_renderer;
	return *this;
}//AWRocket& AWRocket::clone(const AWRocket& other)



AWNode* 
AWRocket::clone() const
{
	AWRocket* retVal = new AWRocket;
	retVal->clone(*this);
	return retVal;
}//AWNode* AWRocket::clone() const


/*
void                    
AWRocket::reAssignTexture(AWPersistable* src, AWPersistable* dest)
{
    AWObject::reAssignTexture(src, dest);//calls for children
    if (m_explosion) m_explosion->reAssignTexture(src, dest);
}//void AWRocket::reAssignTexture(AWPersistable* src, AWPersistable* dest)

*/

void	
AWRocket::fire(AWTransformController* parentTC, float time)
{
   AWMatrix4 projectileStart;
   if (parentTC)
   {
      parentTC->getWSTM(projectileStart);
   }
   //init(0.0f); //shouldn't be required provided all projectiles initialized ONCE
   m_life+=time;
   setController(new AWRocketController(projectileStart, m_speed, 0.0f));
}



AWBoolean	
AWRocket::collideWith(AWObject* target, float time)
{
	AWBoolean retVal = AWObject::collideWith(target, time);
	if (retVal)
	{	
		AWTRACE("AWRocket::collideWith - collision detected\n");
		//perform AWRocket specific collision code here
		//first, this projectile must be condemned to death
		setLife(0.0f);
		//no longer take part in any more collision detection
		setCollide(FALSE);
		//MessageBeep(MB_ICONEXCLAMATION);
		//
		//now pass the explode command through to the target
		//objects that can't be destroyed by rockets won't respond
		//
		
		target->setAnimation(1);     
        target->setLife(time + 2.0f);
        target->setAnimStartTime(time);
		//no longer take part in any more collision detection
		target->setCollide(FALSE);
        AWMatrix4 fireTM;    
        target->getTransform(fireTM);
        //now remove from parent (flock for SpaceGnats)
        AWNode* hitParent = target->getParent();
        if (hitParent && (CLSID_AWFLOCK == hitParent->isA()))
        {   
            //(i) detatch the object from its parent AWFlock as would still take part in the flocking calcs
            //better solution is perhaps to exclude from these calcs?
            //(ii) supply object with a new Controller, which will cause it to
            //continue with same linear momentum
			AWTRACE("  collided with flock member - removing from flock\n");
            hitParent->removeChild(target);
            //now add as a direct child of the root
            if (m_renderer)
            {
               m_renderer->getScene().m_nodeList.getRoot().addChild(target);
            }
			//hide children of this node - in this case the space gnat's engine
			target->setIsVisible(FALSE, TRUE);
			//now unhide the target itself once more
			target->setIsVisible(TRUE, FALSE);
			if (getOwnsTransformer())
			{
				float speed =0.0f;
				//also need the speed. note that this gross code will be 
				//replaced shortly, when speed moved up in class heirachy
				AWTransformController* ctrl = target->getController();
				if (ctrl && (CLSID_AWBOIDCONTROLLER == ctrl->isA()))
				{
					speed = ((AWBoidController*)ctrl)->m_speed;
				}
				ctrl = getController();
				if (ctrl && (CLSID_AWROCKETCONTROLLER == ctrl->isA()))
				{
					AWRocketController* newCtrl = (AWRocketController*)ctrl->copy();
					newCtrl->setFireTM(fireTM);
					newCtrl->setSpeed(speed);
					newCtrl->setLastUpdate(0.0f);
					target->setController(newCtrl);
				}	
			}
			//EXPLOSION, if any
			//should be shown stationary at the posn. of the collision
			showExplosion(fireTM, time);
        }
	}//if (retVal)
	return retVal;
}//AWBoolean AWRocket::collideWith(AWObject* target, float time)



void
AWRocket::showExplosion(const AWMatrix4& fireTM, float time)
{
   AWTRACE1("AWRocket::showExplosion at %f\n", (double)time);
   if (!m_renderer) return;
   if (m_explosion && m_renderer->getNumPasses(m_explosion))
   {  //first, clone the stock explosion object      
      AWObject* temp = (AWObject*)m_explosion->clone();
      AWTRACE1("Explosion object named: %s\n", temp->getName().c_str());
      //need to find life of explosion object, which should be
      //now + the duration of the texture animation on the explosion
      //
		//purely arbitrary value which should NOT be used
		//life should come from user defined properties as with rocket
		float newLife = 2.0f;
      AWMaterial* mat = temp->getMaterial();
      if (mat && mat->getNumTexEffects())
		{
			AWTextureEffect* explodeTex = mat->getTextureEffect(0);
			if (explodeTex && explodeTex->m_tex && 
              (CLSID_AWGLANIMTEXTURE == explodeTex->m_tex->isA()))
			{
            newLife = ((AWAnimatedTexture*)explodeTex->m_tex)->getAnimationDuration();
			}
		}//if (temp->getNumTextures())
		//set the properties for the new explosion object
      temp->setLife(time + newLife);
	  AWTRACE1("  explosion should end at: %f\n", (double)(time+newLife));
      temp->setAnimStartTime(time, FALSE);
      //assign explosion its own [Billboard] transform controller
      AWBillboardController* billCtrl = new AWBillboardController;
      //set to same ws posn as incoming transform fireTM
      AWPoint pos;
      fireTM.getTranslation(pos);
      billCtrl->setPosn(pos);
      billCtrl->m_pointSymmetry = TRUE;
      temp->setController(billCtrl);

      AWObject* shockRing = NULL;
      //for this first pass, if we have a child object
      //this is expected to be the shock ring, and should be detached
      //from the explosion itself, which will be billboarded.
      if (temp->getNumChildren() && (CLSID_AWOBJECT == temp->getChild(0)->isA()))
      {
         shockRing = (AWObject*)temp->removeChildAt(0);
         shockRing->setLife(time + newLife);
         
         AWTransformController* ctrl = new AWTransformController;
         ctrl->setPosn(pos);
         //ctrl->setRotn(AWQuat(fireTM)*AWQuat(1.0f, -1.0f, 0.0f, 0.0f));
         ctrl->setRotn(AWQuat(fireTM));
         shockRing->setController(ctrl);
         shockRing->setTwoSided(TRUE);
         shockRing->setAnimStartTime(time);
         //add new object [and children] to scene list for display
         m_renderer->getScene().m_nodeList.addNode(shockRing);
      }      
      //add new object [and children] to scene list for display
      //-as we're billboarding this node, set its parent to be
      //-the current camera
      m_renderer->getScene().m_nodeList.addNode(temp,
            m_renderer->getScene().m_nodeList.getCurCamera());
      //now do some sounds...
      m_renderer->setTimeToExplode();
   }//if (m_explosion)
}//void AWRocket::showExplosion(const AWMatrix4& fireTM, float time)

void		
AWRocket::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if ((VER_AWROCKET_1 == version) || (VER_AWROCKET_2 == version) ||
        (VER_AWROCKET_3 == version))
	{
		AWObject::restoreGuts(file);
		file.Read(m_speed);
        if (VER_AWROCKET_1 == version)
        { //old version stored the life on the rocket not the node
            //so read it in and then set life on the node
            float readLife;
		    file.Read(readLife);
            setLife(readLife);
        }//if (VER_AWROCKET_1 == version)
        if (VER_AWROCKET_3 == version)
        {
            file >> m_explosion;
        }
	}//if ((VER_AWROCKET_1 == version) || (VER_AWROCKET_2 == version))
}//void	AWRocket::restoreGuts(AWFile& file)



void        
AWRocket::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWObject::saveGuts(file);
	file.Write(m_speed);
    file << m_explosion;
}//void AWRocket::saveGuts(AWFile& file) const



AWspace     
AWRocket::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ + 
					 AWObject::binaryStoreSize() +
					 sizeof(m_speed) +
                     AWPERSISTABLE_POINTER_SIZE(m_explosion);
	return retVal;
}//AWspace AWRocket::binaryStoreSize() const




//this should provide the AI for a bullet
AWRocketController::AWRocketController(const AWMatrix4& parentTM, float speed, 
									   float time)
		: m_speed(speed),
		  m_fireTM(parentTM)
{
    setLastUpdate(time);	
}


AWRocketController::~AWRocketController()
{
}


int			
AWRocketController::hasData()
{
	return 1;
}


int											
AWRocketController::calcTransform(float t, const AWMatrix4& parentTM)
{
    float deltaT = (getLastUpdate() > 0.0f) ? t - getLastUpdate() : 0.0f;

	setLastUpdate(t);
	
	m_wsTM = m_fireTM;
	m_wsTM.awTranslate(getPosn());
    setPosn(getPosn() + deltaT * m_speed * AWPoint(0.0f, 0.0f, -1.0f));
	//m_wsTM.awRotate(m_rotn.getQuat());
	//m_wsTM.awScale(m_scale.getPoint());
    return 1;
}//int AWRocketController::calcTransform(float time)



void		
AWRocketController::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWROCKETCONTROLLER_1 == version)
	{
		AWTransformController::restoreGuts(file);
		file.Read(m_speed);
	}
}//void	AWRocketController::restoreGuts(AWFile& file)



void        
AWRocketController::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTransformController::saveGuts(file);
	file.Write(m_speed);
}//void AWRocketController::saveGuts(AWFile& file) const



AWspace     
AWRocketController::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ + 
					 AWTransformController::binaryStoreSize() +
					 sizeof(m_speed);
	return retVal;
}//AWspace AWRocketController::binaryStoreSize() const
