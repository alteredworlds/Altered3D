#ifndef __AWQUAT_H
#define __AWQUAT_H


#include "AWToolsCommon.h"

class AWExport AWPoint;
class AWExport AWMatrix4;


class AWExport AWQuat : public AWPersistable  
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWQuat);

   //friend class AWInitAWQuat2;

	AWQuat();
	AWQuat(float W, float X, float Y, float Z);
	AWQuat(const AWMatrix4& mat);
	AWQuat(const AWPoint& axis, float angle);

	int operator==(const AWQuat& o)const 
   {return  ((float)fabs(x-o.x)<m_epsilon)&& 
            ((float)fabs(y-o.y)<m_epsilon)&&
            ((float)fabs(z-o.z)<m_epsilon)&&
            ((float)fabs(w-o.w)<m_epsilon);}

	virtual			~AWQuat();

	const AWQuat&	fromEulerZYX(float roll, float pitch, float yaw);
	const AWQuat&	fromEuler(const AWPoint& rot);

	const AWQuat&	fromAxisAngle(const AWPoint& axis, float angle);
	void			toAxisAngle(AWPoint& axis, float& angle)const;

	const AWQuat&	fromMatrix(const AWMatrix4& srce);
	void			toMatrix(AWMatrix4& dest)const;

	const AWQuat&	unitInverse();
	const AWQuat&	normalize();
	AWBoolean		isNull();	//returns TRUE if this is zero rotation;

	friend AWExport AWQuat	operator+ (const AWQuat& p, const AWQuat& q);
	friend AWExport AWQuat	operator- (const AWQuat& p, const AWQuat& q);
	friend AWExport AWQuat	operator- (const AWQuat& p);
	friend AWExport AWQuat	operator* (const AWQuat& p, const AWQuat& q);
	friend AWExport AWQuat	operator/ (const AWQuat& p, const AWQuat& q);
	friend AWExport AWQuat	operator* (float c, const AWQuat& p);
	friend AWExport float	dot (const AWQuat& p, const AWQuat& q);
	friend AWExport AWQuat	slerp (float t, const AWQuat& p, const AWQuat& q);

	//AWPersistable virtuals
   virtual void	restoreGuts(AWFile& file);
   virtual void    saveGuts(AWFile& file) const;

	virtual AWspace binaryStoreSize() const;

   static const float m_epsilon;

	//data
	float			x, y, z, w;
};//class AWQuat : public AWPersistable 

#endif //sentinel __AWQUAT_H
