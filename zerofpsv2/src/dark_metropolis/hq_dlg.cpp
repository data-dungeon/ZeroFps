#include "dark_metropolis.h"
#include "hq_dlg.h"
#include "members_dlg.h"
#include "itemtransaction_dlg.h"
#include "gameplay_dlg.h"

CHQDlg::CHQDlg() : CGameDlg("HQWnd", &g_kDM)
{
	m_pkHQ = NULL;
}

CHQDlg::~CHQDlg(void)
{
}

void CHQDlg::OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd)
{
	if(iKey == KEY_ESCAPE)
	{
		OnCommand(pkCurrentMainWnd, "HQCloseBn", false);
	}
}

void CHQDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName,
							  bool bRMouseBnClick)
{
	if(m_pkHQ == NULL)
	{
		m_pkHQ = (P_DMHQ*) GetDMObject(HQ);
		if(m_pkHQ == NULL)
			printf("error: can't get information about HQ\n");
	}

	if(strClickName == "HQCloseBn")
	{
		LoadDlg("data/script/gui/dm_gameplay_2.lua");
		GetGameDlg(GAMEPLAY_DLG)->InitDlg();
		pkMainWnd->Hide();
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(NULL);
		m_pkDM->PauseGame(false);

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "HQMissionBn")
	{
		LoadDlg("data/script/gui/dm_mission.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("MissionWnd"));
		GetGameDlg(MISSION_DLG)->InitDlg();

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "HQHireBn") 
	{
		LoadDlg("data/script/gui/dm_agents2.lua");
		GetGameDlg(HANDLE_AGENTS_DLG)->InitDlg();
		m_pkGui->SetCaptureToWnd(GetWnd("AgentsWnd"));

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "HQInventoryBn") 
	{		
		LoadDlg("data/script/gui/dm_itemtransaction.lua");
		
		((CItemTransactionDlg*)GetGameDlg(ITEMTRANSACTION_DLG))->m_eViewMode = 
			CItemTransactionDlg::shop;
		GetGameDlg(ITEMTRANSACTION_DLG)->InitDlg();
		
		m_pkGui->SetCaptureToWnd(GetWnd("ItemTransactionWnd"));

		SetText("RemoveItemBn", "Sell");
		SetText("AddItemBn", "Buy");

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
}

void CHQDlg::OpenDlg()
{
	m_pkAudioSys->StartSound("data/sound/computer beep 6.WAV");

	ShowWnd("GamePlayScreen", false);
	ShowWnd("GamePlayPanelWnd", false);

	LoadDlg("data/script/gui/dm_hq.lua");
	m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));

	m_pkDM->PauseGame(true);
}