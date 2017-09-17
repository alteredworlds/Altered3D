#ifndef __AWLIGHT_H
#define __AWLIGHT_H


#include "AWPoint.h"
#include "AWNode.h"
#include "AWAnimatedPoint.h"

class AWExport AWTexture; //forwards

class AWExport AWLight : public AWNode
{
public:
	enum LightType {
		AW_POINT_LIGHT, 
		AW_DIRECTIONAL_LIGHT, 
		AW_SPOT_LIGHT};

	AWDECLARE_VERSIONABLE_PERSISTABLE(AWLight);

	AWLight(const AWCString& name = "", AWNode* parent = NULL, LightType type = AW_POINT_LIGHT);

	AWLight(const AWCString& name, AWNode* parent, 
		    AWBoolean on, const AWPoint& col, const AWPoint& atten);

	virtual		~AWLight();

	virtual void compact();

	AWBoolean	isColorAnimated()				{return m_colour.isAnimated();}
	void		calcColor(float t)				{m_colour.calc(t);}
	void		getColor(AWPoint& col)			{col=m_colour.getVal();}
	void		setColor(const AWPoint& col)	{m_colour.setVal(col);}
	void		addColor(const AWPoint& col, float t)	
												{m_colour.addKey(col, t);}

   virtual void   getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM);
	//update the object state with the values for the specified time
	virtual int		calcTransform(float t, const AWMatrix4& parentTM);

//AWPersistable virtuals
	virtual void	restoreGuts(AWFile& file);
    virtual void	saveGuts(AWFile& file) const;

	virtual AWspace binaryStoreSize() const;
	//virtual void debugDump(ofstream& out)const;
	//AWBoolean		isEqual(const AWPersistable* o) const{return m_name == ((AWLight*)o)->m_name;}
	//int				compareTo(const AWPersistable* o) const{return isEqual(o) ? 0 : -1;}

public:
	int				   m_ID;	//not persisted - GL state data
	LightType		   m_type;
	AWBoolean		   m_on;
	AWAnimatedPoint   m_colour;
	AWPoint			   m_attenuation;
	float			      m_cutOffAngle; //degrees
   AWTexture*        m_proj;        //projected texture
   AWTexture*        m_shadow;      //shadow map
};//class AWLight : public AWNode


inline 
AWLight::AWLight(const AWCString& name, AWNode* parent, 
		    AWBoolean on, const AWPoint& col, const AWPoint& atten)
			: AWNode(name, parent), m_on(on),
			  m_attenuation(atten), m_proj(NULL) 
{
   m_colour.setVal(col);
}


inline void
AWLight::getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM)
{
   AWNode::getViewCone(viewCone, wsTM);
   viewCone.setAngle(2*m_cutOffAngle);
}//inline void AWLight::getViewCone(AWSolidCone& viewCone, AWMatrix4& wsTM)



inline void
AWLight::compact()
{
	m_colour.compact();
}//void AWLight::compact()

#endif //sentinel __AWLIGHT_H
