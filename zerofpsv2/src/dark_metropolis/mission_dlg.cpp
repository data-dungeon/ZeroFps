#include "dark_metropolis.h"
#include "mission_dlg.h"

CMissionDlg::CMissionDlg() : CGameDlg("MissionWnd", &g_kDM)
{
}

CMissionDlg::~CMissionDlg() 
{
}

void CMissionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "BriefingBn")
	{
		ShowWnd("MissionWnd", false, true);
		LoadDlg("data/script/gui/dm_briefing.lua");
	}
	else
	if(strClickName == "MissionCancelBn")
	{
		ShowWnd("MissionWnd", false, true);
	}
}