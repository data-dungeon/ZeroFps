#include "dark_metropolis.h"
#include "newgame_dlg.h"

CNewGameDlg::CNewGameDlg() : CGameDlg("DMStartWnd", &g_kDM)
{
	m_fFadeOffset = 0.0f;
}

CNewGameDlg::~CNewGameDlg()
{

}

void CNewGameDlg::OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd)
{	
	if(iKey == KEY_RETURN)
	{
		OnCommand(pkCurrentMainWnd, "StartNewGameDone", false);
	}
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
	if(GetGameInfo() == NULL)
	{
		GetWnd("ContinueGameBn")->Show();
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

		if(strcmp(pkWnd->GetName(), "ContinueGameBn") != 0)
			pkWnd->Hide();
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
	{
		if(m_fFadeOffset < 9)
		{
			SetFade(1.0f);
			m_fFadeOffset = 10;
		}
		return;
	}

	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[0] = m_fFadeOffset;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[1] = m_fFadeOffset;
	GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[2] = m_fFadeOffset;

	list<ZGuiWnd*> kChilds;
	GetWnd("DMStartWnd")->GetChildrens(kChilds);

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);

		float f = m_fFadeOffset;
		if(f > 0.75f) 
		{
			f = 0.75f;

			if(strcmp(pkWnd->GetName(), "ContinueGameBn") != 0)
				pkWnd->Show();
		}

		pkWnd->GetSkin()->m_afBkColor[0] = f;
		pkWnd->GetSkin()->m_afBkColor[1] = f;
		pkWnd->GetSkin()->m_afBkColor[2] = f;
	}
}

bool CNewGameDlg::InitDlg()
{
	SetFade(1.0f);
	StartFade(0);
	return true;
}

void CNewGameDlg::SetFade(float fColor)
{
	if(GetWnd("DMStartWnd"))
	{
		GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[0] = fColor;
		GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[1] = fColor;
		GetWnd("DMStartWnd")->GetSkin()->m_afBkColor[2] = fColor;

		list<ZGuiWnd*> kChilds;
		GetWnd("DMStartWnd")->GetChildrens(kChilds);

		list<ZGuiWnd*>::iterator it = kChilds.begin();
		for( ; it != kChilds.end(); it++)
		{
			ZGuiWnd* pkWnd = (*it);
			pkWnd->GetSkin()->m_afBkColor[0] = fColor;
			pkWnd->GetSkin()->m_afBkColor[1] = fColor;
			pkWnd->GetSkin()->m_afBkColor[2] = fColor;

			const type_info& t = typeid(*pkWnd);

			if(t == typeid(ZGuiButton))
			{
				((ZGuiButton*)pkWnd)->GetButtonHighLightSkin()->m_afBkColor[0] = 1.00f;
				((ZGuiButton*)pkWnd)->GetButtonHighLightSkin()->m_afBkColor[1] = 1.00f;
				((ZGuiButton*)pkWnd)->GetButtonHighLightSkin()->m_afBkColor[2] = 1.00f;

				((ZGuiButton*)pkWnd)->GetButtonUpSkin()->m_afBkColor[0] = 0.75f;
				((ZGuiButton*)pkWnd)->GetButtonUpSkin()->m_afBkColor[1] = 0.75f;
				((ZGuiButton*)pkWnd)->GetButtonUpSkin()->m_afBkColor[2] = 0.75f;
			}
		}
	}
}







