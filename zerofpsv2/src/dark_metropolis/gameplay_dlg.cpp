#include "gameplay_dlg.h"

CGamePlayDlg::CGamePlayDlg() : CGameDlg(&g_kDM)
{
	
}

CGamePlayDlg::~CGamePlayDlg()
{
}

void CGamePlayDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "GamPlayMenuBn")
	{
		ShowWnd("GamePlayScreen", false);
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();
	}	
	else
	if(strClickName == "HQBn")
	{
		if(IsWndVisible("HQWnd"))
			ShowWnd("HQWnd", false);
		else
		{
			LoadDlg("data/script/gui/dm_hq.lua");
			m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
		}
	}
	else
	if(strClickName == "ActiveCharacterPortraitBn")
	{
		LoadDlg("data/script/gui/dm_members.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("MembersWnd"));
		ShowWnd("MembersEquipBn", false); // dölj equip knappen
		ShowWnd("MembersDropItemBn", true); // visa drop knappen
	}
}
