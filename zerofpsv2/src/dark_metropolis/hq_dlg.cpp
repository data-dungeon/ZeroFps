#include "hq_dlg.h"

CHQDlg::CHQDlg() : CGameDlg(&g_kDM)
{

}

CHQDlg::~CHQDlg(void)
{
}

void CHQDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "HQCloseBn") // the door button
	{
		LoadDlg("data/script/gui/dm_gameplay.lua");
		pkMainWnd->Hide();
		m_pkGui->KillWndCapture();
	}
	else
	if(strClickName == "HQMissionBn") // the noteboard button
	{
		LoadDlg("data/script/gui/dm_mission.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("MissionWnd"));
	}
	else
	if(strClickName == "HQMembersBn") // the folder button
	{
		LoadDlg("data/script/gui/dm_members.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("MembersWnd"));
		ShowWnd("MembersEquipBn", true); // visa equip knappen
		ShowWnd("MembersDropItemBn", false); // dölj drop knappen
	}
	else
	if(strClickName == "HQHireBn") // the telephone button
	{
		LoadDlg("data/script/gui/dm_agents.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("AgentsWnd"));
	}
	else
	if(strClickName == "HQInventoryBn") // the rifle on the wall
	{
		LoadDlg("data/script/gui/dm_itemtransaction.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("ItemTransactionWnd"));

		SetText("RemoveItemBn", "Sell");
		SetText("AddItemBn", "Buy");
	}

	/*if(strClickName == "MenuBn")
	{
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();		

		m_pkDM->StartSong("data/music/dm menu.ogg");
	}
	else
	if(strClickName == "MissionsBn")
	{
		ShowWnd("MembersWnd", false);
		ShowWnd("ItemAddWnd", false);
		ShowWnd("ItemRemoveWnd", false);

		ZGuiWnd* pkMissionWnd = m_pkDM->GetWnd("MissionWnd");
		if(pkMissionWnd == NULL || !pkMissionWnd->IsVisible())
		{
			LoadDlg("data/script/gui/dm_mission.lua");
		}
		else
		{
			pkMissionWnd->Hide();
		}
	}
	else
	if(strClickName == "MembersBn")
	{
		ShowWnd("MissionWnd", false);
		ShowWnd("BriefingWnd", false);
		ShowWnd("ItemAddWnd", false);
		ShowWnd("ItemRemoveWnd", false);

		if(IsWndVisible("MembersWnd"))
			ShowWnd("MembersWnd", false);
		else
		{
			LoadDlg("data/script/gui/dm_members.lua");	

			ClearListbox("MemberSkillsLB");
			AddListItem("MemberSkillsLB", "Heavy Guns : 5");
			AddListItem("MemberSkillsLB", "Rockets : 3");
			AddListItem("MemberSkillsLB", "Lock Pick : 1");
			AddListItem("MemberSkillsLB", "Knife : 1");
		}
	}
	else
	if(strClickName == "ShopBn")
	{
		ShowWnd("MissionWnd", false);
		ShowWnd("MembersWnd", false);
		ShowWnd("BriefingWnd", false);

		if(IsWndVisible("ItemAddWnd"))
		{
			ShowWnd("ItemAddWnd", false);
			ShowWnd("ItemRemoveWnd", false);
		}
		else
		{
			LoadDlg("data/script/gui/dm_itemtransaction.lua");
		}
	}*/
}