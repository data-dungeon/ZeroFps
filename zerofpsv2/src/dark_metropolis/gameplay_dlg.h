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
	void SelectAgent(int iAgent);
	DMCharacterStats* GetAgentStats(int iAgent);

	struct AGENT_ICON
	{
		ZGuiButton* pkButton;
		ZGuiLabel* pkLifeProgressbar;
		ZGuiLabel* pkLifeBk;
		int iAgentObjectID;
	};

	AGENT_ICON m_akAgetIcons[5];

};
