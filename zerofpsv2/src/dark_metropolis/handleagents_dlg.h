#pragma once
#include "gamedlg.h"

class CHandleAgents : public CGameDlg
{
public:
	CHandleAgents();
	~CHandleAgents();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName); 
	bool InitDlg();
	int GetSelAgent() { return m_iSelAgent; }

private:

	vector<pair<ZGuiCheckbox*, int> > m_vkCharsInBaseBns; // button, agent object id
	int m_iSelAgent;
	int m_iStartAgent;

	bool SendOutAgent(int iID);
	void UpdateAgentList(int iStartAgent);
};
