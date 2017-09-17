//************************************************************************** 
//* Export.cpp	- TnT Binary File Exporter
//* 
//* By Tom Gilbert
//*
//*
//* This module contains the main export functions.
//*
//* Copyright (c) 1998, All Rights Reserved. 
//***************************************************************************

#include "AlteredExport.h"


// Round numbers near zero to zero.  This help reduce VRML file size.
inline float round(float f)
{
    // This is used often, so we avoid calling fabs
    if (f < 0.0f) 
    {
        if (f > -1.0e-5)
        {
            return 0.0f;
        }
        return f;
    }//if (f < 0.0f) 
    if (f < 1.0e-5)
    {
        return 0.0f;
    }
    return f;
}//inline float round(float f)



//Global output [Scene info]

// Dump some global animation information.
void 
TnTExprt::ExportGlobalInfo()
{
	Interval range = ip->GetAnimRange();

	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);	// The date string has a \n appended.
	today.remove(today.length()-1);		// Remove the \n
	//fprintf(pStream, "\t%s \"%s\"\n", ID_FILENAME, FixupName(ip->GetCurFileName()));
	//fprintf(pStream, "\t%s %d\n", ID_FIRSTFRAME, range.Start() / GetTicksPerFrame());
	//fprintf(pStream, "\t%s %d\n", ID_LASTFRAME, range.End() / GetTicksPerFrame());
	//fprintf(pStream, "\t%s %d\n", ID_FRAMESPEED, GetFrameRate());
	//fprintf(pStream, "\t%s %d\n", ID_TICKSPERFRAME, GetTicksPerFrame());
	
	Texmap* env = ip->GetEnvironmentMap();
	Control* ambLightCont;
	Control* bgColCont;
	if (env) 
	{
		// NOT SUPPORTED
	}//if (env)
	else 
   { // Output background color
		bgColCont = ip->GetBackGroundController();
		if (bgColCont && bgColCont->IsAnimated()) 
      {  // background color is animated.
		   DumpPoint3Keys(bgColCont, theScene.getBackground());
         theScene.getBackground().calc(0); //to prime it correctly
		}
		else 
		{
			// Background color is not animated
			Color bgCol = ip->GetBackGround(GetStaticFrame(), FOREVER);
         theScene.setBackground(AWPoint(bgCol.r, bgCol.g, bgCol.b));
		}
	}//else
	
	// Output ambient light
	ambLightCont = ip->GetAmbientController();
	if (ambLightCont && ambLightCont->IsAnimated()) 
	{
		// Ambient light is animated.
		DumpPoint3Keys(ambLightCont, theScene.getAmbient());
      theScene.getAmbient().calc(0); //to prime it correctly
	}//if (ambLightCont && ambLightCont->IsAnimated())
	else 
	{
		// Ambient light is not animated
		Color ambLight = ip->GetAmbient(GetStaticFrame(), FOREVER);
		theScene.setAmbient(AWPoint(ambLight.r, ambLight.g, ambLight.b));
	}//else 
}//void TnTExprt::ExportGlobalInfo()