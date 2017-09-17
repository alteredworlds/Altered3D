#ifndef _AW_CUBE_ENV_TEXTURE_H
#define _AW_CUBE_ENV_TEXTURE_H


#include "AWToolsCommon.h"
#include "AWPtrList.h"
#include "AWTexture.h"
#include "AWPoint.h"
#include "AWCString.h"


class AWExport AWCubeEnvTexture : public AWTexture
{
public:
   enum FaceEnum {POS_X=0, NEG_X=1, POS_Y=2, 
                  NEG_Y=3, POS_Z=4, NEG_Z=5};

   static const int   NUM_FACES;
   static const float m_mapRots[6][4];

   AWDECLARE_VERSIONABLE_PERSISTABLE(AWCubeEnvTexture);

   AWCubeEnvTexture(const AWCString& name = "", const AWCString& fileName = "");

   AWCubeEnvTexture(const AWCubeEnvTexture& other) {*this = other;}

   ~AWCubeEnvTexture();

   void                 destroyFaces();

   AWBoolean            getAuto()               {return m_auto;}
   void                 setAuto(AWBoolean v)    {m_auto=v;}
   AWBoolean            needsUpdate(double t);
   void                 setLastUpdate(double t);

   float                getNearClip()           {return m_near;}
   float                getFarClip()            {return m_far;}
   float                getStep()               {return m_step;}

   void                 setNearClip(float v)    {m_near=v;}
   void                 setFarClip(float v)     {m_far=v;}
   void                 setStep(float v)        {m_step=v;}

//HELPER TO INIT WITH/ GENERATE CUBE NORMALIZATION MAP
   int                  generateNormalizeVectorCube(int sizePixels);
   void                 getCubeVector(int i, int size, int x, int y, AWPoint& vec);

   //int                  operator==(const AWCubeEnvTexture& o);

//HELPER TO TRANSFORM TO SPHERE MAP FOR MACHINES WITHOUT CUBE MAPS
   void                 transformToSphereMap(int size=-1);

//TEXTURE OBJECT ACCESS METHODS
   int                  addFace(AWTexture* tex, FaceEnum f);
   AWTexture*           getFace(FaceEnum f);
   AWrgba               indexCube(const AWPoint& vec);
   int                  getIndexFromFaceEnum(FaceEnum f);


//OVERRIDES OF BASE CLASS
	void 			         reSize(int xsize, int ysize);
//AWPERSISTABLE VIRTUALS
	virtual void         restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;

	virtual AWspace      binaryStoreSize() const;

//the set of pointers to AWTextures that make up the cube faces
//AWList includes a flag m_ownsData which indicates whether
//destructors should be called
   AWPtrList<AWTexture*> m_texFaces;
//MEMBER DATA
protected:
   AWBoolean            m_auto;  //is this texture generated per frame?
   //following only used if generated per frame...
   float                m_near;  //near clipping plane
   float                m_far;   //far clipping plane
   // -1  no more rendering required
   // 0   render only initial frame
   // > 0 gives interval in seconds before each render
   float                m_step;
   double               m_lastUpdate;
};//class AWExport AWCubeEnvTexture : public AWTexture



inline AWBoolean            
AWCubeEnvTexture::needsUpdate(double t)
{
   return (-1==m_step) ? FALSE : (0.0 == m_lastUpdate) ? TRUE : (t-m_lastUpdate)>m_step;
}//inline AWBoolean AWCubeEnvTexture::needsUpdate(float t)



inline void
AWCubeEnvTexture::setLastUpdate(double t)
{
   m_lastUpdate = t;
   if (m_step==0.0f)
   {  //we rendered our initial frame - turn off further rendering
      m_step = -1;
   }//if (m_step==0.0f)
}//inline void AWCubeEnvTexture::setLastUpdate(float t)


#endif //sentinel _AW_CUBE_ENV_TEXTURE_H