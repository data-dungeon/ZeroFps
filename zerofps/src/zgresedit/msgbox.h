// msgbox.h: interface for the MsgBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGBOX_H__B2B2C5F7_0D7D_4714_8444_FFB734E48F2A__INCLUDED_)
#define AFX_MSGBOX_H__B2B2C5F7_0D7D_4714_8444_FFB734E48F2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofps/engine/zgui/zguiwindow.h"

class ZGui;
class ZGuiLabel;
class ZGuiSkin;
class ZGuiButton;

class MsgBox : public ZGuiWnd
{
public:
	MsgBox(ZGui* pkGui, char* szText);
	virtual ~MsgBox();

	ZGuiButton* m_pkOKBn, *m_pkCancelBn;
	ZGuiLabel* m_pkLabel;
	ZGuiSkin* m_pkWndSkin, *m_pkButtonSkinUp, *m_pkButtonSkinDown;
	ZGui* m_pkGui;

};

#endif // !defined(AFX_MSGBOX_H__B2B2C5F7_0D7D_4714_8444_FFB734E48F2A__INCLUDED_)
