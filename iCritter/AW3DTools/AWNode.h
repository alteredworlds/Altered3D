#ifndef __AWNODE_H
#define __AWNODE_H


#include "AWToolsCommon.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWSphrCone.h"
#include "AWOrdered.h"
#include "AWTransformController.h"
#include "AWCString.h"
#ifdef _WIN32
#   include <windows.h>
#endif

#define AW_NODE_OWNS_CHILDREN		    0x00000001
#define NOT_AW_NODE_OWNS_CHILDREN		0xfffffffe
#define AW_NODE_OWNS_TRANSFORMER	    0x00000002
#define NOT_AW_NODE_OWNS_TRANSFORMER	0xfffffffd
#define AW_INFINITE_LIFE                -1

class AWExport AWGLRenderer;

class AWExport AWNode	: public AWPersistable
{	
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWNode);
	
	AWNode(const AWCString& name = "", AWNode* parent=NULL, 
		   double life=AW_INFINITE_LIFE);
	AWNode(const AWNode& other);  
	virtual					~AWNode();
	
	AWNode&					operator=(const AWNode& other);
	int						operator==(const AWNode& other) {return this == &other;}
	AWNode&					clone(const AWNode& other);
	virtual AWNode*			clone() const; 
	virtual void			compact();
	virtual void			clearAndDestroy();
	virtual void			init(double time, AWGLRenderer& renderer);

public:
//General Properties
	virtual AWBoolean		isAlive(double time);
	AWBoolean				isAnimated();
	void					setAnimStartTime(double time, AWBoolean andKids=TRUE);
	double					getAnimStartTime()		{return m_animStart;}
	
	const AWCString&		getName()							{return m_name;}
	void					setName(const AWCString& name)	{m_name = name;}
	//indicate ownership of children and transformer
	void					setOwnsChildren(AWBoolean val) 
	{
        if (val)
            m_ownership |= AW_NODE_OWNS_CHILDREN;
		else
            m_ownership &= NOT_AW_NODE_OWNS_CHILDREN;
    }
    
	void					setOwnsTransformer(AWBoolean val)
			{if (val) m_ownership |= AW_NODE_OWNS_TRANSFORMER; else m_ownership &= NOT_AW_NODE_OWNS_TRANSFORMER;}
	AWBoolean				getOwnsChildren() {return m_ownership & AW_NODE_OWNS_CHILDREN;}
	AWBoolean				getOwnsTransformer() {return m_ownership & AW_NODE_OWNS_TRANSFORMER;}
	void					setLastUpdate(double time, AWBoolean andKids=TRUE);
	void					setLife(double life)   {m_life=life;}
	double					getLife()             {return m_life;}

//Data manipulation (virtuals)
	virtual void			scale(const AWPoint& scale);
	virtual void			offset(const AWPoint& offset);

//methods returning the Node transformation
//POSITION ROTATION SCALE
	virtual int				getPosn(AWPoint& pos);
	virtual int				getDirection(AWPoint& dir);
	virtual int				getTransform(AWMatrix4& mat);
	virtual int				getViewTM(AWMatrix4& viewTM);
	virtual const AWMatrix4& getWSTM()  {return m_wsTM;}
	//get the view cone from the wsTM
	virtual void			getViewCone(AWSolidCone& viewCone);
	//this next function is the real one, used by the helper above
	virtual void			getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM);
	
	
	virtual	int				getTransform(float& roll, float& pitch,	
								 float& yaw, AWPoint& posn)		{return 0;}
	//return rotational component in Quaternion form
	virtual	int				getTransform(AWPoint& posn, AWPoint& scale, AWQuat& rotn);
	//return rotational component in Angle/Axis form INCLUDING scale
	virtual	int				getTransform(AWPoint& posn, AWPoint& scale, AWPoint& axis, 
								 float& angle);
	//return rotational component in Angle/Axis form
	virtual	int				getTransform(AWPoint& posn, AWPoint& axis,float& angle);

	//set the steady state transform
	virtual void			setTransform(const AWPoint& posn, const AWPoint& scale, 
					          const AWQuat& rotn);
	//update the object state with the values for the specified time
	virtual int				calcTransform(double t, const AWMatrix4& parentTM);

	//update the object state with the view-dependent values at t
	virtual int				calcViewDependentData(double t, const AWSolidCone& viewCone);

   //update the view-dependent visibility value at t
	virtual int				calcVisibility(double t, const AWSolidCone& viewCone);

	//this factor used on all rotational components of this node. 
	//fingers crossed as to whether this will produce the desired effect...
	virtual void			rotate(const AWQuat& rot);
	void					rotateChildren(const AWQuat& rot);

//Access to the AWTransformController
	AWTransformController*	getController()			{return m_transformer;}
	void					setController(AWTransformController* val);
//manipulation of CHILDREN
	int						getNumChildren() const			{return (int)m_children.getNum();}
	AWNode*					getChild(int index)				{return (AWNode*)m_children[index];}
	int						addChild(AWNode* newChild);
	AWNode*					removeChildAt(int index);
	AWNode*					removeChild(AWNode* child);
	void					setParent(AWNode* parent)		{m_parent=parent;}
	AWNode*					getParent()						{return m_parent;}
	AWNode*					getRoot();
    
    void                    getWSTM(AWMatrix4& wsTM);
	float*					getMatrix();

protected:
	//state data
	AWCString				m_name;
	AWTransformController*	m_transformer;

//MEMBER DATA associated with CHILDREN
	//parent AWNode, if any... (should be NULL if none)
	AWNode*					m_parent;
	//list of child nodes
	AWOrdered				m_children;
	DWORD					m_ownership;
    //added 7/1/99
    double					m_life;
    //start time for animated sequences
    double					m_animStart;    //not persisted
    //used to cache results of 'calcTransform'
    AWMatrix4				m_wsTM;


public:
    //virtual void			reAssignTexture(AWPersistable* src, AWPersistable* dest);
	//int					compareTo(const AWPersistable*) const{return -1;}
	virtual void			restoreGuts(AWFile& file);
    virtual void			saveGuts(AWFile& file) const;

	virtual AWspace			binaryStoreSize() const;
};//class AWNode	: public AWPersistable



inline AWBoolean 
AWNode::isAlive(double time)
{
    return (AW_INFINITE_LIFE == m_life) || (time <= m_life); 
}//inline AWBoolean AWNode::isAlive(float time)



inline void
AWNode::compact()
{
	m_children.pack();
	for (int i=0; i<(int)m_children.getNum(); i++)
	{
		((AWNode*)m_children[i])->compact();
	}
	if (m_transformer) m_transformer->compact();
}//virtual void AWNode::compact()



inline void
AWNode::setController(AWTransformController* val)
{
	if (m_transformer && getOwnsTransformer()) 
		delete m_transformer;
	m_transformer = val;
}//inline void AWNode::setController(AWTransformController* val)



inline AWNode* 
AWNode::getRoot()
{
	AWNode* retVal = this;
	AWNode* next = getParent();
	while (next)
	{
		retVal = next;
		next   = next->getParent();
	}
	return retVal;
}//AWNode* AWNode::getRoot()



inline int			
AWNode::addChild(AWNode* newChild)
{	//returns -1 if node not inserted
	int retVal = -1;
	if (newChild)
	{
		newChild->setParent(this);
		m_children.add(newChild);
		//insertion index will now be at end of collection
		retVal = m_children.getNum()-1;
	}//if (newChild)
	return retVal;
}//int AWNode::addChild(AWNode* newChild)


inline AWNode*			
AWNode::removeChild(AWNode* child)
{
	AWNode* retVal =NULL;
	if (child)
	{
      int index = m_children.find(child);
      if (AW_NPOS != index)
      {
		   retVal = (AWNode*)m_children[index];
         m_children.delIndex(index);
      }//if (AW_NPOS != index)
	}//if (child)
	return retVal;
}//AWNode* AWNode::removeChild(AWNode* child)


inline AWNode*			
AWNode::removeChildAt(int index)
{
   AWNode* retVal = (AWNode*)m_children[index];
   m_children.delIndex(index);
	return retVal;
}//AWNode* AWNode::removeChildAt(int index)



inline AWBoolean		 			
AWNode::isAnimated()
{
	return (m_transformer) ? m_transformer->isAnimated() : FALSE;
}//AWBoolean AWNode::isAnimated()



inline void    
AWNode::init(double t, AWGLRenderer& renderer)
{
    //doesn't do much at the AWNode level, but 
    //passes on to all kids
    for (int i =0; i<getNumChildren(); i++)
    {
        getChild(i)->init(t, renderer);
    }//for (int i =0; i<getNumChildren(); i++)
}//inline void  AWNode::init(float t, GLRenderer& renderer)



inline int	 
AWNode::getPosn(AWPoint& pos)
{
	if (m_transformer)
	{
		m_transformer->getPosn(pos);
		return 1;
	}
	else
	{
		return 0;
	}
}//inline int AWNode::getPosn(AWPoint& pos)



//OBJECT STATE ACCESS
//return rotational component in quaternion form
inline int	 
AWNode::getTransform(AWPoint& posn, AWPoint& scale, AWQuat& rotn)
{
	return (m_transformer) ? m_transformer->getTransform(posn, scale, rotn) : 0;
}//int AWNode::getTransform(AWPoint& posn, AWPoint& scale, AWQuat& rotn)


//get viewing transform for this node
inline int		
AWNode::getViewTM(AWMatrix4& viewTM)
{
	int retVal = 0;
	if (m_transformer)
	{
		retVal = m_transformer->getViewTM(viewTM);
	}
	return retVal;
}//int AWNode::getViewTM(AWMatrix4& viewTM)


inline void                    
AWNode::getWSTM(AWMatrix4& wsTM)
{
    if (m_transformer) 
        m_transformer->getWSTM(wsTM);
    else
        wsTM = m_wsTM;
}

inline float*					
AWNode::getMatrix() 
{
    return (m_transformer) ? 
        m_transformer->getMatrix() : 
        m_wsTM.m_mat;
}


//return rotational component in angle, axis form
inline int			 	 
AWNode::getTransform(AWPoint& posn, AWPoint& axis, float& angle)
{
	int retVal = 0;
	if (m_transformer)
	{
		AWQuat  rotn;
		AWPoint scale;
		if ((retVal = m_transformer->getTransform(posn, scale, rotn)))
		{
			rotn.toAxisAngle(axis, angle);
		}
	}//if (m_transformer)
	return retVal;
}//int AWNode::getTransform(AWPoint& posn, AWPoint& axis, float& angle)


//OBJECT STATE ACCESS
//return rotational component in angle, axis form
inline int	 	 
AWNode::getTransform(AWPoint& posn, AWPoint& scale, AWPoint& axis, float& angle)
{
	int retVal = 0;
	if (m_transformer)
	{
		AWQuat rotn;
		if ((retVal = m_transformer->getTransform(posn, scale, rotn)))
		{
			rotn.toAxisAngle(axis, angle);
		}
	}//if (m_transformer)
	return retVal;
}//int AWNode::getTransform(AWPoint& posn, AWPoint& scale, AWPoint& axis, float& angle)


//matrix form
inline int  	 
AWNode::getTransform(AWMatrix4& mat)
{
	int retVal = 0;
	if (m_transformer)
	{
		retVal = 1;
		m_transformer->getWSTM(mat);
	}//if (m_transformer)
	return retVal;
}//int AWNode::getTransform(AWMatrix4& mat)



inline int	 	 
AWNode::calcViewDependentData(double t, const AWSolidCone& viewCone)
{
	//iterate through children and calcViewDependentData
	for (int i = 0; i < m_children.getNum(); i++)
	{
		((AWNode*)m_children[i])->calcViewDependentData(t, viewCone);
	}//for (int i = 0; i < m_children.getNum(); i++)
	return 1;
}//int AWNode::calcViewDependentData(double t, const AWSolidCone& viewCone)



inline int		   
AWNode::calcVisibility(double t, const AWSolidCone& viewCone)
{  //iterate through children and calcViewDependentData
	for (int i = 0; i < m_children.getNum(); i++)
	{
		((AWNode*)m_children[i])->calcVisibility(t, viewCone);
	}//for (int i = 0; i < m_children.getNum(); i++)
	return 1;
}//int AWNode::calcVisibility(float t, const AWSolidCone& viewCone)



inline void				
AWNode::setAnimStartTime(double t, AWBoolean andKids)
{
	m_animStart = t;
	if (andKids)
	{
		for (int i=0; i< (int)m_children.getNum(); i++)
		{
			getChild(i)->setAnimStartTime(t, andKids);
		}//for (int i=0; i< m_children.getNum(); i++)
	}//if (andKids)
}//void	AWNode::setAnimStartTime(float t, AWBoolean andKids)



inline void				
AWNode::setLastUpdate(double t, AWBoolean andKids)
{
	if (m_transformer)
    {
        m_transformer->setLastUpdate(t);
    }
	if (andKids)
	{
		for (int i=0; i< (int)m_children.getNum(); i++)
		{
			getChild(i)->setLastUpdate(t, andKids);
		}//for (int i=0; i< m_children.getNum(); i++)
	}//if (andKids)
}//void	AWNode::setAnimStartTime(float t, AWBoolean andKids)


//get the approximate spherical cone for this node as a camera
inline void
AWNode::getViewCone(AWSolidCone& viewCone)
{
	AWMatrix4 wsTM;
   getViewCone(viewCone, wsTM);
}//inline void AWNode::getViewCone(AWSolidCone& viewCone)


//alternate version which also returns the wsTM matrix for later use
inline void
AWNode::getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM)
{
	if (getController()) getController()->getWSTM(wsTM);
	wsTM.getDirection(viewCone.m_A);
   //in OpenGL, LIGHTS and CAMERAS by default look down -Z axis
   //hence we need to invert the transformed Z axis direction 
   //retreived with getDirection
	viewCone.m_A = -viewCone.m_A.normalize();
	wsTM.getTranslation(viewCone.m_V);
}//inline void AWNode::getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM)


#endif //sentinel __AWNODE_H