#include "AWSndProviderLin.h"
#include "SDL.h"
#include "SDL_mixer.h"



AWSoundProviderLin::AWSoundProviderLin() :
    m_audioOpen(FALSE)
{
   m_audioRate     = 22050;
   m_audioFormat   = AUDIO_S16;
	m_audioChannels = 2;
	m_audioBuffers  = 4096;
}


AWSoundProviderLin::~AWSoundProviderLin()
{
   shutDown();
}



int
AWSoundProviderLin::init()
{
	if (m_audioOpen) return m_audioOpen;
   //we assume that SDL_Init(SDL_INIT_AUDIO) already called
   if (Mix_OpenAudio(m_audioRate, m_audioFormat, m_audioChannels, m_audioBuffers) < 0)
   {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		m_audioOpen = FALSE;
	}
	else
	{

	   Uint16 test;
		Mix_QuerySpec(&m_audioRate, &test, &m_audioChannels);
		m_audioFormat = test;
		printf("Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", m_audioRate,
			(m_audioFormat&0xFF),
			(m_audioChannels > 1) ? "stereo" : "mono",
		   m_audioBuffers );
		m_audioOpen = TRUE;
	}
	return m_audioOpen;
}//int AWSoundProviderLin::init()


int
AWSoundProviderLin::shutDown()
{
   if (m_audioOpen && Mix_PlayingMusic())
	{
	   Mix_FadeOutMusic(100);
	   SDL_Delay(100);
	}
	//also need to clear up sounds
	//with SDL_FreeMusic SDL_FreeChunk
	if (m_music.m_pSnd)
	{
	   Mix_FreeMusic((Mix_Music*)m_music.m_pSnd);
	   m_music.m_pSnd = NULL;
	}
	for (int i =0; i < AW_NUM_EFFECTS; i++)
	{
	   if (m_sounds[i].m_pSnd)
	   {
	      if (isActive(i))
	      {
	         stop(i);
	      }
	      Mix_FreeChunk((Mix_Chunk*)m_sounds[i].m_pSnd);
	      m_sounds[i].m_pSnd = NULL;
	   }
	}
	if (m_audioOpen)
	{
	   Mix_CloseAudio();
	   m_audioOpen = FALSE;
	}
	return 0;
}//int	AWSoundProviderLin::shutDown()


int
AWSoundProviderLin::loadMusic(const AWCString& waveFile)
{
   int retVal = -1; //fail
   if (m_music.m_pSnd)
   {
      //already loaded music - not supposed to happen
      fprintf(stderr, "Music Already Loaded...\n");
      return retVal;
   }

   void* soundData = (void*)Mix_LoadMUS((char*)waveFile.c_str());
   if (soundData)
   {
	   m_music.m_ID	 = 0;
	   m_music.m_name  = waveFile;
	   m_music.m_pSnd  = soundData;
	   m_music.m_channelID = 0;
	   retVal = 1;
   }
	return retVal;
}//int AWSoundProviderLin::loadMusic(const AWCString& musicFile)


int
AWSoundProviderLin::loadWave(const AWCString& waveFile)
{
   int sndIndex = -1;
   //find the next available effect slot.
   //return if there isn't one
   for (int i=0; i<AW_NUM_EFFECTS; i++)
   {
      if (NULL == m_sounds[i].m_pSnd)
      {
         sndIndex = i; break;
      }
   }//for (int i=0; i<AW_NUM_EFFECTS; i++)
   if (-1 != sndIndex)
   {  //OK, do this shit
      void* soundData = (void*)Mix_LoadWAV((char*)waveFile.c_str());
      if (soundData)
      {
	      m_sounds[sndIndex].m_ID	   = sndIndex;
	      m_sounds[sndIndex].m_name  = waveFile;
	      m_sounds[sndIndex].m_pSnd  = soundData;
      }
      else
      {
         fprintf(stderr, "AlteredSaver FAILED to load wave file %s\n", waveFile.c_str());
         sndIndex = -1;
      }
   }
	return sndIndex;
}//int AWSoundProviderLin::loadWave(const AWCString& waveFile)


int
AWSoundProviderLin::stop(int soundID)
{
   if (m_audioOpen && isValidSndID(soundID)  && m_sounds[soundID].m_pSnd)
   {
      Mix_HaltChannel(m_sounds[soundID].m_channelID);
      m_sounds[soundID].m_channelID = 0;
   }
   return 1;
}//int AWSoundProviderLin::stop(int soundID)


int
AWSoundProviderLin::play(int soundID, int loops)
{
   if (m_audioOpen &&  isValidSndID(soundID) && m_sounds[soundID].m_pSnd)
   {
      m_sounds[soundID].m_channelID = Mix_PlayChannelTimed(-1, (Mix_Chunk*)m_sounds[soundID].m_pSnd, loops, -1);
   }
   return 1;
}//int AWSoAWSoundProviderund::play(int soundID, int loops)


int
AWSoundProviderLin::pause(int soundID)
{
//fprintf(stderr, "\ncpause\n  commandID: % i, soundID: %i\n",
//               AWSND_PAUSE, soundID);
   if (m_audioOpen)
   {
      if (isValidSndID(soundID) && m_sounds[soundID].m_pSnd)
      {
         Mix_Pause(m_sounds[soundID].m_channelID);
      }
      else if (-1 == soundID)
      {
         Mix_Pause(soundID);
         Mix_PauseMusic();
      }
   }
   return 1;
}//int AWSoundProviderLin::pauseLinuxSnd(int soundID)


int
AWSoundProviderLin::resume(int soundID)
{
//fprintf(stderr, "\ncpause\n  commandID: % i, soundID: %i\n",
//               AWSND_RESUME, soundID);
   if (m_audioOpen)
   {
      if (isValidSndID(soundID) && m_sounds[soundID].m_pSnd)
      {
         Mix_Resume(m_sounds[soundID].m_channelID);
      }
      else if (-1 == soundID)
      {
         Mix_Resume(soundID);
         Mix_ResumeMusic();
      }
   }
   return 1;
}//int AWSoundProviderLin::resumeLinuxSnd(int soundID)


int
AWSoundProviderLin::isActive(int soundID)
{
   int retVal = 0;
   if (m_audioOpen && isValidSndID(soundID) && m_sounds[soundID].m_pSnd)
   {
      retVal = Mix_Playing(m_sounds[soundID].m_channelID);
   }
   return retVal;
}//int AWSoundProviderLin::isActive(int soundID)

//MUSIC COMMANDS _ YUK!
int
AWSoundProviderLin::stopMusic()
{
   if (m_audioOpen && m_music.m_pSnd)
   {
      Mix_HaltMusic();
   }
   return 1;
}//int AWSoundProviderLin::stop(int soundID)


int
AWSoundProviderLin::playMusic(int loops)
{
   if (m_audioOpen && m_music.m_pSnd)
   {
      Mix_PlayMusic((Mix_Music*)m_music.m_pSnd, loops);
	}
   return 1;
}//int AWSoAWSoundProviderund::play(int soundID, int loops)


int
AWSoundProviderLin::pauseMusic()
{
   if (m_audioOpen && m_music.m_pSnd)
   {
      Mix_PauseMusic();
   }
   return 1;
}//int AWSoundProviderLin::pauseLinuxSnd(int soundID)


int
AWSoundProviderLin::resumeMusic()
{
   if (m_audioOpen && m_music.m_pSnd)
   {
      Mix_ResumeMusic();
   }
   return 1;
}//int AWSoundProviderLin::resumeMusic()


int
AWSoundProviderLin::isMusicActive()
{
   int retVal = 0;
   if (m_audioOpen && m_music.m_pSnd)
   {
      retVal = Mix_PlayingMusic();
   }
   return retVal;
}//int AWSoundProviderLin::isActive(int soundID)













