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

				m_itStartBase = m_vkStartBaseList.begin(); 
			}

			SelListItem("TeamColorCB", "Red");
		}
		else
		if(strClickName == "QuitBn")
		{
			m_pkFps->QuitEngine();
		}
	}
	else
	if(strMainWnd == "StartNewGameWnd")
	{
		if(strClickName == "StartNewGameDone")
		{
			pkMainWnd->Hide();
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