#pragma once
#include "gamedlg.h"

class CGamePlayDlg : public CGameDlg
{
public:
	CGamePlayDlg();
	~CGamePlayDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
	
protected:
	bool InitDlg();

	struct AGENT_ICON
	{
		ZGuiButton* pkButton;
		ZGuiLabel* pkLifeProgressbar;
		ZGuiLabel* pkLifeBk;
	};

	AGENT_ICON m_akAgetIcons[5];

};
