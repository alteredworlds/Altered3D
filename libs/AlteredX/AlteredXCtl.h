// AlteredXCtl.h : Declaration of the CAlteredXCtl

#ifndef __ALTEREDXCTL_H_
#define __ALTEREDXCTL_H_

#include "resource.h"       // main symbols
#include <atlctl.h>


/////////////////////////////////////////////////////////////////////////////
// CAlteredXCtl
class ATL_NO_VTABLE CAlteredXCtl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IAlteredXCtl,
	public CComControl<CAlteredXCtl>,
	public IPersistStreamInitImpl<CAlteredXCtl>,
	public IOleControlImpl<CAlteredXCtl>,
	public IOleObjectImpl<CAlteredXCtl>,
	public IOleInPlaceActiveObjectImpl<CAlteredXCtl>,
	public IViewObjectExImpl<CAlteredXCtl>,
	public IOleInPlaceObjectWindowlessImpl<CAlteredXCtl>,
	public CComCoClass<CAlteredXCtl, &CLSID_AlteredXCtl>
{
public:
	CAlteredXCtl()
	{
		m_bWindowOnly = TRUE;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ALTEREDXCTL)
DECLARE_NOT_AGGREGATABLE(CAlteredXCtl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAlteredXCtl)
	COM_INTERFACE_ENTRY(IAlteredXCtl)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
END_COM_MAP()

BEGIN_PROP_MAP(CAlteredXCtl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_MSG_MAP(CAlteredXCtl)
	CHAIN_MSG_MAP(CComControl<CAlteredXCtl>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);



// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IAlteredXCtl
public:

	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 3.0 : AlteredXCtl");
		TextOut(di.hdcDraw, 
			(rc.left + rc.right) / 2, 
			(rc.top + rc.bottom) / 2, 
			pszText, 
			lstrlen(pszText));

		return S_OK;
	}
};

#endif //__ALTEREDXCTL_H_
