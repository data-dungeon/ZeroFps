#include "dark_metropolis.h"
#include "gameplay_dlg.h"
#include "members_dlg.h"

CGamePlayDlg::CGamePlayDlg() : CGameDlg("GamePlayScreen", &g_kDM)
{
	m_iSelectedAgent = -1;
	m_bSkillbarIsOut = false;
	m_bActionbarIsOut = false;
	m_bCharbarIsOut = true;
	m_bItembarIsOut = true;
}

CGamePlayDlg::~CGamePlayDlg()
{
}

void CGamePlayDlg::OnCommand(ZGuiWnd *pkMainWnd, string strClickName,
									  bool bRMouseBnClick)
{
	if(strClickName == "GamPlayMenuBn")
	{
		ShowWnd("GamePlayScreen", false);
		LoadDlg("data/script/gui/dm_start.lua");
		pkMainWnd->Hide();
		m_pkGui->SetCaptureToWnd(GetWnd("DMStartWnd"));

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 

		m_pkDM->PauseGame(true); 
	}	
	else
	if(strClickName == "HQBn")
	{
		if(IsWndVisible("HQWnd"))
			ShowWnd("HQWnd", false);
		else
			GetGameDlg(HQ_DLG)->OpenDlg();
	}
	else
	//
	//	Kolla på info om ens agent, equipa andra vapen etc.
	//
	if(strClickName == "ActiveCharacterPortraitBn") 
	{
		Entity* pkActiveCharObject = GetObject(m_iSelectedAgent);

		if(pkActiveCharObject)
		{
			P_DMCharacter* pkCharacter = (P_DMCharacter*)
				pkActiveCharObject->GetProperty("P_DMCharacter");

			if(pkCharacter)
			{
				if(pkCharacter->GetStats()->m_iLife > 0)
				{
					LoadDlg("data/script/gui/dm_members_2.lua");
					m_pkGui->SetCaptureToWnd(GetWnd("MembersWnd"));

					CMembersDlg* pkMembersDlg = (CMembersDlg*) GetGameDlg(MEMBERS_DLG);

					if(pkMembersDlg)
						pkMembersDlg->SetWindowMode(CMembersDlg::IN_GAME); 

					m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
						m_pkAudioSys->GetListnerPos()); 
				}
			}
			else
			{
				UpdateAgentList();
			}
		}
	}
	else
	if(strClickName == "PauseBn")
	{
		if(m_pkDM->GetGameMode() != DarkMetropolis::PAUSED)
			m_pkDM->PauseGame(true);
		else
			m_pkDM->PauseGame(false);

		m_pkAudioSys->StartSound("data/sound/computer beep 5.wav", 
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "skill_button_ext")
	{
		ShowWnd("skill_bar", true);
		ShowWnd("skill_button_ext", false);
		m_bSkillbarIsOut = true;

		m_pkAudioSys->StartSound("data/sound/computer beep 2.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "skill_button")
	{
		ShowWnd("skill_bar", false);
		ShowWnd("skill_button_ext", true);
		m_bSkillbarIsOut = false;

		m_pkAudioSys->StartSound("data/sound/computer beep 3.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "action_button_ext")
	{
		ShowWnd("action_bar", true);
		ShowWnd("action_button_ext", false);
		m_bActionbarIsOut = true;

		m_pkAudioSys->StartSound("data/sound/computer beep 2.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "action_button")
	{
		ShowWnd("action_bar", false);
		ShowWnd("action_button_ext", true);
		m_bActionbarIsOut = false;

		m_pkAudioSys->StartSound("data/sound/computer beep 3.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "char_panel_button_ext")
	{
		InitDlg(); // <- Måste tydligen uppdatera charlsitan en extra gång!?!!?

		ShowWnd("panel_char", true);
		ShowWnd("char_panel_button_ext", false);
		m_bCharbarIsOut = true;

		m_pkAudioSys->StartSound("data/sound/panel_out.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "char_panel_button" /*|| 
		strClickName.find("CharPortBn") != string::npos*/)
	{
		ShowWnd("panel_char", false);
		ShowWnd("char_panel_button_ext", true);
		m_bCharbarIsOut = false;

		m_pkAudioSys->StartSound("data/sound/panel_in.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "item_button_ext")
	{
		ShowWnd("item_bar", true);
		ShowWnd("item_button_ext", false);
		m_bItembarIsOut=true;

		m_pkAudioSys->StartSound("data/sound/panel_out.wav",
			m_pkAudioSys->GetListnerPos()); 
	}
	else
	if(strClickName == "item_button")
	{
		ShowWnd("item_bar", false);
		ShowWnd("item_button_ext", true);
		m_bItembarIsOut=false;

		m_pkAudioSys->StartSound("data/sound/panel_in.wav",
			m_pkAudioSys->GetListnerPos() ); 
	}
	else
	if(strClickName == "MissionInfoOK")
	{
		GetWnd("MissionInfoWnd")->Hide();
	}
	else
	if(strClickName.find("GamePlayDlgQuickItem") != string::npos && m_iSelectedAgent != -1)
	{
		char number[] = { strClickName.c_str()[strClickName.size()-1], '\0' };
		int iItem = atoi(number)-1;

		if(m_iSelectedAgent != -1)
		{
			P_DMCharacter* pkCharacter = (P_DMCharacter*)
				GetObject(m_iSelectedAgent)->GetProperty("P_DMCharacter");

			m_pkAudioSys->StartSound("data/sound/computer beep 6.wav", 
				m_pkAudioSys->GetListnerPos()); 

			pkCharacter->UseQuickItem(iItem, false);
			SelectAgentGUI(m_iSelectedAgent, false);
		}
		else
		{
			printf("Failed to use quickitem, no agent selected\n");
		}
		
	}	

	//
	// Markera en agent
	//
	char* icon_name[] ={
		"CharPortBn1","CharPortBn2","CharPortBn3","CharPortBn4","CharPortBn5",
	};

	for(int i=0; i<5; i++)
		if(strClickName == icon_name[i])
		{
			Entity* pkEnt = GetObject(GetSelAgentObject()); 

			//bool bDoubleClick = false;

			if(pkEnt && bRMouseBnClick)
				m_pkDM->MoveCamera(pkEnt->GetWorldPosV());

			SelectAgentGUI(m_akAgetIcons[i].iAgentObjectID, true);
		}
}

bool CGamePlayDlg::InitDlg()
{
	if(!m_bInitialized)
	{
		ShowWnd("CharPortBn1", false);
		ShowWnd("CharPortBn2", false);
		ShowWnd("CharPortBn3", false);
		ShowWnd("CharPortBn4", false);
		ShowWnd("CharPortBn5", false);
		ShowWnd("CharPortBn6", false);
		ShowWnd("CharPortBn7", false);

		char* szNameID[] =
		{
			"CharPortBn1", "Char1LifeBk1",
			"CharPortBn2", "Char1LifeBk2",
			"CharPortBn3", "Char1LifeBk3",
			"CharPortBn4", "Char1LifeBk4",
			"CharPortBn5", "Char1LifeBk5",
		};

		for(int i=0; i<5; i++)
		{
			m_akAgetIcons[i].pkButton = (ZGuiButton*)GetWnd(szNameID[i*2]);
			m_akAgetIcons[i].pkButton->m_bAcceptRightClicks = true;
			m_akAgetIcons[i].pkLifeProgressbar = (ZGuiLabel*)GetWnd(szNameID[i*2+1]);
		}		

		ZGuiButton* pkActiveCharBn = (ZGuiButton*) 
			GetWnd("ActiveCharacterPortraitBn");
		pkActiveCharBn->Hide();

		((ZGuiTextbox*)GetWnd("MissionInfoLabel"))->ToggleMultiLine(true);
		((ZGuiTextbox*)GetWnd("MissionInfoLabel"))->SetReadOnly(true);	

		m_pkDM->m_pkGamePlayInfoLabel = GetWnd("GamePlayInfoLabel");
	}

	GetWnd("GamePlayDlgQuickItem1")->Hide();
	GetWnd("GamePlayDlgQuickItem2")->Hide();
	GetWnd("GamePlayDlgQuickItem3")->Hide();
	GetWnd("GamePlayDlgQuickItem4")->Hide();
	GetWnd("GamePlayDlgWeaponBn")->Hide();

	if(m_iSelectedAgent == -1)
	{
		ZGuiButton* pkActiveCharBn = (ZGuiButton*) 
			GetWnd("ActiveCharacterPortraitBn");
		pkActiveCharBn->Hide();
	}
 
	GetWnd("MissionInfoWnd")->Hide();

	if(m_bSkillbarIsOut) 
	{
		GetWnd("skill_bar")->Show();
		ShowWnd("skill_button_ext", false);
	}
	else GetWnd("skill_bar")->Hide();
	
	if(m_bActionbarIsOut) 
	{
		GetWnd("action_bar")->Show();
		ShowWnd("action_button_ext", false);
	}
	else GetWnd("action_bar")->Hide();
	
	if(m_bCharbarIsOut) 
	{
		GetWnd("panel_char")->Show();
		ShowWnd("char_panel_button_ext", false);
	}
	else GetWnd("panel_char")->Hide();
	
	if(m_bItembarIsOut) 
	{
		GetWnd("item_bar")->Show();
		ShowWnd("item_button_ext", false);
	}
	else GetWnd("item_bar")->Hide();
	
	UpdateAgentList();

	m_bInitialized = true;

	SelectAgentGUI(m_iSelectedAgent, false);

	return true;
}

void CGamePlayDlg::SelectAgentGUI(int iAgent, bool bSelectModels)
{
	if(m_bInitialized == false)
		return;

//	printf("CGamePlayDlg::SelectAgentGUI\n");

	Entity* pkAgentObject;
	P_DMCharacter* pkCharProperty;
	DMCharacterStats* pkCharacterStats;

	ZGuiButton* pkActiveCharBn = (ZGuiButton*) 
		GetWnd("ActiveCharacterPortraitBn");

	pkActiveCharBn->Hide();

	for(int i=0; i<5; i++)
	{
		m_akAgetIcons[i].pkButton->GetButtonHighLightSkin()->m_unBorderSize = 0; 
		m_akAgetIcons[i].pkButton->GetButtonDownSkin()->m_unBorderSize = 0; 				
		m_akAgetIcons[i].pkButton->GetButtonUpSkin()->m_unBorderSize = 0; 
	}

	if((pkAgentObject = GetObject(iAgent)))
	{
		pkCharProperty = (P_DMCharacter*) 
			pkAgentObject->GetProperty("P_DMCharacter");

		ZFAssert(pkCharProperty, 
			"CMembersDlg::SetCharacterStats - No character property\n");

		pkCharacterStats = pkCharProperty->GetStats();

		string szTexName = string("data/textures/gui/dm/portraits/") +
			pkCharacterStats->m_strIcon;

		pkActiveCharBn->Show();

		SetButtonIcon(pkActiveCharBn, szTexName, false);

		int iItemID;

		if((iItemID=*pkCharProperty->m_pkHand->GetItem(0,0)) != -1)
		{
			Entity* pkEntity = GetObject( iItemID );
			P_DMItem* pkItemProperty;

			if((pkItemProperty=(P_DMItem*)pkEntity->GetProperty("P_DMItem")))
			{
				szTexName = string("data/textures/gui/dm/items/") +
					pkItemProperty->GetIcon();
				ZGuiButton* pkActiveWeaponBn = (ZGuiButton*) 
					GetWnd("GamePlayDlgWeaponBn");
				GetWnd("GamePlayDlgWeaponBn")->Show();
				SetButtonIcon(pkActiveWeaponBn, szTexName, false, true);		
			}
		}
		else
		{
			GetWnd("GamePlayDlgWeaponBn")->Hide();
		}

		for(int i=0; i<4; i++)
		{
			char szQuickItem[50];
			sprintf(szQuickItem, "GamePlayDlgQuickItem%i", i+1);

			ZGuiButton* pkQuickItemBn = (ZGuiButton*) 
				GetWnd(szQuickItem);

			pkQuickItemBn->Hide();

			if((iItemID=*pkCharProperty->m_pkBelt->GetItem(i,0)) != -1)
			{
				Entity* pkEntity = GetObject( iItemID );
				P_DMItem* pkItemProperty;

				if((pkItemProperty=(P_DMItem*)pkEntity->GetProperty("P_DMItem")))
				{
					szTexName = string("data/textures/gui/dm/items/") +
						pkItemProperty->GetIcon();

					pkQuickItemBn->Show();
					SetButtonIcon(pkQuickItemBn, szTexName, false, true);		
				}
			}
		}

		
		// Lägg till en ram kring valt porträtt och ta bort gammla ramar
		for(int i=0; i<5; i++)
		{
			if(m_akAgetIcons[i].iAgentObjectID == iAgent)
			{
				m_akAgetIcons[i].pkButton->GetButtonHighLightSkin()->m_unBorderSize = 2; 
				m_akAgetIcons[i].pkButton->GetButtonDownSkin()->m_unBorderSize = 2; 
				m_akAgetIcons[i].pkButton->GetButtonUpSkin()->m_unBorderSize = 2; 
			}
			else
			{
				m_akAgetIcons[i].pkButton->GetButtonHighLightSkin()->m_unBorderSize = 0; 
				m_akAgetIcons[i].pkButton->GetButtonDownSkin()->m_unBorderSize = 0; 				
				m_akAgetIcons[i].pkButton->GetButtonUpSkin()->m_unBorderSize = 0; 
			}
		}

		bool bMultiSelect = m_pkDM->m_pkInputHandle->VKIsDown("multiselect");

		if(bSelectModels)
			m_pkDM->SelectAgent(iAgent, true, !bMultiSelect, true);

		m_iSelectedAgent = iAgent;
	}

	if(pkAgentObject == NULL)
	{
		m_iSelectedAgent = -1;
		pkActiveCharBn->Hide();
	}

	//ZFAssert(pkAgentObject, "CGamePlayDlg::SelectAgent - Bad agent ID");
}

DMCharacterStats* CGamePlayDlg::GetAgentStats(int iAgent)
{
	Entity* pkAgentObject;
	P_DMCharacter* pkCharProperty;
	DMCharacterStats* pkCharacterStats;

	if((pkAgentObject = GetObject(iAgent)))
	{
		pkCharProperty = (P_DMCharacter*) 
			pkAgentObject->GetProperty("P_DMCharacter");

		ZFAssert(pkCharProperty, 
			"CMembersDlg::SetCharacterStats - No character property\n");

		pkCharacterStats = pkCharProperty->GetStats();
		return pkCharacterStats;
	}

	return NULL;
}

void CGamePlayDlg::UpdateAgentList()
{
	if(!m_bInitialized)
		return;

	vector<Entity*> kMembersInField;
	GetAllAgentsInField(kMembersInField);
	
	for(unsigned int i=0; i<5; i++)
	{
		if(kMembersInField.size() > i)
		{
			m_akAgetIcons[i].pkButton->Show();
			m_akAgetIcons[i].pkLifeProgressbar->Show();
			m_akAgetIcons[i].iAgentObjectID = kMembersInField[i]->GetEntityID();

			string icon = GetAgentStats(kMembersInField[i]->GetEntityID())->m_strIcon;
			string szTexName = string("data/textures/gui/dm/portraits/") + icon;
			SetButtonIcon(m_akAgetIcons[i].pkButton, szTexName, false);

			DMCharacterStats* pkStats = GetAgentStats(m_akAgetIcons[i].iAgentObjectID);

			float curr = pkStats->m_iLife <= pkStats->m_iMaxLife ? 
				pkStats->m_iLife : pkStats->m_iMaxLife;
			float max = float(pkStats->m_iMaxLife);

			if(curr < 0)
				curr = 0;

			float procent_av_max = 1 - (float) curr / (float) max;
			float w = 64.0f - (procent_av_max * 64.f);
			m_akAgetIcons[i].pkLifeProgressbar->Resize((int)w,16,true); 
		}
		else
		{
			m_akAgetIcons[i].pkButton->Hide();
			m_akAgetIcons[i].pkLifeProgressbar->Hide();
			m_akAgetIcons[i].iAgentObjectID = -1;
		}
	}
}

void CGamePlayDlg::UpdateSelAgent(int iID)
{
	//printf("CGamePlayDlg::UpdateSelAgent\n");

	//ZGuiButton* pkActiveCharBn = (ZGuiButton*) GetWnd("ActiveCharacterPortraitBn");
	//pkActiveCharBn->Hide();
	//m_iSelectedAgent = iID;

	//for(int i=0; i<5; i++)
	//{
	//	m_akAgetIcons[i].pkButton->GetButtonHighLightSkin()->m_unBorderSize = 0; 
	//	m_akAgetIcons[i].pkButton->GetButtonDownSkin()->m_unBorderSize = 0; 				
	//	m_akAgetIcons[i].pkButton->GetButtonUpSkin()->m_unBorderSize = 0; 
	//}

	//for(int i=0; i<5; i++)
	//{
	//	if(m_akAgetIcons[i].iAgentObjectID == iID)
	//	{
	//		m_akAgetIcons[i].pkButton->GetButtonHighLightSkin()->m_unBorderSize = 2; 
	//		m_akAgetIcons[i].pkButton->GetButtonDownSkin()->m_unBorderSize = 2; 				
	//		m_akAgetIcons[i].pkButton->GetButtonUpSkin()->m_unBorderSize = 2; 
	//		//pkActiveCharBn->Show();
	//		break;
	//	}
	//}
			
}