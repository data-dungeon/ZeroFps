#include "dark_metropolis.h"
#include "handleagents_dlg.h"
#include "members_dlg.h"

CHandleAgents::CHandleAgents() : CGameDlg("AgentsWnd", &g_kDM)
{
	m_bInitialized = false;
}

CHandleAgents::~CHandleAgents()
{
}

void CHandleAgents::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	InitDlg();

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
		bool bAButtonIsSelected=false;
		for(int i=0; i<m_vkCharsInBaseBns.size(); i++)
		{
			if(m_vkCharsInBaseBns[i]->IsChecked())
			{
				bAButtonIsSelected=true;
				break;
			}
		}

		if(bAButtonIsSelected)
		{
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
			if(m_vkCharsInBaseBns[i]->IsChecked())
			{
				if(SendOutAgent(i))
				{
					m_vkCharsInBaseBns[i]->Hide();
				}
				break;
			}
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

bool CHandleAgents::InitDlg()
{
	if(!m_bInitialized)
	{
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn1"));
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn2"));
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn3"));
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn4"));
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn5"));
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn6"));
		m_vkCharsInBaseBns.push_back((ZGuiCheckbox*)GetWnd("AgentsInHQBn7"));
	}

	for(int i=0; i<7; i++)
	{
		m_vkCharsInBaseBns[i]->GetCheckedSkin()->m_iBkTexID = -1; 
		m_vkCharsInBaseBns[i]->GetUncheckedSkin()->m_iBkTexID = -1; 
	}

	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<int> vkCharsInBase;
		pkHQ->GetCharacters(&vkCharsInBase);

		printf("There are %i in the HQ\n", vkCharsInBase.size());

		for(int i=0; i<7; i++)
		{
			if(i<vkCharsInBase.size())
			{
				int tex_id = GetTexID("data/textures/gui/dm/portrait4.bmp");
				m_vkCharsInBaseBns[i]->Show();
				m_vkCharsInBaseBns[i]->GetUncheckedSkin()->m_iBkTexID = tex_id; 			
				m_vkCharsInBaseBns[i]->GetCheckedSkin()->m_iBkTexID = tex_id; 
				m_vkCharsInBaseBns[i]->GetCheckedSkin()->m_afBkColor[1] = 0;
			}
			else
			{
				m_vkCharsInBaseBns[i]->Hide();
			}
		}
	}
	else
	{
		printf("Failed to get HQ entiry\n");
	}

	m_bInitialized = true;

	return true;
}

bool CHandleAgents::SendOutAgent(int iID)
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<int> vkCharsInBase;
		pkHQ->GetCharacters(&vkCharsInBase);

		if(!vkCharsInBase.empty())
		{
			if(pkHQ->EjectCharacter( vkCharsInBase[iID] ))
			{
				printf("Sending out characterd id %i", vkCharsInBase[iID]);
				return true;
			}
		}
	}

	printf("Failed to send out character\n");
	return false;
}