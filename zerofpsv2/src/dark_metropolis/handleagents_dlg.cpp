#include "dark_metropolis.h"
#include "handleagents_dlg.h"
#include "members_dlg.h"
#include "itemtransaction_dlg.h"
#include "gameplay_dlg.h"

CHandleAgents::CHandleAgents() : CGameDlg("AgentsWnd", &g_kDM)
{
	m_bInitialized = false;
	m_iSelAgent = -1;
	m_iStartAgent = 0;
	m_iStartHireAgent = 0;
	m_iSelAgentToHire = -1;
	m_iAgentPrice = 100;
}

CHandleAgents::~CHandleAgents()
{
}

void CHandleAgents::OnCommand(ZGuiWnd *pkMainWnd, string strClickName,
										bool bRMouseBnClick)
{
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
	if(strClickName == "SendOutAgentBn")
	{
		int MAX_NUM_AGENTS = 5;
		vector<Entity*> kMemberList;
		GetAllAgentsInField(kMemberList);
		if(kMemberList.size() >= MAX_NUM_AGENTS)
		{
			// TODO: släng upp någon messagebox här
			printf("Can't send out agent, already have %i on the field!\n", 
				MAX_NUM_AGENTS);
			return;
		}

		for(unsigned int i=0; i<m_vkCharsInBaseBns.size(); i++)
		{
			if(m_vkCharsInBaseBns[i].first->IsChecked())
			{
				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 

				if(SendOutAgent(m_vkCharsInBaseBns[i].second))
					m_vkCharsInBaseBns[i].first->Hide();

				((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->UpdateAgentList();

				m_iSelAgent = -1;
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
	if(strClickName == "FireAgentBn" && m_iSelAgent != -1)
	{
		if(((P_DMHQ*)GetDMObject(HQ)->GetProperty("P_DMHQ"))->FireCharacter(m_iSelAgent))
		{
			m_iSelAgent = -1;
			UpdateAgentInBaseList(m_iStartAgent);

			P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
				GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");
			
			// Öka pengar
			pkGameInfo->m_iMoney += m_iAgentPrice;

			char szText[50];
			sprintf(szText, "Money: %i", pkGameInfo->m_iMoney);
			SetText("AgentsMoneyLabel", szText);
		}
	}
	else
	if(strClickName == "HireAgentBn" && m_iSelAgentToHire != -1)
	{
		BuyAgent(m_iSelAgentToHire);
	}
	else
	if(strClickName == "AgentsEquip" && m_iSelAgent != -1) 
	{
		LoadDlg("data/script/gui/dm_itemtransaction.lua");
		((CItemTransactionDlg*)GetGameDlg(ITEMTRANSACTION_DLG))->m_eViewMode = 
			CItemTransactionDlg::store_room;

		GetGameDlg(ITEMTRANSACTION_DLG)->InitDlg();

		m_pkGui->KillWndCapture(); 
		m_pkGui->SetCaptureToWnd(GetWnd("ItemTransactionWnd"));

		SetText("RemoveItemBn", "Put in storeroom");
		SetText("AddItemBn", "Add to agent");

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "AgentsShop" && m_iSelAgent != -1) 
	{
		LoadDlg("data/script/gui/dm_itemtransaction.lua");
		((CItemTransactionDlg*)GetGameDlg(ITEMTRANSACTION_DLG))->m_eViewMode = 
			CItemTransactionDlg::shop;

		GetGameDlg(ITEMTRANSACTION_DLG)->InitDlg();

		m_pkGui->KillWndCapture(); 
		m_pkGui->SetCaptureToWnd(GetWnd("ItemTransactionWnd"));

		SetText("RemoveItemBn", "Sell");
		SetText("AddItemBn", "Buy");

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}

	char* szAgentsInBaseBns[] = {
		"AgentsInHQBn1", "AgentsInHQBn2", "AgentsInHQBn3",
		"AgentsInHQBn4", "AgentsInHQBn5", "AgentsInHQBn6",
		"AgentsInHQBn7"
	};

	char* szAgentsToHireBns[] = {
		"AgentsToHireBn1", "AgentsToHireBn2", "AgentsToHireBn3",
		"AgentsToHireBn4", "AgentsToHireBn5", "AgentsToHireBn6",
		"AgentsToHireBn7"
	};

	int pos;
	if((pos=strClickName.find("AgentsInHQBn")) != string::npos)
	{
		string temp(strClickName);
		temp.erase(0,pos+strlen("AgentsInHQBn"));
		
		for(int i=0; i<7; i++)
		{
			if(strClickName != szAgentsInBaseBns[i])
			{
				((ZGuiCheckbox*)GetWnd(szAgentsInBaseBns[i]))->UncheckButton();
			}
			else
			{
				m_iSelAgentToHire = -1;

				for(int j=0; j<7; j++)
					((ZGuiCheckbox*)GetWnd(szAgentsToHireBns[j]))->UncheckButton();

				m_iSelAgent = m_vkCharsInBaseBns[i].second;

				DMCharacterStats* pkStats = 
					((P_DMCharacter*)GetObject(m_iSelAgent)->GetProperty("P_DMCharacter"))->GetStats();

				char szText[150];
				sprintf(szText, "Your agent %i/%i - %s", i+m_iStartAgent+1, 
					GetNumAgentsInBase(), pkStats->m_strName.c_str());
				SetText("AgentInHQLabel", szText);

				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 

				PrintStats(pkStats);
			}
		}
	}

	if((pos=strClickName.find("AgentsToHireBn")) != string::npos)
	{
		string temp(strClickName);
		temp.erase(0,pos+strlen("AgentsToHireBn"));
		
		for(int i=0; i<7; i++)
		{
			if(strClickName != szAgentsToHireBns[i])
				((ZGuiCheckbox*)GetWnd(szAgentsToHireBns[i]))->UncheckButton();
			else
			{
				m_iSelAgent = -1;

				for(int j=0; j<7; j++)
					((ZGuiCheckbox*)GetWnd(szAgentsInBaseBns[j]))->UncheckButton();

				DMCharacterStats kStats = m_vkAgentsToHireBns[i].second;
				char szText[150];
				sprintf(szText, "Agents to hire %i/%i - %s", i+m_iStartHireAgent+1, 
					GetNumAgentsToHire(), kStats.m_strName.c_str() );
				SetText("CurrentAgentToHireLabel", szText);

				m_iSelAgentToHire = i+m_iStartHireAgent;

				m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
					m_pkAudioSys->GetListnerPos()); 

				PrintStats(&kStats);
			}
		}
	}
}

bool CHandleAgents::InitDlg()
{
	m_iStartAgent = 0;
	m_iStartHireAgent = 0;
	m_iSelAgentToHire = -1;

	P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
		GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");

	char szText[50];

	sprintf(szText, "Money: %i", pkGameInfo->m_iMoney);
	SetText("AgentsMoneyLabel", szText);

	sprintf(szText, "Reputation: %f", pkGameInfo->m_fReputation);
	SetText("AgentsReputaionLabel", szText); 

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

		//////////////////////////////////////////////////////////////////////////////
		// Hämta priset på en agent på ett väldigt osnyggt sätt
		// (jag kollar vad som står mellan parantestecknen i
		// texten för knappen som skapas i filen dm_agents2.lua.
		// OSNYGG SISTA I MINUTEN LÖSNING (för att slippa bygga om världen efter att
		// ha laggt till data i character propertyn)
		bool bCount=false;
		char szLabel[50], szPrice[10];
		strcpy(szLabel, GetWnd("HireAgentBn")->GetText());
		int c=0;
		for(int i=0; i<strlen(szLabel); i++)
		{
			if(szLabel[i-1] == '(') bCount=true;
			if(szLabel[i] == ')') bCount=false;
			if(bCount) szPrice[c++] = szLabel[i];
		}
		szPrice[c] = '\0';
		m_iAgentPrice = atoi(szPrice);
		//////////////////////////////////////////////////////////////////////////////
	}

	UpdateAgentInBaseList(m_iStartAgent);
	UpdateAgentToHireList(m_iStartHireAgent);

	SetText("AgentInHQLabel", "");

	m_bInitialized = true;

	return true;
}

bool CHandleAgents::BuyAgent(int iAgentID)
{
	int iPrice = m_iAgentPrice;

	P_DMGameInfo* pkGameInfo = (P_DMGameInfo*)
		GetDMObject(GAME_INFO)->GetProperty("P_DMGameInfo");

	if(pkGameInfo && (pkGameInfo->m_iMoney - iPrice > 0) )
	{
		if(iAgentID != -1)
		{
			((P_DMHQ*)GetDMObject(HQ)->GetProperty("P_DMHQ"))->SpawnNewCharacter(iAgentID);
			UpdateAgentToHireList(m_iStartHireAgent);
			UpdateAgentInBaseList(m_iStartAgent);

			pkGameInfo->m_iMoney -= iPrice;

			char szText[50];
			sprintf(szText, "Money: %i", pkGameInfo->m_iMoney);
			SetText("AgentsMoneyLabel", szText);
			return true;
		}
	}

	return false;
}

bool CHandleAgents::SendOutAgent(int iAgentID)
{
	Entity* pkHQObject = GetDMObject(HQ);

	if(pkHQObject)
	{
		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		if(pkHQ->EjectCharacter( iAgentID ))
		{
			((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->SelectAgentGUI(iAgentID, true);
			((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->UpdateAgentList();
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

		for(unsigned int i=0; i<7; i++)
		{
			m_vkCharsInBaseBns[i].first->UncheckButton();

			if((i+iStartAgent)<vkCharsInBase.size())
			{
				P_DMCharacter* pkCharProperty = (P_DMCharacter*) 
					GetObject(vkCharsInBase[i+iStartAgent])->GetProperty("P_DMCharacter");

				ZFAssert(pkCharProperty, 
					"CMembersDlg::SetCharacterStats - No character property\n");

				string szTexName = string("data/textures/gui/portraits/") +
					pkCharProperty->GetStats()->m_strIcon;

				int tex_id = GetTexID((char*) szTexName.c_str());
				m_vkCharsInBaseBns[i].first->Show();
				m_vkCharsInBaseBns[i].first->GetUncheckedSkin()->m_iBkTexID = tex_id; 			
				m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_iBkTexID = tex_id; 
				m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_afBkColor[0] = 0.5f; 
				m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_afBkColor[1] = 1.0f; 
				m_vkCharsInBaseBns[i].first->GetCheckedSkin()->m_afBkColor[2] = 0.0f; 
				m_vkCharsInBaseBns[i].second = vkCharsInBase[i+iStartAgent];	

				SetButtonIcon(m_vkCharsInBaseBns[i].first, szTexName, false, true);

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

		for(unsigned int i=0; i<7; i++)
		{
			m_vkAgentsToHireBns[i].first->UncheckButton();

			if((i+iStartAgent)<vkAgetsToHire->size())
			{
				DMCharacterStats stats = (*vkAgetsToHire)[i+iStartAgent];

				string szTexName = string("data/textures/gui/portraits/") +
					stats.m_strIcon;

				int tex_id = GetTexID((char*) szTexName.c_str());
				m_vkAgentsToHireBns[i].first->Show();
				m_vkAgentsToHireBns[i].first->GetUncheckedSkin()->m_iBkTexID = tex_id; 			
				m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_iBkTexID = tex_id; 
				m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_afBkColor[0] = 1.0f;
				m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_afBkColor[1] = 0.5f;
				m_vkAgentsToHireBns[i].first->GetCheckedSkin()->m_afBkColor[2] = 0.5f;
				m_vkAgentsToHireBns[i].second = stats;	

				SetButtonIcon(m_vkAgentsToHireBns[i].first, szTexName, false, true);

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

void CHandleAgents::PrintStats(DMCharacterStats* pkStats)
{
	char szText[150];

	sprintf(szText, "Name: %s\n", pkStats->m_strName.c_str());
	SetText("AgentInFocusMemberName", szText);

	sprintf(szText, "Armor: %i\n", (int)pkStats->m_fArmour);
	SetText("AgentInFocusMemberArmor", szText);

	sprintf(szText, "Speed: %i\n", (int)pkStats->m_fSpeed);
	SetText("AgentInFocusMemberSpeed", szText);

	sprintf(szText, "Wage: %i\n", (int)pkStats->m_fWage);
	SetText("AgentInFocusMemberWage", szText);

	sprintf(szText, "Level: %i\n", pkStats->m_iLevel);
	SetText("AgentInFocusMemberLevel", szText);

	sprintf(szText, "HP: %i\n", pkStats->m_iLife);
	SetText("AgentInFocusMemberHP", szText);
}