#pragma once
#include "gamedlg.h"

class CBriefingDlg : public CGameDlg
{
public:
	CBriefingDlg();
	~CBriefingDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
	bool InitDlg();
};
