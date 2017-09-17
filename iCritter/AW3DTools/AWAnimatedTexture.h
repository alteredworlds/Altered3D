#ifndef __AW_GL_ANIMATED_TEXTURE_H
#define __AW_GL_ANIMATED_TEXTURE_H

#include "AWToolsCommon.h"
#include "AWTexture.h"
#include "AWOrdered.h"


class AWExport AWAnimatedTexture : public AWTexture
{
public:
   AWDECLARE_VERSIONABLE_PERSISTABLE(AWAnimatedTexture);

   AWAnimatedTexture(const AWCString& name = "", const AWCString& fileName = "") 
      : AWTexture(name, fileName), m_firstFrame(0), m_lastFrame(0),
         m_curFrame(0), m_frameStep(0), m_frames(5){;}

   AWAnimatedTexture(const AWAnimatedTexture& other);

   ~AWAnimatedTexture();

   virtual void			compact();
   virtual int          init();

//VIRTAL FRAME ACCESS METHODS
	virtual void			calc(double time, AWBoolean loop=TRUE);
	virtual AWTexture*	getTexture(double time);
	virtual int				getNumFrames();
	AWTexture*			   getTexFromArrayIndex(int index);

//TEXTURE OBJECT ACCESS METHODS	
	void						addTexture(AWTexture* tex);

//FRAME ACCESS METHODS
   virtual float        getAnimationDuration();
   const int&		      getFirstFrame()					{return m_firstFrame;}
   const int&		      getLastFrame()					   {return m_lastFrame;}
   void			         setFirstFrame(int firstFrame)	{m_firstFrame=firstFrame;}
   void			         setLastFrame(int lastFrame)	{m_lastFrame=lastFrame;}
   void			         setCurrentFrame(int f)			{m_curFrame=f;}
   const float&	      getFrameStep()					   {return m_frameStep;}
   void			         setFrameStep(float step)		{m_frameStep=step;}
//OVERRIDES OF BASE CLASS
	void 			         reSize(int xsize, int ysize);
//AWPersistable VIRTUALS
	virtual void         restoreGuts(AWFile& file);
   virtual void         saveGuts(AWFile& file) const;

	virtual AWspace      binaryStoreSize() const;

    //the set of child AWTexture objects that make up the animation frames
	AWOrdered	         m_frames;
//MEMBER DATA
protected:
	int				      m_firstFrame;
	int				      m_lastFrame;
	int				      m_curFrame;
	float			         m_frameStep;	//interval in seconds between anim. frames
};//class AWExport AWAnimatedTexture : public AWTexture


inline void 
AWAnimatedTexture::compact()
{
	m_frames.pack();
}//void AWAnimatedTexture::compact()


inline void
AWAnimatedTexture::calc(double time, AWBoolean loop)
{
   m_texID = -1; //default indicates NO VALID TEXTURE
   AWTexture* tex = getTexture(time);
   if (tex) m_texID = tex->m_texID;
   AWTRACEHI2("AWAnimatedTexture::getTextureID(%f) using texID %d\n", (double)time, (int)m_texID);
}//void AWAnimatedTexture::calc(float time)


#endif //sentinel __AW_GL_ANIMATED_TEXTURE_H