#pragma once
#include "gamedlg.h"

class CGamePlayDlg : public CGameDlg
{
public:
	CGamePlayDlg();
	~CGamePlayDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick);
	void SelectAgentGUI(int iAgent, bool bSelectModels);
	int GetSelAgentObject() { return m_iSelectedAgent; } // object id
	
	bool InitDlg();
	void UpdateAgentList();
	void UpdatateMap();

protected:
	
	DMCharacterStats* GetAgentStats(int iAgent);

	struct AGENT_ICON
	{
		ZGuiButton* pkButton;
		ZGuiLabel* pkLifeProgressbar;
		int iAgentObjectID;
	};

	AGENT_ICON m_akAgetIcons[5];

private:
	int m_iSelectedAgent;
	bool m_bSkillbarIsOut;
	bool m_bActionbarIsOut;
	bool m_bCharbarIsOut;
	bool m_bItembarIsOut;

};
