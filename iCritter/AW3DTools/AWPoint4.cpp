//
//  AWPoint4.cpp
//  iCritter
//
//  Created by Tom Gilbert on 01/12/2012.
//
//
#include "AWPoint4.h"


void
AWPoint4::saveGuts(AWFile& file) const
{
	file.Write(x);
	file.Write(y);
	file.Write(z);
    file.Write(w);
}


void
AWPoint4::restoreGuts(AWFile& file)
{
	file.Read(x);
	file.Read(y);
	file.Read(z);
    file.Read(w);
}