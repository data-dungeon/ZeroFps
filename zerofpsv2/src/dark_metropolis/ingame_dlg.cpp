#include "ingame_dlg.h"

CInGameDlg::CInGameDlg() : CGameDlg(&g_kDM)
{

}

CInGameDlg::~CInGameDlg(void)
{
}

void CInGameDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "MenuBn")
	{
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();		

		m_pkDM->StartSong("data/music/dm menu.ogg");
	}
	else
	if(strClickName == "MissionsBn")
	{
		ShowWnd("MembersWnd", false);
		ShowWnd("BuyWnd", false);
		ShowWnd("SellWnd", false);

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
		ShowWnd("BuyWnd", false);
		ShowWnd("SellWnd", false);

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

		if(IsWndVisible("BuyWnd"))
		{
			ShowWnd("BuyWnd", false);
			ShowWnd("SellWnd", false);
		}
		else
		{
			LoadDlg("data/script/gui/dm_shop.lua");
		}
	}
}