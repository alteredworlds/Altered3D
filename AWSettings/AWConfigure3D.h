#ifndef __AWCONFIGURE3D_H
#define __AWCONFIGURE3D_H

#include "AWSaverGame/AWConfig.h"

class AWConfigure3DDlg
{
public:
    AWConfigure3DDlg(AWConfig& cfg, HINSTANCE hInst, HWND parent=NULL);

    void    initDialog(HWND hwnd);
    void    ok();
    void    close();
    void    help();
    void    transferData(AWBoolean toControls);

    void    selectCVAs();
    void    selectDLists();
    void    selectVAs();
    void    selectTexCompress();
    void    lockArraysInformation();
    void    selectBump();
    void    selectCubeEnv();
    void    selectBumpSpecular();

    int     DoModal();

    enum { IDD = IDD_AWCONFIGURE3D };

    HWND        m_hWnd, m_parent;
    HINSTANCE   m_hInst;
    AWCString   m_helpPath;
    static AWBoolean CALLBACK DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

    AWConfig&     m_cfg;
};//class AWConfigure3DDlg


#endif //__AWCONFIGURE3D_H