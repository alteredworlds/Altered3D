#ifndef __AW_TEXTURE_H
#define __AW_TEXTURE_H

#include "AWToolDefs.h"
#include "AWPersistable.h"
#include "AWOrdered.h"
#include "AWCString.h"
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <OpenGLES/ES2/gl.h>


#define AW_NO_TEXTURE         0
#define AW_TEXTURE_2D         GL_TEXTURE_2D
#define AW_TEXTURE_CUBE_MAP   GL_TEXTURE_CUBE_MAP_ARB


class AWExport AWrgba
{
public:
   AWrgba(unsigned char rr=0, unsigned char gg=0, unsigned char bb=0, unsigned char aa=0)
      : r(rr), g(gg), b(bb), a(aa){;}
   unsigned char r, g, b, a;
   unsigned char& operator[](int i);
   AWrgba operator*(const double fac);
   AWrgba operator*(const AWrgba& other);
   AWrgba operator+(const AWrgba& other);
};//class AWExport AWrgba


inline AWrgba 
AWrgba::operator+(const AWrgba& other)
{
   AWrgba retVal(*this);
   retVal.r += other.r;
   retVal.g += other.g;
   retVal.b += other.b;
   retVal.a += other.a;
   return retVal;
}//inline AWrgba AWrgba::operator+(const AWrgba& other)


inline AWrgba 
AWrgba::operator*(const AWrgba& other)
{
   AWrgba retVal(*this);
   retVal.r *= other.r;
   retVal.g *= other.g;
   retVal.b *= other.b;
   retVal.a *= other.a;
   return retVal;
}//inline AWrgba AWrgba::operator*(const AWrgba& other)


inline AWrgba 
AWrgba::operator*(const double fac)
{
	return AWrgba((unsigned char)(fac * (const double)r), 
				  (unsigned char)(fac * (const double)g), 
				  (unsigned char)(fac * (const double)b), 
				  (unsigned char)(fac * (const double)a));
}


inline unsigned char& AWrgba::operator[](int i)
{
   switch (i)
   {
   case 1:
      return g;
      break;
   case 2:
      return b;
      break;
   case 3:
      return a;
      break;
   default:
   case 0:
      return r;
      break;
   }//switch (i)
}//inline unsigned char& AWrgba::operator[](int i)



class AWExport  AWTexture: public AWPersistable
{
public:
   //'INIT' cases allow for easy renderer init
    enum GenMode   {G_NONE, SPHERE, REFLECT, EYE_LINEAR, OBJECT_LINEAR, G_INIT};
    enum BlendMode {B_NONE, ALPHA_TEST, ALPHA_BLEND, B_INIT};
    
	AWDECLARE_VERSIONABLE_PERSISTABLE(AWTexture);
    
	AWTexture(const AWCString& name = "", const AWCString& fileName = "") 
        : m_name(name), m_fileName(fileName), m_target(GL_TEXTURE_2D),
        m_pImage(NULL), m_width(0), m_height(0), 
        m_time(0.0), m_genMode(G_NONE),
        m_blendMode(B_NONE), m_texID(0), m_format(GL_RGBA),
        m_mipMap(TRUE), m_wrap(GL_REPEAT) {;}
    
	AWTexture(const AWTexture& other);
    int operator==(const AWTexture& other) {return compareTo(&other);}
    
	~AWTexture()            {if (m_pImage) delete [] m_pImage;}
    
	virtual void            compact(){;}
    virtual int             init()            {return 1;}
    
    virtual void			calc(double time, AWBoolean loop=TRUE){;}
    virtual int             getTextureID()                  {return m_texID;}
	virtual AWTexture*      getTexture(double time)          {return this;}
	virtual int             getNumFrames()                  {return 1;}
	virtual AWTexture*      getTexFromArrayIndex(int index) {return this;}
    int                     getBpp()const;
    int                     getWrap()const                  {return m_wrap;}
    void                    setWrap(int w)                  {m_wrap=w;}
    virtual int             getDataType()                   {return GL_UNSIGNED_BYTE;}
    
    
    AWrgba                  indexByDouble(double x, double y)const;
    AWrgba                  indexByInt(int x, int y)const;
    unsigned char*&         getDataPtr()                    {return m_pImage;}
    
	const double&            getTime()                       {return m_time;}
	void				    setTime(double t)                {m_time=t;}
    //utility function
    static int              fitToNearestPowerOf2(int num);
    static int              fitToNextNearestPowerOf2(int num, int up=1);
    
	virtual void            reSize(int xsize, int ysize=1);
    virtual void            assignAlphaFromColorIntensity();
    
	AWBoolean               isEqual(const AWPersistable* o) const;
	int                     compareTo(const AWPersistable* o) const;
	virtual void            restoreGuts(AWFile& file);
    virtual void            saveGuts(AWFile& file) const;
    
	virtual AWspace         binaryStoreSize() const;
    
	AWCString               m_fileName;
	AWCString               m_name;
	int                     m_width;
	int                     m_height;
    int                     m_target;
    //currently supports - [GL_RGBA, GL_RGB, GL_BGR_EXT, GL_BGRA_EXT, GL_LUMINANCE]
	int                     m_format;
    int                     m_wrap;
    
    GenMode                 m_genMode;     //implies TexGen mode
    BlendMode               m_blendMode;   //implies blend mode for entire AWObject
    
    GLuint                  m_texID;       //OpenGL texture object ID
    
    double			        m_time;	      //validity start time for texture
    
	unsigned char*          m_pImage;  //image data
    
    AWBoolean               m_mipMap;   //select if mipmapped - not persisted
    static AWBoolean        m_RGBtoRGBAOnLoad;
    
#ifdef __AWTRACE
    const char*             getTargetName();
#endif
};//class AWTexture : public AWPersistable



inline int                  
AWTexture::getBpp()const              
{
   int retVal = 0;
   switch (m_format)
   {
   case GL_RGB:
   //case GL_BGR_EXT:
      retVal = 3;
      break;

   case GL_LUMINANCE:
      retVal = 1;
      break;

   default:
   case GL_RGBA:
   //case GL_BGRA_EXT:
      retVal = 4;
      break;
   }//switch (m_format)
   return retVal;
}//int AWTexture::getBpp()const 



inline AWrgba 
AWTexture::indexByDouble(double x, double y)const
{
   int left   = (int)floor(x);
   int right  = left+1;
   int bottom = (int)floor(y);
   int top    = bottom+1;

   double vf  = top - y;   // vertical factor
   double hf  = right - x;  // horizontal factor

   AWrgba bl(indexByInt(bottom, left));
   AWrgba tl(indexByInt(top, left));
   AWrgba br(indexByInt(bottom, right));
   AWrgba tr(indexByInt(top, right));

   double blf = vf*hf;
   double tlf = (1.0-vf)*hf;
   double brf = vf*(1.0-hf);
   double trf = (1.0-vf)*(1.0-hf);

   return bl*blf + br*brf + tl*tlf + tr*trf;
}//inline AWrgba AWTexture::indexByDouble(double x, double y)const



inline AWrgba 
AWTexture::indexByInt(int x, int y)const 
{
   AWrgba retVal;  
   int    bpp   = getBpp();
   for (int comp=0; comp<bpp; comp++)
   {
      retVal[comp] = m_pImage[x*bpp + m_width*bpp*y + comp];
   }//for (int comp=0; comp<bpp; comp++)
   return retVal;
}//inline AWrgba AWTexture::indexByInt(int x, int y)const 


#endif //sentinel __AW_TEXTURE_H