/* minigzip.c -- simulate gzip using the zlib compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/*
 * minigzip is a minimal implementation of the gzip utility. This is
 * only an example of using zlib and isn't meant to replace the
 * full-featured gzip. No attempt is made to deal with file systems
 * limiting names to 14 or 8+3 characters, etc... Error checking is
 * very limited. So use minigzip only for testing; use gzip for the
 * real thing. On MSDOS, use only on file names without extension
 * or in pipe mode.
 */

/* @(#) $Id$ */

#include <stdio.h>
#include "zlib.h"

#ifdef STDC
#  include <string.h>
#  include <stdlib.h>
#else
extern void exit  OF((int));
#endif

#ifdef USE_MMAP
#  include <sys/types.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#ifdef VMS
#  define unlink delete
#  define GZ_SUFFIX "-gz"
#endif
#ifdef RISCOS
#  define unlink remove
#  define GZ_SUFFIX "-gz"
#  define fileno(file) file->__file
#endif
#if defined(__MWERKS__) && __dest_os != __be_os && __dest_os != __win32_os
#  include <unix.h> /* for fileno */
#endif

#ifndef WIN32 /* unlink already in stdio.h for WIN32 */
//extern int unlink OF((const char *));
#include <unistd.h>
#endif

#ifndef GZ_SUFFIX
#  define GZ_SUFFIX ".gz"
#endif
#define SUFFIX_LEN (sizeof(GZ_SUFFIX)-1)

#define BUFLEN      16384
#define MAX_NAME_LEN 1024

#ifdef MAXSEG_64K
#  define local static
/* Needed for systems with limitation on stack size. */
#else
#  define local
#endif

char *prog;

//void error            OF((const char *msg));
void gz_uncompress    OF((gzFile in, FILE   *out));
void file_uncompress  OF((char  *file));



/* ===========================================================================
 * Uncompress input to output then close both files.
 */
void gz_uncompress(gzFile in, FILE   *out)
{
    local char buf[BUFLEN];
    int len;
    for (;;) {
        len = gzread(in, buf, sizeof(buf));
        if (len < 0) return;
        if (len == 0) break;

        if ((int)fwrite(buf, 1, (unsigned)len, out) != len) {
	    return;
	}
    }
    if (fclose(out)) return;

    if (gzclose(in) != Z_OK) return;
}


/* ===========================================================================
 * Uncompress the given file and remove the original.
 */
void file_uncompress(const char  *file)
{
    local char buf[MAX_NAME_LEN];
    char *infile, *outfile;
    FILE  *out;
    gzFile in;
    int len = strlen(file);

    strcpy(buf, file);

    if (len > SUFFIX_LEN && strcmp(file+len-SUFFIX_LEN, GZ_SUFFIX) == 0) {
        infile = (char*)file;
        outfile = buf;
        outfile[len-3] = '\0';
    } else {
        outfile = (char*)file;
        infile = buf;
        strcat(infile, GZ_SUFFIX);
    }
    in = gzopen(infile, "rb");
    if (in == NULL) {
        fprintf(stderr, "%s: can't gzopen %s\n", prog, infile);
        return;
    }
    out = fopen(outfile, "wb");
    if (out == NULL) {
        perror(file);
        return;
    }

    gz_uncompress(in, out);

    unlink(infile);
}
