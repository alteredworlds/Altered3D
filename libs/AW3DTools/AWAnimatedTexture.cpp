#include "AWAnimatedTexture.h"

#define VER_GLANIMTEXTURE_1	100
#define VER_GLANIMTEXTURE_2	104
 
AWDEFINE_VERSIONABLE_PERSISTABLE(AWAnimatedTexture, CLSID_AWGLANIMTEXTURE, VER_GLANIMTEXTURE_2);


AWAnimatedTexture::AWAnimatedTexture(const AWAnimatedTexture& other)
{  //copy automatic member data
	m_firstFrame = other.m_firstFrame;
	m_lastFrame  = other.m_lastFrame;
	m_curFrame   = other.m_curFrame;
	m_frameStep  = other.m_frameStep;
	//copy over textures
}//AWAnimatedTexture::AWAnimatedTexture(const AWAnimatedTexture& other)


AWAnimatedTexture::~AWAnimatedTexture()
{
	m_frames.destroy();
}



void				 
AWAnimatedTexture::addTexture(AWTexture* tex)
{  //ADD NEW TEXTURE (FRAME)
   //OWNED BY THIS AWANIMATEDTEXTURE
	m_frames.add(tex);
}//void AWAnimatedTexture::addTexture(AWTexture* tex)


int					 
AWAnimatedTexture::getNumFrames()
{
	return m_frames.getNum();
}



float       
AWAnimatedTexture::getAnimationDuration()
{
    return getFrameStep() * getNumFrames();
}//float  AWAnimatedTexture::getAnimationDuration()



//TEXTURE OBJECT ACCESS METHODS
AWTexture*						 
AWAnimatedTexture::getTexture(float time)
{
	AWTexture* retVal = NULL;
	AWTexture* curTex = NULL; 
	//should do a binary search, but just use sequential for now.
	//probably not too many items in the collection anyway,
	//so may not notice much improvement in any case.
	//this code can be improved - for now work out what the 
	//maximum texture time value is by looking rather than
	//storing it as member data
	int num = 0;
	if (num = m_frames.getNum())
	{
		float maxTime = ((AWTexture*)m_frames[num-1])->getTime();
		
		if (time > maxTime + m_frameStep)
		{
			time = (float)fmod(time, (maxTime + m_frameStep));
		}
		for (int i = 0; i < num; i++)
		{
			curTex = (AWTexture*)m_frames[i];
			if (curTex->getTime() <= time)
			{   //either the key matches, or is at time before frame
				//so this is the current candidate for the result
				retVal = curTex;
			}
			else
			{			//we've gone past it - these keys are now valid AFTER
				break;	//the supplied 'frame', but not yet.
			}
		}//for (int i = 0; i < (int)m_frame.entries(); i++)
	}//if (num = m_frames.entries())
	return retVal;
}//AWTexture*	AWAnimatedTexture::getTexture(float time)


AWTexture*						 
AWAnimatedTexture::getTexFromArrayIndex(int index)
{
	return (AWTexture*)m_frames[index];
}



int          
AWAnimatedTexture::init()
{
  for (int i = 0; i < m_frames.getNum(); i++)
  {
	  ((AWTexture*)m_frames[i])->init();
  }
  return 1;
}//int AWAnimatedTexture::init()



void 	
AWAnimatedTexture::reSize(int xsize, int ysize)
{
  for (int i = 0; i < m_frames.getNum(); i++)
  {
	  ((AWTexture*)m_frames[i])->reSize(xsize, ysize);
  }
}//void AWAnimatedTexture::reSize(int xsize, int ysize)



//////////////////////////////////////////////////////////
//
//AWPersistable virtual persistence functions
//
//for class:  AWAnimatedTexture
//
//////////////////////////////////////////////////////////
void	
AWAnimatedTexture::restoreGuts(AWFile& file)
{
	unsigned int version;
	restoreVersion(file, &version);   // Get the saved version #
	if (version <= VER_GLANIMTEXTURE_2)
	{
		AWTexture::restoreGuts(file);
		file.Read(m_firstFrame);
		file.Read(m_curFrame);	
		file.Read(m_frameStep);
		file.Read(m_lastFrame);
      m_frames.read(file, version < VER_GLANIMTEXTURE_2);
	}//if (VER_GLTEXTURE_2 == version)
}//void	AWAnimatedTexture::restoreGuts(AWFile& file)




void        
AWAnimatedTexture::saveGuts(AWFile& file) const
{
	saveVersion(file, uVersion_ );	// version number
	AWTexture::saveGuts(file);
	file.Write(m_firstFrame);
	file.Write(m_curFrame);
	file.Write(m_frameStep);
	file.Write(m_lastFrame);
	file << m_frames;
}//void AWAnimatedTexture::saveGuts(AWFile& file) const




AWspace     
AWAnimatedTexture::binaryStoreSize() const
{
	AWspace retVal = sizeof uVersion_ +
		AWTexture::binaryStoreSize() +
		sizeof (m_firstFrame) +	
		sizeof (m_curFrame) +  
		sizeof(m_frameStep) +
		sizeof(m_lastFrame) +
		m_frames.getStoreSize();
	return retVal;
}//AWspace AWAnimatedTexture::binaryStoreSize() const