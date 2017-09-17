#include "AW3DTools/AWToolsCommon.h"
#include "AWContentPage.h"
#include "AWSettingsDlg.h"
#include "AWDownloadMgr.h"
#include <shlobj.h>


#define ALTERED_LIST_FILENAME "AlteredSaver.txt"
#define AW_CUR_VER_PATH       "/updates/saver3_2/"
#define AW_CUR_VER_NAME       "AlteredSaver3.txt"


AWContentPageDlg::AWContentPageDlg(AWConfigWin& cfg, HINSTANCE hInst, 
                                             HWND parent)
            :	AWTabPageDlg(hInst, parent), m_data(cfg), m_gotContentList(FALSE)
{	
   m_IDD = IDD_CONTENTPAGE;
   m_serverDir     = DEFAULT_SERVER_PATH;
   m_listFileName  = ALTERED_LIST_FILENAME;
}



AWBoolean 
AWContentPageDlg::OnDialogMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (msg)
   {
   case WM_AWDONE:
      transferDone((int)wParam);
      break;
   default:
      break;
   };//switch (msg)
   return retVal;
}//AWBoolean AWContentPageDlg::OnDialogMessage(...)



AWBoolean 
AWContentPageDlg::OnDialogCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = FALSE;
   switch (LOWORD(wParam))
   {
   case IDC_GETNEWSCENES:
      OnGetContentList();
      break;

   case IDC_GETSCENE:
      OnGetScene();
      break;

   case IDC_STOP_DOWNLOAD:
      OnCancelDownload();
      break;
   }//switch (id) WM_COMMAND
   return retVal;
}//AWBoolean AWContentPageDlg::OnDialogCommand(...)



AWBoolean     
AWContentPageDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   AWBoolean retVal = AWTabPageDlg::OnInitDialog(hwnd, msg, wParam, lParam);   
   updateControls();
   showDownloadControls(FALSE);
   return retVal;
}//AWBoolean AWContentPageDlg::OnInitDialog(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



AWBoolean 
AWContentPageDlg::OnClose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}//AWBoolean AWContentPageDlg::OnClose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)



void     
AWContentPageDlg::OnCancelDownload()
{
   m_downloadMgr.cancel();
}//void AWContentPageDlg::OnCancelDownload()



void
AWContentPageDlg::updateControls(int reason)
{	
   if (!m_hDlg) return;
	int index = 0;
      //first, clear out any existing listbox content
   SendMessage(GetDlgItem(m_hDlg, IDC_NEWSCENES), LB_RESETCONTENT, (WPARAM)0, 0);
   //now add the available scene names
   //NOTE - List box must NOT be sorted/using simple index selection
   int       selIndex =0;
   AWCString showName;
   size_t    pos;
   if (ID_AWBAD_CONNECT == reason)
   {
      SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"*Problem reaching AlteredWorlds!*");
      SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"Please try again soon!");
   }//if (ID_AWBAD_CONNECT == reason)
   else if (ID_AWINVALID_USER == reason)
   {
      SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"*Please Register AlteredSaver!*");
      SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"*Evaluation period over - sorry!*");
      SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"http://www.AlteredWorlds.com");
   }//else if (ID_AWINVALID_USER == reason)
   else if (0 == m_newSceneList.size())
   {
      if (m_gotContentList && (ID_AWSUCCESS == reason))
      {
         SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                    (LPARAM)(LPCTSTR)"You're up to date -");
         SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                    (LPARAM)(LPCTSTR)"Please check again soon!");
      }//if (ID_AWSUCCESS == reason)
   }//else if (0 == m_newSceneList.entries())
   else 
   {
      for (int i=0; i<(int)m_newSceneList.size(); i++)
      {
         showName = m_newSceneList[i];
         if ( string::npos != (pos = showName.find(".")) )
         {  //remove file extension, counted from FIRST '.'
            //but want to keep the size info which is after \t
            size_t endFileExtension = showName.find("\t");
            if ((string::npos != endFileExtension) && (endFileExtension > pos))
            {
               showName.remove(pos, endFileExtension-pos);
            }
         }//if ( string::npos != (pos = showName.first('.')) )
         index = SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                    (LPARAM)(LPCTSTR)showName.c_str());
      }
      SendDlgItemMessage(m_hDlg, IDC_NEWSCENES, LB_SETCURSEL, (WPARAM)m_selSceneIndex, 0);
   }//else
}//void AlteredLauncherDlg::updateControls(int reason)



void    
AWContentPageDlg::transferDone(int reason)
{
   if (!m_hDlg) return;
   AWCString downloadedSceneName;
   m_downloadMgr.m_downloading = FALSE;
   switch (reason)
   {
   case ID_AWCANCEL:
   case ID_AWREDIRECT:
      //nothing to display
      break;

   case ID_AWSUCCESS:
      m_data.buildSceneList();
      if (m_downloadMgr.m_toString)
      {
         if (!extractFileList())
         {  //try again with adjusted version info
#ifndef _AW_MINIMAL_CONFIG_GUI
            AWCString msg("An new version of AlteredSaver is available!\n\n");
            msg += "This is a very small download - about 240KB (1.5 min@28.8)\n";
            msg += "To upgrade automatically now, please make sure you are\n";
            msg += "connected to the Internet and then press \'Yes\'.\n";
            msg += "\nUpgrading to this new version provides access to the\n";
            msg += "latest AlteredSaver 3D content and features!\n";
            msg += "\n\nTo upgrade another time, please press \'No\'.\n";
            if (IDYES==MessageBox(m_hDlg, msg.c_str(), "AlteredSaver Update Available!", MB_YESNO))
            {
               PostMessage(m_parent, WM_AWGET_UPDATE, (WPARAM)0, (LPARAM)0);
            }
            else 
            {  //user does not wish to update - so point them to the 
               //older scene files for THIS VERSION!
#else
            {
#endif
               m_serverDir    = AW_CUR_VER_PATH;
               m_listFileName = AW_CUR_VER_NAME;
               PostMessage(m_hDlg, WM_COMMAND, (WPARAM)IDC_GETNEWSCENES, (LPARAM)0);
            }
            return;
         }//if (!extractFileList())
      }//if (m_downloadMgr.m_toString)
      else
      {  //get the on-disk list rebuilt 
         AWCString shortSceneName;
         downloadedSceneName = AWScene::getSceneName(m_newSceneList[m_selSceneIndex]);
         m_selSceneIndex = 0;
         //remove any (now) on-disk scenes still in m_newScenesList
         vector<AWCString>::iterator it;
         for (it=m_newSceneList.begin(); it != m_newSceneList.end(); it++)
         {
            shortSceneName = AWScene::getSceneName(*it);
            if (!sceneNotLoaded(shortSceneName))
            {
               if (downloadedSceneName == shortSceneName)
               { //set this as the scene to use for the saver 
                  m_data.m_scene = m_data.getScenePath(shortSceneName);
               }
               m_newSceneList.erase(it);
               it--;
            }//if (!sceneNotLoaded(shortSceneName))
         }//for (it=m_newSceneList.begin(); it != m_newSceneList.end(); it++)
      }
      break;
   }//switch (reason)
   updateControls(reason);
   showDownloadControls(FALSE);
}//void AWContentPageDlg::transferDone(int reason)



void
AWContentPageDlg::showDownloadControls(AWBoolean show)
{
   EnableWindow(GetDlgItem(m_hDlg, IDC_STOP_DOWNLOAD), show);
   EnableWindow(GetDlgItem(m_hDlg, IDC_GETNEWSCENES),  !show);
   EnableWindow(GetDlgItem(m_hDlg, IDC_SIZE_LABEL),    show);
   EnableWindow(GetDlgItem(m_hDlg, IDC_TOTALSIZE),     show);
   EnableWindow(GetDlgItem(m_hDlg, IDC_RATE_LABEL),    show);
   EnableWindow(GetDlgItem(m_hDlg, IDC_RATE),          show);
   
   // reset the position of the progress bar
   if (!show)
   {
      PostMessage(GetDlgItem(m_hDlg, IDC_DOWNLOAD_PROGRESS), PBM_SETPOS, 
                  (WPARAM)0, 0L);
   }
   EnableWindow(GetDlgItem(m_hDlg, IDC_DOWNLOAD_PROGRESS), show);

   if (!show)
   {  //set values clear here, eg size and rate
      SetDlgItemText(m_hDlg, IDC_TOTALSIZE, "");
      SetDlgItemText(m_hDlg, IDC_RATE, "");
   }//if (!show)
   EnableWindow(GetDlgItem(m_hDlg, IDC_NEWSCENES), !show);
   EnableWindow(GetDlgItem(m_hDlg, IDC_GETSCENE),  
                     (m_newSceneList.size() && !show) ? TRUE : FALSE);
} //void AWContentPageDlg::showDownloadControls(AWBoolean show)



void           
AWContentPageDlg::OnGetContentList()
{
   m_selSceneIndex = 0;
   m_newSceneList.clear();
   m_downloadMgr.init(m_listFileName, "", m_hDlg);
   m_downloadMgr.m_serverPath = m_serverDir;
   m_downloadMgr.m_toString = TRUE;
   showDownloadControls(TRUE);
   m_downloadMgr.start();
}//void AWContentPageDlg::OnGetContentList()



AWBoolean 
AWContentPageDlg::isDownloading()
{
   return m_downloadMgr.m_downloading;
}//AWBoolean AWContentPageDlg::isDownloading()



AWCString 
AWContentPageDlg::getLocalSceneDir()
{
   AWCString retVal;
   if (m_data.getNumScenes())
   {
      retVal = m_data.getScene(0);
      size_t pos = retVal.rfind("\\");
      if (string::npos != pos)
      {
         pos++;
         retVal.remove(pos, retVal.length()-pos);
      }
      else
      {
         retVal = "";
      }
   }//if (m_data.getNumScenes())
   return retVal;
}//AWCString AWContentPageDlg::getLocalSceneDir()



AWBoolean
AWContentPageDlg::sceneNotLoaded(const AWCString& token)
{
   AWBoolean retVal = TRUE;
   AWCString name;
   AWCString matchName(AWScene::getSceneName(token));
   if (token.length())
   {
      for (int i=0; i< m_data.getNumScenes(); i++)
      {
         name = AWScene::getSceneName(m_data.getScene(i));
         if (matchName == name)
         {  //exact match of scene names - this scene has already been loaded
            return FALSE;
         }//if (matchName == name)
      }//for (int i=0; i< m_data.getNumScenes(); i++)
   }//if (token.length())
   return retVal;
}//AWBoolean AWContentPageDlg::sceneNotLoaded(token)



void    
AWContentPageDlg::OnGetScene()
{  //first, get the scene information from the listbox
   m_selSceneIndex = SendMessage(GetDlgItem(m_hDlg, IDC_NEWSCENES), LB_GETCURSEL, 
                              (WPARAM)0, (LPARAM)0);
   if (m_data.isValidUser())
   {
      if (m_newSceneList.size() && (LB_ERR != m_selSceneIndex) && 
          (m_selSceneIndex < (int)m_newSceneList.size()) )
      {  //so we have a selected item. List is not sorted, so will
         //correspond to the index in m_newSceneList (!)
         //keep file name, remove size information
         //can do this by looking for \t\t which follows file name
         AWCString name(m_newSceneList[m_selSceneIndex]);
         size_t pos = string::npos;
         if (string::npos != (pos = name.find("\t\t")))
         {
            name.remove(pos);
         }
         //make sure we save it in the (current) local scene directory
         m_downloadMgr.init(name, getLocalSceneDir(), m_hDlg);
         m_downloadMgr.m_serverPath = "/";//m_serverDir;
		 m_downloadMgr.m_serverName = "www.property-platform.com";
         m_downloadMgr.m_toString = FALSE;
		 m_downloadMgr.m_fileName = "export.txt";
         showDownloadControls(TRUE);
         m_downloadMgr.start();     
      }//if ((LB_ERR != itemIndex) && (itemIndex < m_newSceneList.entries()))
      else
      {
         m_selSceneIndex = 0;
      }
   }//if (m_data.isValidUser())
   else
   {
      m_selSceneIndex = 0;
      updateControls(ID_AWINVALID_USER);
   }
}//void AWContentPageDlg::OnGetScene()



AWBoolean
AWContentPageDlg::versionOK(const AWCString& s)
{  //get floating point versin from line formatted like:
   //AlteredSaver v3.0 Content 04122001
   AWBoolean retVal = FALSE;
   size_t startPos = s.find("Saver v");
   size_t endPos   = s.find(" Content");
   if ((AW_NPOS != startPos) && (AW_NPOS != endPos))
   {
      AWCString valStr;
      startPos += 7;
      valStr.eqSubString(s, startPos, endPos);
      float ver = strtod(valStr.c_str(), NULL);
      retVal =  (ver <= AW_PRODUCT_VERSION_ALTEREDSAVER);
   }//if ((AW_NPOS != startPos) && (AW_NPOS != endPos))
   return retVal;
}//AWBoolean AWContentPageDlg::versionOK(const AWCString& s)



AWBoolean
AWContentPageDlg::extractFileList()
{ 
   AWBoolean retVal = TRUE;
   m_newSceneList.clear();
   AWCString testMe = m_downloadMgr.m_retString;
   int len = m_downloadMgr.m_retString.length();
   if (len)
   {  //FIRST - check file header and ensure version matches
      //if version is newer than this client, post message 
      //indicating we should attempt to UPDATE
      //
      //extract all 'tokens' delimited by "\r\n"
      AWCString tokenStr;
      //grab first line (with version information)
      string::size_type startPos = m_downloadMgr.m_retString.find("\x00A")+1;
      tokenStr.eqSubString(m_downloadMgr.m_retString, 0, startPos-3);
      //check the version
      retVal = versionOK(tokenStr);
      if (retVal)
      {  //now we can extract the list of scenes on the server
         //from the returned text, and compare with a dir listing 
         //of the (current) client scenes directory.
         string::size_type endPos   = m_downloadMgr.m_retString.find("\x00A", startPos);
         while (string::npos != endPos)
         {
            tokenStr.eqSubString(m_downloadMgr.m_retString, startPos, endPos-1);
            if (sceneNotLoaded(AWScene::getSceneName(tokenStr)))
            {
               m_newSceneList.push_back(tokenStr);
            }//if (sceneNotLoaded(tokenStr))
            startPos = endPos + 1;
            endPos   = m_downloadMgr.m_retString.find("\x00A", startPos);
            if ((endPos == string::npos)&&(startPos<len-1))
            {
               endPos = len-1;
            }
         }//while (string::npos != endPos)
         m_gotContentList = TRUE;
      }//if (versionOK(tokenStr))
   }//if (len)
   return retVal;
}//AWBoolean AWContentPageDlg::extractFileList()
