#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../zerofpsv2/engine_systems/propertys/p_linktojoint.h"
#include "dark_metropolis.h"
#include "members_dlg.h"
#include "gameplay_dlg.h"
#include "handleagents_dlg.h"
#include "./members_dlg.h"
#include "../zerofpsv2/basic/zguifont.h"

CMembersDlg::CMembersDlg() : CGameDlg("MembersWnd", &g_kDM)
{
	m_iActiveCharacteInBase = -1;
	m_iCurrentCharacterPage=0;
	m_pkMoveButton = NULL;
	m_iCursorRangeDiffX=m_iCursorRangeDiffY=0;

	m_rcItemContainer				= Rect(355+35,11+51,355+221,11+206);
	m_rcArmorContainer		   = Rect(263+17,70-8,263+17,70-8);
	m_rcCyberneticsContainer   = Rect(303,184,396,277);
	m_rcQuickItemContainer	   = Rect(-38+65,235+46,-38+189,235+77);
	m_rcWeaponContainer			= Rect(17+12,37+31,110+12,99+31);

	m_pkMoveInfo = NULL;
	m_pkSelectInfo = NULL;
	m_pkModellCamera = NULL;

	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
}

CMembersDlg::~CMembersDlg() 
{
}

void CMembersDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName)
{
	// MembersWnd
	if(strClickName == "MembersCloseBn")
	{
		for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
			m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;

		ShowWnd("MembersWnd", false, false);
	//	ShowWnd("TestCameraWnd", false, false);

		if(m_eWidowMode == HQ_EQUIP_MEMBERS)
		{
			m_pkGui->KillWndCapture(); 
			//m_pkGui->SetCaptureToWnd(GetWnd("HQWnd"));
		}
		else
		if(m_eWidowMode == HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING)
		{
			m_pkGui->KillWndCapture(); 
			//m_pkGui->SetCaptureToWnd(GetWnd("AgentsWnd"));
		}
		else
		{
			m_pkGui->KillWndCapture();
		}

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "MembersEquipBn")
	{
		m_pkGui->KillWndCapture(); 
		LoadDlg("data/script/gui/dm_itemtransaction.lua");
		ShowWnd("ItemTransactionWnd", true/*, true*/);
		SetText("RemoveItemBn", "Unequip");
		SetText("AddItemBn", "Equip");

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "PrevMemberBn")
	{
		SwitchCharacter(false);

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "NextMemberBn")
	{
		SwitchCharacter(true);

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "MembersDropItemBn")
	{
		if(DropItem(m_pkSelectInfo))
		{
			m_pkAudioSys->StartSound("data/sound/drop_item.wav", 
				m_pkAudioSys->GetListnerPos()); 
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

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	{
		if(m_pkSelectInfo != NULL)
		{
			delete m_pkSelectInfo;
			m_pkSelectInfo = NULL;
			ShowWnd("MemberSelItemBorder", false);
		}
	}

}

void CMembersDlg::SetWindowMode(WINDOW_MODE eType)
{



	m_eWidowMode = eType;

	Entity* pkHQObject = GetDMObject(HQ);

	ShowWnd("MemberSelItemBorder", false);
	m_pkMoveButton = NULL;

	//if(m_pkModellCamera == NULL)
	//{
	//	CreateCamera();
	//}
	//else
	//{
	//	ShowWnd("TestCameraWnd", true);
	//}

	switch(eType)
	{
		case HQ_EQUIP_MEMBERS:

			printf("HQ_EQUIP_MEMBERS\n");

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
					SetCharacterStats(GetObject(vkCharsInBase[m_iCurrentCharacterPage]));
					m_iActiveCharacteInBase = vkCharsInBase[m_iCurrentCharacterPage];
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

			int iAgentObject;
			iAgentObject = ((CHandleAgents*)GetGameDlg(HANDLE_AGENTS_DLG))->GetSelAgent();

			for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
				m_vkItemButtons[i]->Hide();

			SetStats(&(((CHandleAgents*)GetGameDlg(HANDLE_AGENTS_DLG))->m_kViewAgentInfo));

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
				SetCharacterStats(GetObject(iSelAgentObject));
			
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
			}
			else
			{
				SetCharacterStats(NULL);
			}

			break;
	}
}

void CMembersDlg::SwitchCharacter(bool bNext)
{
	Entity* pkHQObject = GetDMObject(HQ);
	vector<int> vkCharsInBase;

	if(m_pkSelectInfo)
	{
		delete m_pkSelectInfo;
		m_pkSelectInfo = NULL;

		for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
		{
			m_vkItemButtons[i]->GetButtonUpSkin()->m_unBorderSize = 0;
			m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;
		}
	}

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
		SetCharacterStats(GetObject(agent_object));
		m_iActiveCharacteInBase = agent_object;
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

		if(m_kMembersInField.empty() == false)
		{
			SetCharacterStats(m_kMembersInField[m_iCurrentCharacterPage]);
			((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->SelectAgent(
				m_kMembersInField[m_iCurrentCharacterPage]->GetEntityID(), true);
		}
		else
		{
			SetCharacterStats(NULL);
		}

		break;

	}
}

void CMembersDlg::SetStats(DMCharacterStats* pkCharacterStats)
{
	//static ZGuiFont* pkFont = NULL;

	//if(pkFont == NULL)
	//{
	//	ZGuiFont* pkFont = new ZGuiFont(16,16,0,221222);				
	//	pkFont->CreateFromFile("data/textures/text/test2.bmp");			
	//	
	//}

	//
	//	GetWnd("MemberName")->SetFont(pkFont);
	//	GetWnd("MemeberArmor")->SetFont(pkFont);
	//	GetWnd("MemberSpeed")->SetFont(pkFont);
	//	GetWnd("MemeberWage")->SetFont(pkFont);

	char szText[150];	

	sprintf(szText, "Name: %s", pkCharacterStats->m_strName.c_str());
	GetWnd("MemberName")->SetText(szText);

	sprintf(szText, "Armor: %i", (int) pkCharacterStats->m_fArmour);
	GetWnd("MemeberArmor")->SetText(szText);

	sprintf(szText, "Speed: %i", (int) pkCharacterStats->m_fSpeed);
	GetWnd("MemberSpeed")->SetText(szText);

	sprintf(szText, "Wage: %i%", (int) pkCharacterStats->m_fWage);
	GetWnd("MemeberWage")->SetText(szText);


	//char szText[50];ok

	//sprintf(szText, "Agent %s", pkCharacterStats->m_strName.c_str());
	//SetText("CurrentMemberNumberLabel", szText);
	//SetText("MemberNameField", (char*) pkCharacterStats->m_strName.c_str());
	//SetNumber("MembersArmourField", (int) pkCharacterStats->m_fArmour);
	//SetNumber("MemberSpeedField", (int) pkCharacterStats->m_fSpeed);
	//SetNumber("MemberWageField", (int) pkCharacterStats->m_fWage);

	//sprintf(szText, "%i/%i", pkCharacterStats->m_iLife, 
	//	pkCharacterStats->m_iMaxLife);
	//SetText("MemberLifeField", szText);

	UpdateLevelbar(pkCharacterStats);
	UpdateHealthbar(pkCharacterStats);

	//string szTexName = string("data/textures/gui/dm/portraits/") +
	//	pkCharacterStats->m_strIcon;

	//GetWnd("MemberIcon")->GetSkin()->m_iBkTexID = 
	//	GetTexID((char*)szTexName.c_str());
}

void CMembersDlg::SetCharacterStats(Entity* pkCharacterObject)
{
	const int CELL_SIZE = 32;

	P_DMCharacter* pkCharProperty=NULL;
	DMCharacterStats* pkCharacterStats=NULL;
	DMContainer* pkBackPack=NULL, *pkArmor=NULL;

	if(pkCharacterObject == NULL)
	{
		//SetText("MemberNameField", "");
		//SetText("MembersArmourField", "");
		//SetText("MemberSpeedField", "");
		//SetText("MemberLifeField", "");
		SetText("CurrentMemberNumberLabel", "");
	//	GetWnd("MemberIcon")->GetSkin()->m_iBkTexID = -1;
		UpdateLevelbar(NULL);
		UpdateHealthbar(NULL);
		return;
	}

	pkCharProperty = (P_DMCharacter*) 
		pkCharacterObject->GetProperty("P_DMCharacter");

	ZFAssert(pkCharProperty, 
		"CMembersDlg::SetCharacterStats - No character property\n");

	SetStats(pkCharProperty->GetStats());

	pkCharacterStats = pkCharProperty->GetStats();

	//sprintf(szText, "Agent %i", pkCharacterObject->GetEntityID());
	//SetText("CurrentMemberNumberLabel", szText);
	//SetText("MemberNameField", (char*) pkCharacterStats->m_strName.c_str());
	//SetNumber("MembersArmourField", (int) pkCharacterStats->m_fArmour);
	//SetNumber("MemberSpeedField", (int) pkCharacterStats->m_fSpeed);
	//SetNumber("MemberWageField", (int) pkCharacterStats->m_fWage);

	//sprintf(szText, "%i/%i", pkCharacterStats->m_iLife, 
	//	pkCharacterStats->m_iMaxLife);
	//SetText("MemberLifeField", szText);

	//UpdateLevelbar(pkCharacterObject);

	//string szTexName = string("data/textures/gui/dm/portraits/") +
	//	pkCharacterStats->m_strIcon;

	//GetWnd("MemberIcon")->GetSkin()->m_iBkTexID = 
	//	GetTexID((char*)szTexName.c_str());

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
			y-m_iCursorRangeDiffY,true,false);
	}
}

void CMembersDlg::OnClick(int x, int y, bool bMouseDown, bool bLeftButton,
								  ZGuiWnd *pkMain)
{
	if(m_eWidowMode == HQ_BROWSE_MEMBERS_AND_AGENTS_AVAILABLE_FOR_HIRING)
		return;

	int dx, dy, sx, sy;

	int agent_obj_id;

	if(m_eWidowMode == IN_GAME)
		agent_obj_id = ((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->GetSelAgentObject();
	else
		agent_obj_id = m_iActiveCharacteInBase;

	// musknappen har släppts
	if(bMouseDown == false)
	{
		if(m_pkMoveInfo)
		{
			Rect rcBeforeMove = m_pkMoveInfo->m_rcBeforeMove;

			bool bMoveOK = false;
			CONTAINER_INFO kContainer;
			if( GetContainer(x, y, kContainer, agent_obj_id) )
			{
				x = m_pkMoveInfo->m_pkMoveButton->GetScreenRect().Left;
				y = m_pkMoveInfo->m_pkMoveButton->GetScreenRect().Top;

				if(GetItemPosFromCursor(x,y, dx,dy, sx,sy))
				{
					if(kContainer.pkContainer == m_pkMoveInfo->m_kFromContainer.pkContainer)
					{
						if(m_pkMoveInfo->m_kFromContainer.pkContainer->MoveItem(
							*m_pkMoveInfo->m_pMoveObject, sx, sy))
						{
							m_pkMoveInfo->m_pkMoveButton->SetPos(dx,dy,true,true); 
							m_pkMoveInfo->m_pkMoveButton->SetMoveArea(
								m_pkMoveInfo->m_pkMoveButton->GetScreenRect(),true);
							bMoveOK = true;

							m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
								m_pkAudioSys->GetListnerPos()); 
						}
					}
					else
					if((*kContainer.pkContainer->GetItem(sx,sy)) == -1)
					{
						if(m_pkMoveInfo->m_kFromContainer.pkContainer->MoveItem(
							*m_pkMoveInfo->m_pMoveObject, kContainer.pkContainer, sx, sy))
						{
							OnEquip(*kContainer.pkContainer->GetItem(sx,sy), 
								kContainer.pkContainer);

							m_pkMoveInfo->m_pkMoveButton->SetPos(dx,dy,true,true); 
							m_pkMoveInfo->m_pkMoveButton->SetMoveArea(
								m_pkMoveInfo->m_pkMoveButton->GetScreenRect(),true);
							bMoveOK = true;

							m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
								m_pkAudioSys->GetListnerPos()); 
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

			if(!(rcBeforeMove == m_pkMoveInfo->m_pkMoveButton->GetScreenRect()) )
			{
				delete m_pkSelectInfo;
				m_pkSelectInfo = NULL;

				for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
					m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;
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

		for(unsigned int j=0; j<m_vkItemButtons.size(); j++)
		{
			m_vkItemButtons[j]->GetButtonHighLightSkin()->m_unBorderSize = 0;
			m_vkItemButtons[j]->GetButtonDownSkin()->m_unBorderSize = 0;
			m_vkItemButtons[j]->GetButtonUpSkin()->m_unBorderSize = 0;
			m_vkItemButtons[j]->GetSkin()->m_unBorderSize = 0;
		}

		for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
		{
			Rect rc = m_vkItemButtons[i]->GetScreenRect();
			if(x>rc.Left && x<rc.Right && y>rc.Top && y<rc.Bottom )
			{
				CONTAINER_INFO kContainer;
				if( GetContainer(x, y, kContainer, agent_obj_id) )
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

						// Move ikon to top and set movearea to parent window
						static int s_okaZ = 10000;
						m_pkMoveInfo->m_pkMoveButton->m_iZValue = s_okaZ++;
						GetWnd("MembersWnd")->SortChilds(); 
						m_pkMoveInfo->m_pkMoveButton->SetMoveArea(
							GetWnd("MembersWnd")->GetScreenRect(), false); 

						m_bNothingSelected = false;

						m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
							m_pkAudioSys->GetListnerPos()); 
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
			for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
			{
				m_vkItemButtons[i]->GetButtonUpSkin()->m_unBorderSize = 0;
				m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;
			}

			if(m_pkSelectInfo != NULL)
			{
				delete m_pkSelectInfo;
				m_pkSelectInfo = NULL;
				ShowWnd("MemberSelItemBorder", false);
			}
		}

	}
}

// x och y är skärmkoordinater
bool CMembersDlg::GetContainer(int x, int y, CONTAINER_INFO& kContainer, int agent_obj_id)
{
	ZGuiWnd* pkMain;
	DMContainer* pkBackPack=NULL, *pkArmor=NULL;
	DMContainer* pkCybernetics=NULL, *pkQuickSlots=NULL;
	DMContainer* pkWeapons=NULL;
	P_DMCharacter* pkCharProperty=NULL;

	if((pkMain = GetWnd("MembersWnd")) == NULL)
		return false;

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

bool CMembersDlg::DropItem(ITEM_MOVE_INFO* pkObject)
{
	if(pkObject == NULL)
	{
		printf("Failed to drop item\n");
		return false;
	}

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

	if(pkObject->m_kFromContainer.pkContainer->DropItem(
		*pkObject->m_pMoveObject) == true)
	{
		pkObject->m_pkMoveButton->Hide();

		for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
			m_vkItemButtons[i]->GetSkin()->m_unBorderSize = 0;

		delete m_pkSelectInfo;
		m_pkSelectInfo = NULL;

		return true;
	}
	else
	{
		printf("Failed to drop item\n");
	}

	return false;
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

	for(unsigned int i=0; i<m_vkItemButtons.size(); i++)
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

			for(unsigned int i=0; i<kItemList.size(); i++)
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

				SetButtonIcon(pkButton, strIcon, false);
				pkButton->m_bUseAlhpaTest = false;
				pkButton->Show();
				pkButton->Disable();

				// Move ikon to top and set movearea to parent window
				static int s_okaZ = 10000;
				pkButton->m_iZValue = s_okaZ++;
				GetWnd("MembersWnd")->SortChilds(); 

				m_vkItemButtons.push_back(pkButton);
			}
		}
	}
}

void CMembersDlg::UpdateLevelbar(DMCharacterStats* pkCharacterStats)
{
	bool bFailed = false;

	if(pkCharacterStats == NULL)
	{
		bFailed = true;
	}
	else
	{
		if(pkCharacterStats)
		{
			int Level = pkCharacterStats->m_iLevel;
			int XP = (int)pkCharacterStats->m_fExperience;
			int XPNextLevel = (int)pkCharacterStats->m_fNextLevel;

			if(XP > XPNextLevel)
				XP = XPNextLevel;

			char szText[50];
			sprintf(szText, "Level : %i (%i/%i)", Level, XP, XPNextLevel);
			SetText("LevelbarTopic", szText);

			const float MAX_SIZE_LEVELBAR = (float) GetWnd("LevelbarBK")->GetScreenRect().Width();

			float procent_av_next_level = (float) XP / (float) XPNextLevel;

			GetWnd("LevelbarFront")->Resize(
				(int)(procent_av_next_level*MAX_SIZE_LEVELBAR),20,true); 

		}

	}

	if(bFailed)
	{
		SetText("LevelbarTopic", "");
		GetWnd("LevelbarFront")->Resize(0,20,true); 
	}
}

void CMembersDlg::UpdateHealthbar(DMCharacterStats* pkCharacterStats)
{
	bool bFailed = false;

	if(pkCharacterStats == NULL)
	{
		bFailed = true;
	}
	else
	{
		if(pkCharacterStats)
		{
			int XP = (int)pkCharacterStats->m_iLife;
			int XPNextLevel = (int)pkCharacterStats->m_iMaxLife;

			if(XP > XPNextLevel)
				XP = XPNextLevel;

			char szText[50];
			sprintf(szText, "Life : (%i/%i)", XP, XPNextLevel);
			SetText("HealthBarTopic", szText);

			const float MAX_SIZE_LEVELBAR = (float) GetWnd("HealthBarBK")->GetScreenRect().Width();

			float procent_av_next_level = (float) XP / (float) XPNextLevel;

			GetWnd("HealthBarFront")->Resize(
				(int)(procent_av_next_level*MAX_SIZE_LEVELBAR),20,true); 

		}

	}

	if(bFailed)
	{
		SetText("HealthBarTopic", "");
		GetWnd("HealthBarFront")->Resize(0,20,true); 
	}
}

void CMembersDlg::CreateCamera()
{
	//create camera
	m_pkModellCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);	
	m_pkModellCamera->SetName("ModellCamera");
	m_pkModellCamera->SetRender(true);
	m_pkModellCamera->SetSelected(true);
	
	EntityManager* pkObjectMan = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkCameraObject = pkObjectMan->CreateObjectFromScript("data/script/objects/t_camedit.lua");
	
	ZGuiWnd* pkWnd = CreateWnd(Wnd, "TestCameraWnd", "GuiMainWnd", "", 0,0, 256, 256, 0);
	pkWnd->SetRenderTarget(m_pkModellCamera); 
	pkWnd->GetSkin()->m_bTransparent = false; //true; 

	if(m_pkCameraObject) 
	{
		m_pkCameraObject->SetParent( pkObjectMan->GetWorldObject() );
		P_Camera* m_pkCamProp = (P_Camera*)m_pkCameraObject->GetProperty("P_Camera");
		m_pkCameraObject->SetLocalPosV(Vector3(0,0,0));
		m_pkCamProp->SetCamera(m_pkModellCamera);
		m_pkCameraObject->GetSave() = false;
	}
}

void CMembersDlg::UpdateCamera()
{
	//if(m_pkModellCamera)
	//{
	//	if(GetWnd("TestCameraWnd")->IsVisible())
	//	{
	//		m_pkModellCamera->SetRender(true);			
	//		//m_pkModellCamera->ClearViewPort(); 
	//		//m_pkModellCamera->UpdateAll(256, 256);
	//	}
	//	else
	//	{
	//		//m_pkModellCamera->SetRender(false);			
	//	}
	//}
}

void CMembersDlg::OnEquip(int iItemID, DMContainer* pkDestContainer)
{
	printf("Calling OnEquip\n");

	m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
		m_pkAudioSys->GetListnerPos()); 

	Entity* pkEntity = m_pkFps->m_pkObjectMan->GetObjectByNetWorkID ( iItemID );

	if ( !pkEntity )
		return;

	vector<int>* pkItemTypes = pkDestContainer->GetItemTypes();

	// check type of item equipped
	for ( unsigned int i = 0; i < pkItemTypes->size(); i++ )
	{
			
		// equip weapon
		if ( pkItemTypes->at(i) == DMITEM_WEAPON )
		{
			// Get owner object
			Entity* pkOwner = m_pkFps->m_pkObjectMan->GetObjectByNetWorkID ( pkDestContainer->GetOwnerID() ); 
			pkEntity->SetLocalPosV ( Vector3(0,0,0) );

			P_LinkToJoint* pkLink = (P_LinkToJoint*)pkEntity->AddProperty ("P_LinkToJoint");      
			pkLink->SetJoint( "righthand" );
		}

		if ( pkItemTypes->at(i) == DMITEM_ARMOUR )
		{

		}
	}

	((CGamePlayDlg*)GetGameDlg(GAMEPLAY_DLG))->SelectAgent(
		m_kMembersInField[m_iCurrentCharacterPage]->GetEntityID(), true);
}