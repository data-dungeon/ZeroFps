#include "dark_metropolis.h"
#include "gameplay_dlg.h"
#include "members_dlg.h"

CGamePlayDlg::CGamePlayDlg() : CGameDlg("GamePlayScreen", &g_kDM)
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
			GetGameDlg(HQ_DLG)->OpenDlg();
		}
	}
	else
	//
	//	Kolla på info om ens agent, equipa andra vapen etc.
	//
	if(strClickName == "ActiveCharacterPortraitBn") 
	{
		LoadDlg("data/script/gui/dm_members.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("MembersWnd"));

		CMembersDlg* pkMembersDlg = (CMembersDlg*) GetGameDlg(MEMBERS_DLG);

		if(pkMembersDlg)
			pkMembersDlg->SetWindowMode(CMembersDlg::IN_GAME); 
	}
	else
	if(strClickName == "PauseBn")
	{
		if(m_pkDM->GetGameMode() != DarkMetropolis::PAUSED)
			m_pkDM->PauseGame(true);
		else
			m_pkDM->PauseGame(false);
	}
	
	//
	// Markera en agent
	//
	char* icon_name[] ={
		"CharPortBn1","CharPortBn2","CharPortBn3","CharPortBn4","CharPortBn5",
	};

	for(int i=0; i<5; i++)
		if(strClickName == icon_name[i])
			SelectAgent(m_akAgetIcons[i].iAgentObjectID, true);
}

bool CGamePlayDlg::InitDlg()
{
	if(!m_bInitialized)
	{
		ShowWnd("CharPortBn1", false);
		ShowWnd("CharPortBn2", false);
		ShowWnd("CharPortBn3", false);
		ShowWnd("CharPortBn4", false);
		ShowWnd("CharPortBn5", false);
		ShowWnd("CharPortBn6", false);
		ShowWnd("CharPortBn7", false);

		char* szNameID[] =
		{
			"CharPortBn1", "Char1LifeFront1", "Char1LifeBk1",
			"CharPortBn2", "Char1LifeFront2", "Char1LifeBk2",
			"CharPortBn3", "Char1LifeFront3", "Char1LifeBk3",
			"CharPortBn4", "Char1LifeFront4", "Char1LifeBk4",
			"CharPortBn5", "Char1LifeFront5", "Char1LifeBk5",
		};

		for(int i=0; i<5; i++)
		{
			m_akAgetIcons[i].pkButton = (ZGuiButton*)GetWnd(szNameID[i*3]);
			m_akAgetIcons[i].pkLifeProgressbar = (ZGuiLabel*)GetWnd(szNameID[i*3+1]);
			m_akAgetIcons[i].pkLifeBk = (ZGuiLabel*)GetWnd(szNameID[i*3+2]);
		}
	}

	vector<Entity*> kMembersInField;
	GetAllAgentsInField(kMembersInField);
	
	for(int i=0; i<5; i++)
	{
		if(kMembersInField.size() > i)
		{
			m_akAgetIcons[i].pkButton->Show();
			m_akAgetIcons[i].pkLifeProgressbar->Show();
			m_akAgetIcons[i].pkLifeBk->Show();
			m_akAgetIcons[i].iAgentObjectID = kMembersInField[i]->iNetWorkID;

			string icon = GetAgentStats(kMembersInField[i]->iNetWorkID)->m_strIcon;
			string szTexName = string("data/textures/gui/dm/portraits/") + icon;
			SetButtonIcon(m_akAgetIcons[i].pkButton, szTexName);
		}
		else
		{
			m_akAgetIcons[i].pkButton->Hide();
			m_akAgetIcons[i].pkLifeProgressbar->Hide();
			m_akAgetIcons[i].pkLifeBk->Hide();
			m_akAgetIcons[i].iAgentObjectID = -1;
		}
	}

	m_bInitialized = true;

	return true;
}

void CGamePlayDlg::SelectAgent(int iAgent, bool bSelectModels)
{
	Entity* pkAgentObject;
	P_DMCharacter* pkCharProperty;
	DMCharacterStats* pkCharacterStats;

	if((pkAgentObject = GetObject(iAgent)))
	{
		pkCharProperty = (P_DMCharacter*) 
			pkAgentObject->GetProperty("P_DMCharacter");

		ZFAssert(pkCharProperty, 
			"CMembersDlg::SetCharacterStats - No character property\n");

		pkCharacterStats = pkCharProperty->GetStats();

		string szTexName = string("data/textures/gui/dm/portraits/") +
			pkCharacterStats->m_strIcon;

		ZGuiButton* pkActiveCharBn = (ZGuiButton*) 
			GetWnd("ActiveCharacterPortraitBn");

		SetButtonIcon(pkActiveCharBn, szTexName);

		bool bMultiSelect = m_pkDM->m_pkInputHandle->VKIsDown("multiselect");

		m_pkDM->SelectAgent(iAgent, true, !bMultiSelect, true);
	}
}

DMCharacterStats* CGamePlayDlg::GetAgentStats(int iAgent)
{
	Entity* pkAgentObject;
	P_DMCharacter* pkCharProperty;
	DMCharacterStats* pkCharacterStats;

	if((pkAgentObject = GetObject(iAgent)))
	{
		pkCharProperty = (P_DMCharacter*) 
			pkAgentObject->GetProperty("P_DMCharacter");

		ZFAssert(pkCharProperty, 
			"CMembersDlg::SetCharacterStats - No character property\n");

		pkCharacterStats = pkCharProperty->GetStats();
		return pkCharacterStats;
	}

	return NULL;
}
