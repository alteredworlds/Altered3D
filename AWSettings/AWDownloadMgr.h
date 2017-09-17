#ifndef _AW_DOWNLOAD_MGR_H
#define _AW_DOWNLOAD_MGR_H

#ifndef AW_BUILDING_UPDATE
#include "AWSettings/AWSettingsCommon.h"
#else
#include "AWUpdate/AWUpdateCommon.h"
#endif


#define WM_AWDONE           WM_USER + 1
#define ID_AWREDIRECT       1
#define ID_AWSUCCESS        0
#define ID_AWCANCEL         2
#define ID_AWBAD_CONNECT    3


#define DEFAULT_SERVER_NAME "www.alteredworlds.com"
#define DEFAULT_SERVER_PATH "/updates/current/"
#define DEFAULT_SERVER_PORT 80
 

/////////////////////////////////////////////////////////////////////////////
// AWDownloadMgr
class AWDownloadMgr
{
public:
   AWDownloadMgr(){;}
   ~AWDownloadMgr(){;}

   static void             init(const AWCString& fileName,
                                const AWCString& localPath, HWND hWnd);

   static AWBoolean        start();
   static int              cancel();

   static AWBoolean        extractURLInfo();


   static HWND	            m_hWnd;
   static AWCString        m_fileName;
   static AWCString        m_localPath;
   static AWCString        m_serverPath;
   static AWCString        m_serverName;
   static AWCString        m_retString;
   static AWBoolean        m_toString;
   static AWBoolean        m_cancelled;
   static AWBoolean        m_downloading;

protected:
   static char*            m_pProxy;
   static int              m_proxyPort;
   static int              m_serverPort;
   static unsigned long    m_netThread;
   static unsigned int     m_threadID;

   static AWBoolean        getProxyInfo();
   static int              getData();
   static unsigned int     __stdcall goForIt(void*);
   static void             setContentLength(long contentLength);
   static void             closeConnection(FILE* fd, SOCKET& Socket);
};//class AWDownloadMgr


#endif //sentinel _AW_DOWNLOAD_MGR_H