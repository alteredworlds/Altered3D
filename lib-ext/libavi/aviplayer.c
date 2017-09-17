#include <windows.h>
#include <dsound.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "libavi.h"
#include "aviplayer.h"

#define BITS_PER_SAMPLE 16
#define BUFFER_LENGTH 4

const char *
aviplayer_get_error_string(AVIPLAYER *avp)
{
  switch(avp->err) {
  case ERR_AVI:
    return avi_get_error_string(avp->avidata);
  case ERR_CREATESOUNDBUFFER:
    return "Error creating DirectSound buffer";
  default:
    return "Something strange is afoot";
  }
}

int
aviplayer_open_audio(AVIPLAYER *avp, LPDIRECTSOUND lpdsound)
{
  DSBUFFERDESC dsbd;
  WAVEFORMATEX wfx;
  int channels;
  int sps;
  unsigned char *ptr0, *ptr1;
  unsigned int size0, size1;
  void *pcm_ptr;
  int   pcm_count;
  long samples_read;

  if(!avi_audio_stream_open(avp->avidata, &channels, &sps, BITS_PER_SAMPLE, BUFFER_LENGTH)) {
    avp->err = ERR_AVI;
    return 0;
  }

  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.wBitsPerSample = BITS_PER_SAMPLE;
  wfx.nSamplesPerSec = sps;
  wfx.nChannels = channels;
  wfx.nBlockAlign = wfx.nChannels * BITS_PER_SAMPLE / 8;
  wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

  avp->buffer_size = wfx.nAvgBytesPerSec * BUFFER_LENGTH * 2;
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
  dsbd.dwBufferBytes = avp->buffer_size;
  dsbd.dwReserved = 0;
  dsbd.lpwfxFormat = &wfx;

  if(IDirectSound_CreateSoundBuffer(lpdsound, &dsbd, &avp->dsb, NULL) != DS_OK) {
    avp->err = ERR_CREATESOUNDBUFFER;
    return 0;
  }

  IDirectSoundBuffer_Lock(avp->dsb, 0, avp->buffer_size, 
			  (void **)&ptr0, 
			  (DWORD *)&size0, 
			  (void **)&ptr1, 
			  (DWORD *)&size1, 
			  0);
  avi_audio_getframe(avp->avidata, 0, &samples_read, &pcm_ptr, &pcm_count);

  memcpy(ptr0, pcm_ptr, pcm_count);
  avp->last_write_cursor = pcm_count;
  avp->sample_cursor = samples_read;

  IDirectSoundBuffer_Unlock(avp->dsb, ptr0, size0, ptr1, size1);
  IDirectSoundBuffer_Play(avp->dsb, 0, 0, DSBPLAY_LOOPING);

  return 1;
}



int
aviplayer_update_audio(AVIPLAYER *avp)
{
  unsigned int playc;
  unsigned int writec;
  unsigned int locksize;
  unsigned char *ptr0, *ptr1;
  unsigned int size0, size1;
  unsigned int last_write_pos;
  char *pcm_ptr;
  int   pcm_count;
  long  samples_read;

  last_write_pos = avp->last_write_cursor % avp->buffer_size;
  IDirectSoundBuffer_GetCurrentPosition(avp->dsb, (DWORD *)&playc, (DWORD *)&writec);

  if(avp->end) {
    if(playc < avp->last_write_cursor) {
      IDirectSoundBuffer_Stop(avp->dsb);
      return 1;
    }
  }

  if(last_write_pos < playc)
    locksize = playc - last_write_pos;
  else
    locksize = avp->buffer_size - last_write_pos + playc;

  if(locksize < (unsigned int)(avp->buffer_size / 2))
    return 0;


  if(!avi_audio_getframe(avp->avidata, avp->sample_cursor, &samples_read, (void **)&pcm_ptr, &pcm_count))
    avp->end = 1;
  else {
    IDirectSoundBuffer_Lock(avp->dsb, last_write_pos, pcm_count, 
			    (void **)&ptr0, (DWORD *)&size0, 
			    (void **)&ptr1, (DWORD *)&size1, 0);
    memcpy(ptr0, pcm_ptr, size0);
    if(ptr1)
      memcpy(ptr1, pcm_ptr + size0, size1);
    IDirectSoundBuffer_Unlock(avp->dsb, ptr0, size0, ptr1, size1);
    
    avp->sample_cursor += samples_read;
    avp->last_write_cursor += pcm_count;
  }

  return 1;
}

int
aviplayer_open_video(AVIPLAYER *avp)
{
  if(!avi_video_stream_open(avp->avidata, 32)) {
    avp->err = ERR_AVI;
    return 0;
  }

  avp->start = glutGet(GLUT_ELAPSED_TIME);
 
  if(avp->texture_mode) {
    glGenTextures(1, &avp->video_texture);
    glBindTexture(GL_TEXTURE_2D, avp->video_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  }

  avi_video_getframe(avp->avidata, 0, &avp->pbits);

  if(avp->texture_mode) {
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 
		 avi_video_width(avp->avidata),
		 avi_video_height(avp->avidata),
		 0,
		 GL_BGRA_EXT,
		 GL_UNSIGNED_BYTE,
		 avp->pbits);
  }

  return 1;
}
 
void
aviplayer_update_video(AVIPLAYER *avp)
{
  unsigned int now = glutGet(GLUT_ELAPSED_TIME) - avp->start;
  if(!avi_video_getframe(avp->avidata, avi_video_time_to_sample(avp->avidata, now), &avp->pbits)) {
    avp->end = 1;
    return;
  }

  if(avp->texture_mode) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 
		    0, 0, 
		    avi_video_width(avp->avidata), avi_video_height(avp->avidata),
		    GL_BGRA_EXT,
		    GL_UNSIGNED_BYTE,
		    avp->pbits);
  }
}
 
void
aviplayer_draw(AVIPLAYER *avp, int x, int y, int width, int height)
{
  if(avp->texture_mode) {
    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2i(x, y);

    glTexCoord2i(1, 0);
    glVertex2i(x + width, y);

    glTexCoord2i(1, 1);
    glVertex2i(x + width, y + height);

    glTexCoord2i(0, 1);
    glVertex2i(x, y + height);

    glEnd();
  } else {
    float zoomx, zoomy;

    zoomx = (float)width / (float)avi_video_width(avp->avidata);
    zoomy = (float)height / (float)avi_video_height(avp->avidata);

    glPixelZoom(zoomx, zoomy);

    glPushAttrib(GL_LIGHTING_BIT);
    glPushAttrib(GL_TEXTURE_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glRasterPos2i(x, y);
    glDrawPixels(avi_video_width(avp->avidata), 
		 avi_video_height(avp->avidata), 
		 GL_BGRA_EXT, GL_UNSIGNED_BYTE, avp->pbits);

    glPopAttrib();
    glPopAttrib();
  }
}
  
void
aviplayer_update(AVIPLAYER *avp)
{
  aviplayer_update_video(avp);
  aviplayer_update_audio(avp);
}

void
aviplayer_close(AVIPLAYER *avp)
{
  avi_close(avp->avidata);
  avp->start = 0;
  avp->pbits = NULL;
  
  if(avp->dsb) {
    IDirectSoundBuffer_Release(avp->dsb);
    avp->dsb = NULL;
  }
  avp->last_write_cursor = 0;
  avp->sample_cursor = 0;
  avp->end = 0;
  avp->texture_mode = 0;
  avp->err = 0;
}

int
aviplayer_open(AVIPLAYER *avp, const char *fname, int texture_mode, LPDIRECTSOUND lpds)
{
  if(!avi_open(avp->avidata, fname))
    return 0;

  avp->texture_mode = texture_mode;

  if(!aviplayer_open_video(avp))
     return 0;

  if(!aviplayer_open_audio(avp, lpds))
    return 0;

  return 1;
}

int
aviplayer_is_end(AVIPLAYER *avp)
{
  return avp->end;
}

AVIPLAYER *
aviplayer_new()
{
  AVIPLAYER *avp = calloc(1, sizeof(AVIPLAYER));
  avp->avidata = avi_new();
  return avp;
}

void
aviplayer_delete(AVIPLAYER *avp)
{
  aviplayer_close(avp);
  avi_delete(avp->avidata);
  free(avp);
}
