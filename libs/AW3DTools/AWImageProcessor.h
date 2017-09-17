#ifndef __AWIMAGEPROCESSOR_H
#define __AWIMAGEPROCESSOR_H

#include "AWToolsCommon.h"
#include "AWCString.h"
#ifdef _WIN32
#include <windows.h>
#endif

typedef struct _TgaHeader
{
    BYTE   m_idLength;          // Image ID Field Length
    BYTE   m_cmapType;          // Color Map Type
    BYTE   m_type;				// Image Type

    WORD   m_cmapIndex;         // First Entry Index
    WORD   m_cmapLength;        // Color Map Length
    BYTE   m_cmapEntrySize;     // Color Map Entry Size

    WORD   m_XOrigin;           // X-origin of Image
    WORD   m_YOrigin;           // Y-origin of Image
    WORD   m_width;				// Image Width
    WORD   m_height;			// Image Height
    BYTE   m_pixelDepth;        // Pixel Depth
    BYTE   m_desc;				// Image Descriptor
} TGAHEADER;


typedef struct _TgaFooter
{
	LONG	m_extensionOffset;
	LONG	m_developerOffset;
	BYTE	m_name[16];		//TRUEVISION-XFILE
	BYTE	m_period;		//.
	BYTE	m_terminator;	//0x00
} TGAFOOTER;


#define IMG_FMT_JPG 0
#define IMG_FMT_TGA 1

class AWImageProcessor
{
public:
	static int saveRGB(const AWCString& fileName,		
					   BYTE *dataBuf, int width, int height, 
					   int fmt=IMG_FMT_JPG);

	static int saveTGA(const AWCString& fileName,		
					   BYTE *dataBuf, int width, int height);

};//class AWImageProcessor


#endif //sentinel __AWIMAGEPROCESSOR_H