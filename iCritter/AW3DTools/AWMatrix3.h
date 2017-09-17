#ifndef __AWMATRIX3_H
#define __AWMATRIX3_H

#include "AWMatrix4.h"

//represents and OpenGL / AWEngine style 
//COLUMN-MAJOR 4x4 Transformation Matrix
//	[m0		m3		m6]
//	[m1		m4		m7]
//	[m2		m5		m8]
class AWExport AWMatrix3
{
public:
	AWMatrix3()	    {;}
	
	void		    identity();//set to identity matrix
    AWMatrix3&      transpose();
	
	void			setTX(const AWPoint& p);
	void			setTY(const AWPoint& p);
	void			setTZ(const AWPoint& p);
	void			normalizeBasis();
	
	
	const float		operator[] (int index)const		{return (const float)m_mat[index];}
	float&			operator[] (int index)				{return m_mat[index];}
	float&			operator() (int row, int col)		{return m_mat[col*3 + row];}
	const float		operator() (int row, int col)const {return (const float)m_mat[col*3 + row];}
	
	AWMatrix3		operator* (const AWMatrix3& mat) const;
	AWPoint			operator* (const AWPoint& vector)const;
	AWMatrix3&		operator= (const AWMatrix3& other);
    AWMatrix3&		operator= (const AWMatrix4& other);
	AWMatrix3		operator+ (const AWMatrix3& mat) const;
	AWMatrix3		operator- (const AWMatrix3& mat) const;
	
	friend AWMatrix3 operator*(float f, const AWMatrix3& obj);
	friend AWFile&	operator>>(AWFile& file, AWMatrix3& obj);
	friend AWFile&	operator<<(AWFile& file, AWMatrix3& obj);
	
	void			write(AWFile& file) const;
	void			read(AWFile& file);
	
	float			m_mat[9];
    
    //helper function
protected:
	void			swap(float& a, float& b) {float temp=a;a=b;b=temp;}
};//class AW3DExport AWMatrix3


//initialise the matrix to identity
inline void
AWMatrix3::identity()
{
	m_mat[0] = m_mat[4] = m_mat[8] = 1.0f;
	m_mat[1] = m_mat[2] = m_mat[3] = 0.0f;
	m_mat[5] = m_mat[6] = m_mat[7] = 0.0f;
}//inline void AWMatrix3::identity()



inline AWMatrix3&	 
AWMatrix3::operator= (const AWMatrix3& other)
{  //copy via assignment
	memcpy(m_mat, other.m_mat, sizeof(m_mat));
	return *this;
}//AWMatrix3& AWMatrix3::operator= (const AWMatrix3& other)


inline AWMatrix3&
AWMatrix3::operator= (const AWMatrix4& other)
{  //copy via assignment
	m_mat[0] = other[0];
    m_mat[1] = other[1];
    m_mat[2] = other[2];
    m_mat[3] = other[4];
    m_mat[4] = other[5];
    m_mat[5] = other[6];
    m_mat[6] = other[8];
    m_mat[7] = other[9];
    m_mat[8] = other[10];
	return *this;
}//AWMatrix3& AWMatrix3::operator= (const AWMatrix4& other)


inline AWMatrix3&
AWMatrix3::transpose()
{
    swap(m_mat[1], m_mat[3]);
    swap(m_mat[2], m_mat[6]);
    swap(m_mat[5], m_mat[7]);
    
    return *this;
}//inline AWMatrix3& AWMatrix3::transpose()


//operate with this matrix on the supplied vector, returning the result
inline AWPoint		 
AWMatrix3::operator* (const AWPoint& vector)const
{
	return AWPoint(m_mat[0]*vector.x + m_mat[3]*vector.y + m_mat[6]*vector.z,
				      m_mat[1]*vector.x + m_mat[4]*vector.y + m_mat[7]*vector.z,
				      m_mat[2]*vector.x + m_mat[5]*vector.y + m_mat[8]*vector.z);
}



inline AWMatrix3 operator*(float f, const AWMatrix3& obj)
{
	AWMatrix3 prod(obj);
	//first column
	prod[0] *= f;
	prod[1] *= f;
	prod[2] *= f;
	//second column
	prod[3] *= f;
	prod[4] *= f;
	prod[5] *= f;
	//third column
	prod[6] *= f;
	prod[7] *= f;
	prod[8] *= f;
	return prod;
}//inline AWMatrix3 operator*(float f, const AWMatrix3& obj)



inline void           
AWMatrix3::setTX(const AWPoint& p)
{
   m_mat[0]=p.x;
   m_mat[3]=p.y;
   m_mat[6]=p.z;
}//inline void AWMatrix3::setTX(const AWPoint& p)


inline void           
AWMatrix3::setTY(const AWPoint& p)
{
   m_mat[1]=p.x;
   m_mat[4]=p.y;
   m_mat[7]=p.z;
}//inline void AWMatrix3::setTY(const AWPoint& p)


inline void           
AWMatrix3::setTZ(const AWPoint& p)
{
   m_mat[2]=p.x;
   m_mat[5]=p.y;
   m_mat[8]=p.z;
}//inline void AWMatrix3::setTZ(const AWPoint& p)



inline void           
AWMatrix3::normalizeBasis()
{
   AWPoint tmp;
   tmp.x=m_mat[0]; tmp.y=m_mat[1]; tmp.z = m_mat[2];
   tmp.normalize();
   m_mat[0]=tmp.x; m_mat[1]=tmp.y; m_mat[2]=tmp.z;

   tmp.x=m_mat[3]; tmp.y=m_mat[4]; tmp.z = m_mat[5];
   tmp.normalize();
   m_mat[3]=tmp.x; m_mat[4]=tmp.y; m_mat[5]=tmp.z;

   tmp.x=m_mat[6]; tmp.y=m_mat[7]; tmp.z = m_mat[8];
   tmp.normalize();
   m_mat[6]=tmp.x; m_mat[7]=tmp.y; m_mat[8]=tmp.z;
}//inline void AWMatrix3::normalizeBasis()



inline AWMatrix3 
AWMatrix3::operator* (const AWMatrix3& mat) const
{
    AWMatrix3 prd;
	//first col
    prd[0] = 
		(*this)[0]*mat[0]+
      (*this)[3]*mat[1]+
		(*this)[6]*mat[2];
    prd[1] =
        (*this)[1]*mat[0]+
        (*this)[4]*mat[1]+
        (*this)[7]*mat[2];
    prd[2] =
        (*this)[2]*mat[0]+
        (*this)[5]*mat[1]+
        (*this)[8]*mat[2];
	//second column
    prd[3] =
        (*this)[0]*mat[3]+
        (*this)[3]*mat[4]+
        (*this)[6]*mat[5];
    prd[4] =
        (*this)[1]*mat[3]+
        (*this)[4]*mat[4]+
        (*this)[7]*mat[5];
    prd[5] =
        (*this)[2]*mat[3]+
        (*this)[5]*mat[4]+
        (*this)[8]*mat[5];
	//third column
    prd[6] =
        (*this)[0]*mat[6]+
        (*this)[3]*mat[7]+
        (*this)[6]*mat[8];
    prd[7] =
        (*this)[1]*mat[6]+
        (*this)[4]*mat[7]+
        (*this)[7]*mat[8];
    prd[8] =
        (*this)[2]*mat[6]+
        (*this)[5]*mat[7]+
        (*this)[8]*mat[8];
    return prd;
}//inline AWMatrix3 AWMatrix3::operator* (const AWMatrix3& mat) const



inline AWMatrix3 
AWMatrix3::operator-(const AWMatrix3& mat) const
{  //subtract columns
   AWMatrix3 diff;
   //first col
   diff[0] = (*this)[0]-mat[0];
   diff[1] = (*this)[1]-mat[1];
   diff[2] = (*this)[2]-mat[2];
   //second column
   diff[3] = (*this)[3]-mat[3];
   diff[4] = (*this)[4]-mat[4];
   diff[5] = (*this)[5]-mat[5];
   //third column
   diff[6] = (*this)[6]-mat[6];
   diff[7] = (*this)[7]-mat[7];
   diff[8] = (*this)[8]-mat[8];
   return diff;
}//inline AWMatrix3 AWMatrix3::operator- (const AWMatrix3& mat) const



inline AWMatrix3 
AWMatrix3::operator+ (const AWMatrix3& mat) const
{  //add columns
   AWMatrix3 sum;
   //first col
   sum[0] = (*this)[0]+mat[0];
   sum[1] = (*this)[1]+mat[1];
   sum[2] = (*this)[2]+mat[2];
   //second column
   sum[3] = (*this)[3]+mat[3];
   sum[4] = (*this)[4]+mat[4];
   sum[5] = (*this)[5]+mat[5];
   //third column
   sum[6] = (*this)[6]+mat[6];
   sum[7] = (*this)[7]+mat[7];
   sum[8] = (*this)[8]+mat[8];
   return sum;
}//inline AWMatrix3 AWMatrix3::operator+ (const AWMatrix3& mat) const



inline void	
AWMatrix3::write(AWFile& file) const
{
	file.Write(m_mat, sizeof(m_mat));
}//inline void	AWMatrix3::write(AWFile& file) const



inline void	
AWMatrix3::read(AWFile& file)
{
	file.Read(m_mat, sizeof(m_mat));
}//inline void	AWMatrix3::read(AWFile& file)


inline AWFile& operator<<(AWFile& file, AWMatrix3& it)
{
  it.write(file);
  return file;
}//AWFile& operator<<(AWFile& file, AWMatrix3& it)


inline AWFile& operator>>(AWFile& file, AWMatrix3& it)
{
   it.read(file);
   return file;
}//AWFile& operator>>(AWFile& file, AWMatrix3& it)


#endif //sentinel __AWMATRIX3_H