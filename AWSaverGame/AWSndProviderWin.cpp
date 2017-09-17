
#include "AWSndProviderWin.h"
#include <qmdx/qmdx.h>


//need to get the following functions dynamically fro QMDX.dll
//to allow for OPTIONAL use depending on discovery
//
//if not found - GAME sound (only) is disabled...

typedef HQMIXER  (WINAPI * PFNQMDXINITEXPROC) (LPQMIXCONFIG lpConfig);
typedef MMRESULT (WINAPI * PFNQMDXACTIVATEPROC) (HQMIXER hQMixer, BOOL fActivate);
typedef MMRESULT (WINAPI * PFNQMDXOPENCHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags);
typedef MMRESULT (WINAPI * PFNQMDXCONFIGURECHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags, LPCWAVEFORMATEX format,  LPVOID lpvOptions);
typedef MMRESULT (WINAPI * PFNQMDXSETVOLUMEPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags, int volume);
typedef MMRESULT (WINAPI * PFNQMDXSTOPCHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags);
typedef MMRESULT (WINAPI * PFNQMDXCLOSECHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags);
typedef MMRESULT (WINAPI * PFNQMDXFREEWAVEPROC) (HQMIXER hQMixer, LPMIXWAVE lpMixWave);
typedef MMRESULT (WINAPI * PFNQMDXCLOSESESSIONPROC) (HQMIXER hQMixer);
typedef LPMIXWAVE(WINAPI * PFNQMDXOPENWAVEPROC) (HQMIXER hQMixer, LPSTR szWaveFilename,  HINSTANCE hInst, DWORD dwFlags);
typedef BOOL     (WINAPI * PFNQMDXISCHANNELDONEPROC) (HQMIXER hQMixer, int iChannel);
typedef MMRESULT (WINAPI * PFNQMDXSETDISTANCEMAPPINGPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags, const LPQMIX_DISTANCES lpDistances);
typedef int      (WINAPI * PFNQMDXFINDCHANNELPROC) (HQMIXER hQMixer, DWORD dwFlags, const QMIX_CHANNEL_PARAMS* lpParams);
typedef MMRESULT (WINAPI * PFNQMDXPLAYEXPROC) (HQMIXER hQMixer, int iChannel,  DWORD dwFlags, LPMIXWAVE lpMixWave, int iLoops, LPQMIXPLAYPARAMS lpParams);
typedef MMRESULT (WINAPI * PFNQMDXPAUSECHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags);
typedef MMRESULT (WINAPI * PFNQMDXRESTARTCHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags);
typedef MMRESULT (WINAPI * PFNQMDXFLUSHCHANNELPROC) (HQMIXER hQMixer, int iChannel, DWORD dwFlags);


PFNQMDXINITEXPROC             awQMDX_InitEx              = NULL;
PFNQMDXACTIVATEPROC           awQMDX_Activate            = NULL;
PFNQMDXOPENCHANNELPROC        awQMDX_OpenChannel         = NULL;
PFNQMDXCONFIGURECHANNELPROC   awQMDX_ConfigureChannel    = NULL;
PFNQMDXSETVOLUMEPROC          awQMDX_SetVolume           = NULL;
PFNQMDXSTOPCHANNELPROC        awQMDX_StopChannel         = NULL;
PFNQMDXCLOSECHANNELPROC       awQMDX_CloseChannel        = NULL;
PFNQMDXFREEWAVEPROC           awQMDX_FreeWave            = NULL;
PFNQMDXCLOSESESSIONPROC       awQMDX_CloseSession        = NULL;
PFNQMDXOPENWAVEPROC           awQMDX_OpenWave            = NULL;
PFNQMDXISCHANNELDONEPROC      awQMDX_IsChannelDone       = NULL;
PFNQMDXSETDISTANCEMAPPINGPROC awQMDX_SetDistanceMapping  = NULL;
PFNQMDXFINDCHANNELPROC        awQMDX_FindChannel         = NULL;
PFNQMDXPLAYEXPROC             awQMDX_PlayEx              = NULL;
PFNQMDXPAUSECHANNELPROC       awQMDX_PauseChannel        = NULL;
PFNQMDXRESTARTCHANNELPROC     awQMDX_RestartChannel      = NULL;
PFNQMDXFLUSHCHANNELPROC       awQMDX_FlushChannel        = NULL;


void
AWSoundProviderWin::getFunctions()
{  //DLL name is: QMDX.dll
   m_hInst = LoadLibrary("QMDX.dll");
   if (m_hInst)
   {
      awQMDX_InitEx = (PFNQMDXINITEXPROC)GetProcAddress(m_hInst, "QMDX_InitEx");
      awQMDX_Activate = (PFNQMDXACTIVATEPROC)GetProcAddress(m_hInst, "QMDX_Activate");
      awQMDX_OpenChannel = (PFNQMDXOPENCHANNELPROC)GetProcAddress(m_hInst, "QMDX_OpenChannel");
      awQMDX_ConfigureChannel = (PFNQMDXCONFIGURECHANNELPROC)GetProcAddress(m_hInst, "QMDX_ConfigureChannel");     
      awQMDX_SetVolume = (PFNQMDXSETVOLUMEPROC)GetProcAddress(m_hInst, "QMDX_SetVolume");
      awQMDX_StopChannel = (PFNQMDXSTOPCHANNELPROC)GetProcAddress(m_hInst, "QMDX_StopChannel");
      awQMDX_CloseChannel = (PFNQMDXCLOSECHANNELPROC)GetProcAddress(m_hInst, "QMDX_CloseChannel");
      awQMDX_FreeWave = (PFNQMDXFREEWAVEPROC)GetProcAddress(m_hInst, "QMDX_FreeWave");
      awQMDX_CloseSession = (PFNQMDXCLOSESESSIONPROC)GetProcAddress(m_hInst, "QMDX_CloseSession");
      awQMDX_OpenWave = (PFNQMDXOPENWAVEPROC)GetProcAddress(m_hInst, "QMDX_OpenWave");
      awQMDX_IsChannelDone = (PFNQMDXISCHANNELDONEPROC)GetProcAddress(m_hInst, "QMDX_IsChannelDone");   
      awQMDX_SetDistanceMapping = (PFNQMDXSETDISTANCEMAPPINGPROC)GetProcAddress(m_hInst, "QMDX_SetDistanceMapping");
      awQMDX_FindChannel = (PFNQMDXFINDCHANNELPROC)GetProcAddress(m_hInst, "QMDX_FindChannel");
      awQMDX_PlayEx = (PFNQMDXPLAYEXPROC)GetProcAddress(m_hInst, "QMDX_PlayEx");
      awQMDX_PauseChannel = (PFNQMDXPAUSECHANNELPROC)GetProcAddress(m_hInst, "QMDX_PauseChannel");
      awQMDX_RestartChannel = (PFNQMDXRESTARTCHANNELPROC)GetProcAddress(m_hInst, "QMDX_RestartChannel");
      awQMDX_FlushChannel = (PFNQMDXFLUSHCHANNELPROC)GetProcAddress(m_hInst, "QMDX_FlushChannel");
   }//if (m_hInst)
}//void AWSoundProviderWin::getFunctions()
 

//AWSoundProvider acts as a simple cross platform Sound
//library with minimal functionality

AWSoundProviderWin::AWSoundProviderWin() :
    m_hQMixer(NULL), m_hInst(NULL)
{
   getFunctions();
}


AWSoundProviderWin::~AWSoundProviderWin()
{
   shutDown();
   if (m_hInst)
   {
      FreeLibrary(m_hInst);
      m_hInst = NULL;
   }
}


int	
AWSoundProviderWin::init()
{
	int retVal   = 1; //indicates failure
    int channels = 8;

    QMIXCONFIG config;
    memset(&config, 0, sizeof(config));
    config.dwSize    = sizeof(config);
    config.iChannels = channels;

    m_hQMixer = awQMDX_InitEx ? awQMDX_InitEx(&config) : NULL;
	if (m_hQMixer && awQMDX_Activate && awQMDX_OpenChannel && 
       awQMDX_ConfigureChannel && awQMDX_SetVolume) 
	{
      retVal =0;
      awQMDX_Activate((HQMIXER)m_hQMixer, TRUE);
      awQMDX_OpenChannel((HQMIXER)m_hQMixer, 0, QMIX_OPENALL);
      //make sure all channels are 2D only for the sake of speed on NT4
      //we are sticking to the default 8 channels
      for (int channel=0; channel < 8; channel++)
      {
         awQMDX_ConfigureChannel((HQMIXER)m_hQMixer, channel, 
                                 QMIX_CHANNELTYPE_ANYMIXER|QMIX_CHANNELTYPE_2D,0 ,0);
      }
      awQMDX_SetVolume((HQMIXER)m_hQMixer, 0, QMIX_ALL, 25000);
	}
	return retVal;
}


int	
AWSoundProviderWin::shutDown()
{
   if (m_hQMixer && awQMDX_StopChannel && 
       awQMDX_CloseChannel && awQMDX_FreeWave && awQMDX_CloseSession)
   {
      awQMDX_StopChannel((HQMIXER)m_hQMixer, 0, QMIX_ALL);
      awQMDX_CloseChannel((HQMIXER)m_hQMixer, 0, QMIX_ALL);
      for (int snd =0; snd < m_sounds.size(); snd++)
      {
         awQMDX_FreeWave((HQMIXER)m_hQMixer, (LPMIXWAVE)m_sounds[snd].m_pSnd);
         m_sounds[snd].m_pSnd    = NULL;
      }
      awQMDX_CloseSession((HQMIXER)m_hQMixer);
      m_hQMixer = NULL;
   }
	return 0;
}//int	AWSoundProviderWin::shutDown()



int 
AWSoundProviderWin::loadWave(const AWCString& waveFile, int ID)
{
    int retVal = -1;
	AWSound snd;
	snd.m_ID	= ID;
	snd.m_name  = waveFile;
	snd.m_pSnd  = awQMDX_OpenWave?
      awQMDX_OpenWave((HQMIXER)m_hQMixer, (char*)waveFile.c_str(), 0, WMIX_FILE):NULL;
   //this is a value based collection  - inserts a copy
    if (snd.m_pSnd)
    {
        retVal = ID;
        m_sounds.push_back(snd);
    }
	return retVal;
}//int AWSoundProviderWin::loadWave(const AWCString& waveFile, int ID)



int
AWSoundProviderWin::playSync(int soundID, int loops)
{
    int channelID = play(soundID, loops);
    while ((channelID != -1) && m_hQMixer && awQMDX_IsChannelDone && !awQMDX_IsChannelDone((HQMIXER)m_hQMixer, channelID)) 
    {
       Sleep(100);
    }
    return channelID;
}//int AWSoundProviderWin::playSync(int soundID, int loops)



int    
AWSoundProviderWin::setPos(int soundID, const AWVector& v)
{
   int retVal = 0;
   for (int i=0; i< m_sounds.size(); i++)
   {
      if (m_sounds[i].m_ID == soundID)
      {
         m_sounds[i].m_pos3.x=v.x;
         m_sounds[i].m_pos3.y=-v.z;
         m_sounds[i].m_pos3.z=v.y;
         retVal = 1;
         break;
      }//if (m_sounds[i].m_ID == soundID)
   }//for (int i=0; i< m_sounds.size(); i++)
   return retVal;
}//int AWSoundProviderWin::setPos(int soundID, const AWVector& v)



int    
AWSoundProviderWin::setDist(int soundID, float dist)
{
   int retVal = 0;
   for (int i=0; i< m_sounds.size(); i++)
   {
      if (m_sounds[i].m_ID == soundID)
      {
         m_sounds[i].m_pos3.x=0.0f;
         m_sounds[i].m_pos3.y=dist;
         m_sounds[i].m_pos3.z=0.0f;
         retVal = 1;
         break;
      }//if (m_sounds[i].m_ID == soundID)
   }//for (int i=0; i< m_sounds.size(); i++)
   return retVal;
}

#define AWMIN_DIST 200.0f
#define AWMAX_DIST 600.0f
#define AWSND_SCALE 1.0f
#define AWMAX_VOL 32767.0f
#define AWMIN_VOL 100.0f

void
AWSoundProviderWin::setDistanceMapping(int channelID)
{  //boost the exlosion volume
   //QMDX_SetVolume(m_hQMixer, channelID, QMIX_USEONCE|QMIX_IMMEDIATE, AWMAX_VOL);

   QMIX_DISTANCES dist;
   dist.cbSize=sizeof(QMIX_DISTANCES); 	// size of structure (needed for future versions)
   dist.minDistance = AWMIN_DIST; 	// sounds are at full volume if closer than this
   dist.maxDistance = AWMAX_DIST; 	// sounds are muted if further away than this
   dist.scale       = AWSND_SCALE;	// relative amount to adjust rolloff 
   if (awQMDX_SetDistanceMapping)
   {
      awQMDX_SetDistanceMapping((HQMIXER)m_hQMixer, channelID, QMIX_USEONCE|QMIX_IMMEDIATE, &dist);  
   }
   //AWTRACE2("Channel %d volume %d", channelID, (int)dist);
}//void AWSoundProviderWin::setDistanceMapping(int channelID)



int
AWSoundProviderWin::positionSound(int channelID, AWVector& pos3)
{
   if (pos3.x==0.0f && pos3.y == 0.0f && pos3.z == 0.0f) return 0;
   //setDistanceMapping(channelID);
   AWTRACE3("   Setting position (%f, %f, %f)\n", (double)pos3.x, (double)pos3.y, (double)pos3.z);
   //QMDX_SetSourcePosition(m_hQMixer, channelID, QMIX_USEONCE|QMIX_IMMEDIATE, (const struct QSVECTOR*)&pos3);
    
   float explodeVol = 0.0f;
   float dist = pos3.y - AWMIN_DIST;
   if (dist <=0)
   {
      explodeVol = AWMAX_VOL;
   }
   else if (dist >= AWMAX_DIST)
   {
      explodeVol = AWMIN_VOL;
   }
   else
   {  //value lies between MIN_DIST and MAX_DIST
      //scale volume accordingly
      explodeVol =  (float)AWMAX_VOL * ( ((float)AWMAX_DIST- dist) / (AWMAX_DIST - AWMIN_DIST) );
      explodeVol += AWMIN_VOL;
   }
   AWTRACE1("    explodeVol is %d\n", explodeVol);
   if (awQMDX_SetVolume)
   {
      awQMDX_SetVolume((HQMIXER)m_hQMixer, channelID, QMIX_USEONCE|QMIX_IMMEDIATE, explodeVol);
   }
   return 1;
}//int AWSoundProviderWin::positionSound(int channelID, AWVector& pos3)



int	
AWSoundProviderWin::play(int soundID, int loops, void* hwnd)
{
   AWTRACE3("SoundProvider::play(%i, %i, %i)\n", soundID, loops, (int)hwnd);
   int channelID = -1;
	if (m_hQMixer)
	{
		for (int i=0; i< m_sounds.size(); i++)
		{
         AWSound& next = m_sounds.at(i);
			if ((next.m_ID == soundID) && next.m_pSnd)
			{
            AWTRACE1("   Found valid soundID %d - ", soundID);
            //if (m_sounds[i].m_channelID != -1)
            //{
              // DWORD channelStatus = QMDX_GetChannelStatus(m_hQMixer, m_sounds[i].m_channelID);
               //AWTRACE1(" was playing on channel %d - status:\n", m_sounds[i].m_channelID);
               //AWTRACE2("   OPEN %d  QUEUED %d", channelStatus & QMIX_CHANNELSTATUS_OPEN, channelStatus & QMIX_CHANNELSTATUS_QUEUED);
               //AWTRACE2(" PLAYING %d  PAUSED %d\n", channelStatus &  QMIX_CHANNELSTATUS_PLAYING, channelStatus & QMIX_CHANNELSTATUS_PAUSED);
            //}
				channelID =  awQMDX_FindChannel?
               awQMDX_FindChannel((HQMIXER)m_hQMixer, QMIX_FINDCHANNEL_LRU, 0):NULL;
            if (channelID >= 0) 
				{
               AWTRACE1("assigning channel %d\n", channelID);
               next.m_channelID = channelID;
               QMIXPLAYPARAMS params;
               memset(&params, 0, sizeof(params));
               params.dwSize = sizeof(params);
               params.hwndNotify = (HWND)hwnd;
               //relies on AWVector === QS_VECTOR
               //positionSound(channelID, next.m_pos3);
               if (awQMDX_PlayEx)
               {
					   awQMDX_PlayEx((HQMIXER)m_hQMixer, channelID, QMIX_CLEARQUEUE, 
                           (LPMIXWAVE)next.m_pSnd, loops, &params);
               }
				}//if (channelID >= 0) 

				break;
			}//if (next.m_ID == soundID)
		}//for (int i=0; i< m_sounds.entries(); i++)
	}//if (m_hQMixer)
	return channelID;
}//int AWSoundProviderWin::play(int soundID, AWBoolean looped, void* hwnd)


int 
AWSoundProviderWin::isActive(int soundID)
{
    int retVal = 0;
    for (int i=0; i< m_sounds.size(); i++)
	{
        if ((m_sounds[i].m_ID == soundID) && 
            (m_sounds[i].m_channelID != -1))
        {
            retVal = 1; 
            break;
        }
    }
    return retVal;
}//int AWSoundProviderWin::isActive(int soundID)



int 
AWSoundProviderWin::pause(int soundID)
{
    if (m_hQMixer)
	{
      if (awQMDX_PauseChannel && (soundID == -1))
      {
         awQMDX_PauseChannel((HQMIXER)m_hQMixer, 1, QMIX_ALL);
      }
      else
      {
         for (int i=0; i< m_sounds.size(); i++)
         {
            if (awQMDX_PauseChannel && (m_sounds[i].m_ID == soundID))
            {
               awQMDX_PauseChannel((HQMIXER)m_hQMixer, m_sounds[i].m_channelID, 0);
               break;
            }//if (sounds[i].m_ID == soundID)
         }//for (int i=0; i< m_sounds.entries(); i++)
      }//else
	}//if (m_hQMixer)
	return 0;
}//int AWSoundProviderWin::pause(int soundID)


int 
AWSoundProviderWin::resume(int soundID)
{
    if (m_hQMixer)
	{
      if (awQMDX_RestartChannel && (soundID == -1))
      {
         awQMDX_RestartChannel((HQMIXER)m_hQMixer, 1, QMIX_ALL);
      }
      else
      {
         for (int i=0; i< m_sounds.size(); i++)
         {
            if (awQMDX_RestartChannel && (m_sounds[i].m_ID == soundID))
            {
               awQMDX_RestartChannel((HQMIXER)m_hQMixer, m_sounds[i].m_channelID, 0);
               break;
            }//if (sounds[i].m_ID == soundID)
         }//for (int i=0; i< m_sounds.entries(); i++)
      }
	}//if (m_hQMixer)
	return 0;
}//int AWSoundProviderWin::resume(int soundID)



int	
AWSoundProviderWin::stop(int soundID)
{
	if (m_hQMixer)
	{
        if (awQMDX_FlushChannel && (soundID == -1))
        {
            awQMDX_FlushChannel((HQMIXER)m_hQMixer, 0, QMIX_ALL);
        }//if (awQMDX_FlushChannel && (soundID == -1))
        else
        {
		    for (int i=0; i< m_sounds.size(); i++)
		    {
			    if (awQMDX_FlushChannel && (m_sounds[i].m_ID == soundID) && 
                    (m_sounds[i].m_channelID != -1))
			    {
				    awQMDX_FlushChannel((HQMIXER)m_hQMixer, m_sounds[i].m_channelID, 0);
                    m_sounds[i].m_channelID = -1;
				    break;
			    }//if (sounds[i].m_ID == soundID)
		    }//for (int i=0; i< m_sounds.entries(); i++)
        }//else
	}//if (m_hQMixer)
	return 0;
}//int AWSoundProviderWin::stop(int soundID)
