#include "dark_metropolis.h"
#include "newgame_dlg.h"

CNewGameDlg::CNewGameDlg() : CGameDlg("DMStartWnd", &g_kDM)
{
	m_fFadeOffset = 0.0f;
}

CNewGameDlg::~CNewGameDlg()
{

}

void CNewGameDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName,
									 bool bRMouseBnClick)
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
				GetGameDlg(GAMEPLAY_DLG)->InitDlg();
			}

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
/*	else
	if(strClickName == "StartNewGameBack")
	{
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();
	}	*/	
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

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
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

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
}

void CNewGameDlg::AddContinueButton()
{
	// Flytta ner alla knappar under "Start" knappen och lägg visa
	// continue knappen

	if(GetGameInfo() == NULL)
	{
		//char* aButtons[] = {
		//	"OptionsBn", "QuitBn", "SaveNewGameBn", "CreditsBn",
		//	"LoadNewGameBn"
		//};

		//for(int i=0; i<sizeof(aButtons)/sizeof(aButtons[1]); i++)
		//{
		//	GetWnd(aButtons[i])->Move(0,24,false,true);
		//	GetWnd(aButtons[i])->SetMoveArea(
		//		GetWnd(aButtons[i])->GetScreenRect(), true);
		//}

		//ShowWnd("ContinueGameBn", true);
		GetWnd("ContinueGameBn")->Show();
	//	GetWnd("ContinueGameBn")->SetPos(365,122,false,true);
	}
}

void CNewGameDlg::StartFade(float fGameTime)
{
	m_fTimeCheck = fGameTime;

	m_fFadeOffset=0;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[0] = 0;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[1] = 0;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[2] = 0;

	list<ZGuiWnd*> kChilds;
	GetWnd("DMStartWnd")->GetChildrens(kChilds);

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);

		pkWnd->GetSkin()->m_afBkColor[0] = 0;
		pkWnd->GetSkin()->m_afBkColor[1] = 0;
		pkWnd->GetSkin()->m_afBkColor[2] = 0;
	}
}

void CNewGameDlg::EndFade()
{
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[0] = 1;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[1] = 1;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[2] = 1;

	list<ZGuiWnd*> kChilds;
	GetWnd("DMStartWnd")->GetChildrens(kChilds);

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);

		pkWnd->GetSkin()->m_afBkColor[0] = 1;
		pkWnd->GetSkin()->m_afBkColor[1] = 1;
		pkWnd->GetSkin()->m_afBkColor[2] = 1;
	}
}

void CNewGameDlg::UpdateFade(float fFrameTime)
{
	static float FADE_TIME = 1.5f;

	static bool first = true;

	if(first)
	{
		first = false;
		return;
	}

	float procent = (fFrameTime) / FADE_TIME;

	if(m_fFadeOffset < FADE_TIME)
		m_fFadeOffset += procent;
		
	if(m_fFadeOffset > 1)
		m_fFadeOffset = 1;

	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[0] = m_fFadeOffset;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[1] = m_fFadeOffset;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[2] = m_fFadeOffset;

	list<ZGuiWnd*> kChilds;
	GetWnd("DMStartWnd")->GetChildrens(kChilds);

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);

		pkWnd->GetSkin()->m_afBkColor[0] = m_fFadeOffset;
		pkWnd->GetSkin()->m_afBkColor[1] = m_fFadeOffset;
		pkWnd->GetSkin()->m_afBkColor[2] = m_fFadeOffset;
	}
}

bool CNewGameDlg::InitDlg()
{
	StartFade(0);
	return true;
}