#include "AWSettingsCommon.h"
#include "AWNewWorldsDlg.h"

#define AW_NEWWORLDS_HELP "nwindex.htm"

/////////////////////////////////////////////////////////////////////////////
// AWNewWorldsDlg

AWBoolean CALLBACK AWNewWorldsDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	AWBoolean retVal = FALSE;
	AWNewWorldsDlg* dlg = (AWNewWorldsDlg*)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) 
    {
		case WM_INITDIALOG:
			dlg = (AWNewWorldsDlg*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)dlg );
			dlg->initDialog(hDlg);
			return TRUE;			

		case WM_DESTROY:
			return FALSE;

		case WM_MOUSEACTIVATE:
			return FALSE;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			return FALSE;

      case WM_AWDONE:
         dlg->transferDone((int)wParam);
         break;

		case WM_COMMAND:			
			switch( LOWORD(wParam) ) 
			{
         case IDHELP:
            dlg->help();
            break;
         case IDC_GETSCENES:
            dlg->getScenes();
            break;
			case IDOK:
            dlg->close();
            break;
			}//switch( LOWORD(wParam) ) 
			return FALSE;

		default:
			return FALSE;
	}//switch ( message )
   return FALSE;
}//AWBoolean CALLBACK AWNewWorldsDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )




int
AWNewWorldsDlg::DoModal()
{
	return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
		m_parent ? m_parent : GetActiveWindow(), (DLGPROC)DialogProc, (LPARAM)this); 

}



AWNewWorldsDlg::AWNewWorldsDlg(AWConfig& data, const AWCString& helpPath, HINSTANCE hInst, HWND parent) : 
                             m_hWnd(NULL), m_parent(parent), 
							 m_hInst(hInst), m_helpPath(helpPath),
                      m_data(data)
{
   m_helpPath += AW_NEWWORLDS_HELP;
}



AWCString 
AWNewWorldsDlg::getLocalSceneDir()
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
}//AWCString AWNewWorldsDlg::getLocalSceneDir()



AWBoolean
AWNewWorldsDlg::sceneNotLoaded(const AWCString& token)
{
   AWBoolean retVal = TRUE;
   AWCString name;
   if (token.length())
   {
      for (int i=0; i< m_data.getNumScenes(); i++)
      {
         name = m_data.getScene(i);
         size_t namePos = name.rfind("\\");
         if (string::npos != namePos)
         {
            name.remove(0, namePos+1);
            namePos = name.find(".");
            //remove the file extension
            if (string::npos != namePos)
            {
               name.remove(namePos);
            }
            if (token.contains(name))
            {
               return FALSE;
            }
         }//if (string::npos != namePos)
      }//for (int i=0; i< m_data.getNumScenes(); i++)
   }//if (token.length())
   return retVal;
}//AWBoolean AWNewWorldsDlg::sceneNotLoaded(token)


void    
AWNewWorldsDlg::transferDone(int reason)
{
   int i = 0;
   switch (reason)
   {
   case ID_AWCANCEL:
   case ID_AWREDIRECT:
      //nothing to display
      return;

   case ID_AWSUCCESS:
      m_data.buildSceneList();//get the on-disk list rebuilt
      {
         vector<AWCString>::iterator it;
         for (it=m_newSceneList.begin(); it != m_newSceneList.end(); it++)
         {
            if (!sceneNotLoaded(*it))
            {
               m_newSceneList.erase(it);
               it--;
            }//if (!sceneNotLoaded(m_newSceneList[i]))
         }//for (int i=0; i<m_newSceneList.entries(); i++)
      }
      break;
   }//switch (reason)
   transferData(reason);
}//void AWNewWorldsDlg::transferDone(int reason)


void 
AWNewWorldsDlg::initDialog(HWND hwnd)
{
   m_hWnd = hwnd;
   m_data.buildSceneList();
   //
   m_newSceneList.clear();

   AWGetWorldDlg getWorld("AlteredList.txt", "", m_hInst, hwnd);
   getWorld.m_toString = TRUE;
   getWorld.DoModal();
   AWCString retString = getWorld.m_retString;
      
   //now we can build the list of scenes on the server
   //and compare with those listed here on the client
   int len = retString.length();
   if (len)
   {  //extract all 'tokens' delimited by "\r\n"
      AWCString tokenStr;
      string::size_type startPos = retString.find("\x00A")+1;
      string::size_type endPos   = retString.find("\x00A", startPos);
      while (string::npos != endPos)
      {
         tokenStr.eqSubString(retString, startPos, endPos-1);
         m_newSceneList.push_back(tokenStr);
         startPos = endPos + 1;
         endPos   = retString.find("\x00A", startPos);
      }//while (string::npos != endPos)
   }//if (len)
   transferData();
   //load the icon to display in the task bar, etc.
   //HICON hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_AWICON));
   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
   //if there's more than one render choice, output the relevant text
   int items = SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_GETCOUNT, 0, 0);
   //make sure we disable download if timed out!
   EnableWindow(GetDlgItem(m_hWnd, IDC_GETSCENES), AWWinUtils::m_daysLeft);
}//void AWNewWorldsDlg::initDialog(HWND hwnd)




void
AWNewWorldsDlg::transferData(int reason)
{
   int index = 0;
      //first, clear out any existing listbox content
   SendMessage(GetDlgItem(m_hWnd, IDC_NEWSCENES), LB_RESETCONTENT, (WPARAM)0, 0);
   //now add the available scene names
   //NOTE - List box must NOT be sorted/using simple index selection
   int       selIndex =0;
   AWCString showName;
   size_t    pos;
   if (ID_AWBAD_CONNECT == reason)
   {
      SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"*Problem reaching AlteredWorlds!*");
      SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                 (LPARAM)(LPCTSTR)"Please try again soon!");
   }//if (ID_AWBAD_CONNECT == reason)
   else if (0 == m_newSceneList.size())
   {
      if (ID_AWSUCCESS == reason)
      {
         SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                    (LPARAM)(LPCTSTR)"You're up to date -");
         SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
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
         index = SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_ADDSTRING, (WPARAM)0,
                                    (LPARAM)(LPCTSTR)showName.c_str());
      }
      SendDlgItemMessage(m_hWnd, IDC_NEWSCENES, LB_SETCURSEL, (WPARAM)0, 0);
   }//else
}//void AWNewWorldsDlg::transferData(int error)



void
AWNewWorldsDlg::help()
{
    AWWinUtils::showHTMLHelp(m_helpPath);
}//void AWNewWorldsDlg::help()



void
AWNewWorldsDlg::close()
{
    EndDialog(m_hWnd, 0);
}//void AWNewWorldsDlg::close()



void    
AWNewWorldsDlg::getScenes()
{  //first, get the scene information from the listbox
   int itemIndex = SendMessage(GetDlgItem(m_hWnd, IDC_NEWSCENES), LB_GETCURSEL, 
                              (WPARAM)0, (LPARAM)0);
   if ( m_newSceneList.size() && (LB_ERR != itemIndex) && 
        (itemIndex < (int)m_newSceneList.size()) )
   {  //so we have a selected item. List is not sorted, so will
      //correspond to the index in m_newSceneList (!)
      //keep file name, remove size information
      //can do this by looking for \t\t which follows file name
      AWCString name(m_newSceneList[itemIndex]);
      size_t pos = string::npos;
      if (string::npos != (pos = name.find("\t\t")))
      {
         name.remove(pos);
      }
      //make sure we save it in the (current) local scene directory
      AWGetWorldDlg getWorld(name, getLocalSceneDir(), m_hInst, m_hWnd);
      getWorld.DoModal();
   }//if ((LB_ERR != itemIndex) && (itemIndex < m_newSceneList.entries()))
}//void AWNewWorldsDlg::getScenes()
