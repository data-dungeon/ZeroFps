#pragma once
#include "gamedlg.h"

class CHandleAgents : public CGameDlg
{
public:
	CHandleAgents();
	~CHandleAgents();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName); 
	bool InitDlg();

private:

	vector<ZGuiCheckbox*> m_vkCharsInBaseBns;

	bool SendOutAgent(int iID);
};
