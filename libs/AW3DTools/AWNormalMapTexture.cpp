#include "AWNormalMapTexture.h"
#include "AWPoint.h"

#define VER_AWNORMAL_TEXMAP_1	100
#define VER_AWNORMAL_TEXMAP_2 102

 
static const float oneOver127 = float(1.0/127.0);
static const float oneOver255 = float(1.0/255.0);


AWDEFINE_VERSIONABLE_PERSISTABLE(AWNormalMapTexture, CLSID_AWNORMALMAPTEXTURE, VER_AWNORMAL_TEXMAP_2);



AWNormalMapTexture::AWNormalMapTexture(const AWCString& name, const AWCString& fileName) 
            : AWTexture(name, fileName)
{
   m_size = 6.0f;
}



AWNormalMapTexture::~AWNormalMapTexture()
{
}//AWNormalMapTexture::~AWNormalMapTexture()



void 	
AWNormalMapTexture::reSize(int w, int h)
{  // Given a normal map, create a downsampled version of the normal map
   // at half the width and height.  Use a 2x2 box filter to create each
   // downsample.  gluBuild2DMipmaps is not suitable because each downsampled
   // texel must also be renormalized.
   if ((GL_BGRA_EXT != m_format) && (GL_RGBA != m_format))
   {  //this can't be a normal map yet - use standard resize
      AWTexture::reSize(w, h);
      return;
   }//if ((GL_BGRA_EXT != m_format) && (GL_RGBA != m_format))
   float x, y, z, l, invl;
   float mag00, mag01, mag10, mag11;
   int   i, j, ii, jj;

   // Allocate space for the downsampled normal map level.
   Normal* nmap = new Normal[w*h];
   //don't you love C type casting? Unsafe, but very powerful... :)
   Normal* old  = (Normal*)m_pImage;

   for (i=0; i<m_height; i+=2) 
   {
      for (j=0; j<m_width; j+=2) 
      {  // The "%m_width" and "%m_height" modulo arithmetic makes sure that
         //   Nx1 and 1xN normal map levels are handled correctly. 
         //
         // Fetch the magnitude of the four vectors to be downsampled.
         mag00 = oneOver255 * old[ (i  )    *m_width +  (j  )    ].mag;
         mag01 = oneOver255 * old[ (i  )    *m_width + ((j+1)%m_height)].mag;
         mag10 = oneOver255 * old[((i+1)%m_width)*m_width +  (j  )    ].mag;
         mag11 = oneOver255 * old[((i+1)%m_width)*m_width + ((j+1)%m_height)].mag;

         // Sum 2x2 footprint of red component scaled back to [-1,1] floating point range.
         x =  mag00 * (oneOver127 * old[ (i  )    *m_width +  (j  )    ].nx - 1.0f);
         x += mag01 * (oneOver127 * old[ (i  )    *m_width + ((j+1)%m_height)].nx - 1.0f);
         x += mag10 * (oneOver127 * old[((i+1)%m_width)*m_width +  (j  )    ].nx - 1.0f);
         x += mag11 * (oneOver127 * old[((i+1)%m_width)*m_width + ((j+1)%m_height)].nx - 1.0f);

         // Sum 2x2 footprint of green component scaled back to [-1,1] floating point range.
         y =  mag00 * (oneOver127 * old[ (i  )    *m_width +  (j  )    ].ny - 1.0f);
         y += mag01 * (oneOver127 * old[ (i  )    *m_width + ((j+1)%m_height)].ny - 1.0f);
         y += mag10 * (oneOver127 * old[((i+1)%m_width)*m_width +  (j  )    ].ny - 1.0f);
         y += mag11 * (oneOver127 * old[((i+1)%m_width)*m_width + ((j+1)%m_height)].ny - 1.0f);

         // Sum 2x2 footprint of blue component scaled back to [-1,1] floating point range.
         z =  mag00 * (oneOver127 * old[ (i  )    *m_width +  (j  )    ].nz - 1.0f);
         z += mag01 * (oneOver127 * old[ (i  )    *m_width + ((j+1)%m_height)].nz - 1.0f);
         z += mag10 * (oneOver127 * old[((i+1)%m_width)*m_width +  (j  )    ].nz - 1.0f);
         z += mag11 * (oneOver127 * old[((i+1)%m_width)*m_width + ((j+1)%m_height)].nz - 1.0f);

         // Compute length of the (x,y,z) vector. 
         l = (float)sqrt(x*x + y*y + z*z);
         if (l == 0.0f) 
         {  //Ugh, a zero length vector.  Avoid division by zero and just
            //   use the unpeturbed normal (0,0,1).  
            x = 0.0f;
            y = 0.0f;
            z = 1.0f;
         }//if (l == 0.0)
         else 
         {  // Normalize the vector to unit length.  
            invl = 1.0f/l;
            x = x*invl;
            y = y*invl;
            z = z*invl;
         }//else

         ii = i >> 1;
         jj = j >> 1;

         // Pack the normalized vector into an RGB unsigned byte vector
         //   in the downsampled image. 
         nmap[ii*w+jj].nx = (unsigned char)(128.0f + 127.0f*x);
         nmap[ii*w+jj].ny = (unsigned char)(128.0f + 127.0f*y);
         nmap[ii*w+jj].nz = (unsigned char)(128.0f + 127.0f*z);

         // Store the magnitude of the average vector in the alpha
         //   component so we keep track of the magntiude. 
         l = l/4.0f;
         if (l > 1.0f) 
         {
           nmap[ii*w+jj].mag = (unsigned char)(255);
         }
         else
         {
           nmap[ii*w+jj].mag = (unsigned char)(255*l);
         }
      }//for (j=0; j<m_width; j+=2) 
   }//for (i=0; i<m_height; i+=2) 
   delete [] m_pImage;
   m_pImage = (unsigned char*)nmap;
   m_width  = w;
   m_height = h;
}//void AWNormalMapTexture::reSize(int xsize, int ysize)



// NORMAL MAP TEXTURE CONSTRUCTION - based on NVIDIA sample code
void
AWNormalMapTexture::convertHeightFieldToNormalMap(float scale)
{  //Convert a height field image into a normal map.  This involves
   //differencing each texel with its right and upper neighboor, then
   //normalizing the cross product of the two difference vectors.
   if (!m_width || !m_height || (GL_LUMINANCE != m_format)) return;

   int     i, j;
   float   c, cx, cy;
   AWPoint n;

   Normal* nmap = new Normal[m_width*m_height];
   if (!nmap) return;
   for (i=0; i<m_height; i++) 
   {
      for (j=0; j<m_width; j++) 
      { // Expand [0,255] texel values to the [0,1] range. 
         c = m_pImage[i*m_width + j] * oneOver255;
         // Expand the texel to its right. 
         cx = m_pImage[i*m_width + (j+1)%m_width] * oneOver255;
         // Expand the texel one up.
         cy = m_pImage[((i+1)%m_height)*m_width + j] * oneOver255;
         
         n.x = scale  * (c - cy);
         n.y = -scale * (c - cx);
         n.z = 1.0f;
         n.normalize();

         // Repack the normalized vector into an RGB unsigned byte
         //   vector in the normal map image. 
         nmap[i*m_width+j].nx = (unsigned char)(128.0f + 127.0f*n.x);
         nmap[i*m_width+j].ny = (unsigned char)(128.0f + 127.0f*n.y);
         nmap[i*m_width+j].nz = (unsigned char)(128.0f + 127.0f*n.z);

         // The highest resolution mipmap level always has a
         //   unit length magnitude. 
         nmap[i*m_width+j].mag = 255;
      }//for (j=0; j<m_width; j++)
   }//for (i=0; i<m_height; i++)
   //once Normal map has been built, dump the old texture and replace
   //making sure all other texture state info is updated as appropriate
   delete [] m_pImage;
   m_pImage = (unsigned char*)nmap;
   m_format = GL_BGRA_EXT;
}//void AWNormalMapTexture::convertHeightFieldToNormalMap(float scale)
 


int          
AWNormalMapTexture::init()
{
   int retVal = 0;
   if ((GL_LUMINANCE == m_format) && m_pImage && m_width && m_height)
   {  //we have a HEIGHT FIELD - convert to NVIDIA-type NORMAL MAP
      convertHeightFieldToNormalMap(m_size);
   }//if ((GL_LUMINANCE == m_format) && m_pImage && m_width && m_height)
   return retVal;
}//int AWNormalMapTexture::init()

//////////////////////////////////////////////////////////
//
//AWPersistable virtuals
//
//////////////////////////////////////////////////////////

int					
AWNormalMapTexture::compareTo(const AWPersistable* o) const
{
	return AWTexture::compareTo(o);
}


AWBoolean								
AWNormalMapTexture::isEqual(const AWPersistable* o)const
{
	return AWTexture::isEqual(o);
}//AWBoolean AWNormalMapTexture::isEqual(const AWPersistable* o)const



//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWNormalMapTexture
//
//////////////////////////////////////////////////////////
void	
AWNormalMapTexture::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (version <= VER_AWNORMAL_TEXMAP_2)
	{
		AWTexture::restoreGuts(file);
      if (VER_AWNORMAL_TEXMAP_2 == version)
      {
         file >> m_size;
      }//if (VER_AWNORMAL_TEXMAP_2 == version)
	}//if (version <= VER_AWNORMAL_TEXMAP_2)
}//void	AWNormalMapTexture::restoreGuts(AWFile& file)



void        
AWNormalMapTexture::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTexture::saveGuts(file);
   file << m_size;
}//void AWNormalMapTexture::saveGuts(AWFile& file) const



AWspace     
AWNormalMapTexture::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWTexture::binaryStoreSize() +
      sizeof(m_size);
	return retVal;
}//AWspace AWNormalMapTexture::binaryStoreSize() const