#include "newgame_dlg.h"

CNewGameDlg::CNewGameDlg() : CGameDlg(&g_kDM)
{

}

CNewGameDlg::~CNewGameDlg()
{

}

void CNewGameDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "StartNewGameDone")
	{
		if(!strlen(GetWnd("ClanNameEB")->GetText()) == 0)
		{			
			char* szClanName = GetWnd("ClanNameEB")->GetText();
			char* szTeamColor = GetWnd("TeamColorCB")->GetText();

			AddContinueButton();

			if(m_pkDM->StartNewGame(szClanName, szTeamColor))
			{
				m_pkDM->GUI_NewGame(pkMainWnd);
			}
		}
	}
	else
	if(strClickName == "StartNewGameBack")
	{
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();
	}		
	else
	if(strClickName == "SelectNextBaseBn")
	{
		vector<DarkMetropolis::StartBaseInfo*>::iterator n = 
			m_pkDM->m_itStartBase;
			
		n++;

		if(n != m_pkDM->m_vkStartBaseList.end())
		{
			m_pkDM->m_itStartBase++;
			GetWnd("StartBaseNameLabel")->SetText(
				(*m_pkDM->m_itStartBase)->szName);
			GetWnd("StartBaseIconLabel")->SetSkin(
				(*m_pkDM->m_itStartBase)->pkIcon);
		}
	}
	else
	if(strClickName == "SelectPrevBaseBn")
	{
		if(m_pkDM->m_itStartBase != m_pkDM->m_vkStartBaseList.begin())
		{
			m_pkDM->m_itStartBase--;
			GetWnd("StartBaseNameLabel")->SetText(
				(*m_pkDM->m_itStartBase)->szName);
			GetWnd("StartBaseIconLabel")->SetSkin(
				(*m_pkDM->m_itStartBase)->pkIcon);
		}
	}
}

void CNewGameDlg::AddContinueButton()
{
	// Flytta ner alla knappar under "Start" knappen och lägg visa
	// continue knappen

	if(GetGameInfo() == NULL)
	{
		char* aButtons[] = {
			"OptionsBn", "QuitBn", "SaveNewGameBn", "CreditsBn",
			"LoadNewGameBn"
		};

		for(int i=0; i<sizeof(aButtons)/sizeof(aButtons[1]); i++)
		{
			GetWnd(aButtons[i])->Move(0,24,false,true);
			GetWnd(aButtons[i])->SetMoveArea(
				GetWnd(aButtons[i])->GetScreenRect(), true);
		}

		ShowWnd("ContinueGameBn", true);
		GetWnd("ContinueGameBn")->SetPos(365,122,false,true);
	}
}