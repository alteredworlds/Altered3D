#ifndef __AWFACE_H
#define __AWFACE_H

#include "AWToolsCommon.h"
#include "AWPoint.h"



class AWExport AWFace : public AWPersistable
{
public:
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWFace);


	AWFace(int V0=-1, int V1=-1, int V2=-1, 
		    int TV0=-1, int TV1=-1, int TV2=-1,
			int N0=-1, int N1=-1, int N2=-1,
			int cv0=-1, int cv1=-1, int cv2=-1)  
            {v[0]=V0; v[1]=V1; v[2]=V2; 
				Tv[0]=TV0; Tv[1]= TV1; Tv[2]=TV2; 
				n[0]=N0; n[1]=N1; n[2]=N2;
				cV[0]=cv0; cV[1]=cv1; cV[2]=cv2;
				m_matID = -1;}

	AWFace(const AWFace& other) {*this=other;}

	AWFace& operator=(const AWFace& other)
			{v[0]=other.v[0]; v[1]=other.v[1]; v[2]=other.v[2];	
			 Tv[0] = other.Tv[0]; Tv[1] = other.Tv[1], Tv[2] = other.Tv[2]; 
			 n[0] = other.n[0]; n[1] = other.n[1]; n[2] = other.n[2];
			 cV[0] = other.cV[0]; cV[1] = other.cV[1]; cV[2] = other.cV[2];
			 m_matID = other.m_matID;return *this;}

	static float		distanceToPoint(const AWPoint& p, 
						const AWPoint& V0, const AWPoint& V1, const AWPoint& V2,
						double& s, double& t);

	static AWBoolean	collide(AWPoint& center, float radius, 
						const AWPoint& V0,
						const AWPoint& V1,
						const AWPoint& V2);
	static AWBoolean	sphereIntersectsFace(AWPoint& center, float radius, 
						const AWPoint& V0,
						const AWPoint& V1,
						const AWPoint& V2);
	
   int   v[3];          //vertex indeces
   int   Tv[3];         //texture vertex indecex
   int   n[3];           //normal vertex indeces	
   int   cV[3];         //colour (per) vertex indeces

	int   m_matID;         //material ID (for per-face texture mapping)

	//AWPersistable virtuals
	virtual void      restoreGuts(AWFile& file);
   virtual void      saveGuts(AWFile& file) const;
	virtual AWspace   binaryStoreSize() const;
};//class AWFace : public AWPersistable

#endif //sentinel __AWFACE_H