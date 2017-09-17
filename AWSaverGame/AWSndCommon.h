#ifndef __AWSNDCOMMON_H
#define __AWSNDCOMMON_H

#include "AWSndDefs.h"
#include <AW3DTools/AWToolsCommon.h>
#include <AW3DTools/AWCString.h>
#include <math.h>

#ifndef _WIN32
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#endif

#endif //sentinel __AWSNDCOMMON_H