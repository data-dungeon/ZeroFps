#pragma once
#include "gamedlg.h"

class CGamePlayDlg : public CGameDlg
{
public:
	CGamePlayDlg();
	~CGamePlayDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
	void SelectAgent(int iAgent, bool bSelectModels);
	int GetSelAgentObject() { return m_iSelectedAgent; } // object id
	
protected:
	bool InitDlg();
	
	DMCharacterStats* GetAgentStats(int iAgent);

	struct AGENT_ICON
	{
		ZGuiButton* pkButton;
		ZGuiLabel* pkLifeProgressbar;
		ZGuiLabel* pkLifeBk;
		int iAgentObjectID;
	};

	AGENT_ICON m_akAgetIcons[5];

private:
	int m_iSelectedAgent;

};
