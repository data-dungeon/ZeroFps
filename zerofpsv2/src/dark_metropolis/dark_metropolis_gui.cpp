// dark_metropolis_gui.cpp

#include "dark_metropolis.h"
#include "gamedlg.h"
#include "hq_dlg.h"
#include "gameplay_dlg.h"
#include "startdm_dlg.h"
#include "mission_dlg.h"
#include "briefing_dlg.h"
#include "newgame_dlg.h"
#include "itemtransaction_dlg.h"
#include "members_dlg.h"
#include "handleagents_dlg.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"

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
	case ZGM_SELECTLISTITEM:
		g_kDM.GUI_OnSelectLB(((int*)params)[0], ((int*)params)[1], win);
		break;
	case ZGM_KEYPRESS:
		g_kDM.GUI_OnKeyPress(((int*)params)[0], win);		
		break;
	case ZGM_SCN_SETPOS:
		g_kDM.GUI_OnMoveSlider(((int*)params)[0],((int*)params)[1],win);	// id, pos, wnd
		break;
	}
	return true;
}

void DarkMetropolis::GUI_OnIdle()
{
	ZGuiWnd* pkStartWnd = GetWnd("DMStartWnd");

	if(pkStartWnd && pkStartWnd->IsVisible() )
	{
		((CNewGameDlg*) m_pkNewGameDlg)->UpdateFade(m_pkFps->GetFrameTime()); 
		((CStartDMDlg*) m_pkStartDMDlg)->Update(m_pkFps->GetFrameTime());
	}

	if(m_pkGamePlayInfoLabel)
	{
		if(m_strGameInfoText.empty())
		{
			m_pkGamePlayInfoLabel->Hide();
		}
		else
		{
			m_pkGamePlayInfoLabel->Show();
			m_pkGamePlayInfoLabel->SetText((char*)m_strGameInfoText.c_str(), true);

			Rect rc = m_pkGamePlayInfoLabel->GetScreenRect();

			int mx,my;
			m_pkInputHandle->MouseXY(mx,my);

			my+=rc.Height()+16;
			mx-=rc.Width()/2;

			if(my + rc.Height() > m_iHeight)
				my = m_iHeight-rc.Height()-1;
			else
			if(my < 0)
				my = 0;
			
			if(mx + rc.Width() > m_iWidth)
				mx = m_iWidth-rc.Width()-1;
			else
			if(mx < 0)
				mx = 0;

			m_pkGamePlayInfoLabel->SetPos(mx,my,true, true);
		}
	}
}

void DarkMetropolis::GUI_Init()
{
	// Jaja... får väll ge mig dvoid :( Men efter crunchen vill jag ha detta _fixat_
	bool bJagVillHellreHaHWCursorMenWTF = false;
	m_pkGamePlayInfoLabel = NULL;
	m_pkInput->ShowCursor(bJagVillHellreHaHWCursorMenWTF);

	// Create gui script.
	GuiAppLua::Init(&g_kDM, m_pkScript);
	InitGui(m_pkScript,
		"data/textures/text/ms_sans_serif8.bmp",
		"data/script/gui/defskins.lua",
		NULL, bJagVillHellreHaHWCursorMenWTF);

	// Load start sceen.
	LoadGuiFromScript(m_pkScript, "data/script/gui/dm_start.lua");
	pkGui->SetFocus(GetWnd("DMStartWnd"));

	ShowWnd("ContinueGameBn", false);
	
	// Start menu song
	StartSong("data/music/dm menu.ogg");

	// Create all screens
	m_pkStartDMDlg = new CStartDMDlg();
	m_pkNewGameDlg = new CNewGameDlg();
	m_pkHQDlg = new CHQDlg();
	m_pkMissionDlg = new CMissionDlg();
	m_pkBriefingDlg = new CBriefingDlg();
	m_pkGamePlayDlg = new CGamePlayDlg();
	m_pkItemTransactionDlg = new CItemTransactionDlg();
	m_pkMembersDlg = new CMembersDlg();
	m_pkHandleAgents = new CHandleAgents();	

	((CNewGameDlg*) m_pkNewGameDlg)->InitDlg(); 
	((CNewGameDlg*) m_pkNewGameDlg)->SetFade(0.5f);

	pkGui->SetCursor( 0,0, pkTexMan->Load("data/textures/gui/dm/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/dm/cursor_a.bmp", 0), 32, 32);
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

	printf("strClickName = %s, %s\n", strClickName.c_str(), strMainWnd.c_str());

	if(strMainWnd == "DMStartWnd" ||
		strMainWnd == "DMOptionsWnd")
	{
		m_pkStartDMDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "StartNewGameWnd")
	{
		m_pkNewGameDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "HQWnd")
	{
		m_pkHQDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "MembersWnd")
	{
		m_pkMembersDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "MissionWnd")
	{
		m_pkMissionDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "BriefingWnd")
	{
		m_pkBriefingDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "GamePlayPanelWnd" ||
		strMainWnd == "GamePlayScreen" ||
		strMainWnd == "wind_char" ||
		strMainWnd == "skill_bar" ||
		strMainWnd == "action_bar" ||
		strMainWnd == "panel_char" ||
		strMainWnd == "wind_map" ||
		strMainWnd == "item_bar" ||
		strMainWnd == "MissionInfoWnd")
	{
		m_pkGamePlayDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "ItemTransactionWnd" ||
		strMainWnd == "ItemRemoveWnd" ||
		strMainWnd == "ItemAddWnd" )
	{
		m_pkItemTransactionDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "AgentsWnd")
	{		
		m_pkHandleAgents->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	}
	else
	if(strMainWnd == "LoadListWnd")
	{
		if(strClickName == "LoadListCancelBn")
		{
			pkGui->KillWndCapture();
			ShowWnd("LoadListWnd", false);
			pkGui->SetFocus(GetWnd("DMStartWnd"));

			((CNewGameDlg*) m_pkNewGameDlg)->SetFade(1.0f);
			pkGui->PlaceWndFrontBack(GetWnd("DMStartWnd"), true); 
			pkGui->SetCaptureToWnd(GetWnd("DMStartWnd"));
		}
		else
		if(strClickName == "LoadListOKBn")
		{
			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 

			char* szClanName = GetText("SaveLoadFileNameEB");
			if(szClanName && strlen(szClanName) > 0)
			{
				bool bSucess = m_bSaveGame ? SaveGame(szClanName) : 
					LoadGame(szClanName);

				if(bSucess)
				{
					pkGui->KillWndCapture();
					ShowWnd("LoadListWnd", false);
					ShowWnd("DMStartWnd", false);
					GUI_NewGame(pkMainWnd);		
					((CGamePlayDlg*)m_pkGamePlayDlg)->InitDlg();
					PauseGame(false);
					return;
				}
			}
			else
			{
				((CNewGameDlg*) m_pkNewGameDlg)->SetFade(1.0f);
				pkGui->PlaceWndFrontBack(GetWnd("DMStartWnd"), true); 
				pkGui->SetCaptureToWnd(GetWnd("DMStartWnd"));
			}
		}
	}
}

void DarkMetropolis::GUI_OnClick(int x, int y, bool bMouseDown, 
											bool bLeftButton, ZGuiWnd *pkMain)
{
	string strClickName;
	string strMainWnd;

	if(pkMain)
	{
		strMainWnd = pkMain->GetName();
	}

	if(strMainWnd == "MembersWnd")
	{
		m_pkMembersDlg->OnClick(x, y, bMouseDown, bLeftButton, pkMain);
	}

	if(strMainWnd == "DMIntroWnd" /*|| 
		strMainWnd == "StartNewGameWnd"*/)
	{
		m_pkStartDMDlg->OnClick(x, y, bMouseDown, bLeftButton, pkMain);
	}

	if(strMainWnd == "ActiveCharacterPortraitBn")
	{
		if(GetWnd("ActiveCharacterPortraitLabel") && GetWnd("ActiveCharacterPortraitBn"))
		{
			GetWnd("ActiveCharacterPortraitLabel")->m_iZValue = 
				GetWnd("ActiveCharacterPortraitBn")->m_iZValue + 1;

			GetWnd("ActiveCharacterPortraitLabel")->GetParent()->SortChilds();
		}
	}
	
}

void DarkMetropolis::GUI_OnDoubleClick(int x, int y, bool bLeftButton, 
													ZGuiWnd *pkMain)
{
}

void DarkMetropolis::GUI_OnMouseMove(int x, int y, bool bMouseDown, 
												 ZGuiWnd *pkMain)
{
	string strClickName;
	string strMainWnd;

	if(pkMain)
	{
		strMainWnd = pkMain->GetName();
	}

	if(strMainWnd == "MembersWnd")
	{
		m_pkMembersDlg->OnMouseMove(x, y, bMouseDown, pkMain);
	}
}

void DarkMetropolis::GUI_OnScroll(int iID, int iPos, ZGuiWnd *pkMain)
{
	string strMainWnd;

	if(pkMain)
	{
		strMainWnd = pkMain->GetName();
	}

	if(strMainWnd == "MissionWnd")
	{
		m_pkMissionDlg->OnScroll(iID, iPos, pkMain);
	}
}

void DarkMetropolis::GUI_OnSelectCB(int iListBoxID, int iItemIndex, 
												ZGuiWnd *pkMain)
{
	if(pkMain == NULL)
		return;

	ZGuiCombobox* pkComboBox = NULL;
	list<ZGuiWnd*> childs;
	pkMain->GetChildrens(childs);

	list<ZGuiWnd*>::iterator it = childs.begin();
	for(; it != childs.end(); it++)
	{
		ZGuiWnd* pkWnd = (*it);
		if(pkWnd->GetID() == (unsigned int) iListBoxID)
		{
			if(typeid(*pkWnd)==typeid(ZGuiCombobox))
			{
				pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
				break;
			}
		}
	}

	string strMainWnd = pkMain->GetName();

	printf("strMainWnd = %s\n", strMainWnd.c_str());

	if(strMainWnd == "ItemTransactionWnd" && pkComboBox != NULL)
	{
		m_pkItemTransactionDlg->OnSelectCB(pkComboBox, iItemIndex);
	}
}

void DarkMetropolis::GUI_OnSelectLB(int iID, int iIndex, ZGuiWnd* pkMainWnd)
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

	if(strMainWnd == "LoadListWnd")
	{
		if(strClickName == "LoadListLB")
		{
			char* szText = GetSelItem("LoadListLB");
			SetText("SaveLoadFileNameEB", szText);
		}
	}
}

void DarkMetropolis::GUI_OnKeyPress(int iKey, ZGuiWnd *pkWnd)
{
	string strMainWnd = "";

	if(pkWnd)
	{
		strMainWnd = pkWnd->GetName();	
		//printf("pkWnd = %s\n", pkWnd->GetName());
	}
	else
		return;

	if(strMainWnd == "DMStartWnd")
	{
		m_pkStartDMDlg->OnKeyPress(iKey, pkWnd);
	}
	else
	if(strMainWnd == "StartNewGameWnd")
	{
		m_pkNewGameDlg->OnKeyPress(iKey, pkWnd);
	}
	else
	if(strMainWnd == "HQWnd")
	{
		if(GetWnd("HQWnd")->IsVisible())
			m_pkHQDlg->OnKeyPress(iKey, pkWnd);
	}
	else
	if(strMainWnd == "MissionWnd")
	{
		if(GetWnd("MissionWnd")->IsVisible())
			m_pkMissionDlg->OnKeyPress(iKey, pkWnd);
	}
	else
	if(strMainWnd == "MembersWnd")
	{
		m_pkMembersDlg->OnKeyPress(iKey, pkWnd);
	}
	else
	if(strMainWnd == "BriefingWnd")
	{
		m_pkBriefingDlg->OnKeyPress(iKey, pkWnd);
	}
	//else
	//if(strMainWnd == "HQWnd")
	//{
	//	m_pkHQDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	//}
	//else
	//if(strMainWnd == "MembersWnd")
	//{
	//	m_pkMembersDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	//}
	//else
	//if(strMainWnd == "MissionWnd")
	//{
	//	m_pkMissionDlg->OnCommand(pkMainWnd, strClickName, bRMouseBnClick);
	//}
	//else


}

void DarkMetropolis::GUI_OnMoveSlider(int iID, int iPos, ZGuiWnd* pkMainWnd)
{
	string strMainWnd = pkMainWnd->GetName();
	string strCtrl = "";

	if(pkMainWnd)
	{
		list<ZGuiWnd*> kChilds;
		pkMainWnd->GetChildrens(kChilds);

		for(list<ZGuiWnd*>::iterator it=kChilds.begin(); it!=kChilds.end(); it++)
			if((*it)->GetID() == iID)
				strCtrl = (*it)->GetName();
	}
	else
	{
		return;
	}

	if(strCtrl.empty())
		return;

	printf("%s\n", strCtrl.c_str());

	if(strMainWnd == "DMOptionsWnd")
	{
		if(strCtrl == "MusicVolumeSlider")
		{
			// mellan 0-10
			float fNewVol = (float)iPos/10.0f;
			if(iPos < 1)
				fNewVol = 0.001f;

			m_fMusicVolume = fNewVol;

			OggMusic* pkMusic = static_cast<OggMusic*>(
				g_ZFObjSys.GetObjectPtr("OggMusic")); 

			pkMusic->SetVolume(m_fMusicVolume); 
		}
		else
		if(strCtrl == "SFXVolumeSlider")
		{
			// mellan 0-10
			float fSFXVolume = (float)iPos/10.0f;

			if(iPos < 1)
				fSFXVolume = 0.001f;

			m_pkAudioSys->SetVolume(fSFXVolume);
		}
	}
}

void DarkMetropolis::GUI_LoadSave(bool bSave)
{
	bool init = GetWnd("LoadListWnd") == NULL;

	ZGuiWnd* pkLoadListWnd = CreateWnd(Wnd, "LoadListWnd", "GuiMainWnd", 
		"", 800/2-150, 50, 300, 400, 0);
	ZGuiWnd* pkTitle = CreateWnd(Label, "LoadListTitle", "LoadListWnd", 
		"-- Select your clan --", 0, 0, 300, 20-4, 0);
	ZGuiWnd* pkLoadList = CreateWnd(Listbox, "LoadListLB", "LoadListWnd", 
		"", 8, 28, 300-16, 400-60-28, 0);
	ZGuiWnd* pkSaveNameEB = CreateWnd(Textbox, "SaveLoadFileNameEB", "LoadListWnd", 
		"New Game", 0, 400-60+10+2, 300, 22, 0);
	ZGuiWnd* pkOK = CreateWnd(Button, "LoadListOKBn", "LoadListWnd", 
		"OK", 52, 400-60+40, 96, 40, 0);
	ZGuiWnd* pkCancel = CreateWnd(Button, "LoadListCancelBn", "LoadListWnd", 
		"Cacel", 148, 400-60+40, 96, 40, 0);

	pkGui->SetCaptureToWnd(pkLoadListWnd);

	if(init) // only ones
	{
		pkLoadListWnd->SetSkin(new ZGuiSkin());
		pkLoadListWnd->GetSkin()->m_bTransparent = true;
		((ZGuiLabel*)pkTitle)->m_bCenterTextHorz = true;

		ZGuiSkin* BnSkins[3] = {
			new ZGuiSkin(), new ZGuiSkin(), new ZGuiSkin()
		};

		BnSkins[0]->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/misc_button_u.bmp", 0);
		BnSkins[1]->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/misc_button_d.bmp", 0);
		BnSkins[2]->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/misc_button_u.bmp", 0);
		((ZGuiButton*)pkOK)->SetButtonUpSkin(BnSkins[0]);
		((ZGuiButton*)pkOK)->SetButtonDownSkin(BnSkins[1]);
		((ZGuiButton*)pkOK)->SetButtonHighLightSkin(BnSkins[2]);
		((ZGuiButton*)pkCancel)->SetButtonUpSkin(BnSkins[0]);
		((ZGuiButton*)pkCancel)->SetButtonDownSkin(BnSkins[1]);
		((ZGuiButton*)pkCancel)->SetButtonHighLightSkin(BnSkins[2]);

		ZGuiSkin* pkListSkin = new ZGuiSkin();
		ZGuiSkin* pkTitleSkin = new ZGuiSkin();
		
		pkListSkin->m_iBkTexID = pkTexMan->Load("data/textures/gui/dm/final/win_tile.bmp", 0);
		pkListSkin->m_bTileBkSkin = true;
		pkListSkin->m_unBorderSize = 16;
		
		
		pkTitleSkin->m_iHorzBorderTexID = pkListSkin->m_iHorzBorderTexID = pkTexMan->Load("data/textures/gui/dm/final/window/win_edge_ver.bmp", 0);
		pkTitleSkin->m_iVertBorderTexID = pkListSkin->m_iVertBorderTexID = pkTexMan->Load("data/textures/gui/dm/final/window/win_edge_hor.bmp", 0);
		pkTitleSkin->m_iBorderCornerTexID = pkListSkin->m_iBorderCornerTexID = pkTexMan->Load("data/textures/gui/dm/final/window/dm_win_corn.bmp", 0);
		pkTitleSkin->m_iHorzBorderTexAlphaID = pkListSkin->m_iHorzBorderTexAlphaID = pkTexMan->Load("data/textures/gui/dm/final/window/win_edge_ver_a.bmp", 0);
		pkTitleSkin->m_iVertBorderTexAlphaID = pkListSkin->m_iVertBorderTexAlphaID = pkTexMan->Load("data/textures/gui/dm/final/window/win_edge_hor_a.bmp", 0);
		pkTitleSkin->m_iBorderCornerTexAlphaID = pkListSkin->m_iBorderCornerTexAlphaID = pkTexMan->Load("data/textures/gui/dm/final/window/dm_win_corn_a.bmp", 0);

		ZGuiListbox* pkList = (ZGuiListbox*) GetWnd("LoadListLB");
		pkList->SetSkin(pkListSkin);
		
		vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
		pkList->GetWndSkinsDesc(vkSkinDesc);

		for(unsigned int i=0; i<vkSkinDesc.size(); i++)
		{
			if(strcmp("Listbox: Button up", vkSkinDesc[i].second.c_str()) == 0)
			{
				(*vkSkinDesc[i].first)->m_afBkColor[0] =
				(*vkSkinDesc[i].first)->m_afBkColor[1] =
				(*vkSkinDesc[i].first)->m_afBkColor[2] = 0.51f;
			}
			else
			if(strcmp("Listbox: Button down", vkSkinDesc[i].second.c_str()) == 0)
			{
				(*vkSkinDesc[i].first)->m_afBkColor[0] =
				(*vkSkinDesc[i].first)->m_afBkColor[1] =
				(*vkSkinDesc[i].first)->m_afBkColor[2] = 0.71f;
			}
			else
			if(strcmp("Listbox: Button focus", vkSkinDesc[i].second.c_str()) == 0)
			{
				(*vkSkinDesc[i].first)->m_afBkColor[0] =
				(*vkSkinDesc[i].first)->m_afBkColor[1] =
				(*vkSkinDesc[i].first)->m_afBkColor[2] = 0.61f;
			}
		}

		pkTitleSkin->m_afBkColor[0] = pkTitleSkin->m_afBkColor[1] = pkTitleSkin->m_afBkColor[2] = 0.5f;
		pkTitleSkin->m_unBorderSize = 4;
		pkTitle->SetSkin(pkTitleSkin);
		pkSaveNameEB->SetSkin(pkTitleSkin);
	}

	ClearListbox("LoadListLB");

	vector<string> files;
	if(m_pkBasicFS->ListDir(&files, m_strSaveDirectory.c_str(), true))
	{
		for(unsigned int i=0; i<files.size(); i++)
			if(files[i] != string(".."))
				AddListItem("LoadListLB", (char*)files[i].c_str());
	}

	((ZGuiTextbox*) GetWnd("SaveLoadFileNameEB"))->SetReadOnly(!bSave);

	((CNewGameDlg*) m_pkNewGameDlg)->SetFade(0.5f);






	
	m_bSaveGame = bSave;
}

bool DarkMetropolis::GUI_NewGame(ZGuiWnd *pkMainWnd)
{			
	pkMainWnd->Hide();
	
	LoadGuiFromScript(m_pkScript,"data/script/gui/dm_gameplay_2.lua");
	
	StartSong("data/music/dm ingame.ogg");	

	return true;
}

void DarkMetropolis::LoadResourcesOnStartup()
{
	printf("----- start loading resources ---\n");

	char* path_list[7] =
	{
		"data/textures/gui/dm/",
		"data/textures/gui/dm/portraits/",
		"data/textures/gui/dm/intro/",
		"data/textures/gui/dm/items/",
		"data/textures/gui/dm/final/",
		"data/textures/gui/dm/final/window/",
		"data/textures/gui/dm/final/members/"
	};

	vector<string> t;
	char complete_path[1024];

	for(int j=0; j<7; j++)
	{	
		m_pkZFVFileSystem->ListDir(&t, path_list[j]);
		const int antal = t.size();

		for(unsigned int i=0; i<antal; i++)
			if(t[i].find(".bmp") != string::npos)
			{
				sprintf(complete_path, "%s%s", path_list[j], t[i].c_str());
				pkTexMan->Load(complete_path, 0);
			}

		t.clear();
	}

	m_pkAudioSys->LoadSound("data/sound/computer beep 2.wav");
	m_pkAudioSys->LoadSound("data/sound/computer beep 3.wav");
	m_pkAudioSys->LoadSound("data/sound/computer beep 5.wav");
	m_pkAudioSys->LoadSound("data/sound/computer beep 6.wav");
	m_pkAudioSys->LoadSound("data/sound/panel_out.wav");
	m_pkAudioSys->LoadSound("data/sound/panel_in.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_ahh_that_fuckin_hurt.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_careful.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_damn.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_didnt_do_nothin.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_dont_shoot.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_fuck.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_please_dont_shoot_me.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_shit.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/defensive/daq_you_shootin_at_me_for.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/death/death1.wav");
	m_pkAudioSys->LoadSound("data/sound/mechanic/death/death2.wav");

	printf("----- end loading resources ---\n");
}