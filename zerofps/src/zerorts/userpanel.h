// userpanel.h: interface for the UserPanel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERPANEL_H__A8A3CF47_3B60_4C3E_9EB6_42353E9A8A2E__INCLUDED_)
#define AFX_USERPANEL_H__A8A3CF47_3B60_4C3E_9EB6_42353E9A8A2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common/dialogbox.h"

#define _DONT_MAIN

class ZeroRTS;
class GuiBuilder;

class UserPanel : public DlgBox
{
public:
	UserPanel(ZeroRTS* pkZeroRts, ZGuiWndProc oMainWndProc);
	virtual ~UserPanel();

	bool Create(int x, int y, char* szResourceFile, char* szDlgName);
	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

private:

	bool OnOpen(int x, int y);
	bool OnClose(bool bSave);

	ZeroRTS* m_pkZeroRts;
	GuiBuilder* m_pkGuiBuilder;

	vector<ZGuiButton*> m_akCommandBns;

};

#endif // !defined(AFX_USERPANEL_H__A8A3CF47_3B60_4C3E_9EB6_42353E9A8A2E__INCLUDED_)
