#ifndef __AWSNDPROVIDERWIN_H
#define __AWSNDPROVIDERWIN_H


#include <vector>
#include <windows.h>
#include "AWSound.h"

//to re-enable game sound uncomment the following line -

class AWSoundProviderWin
{	
public:
	AWSoundProviderWin();
	virtual           ~AWSoundProviderWin();

	virtual int	      init();
	virtual int	      shutDown();

	virtual int       loadWave(const AWCString& waveFile, int ID);
	virtual int       play(int soundID=0, int loops = 0, void* hwnd = NULL);
   virtual int       playSync(int soundID, int loops = 0);
	virtual int       stop(int soundID=-1);
   virtual int       pause(int soundID=-1);
   virtual int       resume(int soundID=-1);
   virtual int       isActive(int soundID);

   virtual int       setPos(int soundID, const AWVector& v);
   virtual int       setDist(int soundID, float dist=0.0f);

   void              setDistanceMapping(int channelID);
   int               positionSound(int channelID, AWVector& pos3);

protected:
   void              getFunctions();

   void*             m_hQMixer;
	vector<AWSound>   m_sounds;
   HINSTANCE         m_hInst;
};//class  AWSoundProviderWin : public AWSoundProvider


#endif //sentinel __AWSNDPROVIDERWIN_H