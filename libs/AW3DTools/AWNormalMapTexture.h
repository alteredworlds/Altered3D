#ifndef _AW_NORMALMAP_TEXTURE_H
#define _AW_NORMALMAP_TEXTURE_H

#include "AWToolsCommon.h"
#include "AWList.h"
#include "AWTexture.h"

// 'Normal' is helper class - used in internal methods, not exported
// based on NVIDIA sample code: encodes a normal as 8-bit unsigned BGRA vector
typedef struct 
{ // Normalized tangent space peturbed surface normal.  The
  //   [0,1] range of (nx,ny,nz) gets expanded to the [-1,1]
  //   range in the combiners.  The (nx,ny,nz) is always a
  //   normalized vector. 
  GLubyte nz, ny, nx;

  // A scaling factor for the normal.  Mipmap level 0 has a constant
  //   magnitude of 1.0, but downsampled mipmap levels keep track of
  //   the unnormalized vector sum length.  For diffuse per-pixel
  //   lighting, it is preferable to make N' be the _unnormalized_
  //   vector, but for specular lighting to work reasonably, the
  //   normal vector should be normalized.  In the diffuse case, we
  //   can multiply by the "mag" to get the possibly shortened
  //   unnormalized length. 
  GLubyte mag;

  // Why does "mag" make sense for diffuse lighting?

  //   Because sum(L dot Ni)/n == (L dot sum(Ni))/n 
  
  //   Think about a bumpy diffuse surface in the distance.  It should
  //   have a duller illumination than a flat diffuse surface in the
  //   distance. 

  // On NVIDIA GPUs, the RGBA8 internal format is just as memory
  //   efficient as the RGB8 internal texture format so keeping
  //   "mag" around is just as cheap as not having it. 

} Normal;


class AWExport AWNormalMapTexture : public AWTexture
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWNormalMapTexture);

   AWNormalMapTexture(const AWCString& name = "", const AWCString& fileName = "");

   AWNormalMapTexture(const AWNormalMapTexture& other) {*this == other;}

   ~AWNormalMapTexture();

//int                  operator==(const AWNormalMapTexture& o);
//builds NVIDIA-style Normal Map from current data - 
//'source' (artist's) form will be HEIGHT FIELD of type GL_LUMINANCE
   virtual int          init();
   float                getSize();
   void                 setSize(float v)  {m_size=v;}

   void                 convertHeightFieldToNormalMap(float scale);

//OVERRIDES OF BASE CLASS
	void 			         reSize(int xsize, int ysize);
//AWPERSISTABLE VIRTUALS
	AWBoolean		      isEqual(const AWPersistable* o) const;
	int				      compareTo(const AWPersistable* o) const;
	virtual void         restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;

	virtual AWspace      binaryStoreSize() const;

protected:
   float                m_size;
};//class AWExport AWNormalMapTexture : public AWTexture



#endif //sentinel _AW_NORMALMAP_TEXTURE_H