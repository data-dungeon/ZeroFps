#include "dark_metropolis.h"
#include "members_dlg.h"
#include "gameplay_dlg.h"

CMembersDlg::CMembersDlg() : CGameDlg("MembersWnd", &g_kDM)
{
	m_iCurrentCharacterPage=0;
	m_pkMoveButton = NULL;
	m_iCursorRangeDiffX=m_iCursorRangeDiffY=0;

	m_rcItemContainer				= Rect(423,29,640,277);
	m_rcArmorContainer		   = Rect(303,29,396,153);
	m_rcCyberneticsContainer   = Rect(303,184,396,277);
	m_rcQuickItemContainer	   = Rect(43,184,167,215);
	m_rcWeaponContainer			= Rect(74,29,136,122);

	m_pkMoveInfo = NULL;
	m_pkSelectInfo = NULL;
}

CMembersDlg::~CMembersDlg() 
{
}

void CMembersDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	// MembersWnd
	if(strClickName == "MembersClosBn")
	{
		for(int i=0; i<m_vkItemButtons.size(); i++)
			m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;

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
	else
	if(strClickName == "MembersDropItemBn")
	{
		if(m_pkSelectInfo != NULL)
		{
			DropItem(m_pkSelectInfo);
			m_pkSelectInfo = NULL;
		}
	}

	if(strClickName.find("ItemButton") != string::npos)
	{
		ShowWnd("MemberSelItemBorder", true);
		ZGuiWnd* pkSelWnd = GetWnd("MemberSelItemBorder");
		ZGuiWnd* pkClickWnd = GetWnd((char*)strClickName.c_str());
		
		Rect rc = pkClickWnd->GetScreenRect();
		pkSelWnd->SetPos(rc.Left, rc.Top, true, true);
		pkSelWnd->Resize(rc.Width(), rc.Height(), true);  
		pkSelWnd->SetMoveArea(rc, true);
	}
	else
	{
		ShowWnd("MemberSelItemBorder", false);
	}

}

void CMembersDlg::SetWindowMode(WINDOW_MODE eType)
{
	m_eWidowMode = eType;

	Entity* pkHQObject = GetDMObject(HQ);

	SetText("CurrentMemberNumberLabel", "");

	ShowWnd("MemberSelItemBorder", false);
	m_pkMoveButton = NULL;

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

			if(m_pkMoveInfo)
			{
				delete m_pkMoveInfo;
				m_pkMoveInfo = NULL;
			}

			ShowWnd("MembersEquipBn", false); // dölj equip knappen
			ShowWnd("MembersDropItemBn", true); // visa drop knappen

			m_kMembersInField.clear(); 
			GetAllAgentsInField(m_kMembersInField);

			m_iCurrentCharacterPage = 0;

			int iSelAgentObject = 
				((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->GetSelAgentObject();

			if(!m_kMembersInField.empty())
			{
				char text[50];
				sprintf(text, "Agent %i", m_kMembersInField[0]->GetEntityID());
				SetText("CurrentMemberNumberLabel", text);
				
				SetCharacterStats(GetObject(iSelAgentObject));
			}

			if(iSelAgentObject != 1)
			{
				for(int i=0; i<5; i++)
				{
					int iObjectID = m_kMembersInField[i]->GetEntityID();
					if(iSelAgentObject == iObjectID)
					{
						m_iCurrentCharacterPage = i;
						break;
					}
				}
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

		SetCharacterStats(m_kMembersInField[m_iCurrentCharacterPage]);

		((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->SelectAgent(
			m_kMembersInField[m_iCurrentCharacterPage]->GetEntityID(), true);

		break;

	}
}

void CMembersDlg::SetCharacterStats(Entity* pkCharacterObject)
{
	const int CELL_SIZE = 32;

	char szText[50];
	P_DMCharacter* pkCharProperty=NULL;
	DMCharacterStats* pkCharacterStats=NULL;
	DMContainer* pkBackPack=NULL, *pkArmor=NULL;

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

	UpdateInventory(pkCharacterObject);
}

void CMembersDlg::OnMouseMove(int x, int y, bool bMouseDown, ZGuiWnd *pkMain)
{
	if(m_eWidowMode == HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING)
		return;

	// musknappen har tryckts ner
	if(bMouseDown == true && m_pkMoveInfo != NULL) 
	{
		m_pkMoveInfo->m_pkMoveButton->SetPos(x-m_iCursorRangeDiffX,
			y-m_iCursorRangeDiffY,true,true);
	}
}

void CMembersDlg::OnClick(int x, int y, bool bMouseDown, bool bLeftButton,
								  ZGuiWnd *pkMain)
{
	if(m_eWidowMode == HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING)
		return;

	int dx, dy, sx, sy;

	// musknappen har släppts
	if(bMouseDown == false)
	{
		if(m_pkMoveInfo)
		{
			bool bMoveOK = false;
			CONTAINER_INFO kContainer;
			if( GetContainer(x, y, kContainer) )
			{
				x = m_pkMoveInfo->m_pkMoveButton->GetScreenRect().Left + 
					m_pkMoveInfo->m_pkMoveButton->GetScreenRect().Width()/4;
				y = m_pkMoveInfo->m_pkMoveButton->GetScreenRect().Top + 
					m_pkMoveInfo->m_pkMoveButton->GetScreenRect().Height()/4;

				if(GetItemPosFromCursor(x,y, dx,dy, sx,sy))
				{
					if(kContainer.pkContainer == m_pkMoveInfo->m_kFromContainer.pkContainer)
					{
						// Funkar inte i nuläget.
						printf("Don't work right now\n");
					}
					else
					if((*kContainer.pkContainer->GetItem(sx,sy)) == -1)
					{
						if(m_pkMoveInfo->m_kFromContainer.pkContainer->MoveItem(
							*m_pkMoveInfo->m_pMoveObject, kContainer.pkContainer, sx, sy))
						{
							printf("Moving item OK\n");
							kContainer.pkContainer->Print();
							m_pkMoveInfo->m_pkMoveButton->SetPos(dx,dy,true,true); 
							m_pkMoveInfo->m_pkMoveButton->SetMoveArea(
								m_pkMoveInfo->m_pkMoveButton->GetScreenRect(),true);
							bMoveOK = true;
						}
					}
				}
			}

			if(bMoveOK == false)
			{
				m_pkMoveInfo->m_pkMoveButton->SetPos(
					m_pkMoveInfo->m_rcBeforeMove.Left,
					m_pkMoveInfo->m_rcBeforeMove.Top,
					true, true);
			}

			if(m_pkMoveInfo)
			{
				delete m_pkMoveInfo;
				m_pkMoveInfo = NULL;
			}
		}

	}
	else // musknappen har tryckts ner
	{
		bool m_bNothingSelected = true;

		for(int j=0; j<m_vkItemButtons.size(); j++)
		{
			m_vkItemButtons[j]->GetButtonHighLightSkin()->m_unBorderSize = 0;
			m_vkItemButtons[j]->GetButtonDownSkin()->m_unBorderSize = 0;
			m_vkItemButtons[j]->GetButtonUpSkin()->m_unBorderSize = 0;
			m_vkItemButtons[j]->GetSkin()->m_unBorderSize = 0;
		}

		for(int i=0; i<m_vkItemButtons.size(); i++)
		{
			Rect rc = m_vkItemButtons[i]->GetScreenRect();
			if(x>rc.Left && x<rc.Right && y>rc.Top && y<rc.Bottom )
			{
				CONTAINER_INFO kContainer;
				if( GetContainer(x, y, kContainer) )
				{
					if(m_pkMoveInfo)
					{
						m_pkMoveInfo = NULL;
						delete m_pkMoveInfo;
					}

					if( GetItemPosFromCursor(x, y, dx, dy, sx, sy) )
					{
						m_pkMoveInfo = new ITEM_MOVE_INFO;
						m_pkMoveInfo->m_kFromContainer = kContainer;
						m_pkMoveInfo->m_pkMoveButton = m_vkItemButtons[i];
						m_pkMoveInfo->m_pMoveObject = 
							kContainer.pkContainer->GetItem(sx,sy);
						m_pkMoveInfo->m_rcBeforeMove = 
							m_pkMoveInfo->m_pkMoveButton->GetScreenRect();

						m_iCursorRangeDiffX = x-rc.Left; 
						m_iCursorRangeDiffY = y-rc.Top;
						GetWnd("MemberSelItemBorder")->Hide();

						m_pkMoveInfo->m_pkMoveButton->GetSkin()->m_unBorderSize = 2;
						
						if(m_pkSelectInfo)
							delete m_pkSelectInfo;

						m_pkSelectInfo = new ITEM_MOVE_INFO;
						m_pkSelectInfo->m_kFromContainer = m_pkMoveInfo->m_kFromContainer;
						m_pkSelectInfo->m_pkMoveButton = m_pkMoveInfo->m_pkMoveButton;
						m_pkSelectInfo->m_pMoveObject = m_pkMoveInfo->m_pMoveObject;

						m_bNothingSelected = false;
					}
					else
					{
						if(m_pkMoveInfo)
						{
							delete m_pkMoveInfo;
							m_pkMoveInfo = NULL;
						}
					}
				}			
				break;
			}
		}

		// Inte tryckt på nått
		if(m_bNothingSelected)
		{
			for(int i=0; i<m_vkItemButtons.size(); i++)
			{
				m_vkItemButtons[i]->GetButtonUpSkin()->m_unBorderSize = 0;
				m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;
			}
		}

	}
}

// x och y är skärmkoordinater
bool CMembersDlg::GetContainer(int x, int y, CONTAINER_INFO& kContainer)
{
	ZGuiWnd* pkMain;
	DMContainer* pkBackPack=NULL, *pkArmor=NULL;
	DMContainer* pkCybernetics=NULL, *pkQuickSlots=NULL;
	DMContainer* pkWeapons=NULL;
	P_DMCharacter* pkCharProperty=NULL;

	if((pkMain = GetWnd("MembersWnd")) == NULL)
		return false;

	int agent_obj_id = ((CGamePlayDlg*)
		GetGameDlg(GAMEPLAY_DLG))->GetSelAgentObject();

	if(agent_obj_id)
	{
		pkCharProperty = (P_DMCharacter*) 
			GetObject(agent_obj_id)->GetProperty("P_DMCharacter");
	}

	ZFAssert(pkCharProperty, 
		"CMembersDlg::SetCharacterStats - No character property\n");

	if(pkCharProperty)
	{
		pkBackPack = pkCharProperty->m_pkBackPack;
		pkArmor = pkCharProperty->m_pkBody;
		pkCybernetics = pkCharProperty->m_pkImplants;
		pkQuickSlots = pkCharProperty->m_pkBelt;
		pkWeapons = pkCharProperty->m_pkHand;
	}

	x -= pkMain->GetScreenRect().Left;
	y -= pkMain->GetScreenRect().Top;

	if(m_rcItemContainer.Inside(x,y))
	{
		kContainer.m_rcItemContainer = m_rcItemContainer;
		kContainer.pkContainer = pkBackPack;
	}
	else
	if(m_rcArmorContainer.Inside(x,y))
	{
		kContainer.m_rcItemContainer = m_rcItemContainer;
		kContainer.pkContainer = pkArmor;
	}
	else
	if(m_rcCyberneticsContainer.Inside(x,y))
	{
		kContainer.m_rcItemContainer = m_rcCyberneticsContainer;
		kContainer.pkContainer = pkCybernetics;
	}
	else
	if(m_rcQuickItemContainer.Inside(x,y))
	{
		kContainer.m_rcItemContainer = m_rcQuickItemContainer;
		kContainer.pkContainer = pkQuickSlots;
	}
	else
	if(m_rcWeaponContainer.Inside(x,y))
	{
		kContainer.m_rcItemContainer = m_rcWeaponContainer;
		kContainer.pkContainer = pkWeapons;
	}
	else
		return false;

	return true;

}

bool CMembersDlg::GetItemPosFromCursor(int x, int y, 
													int& itempos_x, int& itempos_y,
													int& slotpos_x, int& slotpos_y)
{
	ZGuiWnd* pkMain;
	Rect rc;

	int x_offset, y_offset;

	if((pkMain = GetWnd("MembersWnd")) == NULL)
		return false;

	x -= pkMain->GetScreenRect().Left;
	y -= pkMain->GetScreenRect().Top;

	if(m_rcItemContainer.Inside(x,y))
	{
		rc = m_rcItemContainer;
	}
	else
	if(m_rcArmorContainer.Inside(x,y))
	{
		rc = m_rcArmorContainer;
	}
	else
	if(m_rcCyberneticsContainer.Inside(x,y))
	{
		rc = m_rcCyberneticsContainer;
	}
	else
	if(m_rcQuickItemContainer.Inside(x,y))
	{
		rc = m_rcQuickItemContainer;
	}
	else
	if(m_rcWeaponContainer.Inside(x,y))
	{
		rc = m_rcWeaponContainer;
	}
	else
	{
		return false;
	}

	x_offset = x - rc.Left;
	y_offset = y - rc.Top;

	x_offset = x_offset / 31;
	y_offset = y_offset / 31;

	slotpos_x = x_offset;
	slotpos_y = y_offset;


	x -= rc.Left; 
	y -= rc.Top;

	x -= (x % 31); 
	y -= (y % 31);

	itempos_x = pkMain->GetScreenRect().Left + rc.Left + x;
	itempos_y = pkMain->GetScreenRect().Top + rc.Top + y;

	return true;
}

void CMembersDlg::DropItem(ITEM_MOVE_INFO* pkObject)
{
	if(pkObject == NULL)
		return;

	DMContainer* pkBackPack=NULL, *pkArmor=NULL;
	DMContainer* pkCybernetics=NULL, *pkQuickSlots=NULL;
	DMContainer* pkWeapons=NULL;
	P_DMCharacter* pkCharProperty=NULL;

	int agent_obj_id = ((CGamePlayDlg*)
		GetGameDlg(GAMEPLAY_DLG))->GetSelAgentObject();

	if(agent_obj_id)
	{
		pkCharProperty = (P_DMCharacter*) 
			GetObject(agent_obj_id)->GetProperty("P_DMCharacter");
	}

	pkObject->m_kFromContainer.pkContainer->DropItem(
		*pkObject->m_pMoveObject);

	pkObject->m_pkMoveButton->Hide();

	for(int i=0; i<m_vkItemButtons.size(); i++)
		m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;
}

void CMembersDlg::UpdateInventory(Entity* pkCharacterObject)
{
	int x, y, w, h;
	char szName[50];
	const int CELL_SIZE = 31;

	P_DMCharacter* pkCharProperty=NULL;
	
	pkCharProperty = (P_DMCharacter*) 
		pkCharacterObject->GetProperty("P_DMCharacter");

	ZFAssert(pkCharProperty, 
		"CMembersDlg::SetCharacterStats - No character property\n");

	for(int i=0; i<m_vkItemButtons.size(); i++)
		m_vkItemButtons[i]->Hide();

	DMContainer* akContainerObjects[] = 
	{
		pkCharProperty->m_pkBackPack,		// 0) Backpack
		pkCharProperty->m_pkBody,			// 1) Armor
		pkCharProperty->m_pkImplants,		// 2) Cybernectics
		pkCharProperty->m_pkBelt,			// 3) Quickitem
		pkCharProperty->m_pkHand,			// 4) Weapon
	};

	Rect aContainerRect[] =
	{
		m_rcItemContainer,				// 0) Backpack
		m_rcArmorContainer,				// 1) Armor
		m_rcCyberneticsContainer,		// 2) Cybernectics
		m_rcQuickItemContainer,			// 3) Quickitem
		m_rcWeaponContainer,				// 4) Weapon
	};

	m_vkItemButtons.clear();

	int item_counter=0;

	for(int c=0; c<5; c++)
	{
		DMContainer* pkContainer = akContainerObjects[c];
		Rect kRect = aContainerRect[c];

		if(pkContainer)
		{
			vector<ContainerInfo> kItemList;
			pkContainer->GetItemList(&kItemList);

			for(int i=0; i<kItemList.size(); i++)
			{
				x = kRect.Left + kItemList[i].m_iItemX*CELL_SIZE;
				y = kRect.Top + kItemList[i].m_iItemY*CELL_SIZE;
				w = kItemList[i].m_iItemW*CELL_SIZE;
				h = kItemList[i].m_iItemH*CELL_SIZE;

				sprintf(szName, "ItemButton%i", item_counter++);

				ZGuiButton* pkButton;
				ZGuiSkin* pkSkinUp, *pkSkinDown, *pkSkinFocus;
				
				if((pkButton=(ZGuiButton*)GetWnd(szName)) == NULL)
				{
					pkSkinUp = new ZGuiSkin();
					pkSkinDown = new ZGuiSkin();
					pkSkinFocus = new ZGuiSkin();
				}
				else
				{
					pkSkinUp = pkButton->GetButtonUpSkin();
					pkSkinDown = pkButton->GetButtonDownSkin();
					pkSkinFocus = pkButton->GetButtonHighLightSkin();
				}

				pkButton = (ZGuiButton*) CreateWnd(Button, szName, 
					"MembersWnd", "", x, y, w, h, 0);

				pkButton->Resize(w,h,true);
				pkButton->SetPos(x,y,false,true);
				pkButton->SetMoveArea(pkButton->GetScreenRect()); 

				pkButton->SetButtonDownSkin(pkSkinDown);
				pkButton->SetButtonHighLightSkin(pkSkinFocus);
				pkButton->SetButtonUpSkin(pkSkinUp);
				pkButton->SetSkin(pkSkinUp);

				string strIcon = "data/textures/gui/dm/items/" + kItemList[i].m_strIcon;

				SetButtonIcon(pkButton, strIcon, true);
				pkButton->m_bUseAlhpaTest = false;
				pkButton->Show();
				pkButton->Disable();

				m_vkItemButtons.push_back(pkButton);
			}
		}
	}
}