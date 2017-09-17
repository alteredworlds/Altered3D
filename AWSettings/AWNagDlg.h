#ifndef __AWNAGDLG_H
#define __AWNAGDLG_H

#include "AWSaverGame\AWConfig.h"

 
class AWNagDlg
{
public:
    AWNagDlg(AWConfig& cfg, int daysLeft, const AWCString& helpPath, HINSTANCE hInst, HWND parent=NULL);

    void                initDialog(HWND hwnd);
    void                close();
    void                help();
    void                about();
    void                buyNow();
    void                onTimer(UINT timerID);

    int                 DoModal();
    HWND                DoModeless();

    AWBoolean           realSaverNT();

    enum                { IDD = IDD_NAGDLG };

    HWND                m_hWnd, m_parent;
    HINSTANCE           m_hInst;
    UINT                m_timerID, m_goAwayTimerID;
    AWCString           m_helpPath;
    AWBoolean           m_buyNowVisible;
    int                 m_daysLeft;

    AWConfig&           m_cfg;

    static  AWBoolean   m_noExtraBS;

    static AWBoolean CALLBACK DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
};//class AWNagDlg


#endif //__AWNAGDLG_H