#ifndef __AWNEWWORLDS_H
#define __AWNEWWORLDS_H

#include "AWGetWorldDlg.h"

class AWNewWorldsDlg
{
public:
    AWNewWorldsDlg(AWConfig& data, const AWCString& helpPath, HINSTANCE hInst, HWND parent=NULL);

    void                initDialog(HWND hwnd);
    void                transferDone(int reason);

    void                close();
    void                help();
    void                transferData(int error=ID_AWSUCCESS);
    AWBoolean           sceneNotLoaded(const AWCString& token);
    AWCString           getLocalSceneDir();

    void                getScenes();

    int                 DoModal();

    enum                { IDD = IDD_AWNEWWORLDS };

    HWND                m_hWnd, m_parent;
    HINSTANCE           m_hInst;
    AWCString           m_helpPath;
    AWConfig&           m_data;
    vector<AWCString>   m_newSceneList;
    static AWBoolean CALLBACK DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
};//class AWNewWorldsDlg


#endif //__AWNEWWORLDS_H