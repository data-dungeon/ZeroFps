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
		}
		else
		{
			m_akAgetIcons[i].pkButton->Hide();
			m_akAgetIcons[i].pkLifeProgressbar->Hide();
			m_akAgetIcons[i].pkLifeBk->Hide();
		}
	}

	m_bInitialized = true;

	return true;
}