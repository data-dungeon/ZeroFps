#include "handleagents_dlg.h"

CHandleAgents::CHandleAgents() : CGameDlg(&g_kDM)
{
}

CHandleAgents::~CHandleAgents()
{
}

void CHandleAgents::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	if(strClickName == "AgentsClose")
	{
		LoadDlg("data/script/gui/dm_hq.lua");
		pkMainWnd->Hide();	
		m_pkGui->KillWndCapture();
	}
	else
	if(strClickName == "AgentsInfoBn")
	{
		m_pkGui->KillWndCapture();
		LoadDlg("data/script/gui/dm_members.lua");
		ShowWnd("MembersWnd", true, true);

		ShowWnd("MembersEquipBn", false); // dölj equip knappen
		ShowWnd("MembersDropItemBn", false); // dölj drop knappen
	}

	int pos;
	if((pos=strClickName.find("AgentsInHQBn")) != string::npos)
	{
		string temp(strClickName);
		temp.erase(0,pos+strlen("AgentsInHQBn"));
		
		char* szButtons[] = {
			"AgentsInHQBn1", "AgentsInHQBn2", "AgentsInHQBn3",
			"AgentsInHQBn4", "AgentsInHQBn5", "AgentsInHQBn6",
			"AgentsInHQBn7"
		};

		for(int i=0; i<7; i++)
		{
			if(strClickName != szButtons[i])
				((ZGuiCheckbox*)GetWnd(szButtons[i]))->UncheckButton();
		}
	}

	if((pos=strClickName.find("AgentsToHireBn")) != string::npos)
	{
		string temp(strClickName);
		temp.erase(0,pos+strlen("AgentsInHQBn"));
		
		char* szButtons[] = {
			"AgentsToHireBn1", "AgentsToHireBn2", "AgentsToHireBn3",
			"AgentsToHireBn4", "AgentsToHireBn5", "AgentsToHireBn6",
			"AgentsToHireBn7"
		};

		for(int i=0; i<7; i++)
		{
			if(strClickName != szButtons[i])
				((ZGuiCheckbox*)GetWnd(szButtons[i]))->UncheckButton();
		}
	}
}
