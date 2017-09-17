//
//  AWPoint44.h
//  iCritter
//
//  Created by Tom Gilbert on 01/12/2012.
//
//

#ifndef iCritter_AWPoint4_h
#define iCritter_AWPoint4_h

#include "AWToolsCommon.h"

//no virtual functions, and only class data is
//4 floats. Rely on:
// sizeof(AWPoint4) == 4 * sizeof(float)
// memory layout is float x float y float z float x
//
class AWExport AWPoint4
{
public:
	AWPoint4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {;}
	AWPoint4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {;}
    
	AWPoint4(const AWPoint4& other)			{*this=other;}
    ~AWPoint4() {;}
	AWPoint4& operator=(const AWPoint4& other){x=other.x; y=other.y; z=other.z;; w=other.w; return *this;}
    
	float x, y, z, w;
	
	AWBoolean           isZero();
	void				setZero();
	double              distanceFrom(const AWPoint4& p2);
    
	float				sqrMagnitude();
	float				magnitude();
	const AWPoint4&     normalize();
	void				getMax(const AWPoint4& other);
	void				getMin(const AWPoint4& other);
	void				assignToFloat4(float* o) {o[0]=x; o[1]=y; o[2]=z; o[3]=w;}
    
    const float         operator[](int i)const;
    float&              operator[](int i);
    
	const AWPoint4&     operator+= (const AWPoint4& q);
	const AWPoint4&     operator-= (const AWPoint4& q);
	const AWPoint4&     operator/= (float q);
	const AWPoint4&     operator*= (float q);
	const AWPoint4&     operator*= (const AWPoint4& q);
    
    friend AWFile&      operator>>(AWFile& file, AWPoint4& obj);
    friend AWFile&      operator<<(AWFile& file, AWPoint4& obj);
	
    
	friend float        dot(const AWPoint4& p, const AWPoint4& q);
	friend AWPoint4     cross(const AWPoint4& p, const AWPoint4& q);
    
	friend AWExport AWPoint4	operator+ (const AWPoint4& p, const AWPoint4& q);
	friend AWExport AWPoint4	operator- (const AWPoint4& p, const AWPoint4& q);
	friend AWExport AWPoint4	operator* (float c, const AWPoint4& p);
	friend AWExport AWPoint4	operator/ (const AWPoint4& p, float c);
	friend AWExport AWPoint4	operator- (const AWPoint4& p);
	friend AWExport int         operator==(const AWPoint4& a, const AWPoint4& b)
            {return (a.x==b.x) && (a.y==b.y) && (a.z==b.z) && (a.w==b.w);}
    
    friend AWExport int         operator!=(const AWPoint4& a, const AWPoint4& b)
            {return !(a==b);}
    
	void                        saveGuts(AWFile& file) const;
	void                        restoreGuts(AWFile& file);
};


inline AWBoolean
AWPoint4::isZero()
{
	return (0.0 == x) && (0.0 == y) && (0.0 == z) && (0.0 == w);
}

inline void
AWPoint4::setZero()
{
	x = 0.0; y = 0.0; z = 0.0; w = 0.0;
}

inline float
AWPoint4::sqrMagnitude()
{
	return x*x + y*y + z*z + w*w;
}

inline float
AWPoint4::magnitude()
{
	return (float)sqrt(x*x + y*y + z*z + w*w);
}

inline const AWPoint4&
AWPoint4::normalize()
{
	double magnitude = sqrt(x*x + y*y + z*z + w*w);
	//NOTE: multiplication is faster than division, so use
	//reciprocal of magnitude
	if (magnitude > 0)
	{
		magnitude = 1.0 / magnitude;
		x *= (float)magnitude;
		y *= (float)magnitude;
		z *= (float)magnitude;
        w *= (float)magnitude;
	}
	return *this;
}

inline void
AWPoint4::getMax(const AWPoint4& other)
{
	if (other.x > x) x = other.x;
	if (other.y > y) y = other.y;
	if (other.z > z) z = other.z;
    if (other.w > w) w = other.w;
}

inline void
AWPoint4::getMin(const AWPoint4& other)
{
	if (other.x < x) x = other.x;
	if (other.y < y) y = other.y;
	if (other.z < z) z = other.z;
    if (other.w < w) w = other.w;
}

inline float&
AWPoint4::operator[](int i)
{
    switch (i)
    {
        case 0:
            return x;
            break;
            
        case 1:
            return y;
            
        case 2:
            return z;
            
        case 3:
        default:
            return w;
    }//switch
}//inline float& AWPoint4::operator[](int i)


inline const float
AWPoint4::operator[](int i) const
{
    switch (i)
    {
        case 0:
            return x;
            break;
            
        case 1:
            return y;
            
        case 2:
            return z;
            
        case 3:
        default:
            return w;
    }//switch
}//inline const float AWPoint4::operator[](int i) const



inline const AWPoint4&
AWPoint4::operator+= (const AWPoint4& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
    w += q.w;
	return *this;
}//const AWPoint4& AWPoint4::operator+= (const AWPoint4& q)


inline const AWPoint4&
AWPoint4::operator-= (const AWPoint4& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
    w -= q.w;
	return *this;
}


inline const AWPoint4&
AWPoint4::operator/= (float q)
{
	x /= q;
	y /= q;
	z /= q;
    w /= q;
	return *this;
}


inline const AWPoint4&
AWPoint4::operator*= (float q)
{
	x *= q;
	y *= q;
	z *= q;
    w *= q;
	return *this;
}

inline const AWPoint4&
AWPoint4::operator*= (const AWPoint4& q)
{
	x *= q.x;
	y *= q.y;
	z *= q.z;
    w *= q.w;
	return *this;
}

inline float dot(const AWPoint4& p, const AWPoint4& q)
{
	return
    (p.x * q.x) +
    (p.y * q.y) +
    (p.z * q.z) +
    (p.w * q.w);
}//float dot(const AWPoint4& p, const AWPoint4& q)


inline AWPoint4 operator* (float c, const AWPoint4& p)
{
	return AWPoint4(c*p.x, c*p.y, c*p.z, c*p.w);
}


inline AWPoint4 operator/ (const AWPoint4& p, float c)
{
	if (0==c)
	{
		return AWPoint4(p.x, p.y, p.z, p.w);
	}
	else
	{
		return AWPoint4(p.x/c, p.y/c, p.z/c, p.w/c);
	}
}//AWPoint4 operator/ (const AWPoint4& p, float c)


inline AWPoint4 operator- (const AWPoint4& p, const AWPoint4& q)
{
	return AWPoint4(p.x - q.x, p.y - q.y, p.z - q.z, p.w - q.w);
}


inline AWPoint4 operator+ (const AWPoint4& p, const AWPoint4& q)
{
	return AWPoint4(p.x + q.x, p.y + q.y, p.z + q.z, p.w + q.w);
}

inline AWPoint4 operator- (const AWPoint4& p)
{
	return AWPoint4(-p.x, -p.y, -p.z, -p.w);
}


inline AWFile& operator<<(AWFile& file, AWPoint4& it)
{
    it.saveGuts(file);
    return file;
}//AWFile& operator<<(AWFile& file, AWPoint4& it)


inline AWFile& operator>>(AWFile& file, AWPoint4& it)
{
    it.restoreGuts(file);
    return file;
}//AWFile& operator>>(AWFile& file, AWPoint4& it)

#endif
