#ifndef __AWPOINT_H
#define __AWPOINT_H

#include "AWToolsCommon.h"

//no virtual functions, and only class data is 
//3 floats. Rely on:
// sizeof(AWPoint) == 3* sizeof(float)
// memory layout is float x float y float z
//
class AWExport AWPoint
{
public:
	AWPoint() : x(0.0f), y(0.0f), z(0.0f) {;}
	AWPoint(float X, float Y, float Z) : x(X), y(Y), z(Z) {;}

	AWPoint(const AWPoint& other)			{*this=other;}
   ~AWPoint() {;}
	AWPoint& operator=(const AWPoint& other){x=other.x; y=other.y; z=other.z;return *this;}
   //int operator==(const AWPoint& o)const {return x==o.x && y==o.y && z==o.z;}
   

//int operator==(const AWPoint& o)
//		{return (x==o.x) && (y==o.y)&&(z==o.z);}
	//data
	float x,y,z;

	float			sqrMagnitude();
	float			magnitude();
	const AWPoint&	normalize();
	void			getMax(const AWPoint& other);
	void			getMin(const AWPoint& other);
	void			assignToFloat4(float* o)
		{o[0]=x; o[1]=y; o[2]=z;}

   const float    operator[](int i)const;
   float&         operator[](int i);

	const AWPoint&	operator+= (const AWPoint& q);
	const AWPoint&	operator-= (const AWPoint& q);
	const AWPoint&	operator/= (float q);
	const AWPoint&	operator*= (float q);
	const AWPoint&	operator*= (const AWPoint& q);

   friend AWFile& operator>>(AWFile& file, AWPoint& obj);
   friend AWFile& operator<<(AWFile& file, AWPoint& obj);
	

	friend float	dot(const AWPoint& p, const AWPoint& q);
	friend AWPoint	cross(const AWPoint& p, const AWPoint& q);

	friend AWExport AWPoint	operator+ (const AWPoint& p, const AWPoint& q);
	friend AWExport AWPoint	operator- (const AWPoint& p, const AWPoint& q);
	friend AWExport AWPoint	operator* (float c, const AWPoint& p);
	friend AWExport AWPoint	operator/ (const AWPoint& p, float c);
	friend AWExport AWPoint	operator- (const AWPoint& p);
	friend AWExport int		operator==(const AWPoint& a, const AWPoint& b)
		{return (a.x==b.x) && (a.y==b.y) && (a.z==b.z);}

   friend AWExport int      operator!=(const AWPoint& a, const AWPoint& b)
      {return !(a==b);}
   
	void			saveGuts(AWFile& file) const;
	void			restoreGuts(AWFile& file);
};//class AWPoint



class AWExport AWColVertex 
{
public:
   AWColVertex(const AWColVertex& p) {*this=p;}
   AWColVertex(unsigned char ix=0, unsigned char iy=0, unsigned char iz=0) 
      : x(ix), y(iy), z(iz) {;}

   ~AWColVertex() {;}

   AWColVertex& operator=(const AWColVertex& ot) {x = ot.x; y = ot.y; z = ot.z; return *this;}
   int operator==(const AWColVertex& o)const {return x==o.x && y==o.y && z==o.z;}

   friend AWFile& operator>>(AWFile& file, AWColVertex& obj);
   friend AWFile& operator<<(AWFile& file, AWColVertex& obj);

   void			saveGuts(AWFile& file) const;
	void			restoreGuts(AWFile& file);

   unsigned char x;
   unsigned char y;
   unsigned char z;
};//class AWColVertex


inline float
AWPoint::sqrMagnitude()
{
	return x*x + y*y + z*z;
}


inline float					
AWPoint::magnitude()
{
	return (float)sqrt(x*x + y*y + z*z);
}//float AWPoint::magnitude()


inline const AWPoint&		
AWPoint::normalize()
{
	double magnitude = sqrt(x*x + y*y + z*z);
	//NOTE: multiplication is faster than division, so use
	//reciprocal of magnitude
	if (magnitude > 0)
	{
		magnitude = 1.0 / magnitude;
		x *= (float)magnitude;
		y *= (float)magnitude;
		z *= (float)magnitude;
	}
	return *this;
}//void	AWPoint::normalize()


inline void			 
AWPoint::getMax(const AWPoint& other)
{
	if (other.x > x) x = other.x;
	if (other.y > y) y = other.y;
	if (other.z > z) z = other.z;
}//void AWPoint::getMax(const AWPoint& other)


inline void			 
AWPoint::getMin(const AWPoint& other)
{
	if (other.x < x) x = other.x;
	if (other.y < y) y = other.y;
	if (other.z < z) z = other.z;
}//void	AWPoint::getMin(const AWPoint& other)


inline float&               
AWPoint::operator[](int i)
{
   switch (i)
   {
   case 0:
      return x;
      break;

   case 1:
      return y;

   case 2:
   default:
      return z;
   }//switch
}//inline float& AWPoint::operator[](int i)


inline const float          
AWPoint::operator[](int i) const
{
   switch (i)
   {
   case 0:
      return x;
      break;

   case 1:
      return y;

   case 2:
   default:
      return z;
   }//switch
}//inline const float AWPoint::operator[](int i) const



inline const AWPoint& 			 
AWPoint::operator+= (const AWPoint& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}//const AWPoint& AWPoint::operator+= (const AWPoint& q)


inline const AWPoint&  			 
AWPoint::operator-= (const AWPoint& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	return *this;
}


inline const AWPoint&  			 
AWPoint::operator/= (float q)
{
	x /= q;
	y /= q;
	z /= q;
	return *this;
}


inline const AWPoint&  			 
AWPoint::operator*= (float q)
{
	x *= q;
	y *= q;
	z *= q;
	return *this;
}

inline const AWPoint&  			 
AWPoint::operator*= (const AWPoint& q)
{
	x *= q.x;
	y *= q.y;
	z *= q.z;
	return *this;
}

inline float dot(const AWPoint& p, const AWPoint& q)
{
	return	(p.x * q.x) + 
			(p.y * q.y) + 
			(p.z * q.z);
}//float dot(const AWPoint& p, const AWPoint& q)


inline AWPoint cross(const AWPoint& p, const AWPoint& q)
{
	// COMPUTE THE CROSS PRODUCT
	AWPoint dest;
	dest.x = (p.y * q.z) - (p.z * q.y);
	dest.y = (p.z * q.x) - (p.x * q.z);
	dest.z = (p.x * q.y) - (p.y * q.x);
	return dest;
}//AWPoint cross(const AWPoint& p, const AWPoint& q)


inline AWPoint operator* (float c, const AWPoint& p)
{
	return AWPoint(c*p.x, c*p.y, c*p.z);
}



inline AWPoint operator/ (const AWPoint& p, float c)
{
	if (0==c)
	{
		return AWPoint(p.x, p.y, p.z);
	}
	else
	{
		return AWPoint(p.x/c, p.y/c, p.z/c);
	}
}//AWPoint operator/ (const AWPoint& p, float c)


inline AWPoint operator- (const AWPoint& p, const AWPoint& q)
{
	return AWPoint(p.x - q.x, p.y - q.y, p.z - q.z);
}


inline AWPoint operator+ (const AWPoint& p, const AWPoint& q)
{
	return AWPoint(p.x + q.x, p.y + q.y, p.z + q.z);
}

inline AWPoint operator- (const AWPoint& p)
{
	return AWPoint(-p.x, -p.y, -p.z);
}


inline AWFile& operator<<(AWFile& file, AWPoint& it)
{
  it.saveGuts(file);
  return file;
}//AWFile& operator<<(AWFile& file, AWPoint& it)


inline AWFile& operator>>(AWFile& file, AWPoint& it)
{
   it.restoreGuts(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWPoint& it)


inline AWFile& operator<<(AWFile& file, AWColVertex& it)
{
  it.saveGuts(file);
  return file;
}//AWFile& operator<<(AWFile& file, AWColVertex& it)


inline AWFile& operator>>(AWFile& file, AWColVertex& it)
{
   it.restoreGuts(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWColVertex& it)



#endif //sentinel __AWPOINT_H
