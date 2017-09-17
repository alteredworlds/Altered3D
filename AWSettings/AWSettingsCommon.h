#ifndef _AW_SETTINGS_COMMON_H
#define _AW_SETTINGS_COMMON_H

#include <windows.h>
#include <regstr.h>
#include <stdio.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <AW3DTools/AWToolsCommon.h>
#include <AW3DTools/AWCString.h>
#include <AWSaverGame/AWWinUtils.h>
#include <AWSaverGame/AWConfigWin.h>
#include <AWSaverGame/AWSpaceGnats.h>

#include <AWSerialize/AWVerify.h>

#include "res/AlteredSettingsRes.h"

#define VISIBLE_TIME   1500
#define INVISIBLE_TIME 300

#define AW_PRODUCT_VERSION_ALTEREDSAVER 3.2f
#define WM_AWGET_UPDATE     WM_USER + 2



#define REGSTR_PATH_PLUSSCR (REGSTR_PATH_SETUP "\\Screen Savers")
#define REGSTR_PATH_CONFIG  ("Software\\AlteredWorlds\\Saver")


#endif //sentinel _AW_SETTINGS_COMMON_H