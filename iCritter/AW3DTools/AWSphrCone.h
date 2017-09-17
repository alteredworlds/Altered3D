#ifndef __AW_SPHRCONE_H
#define __AW_SPHRCONE_H

#include "AWToolsCommon.h"
#include "AWPoint.h"



class AWExport AWSolidCone
{
    // Acute cone is dot(m_A,X-m_V) = |X-m_V| cos(angle) where 0 < angle < pi/2
    // and |m_A| = 1.  Cone interior is defined by the inequality
    // dot(m_A,X-m_V) >= |X-m_V| cos(angle).  Since cos(angle) > 0, we can
    // avoid taking square roots.  Solid cone is
    //
    //     dot(m_A,X-m_V)^2 >= dot(X-m_V,X-m_V) cos(angle)^2
public:
   AWPoint  m_V;
   AWPoint  m_A;
   float	   m_cs, m_cs2, m_sn;  // cos(angle), cos(angle)^2, sin(angle)
   float	   m_farClip;

   void	   setAngle(float theta);   //supply angle in DEGREES
   int		intersectSphere(const AWPoint& center, float radius)const;
};//class AWExport AWSolidCone


inline void	
AWSolidCone::setAngle(float theta)   //supply angle in DEGREES
{
	double rads=(double)theta*degToRad; 
	m_cs= (float)cos(rads); 
	m_cs2=m_cs*m_cs; 
	m_sn=(float)sin(rads);
}//inline void AWSolidCone::setAngle(float theta)



inline int 
AWSolidCone::intersectSphere(const AWPoint& center, float radius)const
{
    // test if cone vertex is in sphere
    AWPoint diff(center - m_V);
    double r2 = radius*radius;
    double len2 = diff.sqrMagnitude();
    double len2mr2 = len2-r2;
    if ( len2mr2 <= 0.0 )
        return 1;

    // test if sphere center is in cone
    double dotP = dot(m_A , diff);
    double dot2 = dotP*dotP;
    if ((dot2 >= len2*m_cs2) && (dotP > 0))
        // sphere center is inside cone, so sphere and cone intersect
        return 1;

    // Sphere center is outside cone.  Problem now reduces to looking for
    // an intersection between circle and ray in the plane containing
    // cone vertex and spanned by cone axis and vector from vertex to
    // sphere center.

    // Ray is t*D+m_V (t >= 0) where |D| = 1 and dot(m_A,D) = cos(angle).
    // Also, D = e*m_A+f*(C-m_V).  Plugging ray equation into sphere equation
    // yields R^2 = |t*D+m_V-C|^2, so the quadratic for intersections is
    // t^2 - 2*dot(D,C-m_V)*t + |C-m_V|^2 - R^2 = 0.  An intersection occurs
    // if and only if the discriminant is nonnegative.  This test becomes
    //
    //     dot(D,C-m_V)^2 >= dot(C-m_V,C-m_V) - R^2
    //
    // Note that if the right-hand side is nonpositive, then the inequality
    // is true (the sphere contains m_V).  I have already ruled this out in
    // the first block of code in this function.

    double ulen = sqrt(fabs(len2-dot2));  // assert len2 > dot2
    double test = m_cs*dotP+m_sn*ulen;
    double discr = test*test-len2mr2;

    return ( discr >= 0.0 && test >= 0.0 );
}//int AWSolidCone::intersectSphere(const AWSphere& sphere)


#endif //sentinel __AW_SPHRCONE_H
