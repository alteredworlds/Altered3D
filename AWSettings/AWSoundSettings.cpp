#include "AWSettingsCommon.h"
#include "AWSoundSettings.h"
#include "AWSettingsDlg.h"
 

#define MAX_SOUND_NAME_DISPLAY_LENGTH 28


AWSoundSettingsDlg::AWSoundSettingsDlg(AWConfig& cfg, HINSTANCE hInst, HWND parent) : 
					AWTabPageDlg(hInst, parent),
               m_data(cfg)
{
   m_IDD = IDD_SOUNDSETTINGS_TAB;
}



AWBoolean 
AWSoundSettingsDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (LOWORD(wParam))
   {
   case IDC_PICK_SAVER_WAVE:
      pickWave();
      break;

   case IDC_NO_SOUNDS_CHECK:
      noSoundsCheck();
      break;

   case IDC_PLAY_WAVE_CHECK:
      playWaveCheck();
      break;

   case IDC_LAUNCH_HELPER_CHECK:
      launchHelperCheck();
      break;

   case IDC_GAME_MUSIC:
      gameMusicCheck();
      break;

   case IDC_PLAY_AUDIOCD_CHECK:
      playAudioCDCheck();
      break;
   }//switch (id) WM_COMMAND
   return retVal;
}//AWBoolean AWSceneSettingsDlg::OnDialogCommand(...)

/*enum AWSounds     {awNone=0, awNotUsed, awPlayFile, awPlayCD, awPlayGameMusic};*/


void
AWSoundSettingsDlg::transferData(AWBoolean fromControls)
{	//from/to display settings section
	//width/height/bpp fullscreen
   if (!m_hDlg) return;
	if (fromControls)
   {  //retrieve the current / switch mode setting
      m_data.m_playEffects = 0;
		if (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_LAUNCH_HELPER_CHECK))
		{
         m_data.m_playSound = AWConfig::awPlayFile;           
		}
		else if (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_PLAY_AUDIOCD_CHECK))
		{
			m_data.m_playSound = AWConfig::awPlayCD;
			//now need to get the selected CDROM drive letter
			//required so that NT can initiate playback of CD Audio properly
			int index = SendDlgItemMessage(m_hDlg, IDC_CDROM_DRIVE, CB_GETCURSEL, 
								(WPARAM)0, (LPARAM)0);
			if (CB_ERR != index)
			{
				char temp[10];
				SendDlgItemMessage(m_hDlg, IDC_CDROM_DRIVE, CB_GETLBTEXT, 
								  (WPARAM) index, (LPARAM) (LPCSTR) temp);
				m_data.m_cdrom = temp;
			}
		}//else if (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_PLAY_AUDIOCD_CHECK))
		else if (BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_GAME_MUSIC))
		{
         m_data.m_playSound = AWConfig::awPlayGameMusic;
         m_data.m_playEffects = BST_CHECKED == IsDlgButtonChecked(m_hDlg, IDC_GAME_SOUNDEFFECTS);
		}
      else m_data.m_playSound = AWConfig::awNone;
      
	}
	else //toControls
	{
		CheckDlgButton(m_hDlg, IDC_NO_SOUNDS_CHECK, 
			m_data.getNoMusic() ? BST_CHECKED : BST_UNCHECKED);

      CheckDlgButton(m_hDlg, IDC_GAME_MUSIC,
         m_data.getPlayGameMusic() ? BST_CHECKED : BST_UNCHECKED);

      CheckDlgButton(m_hDlg, IDC_GAME_SOUNDEFFECTS, 
			m_data.getPlayEffects() ? BST_CHECKED : BST_UNCHECKED);

      EnableWindow(GetDlgItem(m_hDlg, IDC_GAME_SOUNDEFFECTS), m_data.getPlayGameMusic() );

		CheckDlgButton(m_hDlg, IDC_LAUNCH_HELPER_CHECK, m_data.getPlayFile() ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(m_hDlg, IDC_PLAY_AUDIOCD_CHECK, m_data.getPlayCD() ? BST_CHECKED : BST_UNCHECKED);
		EnableWindow(GetDlgItem(m_hDlg, IDC_CDROM_DRIVE), m_data.getPlayCD());
		
		EnableWindow(GetDlgItem(m_hDlg, IDC_SAVER_WAVE), m_data.getPlayFile() );

		EnableWindow(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), m_data.getPlayFile());

		//get name for the wave file name
		size_t strIndex=0;
		AWCString waveFile;
		if ( string::npos != (strIndex = m_data.m_waveFile.rfind("\\")) &&
			(strIndex < m_data.m_waveFile.length()-1) )
		{
         waveFile.eqSubString(m_data.m_waveFile, strIndex+1, m_data.m_waveFile.length()-1);
			//waveFile = m_data.m_waveFile(strIndex+1, m_data.m_waveFile.length()-strIndex-1);
		}

      SetWindowText(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), getDisplayName(waveFile, MAX_SOUND_NAME_DISPLAY_LENGTH).c_str());
		fillCDROMList(); //does nothing if there are already entries in the list
		if (CB_ERR  == SendDlgItemMessage(m_hDlg, IDC_CDROM_DRIVE, CB_SELECTSTRING, 
								(WPARAM)0, (LPARAM)(LPCSTR)m_data.m_cdrom.c_str()))
		{
			SendDlgItemMessage(m_hDlg, IDC_CDROM_DRIVE, CB_SETCURSEL, 
								(WPARAM)0, (LPARAM)0);
		}
	}//else //toControls
}//void AlteredLauncherDlg::transferData(AWBoolean fromControls)


void
AWSoundSettingsDlg::gameMusicCheck()
{  //if we're setting this, disable the file picker
   AWBoolean testVal = IsDlgButtonChecked(m_hDlg, IDC_GAME_MUSIC)!=BST_CHECKED;
   EnableWindow(GetDlgItem(m_hDlg, IDC_GAME_SOUNDEFFECTS), !testVal);
   EnableWindow(GetDlgItem(m_hDlg, IDC_SAVER_WAVE), testVal);
   EnableWindow(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), testVal);
   EnableWindow(GetDlgItem(m_hDlg, IDC_CDROM_DRIVE), FALSE);
}//void AWSoundSettingsDlg::gameMusicCheck()


void
AWSoundSettingsDlg::noSoundsCheck()
{
   //if we're setting this, disable the file picker
   AWBoolean testVal = IsDlgButtonChecked(m_hDlg, IDC_NO_SOUNDS_CHECK)!=BST_CHECKED;
   EnableWindow(GetDlgItem(m_hDlg, IDC_SAVER_WAVE), testVal);
   EnableWindow(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), testVal);
   EnableWindow(GetDlgItem(m_hDlg, IDC_CDROM_DRIVE), FALSE);
   EnableWindow(GetDlgItem(m_hDlg, IDC_GAME_SOUNDEFFECTS), FALSE);
}//void AWSoundSettingsDlg::noSoundsCheck()


void
AWSoundSettingsDlg::playWaveCheck()
{
    //if we're setting this, enable the file picker
    AWBoolean boolVal = IsDlgButtonChecked(m_hDlg, IDC_PLAY_WAVE_CHECK)==BST_CHECKED;
    EnableWindow(GetDlgItem(m_hDlg, IDC_SAVER_WAVE), boolVal);
    EnableWindow(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), boolVal);
    if (boolVal)
    {	//if we have an entry for the sound file and it is NOT a WAV
	    //then clear it
       if (string::npos == m_data.m_waveFile.find(".wav"))
	    {
		    m_data.m_waveFile = "";
		    SetDlgItemText(m_hDlg, IDC_SAVER_WAVE, m_data.m_waveFile.c_str());
	    }
    }
    EnableWindow(GetDlgItem(m_hDlg, IDC_CDROM_DRIVE), FALSE);
    EnableWindow(GetDlgItem(m_hDlg, IDC_GAME_SOUNDEFFECTS), FALSE);
}//void AWSoundSettingsDlg::playWaveCheck()



void
AWSoundSettingsDlg::playAudioCDCheck()
{   //if we're setting this, disable the file picker
	AWBoolean boolVal = IsDlgButtonChecked(m_hDlg, IDC_PLAY_AUDIOCD_CHECK)!=BST_CHECKED;
	EnableWindow(GetDlgItem(m_hDlg, IDC_SAVER_WAVE), boolVal);
	EnableWindow(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), boolVal);
	EnableWindow(GetDlgItem(m_hDlg, IDC_CDROM_DRIVE), TRUE);
   EnableWindow(GetDlgItem(m_hDlg, IDC_GAME_SOUNDEFFECTS), FALSE);
}//void AWSoundSettingsDlg::playAudioCDCheck()



void
AWSoundSettingsDlg::launchHelperCheck()
{   //if we're setting this, enable the file picker
	AWBoolean boolVal = IsDlgButtonChecked(m_hDlg, IDC_LAUNCH_HELPER_CHECK)==BST_CHECKED;
	EnableWindow(GetDlgItem(m_hDlg, IDC_SAVER_WAVE), boolVal);
	EnableWindow(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), boolVal);
	EnableWindow(GetDlgItem(m_hDlg, IDC_CDROM_DRIVE), FALSE);
   EnableWindow(GetDlgItem(m_hDlg, IDC_GAME_SOUNDEFFECTS), FALSE);
}//void AWSoundSettingsDlg::launchHelperCheck()



void
AWSoundSettingsDlg::pickWave()
{  //MODIFIED 6/11/99 to allow picking of any file if using external helper
	//to play sounds
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[300];       // buffer for filename
	szFile[0]=NULL;
	AWCString dir(m_data.m_waveFile);
   if (string::npos != dir.find("\\"))
	{//need to remove the filename for specification of initial directory
	 //for openfile dialog
		size_t index=0;
		if (string::npos != (index = dir.rfind("\\")))
		{
			if (index+1 < dir.length()) 
         {
            dir.replace(index+1, dir.length()-index-1, "");
         }
		}//if (string::npos != (index = dir.rfind("\\")))
	}//if (string::npos != dir.find("\\"))
	// Initialize OPENFILENAME
	char fileNameOnly[400];
	fileNameOnly[0]=NULL;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hDlg;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);

	if (IsDlgButtonChecked(m_hDlg, IDC_PLAY_WAVE_CHECK)==BST_CHECKED)
	{
		ofn.lpstrFilter = "Wave Files\0*.wav\0";
		ofn.lpstrDefExt = "wav";
	}
	else
	{
		ofn.lpstrFilter = "All Files\0*.*\0";
		ofn.lpstrDefExt = NULL;
	}

	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = fileNameOnly;
	ofn.nMaxFileTitle = 99;
	ofn.lpstrTitle  = "Choose your Sounds...";
	
	/*use the initial dir*/
	ofn.lpstrInitialDir = dir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOTESTFILECREATE;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn)==TRUE) 
	{
		//record the full path
		m_data.m_waveFile = ofn.lpstrFile;
		//but just show the user the file name  
      SetWindowText(GetDlgItem(m_hDlg, IDC_PICK_SAVER_WAVE), getDisplayName(ofn.lpstrFileTitle, MAX_SOUND_NAME_DISPLAY_LENGTH).c_str());
	}
}//void AWSoundSettingsDlg::pickWave()



AWCString
AWSoundSettingsDlg::getDisplayName(const AWCString& fullName, int maxLen)
{
   AWCString showName(fullName);
   int len = showName.size();
   if (len>maxLen)
   {
      showName.replace(10, len-maxLen, "...");
   }
   return showName;
}//AWCString AWSoundSettingsDlg::getDisplayName(const AWCString& fullName, int maxLen)


void
AWSoundSettingsDlg::fillCDROMList()
{
	int num = SendDlgItemMessage(m_hDlg, IDC_CDROM_DRIVE, CB_GETCOUNT, 0, 0);
	if (0 == num)
	{
		char driveStr[] = "Z:";
		//LOOK AT ALL DRIVE LETTERS FROM Z TO C
		for (int letter='Z'; letter >= 'C'; letter--)
		{
			driveStr[0] = letter;
			if (DRIVE_CDROM == GetDriveType(driveStr))
			{
				SendDlgItemMessage(m_hDlg, IDC_CDROM_DRIVE, CB_ADDSTRING, (WPARAM)0,
						  (LPARAM)(LPCTSTR)driveStr);
			}
		}//for
	}//if (LB_ERR != sel)
}//void AWSoundSettingsDlg::getCDROMList()