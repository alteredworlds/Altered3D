#include <windows.h>
#include <dsound.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "libavi.h"
#include "aviplayer.h"

const char *window_title = "libavi";

LPDIRECTSOUND dsound;
LPDIRECTSOUNDBUFFER dsprimary;

int window_width;
int window_height;

AVIPLAYER *avp;
AVIWRITEDATA *awd;

int
dsound_load(HWND hwnd)
     /* load DirectSound - set the primary buffer to CD quality */
{
  WAVEFORMATEX wfx;
  DSBUFFERDESC dsbd;

  if(DirectSoundCreate(NULL, &dsound, NULL) == DS_OK) {
    if(IDirectSound_SetCooperativeLevel(dsound, hwnd, DSSCL_PRIORITY) != DS_OK) {
      IDirectSound_Release(dsound);
      printf("Failed to set dsound cooperative level");
      return 0;
    }
    
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;
    
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.dwReserved = 0;
    dsbd.lpwfxFormat = NULL;
    
    if(IDirectSound_CreateSoundBuffer(dsound, &dsbd, &dsprimary, NULL) != DS_OK) {
      IDirectSound_Release(dsound);
      return 0;
    }
    
    IDirectSoundBuffer_SetFormat(dsprimary, &wfx);
    
    return 1;
  }
  
  return 0;
}
 
void
display(void)
{
  static char *bits = NULL;

  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  aviplayer_draw(avp, 0, 0, window_width, window_height);
  glutSwapBuffers();

  if(!bits)
    bits = calloc(1, 500 * 500 * 4);
  glReadPixels(0, 0, 500, 500, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits);
  avi_output_put_frame(awd, bits);
}

void
update(void)
{
  aviplayer_update(avp);

  if(aviplayer_is_end(avp))
    printf("End!\n");

  glutPostRedisplay();
}

void
reshape(int w, int h)
{
  window_width = w;
  window_height = h;

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, 100.0, -100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void
keyb(unsigned char c, int x, int y)
{
  switch(c) {
  case 'q':
    exit(0);
    break;
  }
}

void
init(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow(window_title);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(update);
  glutKeyboardFunc(keyb);

  glEnable(GL_TEXTURE_2D);
}

void
cleanup()
{
  avi_output_close(awd);
  avi_output_delete(awd);
}

int
main(int argc, char **argv)
{
  HWND hwnd;

  if(argc != 2) {
    printf("Usage: %s <avifile>\n", argv[0]);
    return -1;
  }

  init(argc, argv);

  hwnd = FindWindow(NULL, window_title);
  if(!hwnd) {
    printf("Failed to find glut window\n");
    return -1;
  }

  if(!dsound_load(hwnd))
    return -1;
    
  avp = aviplayer_new();
  if(!aviplayer_open(avp, argv[1], 0, dsound)) {
    printf("%s\n", aviplayer_get_error_string(avp));
    return -1;
  }

  awd = avi_output_new();
  if(!avi_output_open(awd, hwnd, "test.avi", 500, 500, 25)) {
    printf("Error creating\n");
    return -1;
  }
  atexit(cleanup);

  glutMainLoop();
  return 0;
}
