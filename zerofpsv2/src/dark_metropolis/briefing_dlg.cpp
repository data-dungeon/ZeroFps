#include "dark_metropolis.h"
#include "briefing_dlg.h"

CBriefingDlg::CBriefingDlg() : CGameDlg("BriefingWnd", &g_kDM)
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
		LoadDlg("data/script/gui/dm_gameplay_2.lua");
		GetGameDlg(GAMEPLAY_DLG)->InitDlg();
		pkMainWnd->Hide();	
		m_pkGui->KillWndCapture();
		m_pkDM->PauseGame(false); 
	}
	else
	if(strClickName == "BriefingCancelBn")
	{
		pkMainWnd->Hide();
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
	}
}

bool CBriefingDlg::InitDlg()
{
	//SetText("Mission_A_Eb", "Kill Bill");
	return true;
}