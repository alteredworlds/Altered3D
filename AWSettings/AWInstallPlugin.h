#ifndef __AWINSTALLPLUGIN_H
#define __AWINSTALLPLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif
LONG WINAPI AWGet1( HWND hWndParent,    LPSTR lpszName,
                   LPSTR lpszEmail,    LPSTR lpszDelivery);
LONG WINAPI AWGet2( HWND hWndParent,    LPSTR lpszName,
                   LPSTR lpszEmail,    LPSTR lpszDelivery,
                   LPSTR lpzSerial);

#ifdef __cplusplus
}
#endif
#endif //__AWINSTALLPLUGIN_H
