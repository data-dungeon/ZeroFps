#include "dark_metropolis.h"
#include "handleagents_dlg.h"
#include "members_dlg.h"

CHandleAgents::CHandleAgents() : CGameDlg("AgentsWnd", &g_kDM)
{
	m_bInitialized = false;
	m_iSelAgent = -1;
	m_iStartAgent = 0;
}

CHandleAgents::~CHandleAgents()
{
}

void CHandleAgents::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	//InitDlg();

	if(strClickName == "AgentsClose")
	{
		LoadDlg("data/script/gui/dm_hq.lua");
		pkMainWnd->Hide();	
		m_pkGui->KillWndCapture();
		m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
	}
	else
	if(strClickName == "AgentsInfoBn")
	{
		int sel_agent;
		bool bAButtonIsSelected=false;
		for(int i=0; i<m_vkCharsInBaseBns.size(); i++)
		{
			if(m_vkCharsInBaseBns[i].first->IsChecked())
			{
				sel_agent = i;
				bAButtonIsSelected=true;
				break;
			}
		}

		if(bAButtonIsSelected)
		{
			m_iSelAgent = m_vkCharsInBaseBns[sel_agent].second;

			m_pkGui->KillWndCapture();
			LoadDlg("data/script/gui/dm_members.lua");
			ShowWnd("MembersWnd", true, true);

			CMembersDlg* pkMembersDlg = (CMembersDlg*) GetGameDlg(MEMBERS_DLG);

			if(pkMembersDlg)
				pkMembersDlg->SetWindowMode(
					CMembersDlg::HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING); 
		}
	}
	else
	if(strClickName == "SendOutAgentBn")
	{
		for(int i=0; i<m_vkCharsInBaseBns.size(); i++)
		{
			if(m_vkCharsInBaseBns[i].first->IsChecked())
			{
				if(SendOutAgent(m_vkCharsInBaseBns[i].second))
					m_vkCharsInBaseBns[i].first->Hide();
				break;
			}
		}
	}
	else
	if(strClickName == "PrevAgentInHQBn")
	{
		if(m_iStartAgent > 0)
		{
			m_iStartAgent--;
			UpdateAgentList(m_iStartAgent);
		}
	}
	else
	if(strClickName == "NextAgentInHQ")
	{
		if(GetNumAgentsInBase()-m_iStartAgent > 7)
		{
			m_iStartAgent++;
			UpdateAgentList(m_iStartAgent);
		}
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
			{
				((ZGuiCheckbox*)GetWnd(szButtons[i]))->UncheckButton();
			}
			else
			{
				m_iSelAgent = m_vkCharsInBaseBns[i].second;
				char szText[50];
				sprintf(szText, "Your agent %i/%i - Mr Smith %i", i+m_iStartAgent+1, 
					GetNumAgentsInBase(), m_iSelAgent);
				SetText("AgentInHQLabel", szText);
			}
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

bool CHandleAgents::InitDlg()
{
	m_iStartAgent = 0;

	if(!m_bInitialized)
	{
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn1"),-1));
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn2"),-1));
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn3"),-1));
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn4"),-1));
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn5"),-1));
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn6"),-1));
		m_vkCharsInBaseBns.push_back(pair<ZGuiCheckbox*, int>
			((ZGuiCheckbox*)GetWnd("AgentsInHQBn7"),-1));


		for(int i=0; i<7; i++)
		{
			m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_iBkTexID = -1; 
			m_vkCharsInBaseBns[i].first->GetUncheckedSkin()->m_iBkTexID = -1; 
			m_vkCharsInBaseBns[i].second = -1;
		}
	}

	UpdateAgentList(m_iStartAgent);

	SetText("AgentInHQLabel", "");

	m_bInitialized = true;

	return true;
}

bool CHandleAgents::SendOutAgent(int iAgentID)
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		if(pkHQ->EjectCharacter( iAgentID ))
		{
			printf("Sending out characterd id %i", iAgentID);
			return true;
		}
	}

	printf("Failed to send out character\n");
	return false;
}

void CHandleAgents::UpdateAgentList(int iStartAgent)
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<int> vkCharsInBase;
		pkHQ->GetCharacters(&vkCharsInBase);

		printf("There are %i in the HQ\n", vkCharsInBase.size());

		for(int i=0; i<7; i++)
		{
			m_vkCharsInBaseBns[i].first->UncheckButton();

			if((i+iStartAgent)<vkCharsInBase.size())
			{
				P_DMCharacter* pkCharProperty = (P_DMCharacter*) 
					GetObject(vkCharsInBase[i+iStartAgent])->GetProperty("P_DMCharacter");

				ZFAssert(pkCharProperty, 
					"CMembersDlg::SetCharacterStats - No character property\n");

				string szTexName = string("data/textures/gui/dm/portraits/") +
					pkCharProperty->GetStats()->m_strIcon;

				int tex_id = GetTexID((char*) szTexName.c_str());
				m_vkCharsInBaseBns[i].first->Show();
				m_vkCharsInBaseBns[i].first->GetUncheckedSkin()->m_iBkTexID = tex_id; 			
				m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_iBkTexID = tex_id; 
				m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_afBkColor[1] = 0;
				m_vkCharsInBaseBns[i].second = vkCharsInBase[i+iStartAgent];	

				if(m_vkCharsInBaseBns[i].second == m_iSelAgent)
				{
					m_vkCharsInBaseBns[i].first->CheckButton();
				}
			}
			else
			{
				m_vkCharsInBaseBns[i].first->Hide();
				m_vkCharsInBaseBns[i].second = -1;
			}
		}
	}
	else
	{
		printf("Failed to get HQ entiry\n");
	}
}

int CHandleAgents::GetNumAgentsInBase()
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<int> vkCharsInBase;
		pkHQ->GetCharacters(&vkCharsInBase);

		return vkCharsInBase.size();
	}

	return -1;
}