#ifndef _libavi_h_
#define _libavi_h_

#ifdef LIBAVI_INTERNAL

#include <windows.h>
#include <vfw.h>

enum {
  ERROR_VFW,
  ERROR_GETFRAME,
  ERROR_ACM_OPEN,
  ERROR_ACM_PH,
  ERROR_ACM_CONVERT
};

typedef struct __AVIDATA {
  PAVIFILE        avifile;
  PAVISTREAM      videostream;
  PAVISTREAM      audiostream;

  PGETFRAME       getframe;
  HACMSTREAM      acmstream;
  ACMSTREAMHEADER acmheader;
  int             acmheader_valid;

  int             err;
  int             ret;

  LPBITMAPINFO    lpbi;
  int             bitcount;
  int             width;
  int             height;

  int             buffer_time;
  int             src_bytes_per_sec;
  int             src_bits_per_sample;
} AVIDATA;

typedef struct __AVIWRITEDATA {
  PAVIFILE       avifile;
  PAVISTREAM     avistream;
  PAVISTREAM     avistream_compressed;
  int            width;
  int            height;
  int            err;
  int            ret;
  int            sample;
} AVIWRITEDATA;

#else

typedef void AVIDATA;
typedef void AVIWRITEDATA;

typedef struct __VIDEOHDR {
  LPBYTE lpData;
  DWORD dwBufferLength;
  DWORD dwBytesUsed;
  DWORD dwTimeCaptured;
  DWORD dwUser;
  DWORD dwFlags;
  DWORD dwReserved[4];
} VIDEOHDR;

#endif

typedef LRESULT (WINAPI *CAP_CALLBACK_ON_FRAME_FPTR)(HWND hwnd, VIDEOHDR *vhdr);
typedef LRESULT (WINAPI *CAP_CALLBACK_ON_ERROR_FPTR)(HWND hwnd, int id, LPSTR text);

#ifdef __cplusplus
extern "C" {
#endif

const char *avi_get_error_string(AVIDATA *ad);
AVIDATA    *avi_new();
void        avi_delete(AVIDATA *ad);
int         avi_open(AVIDATA *ad, const char *fname);
void        avi_close(AVIDATA *ad);

int         avi_video_stream_open(AVIDATA *ad, int bitcount);
int         avi_video_getframe(AVIDATA *ad, long pos, char **pbits);
int         avi_video_width(AVIDATA *ad);
int         avi_video_height(AVIDATA *ad);
int         avi_video_time_to_sample(AVIDATA *ad, long ltime);
int         avi_video_sample_to_time(AVIDATA *ad, int sample);
int         avi_video_stream_length(AVIDATA *ad);

int         avi_audio_stream_open(AVIDATA *ad, int *channels, int *samples_per_sec, int bps, int buffer_time);
int         avi_audio_getframe(AVIDATA *ad, long pos, long *read, void **pbits, int *cbbits);
int         avi_audio_stream_length(AVIDATA *ad);

AVIWRITEDATA *avi_output_new();
void          avi_output_delete(AVIWRITEDATA *awd);

int           avi_output_open(AVIWRITEDATA *awd, HWND hwnd, const char *fname, int width, int height, int fps);
int           avi_output_put_frame(AVIWRITEDATA *awd, void *bits);
void          avi_output_close(AVIWRITEDATA *awd);
void          avi_output_get_error(AVIWRITEDATA *awd, char *buf);

  HWND cap_create(HWND hwnd, DWORD style, int x, int y, int width, int height, int id);
  void cap_destroy(HWND hwnd);
  int cap_driver_connect(HWND hwnd, int id);
  int cap_set_callback_on_frame(HWND hwnd, CAP_CALLBACK_ON_FRAME_FPTR callback);
  int cap_set_callback_on_error(HWND hwnd, CAP_CALLBACK_ON_ERROR_FPTR callback);
  int cap_preview_rate(HWND hwnd, int mills);
  int cap_preview(HWND hwnd, int preview);
  int cap_set_video_format(HWND hwnd, int width, int height, int bitcount);
  int cap_dlg_video_source(HWND hwnd);
  

#ifdef __cplusplus
}
#endif

#endif
