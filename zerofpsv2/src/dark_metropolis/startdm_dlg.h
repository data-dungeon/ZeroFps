#pragma once
#include "gamedlg.h"

class CStartDMDlg : public CGameDlg
{
public:
	CStartDMDlg();
	~CStartDMDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);

private:

	ZeroFps* m_pkFps;
	
};
