#include "AWCubeEnvTexture.h"

#define VER_AWCUBE_ENV_TEXMAP_1	100
#define VER_AWCUBE_ENV_TEXMAP_2	102

#include <OpenGLES/ES1/glext.h>


const int AWCubeEnvTexture::NUM_FACES = 6;

//These are the rotation angles, 
const float AWCubeEnvTexture::m_mapRots[6][4] = {		
	{	90.0f,		0.0f,	1.0f,	0.0f	},
	{	-90.0f,		0.0f,	1.0f,	0.0f	},

	{	-90.0f,		1.0f,	0.0f,	0.0f	},
	{	90.0f,		1.0f,	0.0f,	0.0f	},

	{	180.0f,		0.0f,	1.0f,	0.0f	},
	{	0.0f,		   0.0f,	1.0f,	0.0f	},
};

 
AWDEFINE_VERSIONABLE_PERSISTABLE(AWCubeEnvTexture, CLSID_AWCUBEENVTEXTURE, VER_AWCUBE_ENV_TEXMAP_2);


AWCubeEnvTexture::AWCubeEnvTexture(const AWCString& name, const AWCString& fileName) 
            : AWTexture(name, fileName)
{
   m_auto       = FALSE;
   m_genMode    = AWTexture::REFLECT;
   m_texFaces.fillWith((AWTexture*)NULL, NUM_FACES);
   m_lastUpdate = 0.0f;
   m_wrap       = GL_CLAMP_TO_EDGE;
   m_target     = GL_TEXTURE_CUBE_MAP_ARB;
}



AWCubeEnvTexture::~AWCubeEnvTexture()
{
   destroyFaces();  
}//AWCubeEnvTexture::~AWCubeEnvTexture()



void                 
AWCubeEnvTexture::destroyFaces()
{
   for (int i=0; i<m_texFaces.getNum(); i++ )
   {
      if (m_texFaces.getOwnsObjects())
      {
         delete m_texFaces[i];         
      }//if (m_texFaces[i])
      m_texFaces[i]=NULL;
   }//for (int i=0; i<NUM_FACES; i++ )
}//void AWCubeEnvTexture::destroyFaces()



void 	
AWCubeEnvTexture::reSize(int xsize, int ysize)
{
   if (m_pImage) 
   {
      AWTexture::reSize(xsize, ysize);
   }
   else
   {
      if (NUM_FACES != m_texFaces.getNum()) return;
      for (int i=0; i<NUM_FACES; i++ )
      {
         if (m_texFaces[i])
         {
            m_texFaces[i]->reSize(xsize, ysize);
         }//if (m_texFaces[i])
      }//for (int i=0; i<NUM_FACES; i++ )
   }
}//void AWCubeEnvTexture::reSize(int xsize, int ysize)



/*enum FaceEnum {POS_X=0, NEG_X=1, POS_Y=2, 
                  NEG_Y=3, POS_Z=4, NEG_Z=5};*/
int
AWCubeEnvTexture::getIndexFromFaceEnum( FaceEnum f)
{
   int retVal = -1;
   switch (f)
   {
   case POS_X:
      retVal = 0;
      break;
   case NEG_X:
      retVal = 1;
      break;
   case POS_Y:
      retVal = 2;
      break;
   case NEG_Y:
      retVal = 3;
      break;
   case POS_Z:
      retVal = 4;
      break;
   case NEG_Z:
      retVal = 5;
      break;
   }//switch
   return retVal;
}//int AWCubeEnvTexture::getIndexFromFaceEnum( FaceEnum f)


int                  
AWCubeEnvTexture::addFace(AWTexture* tex, FaceEnum f)
{
   int retVal = getIndexFromFaceEnum(f);
   if (-1 != retVal)
   {
      m_texFaces[retVal] = tex;
   }//if (-1 != retVal)
   return retVal;
}//int AWCubeEnvTexture::addFace(AWTexture* tex, FaceEnum f)


AWTexture*           
AWCubeEnvTexture::getFace(FaceEnum f)
{
   AWTexture* retVal = NULL;
   int index = getIndexFromFaceEnum(f);
   if (-1 != index)
   {
      retVal = m_texFaces[index];
   }//if (-1 != retVal)
   return retVal;
}//int AWCubeEnvTexture::addFace(AWTexture* tex, FaceEnum f)



AWrgba               
AWCubeEnvTexture::indexCube(const AWPoint& v)
{
   AWrgba retVal;

   double ax = fabs(v.x);
   double ay = fabs(v.y);
   double az = fabs(v.z);

   double i, j, k;

   FaceEnum fenum;

   if ((az >= ay) && (az >= ax)) 
   {
      if (v.z>0) 
      {
         fenum = POS_Z;
         i = -v.x;
         j = v.y;
         k = v.z;
      } 
      else 
      {
         fenum = NEG_Z;
         i = v.x;
         j = v.y;
         k = -v.z;
      }
   } //if ((az >= ay) && (az >= ax)) 
   else if ((ax > ay) && (ax > az)) 
   {
      if (v.x>0) 
      {
         fenum = POS_X;
         i = v.z;
         j = v.y;
         k = v.x;
      } 
      else 
      {
         fenum = NEG_X;
         i = -v.z;
         j = v.y;
         k = -v.x;
      }
   }//else if ((ax > ay) && (ax > az))
   else 
   {
      AWBoolean test = ((ay>=ax) && (ay>=az));
      if (v.y>0) 
      {
         fenum = POS_Y;
         i = v.x;
         j = v.z;
         k = v.y;
      }//if (y>0)  
      else 
      {
         fenum = NEG_Y;
         i = v.x;
         j = -v.z;
         k = -v.y;
      }
   }//else
   AWTexture* face = getFace(fenum);
   if (face)
   {
      int u = 0;
      int v = 0;
      float size = (float)face->m_width;

      u = (int)(size * (j/k + 1.0f) / 2.0f - 0.5f);
      v = (int)(size * (i/k + 1.0f) / 2.0f - 0.5f);
      
      int faceBpp = face->getBpp();
      for (int comp=0; comp<faceBpp; comp++)
      {
         retVal[comp] = face->m_pImage[v*face->m_width*faceBpp + u*faceBpp + comp];   
      }//for (int comp=0; comp<faceBpp; comp++)
      //return indexByInt(u, v);
      //retVal = indexByDouble(j,i);
   }//if (face)
   else
   {
      return retVal;
   }
   return retVal;
}//AWrgba AWCubeEnvTexture::indexCube(const AWPoint& vec)


/*enum FaceEnum {POS_X=0, NEG_X=1, POS_Y=2, 
                  NEG_Y=3, POS_Z=4, NEG_Z=5};*/

// NORMALIZATION CUBE MAP CONSTRUCTION 

// Given a cube map face index, cube map size, and integer 2D face position,
// return the cooresponding normalized vector.
void
AWCubeEnvTexture::getCubeVector(int i, int size, int x, int y, AWPoint& vec)
{
   float s, t, sc, tc;

   s = ((float)x + 0.5f) / (float)size;
   t = ((float)y + 0.5f) / (float)size;
   sc = s*2.0f - 1.0f;
   tc = t*2.0f - 1.0f;

   switch (i) 
   {
   case 0: //POS_X
      vec.x = 1.0f;
      vec.y = -tc;
      vec.z = -sc;
      break;
   case 1: //NEG_X
      vec.x = -1.0f;
      vec.y = -tc;
      vec.z = sc;
      break;
   case 2: //POS_Y
      vec.x = sc;
      vec.y = 1.0f;
      vec.z = tc;
      break;
   case 3: //NEG_Y
      vec.x = sc;
      vec.y = -1.0f;
      vec.z = -tc;
      break;
   case 4: //POS_Z
      vec.x = sc;
      vec.y = -tc;
      vec.z = 1.0f;
      break;
   case 5: ////NEG_Z
      vec.x = -sc;
      vec.y = -tc;
      vec.z = -1.0f;
      break;
   }// switch (i) 
   vec.normalize();
}//void AWCubeEnvTexture::getCubeVector(int i, int size, int x, int y, AWPoint& vec)




// Initialize a cube map texture object that generates RGB values
// that when expanded to a [-1,1] range in the register combiners
// form a normalized vector matching the per-pixel vector used to
// access the cube map.
int                  
AWCubeEnvTexture::generateNormalizeVectorCube(int size)
{  //first, destroy any existing data
   m_texFaces.destroy();
   m_texFaces.fillWith((AWTexture*)NULL, NUM_FACES);
   m_mipMap  = FALSE;
   m_genMode = G_NONE;
   m_format  = GL_RGB;
   //now create six texture faces where:
   //m_format == GL_RGB;  m_width == m_height == sizePixels;
   int         x,y;
   AWPoint     vec;
   AWTexture*  tex = NULL;
   char        texName[100];
   for (int f=0; f<NUM_FACES; f++ ) 
   {  //generate the data
      sprintf(texName, "normalize%i", f);
      tex = new AWTexture(texName);
      tex->m_format = GL_RGB;
      tex->m_width  = size;
      tex->m_height = size;
      tex->m_pImage = new unsigned char[tex->m_width*tex->m_height*3];
      for (y = 0; y < size; y++) 
      {
         for (x = 0; x < size; x++) 
         {
            getCubeVector(f, size, x, y, vec);
            //vec.x=0.0f;vec.y=0.0f;vec.z=1.0f;vec.normalize();
            tex->m_pImage[3*(y*size+x) + 0] = 128 + (unsigned char)(127.0f*vec.x);
            tex->m_pImage[3*(y*size+x) + 1] = 128 + (unsigned char)(127.0f*vec.y);
            tex->m_pImage[3*(y*size+x) + 2] = 128 + (unsigned char)(127.0f*vec.z);
         }//for (x = 0; x < size; x++) 
      }//for (y = 0; y < size; y++) 
      addFace(tex, enum FaceEnum(f));
   }//for (int f=0; f<NUM_FACES; f++ )
   return 1;
}//int AWCubeEnvTexture::generateNormalizeVectorCube(int sizePixels)



void                 
AWCubeEnvTexture::transformToSphereMap(int size)
{
   if (!m_texFaces.getNum() || !m_texFaces[0]) return;
   if (size <= 0)
   {  //use dimensions of one of the cube map faces for
      //the new sphere map.
      size = 2 * m_texFaces[0]->m_width;
      m_format = m_texFaces[0]->m_format;
      if (!size) return;
   }//if (-1 == size)
   m_genMode = AWTexture::SPHERE;
   int bpp   = getBpp();
   //iterate over the 6 cube faces using a vector. 
   //Grab pixel data for given vector and build Sphere map.
   size     = fitToNearestPowerOf2(size);
   m_width  = size;
   m_height = size;
   m_pImage = new unsigned char[m_width*m_height*bpp];

   AWrgba c;  
   for (int j=0; j<size; ++j) 
   {
      for (int i=0; i<size; ++i) 
      {
         double x = (2.0/size) * double(i) - 1.0 + 0.5/size;
         double y = (2.0/size) * double(j) - 1.0 + 0.5/size;
         double zsq = 1.0 - x*x - y*y;

         if (zsq < 0.0) 
         {
            c.r=0; c.g=0; c.b=0; c.a=0;
         } 
         else 
         {
	         double z = sqrt(zsq);
	         AWPoint n((float)x,(float)y,(float)z);
	         AWPoint e(0.0f, 0.0f, -1.0f);
	         AWPoint r = e - 2.0f * dot(e, n) * n;
	         c = indexCube(r);
         }
         for (int k=0; k<bpp; k++)
         {
            m_pImage[j*m_width*bpp + i*bpp + k] = (unsigned char)c[k];
         }
      }//for (int i=0; i<size; ++i) 
   }//for (int j=0; j<size; ++j) 
   //remove the 6 cube faces as they will no longer be used.
   destroyFaces();
   m_target = GL_TEXTURE_2D;
}//void AWCubeEnvTexture::transformToSphereMap(int size)


//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWCubeEnvTexture
//
//////////////////////////////////////////////////////////
void	
AWCubeEnvTexture::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (version <= VER_AWCUBE_ENV_TEXMAP_2)
	{
		AWTexture::restoreGuts(file);
      m_texFaces.read(file);	
      if (VER_AWCUBE_ENV_TEXMAP_2 == version)
      {
         file.Read(m_auto);
         if (m_auto)
         {
            file.Read(m_near);
            file.Read(m_far);
            file.Read(m_step);
         }//if (m_auto)
      }//if (VER_AWCUBE_ENV_TEXMAP_2 == version)
	}//if (version <= VER_AWCUBE_ENV_TEXMAP_2)
   m_target = GL_TEXTURE_CUBE_MAP_ARB;
}//void	AWCubeEnvTexture::restoreGuts(AWFile& file)




void        
AWCubeEnvTexture::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTexture::saveGuts(file);
	//file << m_texFaces;
   m_texFaces.write(file);
   file.Write(m_auto);
   if (m_auto)
   {
      file.Write(m_near);
      file.Write(m_far);
      file.Write(m_step);
   }//if (m_auto)
}//void AWCubeEnvTexture::saveGuts(AWFile& file) const




AWspace     
AWCubeEnvTexture::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWTexture::binaryStoreSize() +
      m_texFaces.getStoreSize() +
      sizeof(m_auto);
   if (m_auto)
   {
      retVal =+ sizeof(m_near) + 
                sizeof(m_far)  + 
                sizeof(m_step);
   }//if (m_auto)
	return retVal;
}//AWspace AWCubeEnvTexture::binaryStoreSize() const