#pragma once
#include "gamedlg.h"

class CHandleAgents : public CGameDlg
{
public:
	CHandleAgents();
	~CHandleAgents();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName, bool bRMouseBnClick); 
	bool InitDlg();
	int GetSelAgent() { return m_iSelAgent; }

	DMCharacterStats m_kViewAgentInfo;

private:

	vector<pair<ZGuiCheckbox*, int> > m_vkCharsInBaseBns; // button, agent object id
	vector<pair<ZGuiCheckbox*, DMCharacterStats> > m_vkAgentsToHireBns; // button, agent object id
	
	int m_iSelAgent;
	int m_iSelAgentToHire;
	
	int m_iStartAgent;
	int m_iStartHireAgent;
	
	bool SendOutAgent(int iID);
	void UpdateAgentInBaseList(int iStartAgent);
	void UpdateAgentToHireList(int iStartAgent);
	int GetNumAgentsInBase();
	int GetNumAgentsToHire();
	void PrintStats(DMCharacterStats* pkStats);
};
