#include "mission_dlg.h"

CMissionDlg::CMissionDlg() : CGameDlg(&g_kDM)
{
}

CMissionDlg::~CMissionDlg() 
{
}

void CMissionDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "BriefingBn")
	{
		LoadDlg("data/script/gui/dm_briefing.lua");
		pkMainWnd->Hide();	
	}
	else
	if(strClickName == "MissionCancelBn")
	{
		pkMainWnd->Hide();
	}
}