#pragma once
#include "gamedlg.h"

class CMissionDlg : public CGameDlg
{
public:
	CMissionDlg();
	~CMissionDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
};
