#pragma once
#include "gamedlg.h"

class CNewGameDlg : public CGameDlg
{
public:
	CNewGameDlg();
	~CNewGameDlg();
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
};
