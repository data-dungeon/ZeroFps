#pragma once
#include "gamedlg.h"

class CItemTransactionDlg : public CGameDlg
{
public:
	CItemTransactionDlg();
	~CItemTransactionDlg();

	void OnCommand(ZGuiWnd *pkMainWnd, string strClickName);
};
