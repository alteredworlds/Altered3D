#include "AWToolsCommon.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWMatrix4.h"

static float FAC_PI = (float)(2.0*M_PI/360.0);


void		 		 
AWMatrix4::awRotateX(float angle, bool degress)
{	//this is probably a very slow way to do it, but this is 
	//just for testing purposes right now. Optimize LATER!
	AWMatrix4 mult;
	float radAngle = degress ? angle * FAC_PI : angle;
	mult[5] = (float)cos(radAngle);
	mult[6] = (float)sin(radAngle);
	mult[9] = -mult[6];
	mult[10]= mult[5];
	(*this) = (*this) * mult;
}


void		 		 
AWMatrix4::awRotateY(float angle, bool degress)
{  //this is probably a very slow way to do it, but this is 
	//just for testing purposes right now. Optimize LATER!
	AWMatrix4 mult;
	float radAngle = degress ? angle * FAC_PI : angle;
	mult[0] = (float)cos(radAngle);
	mult[8] = (float)sin(radAngle);
	mult[2] = -mult[8];
	mult[10]= mult[0];
	(*this) = (*this) * mult;
}


void		 		 
AWMatrix4::awRotateZ(float angle, bool degress)
{	//this is probably a very slow way to do it, but this is 
	//just for testing purposes right now. Optimize LATER!
	AWMatrix4 mult;
	float radAngle = degress ? angle * FAC_PI : angle;
	mult[0] = (float)cos(radAngle);
	mult[1] = (float)sin(radAngle);
	mult[4] = -mult[1];
	mult[5]= mult[0];
	(*this) = (*this) * mult;
}


void		 
AWMatrix4::invert(AWMatrix4& inv)const
{	
	AWMatrix4 temp(*this);
	inv.identity();
	//deal with the first Pivot point
	float factor = 1.0f / temp[0];
	temp.multRow(0, factor);
	inv.multRow(0, factor);
	inv.subRow(1, 0, temp[1]);
	temp.subRow(1, 0, temp[1]);
	inv.subRow(2, 0, temp[2]);
	temp.subRow(2, 0, temp[2]);
	//deal with second pivot point
	factor = 1.0f / temp[5];
	inv.multRow(1, factor);
	temp.multRow(1, factor);
	inv.subRow(0, 1, temp[4]);
	temp.subRow(0, 1, temp[4]);
	inv.subRow(2, 1, temp[6]);
	temp.subRow(2, 1, temp[6]);
	//deal with third pivot point
	factor = 1.0f / temp[10];
	temp.multRow(2, factor);
	inv.multRow(2, factor);
	inv.subRow(0, 2, temp[8]);
	temp.subRow(0, 2, temp[8]);
	inv.subRow(1, 2, temp[9]);
	temp.subRow(1, 2, temp[9]);
	//deal with fourth pivot point - is this required?
	inv.subRow(0, 3, temp[12]);
	temp.subRow(0, 3, temp[12]);
	inv.subRow(1, 3, temp[13]);
	temp.subRow(1, 3, temp[13]);
	inv.subRow(2, 3, temp[14]);
	temp.subRow(2, 3, temp[14]);
}//void AWMatrix4::invert(AWMatrix4& inv)


void		
AWMatrix4::dump()
{
	AWTRACEHI4("(%12f, %12f, %12f, %12f)\n", 
				(double)m_mat[0], (double)m_mat[4], (double)m_mat[8], (double)m_mat[12]);
	AWTRACEHI4("(%12f, %12f, %12f, %12f)\n", 
				(double)m_mat[1], (double)m_mat[5], (double)m_mat[9], (double)m_mat[13]);
	AWTRACEHI4("(%12f, %12f, %12f, %12f)\n", 
				(double)m_mat[2], (double)m_mat[6], (double)m_mat[10], (double)m_mat[14]);
	AWTRACEHI4("(%12f, %12f, %12f, %12f)\n", 
				(double)m_mat[3], (double)m_mat[7], (double)m_mat[11], (double)m_mat[15]);
}

//EXPERIMENTAL: doesn't yet work.
void
AWMatrix4::lookAt(const AWPoint& direction, const AWPoint& upVector3D)
{
	AWPoint forward(direction), side, up;
	identity();
	//------------------
	forward.normalize();
	//------------------
	//Side = forward x up
	side = cross(forward, upVector3D);
	side.normalize();
	//------------------
	//Recompute up as: up = side x forward
	up = cross(side, forward);
	//------------------
	m_mat[0] = side.x;
	m_mat[4] = side.y;
	m_mat[8] = side.z;
	m_mat[12] = 0.0f;
	//------------------
	m_mat[1] = up.x;
	m_mat[5] = up.y;
	m_mat[9] = up.z;
	m_mat[13] = 0.0f;
	//------------------
	m_mat[2] = -forward.x;
	m_mat[6] = -forward.y;
	m_mat[10] = -forward.z;
	m_mat[14] = 0.0f;
	//------------------
	m_mat[3] = m_mat[7] = m_mat[11] = 0.0f;
	m_mat[15] = 1.0f;
}