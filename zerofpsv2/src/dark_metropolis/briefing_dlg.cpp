#include "briefing_dlg.h"

CBriefingDlg::CBriefingDlg() : CGameDlg(&g_kDM)
{
}

CBriefingDlg::~CBriefingDlg()
{
}

void CBriefingDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "BriefingAcceptBn")
	{
		ShowWnd("HQWnd", false, false);
		LoadDlg("data/script/gui/dm_gameplay.lua");
		pkMainWnd->Hide();	
		m_pkGui->KillWndCapture();
	}
	else
	if(strClickName == "BriefingCancelBn")
	{
		pkMainWnd->Hide();
	}
}
