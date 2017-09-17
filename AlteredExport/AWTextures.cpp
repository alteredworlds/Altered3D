#include "AlteredExport.h"
#include "bmmlib.h"
#include "iparamm2.h"

#include <AW3DTools/AWToolsCommon.h>
#include <AW3DTools/AWAnimatedTexture.h>
#include <AW3DTools/AWFlightController.h>
#include <AW3DTools/AWFlock.h>
#include <AW3DTools/AWMaterial.h>
#include <AW3DTools/AWFog.h>
#include <AW3DTools/AWSea.h>
#include <AW3DTools/AWMaterial.h>


AWMaterial* 
TnTExprt::getMaterial(int i)               
{
   AWMaterial* retVal = NULL;
   if ((-1!=i) && (i<m_mat.Count()))
   {
      retVal = m_mat[i];
   }
   else
   {
      fprintf(stderr, "TnTExprt::getMaterial called before Materials added!\n");;
   }
   return retVal;
}//AWMaterial* TnTExprt::getMaterial(int i) 



int TnTExprt::addMaterial(AWMaterial* m)
{
   if (!m) 
	{
		return -1;
	}

	int numMtls = m_mat.Count();
	for (int i=0; i<numMtls; i++) 
	{
		if (m_mat[i] == m) 
		{
			return FALSE;
		}
	}
	m_mat.Append(1, &m, 25);

	return TRUE;
}




Bitmap*
TnTExprt::getMAXBitmap(Texmap* map, AWBoolean& hasAlpha, float& frameStart, float& frameEnd, 
					   AWCString& name, float useFrame)
{
	Bitmap* bmp = NULL;
	frameStart = 0.0f;
	frameEnd   = 0.0f;
	if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
	{	//we're loading a Bitmap from a file on disk
		BitmapTex* bmpTex = (BitmapTex*)map;
      
		name = bmpTex->GetMapName();
		BitmapInfo bi;
		bi.SetName(name);
		bi.SetCurrentFrame(useFrame);
		BMMRES status;
		bmp = TheManager->Load(&bi, &status);	
		int type = bi.Type();	
		if (status != BMMRES_SUCCESS) 
		{ 
			MessageBox(NULL, _T("Error loading bitmap."), _T("Error"), MB_ICONSTOP);
			return NULL;
		}
		frameStart = bi.FirstFrame();
		frameEnd   = bi.LastFrame();
		//float customStep = bmpTex->GetPlaybackRate();
      //hasAlpha   = bi.Flags()&MAP_HAS_ALPHA;
		hasAlpha   = bmp->HasAlpha() && (ALPHA_NONE != bmpTex->GetAlphaSource());
	}//if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
	else
	{	//this is most likely a procedural texture, so try and get MAX to render it
		//for the specified frame. We must create the bitmap first, and its properties
		//define the resolution, color depth and so on.
		BitmapInfo bi;
		// Initialize the BitmapInfo instance
		bi.SetType(BMM_TRUE_64);
		bi.SetWidth(256);
		bi.SetHeight(256);
		bi.SetFlags(MAP_HAS_ALPHA);
		bi.SetCustomFlag(0);
		bmp = TheManager->Create(&bi);
		map->RenderBitmap(useFrame, bmp, 1.0f, TRUE);
		hasAlpha = FALSE; //investigate this behaviour later
	}
	return bmp;
}//Bitmap* TnTExprt::getMAXBitmap(Texmap* map, AWBoolean& hasAlpha, float& frameStart, float& frameEnd, 
//					   AWCString& name, float useFrame)

//we are mapping from MAX's idea of UP/DOWN/LEFT/RIGHT/FRONT/BACK
//to OpenGL's axis nomenclature
//MAX source... 'Front' map means the front-most
//ie if I'm to be mapped, and I stand at origin looking down
//negative Z in good OpenGL manner, then MAX's 
//FRONT map will be used to texture my back  (hence POSITIVE_Z)
//BACK  map will be used to texture my front (hence NEGATIVE_Z)
//"_RT"  => GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
//"_LF"  => GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB
//"_UP"  => GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
//"_DN"  => GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
//"_BK"  => GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
//"_FR"  => GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
//

//STATIC INIT - mapping from
//enum FaceEnum {POS_X=0, NEG_X=1, POS_Y=2, NEG_Y=3, POS_Z=4, NEG_Z=5};
TCHAR* TnTExprt::suffixes[6] = 
{ 
   _T("_RT"), _T("_LF"), _T("_UP"), _T("_DN"), _T("_FR"), _T("_BK")
};

const TCHAR * 
TnTExprt::getCubeFaceExt(int f)
{
   return suffixes[(int)f];
}

//up down left right front back
//0    1    2    3     4     5

int getMAXSlotNumberFromAWCubeEnum(enum AWCubeEnvTexture::FaceEnum f)
{
   int retVal = 0;
   switch (f)
   {
   case AWCubeEnvTexture::POS_X:
      retVal = 3;
      break;
   case AWCubeEnvTexture::NEG_X:
      retVal = 2;
      break;
   case AWCubeEnvTexture::POS_Y:
      retVal = 0;
      break;
   case AWCubeEnvTexture::NEG_Y:
      retVal = 1;
      break;
   case AWCubeEnvTexture::POS_Z:
      retVal = 4;
      break;
   case AWCubeEnvTexture::NEG_Z:
      retVal = 5;
      break;
   }//switch (f)
   return retVal;
}//int getMAXSlotNumberFromAWCubeEnum()


AWCubeEnvTexture* 
TnTExprt::initCubeTexture(Texmap* map, const AWCString& matName, 
                          const AWCString& fileName,  float alphaFac, 
                          AWBoolean alphaFromRGB)
{
   if (!map) return FALSE;
	AWCubeEnvTexture*  retVal = new AWCubeEnvTexture(matName, fileName);
   //contains 6 sub-textures which act as the faces
   //need to find the names of these files
   //load each in turn and grab the image data
   //put image data from each face into a new AWTexture
   //and add faces
   enum 
   { 
	   acubic_size, acubic_blur, acubic_bluroffset, acubic_near, acubic_far,
	   acubic_source, acubic_useatmospheric, acubic_applyblur,
	   acubic_frametype, acubic_nthframe,
	   acubic_bitmap_names,
	   acubic_outputname,
   };
 
   int      isCubicMap  = map->ClassID() == Class_ID(ACUBIC_CLASS_ID, 0);
   Class_ID clid        = map->ClassID();
   IParamBlock2* tn = NULL;
   AWCString     fName;
   if (map->NumParamBlocks())
   {
      Interval iv;
      TCHAR*   faceFileName   = NULL;	
      int      num     = 0;
      int      srcVal  = 0;
      int      notAuto = 0;
      int      maxSlotNum = 0;
      int      facesAdded = 0;
      tn = map->GetParamBlock(0);     
      if (tn)
      {  //SEE IF THIS SHOULD GENERATE AT RUN TIME, OR LET MAX DO IT...
         tn->GetValue(acubic_source, GetStaticFrame(), notAuto, iv, num);
         if (notAuto)
         {  //we're using MAX generated images/ NON-AUTO CASE...          
            AWTexture* nextFace = NULL;
            AWCString baseExt, nextFaceExt, faceName, faceFile;
            maxSlotNum = getMAXSlotNumberFromAWCubeEnum(enum AWCubeEnvTexture::FaceEnum(0));
            //GET THE FIRST FILE NAME
            if (tn->GetValue(acubic_bitmap_names, 0, faceFileName, iv, maxSlotNum))
            {                
               int faceNum = 0;
               while ((faceNum<6) && faceFileName && _tcslen(faceFileName))
               { 
                  nextFaceExt = getCubeFaceExt(faceNum);
                  faceName = "awcube";
                  faceName += nextFaceExt;
              
                  nextFace = new AWTexture(faceName, faceFileName);
                  BitmapInfo bi;
		            bi.SetName(faceFileName);
		            bi.SetCurrentFrame(0);
		            BMMRES status;
                  if (TRUE)//AWFile::Exists(faceFileName))
                  {  //NOTE: if file doesn't exist in the specified posn,
                     //MAX's Bitmap Manager looks for it - and usually finds
                     //it just fine if it's anywhere sensible...
		               Bitmap* bmp = TheManager->Load(&bi, &status);	
		               int type = bi.Type();	
		               if (status == BMMRES_SUCCESS) 
                     {  //found & loaded OK
                        facesAdded++;
                        nextFace = new AWTexture(faceName, faceFileName);
                        AWBoolean hasAlpha = bi.Flags()&MAP_HAS_ALPHA;
                        initTexture(nextFace, bmp, hasAlpha, alphaFac);	
                        retVal->addFace(nextFace, enum AWCubeEnvTexture::FaceEnum(faceNum));
                     }//if (status == BMMRES_SUCCESS) 
                     else
                     {
                        delete retVal;
			               MessageBox(NULL, _T("Error loading bitmap."), _T("Error"), MB_ICONSTOP);
			               return NULL;
		               } 
                  } //if (TRUE)//AWFile::Exists(faceFileName))

                  faceNum++;  
                  if (faceNum<6) 
                  {
                     maxSlotNum = getMAXSlotNumberFromAWCubeEnum(enum AWCubeEnvTexture::FaceEnum(faceNum));
                     tn->GetValue(acubic_bitmap_names, 0, faceFileName, iv, maxSlotNum);
                  }//if (faceNum<6) 
               }//while ((faceNum<6) && faceFileName && _tcslen(faceFileName))
            }//if (tn->GetValue(acubic_bitmap_names, 0, faceFileName, iv, maxSlotNum))
            if (facesAdded < 6)
            {
               char msg[300];
               sprintf(msg, "Map Name: %s\n\nPlease supply all 6 texture file names!\n", matName.c_str());
               ::MessageBox(NULL, msg, "WARNING - Cube Map Texture Unspecified", MB_OK);
               delete retVal;
               retVal = NULL;
            }//if (facesAdded < 6)
         }//if (notAuto)
         else
         {  //AUTO - grab the 'camera' params from the material rollout
            float fParam;           
            retVal->setAuto(TRUE);
            tn->GetValue(acubic_size,     GetStaticFrame(), srcVal, iv, num);
            //WANT TO FIT THESE VALUES TO NEAREST POWER OF TWO,
            //either here or at load time. Better here, as best to 
            //save a self-consistent data set.
            retVal->m_width = retVal->m_height =  AWTexture::fitToNearestPowerOf2(srcVal);

            tn->GetValue(acubic_far,      GetStaticFrame(), fParam, iv, num);
            retVal->setFarClip(fParam);

            tn->GetValue(acubic_near,     GetStaticFrame(), fParam, iv, num);
            //protect against the 0.0 near view clipping plane
            //so long as we have any non-zero value in the far clipping plane,
            //set near plane to 1.0f
            if ((fParam <= 0.0f) && retVal->getFarClip())
            {
               fParam = 1.0f;
            }
            retVal->setNearClip(fParam);
            //determine if first frame only, or every nth 0 => first only
            tn->GetValue(acubic_frametype, GetStaticFrame(), srcVal, iv, num);
            if (!srcVal)
            {  //FIRST FRAME ONLY
               retVal->setStep(0.0f);
            }
            else
            {
               tn->GetValue(acubic_nthframe, GetStaticFrame(), srcVal, iv, num);
               retVal->setStep(TicksToSec(srcVal*GetTicksPerFrame()));
            }
            
         }//else
      }//if (tn)    
   }//if (test2)
   
   if (retVal)
   {
      retVal->m_genMode = AWTexture::GenMode::REFLECT;
   }//if (retVal)
   return retVal;
}//AWBoolean TnTExprt::initCubeTexture(Texmap* map, float alphaFac, AWBoolean alphaFromRGB)


//MAY BE BUG HERE... [new string handling]
void
TnTExprt::getBaseFileNameAndExtension(AWCString& fName, AWCString& baseExt)
{  //IN: fName - full file name including "_UP.xyz"
   //OUT: fName up to this string
   //     baseExt contains ".xyz"
   string::size_type pos = fName.find("_UP.");
   if (string::npos != pos)
   {
      baseExt = fName;
      baseExt.replace(0, pos+3, "");
      fName.replace(pos, fName.length()-pos, "");
   }//if (string::npos != pos)
}//void TnTExprt::getBaseFileNameAndExtension(AWCString& fName, AWCString& baseExt)




AWBoolean
TnTExprt::initTexture(AWTexture* tex, Bitmap* maxMap, 
					  AWBoolean useAlpha, float alphaFac, AWBoolean alphaFromRGB, 
                 AWBoolean luminance)
{
	//load in a MAX bitmap corresponding to the Texture passed in
	//this allows opening of any Image format that can be in this
	//material slot in MAX
	if (!tex || !maxMap) return FALSE;
	AWBoolean retVal = TRUE;
	//Alpha Blended?
	//alphaFac is from MAX Opacity for the material
	//alphaFromRGB alpha = r + g + b /3
	//useAlpha means take native image Alpha
	//new code - 
	//we want 
	//        alphaFromRGB || alphaFac != 1.0 means AW_BLENDTYPE_ALPHA
	//        useAlpha && alphaFac == 1.0 means AW_BLENDTYPE_ALPHATEST
   AWBoolean requireAlpha = !luminance && (useAlpha || (alphaFac != 1.0f) || alphaFromRGB);
   if (requireAlpha)
   {  //may be changed later to AWTexture::ALPHA_TEST if appropriate
      tex->m_blendMode = AWTexture::ALPHA_BLEND;
      tex->m_format    = GL_RGBA;
   }
   else
   {
      tex->m_blendMode = AWTexture::B_NONE;
      tex->m_format    = luminance ? GL_LUMINANCE : GL_RGB;
   }
	
    //allocate memory for local storage of the texture 
	tex->m_width   = maxMap->Width();
	tex->m_height  = maxMap->Height();

	tex->m_pImage = new unsigned char[tex->m_width * tex->m_height * tex->getBpp()];
	unsigned char* dest = tex->m_pImage; 
	//copy data from maxMap to texture object
	float alphaValue = 1.0f;
	float percentInBetween = 0.0f;
	BMM_Color_64 *line64, *l64 = NULL;
	line64 = (BMM_Color_64 *)calloc(tex->m_width, sizeof(BMM_Color_64));
	for (int row=0; row< tex->m_height; row++)
	{
		l64 = line64;
		int works = maxMap->GetLinearPixels(0, row, tex->m_width, line64);
		works += 25;
		unsigned char r, g, b;
		unsigned char* destPixelPtr = &tex->m_pImage[row * tex->m_width * tex->getBpp()];
		for (int col = 0; col < tex->m_width; col++)//, l64++)
		{
			r = (unsigned char)(l64[col].r >> 8);
         g = (unsigned char)(l64[col].g >> 8);
			b = (unsigned char)(l64[col].b >> 8);
         if (luminance)
         {
            *destPixelPtr++ = (r + g + b) / 3;
         }
         else
         {
            *destPixelPtr++ = r;
			   *destPixelPtr++ = g;
			   *destPixelPtr++ = b;
            if (requireAlpha)
            {
			      alphaValue = (unsigned char)255;
			      if (alphaFromRGB)
			      {	//use simple average
				      alphaValue = ((float)(r + g + b )) / 3.0f;
			      }
			      else if (useAlpha)
			      {
				      alphaValue = (float)((unsigned char)(l64[col].a >> 8));
			      }
			      alphaValue *= alphaFac;
			      if ((255.0f != alphaValue) & (0.0f != alphaValue)) 
				      percentInBetween++; 
			      *destPixelPtr++ = (unsigned char)alphaValue;
            }//if (requireAlpha)
         }//if (!luminance)
		}//for (int col=0; col< tex->m_width; col++)	
	}//for (int row=0; row< tex->m_width; row++)
	free(line64);
	//find the percentage of pixels with alpha exactly 0 or 1
	if (requireAlpha && tex->m_height && tex->m_width)
	{
		percentInBetween = percentInBetween / (float)(tex->m_height * tex->m_width);
		if (percentInBetween < 0.1f)
		{   //90% of pixels in this image are either 0 or 1
			//this surely means that this is a very suitable candidate 
			//for alpha testing for this texture
			tex->m_blendMode = AWTexture::BlendMode::ALPHA_TEST;
		}//if (percentInBetween < 0.1f)
	}//if (requireAlpha && tex->m_height && tex->m_width)
	//so NOW SCALE IMAGE IF REQUIRED  VOODOO needs max 256*256
	int oldWidth  = tex->m_width;
	int oldHeight = tex->m_height;	
	//fit width and height to nearest power of two, and resize if
	//necessary.
	int newWidth  = AWTexture::fitToNearestPowerOf2(oldWidth);
	int newHeight = AWTexture::fitToNearestPowerOf2(oldHeight);
   if (GetUseLimitTextureSize())
   {
      if (newWidth > GetTextureSizeLimit())
      {
         newWidth = GetTextureSizeLimit();
      }
      if (newHeight > GetTextureSizeLimit())
      {
         newHeight = GetTextureSizeLimit();
      }
   }//if (GetUseLimitTextureSize())
	//finally limit the maximum size to 256x256 for performance
	//and for the sake of older 3d cards qhich require this maximum
	//if (newWidth > 256)  newWidth  = 256;
	//if (newHeight > 256) newHeight = 256;
	if ((oldWidth != newWidth) || (oldHeight != newHeight))
	{	//finally, resize texture if required
		tex->reSize(newWidth, newHeight);
	} 
	return retVal;
}//AWBoolean TnTExprt::initTexture(AWTexture* tex, Bitmap* maxMap,
 //AWBoolean useAlpha, float alphaFac, AWBoolean alphaFromRGB)



AWTexture*
TnTExprt::dumpAnimatedTextureSample(Texmap* map, 
									float frameStart, 
									float frameEnd, 
									const AWCString& matName, 
									AWCString& fileName, 
									AWBoolean useAlpha, 
									float alphaFac,
									AWBoolean alphaFromRGB,
                           AWBoolean useNormalMapTex) 
{
	TimeValue			t;
	AWTexture*	tex = NULL;
	Bitmap*				maxMap = NULL;
	AWBoolean			hasAlpha = FALSE;
	int					delta = GetTextureFrameStep();

	AWAnimatedTexture* animTex = new AWAnimatedTexture(matName, fileName);
	float deltaInTicks = delta * GetTicksPerFrame();
	float deltaInSecs = TicksToSec(deltaInTicks);
	float playbackRate = 1.0f;
	if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
	{	//we're loading a Bitmap from a file on disk
		BitmapTex* bmpTex = (BitmapTex*)map;
		if (0.0f < bmpTex->GetPlaybackRate())
		{
			playbackRate = (float)(1.0 / bmpTex->GetPlaybackRate());
		}
	}
	animTex->setFrameStep(deltaInSecs*playbackRate);
	animTex->setFirstFrame(frameStart);
	//want to limit max number of frames to ?100? for memory reasons..!
	int duration = abs((frameEnd - frameStart)/delta);
	if (duration > 100)
	{
		frameEnd = frameStart + 99*delta;
	}
	animTex->setLastFrame(frameEnd);
	
	for (t=frameStart; t<=frameEnd; t+=delta) 
	{
		maxMap = getMAXBitmap(map, hasAlpha, frameStart, frameEnd, fileName, t);
		if (maxMap)
		{
         tex = useNormalMapTex ? new AWNormalMapTexture(matName, fileName)
                     : new AWTexture(matName, fileName);
			//tex->setTime(t );
			tex->setTime(playbackRate * TicksToSec((t-frameStart)*GetTicksPerFrame()));
			if (initTexture(tex, maxMap, useAlpha, alphaFac, alphaFromRGB, useNormalMapTex))
			{
				animTex->addTexture(tex);
				animTex->m_blendMode = tex->m_blendMode;
			}
			else 
			{
				delete tex;
				break;
			}
			maxMap->DeleteThis();
		}//if (maxMap)
	}//for (t=start; t<=end; t+=delta)
	return animTex;
}//AWTexture* TnTExprt::dumpAnimatedTextureSample(INode* node, const AWCString& matName, const AWCString& fileName) 



void        
TnTExprt::applyMaterial(AWObject* obj, int matID)
{  //both applies the base 'standard material properties'
   if (matID >= getNumMaterials()) return;
   AWMaterial* mat = m_mat[matID];
   //if passed:
   obj->addMaterial(mat); 
}//void TnTExprt::applyMaterial(AWObject* obj, int matID)



void
TnTExprt::getStdMaterial(AWMaterial* awMat, StdMat* mat)
{
   if (!awMat && !mat) return;
   awMat->setTwoSided(mat->GetTwoSided());

   awMat->setWire(mat->GetWire());

	Color col;
	col = mat->GetAmbient(GetStaticFrame());
	awMat->m_ambient = AWPoint(col.r, col.g, col.b);

	col = mat->GetDiffuse(GetStaticFrame());
	awMat->m_diffuse = AWPoint(col.r, col.g, col.b);

	col = mat->GetSpecular(GetStaticFrame());
	awMat->m_specular = AWPoint(col.r, col.g, col.b);
   //GetShinStr returns value   in range [0..10]
   float shinStrength = mat->GetShinStr(GetStaticFrame());
   //GetShininess returns value in range [0..1]
   float shinAmt      = mat->GetShininess(GetStaticFrame());
   //OpenGL GL_SHININESS expects value in range [0..128]
	awMat->m_shininess =  12.8f * shinStrength * shinAmt;
	col = mat->GetSpecular(GetStaticFrame()) * mat->GetSelfIllum(GetStaticFrame());
	
   AWPoint emissive;
   if (((StdMat2*)mat)->GetSelfIllumColorOn())
   {
      col = ((StdMat2*)mat)->GetSelfIllumColor(GetStaticFrame());
      emissive.x=col.r; emissive.y=col.g; emissive.z=col.b;
   }//if (((StdMat2*)mat)->GetSelfIllumColorOn())
   else
   {
      emissive = mat->GetSelfIllum(GetStaticFrame()) * awMat->m_diffuse;
   }
   awMat->m_emissive = emissive;  
}//void TnTExprt::getStdMaterial(AWMaterial* mat, StdMat* mat)



int
TnTExprt::getMapIndexIfAlreadyLoaded(Texmap* map, const AWCString& mapName)
{
   int texIndex = theScene.m_textureList.find(&AWTexture(mapName));
   if ( (AW_NPOS!=texIndex) && (Class_ID(BMTEX_CLASS_ID, 0)==map->ClassID()) )
   {  //make sure that the texture names are the same.
      //we're loading a Bitmap from a file on disk
		BitmapTex* bmpTex = (BitmapTex*)map;
		AWCString name(bmpTex->GetMapName());
      AWCString loadedMapName(((AWTexture*)theScene.m_textureList[texIndex])->m_fileName);
      if (loadedMapName!=name)
      {
         texIndex = AW_NPOS;
      }//if (loadedMapName==name)
   }//if (AW_NPOS!=texIndex)
   return texIndex;
}//int TnTExprt::getMapIndexIfAlreadyLoaded(Texmap* map, const AWCString& mapName)



AWTexture*
TnTExprt::getMapData(const AWCString& matName, Texmap* map, 
                     float alphaFac, AWBoolean alphaFromRGB, 
                     AWBoolean useNormalMapTex)
{
   AWCString useName(matName);
   char convertStr[100];
   AWTexture* tex = NULL;
   if (map)
	{
      sprintf(convertStr, ":%.2f:%1i", alphaFac, (int)alphaFromRGB);
      useName += convertStr;
      //NOTE - include alphaFac, && alphaFromRGB 
      //in name in standardized manner. Thus we see if it's been used before.

		//have we already grabbed this TexMap? see if it can be found in the scene    
      AWCString fileName;
      if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
	   {	//we're loading a Bitmap from a file on disk
		   fileName = ((BitmapTex*)map)->GetMapName();
      }//if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
      int texIndex = theScene.m_textureList.find(&AWTexture(useName, fileName));;
      AWTRACE3("Checking material %s filename %s - already loaded? %i\n", useName.c_str(), fileName.c_str(), (int)(-1!=texIndex));
      //texIndex = getMapIndexIfAlreadyLoaded(map, useName);	
      //AWTRACE1("second test %i\n", (int)(-1!=texIndex));
		if (AW_NPOS == texIndex)
		{	//if it can't be found, we need to get it.
			AWCString fileName;						
         if (Class_ID(ACUBIC_CLASS_ID, 0) == map->ClassID())
         {
            tex = initCubeTexture(map, useName, fileName, alphaFac, alphaFromRGB);		
         }
			else 
         {
            AWBoolean hasAlpha; 
            float	    frame0, frameN;
			   Bitmap*	 bmp = getMAXBitmap(map, hasAlpha, frame0, frameN, fileName);
			   if (bmp)
			   {
               if (GetIncludeAnimTextures() && (frame0 != frameN))
				   {
					   tex = dumpAnimatedTextureSample(map, frame0, frameN, useName, 
											                 fileName, hasAlpha, alphaFac, 
                                                  alphaFromRGB, useNormalMapTex);
				   }//if (GetIncludeAnimTextures() && (frame0 != frameN))
				   else
				   {
                  tex = useNormalMapTex ? new AWNormalMapTexture(useName, fileName)
                                          : new AWTexture(useName, fileName);
					   initTexture(tex, bmp, hasAlpha, alphaFac, alphaFromRGB, useNormalMapTex);		
				   }//else
				   bmp->DeleteThis();
			   }//if (bmp)  
         }//else
         if (tex) 
	      {
            theScene.m_textureList.add(tex);
	      }//if (tex) 
		}//if (AW_NPOS == texIndex)
      else
      {
         tex = (AWTexture*)theScene.m_textureList[texIndex];
      }    
	}//if (map)
   return tex;
}//AWTexture* TnTExprt::getMapData(const AWCString& matName, Texmap* tex, Matrix3& uvTransform, float alphaFac, AWBoolean alphaFromRGB)


//get the texture transform matrix for this face
void 
TnTExprt::getTextureMatrix(Matrix3& uvTransform, int matID)
{
   uvTransform.IdentityMatrix();
   if (1==getNumMaterials()) 
   {  //for just one material, matID doesn't matter at all
      //we just want the texture transform matrix!
      matID=0;
   }//if (1==getNumMaterials()) 
   if (getNumMaterials()>matID)
   {
      AWMaterial* mat = getMaterial(matID);
      if ( mat && mat->getNumTexEffects() )
	   {  //grab the material transformation matrix
         getTexMatrix(uvTransform, mat->getTextureEffect(0));      
	   }//if ( mat && mat->getNumTexEffects() )
   }//if (GetExportTextures() && getNumMaterials())
}//void TnTExprt::getTextureMatrix(Matrix3& uvTransform, int matID)


void
TnTExprt::getTexMatrix(AWTextureEffect* texEffect, Texmap* map)
{  //get the uv transform matrix for use when extracting tex coord from object
   if (texEffect && map)
   {
      Matrix3 uvTransform; //data is float m[4][3]; 4 rows of 3 floats
		map->GetUVTransform(uvTransform);
      //now copy over to the AWTextureEffect matrix for later use
      AWMatrix4& tmp = texEffect->m_uvTransform;
      tmp.identity();
      for (int row=0; row<4; row++)
      {  //grab each row, which consists of 3 floats
         //copying to structure which has 4 floats per row
         int s = row*4; //the start of the destination row
         Point3 rowPt = uvTransform.GetRow(row);
         tmp[s]   = rowPt[0];
         tmp[s+1] = rowPt[1];
         tmp[s+2] = rowPt[2];
         tmp[s+3] = 0.0f;
      }//for (int row=0; row<4; row++)
   }//if (texEffect && map)
}//void TnTExprt::getTexMatrix(AWTextureEffect* texEffect, Texmap* map)

void
TnTExprt::getTexMatrix(Matrix3& m, AWTextureEffect* texEffect)
{  //get the uv transform matrix for use when extracting tex coord from object
   if (texEffect)
   {
      //now copy over to the AWTextureEffect matrix for later use
      AWMatrix4& tmp = texEffect->m_uvTransform;
      for (int row=0; row<4; row++)
      {  //grab each row, which consists of 3 floats
         //copying from structure which has 4 floats per row
         int s = row*4; //the start of the destination row
         Point3 rowPt;
         rowPt[0] = tmp[s];
         rowPt[1] = tmp[s+1];
         rowPt[2] = tmp[s+2];
         //tmp[s+3] = 0.0f;
         m.SetRow(row, rowPt);
      }//for (int row=0; row<4; row++)
   }//if (texEffect && map)
}//void TnTExprt::getTexMatrix(Matrix3& m, AWTextureEffect* texEffect)



AWMaterial* 
TnTExprt::getStandardMaterialData(Mtl* mtl, int& meshType, AWBoolean forceMaterial)
{
   AWMaterial* mat = NULL;
   meshType = 0; //by default, normal AWMesh will be used.
	if (mtl && (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
	{           
		StdMat* std = (StdMat*)mtl;	
		//we now have access to the material for this node
		//first, retrieve the general material properties
		//access through the MAX class StdMat	
      AWCString matName(mtl->GetName());
      mat = new AWMaterial(matName);
      if (!mat) return NULL; //memory problem - 
      //have we already got this material?
      int tmpIndex = theScene.m_materialList.find(mat);
      if (AW_NPOS != tmpIndex)
      {
         delete mat;
         AWTRACE1("Material %s already exported - using previous version\n", matName.c_str());
         return (AWMaterial*)theScene.m_materialList[tmpIndex];
      }
      else
      {
         theScene.m_materialList.add(mat);
      }
      if (GetIncludeMtl() || forceMaterial) 
      {  //get MATERIAL data
         getStdMaterial(mat, std);
      }//if (GetIncludeMtl())
      else
      {
         mat->setUseDefaults(TRUE);
      }
      float alphaFac = std->GetOpacity(GetStaticFrame());
      //Access the two sided property of the material
      mat->setTwoSided(std->GetTwoSided());
      
      Matrix3 useMatrix;
      int     numSub = 0;
      if (GetExportTextures() && (numSub = mtl->NumSubTexmaps()))
      {
         AWTextureEffect* texEffect = NULL;
         AWTexture* tmpAWTex = NULL;
         //get the DIFFUSE TEXTURE (if any)
         Texmap* map = mtl->GetSubTexmap(ID_DI);
         if (map && mtl->SubTexmapOn(ID_DI))
         {
            //bit of a hack here - idea seems to be that if you want to
            //use the image RGB to supply the A, then you'd put a copy
            //of the image in the MAX Opacity Channel. 
			   AWBoolean alphaFromRGB = FALSE;
            if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
	         {	//we can find where alpha from...
		         alphaFromRGB = ALPHA_RGB == ((BitmapTex*)map)->GetAlphaSource();
            }
            
            tmpAWTex = getMapData((char*)map->GetName(), map, alphaFac, alphaFromRGB);
            //unused texFunc param is used to indicated if there's ALPHA BLENDING		      
            if (tmpAWTex) 
	         {  //needs to be set on object, eventually, according to current design
               if (CLSID_AWCUBEENVTEXTURE==tmpAWTex->isA())
               {
                  texEffect = new AWTextureEffect(tmpAWTex, -1, AWTextureEffect::AW_REFLECT);
               }
               else
               {
                  texEffect = new AWTextureEffect(tmpAWTex, 0);
               }
               if (texEffect)
               {
                  getTexMatrix(texEffect, map);
                  mat->setBlendType(tmpAWTex->m_blendMode);
                  mat->addTextureEffect(texEffect);
               }
	         }//if (tmpAWTex) 
         }//if (map && mtl->SubTexmapOn(ID_DI))
      
         //REFLECT/REFRACT MAP?
         map = mtl->GetSubTexmap(ID_RL);
         if (map && mtl->SubTexmapOn(ID_RL))
         {
            //try and get the % amount for this map
            AWBoolean alphaFromRGB = FALSE;
            if (Class_ID(BMTEX_CLASS_ID, 0) == map->ClassID())
	         {	//we can find where alpha from...
		         alphaFromRGB = ALPHA_RGB == ((BitmapTex*)map)->GetAlphaSource();
            }
            float opacity = std->GetTexmapAmt(ID_RL, GetStaticFrame());
            tmpAWTex = getMapData((char*)map->GetName(), map, alphaFac, alphaFromRGB);
            if (tmpAWTex) 
	         {
               texEffect = new AWTextureEffect(tmpAWTex, -1, AWTextureEffect::AW_REFLECT, opacity);
               if (texEffect)
               {
                  getTexMatrix(texEffect, map);
                  mat->addTextureEffect(texEffect);
               }
	         }//if (tmpAWTex) 
         }//if (map && mtl->SubTexmapOn(ID_RL))
		 
		 //BUMP MAP?
         map = mtl->GetSubTexmap(ID_BU);
         if (map && mtl->SubTexmapOn(ID_BU))
         {  //try and get the % amount for this map
            float opacity = std->GetTexmapAmt(ID_BU, GetStaticFrame());
            //grab this texture as GL_LUMINANCE eg one component only
            tmpAWTex = getMapData((char*)map->GetName(), map, alphaFac, FALSE, TRUE);
            if (tmpAWTex) 
	         {
               if (CLSID_AWNORMALMAPTEXTURE==tmpAWTex->isA())
               {
                  //average (visible) bump is about +5.0f
                  //returned range for opacity is [-9.99..9.99]
                  //default returned setting is 0.0
                  opacity += 5.0f; //move default position to 5.0                 
                  ((AWNormalMapTexture*)tmpAWTex)->setSize(opacity);
               }//if (CLSID_AWNORMALMAPTEXTURE==tmpAWTex->isA())
               meshType = 1; //we're going to need AWBumpMesh
               AWTextureEffect::EffectType awtexEff = AWTextureEffect::AW_BUMP;
               if (mtl->SubTexmapOn(ID_SP))
               {
                  awtexEff = AWTextureEffect::AW_BUMP_AND_SPECULAR;
               }
               texEffect = new AWTextureEffect(tmpAWTex, 0, awtexEff, opacity);
               if (texEffect)
               {
                  getTexMatrix(texEffect, map);
                  mat->addTextureEffect(texEffect);
               }
	         }//if (tmpAWTex)
         }//if (map && mtl->SubTexmapOn(ID_BU))	
		}//if (GetIncludeTextures() && (numSub = mtl->NumSubTexmaps()))
	}//if (mtl && (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
   return mat;
}//AWMaterial* TnTExprt::getStandardMaterialData(Mtl* mtl, int& meshType, AWBoolean forceMaterial)



int   
TnTExprt::getNumSubMaterials(INode* node)
{
   int retVal = 0;
   if (node)
   {
      Mtl* mtl = node->GetMtl();
      if (mtl && (mtl->ClassID() == Class_ID(MULTI_CLASS_ID, 0)))
      {
         retVal = mtl->NumSubMtls();
      }//if (mtl && (mtl->ClassID() == Class_ID(MULTI_CLASS_ID, 0)))
   }//if (node)
   return retVal;
}//int TnTExprt::getNumSubMaterials(INode* node)


void                    
TnTExprt::clearTrackBumps()
{
   for (int i=0; i<m_trackBumps.Count(); i++)
   {
      delete m_trackBumps[i];
      m_trackBumps[i]=NULL;
   }
   m_trackBumps.Resize(0);
}//void TnTExprt::clearTrackBumps()


void
TnTExprt::clearFogNames()
{
   for (int i=0; i<m_fogNames.Count(); i++)
   {
      delete m_fogNames[i];
      m_fogNames[i]=NULL;
   }
   m_fogNames.Resize(0);
}//void TnTExprt::clearFogNames()



void
TnTExprt::clearCameras()
{
   for (int i=0; i<m_cameras.Count(); i++)
   {
      m_cameras[i]=NULL;
   }
   m_cameras.Resize(0);
}//void TnTExprt::clearCameras()


void
TnTExprt::clearMaterials()
{
   for (int i=0; i<m_mat.Count(); i++)
   {
      m_mat[i]=NULL;
   }
   m_mat.Resize(0);
}//void TnTExprt::clearMaterials()


int 
TnTExprt::getMaterialData(INode* n, int& meshType, AWBoolean forceMaterial)
{
   clearMaterials();
   AWMaterial* curMat = NULL;
   if (!forceMaterial)
   {
      AWBoolean val;
      forceMaterial = n->GetUserPropBool(_T("MATERIAL"), val);
   }//if (!forceMaterial)
	Mtl* mtl = n->GetMtl();
	if (mtl && (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
	{	//adds to object and scene
      curMat = getStandardMaterialData(mtl, meshType, forceMaterial);
      if (curMat)
      {		   
         addMaterial(curMat);
      }//if (curMat)
	}//if (mtl && (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
	else if (mtl && (mtl->ClassID() == Class_ID(MULTI_CLASS_ID, 0)))
   {  //OK, need to get the multiple materials that apply to this object 
		for (int i=0; i < mtl->NumSubMtls(); i++)
		{
			Mtl* subMtl = mtl->GetSubMtl(i);
			if (subMtl) 
			{	
            curMat = getStandardMaterialData(subMtl, meshType, forceMaterial);
            if (curMat)
            {				   
               addMaterial(curMat);
            }//if (curMat)
			}//if (subMtl)
		}//for (i=0; i<mtl->NumSubMtls(); i++)
	}//else if (mtl && (mtl->ClassID() == Class_ID(MULTI_CLASS_ID, 0)))
   return getNumMaterials();
}//void TnTExprt::getMaterialData(INode* n, int& meshType, AWBoolean forceMaterial)
