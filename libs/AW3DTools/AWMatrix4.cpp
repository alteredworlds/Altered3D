#include "AWToolsCommon.h"
#include "AWPoint.h"
#include "AWQuat.h"
#include "AWMatrix4.h"

static float FAC_PI = (float)(2.0*M_PI/360.0);


void		 		 
AWMatrix4::awRotateX(float angle)
{	//this is probably a very slow way to do it, but this is 
	//just for testing purposes right now. Optimize LATER!
	AWMatrix4 mult;
	float radAngle = angle * FAC_PI;
	mult[5] = (float)cos(radAngle);
	mult[6] = (float)sin(radAngle);
	mult[9] = -mult[6];
	mult[10]= mult[5];
	(*this) = (*this) * mult;
}


void		 		 
AWMatrix4::awRotateY(float angle)
{  //this is probably a very slow way to do it, but this is 
	//just for testing purposes right now. Optimize LATER!
	AWMatrix4 mult;
	float radAngle = angle * FAC_PI;
	mult[0] = (float)cos(radAngle);
	mult[8] = (float)sin(radAngle);
	mult[2] = -mult[8];
	mult[10]= mult[0];
	(*this) = (*this) * mult;
}


void		 		 
AWMatrix4::awRotateZ(float angle)
{	//this is probably a very slow way to do it, but this is 
	//just for testing purposes right now. Optimize LATER!
	AWMatrix4 mult;
	float radAngle = angle * FAC_PI;
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
