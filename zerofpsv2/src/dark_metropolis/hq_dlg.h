#pragma once
#include "gamedlg.h"

class CHQDlg : public CGameDlg
{
public:
	CHQDlg();
	~CHQDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
	void OpenDlg();

private:
	P_DMHQ* m_pkHQ;
};
