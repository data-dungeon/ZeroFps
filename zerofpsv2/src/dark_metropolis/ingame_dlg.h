#pragma once
#include "gamedlg.h"

class CInGameDlg : public CGameDlg
{
public:
	CInGameDlg();
	~CInGameDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);

};
