#include "AWSettingsCommon.h"
#include "AWSettingsDlg.h"


AWBoolean CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,  LPARAM lParam);



//If you need to detect whether another instance already exists, 
// create a named mutex using the CreateMutex function. If the 
// GetLastError function returns ERROR_ALREADY_EXISTS, another 
// instance of your application exists (it created the mutex). 
 


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE mutex = CreateMutex( NULL, TRUE,  "AlteredSettings");
    if (0 != GetLastError())
    {
      return 0;
    }
    else
    {
        AWSettingsDlg lnch(hInstance);
        int test = lnch.DoModal();
    }
	return 1;
}//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
