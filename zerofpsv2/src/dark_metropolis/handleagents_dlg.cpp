#include "handleagents_dlg.h"

CHandleAgents::CHandleAgents() : CGameDlg(&g_kDM)
{
}

CHandleAgents::~CHandleAgents()
{
}

void CHandleAgents::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "AgentsClose")
	{
		LoadDlg("data/script/gui/dm_hq.lua");
		pkMainWnd->Hide();	
		m_pkGui->KillWndCapture();
	}
	else
	if(strClickName == "AgentsInfoBn")
	{
		m_pkGui->KillWndCapture();
		LoadDlg("data/script/gui/dm_members.lua");
		ShowWnd("MembersWnd", true, true);

		ShowWnd("MembersEquipBn", false); // d�lj equip knappen
		ShowWnd("MembersDropItemBn", false); // d�lj drop knappen
	}
}
