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
					sprintf(text, "Agent %i", vkCharsInBase[m_iCurrentCharacterPage]);
					SetText("CurrentMemberNumberLabel", text);
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
			break;
	}
}

void CMembersDlg::SwitchCharacter(bool bNext)
{
	Entity* pkHQObject = GetDMObject(HQ);
	
	switch(m_eWidowMode)
	{
	case HQ_EQUIP_MEMBERS:
		if(pkHQObject == NULL)
		{
			printf("Failed to get HQ object\n");
			return;
		}

		P_DMHQ* pkHQ = (P_DMHQ*) pkHQObject->GetProperty("P_DMHQ");

		vector<int> vkCharsInBase;
		pkHQ->GetCharacters(&vkCharsInBase);

		printf("There are %i chars in the base\n",
			vkCharsInBase.size());

		if(vkCharsInBase.empty())
			return;

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

		char text[50];
		sprintf(text, "Agent %i", vkCharsInBase[m_iCurrentCharacterPage]);
		SetText("CurrentMemberNumberLabel", text);

		break;
	}
}