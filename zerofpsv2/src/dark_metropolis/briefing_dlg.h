#pragma once
#include "gamedlg.h"

class CBriefingDlg : public CGameDlg
{
public:
	CBriefingDlg();
	~CBriefingDlg();

	void OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd);
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	bool InitDlg();
};
