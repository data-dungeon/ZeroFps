#pragma once
#include "gamedlg.h"

class CHQDlg : public CGameDlg
{
public:
	CHQDlg();
	~CHQDlg();

	void OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd);
	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	void OpenDlg();

private:
	P_DMHQ* m_pkHQ;
};
