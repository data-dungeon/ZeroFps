// dark_metropolis_gui.cpp

#include "dark_metropolis.h"

bool GUIPROC(ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kDM.GUI_OnCommand(((int*)params)[0], 
			(((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_LBUTTONDOWN:
		g_kDM.GUI_OnClick(((int*)params)[0], ((int*)params)[1], true, true, win);
		break;
	case ZGM_LBUTTONUP:
		g_kDM.GUI_OnClick(((int*)params)[0], ((int*)params)[1], false, true, win);
		break;
	case ZGM_RBUTTONDOWN:    
		g_kDM.GUI_OnClick(((int*)params)[0], ((int*)params)[1], true, false, win);
		break;
	case ZGM_RBUTTONUP:
		g_kDM.GUI_OnClick(((int*)params)[0], ((int*)params)[1], false, false, 
			win);
		break;
	case ZGM_LBUTTONDBLCLK:
		g_kDM.GUI_OnDoubleClick(((int*)params)[0], ((int*)params)[1], true, win);
		break;
	case ZGM_RBUTTONDBLCLK:
		g_kDM.GUI_OnDoubleClick(((int*)params)[0], ((int*)params)[1], false, win);
		break;
	case ZGM_MOUSEMOVE:
		g_kDM.GUI_OnMouseMove(((int*)params)[1], ((int*)params)[2], 
			((int*)params)[0] == 1 ? true : false, win);
		break;
	case ZGM_SCROLL:
		g_kDM.GUI_OnScroll(((int*)params)[0], ((int*)params)[2], win);
		break;
	case ZGM_CBN_SELENDOK:
		g_kDM.GUI_OnSelectCB(((int*)params)[0], ((int*)params)[1], win);
		break;
	case ZGM_KEYPRESS:
		g_kDM.GUI_OnKeyPress(((int*)params)[0], win);		
		break;
	}
	return true;
}

void DarkMetropolis::GUI_OnCommand(int iID, bool bRMouseBnClick, 
											  ZGuiWnd *pkMainWnd)
{
	string strClickName;
	string strMainWnd;

	if(pkMainWnd)
	{
		strMainWnd = pkMainWnd->GetName();

		list<ZGuiWnd*> kChilds;
		pkMainWnd->GetChildrens(kChilds);

		for(list<ZGuiWnd*>::iterator it=kChilds.begin(); it!=kChilds.end(); it++)
			if((*it)->GetID() == iID)
				strClickName = (*it)->GetName();
	}

	if(strClickName.empty())
		return;

	if(strMainWnd == "DMStartWnd")
	{
		if(strClickName == "StarNewGameBn")
		{
			LoadGuiFromScript(m_pkScript, 
				"data/script/gui/dm_start_new_game.lua");

			pkMainWnd->Hide();

			if(m_vkStartBaseList.empty())
			{
				StartBaseInfo* info;

				int id[] =
				{
					pkTexMan->Load("data/textures/gui/dm/start_base1.bmp", 0),
					pkTexMan->Load("data/textures/gui/dm/start_base2.bmp", 0),
				};

				char* acBaseName[] =
				{
					"Old warehouse",
					"The Library of Death",
				};

				for(int i=0; i<2; i++)
				{
					info = new StartBaseInfo;
					info->pkIcon = new ZGuiSkin(id[i], false);
					info->szName = new char[25];
					strcpy(info->szName, acBaseName[i]);
					m_vkStartBaseList.push_back(info);
				}
			}

			SelListItem("TeamColorCB", "Red");
			m_itStartBase = m_vkStartBaseList.begin();
			GetWnd("StartBaseNameLabel")->SetText((*m_itStartBase)->szName);
			GetWnd("StartBaseIconLabel")->SetSkin((*m_itStartBase)->pkIcon);
		}
		else
		if(strClickName == "LoadNewGameBn")
		{
			GUI_CreateLoadMenu();
		}
		else
		if(strClickName == "QuitBn")
		{
			m_pkFps->QuitEngine();
		}
	}
	else
	if(strMainWnd == "LoadListWnd")
	{
		if(strClickName == "LoadListCancelBn")
		{
			pkGui->KillWndCapture();
			ShowWnd("LoadListWnd", false);
		}
		else
		if(strClickName == "LoadListOKBn")
		{
			char* szClanName = GetSelItem("LoadListLB");

			if(szClanName && strlen(szClanName) > 0)
			{
			
				
				if(LoadGame(szClanName))
				{
					pkGui->KillWndCapture();
					ShowWnd("LoadListWnd", false);
					ShowWnd("DMStartWnd", false);
					
					GUI_NewGame(pkMainWnd);
					
				}

			}
		}
	}
	else
	if(strMainWnd == "StartNewGameWnd")
	{
		if(strClickName == "StartNewGameDone")
		{
			if(!strlen(GetWnd("ClanNameEB")->GetText()) == 0)
			{			
				char* szClanName = GetWnd("ClanNameEB")->GetText();
				char* szTeamColor = GetWnd("TeamColorCB")->GetText();

				if(StartNewGame(szClanName, szTeamColor))
					GUI_NewGame(pkMainWnd);
			}
		}
		else
		if(strClickName == "StartNewGameBack")
		{
			LoadGuiFromScript(m_pkScript, 
				"data/script/gui/dm_start.lua");
			pkMainWnd->Hide();
		}		
		else
		if(strClickName == "SelectNextBaseBn")
		{
			vector<StartBaseInfo*>::iterator n = m_itStartBase;
			n++;

			if(n != m_vkStartBaseList.end())
			{
				m_itStartBase++;
				GetWnd("StartBaseNameLabel")->SetText((*m_itStartBase)->szName);
				GetWnd("StartBaseIconLabel")->SetSkin((*m_itStartBase)->pkIcon);
			}
		}
		else
		if(strClickName == "SelectPrevBaseBn")
		{
			if(m_itStartBase != m_vkStartBaseList.begin())
			{
				m_itStartBase--;
				GetWnd("StartBaseNameLabel")->SetText((*m_itStartBase)->szName);
				GetWnd("StartBaseIconLabel")->SetSkin((*m_itStartBase)->pkIcon);
			}
		}
	}
	else
	if(strMainWnd == "InGamePanelWnd")
	{
		if(strClickName == "MenuBn")
		{
			LoadGuiFromScript(m_pkScript, 
				"data/script/gui/dm_start.lua");
			pkMainWnd->Hide();		

			StartSong("data/music/dm_menu.ogg");
		}
		else
		if(strClickName == "MissionsBn")
		{
			ShowWnd("MembersWnd", false);
			ShowWnd("BuyWnd", false);
			ShowWnd("SellWnd", false);

			ZGuiWnd* pkMissionWnd = GetWnd("MissionWnd");
			if(pkMissionWnd == NULL || !pkMissionWnd->IsVisible())
			{
				LoadGuiFromScript(m_pkScript, 
					"data/script/gui/dm_mission.lua");
			}
			else
			{
				pkMissionWnd->Hide();
			}
		}
		else
		if(strClickName == "MembersBn")
		{
			ShowWnd("MissionWnd", false);
			ShowWnd("BriefingWnd", false);
			ShowWnd("BuyWnd", false);
			ShowWnd("SellWnd", false);

			if(IsWndVisible("MembersWnd"))
				ShowWnd("MembersWnd", false);
			else
			{
				LoadGuiFromScript(m_pkScript, 
					"data/script/gui/dm_members.lua");	

				ClearListbox("MemberSkillsLB");
				AddListItem("MemberSkillsLB", "Heavy Guns : 5");
				AddListItem("MemberSkillsLB", "Rockets : 3");
				AddListItem("MemberSkillsLB", "Lock Pick : 1");
				AddListItem("MemberSkillsLB", "Knife : 1");
			}
		}
		else
		if(strClickName == "ShopBn")
		{
			ShowWnd("MissionWnd", false);
			ShowWnd("MembersWnd", false);
			ShowWnd("BriefingWnd", false);

			if(IsWndVisible("BuyWnd"))
			{
				ShowWnd("BuyWnd", false);
				ShowWnd("SellWnd", false);
			}
			else
			{
				LoadGuiFromScript(m_pkScript, 
					"data/script/gui/dm_shop.lua");	
			}
		}
	}
	else
	if(strMainWnd == "MissionWnd")
	{
		if(strClickName == "BriefingBn")
		{
			LoadGuiFromScript(m_pkScript, 
				"data/script/gui/dm_briefing.lua");
			pkMainWnd->Hide();	
		}
		else
		if(strClickName == "MissionCancelBn")
		{
			pkMainWnd->Hide();
		}
	}
	else
	if(strMainWnd == "BriefingWnd")
	{
		if(strClickName == "BriefingAcceptBn")
		{
			ZGuiWnd* pkInGamePanel = GetWnd("InGamePanelWnd");

			if(pkInGamePanel)
				pkInGamePanel->Hide();

			GetWnd("ReputationLabel")->Hide();
			GetWnd("MoneyLabel")->Hide();

			LoadGuiFromScript(m_pkScript, 
				"data/script/gui/dm_gameplay.lua");
			pkMainWnd->Hide();	
		}
		else
		if(strClickName == "BriefingCancelBn")
		{
			pkMainWnd->Hide();
		}
	}
	else
	if(strMainWnd == "GamePlayPanelWnd")
	{
		if(strClickName == "GamPlayMenuBn")
		{
			LoadGuiFromScript(m_pkScript, 
				"data/script/gui/dm_start.lua");
			pkMainWnd->Hide();	
		}
	}
}

void DarkMetropolis::GUI_OnClick(int x, int y, bool bMouseDown, 
											bool bLeftButton, ZGuiWnd *pkMain)
{
}

void DarkMetropolis::GUI_OnDoubleClick(int x, int y, bool bLeftButton, 
											  ZGuiWnd *pkMain)
{
}

void DarkMetropolis::GUI_OnMouseMove(int x, int y, bool bMouseDown, 
												 ZGuiWnd *pkMain)
{
}

void DarkMetropolis::GUI_OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
}

void DarkMetropolis::GUI_OnSelectCB(int ListBoxID, int iItemIndex, 
												ZGuiWnd *pkMain)
{
}

void DarkMetropolis::GUI_OnKeyPress(int iKey, ZGuiWnd *pkWnd)
{
}

void DarkMetropolis::GUI_CreateLoadMenu()
{
	bool init = GetWnd("LoadListWnd") == NULL;

	ZGuiWnd* pkLoadListWnd = CreateWnd(Wnd, "LoadListWnd", "GuiMainWnd", 
		"", 800/2-150, 50, 300, 400, 0);
	ZGuiWnd* pkTitle = CreateWnd(Label, "LoadListTitle", "LoadListWnd", 
		"Select your clan", 0, 0, 300, 20, 0);
	ZGuiWnd* pkLoadList = CreateWnd(Listbox, "LoadListLB", "LoadListWnd", 
		"", 8, 28, 300-16, 400-60-28, 0);
	ZGuiWnd* pkOK = CreateWnd(Button, "LoadListOKBn", "LoadListWnd", 
		"OK", 50, 400-60+20, 60, 20, 0);
	ZGuiWnd* pkCancel = CreateWnd(Button, "LoadListCancelBn", "LoadListWnd", 
		"Cacel", 200, 400-60+20, 60, 20, 0);

	pkGui->SetCaptureToWnd(pkLoadListWnd);

	if(init) // only ones
	{
		pkLoadListWnd->SetSkin(new ZGuiSkin());
		pkLoadListWnd->GetSkin()->m_bTransparent = true;

		pkTitle->SetSkin(new ZGuiSkin());
		pkTitle->GetSkin()->m_afBkColor[0] = 0.63;
		pkTitle->GetSkin()->m_afBkColor[1] = 0.5f;
		pkTitle->GetSkin()->m_afBkColor[2] = 1;

		ZGuiSkin* BnSkins[3] = {
			new ZGuiSkin(), new ZGuiSkin(), new ZGuiSkin()
		};

		BnSkins[0]->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/bn1u.bmp", 0);
		BnSkins[1]->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/bn1d.bmp", 0);
		BnSkins[2]->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/bn1f.bmp", 0);
		((ZGuiButton*)pkOK)->SetButtonUpSkin(BnSkins[0]);
		((ZGuiButton*)pkOK)->SetButtonDownSkin(BnSkins[1]);
		((ZGuiButton*)pkOK)->SetButtonHighLightSkin(BnSkins[2]);
		((ZGuiButton*)pkCancel)->SetButtonUpSkin(BnSkins[0]);
		((ZGuiButton*)pkCancel)->SetButtonDownSkin(BnSkins[1]);
		((ZGuiButton*)pkCancel)->SetButtonHighLightSkin(BnSkins[2]);

		ClearListbox("LoadListLB");

		vector<string> files;
		if(m_pkBasicFS->ListDir(&files, m_strSaveDirectory.c_str(), true))
		{
			for(int i=0; i<files.size(); i++)
				if(files[i] != string(".."))
					AddListItem("LoadListLB", (char*)files[i].c_str());
		}
	}
}

//bool DarkMetropolis::GUI_NewGame(char* szClanName, char* szTeamColor,ZGuiWnd *pkMainWnd)
bool DarkMetropolis::GUI_NewGame(ZGuiWnd *pkMainWnd)
{
	//start game
//	if(StartNewGame(szClanName, szTeamColor))
//	{				
		pkMainWnd->Hide();
		
		LoadGuiFromScript(m_pkScript,"data/script/gui/dm_ingame.lua");
		
		char* szWndToHide[] =
		{
			"GamePlayChar1Wnd", "GamePlayChar2Wnd", 
			"GamePlayChar3Wnd", "GamePlayChar4Wnd", 
			"GamePlayChar5Wnd", "GamePlayPanelWnd",
			"GamePlayInfoWnd", "MembersWnd",
			"MissionWnd", "BriefingWnd",
			"BuyWnd", "SellWnd", 
			"GamePlayInfoWnd",
		};

		for(int i=0; i<sizeof(szWndToHide)/sizeof(szWndToHide[1]); i++)
			ShowWnd(szWndToHide[i], false);	

		StartSong("data/music/dm_ingame.ogg");
		
		return true;
/*	}
	else
	{
		//här fär du gärna lägga till något klagomål på att en 
		//clan med det namnet redan fins				
	}
	
	return false;*/
}