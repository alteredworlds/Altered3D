#include "AWQuat.h"
#include "AWPoint.h"
#include "AWMatrix4.h"


#define VER_AWQUAT_1					100


const float AWQuat::m_epsilon=1.0e-6f;


AWDEFINE_VERSIONABLE_PERSISTABLE(AWQuat, CLSID_AWQUAT, VER_AWQUAT_1);



AWQuat::AWQuat() : x(0), y(0), z(0), w(1)
{
}


AWQuat::AWQuat(float W, float X, float Y, float Z) : w(W), x(X), y(Y), z(Z)
{
}


AWQuat::AWQuat(const AWPoint& axis, float angle)
{
	fromAxisAngle(axis, angle);
}



AWQuat::~AWQuat()
{
}


//this method is used to detect if there is a rotation specified by this
//Quaternion - helps optimize rendering by preventing unecessary calls to glRotate
//NOTE - initial version assumes that the Quaternion is NORMALIZED
//returns TRUE iff rotation angle is zero
AWBoolean 
AWQuat::isNull()
{
	return ((x == 0.0f) && (y == 0.0f) &&  (z == 0.0f) && (w == 1.0f));
}//AWBoolean AWQuat::isNull()




const AWQuat&
AWQuat::normalize()
{
	double magnitude = sqrt((x * x) +  (y * y) +  (z * z) +  (w * w));
	//NOTE: multiplication is faster than division, so use
	//reciprocal of magnitude
	if (magnitude > 0.0f)
	{
		magnitude = 1.0 / magnitude;
		x *= (float)magnitude;
		y *= (float)magnitude;
		z *= (float)magnitude;
		w *= (float)magnitude;
	}//if (magnitude > 0.0f)
	return *this;
}//void AWQuat::normalize()


const AWQuat& 
AWQuat::fromAxisAngle(const AWPoint& axis, float angle)
{
    // assert:  x^2 + y^2 + z^2 = 1
    double halfAngle = angle*M_PI/360.0f;
    double cs = cos(halfAngle);
    double sn = sin(halfAngle);
    w = (float)cs;
    x = (float)sn*axis.x;
    y = (float)sn*axis.y;
    z = (float)sn*axis.z;
	normalize();
	return *this;
}//const AWQuat& AWQuat::fromAxisAngle(const AWPoint& axis, float angle)


void
AWQuat::toAxisAngle (AWPoint& axis, float& angle)const
{
    double halfAngle = acos(w);
    float sn = (float)sin(halfAngle);
    if ( fabs(sn) > m_epsilon )
    {
        axis.x = x / sn;
        axis.y = y / sn;
        axis.z = z / sn;
        angle = (float)(2.0*halfAngle*180.0/M_PI);
    }
    else
    {
        axis.x = 0.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
        angle = 0.0f;
    }
}//void AWQuat::toAxisAngle(AWPoint& axis, float& angle)



const AWQuat& 
AWQuat::unitInverse()
{
    // requires:  p is a unit AWQuat
    x = -x;
	y = -y;
	z = -z;
    return *this;
}


AWQuat slerp(float t, const AWQuat& p, const AWQuat& q)
{
    // assert:  dot(p,q) >= 0 (obtained by preprocessing input)
    double cs = dot(p,q);
    double sn = sqrt(fabs(1-cs*cs));
    if ( fabs(sn) < AWQuat::m_epsilon )
        return p;

    double angle = atan2(sn,cs);
    double invSn = 1.0/sn;
    double c0 = sin((1-t)*angle)*invSn;
    double c1 = sin(t*angle)*invSn;

    return (float)c0*p + (float)c1*q;
}//AWQuat Slerp (float t, const AWQuat& p, const AWQuat& q)


AWQuat::AWQuat(const AWMatrix4& mat)
{
	fromMatrix(mat);
}//AWQuat::AWQuat(AWMatrix4& mat)



//************************************************************************
//
//  Action:   Converts quaternion representation of a rotation to a matrix
//			representation
//
//  Params:   AWMatrix4 (4x4 matrix)
//
//
//  Comments: remember matrix (in OGL) is represented in COLUMN major form
//
//************************************************************************

void				
AWQuat::toMatrix(AWMatrix4& dest)const
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = x + x;    y2 = y + y;    z2 = z + z;
	xx = x * x2;   xy = x * y2;   xz = x * z2;
	yy = y * y2;   yz = y * z2;   zz = z * z2;
	wx = w * x2;   wy = w * y2;   wz = w * z2;
	//first row
	dest[0] = 1.0f - (yy + zz);
	dest[4] = xy - wz;
	dest[8] = xz + wy;
	dest[12] = 0.0f;
	//second row
	dest[1] = xy + wz;
	dest[5] = 1.0f - (xx + zz);
	dest[9] = yz - wx;
	dest[13] = 0.0f;
	//third row
	dest[2] = xz - wy;
	dest[6] = yz + wx;
	dest[10] = 1.0f - (xx + yy);
	dest[14] = 0.0f;
	//fourth row
	dest[3] = 0.0f;
	dest[7] = 0.0f;
	dest[11] = 0.0f;
	dest[15] = 1.0f;
}//void	AWQuat::toMatrix(AWMatrix4& dest)




const AWQuat&		
AWQuat::fromMatrix(const AWMatrix4& rotate)
{
	double trace = rotate[0] + rotate[5] + rotate[10] + 1; //=4W^2
	//NOTE - this boundary check is still not working well enough
	//for small euler angle -> matrix -> quat -> matrix -> euler
	//get lots of garbage out.
	if (trace > m_epsilon)
	{
		w = (float)sqrt(trace/4.0);
		trace=1.0/4.0*w;
		x = (rotate[10] - rotate[13]) / (float)trace;
		y = (rotate[8] - rotate[2])   / (float)trace;
		z = (rotate[1] - rotate[4])   / (float)trace;
	}
	else
	{
		w = 1.0f;
		x = y = z = 0.0f;
	}
	normalize();
	return *this;
}//const AWQuat& AWQuat::fromMatrix(AWMatrix4 srce)
/*
//---------------------------------------------------------------------------
void MatrixToQuaternion (double rotate[3][3], Quaternion& q)
{
	
}//static void MatrixToQuaternion (double rotate[3][3], Quaternion& q)
*/
/*SDOC***********************************************************************

  Name:		gluEulerToQuat_EXT

  Action:   Converts representation of a rotation from Euler angles to
			quaternion representation

  Params:   GLfloat (roll), GLfloat (pitch), GLfloat (yaw), GL_QUAT* (quat)

  Returns:  nothing

  Comments: remember:	roll  - rotation around X axis
						pitch - rotation around Y axis
						yaw   - rotation around Z axis
			
			rotations are performed in the following order:
					yaw -> pitch -> roll

			Qfinal = Qyaw Qpitch Qroll

***********************************************************************EDOC*/
//const AWQuat&  
//AWQuat::fromEulerZYX(float Ez, float Ex, float Ey)
const AWQuat& 
AWQuat::fromEulerZYX(float Ex, float Ey, float Ez)
{	
	double cr, cp, cy, sr, sp, sy, cpcy, spsy;
	
	// FIRST STEP, CONVERT ANGLES TO RADIAN HALF ANGLES
	Ex  = (Ex * (float)M_PI) / 360.0f;
	Ey  = (Ey * (float)M_PI) / 360.0f;
	Ez  = (Ez * (float)M_PI) / 360.0f;

	cr = cos(Ex);
	cp = cos(Ey);
	cy = cos(Ez);

	sr = sin(Ex);
	sp = sin(Ey);
	sy = sin(Ez);
	
	cpcy = cp * cy;
	spsy = sp * sy;

	w = (float)(cr * cpcy + sr * spsy);
	x = (float)(sr * cpcy - cr * spsy);
	y = (float)(cr * sp * cy + sr * cp * sy);
	z = (float)(cr * cp * sy - sr * sp * cy);
	normalize();

	return *this;
}//const AWQuat&  AWQuat::fromEulerZYX(float x, float y, float z)

///////////////////////////////////////////////////////////////////////////////
// Function:	fromEuler
// Purpose:		Convert a set of Euler angles to a Quaternion
// Arguments:	A rotation set of 3 angles, a quaternion to set
// Discussion:  As the order of rotations is important I am
//				using the Quantum Mechanics convention of (X,Y,Z)
//				a Yaw-Pitch-Roll (Y,X,Z) system would have to be
//				adjusted.  It is more efficient this way though.
///////////////////////////////////////////////////////////////////////////////
const AWQuat& 
AWQuat::fromEuler(const AWPoint& rot)
{
	float rx,ry,rz,tx,ty,tz,cx,cy,cz,sx,sy,sz,cc,cs,sc,ss;
	// FIRST STEP, CONVERT ANGLES TO RADIANS
	rx =  (rot.x * (float)M_PI) / (360 / 2);
	ry =  (rot.y * (float)M_PI) / (360 / 2);
	rz =  (rot.z * (float)M_PI) / (360 / 2);
	// GET THE HALF ANGLES
	tx = rx * 0.5f;
	ty = ry * 0.5f;
	tz = rz * 0.5f;
	cx = (float)cos(tx);
	cy = (float)cos(ty);
	cz = (float)cos(tz);
	sx = (float)sin(tx);
	sy = (float)sin(ty);
	sz = (float)sin(tz);

	cc = cx * cz;
	cs = cx * sz;
	sc = sx * cz;
	ss = sx * sz;

	x = (cy * sc) - (sy * cs);
	y = (cy * ss) + (sy * cc);
	z = (cy * cs) - (sy * sc);
	w = (cy * cc) + (sy * ss);

	// INSURE THE QUATERNION IS NORMALIZED
	// PROBABLY NOT NECESSARY IN MOST CASES
	normalize();
	return *this;
}//void AWQuat::fromEuler(const AWPoint& rot)



AWQuat operator+ (const AWQuat& p, const AWQuat& q)
{
    return AWQuat(p.w+q.w, p.x+q.x, p.y+q.y, p.z+q.z);
}


AWQuat operator- (const AWQuat& p, const AWQuat& q)
{
    return AWQuat(p.w-q.w, p.x-q.x, p.y-q.y, p.z-q.z);
}


AWQuat operator- (const AWQuat& p)
{
    return AWQuat(-p.w, -p.x, -p.y, -p.z );
}



AWQuat	operator/ (const AWQuat& q1, const AWQuat& q2)
{
	//Action:   Divide two quaternions
	//Params:   GL_QUAT* (q1 / q2 = res)
	AWQuat r, s, q(q2);
	// invert vector
    q.x = -q.x;
    q.y = -q.y;
    q.z = -q.z;
	r = q1 * q;
	s = q  * q;
	//gluQuatMul_EXT(q1, &q, &r);
	//gluQuatMul_EXT(&q, &q, &s);
	return AWQuat(r.x / s.w, r.y / s.w, r.z / s.w, r.w / s.w);
}//AWQuat	operator/ (const AWQuat& q1, const AWQuat& q2)


/*
SDOC***********************************************************************

  Name:		gluQuatMul_EXT

  Action:   Multiplies two quaternions

  Params:   GL_QUAT ( q1 * q2 = res)

  Returns:  nothing

  Comments: NOTE: multiplication is not commutative

***********************************************************************EDOC
void APIENTRY gluQuatMul_EXT(GL_QUAT* q1, GL_QUAT* q2, GL_QUAT* res)
{

	res->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
	res->y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
	res->z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
	res->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;


	// make sure the resulting quaternion is a unit quat.
	gluQuatNormalize_EXT(res);

}

*/


AWQuat operator* (const AWQuat& p, const AWQuat& q)
{
    // NOTE:  Multiplication is not commutative - p*q != q*p.
    return 
		AWQuat
		(
			p.w*q.w-p.x*q.x-p.y*q.y-p.z*q.z,
			p.w*q.x+p.x*q.w+p.y*q.z-p.z*q.y,
			p.w*q.y+p.y*q.w+p.z*q.x-p.x*q.z,
			p.w*q.z+p.z*q.w+p.x*q.y-p.y*q.x
		).normalize();
}


AWQuat operator* (float c, const AWQuat& p)
{
    return AWQuat(c*p.w, c*p.x, c*p.y, c*p.z);
}



float dot (const AWQuat& p, const AWQuat& q)
{
    return p.w*q.w+p.x*q.x+p.y*q.y+p.z*q.z;
}







//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWQuat
//
//////////////////////////////////////////////////////////
void		
AWQuat::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (VER_AWQUAT_1 == version)
	{
		AWPersistable::restoreGuts(file);
		file.Read(x);
		file.Read(y);
		file.Read(z);
		file.Read(w);
	}
}//void	AWQuat::restoreGuts(AWFile& file)



void        
AWQuat::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file.Write(x);
	file.Write(y);
	file.Write(z);
	file.Write(w);
}


AWspace     
AWQuat::binaryStoreSize() const
{
	AWspace count = sizeof uVersion_ +
		AWPersistable::binaryStoreSize() +
		sizeof(x) + sizeof(y) + sizeof(z) + sizeof(w); 
	return count;
}

