#include "dark_metropolis.h"
#include "startdm_dlg.h"
#include <ctime>

char* CStartDMDlg::Labels[] =
{
	"loggaLabel",
	"dvoidLabel",
	"zeromLabel",
	"vimLabel",
	"zebLabel",
	"eldLabel",
	"manfredLabel",
	"lastimageLabel",
};

CStartDMDlg::CStartDMDlg() : CGameDlg("StartNewGameWnd", &g_kDM)
{
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_bPlayIntro = false;
	m_fFadeOffset = 0;
	m_iNumPictures = sizeof(CStartDMDlg::Labels) / sizeof(CStartDMDlg::Labels[1]);

//	srand(time(0)); // s�ett slumpfr�f� r�tvisans skull
}

CStartDMDlg::~CStartDMDlg()
{
}

void CStartDMDlg::OnKeyPress(int iKey, ZGuiWnd* pkCurrentMainWnd)
{
	if(iKey == KEY_RETURN)
	{
		OnCommand(pkCurrentMainWnd, "StarNewGameBn", false);
	}
}

void CStartDMDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName,
									 bool bRMouseBnClick)
{
	if(strClickName == "StarNewGameBn")
	{
	/*	m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 

		LoadDlg("data/script/gui/dm_start_new_game.lua");
	
		pkMainWnd->Hide();

		if(m_pkDM->m_vkStartBaseList.empty())
		{
			DarkMetropolis::StartBaseInfo* info;

			int id[] =
			{
				GetTexID("data/textures/gui/start_base1.bmp"),
				GetTexID("data/textures/gui/start_base2.bmp"),
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

		*/

//		GetWnd("ContinueGameBn")->Show();

		if(m_pkDM->StartNewGame("olle", "Red"))
		{
			m_pkDM->GUI_NewGame(pkMainWnd);
			GetGameDlg(GAMEPLAY_DLG)->InitDlg();
		}

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 

		m_pkGui->KillWndCapture(); 

		m_pkDM->PauseGame(false);
	}
	else
	if(strClickName == "LoadNewGameBn")
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
		m_pkDM->GUI_LoadSave(false);
		
		m_pkGui->PlaceWndFrontBack(GetWnd("DMStartWnd"), true); 
		m_pkGui->PlaceWndFrontBack(GetWnd("LoadListWnd"), true); 
	}
	else
	if(strClickName == "SaveNewGameBn")
	{
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
		m_pkDM->GUI_LoadSave(true);
		
		m_pkGui->PlaceWndFrontBack(GetWnd("DMStartWnd"), true); 
		m_pkGui->PlaceWndFrontBack(GetWnd("LoadListWnd"), true); 
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

		m_pkDM->PauseGame(false);
	}
	else
	if(strClickName == "CreditsBn")
	{
		m_kApa.FADE_TIME = 4.5f;
		m_kApa.oka = 0;
		m_kApa.first = true;
		m_kApa.pkCurrent = NULL;
		m_bPlayIntro = true;
		m_vkPlayedPictures.clear();
		PlayIntroScreen();
	}
	else
	if(strClickName == "OptionsBn")
	{
		LoadDlg("data/script/gui/dm_options.lua");
		m_pkGui->SetCaptureToWnd(GetWnd("DMOptionsWnd"));
		((ZGuiSlider*)GetWnd("MusicVolumeSlider"))->SetPos(int(m_pkDM->GetMusicVol()*10), true, false);
		((ZGuiSlider*)GetWnd("SFXVolumeSlider"))->SetPos(int(m_pkAudioSys->GetVolume()*10), true, false);
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 

		m_pkGui->PlaceWndFrontBack(GetWnd("DMStartWnd"), true); 
		m_pkGui->PlaceWndFrontBack(GetWnd("DMOptionsWnd"), true); 
	}
	else
	if(strClickName == "OptionsCloseBn")
	{
		pkMainWnd->Hide();
		m_pkGui->KillWndCapture(); 
		m_pkGui->SetCaptureToWnd(GetWnd("DMStartWnd"));
		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
}

void CStartDMDlg::PlayIntroScreen()
{
	LoadDlg("data/script/gui/dm_intro.lua");
	m_pkGui->SetCaptureToWnd(GetWnd("DMIntroWnd")); 
	m_pkGui->SetFocus(GetWnd("DMIntroWnd"));

	int pw = GetWnd("DMIntroWnd")->GetScreenRect().Width();
	int ph = GetWnd("DMIntroWnd")->GetScreenRect().Height();
	
	for(int i=0; i<m_iNumPictures; i++)
	{
		ZGuiWnd* pkWnd = GetWnd(CStartDMDlg::Labels[i]);

		ZGuiSkin* pkSkin = pkWnd->GetSkin() ;

		if(pkSkin)
		{
			if(pkSkin->m_iBkTexID)
			{
				GetTexMan()->BindTexture(pkSkin->m_iBkTexID);
				GetTexMan()->EditStart(pkSkin->m_iBkTexID);
				Image* surface = GetTexMan()->EditGetImage(pkSkin->m_iBkTexID);
				pkWnd->Resize(surface->m_iWidth, surface->m_iHeight);
				GetTexMan()->EditEnd(pkSkin->m_iBkTexID);
			}
		}

		int x = pw/2 - pkWnd->GetScreenRect().Width()/2; 
		int y = ph/2 - pkWnd->GetScreenRect().Height()/2;

		pkWnd->SetPos(x, y, true, true); 
		pkWnd->Hide();
	}
}

void CStartDMDlg::Update(float fFrameTime)
{
	static float fViewTime = 0;

	if(m_bPlayIntro && m_kApa.oka <= m_iNumPictures)
	{
		if(m_kApa.first)
		{
			m_kApa.first = false;
			return;
		}

		float procent = (fFrameTime) / m_kApa.FADE_TIME;

		if(m_fFadeOffset < m_kApa.FADE_TIME)
			m_fFadeOffset += procent;
		
		if(m_kApa.oka != m_iNumPictures)
		{
			if(m_fFadeOffset > 0.99f)
			{
				fViewTime += procent;

				if(fViewTime > 0.99f)
				{
					m_fFadeOffset = 1;
					m_kApa.pkCurrent->Hide();
					m_kApa.pkCurrent = NULL;
					fFrameTime = fFrameTime;
					fViewTime = 0;
				}
			}

			if(m_kApa.pkCurrent == NULL)
			{
				int picture = GetNextPicture();

				m_kApa.pkCurrent = GetWnd(Labels[picture]); 

				if(GetWnd(Labels[picture])->IsVisible() == false)
				{
					ZGuiWnd* pkWnd = GetWnd(Labels[picture]);

					pkWnd->Show();	

					m_kApa.oka++;
					m_fFadeOffset = 0;
				}
			}
		}
		else
		{
			if(m_fFadeOffset > 0.98)
			{
				m_bPlayIntro = false;
				//CancelIntro();
			}
		}

		m_kApa.pkCurrent->GetSkin()->m_afBkColor[0] =  m_fFadeOffset;
		m_kApa.pkCurrent->GetSkin()->m_afBkColor[1] =  m_fFadeOffset;
		m_kApa.pkCurrent->GetSkin()->m_afBkColor[2] =  m_fFadeOffset;
	}
}

void CStartDMDlg::OnClick(int x, int y, bool bMouseDown, bool bLeftButton, ZGuiWnd *pkMain)
{
	CancelIntro();
}

void CStartDMDlg::CancelIntro()
{
	GetWnd("DMIntroWnd")->Hide();
	m_bPlayIntro = false;	

	m_pkGui->KillWndCapture();
	m_pkGui->SetCaptureToWnd(GetWnd("DMStartWnd")); 
	
	m_pkGui->PlaceWndFrontBack(GetWnd("DMStartWnd"), true); 
}

int CStartDMDlg::GetNextPicture()
{
	// K� alltid f�sta bilden f�st
	if(m_vkPlayedPictures.empty())
	{
		m_vkPlayedPictures.push_back(0);
		return 0;
	}

	// K� alltid sista bilden sist
	if(m_vkPlayedPictures.size() == m_iNumPictures-1)
	{
		m_vkPlayedPictures.push_back(m_iNumPictures-1);
		return m_iNumPictures-1;
	}

	while(1)
	{
		int picture = rand() % m_iNumPictures;
		bool bOK = true;

		if(picture == 0 || picture == m_iNumPictures-1)
		{
			bOK = false;
		}
		else
		{
			for(unsigned int i=0; i<m_vkPlayedPictures.size(); i++)
			{
				if(m_vkPlayedPictures[i] == picture)
				{
					bOK = false;
					break;
				}
			}
		}
		
		if(bOK)
		{
			m_vkPlayedPictures.push_back(picture);
			//printf("Printing credits for: %s\n", Labels[picture]);
			return picture;
		}
	}
}









