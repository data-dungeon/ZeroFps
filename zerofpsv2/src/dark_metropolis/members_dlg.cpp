#include "dark_metropolis.h"
#include "members_dlg.h"

CMembersDlg::CMembersDlg() : CGameDlg("MembersWnd", &g_kDM)
{
	m_iCurrentCharacterPage=0;
}

CMembersDlg::~CMembersDlg() 
{
}

void CMembersDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	// MembersWnd
	if(strClickName == "MembersClosBn")
	{
		ShowWnd("MembersWnd", false, true);
	}
	else
	if(strClickName == "MembersEquipBn")
	{
		m_pkGui->KillWndCapture(); 
		LoadDlg("data/script/gui/dm_itemtransaction.lua");
		ShowWnd("ItemTransactionWnd", true, true);
		SetText("RemoveItemBn", "Unequip");
		SetText("AddItemBn", "Equip");
	}
	else
	if(strClickName == "PrevMemberBn")
	{
		SwitchCharacter(false);
	}
	else
	if(strClickName == "NextMemberBn")
	{
		SwitchCharacter(true);
	}

}

void CMembersDlg::SetWindowMode(WINDOW_MODE eType)
{
	m_eWidowMode = eType;

	Entity* pkHQObject = GetDMObject(HQ);

	SetText("CurrentMemberNumberLabel", "");

	switch(eType)
	{
		case HQ_EQUIP_MEMBERS:
			ShowWnd("MembersEquipBn", true); // visa equip knappen
			ShowWnd("MembersDropItemBn", false); // dölj drop knappen

			if(pkHQObject)
			{
				m_iCurrentCharacterPage = 0;

				P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");
				vector<int> vkCharsInBase;
				pkHQ->GetCharacters(&vkCharsInBase);

				if(!vkCharsInBase.empty())
				{
					char text[50];
					sprintf(text, "Agent %i", 
						vkCharsInBase[m_iCurrentCharacterPage]);
					SetText("CurrentMemberNumberLabel", text);
					SetCharacterStats(GetObject(vkCharsInBase[m_iCurrentCharacterPage]));
				}
				else
				{
					SetCharacterStats(NULL);
				}
			}

			break;

		case HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING:
			ShowWnd("MembersEquipBn", false); // dölj equip knappen
			ShowWnd("MembersDropItemBn", false); // dölj drop knappen
			break;

		case IN_GAME:
			ShowWnd("MembersEquipBn", false); // dölj equip knappen
			ShowWnd("MembersDropItemBn", true); // visa drop knappen

			m_kMembersInField.clear(); 
			GetAllAgentsInField(m_kMembersInField);

			m_iCurrentCharacterPage = 0;

			if(!m_kMembersInField.empty())
			{
				char text[50];
				sprintf(text, "Agent %i", m_kMembersInField[0]->GetEntityID());
				SetText("CurrentMemberNumberLabel", text);
				SetCharacterStats(m_kMembersInField[0]);
			}

			break;
	}
}

void CMembersDlg::SwitchCharacter(bool bNext)
{
	Entity* pkHQObject = GetDMObject(HQ);
	vector<int> vkCharsInBase;
	char text[50];

	switch(m_eWidowMode)
	{
	case HQ_EQUIP_MEMBERS:
		if(pkHQObject == NULL)
		{
			printf("Failed to get HQ object\n");
			return;
		}

		P_DMHQ* pkHQ;
		pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");
		pkHQ->GetCharacters(&vkCharsInBase);

		printf("There are %i chars in the base\n",
			vkCharsInBase.size());

		if(vkCharsInBase.empty())
		{
			SetCharacterStats(NULL);
			return;
		}

		if(bNext)
		{
			if(m_iCurrentCharacterPage < vkCharsInBase.size()-1)
				m_iCurrentCharacterPage++;
		}
		else
		{
			if(m_iCurrentCharacterPage > 0)
				m_iCurrentCharacterPage--;
		}

		int agent_object;
		agent_object = vkCharsInBase[m_iCurrentCharacterPage];
		sprintf(text, "Agent %i", agent_object);
		SetText("CurrentMemberNumberLabel", text);
		SetCharacterStats(GetObject(agent_object));
		break;

	case IN_GAME:

		if(bNext)
		{
			if(m_iCurrentCharacterPage < m_kMembersInField.size()-1)
				m_iCurrentCharacterPage++;
		}
		else
		{
			if(m_iCurrentCharacterPage > 0)
				m_iCurrentCharacterPage--;
		}

		sprintf(text, "Agent %i", 
			m_kMembersInField[m_iCurrentCharacterPage]->GetEntityID());
		SetText("CurrentMemberNumberLabel", text);
		break;

	}
}

void CMembersDlg::SetCharacterStats(Entity* pkCharacterObject)
{
	char szText[50];
	P_DMCharacter* pkCharProperty;
	DMCharacterStats* pkCharacterStats;
	
	if(pkCharacterObject == NULL)
	{
		SetText("MemberNameField", "");
		SetText("MembersArmourField", "");
		SetText("MemberSpeedField", "");
		SetText("MemberLifeField", "");
		SetText("LevelbarTopic", "");
		GetWnd("MemberIcon")->GetSkin()->m_iBkTexID = -1;
		return;
	}

	pkCharProperty = (P_DMCharacter*) 
		pkCharacterObject->GetProperty("P_DMCharacter");

	ZFAssert(pkCharProperty, 
		"CMembersDlg::SetCharacterStats - No character property\n");

	pkCharacterStats = pkCharProperty->GetStats();

	SetText("MemberNameField", (char*) pkCharacterStats->m_strName.c_str());
	SetNumber("MembersArmourField", (int) pkCharacterStats->m_fArmour);
	SetNumber("MemberSpeedField", (int) pkCharacterStats->m_fSpeed);
	SetNumber("MemberWageField", (int) pkCharacterStats->m_fWage);

	sprintf(szText, "%i/%i", pkCharacterStats->m_iLife, 
		pkCharacterStats->m_iMaxLife);
	SetText("MemberLifeField", szText);

	sprintf(szText, "Level : %i (%i/%i)", pkCharacterStats->m_iLevel,
		(int)(pkCharacterStats->m_fExperience), 
		(int)(pkCharacterStats->m_fNextLevel));
	SetText("LevelbarTopic", szText);

	string szTexName = string("data/textures/gui/dm/portraits/") +
		pkCharacterStats->m_strIcon;

	GetWnd("MemberIcon")->GetSkin()->m_iBkTexID = 
		GetTexID((char*)szTexName.c_str());

}