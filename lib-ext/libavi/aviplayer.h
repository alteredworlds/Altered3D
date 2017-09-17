#ifndef _aviplayer_h_
#define _aviplayer_h_

enum {
  ERR_AVI,
  ERR_CREATESOUNDBUFFER
};

typedef struct __AVIPLAYER {
  AVIDATA *avidata;
  unsigned int start;
  char *pbits;
  int video_texture;
  LPDIRECTSOUNDBUFFER dsb;
  unsigned int last_write_cursor;
  long sample_cursor;
  int buffer_size;
  int end;
  int texture_mode;
  int err;
} AVIPLAYER;

AVIPLAYER * aviplayer_new();
void        aviplayer_delete();
int         aviplayer_is_end();
int         aviplayer_open(AVIPLAYER *avp, const char *fname, int tm, LPDIRECTSOUND lpds);
void        aviplayer_close(AVIPLAYER *avp);
void        aviplayer_update(AVIPLAYER *avp);
void        aviplayer_draw(AVIPLAYER *avp, int x, int y, int width, int height);
const char *aviplayer_get_error_string(AVIPLAYER *avp);

#endif
