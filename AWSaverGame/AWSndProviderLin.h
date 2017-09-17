#ifndef __AWSNDPROVIDERLIN_H
#define __AWSNDPROVIDERLIN_H

#include "AWSound.h"

#define AW_NUM_EFFECTS 4

class  AWSoundProviderLin
{	
public:
	AWSoundProviderLin();
	virtual           ~AWSoundProviderLin();

	virtual int	      init();
	virtual int	      shutDown();

	virtual int       loadMusic(const AWCString& waveFile);
	virtual int       loadWave(const AWCString& waveFile);

	virtual int       playMusic(int loops =0);
	virtual int       stopMusic();
   virtual int       pauseMusic();
   virtual int       resumeMusic();
   virtual int       isMusicActive();
	
	virtual int       play(int soundID=0, int loops = 0);
	virtual int       stop(int soundID=-1);
   virtual int       pause(int soundID=-1);
   virtual int       resume(int soundID=-1);
   virtual int       isActive(int soundID);


protected:	
   AWBoolean         m_audioOpen;
   AWSound           m_music;
   AWSound           m_sounds[AW_NUM_EFFECTS];

   int               isValidSndID(int v) {return (v >= 0) && (v < AW_NUM_EFFECTS);}

   int               m_audioRate;
   int               m_audioFormat;
	int               m_audioChannels;
	int               m_audioBuffers;
	int					m_externalMusicPlayer;
};//class  AWSoundProviderLin : public AWSoundProvider


#endif //sentinel __AWSNDPROVIDERLIN_H


