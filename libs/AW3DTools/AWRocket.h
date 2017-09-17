#ifndef __ROCKET_AW_H
#define __ROCKET_AW_H

#include "AWTransformController.h"

class AWExport GLRenderer; //forward definition


class AWExport AWRocketController : public AWTransformController
{
public:
    friend class AWExport AWRocket;

	AWDECLARE_VERSIONABLE_PERSISTABLE(AWRocketController);

	AWRocketController() : m_speed(0.0f){;}
	AWRocketController(const AWMatrix4& parentTM, float speed, float time);
	virtual				~AWRocketController();

	virtual	int			hasData();

	int					calcTransform(float t, const AWMatrix4& parentTM);

	float				getSpeed()const		  {return m_speed;}
	void				setSpeed(float speed) {m_speed=speed;}

    void                setFireTM(const AWMatrix4& m) 
        //setPosn(0) required here because of way calcTM works
        //this is essentially a 'reset to new starting posn&orientation'
                            {m_fireTM=m; setPosn(AWPoint(0.0f,0.0f,0.0f));}
    const AWMatrix4&    getFireTM()           {return m_fireTM;}

	//AWPersistable virtuals
	/*
	int				compareTo(const AWPersistable* o) const;
	AWBoolean			isEqual(const AWPersistable* o)const;*/
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

protected:
	float				m_speed;	  //scalar speed in units per second
	AWMatrix4			m_fireTM;
};//class AWRocketController : public AWTransformController




class AWExport AWRocket : public AWObject
{	
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWRocket);

	AWRocket(const AWCString& name = "", AWNode* parent=NULL,
			 float speed = 20.0f, float life = 10.0f);
	~AWRocket();

	AWRocket&			clone(const AWRocket& other);
	virtual AWNode*		clone() const; 

   void           setRenderer(GLRenderer* renderer) {m_renderer=renderer;}

	float				getSpeed()const {return m_speed;}
	float				getLife()const  {return m_life;}
	void				setSpeed(float speed) {m_speed=speed;}
	void				setLife(float life)   {m_life=life;}

    AWObject*           getExplosion()        {return m_explosion;}
    void                setExplosion(AWObject* e) {m_explosion=e;}

	virtual AWBoolean	collideWith(AWObject* target, float time=0.0f);

	void				fire(AWTransformController* parentTC, float time);
	void				showExplosion(const AWMatrix4& fireTM, float time);

	//AWPersistable virtuals
    //virtual void        reAssignTexture(AWPersistable* src, AWPersistable* dest);
	virtual void		restoreGuts(AWFile& file);
    virtual void        saveGuts(AWFile& file) const;
	virtual AWspace     binaryStoreSize() const;

protected:
	float				m_speed; //scalar speed (units per second) for this projectile class
    //hack for SpaceGnats - explosion effect attached to rocket
    AWObject*         m_explosion; 
    GLRenderer*       m_renderer;
};//class AWRocket	: public AWPersistable


#endif