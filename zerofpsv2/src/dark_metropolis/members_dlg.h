#pragma once
#include "gamedlg.h"

class CMembersDlg : public CGameDlg
{
public:
	CMembersDlg();
	~CMembersDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
};
