#include "AWFace.h"

#define VER_AWFACE_1	100
#define VER_AWFACE_2	101


AWDEFINE_VERSIONABLE_PERSISTABLE(AWFace, CLSID_AWFACE, VER_AWFACE_2);


float
AWFace::distanceToPoint(const AWPoint& p, 
						const AWPoint& v0, const AWPoint& v1, const AWPoint& v2,
						double& s, double& t)
{
	AWPoint diff(v0.x-p.x, v0.y-p.y, v0.z-p.z);

	double A = dot(v1,v1);
	double B = dot(v1,v2);
	double C = dot(v2,v2);
	double D = dot(v1,diff);
	double E = dot(v2,diff);
	double F = dot(diff,diff);
	double det = A*C-B*B;  // assert:  det != 0 for triangles

	s = (B*E-C*D)/det;
	t = (B*D-A*E)/det;

	if ( s+t <= 1.0 )
	{
		if ( s < 0.0 )
		{
			if ( t < 0.0 )  // region 4
			{
				if ( D < 0 )
				{
					t = 0.0;
					s = -D/A;
					if ( s > 1.0 ) s = 1.0;
				}//if ( D < 0 )
				else if ( E < 0 )
				{
					s = 0.0;
					t = -E/C;
					if ( t > 1.0 ) t = 1.0;
				}//else if ( E < 0 )
				else
				{
					s = 0.0;
					t = 0.0;
				}//else
			}//if ( t < 0.0 )
			else  // region 3
			{
				s = 0.0;
				t = -E/C;
				if ( t < 0.0 ) t = 0.0; else if ( t > 1.0 ) t = 1.0;
			}
		}//if ( s < 0.0 )
		else if ( t < 0.0 )  // region 5
		{
			t = 0.0;
			s = -D/A;
			if ( s < 0.0 ) s = 0.0; else if ( s > 1.0 ) s = 1.0;
		}//else if ( t < 0.0 )
		else  // region 0
		{
			// minimum at interior point
		}
	}//if ( s+t <= 1.0 )
	else
	{
		if ( s < 0.0 )  // region 2
		{
			if ( B-C+D-E < 0.0 )
			{
				s = -(B-C+D-E)/(A-2*B+C);
				if ( s < 0.0 ) s = 0.0; else if ( s > 1.0 ) s = 1.0;
				t = 1.0-s;
			}
			else if ( C+E > 0.0 )
			{
				s = 0.0;
				t = -E/C;
				if ( t < 0.0 ) t = 0.0; else if ( t > 1.0 ) t = 1.0;
			}
			else
			{
				s = 0.0;
				t = 1.0;
			}
		}//if ( s < 0.0 )
		else if ( t < 0.0 )  // region 6
		{
			if ( A-B+D-E > 0.0 )
			{
				t = (A-B+D-E)/(A-2*B+C);
				if ( t < 0.0 ) t = 0.0; else if ( t > 1.0 ) t = 1.0;
				s = 1.0-t;
			}
			else if ( A+D > 0.0 )
			{
				t = 0.0;
				s = -D/A;
				if ( s < 0.0 ) s = 0.0; else if ( s > 1.0 ) s = 1.0;
			}
			else
			{
				s = 1.0;
				t = 0.0;
			}
		}//else if ( t < 0.0 )
		else  // region 1
		{
			s = -(B-C+D-E)/(A-2*B+C);
			if ( s < 0.0 ) s = 0.0; else if ( s > 1.0 ) s = 1.0;
			t = 1.0-s;
		}
	}//else
	return (float)sqrt(fabs(s*(A*s+B*t+2*D)+t*(B*s+C*t+2*E)+F));
}//float AWFace::distanceToPoint(const AWPoint& p, 
//						const AWPoint& v[0], const AWPoint& v[1], const AWPoint& v[2],
//						double& s, double& t)


AWBoolean
AWFace::sphereIntersectsFace(AWPoint& center, float radius, 
						const AWPoint& v0,
						const AWPoint& v1,
						const AWPoint& v2)
{
	double s,t;
	float dist = distanceToPoint(center, v0, v1, v2, s, t);
	return dist <= radius;
}//AWBoolean AWFace::sphereIntersectsFace(...)



AWBoolean 	 
AWFace::collide(AWPoint& center, float radius, 
						const AWPoint& v0,
						const AWPoint& v1,
						const AWPoint& v2)
{
	AWBoolean	retVal = FALSE;
	//check to see if a collision has occurred between sphere 
   //and this face
   if (sphereIntersectsFace(center, radius, v0, v1, v2))
   {  //sphere intersects with this face
		//at this point try initial go at COLLISION AVOIDANCE		
		retVal     = TRUE;
	}//if (sphereIntersectsFace(center, radius, v[0], v[1], v[2]))	
	return retVal;
}//AWBoolean AWFace::collide(const AWPoint& center, float radius)

//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWFace
//
//////////////////////////////////////////////////////////
void	
AWFace::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if ((VER_AWFACE_2 == version) || (VER_AWFACE_1 == version))
	{
		AWPersistable::restoreGuts(file);
		file >> v[0] >> v[1] >> v[2];
		file >> Tv[0] >> Tv[1] >> Tv[2];
		file >> n[0] >> n[1] >> n[2];
		file >> cV[0] >> cV[1] >> cV[2];
		if (VER_AWFACE_2 == version)
		{
			file >> m_matID;
		}
	}
}//void AWFace::restoreGuts(AWFile& file)



void        
AWFace::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << v[0] << v[1] << v[2];
	file << Tv[0] << Tv[1] << Tv[2];
	file << n[0] << n[1] << n[2];
	file << cV[0] << cV[1] << cV[2];
	file << m_matID;
}//void AWFace::saveGuts(AWFile& file) const




AWspace     
AWFace::binaryStoreSize() const
{
	return sizeof uVersion_ +
		AWPersistable::binaryStoreSize()
		+ sizeof(v) 
		+ sizeof(Tv)
		+ sizeof(n)
		+ sizeof(cV)
		+ sizeof(m_matID);
}//AWspace  AWFace::binaryStoreSize() const
