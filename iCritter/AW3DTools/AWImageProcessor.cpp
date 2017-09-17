//currently used to write a particular kind of TGA image only
//supply RGBA data and 
//write True Colour TGA with no alpha channel

#ifdef _WIN32
//#include <windows.h>
//#include "Jpegfile.h"
#endif
#include "AWImageProcessor.h"

int 
AWImageProcessor::saveRGB(const AWCString& fileName,		
					      BYTE *dataBuf, int width, int height, int fmt)
{
	//file name now comes in without any extension - must add
	int retVal = 0;
	AWCString fullName(fileName);
	switch (fmt)
	{
	case IMG_FMT_TGA:
		fullName += ".tga";
		retVal = saveTGA(fullName, dataBuf,  width,  height);
		break;
	default:
	case IMG_FMT_JPG:
		fullName += ".jpg";
//#ifdef _WIN32
//        JpegFile::RGBToJpegFile(fullName, dataBuf, width, height,
//								TRUE, 100);
//#endif
		break;
	}//switch
	return retVal;
}//int AWImageProcessor::saveRGB( )



int 
AWImageProcessor::saveTGA(const AWCString& fileName,		
					   BYTE *dataBuf, int width, int height)
{
	//first initialise the TGAHEADER
	TGAHEADER head;
	head.m_idLength=0;          // no ID field
    head.m_cmapType=0;          // no colour map (True Colour)
    head.m_type=2;				// True Colour

    head.m_cmapIndex=0;         // First Entry Index
    head.m_cmapLength=0;        // Color Map Length
    head.m_cmapEntrySize=0;     // Color Map Entry Size

    head.m_XOrigin=0;           // X-origin of Image
    head.m_YOrigin=0;           // Y-origin of Image
    head.m_width=width;			
    head.m_height=height;		
    head.m_pixelDepth=24;       // 8 bit R, G, B
    head.m_desc=0;	
	
	//now initialise a TGAFOOTER
	TGAFOOTER foot;
	foot.m_extensionOffset=0;
	foot.m_developerOffset=0;
	char cRubbish[] = "TRUEVISION-XFILE";
  int i=0;
	for (i=0; i<16; i++)
	{
		foot.m_name[i]=cRubbish[i];
	}
	foot.m_period='.';		//.
	foot.m_terminator=0x00;	//0x00

	//now open the file for BINARY output
	AWFile outFile(fileName.c_str());
	if (outFile.isValid())
	{
		//WRITE HEADER
		outFile.Write(head.m_idLength);          // no ID field
		outFile.Write(head.m_cmapType);          // no colour map (True Colour)
		outFile.Write(head.m_type);				// True Colour

		outFile.Write(head.m_cmapIndex);         // First Entry Index
		outFile.Write(head.m_cmapLength);        // Color Map Length
		outFile.Write(head.m_cmapEntrySize);     // Color Map Entry Size

		outFile.Write(head.m_XOrigin);           // X-origin of Image
		outFile.Write(head.m_YOrigin);           // Y-origin of Image
		outFile.Write(head.m_width);			
		outFile.Write(head.m_height);		
		outFile.Write(head.m_pixelDepth);       // 8 bit R, G, B
		outFile.Write(head.m_desc);

		//WRITE IMAGE RGB DATA
		//so apparently each RGB triplet needs to be written
		//as BGR?
		//outFile.Write(dataBuf, width*height*3);
		for (i=0; i<width*height; i++)
		{	//find each RGB triplet
			//there are width*height pixels, 
			//hence width*height RGB triplets
			//and 3*width*height bytes in dataBuf
			outFile.Write(dataBuf[(i*3)+2]);  //B
			outFile.Write(dataBuf[(i*3)+1]);  //G
			outFile.Write(dataBuf[i*3]);    //R
		}
		//WRITE FOOTER
		outFile.Write(foot.m_extensionOffset);
		outFile.Write(foot.m_developerOffset);
		outFile.Write(foot.m_name, sizeof(foot.m_name));		//TRUEVISION-XFILE
		outFile.Write(foot.m_period);		//.
		outFile.Write(foot.m_terminator);	//0x00
		outFile.Flush();
	}
	return 1;
}

