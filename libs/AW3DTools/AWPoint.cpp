#include "AWPoint.h"
#include "AWTVert.h"


void	
AWPoint::saveGuts(AWFile& file) const
{
	file.Write(x);
	file.Write(y);
	file.Write(z);
}


void	
AWPoint::restoreGuts(AWFile& file)
{
	file.Read(x);
	file.Read(y);
	file.Read(z);
}



void	
AWColVertex::saveGuts(AWFile& file) const
{
	file.Write(x);
	file.Write(y);
	file.Write(z);
}


void	
AWColVertex::restoreGuts(AWFile& file)
{
	file.Read(x);
	file.Read(y);
	file.Read(z);
}



void	
AWTVert::saveGuts(AWFile& file) const
{
	file.Write(x);
	file.Write(y);
}


void	
AWTVert::restoreGuts(AWFile& file)
{
	file.Read(x);
	file.Read(y);
}