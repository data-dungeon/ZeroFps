#include "dark_metropolis.h"
#include "startdm_dlg.h"

char* CStartDMDlg::Labels[] =
{
	"loggaLabel",
	"dvoidLabel",
	"zeromLabel",
	"vimLabel",
	"zebLabel",
	"eldLabel",
	"manfredLabel",
};

CStartDMDlg::CStartDMDlg() : CGameDlg("StartNewGameWnd", &g_kDM)
{
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_bPlayIntro = false;
	m_fFadeOffset = 0;
}

CStartDMDlg::~CStartDMDlg()
{
}

void CStartDMDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "StarNewGameBn")
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 

		LoadDlg("data/script/gui/dm_start_new_game.lua");
	//	m_pkGui->PlaceWndFrontBack(GetWnd("StartNewGameWnd"), false);

		pkMainWnd->Hide();

		if(m_pkDM->m_vkStartBaseList.empty())
		{
			DarkMetropolis::StartBaseInfo* info;

			int id[] =
			{
				GetTexID("data/textures/gui/dm/start_base1.bmp"),
				GetTexID("data/textures/gui/dm/start_base2.bmp"),
			};

			char* acBaseName[] =
			{
				"Old warehouse",
				"The Library of Death",
			};

			for(int i=0; i<2; i++)
			{
				info = new DarkMetropolis::StartBaseInfo;
				info->pkIcon = new ZGuiSkin(id[i], false);
				info->szName = new char[25];
				strcpy(info->szName, acBaseName[i]);
				m_pkDM->m_vkStartBaseList.push_back(info);
			}
		}

		SetText("ClanNameEB", "olle");
		SelListItem("TeamColorCB", "Red");

		m_pkDM->m_itStartBase = m_pkDM->m_vkStartBaseList.begin();
		GetWnd("StartBaseNameLabel")->SetText((*m_pkDM->m_itStartBase)->szName);
		GetWnd("StartBaseIconLabel")->SetSkin((*m_pkDM->m_itStartBase)->pkIcon);

		ShowWnd("StartNewGameWnd", true);
		m_pkGui->PlaceWndFrontBack(GetWnd("StartNewGameWnd"), true);
	}
	else
	if(strClickName == "LoadNewGameBn")
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
		m_pkDM->GUI_LoadSave(false);
	}
	else
	if(strClickName == "SaveNewGameBn")
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
		m_pkDM->GUI_LoadSave(true);
	}
	else
	if(strClickName == "QuitBn")
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
		m_pkFps->QuitEngine();
	}
	else
	if(strClickName == "ContinueGameBn")
	{
		pkMainWnd->Hide();
		LoadDlg("data/script/gui/dm_gameplay_2.lua");
		GetGameDlg(GAMEPLAY_DLG)->InitDlg();
		m_pkGui->KillWndCapture(); 

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "CreditsBn")
	{
		m_kApa.FADE_TIME = 4.5f;
		m_kApa.oka = 0;
		m_kApa.first = true;
		m_kApa.pkCurrent = NULL;
		m_bPlayIntro = true;
		PlayIntroScreen();
	}
}

void CStartDMDlg::PlayIntroScreen()
{
	LoadDlg("data/script/gui/dm_intro.lua");
	
	int antal = sizeof(CStartDMDlg::Labels) / sizeof(CStartDMDlg::Labels[1]);
	for(int i=0; i<antal; i++)
		GetWnd(CStartDMDlg::Labels[i])->Hide();
}

void CStartDMDlg::Update(float fFrameTime)
{
	int antal_bilder = sizeof(Labels) / sizeof(Labels[1]);

	if(m_bPlayIntro && m_kApa.oka <= antal_bilder)
	{
		if(m_kApa.first)
		{
			m_kApa.first = false;
			return;
		}

		float procent = (fFrameTime) / m_kApa.FADE_TIME;

		if(m_fFadeOffset < m_kApa.FADE_TIME)
			m_fFadeOffset += procent;
		
		if(m_kApa.oka != antal_bilder)
		{
			if(m_fFadeOffset > 1)
			{
				m_fFadeOffset = 1;
				m_kApa.pkCurrent->Hide();
				m_kApa.pkCurrent = NULL;
			}

			if(m_kApa.pkCurrent == NULL)
			{
				m_kApa.pkCurrent = GetWnd(Labels[m_kApa.oka]); 

				if(GetWnd(Labels[m_kApa.oka])->IsVisible() == false)
				{
					GetWnd(Labels[m_kApa.oka])->Show();	
				}

				m_kApa.oka++;
				m_fFadeOffset = 0;
			}
		}
		else
		{
			if(m_fFadeOffset > 0.98)
			{
				GetWnd("DMIntroWnd")->Hide();
				m_bPlayIntro = false;
			}
		}

		m_kApa.pkCurrent->GetSkin()->m_afBkColor[0] =  m_fFadeOffset;
		m_kApa.pkCurrent->GetSkin()->m_afBkColor[1] =  m_fFadeOffset;
		m_kApa.pkCurrent->GetSkin()->m_afBkColor[2] =  m_fFadeOffset;
	}
}

