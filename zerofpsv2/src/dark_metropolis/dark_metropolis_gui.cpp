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
	}
	return true;
}

void DarkMetropolis::GUI_Init()
{
	// Create gui script.
	GuiAppLua::Init(&g_kDM, m_pkScript);
	InitGui(m_pkScript,
		"data/textures/text/ms_sans_serif8.bmp",
		"data/script/gui/defskins.lua",
		NULL);

	// Load start sceen.
	LoadGuiFromScript(m_pkScript, "data/script/gui/dm_start.lua");

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

	printf("strMainWnd = %s\n", strMainWnd.c_str());

	if(strMainWnd == "DMStartWnd")
	{
		m_pkStartDMDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "StartNewGameWnd")
	{
		m_pkNewGameDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "HQWnd")
	{
		m_pkHQDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "MembersWnd")
	{
		m_pkMembersDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "MissionWnd")
	{
		m_pkMissionDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "BriefingWnd")
	{
		m_pkBriefingDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "GamePlayPanelWnd")
	{
		m_pkGamePlayDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "ItemTransactionWnd")
	{
		m_pkItemTransactionDlg->OnCommand(pkMainWnd, strClickName);
	}
	else
	if(strMainWnd == "AgentsWnd")
	{		
		m_pkHandleAgents->OnCommand(pkMainWnd, strClickName);
	}
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
				}
			}
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
}

void DarkMetropolis::GUI_LoadSave(bool bSave)
{
	bool init = GetWnd("LoadListWnd") == NULL;

	ZGuiWnd* pkLoadListWnd = CreateWnd(Wnd, "LoadListWnd", "GuiMainWnd", 
		"", 800/2-150, 50, 300, 400, 0);
	ZGuiWnd* pkTitle = CreateWnd(Label, "LoadListTitle", "LoadListWnd", 
		"Select your clan", 0, 0, 300, 20, 0);
	ZGuiWnd* pkLoadList = CreateWnd(Listbox, "LoadListLB", "LoadListWnd", 
		"", 8, 28, 300-16, 400-60-28, 0);
	ZGuiWnd* pkSaveNameEB = CreateWnd(Textbox, "SaveLoadFileNameEB", "LoadListWnd", 
		"New Game", 0, 400-60+10, 300, 20, 0);
	ZGuiWnd* pkOK = CreateWnd(Button, "LoadListOKBn", "LoadListWnd", 
		"OK", 50, 400-60+40, 60, 20, 0);
	ZGuiWnd* pkCancel = CreateWnd(Button, "LoadListCancelBn", "LoadListWnd", 
		"Cacel", 200, 400-60+40, 60, 20, 0);

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
	}

	ClearListbox("LoadListLB");

	vector<string> files;
	if(m_pkBasicFS->ListDir(&files, m_strSaveDirectory.c_str(), true))
	{
		for(int i=0; i<files.size(); i++)
			if(files[i] != string(".."))
				AddListItem("LoadListLB", (char*)files[i].c_str());
	}

	((ZGuiTextbox*) GetWnd("SaveLoadFileNameEB"))->SetReadOnly(!bSave);

	m_bSaveGame = bSave;
}

bool DarkMetropolis::GUI_NewGame(ZGuiWnd *pkMainWnd)
{			
	pkMainWnd->Hide();
	
	LoadGuiFromScript(m_pkScript,"data/script/gui/dm_gameplay.lua");
	
	//char* szWndToHide[] =
	//{
	//	"GamePlayChar1Wnd", "GamePlayChar2Wnd", 
	//	"GamePlayChar3Wnd", "GamePlayChar4Wnd", 
	//	"GamePlayChar5Wnd", "GamePlayPanelWnd",
	//	"GamePlayInfoWnd", "MembersWnd",
	//	"MissionWnd", "BriefingWnd",
	//	"ItemAddWnd", "ItemRemoveWnd", 
	//};

	//for(int i=0; i<sizeof(szWndToHide)/sizeof(szWndToHide[1]); i++)
	//	ShowWnd(szWndToHide[i], false);	

	StartSong("data/music/dm ingame.ogg");
	
	return true;
}