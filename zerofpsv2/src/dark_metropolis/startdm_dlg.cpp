#include "dark_metropolis.h"
#include "startdm_dlg.h"

CStartDMDlg::CStartDMDlg() : CGameDlg("StartNewGameWnd", &g_kDM)
{
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
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
}

