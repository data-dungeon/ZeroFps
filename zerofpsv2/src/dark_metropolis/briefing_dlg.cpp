#include "briefing_dlg.h"

CBriefingDlg::CBriefingDlg() : CGameDlg(&g_kDM)
{
}

CBriefingDlg::~CBriefingDlg()
{
}

void CBriefingDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	//if(strClickName == "BriefingAcceptBn")
	//{
	//	ZGuiWnd* pkInGamePanel = GetWnd("InGamePanelWnd");

	//	if(pkInGamePanel)
	//		pkInGamePanel->Hide();

	//	GetWnd("ReputationLabel")->Hide();
	//	GetWnd("MoneyLabel")->Hide();

	//	LoadDlg("data/script/gui/dm_gameplay.lua");
	//	pkMainWnd->Hide();	
	//}
	//else
	//if(strClickName == "BriefingCancelBn")
	//{
	//	pkMainWnd->Hide();
	//}
}
