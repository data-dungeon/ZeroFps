#pragma once
#include "gamedlg.h"

class CHandleAgents : public CGameDlg
{
public:
	CHandleAgents();
	~CHandleAgents();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
};
