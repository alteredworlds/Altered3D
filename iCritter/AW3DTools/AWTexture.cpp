#include "AWTexture.h"
#include "math.h"

AWBoolean     AWTexture::m_RGBtoRGBAOnLoad = TRUE;

#define VER_GLTEXTURE_1	 100
#define VER_GLTEXTURE_2	 110
#define VER_GLTEXTURE_3  111
#define VER_GLTEXTURE_4  112
#define VER_GLTEXTURE_5  113
#define VER_GLTEXTURE_6  114
#define VER_GLTEXTURE_7  115
#define VER_GLTEXTURE_8  116
#define VER_GLTEXTURE_9  117



AWDEFINE_VERSIONABLE_PERSISTABLE(AWTexture, CLSID_AWGLTEXTURE, VER_GLTEXTURE_9);


AWTexture::AWTexture(const AWTexture& other)
{
	m_fileName		= other.m_fileName;
    m_target        = other.m_target;
	m_name			= other.m_name;
	m_width			= other.m_width;
	m_height        = other.m_height;
    
    m_format        = other.m_format;
    m_genMode       = other.m_genMode;
    m_blendMode     = other.m_blendMode;
    
	m_texID			= other.m_texID;
	m_time			= other.m_time;
    
    m_mipMap        = other.m_mipMap;
    m_wrap          = other.m_wrap;
    
	if (other.m_pImage)
	{
		m_pImage = new unsigned char[getBpp() * m_width * m_height];
		memcpy(m_pImage, other.m_pImage, getBpp() * m_width * m_height);
	}//if (other.m_pImage)
	else
	{
		m_pImage = NULL;
	}
}//AWTexture::AWTexture(const AWTexture& other)




void 	
AWTexture::reSize(int xsize, int ysize)
{
   float sx, sy;
   if (xsize > 1)
   {
      sx = (float)(m_width-1) / (float) (xsize-1);
   }
   else
   {
      sx = (float)(m_width-1);
   }

   if (ysize > 1)
   {
      sy = (float)(m_height-1) / (float) (ysize-1);
   }
   else
   {
      sy = (float)(m_height-1);
   }
   unsigned char* pixels = new unsigned char[xsize * ysize * getBpp()];
   // shrink width and/or height:  use an unweighted box filter 
   int i0, i1, row;
   int j0, j1, col;
   int ii, jj, k;
   int index =0;
   float sum;
   //for each row of output image
   for (row=0; row < ysize; row++) 
   {
      i0 = (int)((float)row * sy);
      i1 = i0 + 1;
      if (i1 >= m_height) i1 = m_height-1; 
      for (col=0; col < xsize; col++) //for each pixel along row of output image
      {
         j0 = (int)((float)col * sx);
         j1 = j0 + 1;
         if (j1 >= m_width) j1 = m_width-1;
         // compute average of pixels in the rectangle (i0,j0)-(i1,j1) 
         for (k=0; k<getBpp(); k++) 
         {
            sum = 0.0f;
            for (ii=i0; ii<=i1; ii++) 
            {
               for (jj=j0; jj<=j1; jj++) 
               {
                  sum += *(m_pImage + (ii * m_width + jj)*getBpp() + k);
               }
            }
            sum /= (j1-j0+1) * (i1-i0+1);
            pixels[index++] = (unsigned char)sum;
         }//for (k=0; k<getBpp(); k++) 
         //don't average ALPHA - make above loop in getBpp()-1
         // *dst++ = *(m_pImage + (i0 * m_width + j0)*getBpp() + 3);
      }//for (col=0; col<xsize; col++)
   }//for (row=0; row<ysize; row++)
   delete [] m_pImage;
   m_pImage = pixels;
   m_width		 = xsize;
   m_height     = ysize;
}//void AWTexture:;reSize(int xsize, int ysize)



void 
AWTexture::assignAlphaFromColorIntensity()
{
   //we can assume for this initial version that
   //the format is GL_RGBA already (may have been 
   //converted on load from GL_RGB) 
   AWrgba val;
   if ((GL_RGBA!=m_format)/*&&(GL_RGBA8!=m_format)*/) return;
   if (!m_width || !m_height || !m_pImage) return;
   int dataIndex = 0;
   for (int row=0; row<m_height; row++)
   {
      for (int p=0; p<m_width; p++)
      {
         dataIndex = row*m_width*4 + p*4;
         val.r = m_pImage[dataIndex];
         val.g = m_pImage[dataIndex+1];
         val.b = m_pImage[dataIndex+2];
         //compute alpha value as sum of colors.
         m_pImage[dataIndex+3] = (unsigned char)(val.r + val.g + val.b);
      }//for (int p=0; p<m_width; p++)
   }//for (int row=0; row<m_height; row++)
}//void AWTexture::assignAlphaFromColorIntensity()



//////////////////////////////////////////////////////////
//
//AWPersistable virtuals
//
//////////////////////////////////////////////////////////

int					
AWTexture::compareTo(const AWPersistable* o) const
{
	//for greater than / less than only bother to compare the TIME, 
	//on which keys are sorted.
	//For equality, also compare the texture name
	//factor of 100 is to ensure that accuracy .0n is OK
	//may cause arithmetic overflow? depends on usual range of
	//time values - should be OK for now.
	int retVal = (int)(100.0f * (m_time - ((AWTexture*)o)->m_time));
	if (retVal == 0)
	{
		retVal = isEqual(o);
	}
	return retVal;
}//int AWTexture::compareTo(const AWPersistable* o) const


AWBoolean								
AWTexture::isEqual(const AWPersistable* o)const
{  // compare the texture name
   AWBoolean retVal = (0==m_name.compare(((AWTexture*)o)->m_name));
   retVal &= (0==m_fileName.compare(((AWTexture*)o)->m_fileName));
   retVal &= (m_time == ((AWTexture*)o)->m_time);
   //the following has a bug in it - MSVC++ 6.0 SP4?
	//return  ( (m_name == ((AWTexture*)o)->m_name) &&
   //          (m_fileName == ((AWTexture*)o)->m_fileName) &&
	//		    (m_time == ((AWTexture*)o)->m_time) );
   return retVal;
}//AWBoolean AWTexture::isEqual(const AWPersistable* o)const


//UTILITY FUNCTION
int           
AWTexture::fitToNearestPowerOf2(int num)
{
   int retVal = 0;
	if (num)
	{
		//we can find the power of 2 that's nearest
		//to the supplied number using the following highschool maths
		//
		//if x = 2^n 
		//then:	ln(x) = ln(2) * n
		//and thus n = ln(x) / ln(2)
		double pow, fraction, integer;
		pow = log((double)num) / log(2.0);
		fraction = modf(pow, &integer);
		retVal = (int)integer;
		if (fraction >= 0.5)
		{
			retVal++;
		}//if (fraction >= 0.5)
		//now use double pow( double x, double y ); to get the result
	}//if (num)
	retVal = (int)pow( 2.0, (double)retVal );
	return retVal;
}//int AWTexture::fitToNearestPowerOf2(int num)


int           
AWTexture::fitToNextNearestPowerOf2(int num, int up)
{
    int retVal = 0;
	if (num)
	{
		//we can find the power of 2 that's nearest
		//to the supplied number using the following highschool maths
		//
		//if x = 2^n 
		//then:	ln(x) = ln(2) * n
		//and thus n = ln(x) / ln(2)
		double pow, fraction, integer;
		pow = log((double)num) / log(2.0);
		fraction = modf(pow, &integer);
		retVal = (int)integer;
		if (fraction >= 0.5)
		{
			retVal++;
		}//if (fraction >= 0.5)
		//now use double pow( double x, double y ); to get the result
	}//if (num)
    if (up)
    {
        retVal++;
    }
    else if (retVal>1)
    {
        retVal--;
    }
	retVal = (int)pow( 2.0, (double)retVal );
	return retVal;
}//int AWTexture::fitToNearestPowerOf2(int num, int up)


//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWTexture
//
//////////////////////////////////////////////////////////
#define AW_BLENDTYPE_NONE			0
#define AW_BLENDTYPE_ALPHATEST	8
#define AW_BLENDTYPE_ALPHA			4
void	
AWTexture::restoreGuts(AWFile& file)
{
    int            temp;
    long           imageSize;
	unsigned int   version;
    
    m_pImage   =  NULL;
    
	restoreVersion(file, &version);   // Get the saved version #
	if ( version <= VER_GLTEXTURE_9 )
	{
		AWPersistable::restoreGuts(file);
        
		file        >> m_fileName;
        file        >> m_name;
        if (version < VER_GLTEXTURE_6)
        {
            AWBoolean tmp;
            file.Read(tmp);
        }//if (version < VER_GLTEXTURE_6)
        
		file.Read(m_target);	//correct in v8 and beyond...
		file.Read(m_width);
		file.Read(m_height);
        
        if (version >= VER_GLTEXTURE_4)
        {  //version 4 stores all data in updated format
            file.Read(m_format);
            
            file.Read(temp);  //stored as an int
            m_genMode = (enum GenMode)(temp);
            
            file.Read(temp);  //stored as an int
            m_blendMode = (enum BlendMode)(temp);
            
            //VER_GLTEXTURE_4 incorrectly stored m_format == GL_RGBA 
            //for non-blended case
            m_format = ((VER_GLTEXTURE_4==version) &&
                        (GL_RGBA==m_format) &&
                        (B_NONE==m_blendMode)) ? GL_RGB : m_format;
        }//if (version >= VER_GLTEXTURE_4)
        else
        {
            m_genMode   = G_NONE;
            //by default, used GL_RGBA
            m_format    = GL_RGBA;
            //m_target contained the texFunc
            switch (m_target)
            {
                case AW_BLENDTYPE_ALPHATEST:
                    m_blendMode = ALPHA_TEST;
                    break;
                case AW_BLENDTYPE_ALPHA:
                    m_blendMode = ALPHA_BLEND;
                    break;
                case AW_BLENDTYPE_NONE:
                default:
                    m_blendMode = B_NONE;
                    //no ALPHA stored for no-blend case
                    m_format    = GL_RGB;
            }//switch (readTexFunc)
        }//else
        if (version < VER_GLTEXTURE_8)
        {
            m_target = GL_TEXTURE_2D;
        }//if (version < VER_GLTEXTURE_8)
        if (version < VER_GLTEXTURE_9)
        {
            float tmp;
            file.Read(tmp);
            m_time = (double)tmp;
        }
        else
            file.Read(m_time);
        m_wrap = GL_REPEAT;
        if (version >= VER_GLTEXTURE_7)
        {
            file >> m_wrap;
        }//if (version >= VER_GLTEXTURE_7)
		file.Read(imageSize);
        
		if (imageSize)
		{
            if (m_RGBtoRGBAOnLoad && (GL_RGB==m_format))
			{  //IMAGE STORED AS GL_RGB, CONVERT TO GL_RGBA
                m_format = GL_RGBA; //affects getBpp() below
				m_pImage = new unsigned char[m_width*m_height*getBpp()];
				unsigned char* pNextRead = m_pImage;
				for (int pix=0; pix<m_width*m_height; pix++)
				{
					file.Read(pNextRead, 3);
					pNextRead += 3;
					*pNextRead++=1;
				}//for (int pix=0; pix<m_width*m_height; pix++)
			}//if (GL_RGBA == loadFormat)
			else
			{  //IMAGE STORED as described by m_format
                //imageSize = m_width*m_height*getBpp();
                m_pImage =new unsigned char[imageSize];
				file.Read(m_pImage, imageSize);
			}//else
		}//if (imageSize)
	}//if ( version <= VER_GLTEXTURE_6 ) 
}//void	AWTexture::restoreGuts(AWFile& file)



void        
AWTexture::saveGuts(AWFile& file) const
{
    long imageSize = m_pImage ? m_width * m_height * getBpp() : 0;
    
	saveVersion(file, uVersion_ );	// version number
	AWPersistable::saveGuts(file);
	file << m_fileName;
    file << m_name;
	file.Write(m_target);
	file.Write(m_width);
	file.Write(m_height);
    file.Write(m_format);
    file.Write((int)m_genMode);
    file.Write((int)m_blendMode);
	file.Write(m_time); 
    file.Write(m_wrap);
	file.Write(imageSize);
    if (imageSize)
	{
		file.Write(m_pImage, imageSize);
	}//if (imageSize)
}//void AWTexture::saveGuts(AWFile& file) const



AWspace     
AWTexture::binaryStoreSize() const
{
   AWspace texSize = sizeof(unsigned char) * 
                     (m_pImage ? m_width * m_height * getBpp() : 0);

   AWspace retVal = sizeof uVersion_ +
                     AWPersistable::binaryStoreSize() +
                     m_fileName.binaryStoreSize() +
                     m_name.binaryStoreSize() +
                     sizeof(m_target) +  
                     sizeof(m_width) +
                     sizeof(m_height) +
                     sizeof(m_format) +
                     sizeof(int) +        //m_genMode
                     sizeof(int) +        //m_blendMode                 
                     sizeof(m_time) +
                     sizeof(long) +	      //imagesize
                     sizeof(m_wrap) +
                     texSize;
   return retVal;
}//AWspace AWTexture::binaryStoreSize() const

//debugging TRACE helpers
#ifdef __AWTRACE
static const char* targetStr[] = {"GL_TEXTURE_1D", "GL_TEXTURE_2D", "GL_TEXTURE_3D_EXT", "GL_TEXTURE_CUBE_MAP_ARB", "UNKNOWN"};
const char* AWTexture::getTargetName()
{
   const char* retVal = NULL;
   switch (m_target)
   {
   /*case GL_TEXTURE_1D:
      retVal = targetStr[0];
      break;*/

   case GL_TEXTURE_2D:
      retVal = targetStr[1];
      break;

   /*case GL_TEXTURE_3D_EXT:
      retVal = targetStr[2];
      break;

   case GL_TEXTURE_CUBE_MAP_ARB:
      retVal = targetStr[3];
      break;*/

   default:
      retVal = targetStr[4];
      break;
   }//switch (m_target)
   return retVal;
}//const char* AWTexture::getTargetName()
#endif
