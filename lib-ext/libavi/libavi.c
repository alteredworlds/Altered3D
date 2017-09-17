#include <windows.h>
#include <vfw.h>
#include <stdio.h>
#include <stdlib.h>

#define LIBAVI_INTERNAL
#include "libavi.h"

  
const char *
avi_get_error_string(AVIDATA *ad)
{
  static char buf[256];
  char buf2[64];

  switch(ad->err) {
  case ERROR_VFW:
    sprintf(buf, "Video for windows error: %08x", ad->ret);
    break;
  case ERROR_GETFRAME:
    sprintf(buf, "No suitable video codec found");
    break;
  case ERROR_ACM_OPEN:
    switch(ad->ret) {
    case ACMERR_NOTPOSSIBLE:
      sprintf(buf2, "No audio codec could be found");
      break;
    case MMSYSERR_INVALFLAG:
      sprintf(buf2, "Invalid flag");
      break;
    case MMSYSERR_INVALPARAM:
      sprintf(buf2, "Invalid parameter");
      break;
    case MMSYSERR_NOMEM:
      sprintf(buf2, "Not enough memory");
      break;
    default:
      sprintf(buf2, "Dont know what this means, sorry");
      break;
    }
    sprintf(buf, "Audio Compression Manager error opening: %08x (%s)", ad->ret, buf2);
    break;
  case ERROR_ACM_PH:
    sprintf(buf, "Audio Compression Manager error preparing header: %08x", ad->ret);
    break;
  case ERROR_ACM_CONVERT:
    sprintf(buf, "Audio Compression Manager error converting: %08x", ad->ret);
    break;
  }

  return buf;
}

AVIDATA *
avi_new()
{
  AVIFileInit();
  return calloc(1, sizeof(AVIDATA));
}

void
avi_delete(AVIDATA *ad)
{  
  avi_close(ad);
  free(ad);
  AVIFileExit();
}

int
avi_open(AVIDATA *ad, const char *fname) 
{
  ad->ret = AVIFileOpen(&ad->avifile, fname, OF_READ, NULL);
  return (ad->ret == 0);
}

void
avi_close(AVIDATA *ad)
{
  if(ad->acmheader_valid) {
    acmStreamUnprepareHeader(ad->acmstream, &ad->acmheader, 0);
    free(ad->acmheader.pbSrc);
    free(ad->acmheader.pbDst);
    ad->acmheader_valid = 0;
  }

  if(ad->acmstream)
    acmStreamClose(ad->acmstream, 0);

  if(ad->getframe)
    AVIStreamGetFrameClose(ad->getframe);
  if(ad->videostream)
    AVIStreamRelease(ad->videostream);
  if(ad->audiostream)
    AVIStreamRelease(ad->audiostream);
  if(ad->avifile)
    AVIFileRelease(ad->avifile);

  memset(ad, 0, sizeof(AVIDATA));
}

int
avi_video_stream_open(AVIDATA *ad, int bitcount)
{
  AVIFILEINFO ai;
  BITMAPINFOHEADER bmi;

  ad->ret = AVIFileGetStream(ad->avifile, 
			     &ad->videostream, 
			     streamtypeVIDEO,
			     0);
  if(ad->ret != 0)
    return 0;

  AVIFileInfo(ad->avifile, &ai, sizeof(ai));

  memset(&bmi, 0, sizeof(bmi));
  bmi.biSize = sizeof(bmi);
  bmi.biWidth = ai.dwWidth;
  bmi.biHeight = ai.dwHeight;
  bmi.biCompression = BI_RGB;
  bmi.biBitCount = bitcount;

  ad->getframe = AVIStreamGetFrameOpen(ad->videostream, &bmi);
  if(ad->getframe == NULL) {
    ad->err = ERROR_GETFRAME;
    return 0;
  }

  ad->bitcount = bitcount;
  ad->width = ai.dwWidth;
  ad->height = ai.dwHeight;

  return 1;
}

int
avi_video_getframe(AVIDATA *ad, long pos, char **pbits)
{
  ad->lpbi = (LPBITMAPINFO)AVIStreamGetFrame(ad->getframe, pos);

  if(ad->lpbi == NULL)
    return 0;

  *pbits = ((unsigned char *)ad->lpbi) + ad->lpbi->bmiHeader.biSize;
  *pbits += ((unsigned int)*pbits % 4);

  return 1;
}

int
avi_video_width(AVIDATA *ad)
{
  return ad->width;
}

int 
avi_video_height(AVIDATA *ad)     
{
  return ad->height;
}

int
avi_video_time_to_sample(AVIDATA *ad, long ltime)
{
  return AVIStreamTimeToSample(ad->videostream, ltime);
}

int
avi_video_sample_to_time(AVIDATA *ad, int sample)
{
  return AVIStreamSampleToTime(ad->videostream, sample);
}

int
avi_video_stream_length(AVIDATA *ad)
{
  return AVIStreamLength(ad->videostream);
}

int
avi_audio_stream_open(AVIDATA *ad, int *channels, int *samples_per_sec, int bits_per_sample, int buffer_time)
{
  int wfx_src_size;
  WAVEFORMATEX *pwfx_src;
  WAVEFORMATEX wfx_dst;

  ad->ret = AVIFileGetStream(ad->avifile, &ad->audiostream, streamtypeAUDIO, 0);
  if(ad->ret != 0)
    return 0;

  AVIStreamFormatSize(ad->audiostream, 0, &wfx_src_size);
  pwfx_src = calloc(1, wfx_src_size);
  AVIStreamReadFormat(ad->audiostream, 0, pwfx_src, &wfx_src_size);

  *channels = pwfx_src->nChannels;
  *samples_per_sec = pwfx_src->nSamplesPerSec;

  wfx_dst.wFormatTag = WAVE_FORMAT_PCM;
  wfx_dst.nChannels = *channels;
  wfx_dst.nSamplesPerSec = *samples_per_sec;
  wfx_dst.wBitsPerSample = bits_per_sample;
  wfx_dst.nBlockAlign = (*channels * bits_per_sample) / 8;
  wfx_dst.nAvgBytesPerSec = *samples_per_sec * wfx_dst.nBlockAlign;

  ad->ret = acmStreamOpen(&ad->acmstream, 
			  NULL,
			  pwfx_src,
			  &wfx_dst,
			  NULL,
			  0,
			  0,
			  0);

  if(ad->ret != 0) {
    free(pwfx_src);
    ad->err = ERROR_ACM_OPEN;
    return 0;
  }

  memset(&ad->acmheader, 0, sizeof(ACMSTREAMHEADER));
  ad->acmheader.cbStruct = sizeof(ACMSTREAMHEADER);
  ad->acmheader.cbSrcLength = pwfx_src->nAvgBytesPerSec * buffer_time;
  ad->acmheader.pbSrc = malloc(ad->acmheader.cbSrcLength);
  ad->acmheader.cbDstLength = wfx_dst.nAvgBytesPerSec * buffer_time;
  ad->acmheader.pbDst = malloc(ad->acmheader.cbDstLength);

  ad->ret = acmStreamPrepareHeader(ad->acmstream, &ad->acmheader, 0);
  if(ad->ret != 0) {
    ad->err = ERROR_ACM_PH;
    free(pwfx_src);
    free(ad->acmheader.pbSrc);
    free(ad->acmheader.pbDst);
    return 0;
  }

  ad->acmheader_valid = 1;
  ad->src_bytes_per_sec = pwfx_src->nAvgBytesPerSec;
  ad->src_bits_per_sample = pwfx_src->wBitsPerSample;
  ad->buffer_time = buffer_time;
  
  free(pwfx_src);
  return 1;
}

int
avi_audio_getframe(AVIDATA *ad, long pos, long *read, void **pbits, int *cbbits)
{
  long bytes_read;
  long samples;
  int end = 0;

  samples = ad->buffer_time * ad->src_bytes_per_sec * (ad->src_bits_per_sample / 8);

  if((pos + samples) > AVIStreamLength(ad->audiostream)) {
    samples = AVIStreamLength(ad->audiostream) - pos;
    end = 1;
  }

  ad->ret = AVIStreamRead(ad->audiostream, 
			  pos, 
			  samples, 
			  ad->acmheader.pbSrc, 
			  ad->acmheader.cbSrcLength,
			  &bytes_read,
			  read);
  if(ad->ret != 0)
    return 0;

  ad->ret = acmStreamConvert(ad->acmstream, 
			     &ad->acmheader, 
			     (end ? ACM_STREAMCONVERTF_END : 0) | (pos == 0 ? ACM_STREAMCONVERTF_START : 0));
  if(ad->ret != 0) {
    ad->err = ERROR_ACM_CONVERT;
    return 0;
  }

  *pbits = ad->acmheader.pbDst;
  *cbbits = ad->acmheader.cbDstLength;

  return 1;
}

int
avi_audio_stream_length(AVIDATA *ad)
{
  return AVIStreamLength(ad->audiostream);
}


  
AVIWRITEDATA *
avi_output_new()
{
  AVIWRITEDATA *awd;

  awd = calloc(1, sizeof(AVIWRITEDATA));
  AVIFileInit();
  return awd;
}

void
avi_output_delete(AVIWRITEDATA *awd)
{
  avi_output_close(awd);
  free(awd);
  AVIFileExit();
}

void
avi_output_close(AVIWRITEDATA *awd)
{
  if(awd->avistream_compressed)
    AVIStreamClose(awd->avistream_compressed);
  if(awd->avistream)
    AVIStreamClose(awd->avistream);
  if(awd->avifile)
    AVIFileClose(awd->avifile);
  memset(awd, 0, sizeof(AVIWRITEDATA));
}

int
avi_output_open(AVIWRITEDATA *awd, HWND hwnd, const char *fname, int width, int height, int fps)
{
  BITMAPINFOHEADER bmi;
  AVICOMPRESSOPTIONS opts;
  AVICOMPRESSOPTIONS *popts;
  AVISTREAMINFO info;

  awd->width = width;
  awd->height = height;

  awd->ret = AVIFileOpen(&awd->avifile, fname, OF_CREATE|OF_WRITE, NULL);
  if(awd->ret != 0) {
    awd->err = ERROR_VFW;
    return 0;
  }

  memset(&info, 0, sizeof(AVISTREAMINFO));
  info.fccType = streamtypeVIDEO;
  info.dwScale = 1;
  info.dwRate = fps;
  info.rcFrame.right = width;
  info.rcFrame.bottom = height;

  awd->ret = AVIFileCreateStream(awd->avifile, &awd->avistream, &info);
  if(awd->ret != 0) {
    awd->err = ERROR_VFW;
    return 0;
  }

  memset(&opts, 0, sizeof(AVICOMPRESSOPTIONS));
  popts = &opts;
  awd->ret = AVISaveOptions(hwnd, 0, 1, &awd->avistream, &popts);
  if(awd->ret != TRUE) {
    awd->err = ERROR_VFW;
    return 0;
  }

  awd->ret = AVIMakeCompressedStream(&awd->avistream_compressed, awd->avistream, &opts, NULL);
  if(awd->ret != 0) {
    awd->err = ERROR_VFW;
    return 0;
  }

  memset(&bmi, 0, sizeof(BITMAPINFOHEADER));
  bmi.biSize = sizeof(BITMAPINFOHEADER);
  bmi.biWidth = width;
  bmi.biHeight = height;
  bmi.biPlanes = 1;
  bmi.biBitCount = 32;
  bmi.biCompression = BI_RGB;

  awd->ret = AVIStreamSetFormat(awd->avistream_compressed, 0, &bmi, sizeof(BITMAPINFOHEADER));
  if(awd->ret != 0) {
    awd->err = ERROR_VFW;
    return 0;
  }

  return 1;
}

int
avi_output_put_frame(AVIWRITEDATA *awd, void *bits)
{
  awd->ret = AVIStreamWrite(awd->avistream_compressed, 
			    awd->sample,
			    1, 
			    bits,
			    awd->width * awd->height * 4,
			    AVIIF_KEYFRAME,
			    NULL,
			    NULL);
  if(awd->ret != 0) {
    awd->err = ERROR_VFW;
    return 0;
  }

  awd->sample++;
  return 1;
}

void
avi_output_get_error(AVIWRITEDATA *awd, char *buf)
{
  switch(awd->ret) {
  case AVIERR_BADFORMAT:
    strcpy(buf, "The file could not be read, its either corrupt or an unrecogizeable format");
    return;
  case AVIERR_MEMORY:
    strcpy(buf, "Insuffient memory\n");
    return;
  case AVIERR_FILEREAD:
    strcpy(buf, "A disk error occured while reading the file");
    return;
  case AVIERR_FILEOPEN:
    strcpy(buf, "A disk error occured opening the file");
    return;
  case REGDB_E_CLASSNOTREG:
    strcpy(buf, "A handler could not be found for the file");
    return;
  case AVIERR_NOCOMPRESSOR:
    strcpy(buf, "A suitable compressor could not be found");
    return;
  case AVIERR_UNSUPPORTED:
    strcpy(buf, "Compression is not support for this data type");
    return;
  default:
    sprintf(buf, "Video For Windows error 0x%08x", awd->ret);
    return;
  }
}

HWND
cap_create(HWND hwnd, DWORD style, int x, int y, int width, int height, int id)
{
  return capCreateCaptureWindow("libavi", style, x, y, width, height, hwnd, id);
}

void
cap_destroy(HWND hwnd)
{
  capSetCallbackOnFrame(hwnd, NULL);
  capSetCallbackOnError(hwnd, NULL);
  DestroyWindow(hwnd);
}

int
cap_driver_connect(HWND hwnd, int id)
{
  return capDriverConnect(hwnd, id);
}

int
cap_set_callback_on_frame(HWND hwnd, CAP_CALLBACK_ON_FRAME_FPTR callback)
{
  return capSetCallbackOnFrame(hwnd, callback);
}

int
cap_set_callback_on_error(HWND hwnd, CAP_CALLBACK_ON_ERROR_FPTR callback)
{
  return capSetCallbackOnError(hwnd, callback);
}

int
cap_preview_rate(HWND hwnd, int mills)
{
  return capPreviewRate(hwnd, mills);
}

int
cap_preview(HWND hwnd, int preview)
{
  return capPreview(hwnd, preview);
}

int
cap_set_video_format(HWND hwnd, int width, int height, int bitcount)
{
  BITMAPINFOHEADER bmh;

  memset(&bmh, 0, sizeof(BITMAPINFOHEADER));
  bmh.biSize = sizeof(BITMAPINFOHEADER);
  bmh.biWidth = width;
  bmh.biHeight = height;
  bmh.biPlanes = 1;
  bmh.biBitCount = bitcount;
  bmh.biCompression = BI_RGB;

  return capSetVideoFormat(hwnd, (BITMAPINFO *)&bmh, sizeof(BITMAPINFO));
}

int
cap_dlg_video_source(HWND hwnd)
{
  return capDlgVideoSource(hwnd);
}


  
		     
  
  
    

  

  
  
  
  


  
  
  
  
  
  
  
