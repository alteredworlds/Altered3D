#ifndef __AWMATRIX4_H
#define __AWMATRIX4_H

#include "AWPoint.h"
#include "AWQuat.h"


#define STACK_DEPTH 32
static double epsilon = 1e-06;

//represents and OpenGL / AWEngine style 
//COLUMN-MAJOR 4x4 Transformation Matrix
//	[m0		m4		m8		m12]
//	[m1		m5		m9		m13]
//	[m2		m6		m10		m14]
//	[m3		m7		m11		m15]
class AWExport AWMatrix4
{
public:
	AWMatrix4()	{identity();}

   AWBoolean   isIdentity();
	void		 identity();//set to identity matrix
	void		 invert(AWMatrix4& inv)const;	//assumes is glModelView matrix
   AWMatrix4& invertAffine();
	const float  operator[] (int index)const		{return (const float)m_mat[index];}
	float&		 operator[] (int index)				{return m_mat[index];}
	float&		 operator() (int row, int col)		{return m_mat[col*4 + row];}
	const float	 operator() (int row, int col)const {return (const float)m_mat[col*4 + row];}

	AWMatrix4	 operator* (const AWMatrix4& mat) const;
	AWPoint		 operator* (const AWPoint& vec)const;
	AWMatrix4&	 operator= (const AWMatrix4& other);
   int          operator==(const AWMatrix4& other)const;
   int          operator!=(const AWMatrix4& other)const;

	//mimic gl matrix operations
	void		 awTranslate(const AWPoint& trans);
	void		 awRotate(const AWQuat& rot);
   void		 awRotate(float angle, float x, float y, float z);
	void		 awScale(const AWPoint& scale);
	void		 awRotateX(float angle);
	void		 awRotateY(float angle);
	void		 awRotateZ(float angle);

   void      frustum(float l, float r, float b, float t,
				         float zNear, float zFar);
   void      frustumInverse(float l, float r, float b, float t, 
                            float zNear, float zFar);
   void      perspective(float fovy, float aspect, float zNear, float zFar);
   void      perspectiveInverse(float fovy, float aspect, float zNear, float zFar);

	//helper functions for finding inverse matrix
	void		 multRow(int row, float factor);
	void		 subRow(int subFromRow, int subRow, float factor);

	//get / set translation component of matrix
	void		 setTranslation(const AWPoint& trans)
					{m_mat[12]=trans.x; m_mat[13]=trans.y; 
					 m_mat[14]=trans.z; m_mat[15] = 1.0f;}
	void		 getTranslation(AWPoint& vec)const
					{vec.x=m_mat[12]; vec.y=m_mat[13]; vec.z=m_mat[14];}
	void		 getDirection(AWPoint& dir)const
					{dir.x=m_mat[8]; dir.y=m_mat[9]; dir.z=m_mat[10];}
	void		 setScale(const AWPoint& scale)
					{m_mat[0] =scale.x; m_mat[5] =scale.y; 
					 m_mat[10]=scale.z; m_mat[15]=1.0f;}
	void		 getScale(AWPoint& scale)const
					{scale.x=m_mat[0]; scale.y=m_mat[5]; scale.z=m_mat[10];}

	void		 getEulerAngles(AWPoint& euler);

	float m_mat[16];
	//helper function
protected:
	void		 swap(float& a, float& b) {float temp=a;a=b;b=temp;}
	float		 clamp(float val, float min, float max)
					{if (val  > max) val  -= max; if (val  < min) val  += max; return val;}
};




class AWExport AWMatrix4Stack : public AWMatrix4
{	//LIFO structore to simulate OpenGL matrix stack(!)
public:
	AWMatrix4Stack() : m_stackIndex(-1) {;}
	int	awPush();
	int	awPop();

protected:
	//initial stack size is 32 4x4 matrices. 
	//can be altered if desired - but for now the size is fixed, and
	//overflow generates a return code of 0 from fns above.
	AWMatrix4 m_stack[STACK_DEPTH];
	int		  m_stackIndex;
};




//initialise the matrix to identity
inline void
AWMatrix4::identity()
{
	m_mat[0] = m_mat[5]	= m_mat[10]	= m_mat[15] = 1.0f;
	m_mat[4] = m_mat[8]	= m_mat[12] = m_mat[1] = m_mat[9] = m_mat[13] = 0.0f;
	m_mat[2] = m_mat[6]	= m_mat[14] = m_mat[3] = m_mat[7] = m_mat[11] = 0.0f;		
}


inline AWBoolean   
AWMatrix4::isIdentity()
{  //doesn't use epsilon like it should - oh well
   //we want to recognise an assigned/ explicitly set identity
   //rather than a calculated one, for now.
   return ( (1.0f == m_mat[0])  && (1.0f == m_mat[5]) && 
            (1.0f == m_mat[10]) && (1.0f == m_mat[15]) &&
            !m_mat[4]  && !m_mat[8]  && !m_mat[12] && !m_mat[1] && 
            !m_mat[9]  && !m_mat[13] && !m_mat[2]  && !m_mat[6] && 
            !m_mat[14] && !m_mat[3]  && !m_mat[7]  && !m_mat[11]);	
}//inline AWBoolean AWMatrix4::isIdentity()


inline void		 
AWMatrix4::multRow(int row, float factor)
{	//this fn. is not protected against bad arguments
	//argument for row should be [0..3] inclusive
	m_mat[row]		*= factor;
	m_mat[4 + row]	*= factor;
	m_mat[8 + row]	*= factor;
	m_mat[12 + row]	*= factor;
}


inline void 
AWMatrix4::getEulerAngles(AWPoint& euler)
{
	//for now this routine always assumes rotation order
	//suitable for plane as per gl red book p119
	/*
	Given that the rotation matrix is:         
	|  CyCz   CzSySx-SzCx  CyCx			0 |
    |  SzCy   SzSySx+CzCx  SzSyCx-CzSx  0 |         
	|  -Sy    CySx		   CyCx			0 |
    |  0      0			   0			1 |
  where Cx,Sx are the cosine and sine of the X-axis rotation axis,
        Cy,Sy are the cosine and sine of the Y-axis rotation axis,
        Cz,Sz are the cosine and sine of the Z-axis rotation axis.

  derived from multiplying out the product RzRyRx where Rx is the XRotn matrix
  corresponds to VIEWING TRANSFORM -
  glRotate(z, 0, 0, 1)
  glRotate(y, 0, 1, 0)
  glRotate(x, 1, 0, 0)
    */
	euler.y  = (float)asin( -m_mat[2]);    
	double Cy =  cos( euler.y );
    euler.y  *= (float)radToDeg;  
	double	 S, C;
	if ( fabs( euler.y ) > 0.0005f )      
	{
		C		=  m_mat[10] / Cy;      
		S		=  m_mat[6]  / Cy;
		euler.x =  (float)(atan2( S, C ) * radToDeg);      
		C		=  m_mat[0] / Cy;
		S		=  m_mat[1] / Cy;      
		euler.z =  (float)(atan2( S, C ) * radToDeg);
    }    
	else      
	{      
		euler.x = 0.0f;      
		C		= m_mat[5];
		S		= -m_mat[4];      
		euler.z = (float)(atan2( S, C ) * radToDeg);      
	}
    euler.x = clamp( euler.x, 0, 360 );    
	euler.y = clamp( euler.y, 0, 360 );
    euler.z = clamp( euler.z, 0, 360 );
}//inline void AWMatrix4::getEulerAngles(AWPoint& euler)


inline void		 
AWMatrix4::subRow(int subFromRow, int subRow, float factor)
{	//this fn. is not protected against bad arguments
	m_mat[subFromRow]		-= factor * m_mat[subRow];
	m_mat[4 + subFromRow]	-= factor * m_mat[4 + subRow];
	m_mat[8 + subFromRow]	-= factor * m_mat[8 + subRow];
	m_mat[12 + subFromRow]	-= factor * m_mat[12 + subRow];
}



//copy via assignment
inline AWMatrix4&	 
AWMatrix4::operator= (const AWMatrix4& other)
{	//can we have for loops in inline functions?
	memcpy(m_mat, other.m_mat, sizeof(m_mat));
	return *this;
}//AWMatrix4& AWMatrix4::operator= (const AWMatrix4& other)


inline int         
AWMatrix4::operator==(const AWMatrix4& other)const
{
   return 0==memcmp(m_mat, other.m_mat, sizeof(m_mat));
}


inline int         
AWMatrix4::operator!=(const AWMatrix4& other)const
{
   return memcmp(m_mat, other.m_mat, sizeof(m_mat));
}


//---------------------------------------------------------------------------
//operate with this matrix on the supplied vec, returning the result
inline AWPoint		 
AWMatrix4::operator* (const AWPoint& vec)const
{
	return AWPoint(m_mat[0]*vec.x + m_mat[4]*vec.y + m_mat[8]*vec.z + m_mat[12],
				   m_mat[1]*vec.x + m_mat[5]*vec.y + m_mat[9]*vec.z + m_mat[13],
				   m_mat[2]*vec.x + m_mat[6]*vec.y + m_mat[10]*vec.z + m_mat[14]);
}


inline void		  
AWMatrix4::awTranslate(const AWPoint& trans)
{
	AWMatrix4 transMat;
	transMat.setTranslation(trans);
	(*this) = (*this) * transMat;
}//void AWMatrix4::awTranslate(const AWPoint& trans)


inline void		  
AWMatrix4::awRotate(const AWQuat& rot)
{
	AWMatrix4 rotMat;
	rot.toMatrix(rotMat);
	(*this) = (*this) * rotMat;
}//void AWMatrix4::awRotate(const AWQuat& rot)


inline void		 
AWMatrix4::awRotate(float angle, float x, float y, float z)
{
   AWQuat tmp(AWPoint(x, y, z), angle);
   awRotate(tmp);  
}//inline void	AWMatrix4::awRotate(float angle, float x, float y, float z)



inline void		  
AWMatrix4::awScale(const AWPoint& scale)
{
	AWMatrix4 scaleMat;
	scaleMat.setScale(scale);
	(*this) = (*this) * scaleMat;
}//void AWMatrix4::awScale(const AWPoint& scale)


//---------------------------------------------------------------------------
inline AWMatrix4 
AWMatrix4::operator* (const AWMatrix4& mat) const
{
    AWMatrix4 prd;
	//first col
    prd[0] = 
		(*this)[0]*mat[0]+
        (*this)[4]*mat[1]+
		(*this)[8]*mat[2]+
        (*this)[12]*mat[3];
    prd[1] =
        (*this)[1]*mat[0]+
        (*this)[5]*mat[1]+
        (*this)[9]*mat[2]+
		(*this)[13]*mat[3];
    prd[2] =
        (*this)[2]*mat[0]+
        (*this)[6]*mat[1]+
        (*this)[10]*mat[2]+
		(*this)[14]*mat[3];
	prd[3] =
        (*this)[3]*mat[0]+
        (*this)[7]*mat[1]+
        (*this)[11]*mat[2]+
		(*this)[15]*mat[3];
	//second column
    prd[4] =
        (*this)[0]*mat[4]+
        (*this)[4]*mat[5]+
        (*this)[8]*mat[6]+
		(*this)[12]*mat[7];
    prd[5] =
        (*this)[1]*mat[4]+
        (*this)[5]*mat[5]+
        (*this)[9]*mat[6]+
		(*this)[13]*mat[7];
    prd[6] =
        (*this)[2]*mat[4]+
        (*this)[6]*mat[5]+
        (*this)[10]*mat[6]+
		(*this)[14]*mat[7];
	prd[7] =
        (*this)[3]*mat[4]+
        (*this)[7]*mat[5]+
        (*this)[11]*mat[6]+
		(*this)[15]*mat[7];
	//third column
    prd[8] =
        (*this)[0]*mat[8]+
        (*this)[4]*mat[9]+
        (*this)[8]*mat[10]+
		(*this)[12]*mat[11];
    prd[9] =
        (*this)[1]*mat[8]+
        (*this)[5]*mat[9]+
        (*this)[9]*mat[10]+
		(*this)[13]*mat[11];
    prd[10] =
        (*this)[2]*mat[8]+
        (*this)[6]*mat[9]+
        (*this)[10]*mat[10]+
		(*this)[14]*mat[11];
	prd[11] =
        (*this)[3]*mat[8]+
        (*this)[7]*mat[9]+
        (*this)[11]*mat[10]+
		(*this)[15]*mat[11];
	//fourth column
	prd[12] =
        (*this)[0]*mat[12]+
        (*this)[4]*mat[13]+
        (*this)[8]*mat[14]+
		(*this)[12]*mat[15];
    prd[13] =
        (*this)[1]*mat[12]+
        (*this)[5]*mat[13]+
        (*this)[9]*mat[14]+
		(*this)[13]*mat[15];
    prd[14] =
        (*this)[2]*mat[12]+
        (*this)[6]*mat[13]+
        (*this)[10]*mat[14]+
		(*this)[14]*mat[15];
	prd[15] =
        (*this)[3]*mat[12]+
        (*this)[7]*mat[13]+
        (*this)[11]*mat[14]+
		(*this)[15]*mat[15];
    return prd;
}//inline AWMatrix4 AWMatrix4::operator* (const AWMatrix4& mat) const



inline AWMatrix4& 
AWMatrix4::invertAffine()
{
   m_mat[3] = m_mat[7] = m_mat[11] = m_mat[12] = m_mat[13] = m_mat[14] = 0.0f;

   swap(m_mat[1], m_mat[4]);
   swap(m_mat[2], m_mat[8]);
   swap(m_mat[6], m_mat[9]);

   return *this;
}//inline AWMatrix4& AWMatrix4::invertAffine()



inline void 
AWMatrix4::frustum(float l, float r, float b, float t, float zNear, float zFar)
{
   identity();
   m_mat[0]  = (2.0f*zNear)/(r - l);
   m_mat[8]  = (r+l)/(r- l);

   m_mat[5]  = (2.0f*zNear)/(t-b);
   m_mat[9]  = (t+b)/(t-b);

   m_mat[10] = -(zFar+zNear)/(zFar-zNear);
   m_mat[14] = -2.0f*zFar*zNear/(zFar-zNear);

   m_mat[11] = -1;
   m_mat[15] = 0;
}//inline void AWMatrix4::frustum(float l, float r, float b, float t, float zNear, float zFar)



inline void       
AWMatrix4::frustumInverse(float l, float r, float b, float t, 
                            float zNear, float zFar)
{
   identity();

   m_mat[0]  = (r - l) / (2.0f * zNear);
   m_mat[12] = (r + l) / (2.0f * zNear);

   m_mat[5]  = (t - b) / (2.0f * zNear);
   m_mat[13] = (t + b) / (2.0f * zNear);

   m_mat[10] = 0;
   m_mat[14] = -1;

   m_mat[11] = -(zFar - zNear) / (2.0f * zFar * zNear);
   m_mat[15] = (zFar + zNear) / (2.0f * zFar * zNear);
}//inline void AWMatrix4::frustumInverse(...)



inline void 
AWMatrix4::perspective(float fovy, float aspect, float zNear, float zFar)
{
   double tangent = tan(radToDeg*(fovy/2.0f));
   float y = (float)(tangent * (double)zNear);
   float x = aspect * y;
   frustum(-x, x, -y, y, zNear, zFar);
}//inline void AWMatrix4::perspective(float fovy, float aspect, float zNear, float zFar)



inline void 
AWMatrix4::perspectiveInverse(float fovy, float aspect, float zNear, float zFar)
{
   double tangent = tan(radToDeg*(fovy/2.0f));
   float y = (float)(tangent * (double)zNear);
   float x = aspect * y;
   frustumInverse(-x, x, -y, y, zNear, zFar);
}//inline void AWMatrix4::perspectiveInverse(float fovy, float aspect, float zNear, float zFar)



inline int
AWMatrix4Stack::awPush()
{
	int retVal = 0;
	m_stackIndex++;
	if (m_stackIndex >= STACK_DEPTH)
	{
		m_stackIndex--;
	}
	else
	{
		memcpy(m_stack[m_stackIndex].m_mat, m_mat, sizeof(m_mat));
	}
	return retVal;
}//inline int AWMatrix4Stack::awPush()



inline int
AWMatrix4Stack::awPop()
{
	int retVal = 0;
	if (m_stackIndex != -1)
	{
		memcpy(m_mat, m_stack[m_stackIndex].m_mat, sizeof(m_mat));
		m_stackIndex--;
		retVal = 1;
	}
	return retVal;
}//inline int AWMatrix4Stack::awPop()


#endif //sentinel __AWMATRIX4_H