#include "startdm_dlg.h"

CStartDMDlg::CStartDMDlg() : CGameDlg(&g_kDM)
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
		LoadDlg("data/script/gui/dm_start_new_game.lua");

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
	}
	else
	if(strClickName == "LoadNewGameBn")
	{
		m_pkDM->GUI_LoadSave(false);
	}
	else
	if(strClickName == "SaveNewGameBn")
	{
		m_pkDM->GUI_LoadSave(true);
	}
	else
	if(strClickName == "QuitBn")
	{
		m_pkFps->QuitEngine();
	}
}