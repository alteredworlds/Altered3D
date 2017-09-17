// AWDownloadMgr.cpp : Implementation of AWDownloadMgr
#include "AWSettings/AWDownloadMgr.h" 
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <winsock.h>    
#include <process.h>
#include <commctrl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INETSETTINGS "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"
#define CONTENT_LEN_TAG "Content-Length: "
#define REDIRECT_TAG "Location: "
#define RATE_SAMPLE_INTERVAL 1000.0f  


#define WM_AWCONTENT_LENGTH WM_USER + 2


extern void file_uncompress(const char* file);


// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError()) 

char*          AWDownloadMgr::m_pProxy      = 0;
int            AWDownloadMgr::m_proxyPort   = 80;
int            AWDownloadMgr::m_serverPort  = DEFAULT_SERVER_PORT;
unsigned long  AWDownloadMgr::m_netThread   = 0;
unsigned int   AWDownloadMgr::m_threadID    = 0;
AWBoolean      AWDownloadMgr::m_toString    = FALSE;
AWBoolean      AWDownloadMgr::m_cancelled   = FALSE;
AWBoolean      AWDownloadMgr::m_downloading = FALSE;

AWCString      AWDownloadMgr::m_fileName    = "";
AWCString      AWDownloadMgr::m_localPath   = "";
AWCString      AWDownloadMgr::m_retString   = "";
AWCString      AWDownloadMgr::m_serverPath  = DEFAULT_SERVER_PATH;
AWCString      AWDownloadMgr::m_serverName  = DEFAULT_SERVER_NAME;


HWND	         AWDownloadMgr::m_hWnd        = NULL;

/////////////////////////////////////////////////////////////////////////////
// AWDownloadMgr


void             
AWDownloadMgr::init(const AWCString& fileName,
                    const AWCString& localPath, HWND hWnd)
{
   m_cancelled  = FALSE;
   m_hWnd       = hWnd;
   m_netThread  = 0;
   m_threadID   = 0;
   m_fileName   = fileName;
   m_localPath  = localPath;
   m_toString   = FALSE;
   m_retString  = "";
   m_serverPort = DEFAULT_SERVER_PORT;
   m_serverPath = DEFAULT_SERVER_PATH;
   m_serverName = DEFAULT_SERVER_NAME;
}//void AWDownloadMgr::init(const AWCString& fileName, const AWCString& localPath, HWND hWnd)



AWBoolean AWDownloadMgr::start()
{  //Create the thread
   m_netThread = _beginthreadex(NULL, 0, goForIt, 0, 0, &m_threadID);
   return TRUE;
}//AWBoolean AWDownloadMgr::start()



int 
AWDownloadMgr::cancel()
{
   m_cancelled = TRUE;
   m_retString = "";
   ::PostMessage(m_hWnd, WM_AWDONE, (WPARAM)ID_AWCANCEL, (LPARAM)0);
	return 0;
}//int AWDownloadMgr::cancel()



AWBoolean           
AWDownloadMgr::getProxyInfo()
{  //determine if Windows thinks a Proxy is in use - and if so
   //get the server (name) and port number from registry
   if (m_pProxy) return TRUE; //we aready found the proxy
   AWBoolean retVal = FALSE;//no proxy by default
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
}//AWBoolean AWDownloadMgr::getProxyInfo()



void
AWDownloadMgr::setContentLength(long contentLength)
{
   char sizeOut[100];
   sizeOut[0]=0;
   double showVal = 0.0;
   if (contentLength > 1048576)
   { //MB
      showVal = (double)contentLength / 1048576.0;
      sprintf(sizeOut, "%3.2f MB", showVal);
   }
   else 
   {//assume KB
      showVal = (double)contentLength / 1024.0;
      sprintf(sizeOut, "%3.2f KB", showVal);
   }
   SendMessage(GetDlgItem(m_hWnd, IDC_TOTALSIZE), WM_SETTEXT, (WPARAM)0, (LPARAM)(LPCTSTR)sizeOut);

   // Set the range for the progress bar.
   PostMessage (GetDlgItem(m_hWnd, IDC_DOWNLOAD_PROGRESS), PBM_SETRANGE32, 0L, (LPARAM)contentLength);
}//void AWDownloadMgr::setContentLength(long contentLength)



unsigned int  
AWDownloadMgr::goForIt(void*)
{
   AWBoolean retVal = 0;
   m_downloading    = TRUE;
   // Check arguments
	if (!m_fileName.length()   || (!m_localPath.length() && !m_toString) || 
       !m_serverPath.length() || !m_serverName.length()  )
	{
		return retVal;
	}
   AWCString showName(m_fileName);
   size_t pos;
   if (string::npos != (pos = showName.find(".")) )
   {
      showName.remove(pos);
   }
   SendMessage(GetDlgItem(m_hWnd, IDC_SCENENAME), WM_SETTEXT, (WPARAM)0, (LPARAM)(LPCTSTR)showName.c_str());
   // Initialize WinSock.dll
   WORD wVersionRequested = MAKEWORD(1,1);
   WSADATA wsaData;  
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		WSACleanup();
		return ID_AWBAD_CONNECT;
	}	   
	// Check WinSock version
	if (wsaData.wVersion != wVersionRequested)
	{
		fprintf(stderr,"\nWinSock version not supported\n");
		WSACleanup();
		return ID_AWBAD_CONNECT;
	}
	// Set "stdout" to binary mode so that redirection will work
	_setmode(_fileno(stdout), _O_BINARY); //not needed?

	retVal = getData();   // Call getData() to do all the work
   if (!m_cancelled && (ID_AWREDIRECT == retVal) && extractURLInfo())
   {  //redirect - extract the server info and do it again
      retVal = getData();
   }
   if (!m_cancelled && (ID_AWSUCCESS == retVal) && 
       (m_fileName.contains(".gz") || m_fileName.contains(".tnz")))
   {  //we should decompress it now using zlib
      AWCString temp(m_localPath);
      temp += m_fileName;
      if (AWFile::Exists(temp))
      {
         file_uncompress(temp.c_str());
      }
   }//if (m_fileName.contains(".gz"))
  
	// Release WinSock
	WSACleanup();
   delete m_pProxy;
   m_pProxy = NULL;
   //send a message to dialog window indicating finished
   if (ID_AWCANCEL != retVal)
   {
      PostMessage(m_hWnd, WM_AWDONE, (WPARAM)retVal, 0);
   }
   return retVal;
}//AWBoolean  AWDownloadMgr::goForIt()



AWBoolean
AWDownloadMgr::extractURLInfo()
{//from m_retString into
   //http://www.somewhere.com/some/or/other/document.tnt.gz
   //must be a more elegant way of doing this
   if (!m_retString.length()) return FALSE;
   //remove leading http://, if any
   size_t pos = m_retString.find("http://");
   if (string::npos != pos)
   {
      m_retString.remove(0, 7);
   }
   //get sever name and possible port?
   pos = m_retString.find("/");
   if ((string::npos != pos) && (pos > 1))
   {
      m_serverName.eqSubString(m_retString, 0, pos-1);
      m_retString.remove(0, pos);
      //server name may include a port number
      pos = m_retString.find(":");
      if (string::npos != pos)
      {
         if (pos < m_serverName.length()-2)
         {
            m_serverPort = atoi(m_serverName.c_str()+pos+1);
         }//if (pos < m_serverName.length()-2)
         m_serverName.remove(pos); 
      }//if (string::npos != pos)
   }//if ((string::npos != pos) && (pos > 1))
   //now get the server path
   pos = m_retString.rfind("/");
   if ((string::npos != pos) & (pos < m_retString.length()-2))
   {
      m_serverPath.eqSubString(m_retString, 0, pos);
      m_retString.remove(0, pos+1);
   }//if ((string::npos != pos) & (pos < m_retString.length()-2))
   //the remainder should be the file name...
   m_fileName = m_retString;
   return TRUE;
}//AWBoolean AWDownloadMgr::extractURLInfo()


void
AWDownloadMgr::closeConnection(FILE* fd, SOCKET& Socket)
{
   if (fd)
   {
      fflush(fd);
      fclose(fd);
      if (m_cancelled)
      { //remove any output file, if created
         AWCString temp(m_localPath);
         temp += m_fileName;
         //remove(temp.c_str());
         //m_cancelled = FALSE;
      }
   }//if (fd)
	closesocket(Socket);  
}//void AWDownloadMgr::closeConnection(FLE* fd, SOCKET& Socket)

////////////////////////////////////////////////////////////
int 
AWDownloadMgr::getData()
{  
   //USES:     m_serverName, m_serverPath, m_fileName
   //USES:    http port m_serverPort on m_serverName
   // Use inet_addr() to determine if we're dealing with a name
	// or an address
	IN_ADDR		iaHost;
	LPHOSTENT	lpHostEntry;

   getProxyInfo();
   const char* useServer = m_pProxy ? m_pProxy : m_serverName.c_str();
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
		return ID_AWBAD_CONNECT;
	}
	// Create a TCP/IP stream socket
	SOCKET	Socket;	
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket()"); 
		return ID_AWBAD_CONNECT;
	}
   // we're either connecting via proxy port number, or direct connect
   // to port 80 on alteredworlds.com
	SOCKADDR_IN saServer;
   saServer.sin_port = m_pProxy ? htons(m_proxyPort) : htons(m_serverPort);
	// Fill in the rest of the server address structure
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
   // Connect the socket
	int nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		fprintf(stderr, "connect()");
		closesocket(Socket);
		return ID_AWBAD_CONNECT;
	}
   //see if we've got some of this file already
	int alreadyDownloadedOffset = 0;
	if (!m_toString)
	{
	   struct _stat st;
	   AWCString temp(m_localPath);
      temp += m_fileName;
      if (_stat(temp.c_str(), &st) != -1 && st.st_size)
      {  //note that we don't check this value is reasonable, here
         //relative to the size of the file to be downloaded
		   alreadyDownloadedOffset = st.st_size;
		}//if (stat(temp.c_str(), &st) != -1 && st.st_size)
   }//if (!m_toString)
	// Format the HTTP request
	char szBuffer[8192];
   AWCString serverAndPort;
   AWCString fullHost;
   if (m_pProxy)
   {
      char temp[20];
      sprintf(temp, ":%d", m_serverPort);
      serverAndPort = m_serverName;
      serverAndPort += temp;
      fullHost      = "Host: ";
      fullHost      += serverAndPort;
   }//if (m_pProxy)
   memset(szBuffer, 0, sizeof(szBuffer));
   if (fullHost.length())
   {
   sprintf(szBuffer, "GET %s%s%s HTTP/1.0\r\nUser-Agent: AlteredWorlds/3.2.0\r\n%s\r\nAccept: */*\r\nPragma: no-cache\r\nRange: bytes=%u-\r\n\r\n", 
      serverAndPort.c_str(), 
      m_serverPath.c_str(),
      m_fileName.c_str(), 
      fullHost.c_str(),
      (unsigned int)alreadyDownloadedOffset);
   }
   else
   {
      sprintf(szBuffer, "GET %s%s%s HTTP/1.0\r\nUser-Agent: AlteredWorlds/3.2.0\r\nAccept: */*\r\nPragma: no-cache\r\nRange: bytes=%u-\r\n\r\n", 
      serverAndPort.c_str(), 
      m_serverPath.c_str(),
      m_fileName.c_str(), 
      (unsigned int)alreadyDownloadedOffset);
   }
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
		fprintf(stderr, "send()");
		closesocket(Socket);	
		return ID_AWBAD_CONNECT;
	}
   AWCString httpHeader;
   int   inStrLen      = 0;
   long  contentLength = 0;
   long  bytesRead     = 0;
   long  displayBytesRead = 0;
   AWBoolean  readingData   = FALSE;
   FILE* fd            = NULL;
   float lastTime = 0; 
   float nextTime = 0;
   float displayUpdate = (float)timeGetTime();
   double rate = 0.0f;
	while (!m_cancelled)
   {  // Wait to receive, nRet = NumberOfBytesReceived
      lastTime = (float)timeGetTime();
		nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
      nextTime = (float)timeGetTime();
		if (nRet == SOCKET_ERROR)
		{
			fprintf(stderr, "recv() error\n");
         closeConnection(fd, Socket);
         return ID_AWBAD_CONNECT; //usual
			break;
		}//if (nRet == SOCKET_ERROR)
      else if (0 == nRet)
      {  //server closed the connection
         szBuffer[inStrLen]=0;
         break;
      }//else if (0 == nRet)
      else //read from input stream
      {
         char* readBuffer = szBuffer;
         if (!readingData)
         {  //we're still reading the HTTP header
            //check to see if this data block contains the
            //end of the header identified by "\r\n\r\n"
            readBuffer = strstr(szBuffer, "\r\n\r\n");
            int numToCopy = nRet;
            if (readBuffer)
            {  //OK, header ending. so terminate it with NULL
               //and copy what there is of it to our buffer
               //move to start of data
               readBuffer += 4;
               *(readBuffer-1) = 0;
               numToCopy = strlen(szBuffer);
               readingData = TRUE;
               nRet = nRet - numToCopy - 1;
            }//if (readBuffer)
            inStrLen += numToCopy;
            szBuffer[numToCopy]=0;
            if (inStrLen <  2048)
            {
               httpHeader += szBuffer;
               //if we requested starting in the middle, expect back confirmation
               //in the form:   Content-Range: bytes X-Y/Z
               if (alreadyDownloadedOffset && !httpHeader.contains("Content-Range"))
               {
                  fprintf(stderr, "AlteredSaver - server doesn't seem to support resuming downloads.\n");
                  fprintf(stderr, "AlteredSaver - starting from the beginning, I'm afraid!\n");
                  alreadyDownloadedOffset = 0;
               }
            }//if (inStrLen <  2048)
            if (readingData)
            {  //check to see if this is a redirect
               char* inStr = (char*)strstr(httpHeader.c_str(), REDIRECT_TAG);
               if (inStr)
               { //content size should be in here...
                  inStr += strlen(REDIRECT_TAG);
                  if (inStr)
                  {  //extract the location of the document
                     //follows from inStr to next space
                     char* redirectEnd = strstr(inStr, "\r\n");
                     if (redirectEnd)
                     {
                        *redirectEnd = 0;
                        m_retString += inStr;
                     }
                     // and now finish. New GET info in m_retString
                     closesocket(Socket);
                     return ID_AWREDIRECT; //indicates redirect
                  }//if (inStr)
               }//if (inStr)
               //can also grab the reported total size 
               //of the data section
               inStr = (char*)strstr(httpHeader.c_str(), CONTENT_LEN_TAG);
               if (inStr)
               { //content size should be in here...
                  inStr += strlen(CONTENT_LEN_TAG);
                  if (inStr)
                  {
                     contentLength = atoi(inStr);
                     setContentLength(contentLength);
                  }//if (inStr)
               }//if (inStr)            
            }//if (readingData)
         }//if (!readingData) 
         if (readingData && contentLength)
         {  //copy nRet bytes to file 
            if (!m_toString)
            {
               if (!fd)
               {  //if file not yet open, open it
                  //if file not yet open, open it
                  AWCString temp(m_localPath);
                  temp += m_fileName;
                  fd = fopen(temp.c_str(), alreadyDownloadedOffset ? "ab" : "wb");
                  //fpos_t pos;
                  //fgetpos(fd, &pos);
               }//if (!fd)          
               if (fd)
               {  //write it to file
                  fwrite(readBuffer, nRet, 1, fd);
                  fflush(fd);              
               }//if (fd)
            }//if (!m_toString)
            else
            {
               readBuffer[nRet-1]=0;
               m_retString += readBuffer;
            }//if (toString)
            bytesRead += nRet;
            // update the position of the progress bar
            if (!m_cancelled)
            {
               PostMessage(GetDlgItem(m_hWnd, IDC_DOWNLOAD_PROGRESS), PBM_SETPOS, 
                        (WPARAM)bytesRead, 0L);
            }
        
            // update the download rate display
            if (nextTime - displayUpdate >= RATE_SAMPLE_INTERVAL) //since last average
            {  //determine download rate in KBps
               rate = ((bytesRead - displayBytesRead) * 1000.0) / ((nextTime  - displayUpdate) * 1024.0);
               displayUpdate = nextTime + RATE_SAMPLE_INTERVAL;
               displayBytesRead = bytesRead;
               sprintf(szBuffer, "%5.1f KBps", rate);
               SendMessage(GetDlgItem(m_hWnd, IDC_RATE), WM_SETTEXT, 
                           (WPARAM)0, (LPARAM)(LPCTSTR)szBuffer);
            }//if (nextTime - displayUpdate >= RATE_SAMPLE_INTERVAL)
         }//if (readingData && contentLength)
      }//else //read from input stream
	}//while(1)  
   closeConnection(fd, Socket);
   return m_cancelled ? ID_AWCANCEL : ID_AWSUCCESS; //usual
}//void AWDownloadMgr::getData()


