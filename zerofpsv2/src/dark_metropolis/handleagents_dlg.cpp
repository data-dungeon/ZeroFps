#include "dark_metropolis.h"
#include "handleagents_dlg.h"
#include "members_dlg.h"

CHandleAgents::CHandleAgents() : CGameDlg("AgentsWnd", &g_kDM)
{
	m_bInitialized = false;
	m_iSelAgent = -1;
	m_iStartAgent = 0;
	m_iStartHireAgent = 0;
	m_iAgentToHire = -1;
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

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "AgentsInfoBn")
	{
		DMCharacterStats empty;
		empty.m_strName = "";

		m_kViewAgentInfo = empty;

		int sel_agent;
		bool bAButtonIsSelected=false;
		for(unsigned int i=0; i<m_vkCharsInBaseBns.size(); i++)
		{
			if(m_vkCharsInBaseBns[i].first->IsChecked())
			{
				sel_agent = i;
				m_iSelAgent = m_vkCharsInBaseBns[sel_agent].second;
				bAButtonIsSelected=true;

				m_kViewAgentInfo = *((P_DMCharacter*)
					GetObject(m_iSelAgent)->GetProperty("P_DMCharacter"))->GetStats(); 
				break;
			}
		}

		for( unsigned int i=0; i<m_vkAgentsToHireBns.size(); i++)
		{
			if(m_vkAgentsToHireBns[i].first->IsChecked())
			{
				m_kViewAgentInfo = m_vkAgentsToHireBns[i].second;
				bAButtonIsSelected=true;
				break;
			}
		}

		if(bAButtonIsSelected)
		{
			m_pkGui->KillWndCapture();
			LoadDlg("data/script/gui/dm_members_2.lua");
			ShowWnd("MembersWnd", true/*, true*/);

			CMembersDlg* pkMembersDlg = (CMembersDlg*) GetGameDlg(MEMBERS_DLG);

			if(pkMembersDlg)
				pkMembersDlg->SetWindowMode(
					CMembersDlg::HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING); 

			m_pkGui->SetCaptureToWnd(GetWnd("MembersWnd"));

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
	else
	if(strClickName == "SendOutAgentBn")
	{
		for(unsigned int i=0; i<m_vkCharsInBaseBns.size(); i++)
		{
			if(m_vkCharsInBaseBns[i].first->IsChecked())
			{
				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 

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
			UpdateAgentInBaseList(m_iStartAgent);

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
	else
	if(strClickName == "NextAgentInHQ")
	{
		if(GetNumAgentsInBase()-m_iStartAgent > 7)
		{
			m_iStartAgent++;
			UpdateAgentInBaseList(m_iStartAgent);

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
	else
	if(strClickName == "PrevAgentToHireBn")
	{
		if(m_iStartHireAgent > 0)
		{
			m_iStartHireAgent--;
			UpdateAgentToHireList(m_iStartHireAgent);

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
	else
	if(strClickName == "NextAgentToHireBn")
	{
		if(GetNumAgentsToHire()-m_iStartHireAgent > 7)
		{
			m_iStartHireAgent++;
			UpdateAgentToHireList(m_iStartHireAgent);

			m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
				m_pkAudioSys->GetListnerPos()); 
		}
	}
	else
	if(strClickName == "HireAgentBn")
	{
		((P_DMHQ*)GetDMObject(HQ)->GetProperty("P_DMHQ"))->SpawnNewCharacter(m_iAgentToHire);
		UpdateAgentToHireList(m_iStartHireAgent);
		UpdateAgentInBaseList(m_iStartAgent);
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

				DMCharacterStats* pkStats = 
					((P_DMCharacter*)GetObject(m_iSelAgent)->GetProperty("P_DMCharacter"))->GetStats();

				char szText[150];
				sprintf(szText, "Your agent %i/%i - %s", i+m_iStartAgent+1, 
					GetNumAgentsInBase(), pkStats->m_strName.c_str());
				SetText("AgentInHQLabel", szText);

				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 
			}
		}
	}

	if((pos=strClickName.find("AgentsToHireBn")) != string::npos)
	{
		string temp(strClickName);
		temp.erase(0,pos+strlen("AgentsToHireBn"));
		
		char* szButtons[] = {
			"AgentsToHireBn1", "AgentsToHireBn2", "AgentsToHireBn3",
			"AgentsToHireBn4", "AgentsToHireBn5", "AgentsToHireBn6",
			"AgentsToHireBn7"
		};

		for(int i=0; i<7; i++)
		{
			if(strClickName != szButtons[i])
				((ZGuiCheckbox*)GetWnd(szButtons[i]))->UncheckButton();
			else
			{
				DMCharacterStats kStats = m_vkAgentsToHireBns[i].second;
				char szText[150];
				sprintf(szText, "Your agent %i/%i - %s", i+m_iStartHireAgent+1, 
					GetNumAgentsToHire(), kStats.m_strName.c_str() );
				SetText("CurrentAgentToHireLabel", szText);

				m_iAgentToHire = i+m_iStartHireAgent;

				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 
			}
		}
	}
}

bool CHandleAgents::InitDlg()
{
	m_iStartAgent = 0;
	m_iStartHireAgent = 0;
	m_iAgentToHire = -1;

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

		DMCharacterStats empty;
		empty.m_strName = "";

		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn1"),empty));
		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn2"),empty));
		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn3"),empty));
		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn4"),empty));
		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn5"),empty));
		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn6"),empty));
		m_vkAgentsToHireBns.push_back(pair<ZGuiCheckbox*, DMCharacterStats>
			((ZGuiCheckbox*)GetWnd("AgentsToHireBn7"),empty));

		for(int i=0; i<7; i++)
		{
			m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_iBkTexID = -1; 
			m_vkAgentsToHireBns[i].first->GetUncheckedSkin()->m_iBkTexID = -1; 
			m_vkAgentsToHireBns[i].second.m_strName = "";
		}		
	}

	UpdateAgentInBaseList(m_iStartAgent);
	UpdateAgentToHireList(m_iStartHireAgent);

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

void CHandleAgents::UpdateAgentInBaseList(int iStartAgent)
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<int> vkCharsInBase;
		pkHQ->GetCharacters(&vkCharsInBase);

		printf("There are %i in the HQ\n", vkCharsInBase.size());

		for(unsigned int i=0; i<7; i++)
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

int CHandleAgents::GetNumAgentsToHire()
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");
		return pkHQ->GetHireList()->size();
	}

	return -1;
}



void CHandleAgents::UpdateAgentToHireList(int iStartAgent)
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<DMCharacterStats>* vkAgetsToHire = pkHQ->GetHireList();

		printf("There are %i to hire!\n", vkAgetsToHire->size());

		for(unsigned int i=0; i<7; i++)
		{
			m_vkAgentsToHireBns[i].first->UncheckButton();

			if((i+iStartAgent)<vkAgetsToHire->size())
			{
				DMCharacterStats stats = (*vkAgetsToHire)[i+iStartAgent];

				string szTexName = string("data/textures/gui/dm/portraits/") +
					stats.m_strIcon;

				int tex_id = GetTexID((char*) szTexName.c_str());
				m_vkAgentsToHireBns[i].first->Show();
				m_vkAgentsToHireBns[i].first->GetUncheckedSkin()->m_iBkTexID = tex_id; 			
				m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_iBkTexID = tex_id; 
				m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_afBkColor[1] = 0;
				m_vkAgentsToHireBns[i].second = stats;	

		/*		if(m_vkCharsInBaseBns[i].second == m_iSelAgent)
				{
					m_vkCharsInBaseBns[i].first->CheckButton();
				}*/
			}
			else
			{
				m_vkAgentsToHireBns[i].first->Hide();
				m_vkAgentsToHireBns[i].second.m_strName = "xx";
			}
		}
	}
	else
	{
		printf("Failed to get HQ entiry\n");
	}
}