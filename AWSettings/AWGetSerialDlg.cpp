// AlteredGetSerialDlg.cpp : Implementation of AlteredGetSerialDlg
#include  <windows.h>
#include "AWGetSerialDlg.h" 
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <winsock.h>     
#include <process.h>

#define AWSERGENNAME /cgi-bin/awGen
#define SERVERNAME "www.alteredworlds.com"
#define INETSETTINGS "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"


// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError()) 

char*          AlteredGetSerialDlg::m_pProxy = 0;
int            AlteredGetSerialDlg::m_proxyPort = 80;
unsigned long  AlteredGetSerialDlg::m_netThread = 0;
char           AlteredGetSerialDlg::m_userName[] = ""; //[USER_INFO_FIELD_LEN];
char           AlteredGetSerialDlg::m_email[] = "";    //[USER_INFO_FIELD_LEN];
char           AlteredGetSerialDlg::m_serial[] = "";   //[ASCII_SERIAL_LEN];
char           AlteredGetSerialDlg::m_delivery[] = ""; //[SERIAL_FIELD_LEN];   
unsigned int   AlteredGetSerialDlg::m_threadID = 0;
char           AlteredGetSerialDlg::m_useName[] = "";
HWND	         AlteredGetSerialDlg::m_hWnd = NULL;

#define WM_AWDONE WM_USER + 1

/////////////////////////////////////////////////////////////////////////////
// AlteredGetSerialDlg

BOOL CALLBACK AlteredGetSerialDlg::DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	BOOL retVal = FALSE;
	AlteredGetSerialDlg* dlg = (AlteredGetSerialDlg*)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	switch ( message ) 
   {
		case WM_INITDIALOG:
			dlg = (AlteredGetSerialDlg*)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)dlg );
			dlg->m_hWnd = hDlg;
			dlg->OnInitDialog(WM_INITDIALOG, wParam, lParam, retVal);
			retVal = TRUE;			

		case WM_DESTROY:
			break;

		case WM_MOUSEACTIVATE:
			break;

      case WM_ACTIVATE:
         UpdateWindow(hDlg);
         break;

      case WM_AWDONE:
         ::EndDialog(dlg->m_hWnd, (int)wParam);
         break;

      case WM_TIMER:
         if (dlg->m_netThread && !dlg->NetThreadStillRunning())
         {
            //EndDialog(dlg->m_hWnd, IDCANCEL);
         }
         break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			break;

		case WM_COMMAND:			
			switch( LOWORD(wParam) ) 
			{
			case IDCANCEL:
				dlg->OnCancel(WM_COMMAND, IDC_AWCANCEL, hDlg, retVal);
				break;
			}//switch( LOWORD(wParam) ) 

		default:
			break;
	}//switch ( message ) 
   return retVal;
}//BOOL CALLBACK AlteredGetSerialDlg::SceneMgrDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )




int
AlteredGetSerialDlg::DoModal()
{
	return DialogBoxParam( m_hInst, MAKEINTRESOURCE(IDD), 
		m_parent ? m_parent : GetActiveWindow(), (DLGPROC)DialogProc, (LPARAM)this); 

}

AlteredGetSerialDlg::AlteredGetSerialDlg(HINSTANCE hInst, HWND parent) : 
                    m_hInst(hInst), m_parent(parent)
{
   m_userName[0]=0;
   m_email[0]=0;
   m_serial[0]=0;
   memset(m_serial, 0, sizeof(m_serial));
   m_delivery[0]=0;
   m_netThread = 0;
   m_threadID = 0;
}

AlteredGetSerialDlg::~AlteredGetSerialDlg()
{
}



LRESULT AlteredGetSerialDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(IDI_AWICON), 
                                  IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR| LR_SHARED);
   SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

   //Create the thread
   //set inspection timer
   SetTimer(m_hWnd, 1, 4000, NULL);
   //create the thread -
   m_netThread = _beginthreadex(NULL, 0, getSerialNum, &m_serial, 0, &m_threadID);
	return 1;  // Let the system set the focus
}



bool           
AlteredGetSerialDlg::NetThreadStillRunning()
{
   DWORD test;
   bool retVal = false;
   if (m_netThread && GetExitCodeThread(&m_netThread, &test))
   {
      retVal = (STILL_ACTIVE == test);
   }
   return retVal;
}


LRESULT 
AlteredGetSerialDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   DWORD test = 0;
   if (NetThreadStillRunning())
   {//need to kill it 
      TerminateThread(&m_netThread, test);
   }
   EndDialog(m_hWnd, IDCANCEL);
	return 0;
}//LRESULT AlteredGetSerialDlg::OnShowConfig(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)




unsigned int  
AlteredGetSerialDlg::getSerialNum(void* serial)
{
   bool retVal = false;
   // Check arguments
	if (!m_userName || !m_email || !m_delivery)
	{
		return retVal;
	}
   //make sure any embedded spaces in the NAME are turned into _
   //make a copy to use from this point on as we don't want to alter
   //the original string in this way (shown to user)
   
   strncpy(m_useName, m_userName, USER_INFO_FIELD_LEN-1);
   m_useName[USER_INFO_FIELD_LEN-1]=0;
   unspace(m_useName);
   if (serial && strlen((char*)serial))
   {
      strncpy(m_serial, (char*)serial, ASCII_SERIAL_LEN-1);
      m_serial[ASCII_SERIAL_LEN-1]=0;
   }//if (serial && strlen(serial))
   else
   {
      char buf[1000];
      WORD wVersionRequested = MAKEWORD(1,1);
      WSADATA wsaData;  
      sprintf(buf, "/cgi-bin/awGen?%s:%s:%s", m_useName, m_email, m_delivery);

      // Initialize WinSock.dll
	   int nRet = WSAStartup(wVersionRequested, &wsaData);
	   if (nRet)
	   {
		   //fprintf(stderr,"\nWSAStartup(): %d\n", nRet);
		   WSACleanup();
		   return retVal;
	   }
	   
	   // Check WinSock version
	   if (wsaData.wVersion != wVersionRequested)
	   {
		   fprintf(stderr,"\nWinSock version not supported\n");
		   WSACleanup();
		   return retVal;
	   }

	   // Set "stdout" to binary mode so that redirection will work
	   // for .gif and .jpg files
	   _setmode(_fileno(stdout), _O_BINARY);

	   // Call GetHTTP() to do all the work
	   getHTTP(SERVERNAME, buf);
      char* snStr = NULL;
      if ((snStr = strstr(buf, "AWSN#")) && (snStr+36-buf < (int)strlen(buf)))
      {
         snStr+=5;
         //we're at the start of the serial number in 16 x %02x format
         //and we know there's enough there to go ahead and get it all
         //retain in 32 char ASCII format for saving in registry
         strncpy(m_serial, snStr, ASCII_SERIAL_LEN-1);
         m_serial[ASCII_SERIAL_LEN-1]=0; //terminate     
      }//if ((snStr = strstr(buf, "AWSN#")) && (snStr+36-buf < strlen(buf)))
	   // Release WinSock
	   WSACleanup();
   }//else / if (serial && strlen(serial))
   retVal = validate(m_useName, m_email, m_delivery, m_serial);
   PostMessage(m_hWnd, WM_AWDONE, (WPARAM)(retVal ? (LPARAM)0 : IDCANCEL), 0);
   return retVal;
}//bool  AlteredGetSerialDlg::getSerialNum()



bool           
AlteredGetSerialDlg::getProxyInfo()
{  //determine if Windows thinks a Proxy is in use - and if so
   //get the server (name) and port number from registry
   bool retVal = false;//no proxy by default
   //get proxy options
   HKEY inetSettings;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, INETSETTINGS, 0, 
                                     KEY_QUERY_VALUE, &inetSettings))
	{
		DWORD valueType;
		DWORD bufSize = 1000;
      BYTE  data[1000];
		//lazy code assumes all keys are correctly of type REG_DWORD
		if (ERROR_SUCCESS == RegQueryValueEx(inetSettings, "ProxyEnable",  NULL, 
							                      &valueType, data, &bufSize ))
		{  
			if (1 == (int)(*(DWORD*)data))
         {  //we *are* using a Proxy - now look for the server info
            bufSize=1000;
		      if (ERROR_SUCCESS == RegQueryValueEx(inetSettings, "ProxyServer",  NULL, 
							                            &valueType, data, &bufSize ))
		      {	//ANSI chars - CAREFUL should really be using UNICODE throughout
               m_pProxy = new char[bufSize+1];
               strncpy(m_pProxy, (const char*)data, bufSize);
               char* portNum = NULL;
               if (portNum = strchr(m_pProxy, ':'))
               {
                  if (strlen(portNum) > 1)
                  {
                     m_proxyPort = atoi(portNum+1);
                  }
                  *portNum=0;
               }//if (portNum = strchr(m_pProxy, ':'))
		      }//if (ERROR_SUCCESS == RegQueryValueEx(inetSettings, "ProxyServer",  NULL,
         }//if (1 == (int)(*(DWORD*)data))
		}//if (ERROR_SUCCESS == RegQueryValueEx(inetSettings, "ProxyEnable",  NULL,
		//close inet settings key
		RegCloseKey(inetSettings);
	}//if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, INETSETTINGS, 0,...
   return retVal;
}//bool AlteredGetSerialDlg::getProxyInfo()

////////////////////////////////////////////////////////////

void 
AlteredGetSerialDlg::getHTTP(char* lpServerName, char* buf)
{  // Use inet_addr() to determine if we're dealing with a name
	// or an address
	IN_ADDR		iaHost;
	LPHOSTENT	lpHostEntry;

   getProxyInfo();
   char* useServer = m_pProxy ? m_pProxy : lpServerName;
   iaHost.s_addr = inet_addr(useServer);
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(useServer);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost, 
						sizeof(struct in_addr), AF_INET);
	}
	if (lpHostEntry == NULL)
	{
		fprintf(stderr, "gethostbyname()");
		return;
	}

	// Create a TCP/IP stream socket
	SOCKET	Socket;	
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket()"); 
		return;
	}

	// Find the port number for the HTTP service on TCP
   // we're either connecting via proxy port number, or direct connect
   // to port 80 on alteredworlds.com
	//LPSERVENT   lpServEnt;
	SOCKADDR_IN saServer;
	
   saServer.sin_port = m_pProxy ? htons(m_proxyPort) : htons(80);

	// Fill in the rest of the server address structure
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

   // Connect the socket
	int nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		fprintf(stderr, "connect()");
		closesocket(Socket);
		return;
	}

	// Format the HTTP request
	char szBuffer[2048];
   szBuffer[0]=0;
   sprintf(szBuffer, "GET %s%s HTTP/1.0\r\nUser-Agent: AlteredWorlds/3.2.5\r\n%s\r\nAccept: */*\r\nPragma: no-cache\r\n\r\n", 
      m_pProxy ? "http://www.alteredworlds.com:80" : "", 
      buf, 
      m_pProxy ? "Host: www.alteredworlds.com:80" : "");
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
		fprintf(stderr, "send()");
		closesocket(Socket);	
		return;
	}
   buf[0]=0;
   int inStrLen = 0;
	while(1)
	{
		// Wait to receive, nRet = NumberOfBytesReceived
		nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
		if (nRet == SOCKET_ERROR)
		{
			fprintf(stderr, "recv() error\n");
			break;
		}
      else if (0 == nRet)
      {  //server closed the connection
         szBuffer[inStrLen]=0;
         break;
      }
      else
      {
         inStrLen += nRet;
         szBuffer[nRet]=0;
         if (inStrLen <  1024)
         {
            strcat(buf, szBuffer);
         }
      }
      fprintf(stderr,"\nrecv() returned %d bytes", nRet);		
	}
	closesocket(Socket);	
}//void AlteredGetSerialDlg::getHTTP(char* lpServerName, char* buf)

