// statsdlg.cpp: implementation of the StatsDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "statsdlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StatsDlg::StatsDlg(ZGuiApp* pkApp)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkApp = pkApp;
	m_pkCharProperty = NULL;
	m_pkDialog = NULL;
}

StatsDlg::~StatsDlg()
{

}

void StatsDlg::Update()
{	
	if(m_pkCharProperty)
	{
		CharacterStats* pkCharStats = m_pkCharProperty->GetCharStats();
		
		// Update texboxes data
		map<string,string> kDataList;
		pkCharStats->GetData(kDataList); 
		
		m_pkApp->GetWnd("CharacterNameLabel")->SetText((char*)kDataList["Name"].c_str());
		m_pkApp->GetWnd("CharacterRaceLabel")->SetText((char*)kDataList["Race"].c_str());
		m_pkApp->GetWnd("CharacterSexLabel")->SetText((char*)kDataList["Sex"].c_str());

		// Update texboxes attributes
		map<string,StatDescriber> kAttribList;
		pkCharStats->GetAttributes(kAttribList); 

		m_pkApp->SetTextInt("CharacterAttribSTYLabel", kAttribList["str"].m_iValue); 
		m_pkApp->SetTextInt("CharacterAttribCONLabel", kAttribList["con"].m_iValue);
		m_pkApp->SetTextInt("CharacterAttribDEXLabel", kAttribList["dex"].m_iValue);
		m_pkApp->SetTextInt("CharacterAttribINTLabel", kAttribList["int"].m_iValue);
		m_pkApp->SetTextInt("CharacterAttribPIELabel", kAttribList["pie"].m_iValue);

		// Update mp/hp
		m_pkApp->SetTextInt("CharacterCounterHPLabel", pkCharStats->GetHP()); 
		m_pkApp->SetTextInt("CharacterCounterMPLabel", pkCharStats->GetMP()); 

		// Update listbox skills
		map<string,StatDescriber> kSkillList;
		pkCharStats->GetSkills(kSkillList);

		if((unsigned int) m_pkSkillList->GetItemCount() != kSkillList.size())
		{
			int id=0;
			m_pkSkillList->RemoveAllItems();
			map<string,StatDescriber>::iterator itSkills = kSkillList.begin();
			for( ; itSkills != kSkillList.end(); itSkills++)
			{
				char szItemName[50];
				sprintf(szItemName, "%s %i", itSkills->first.c_str(), itSkills->second.m_iValue);
				m_pkSkillList->AddItem(szItemName, id, false);

				id++;
			}
		}
	}
}

void StatsDlg::Init()
{
	int screen_w = 800; //m_pkApp->GetWidth(); 
	int screen_h = 600; //m_pkApp->GetHeight();

	// Set textures skills listbox
	m_pkSkillList = (ZGuiListbox*) m_pkApp->GetWnd("SkillsListbox");
	m_pkAttackDefList =  (ZGuiListbox*) m_pkApp->GetWnd("AttackDefListbox");

	m_pkSkillList->SetSkin( new ZGuiSkin());			
	m_pkSkillList->GetSkin()->m_iBkTexID = 
		m_pkTexMan->Load("/data/textures/gui/stone.bmp", 0);
	m_pkSkillList->GetSkin()->m_bTileBkSkin = true;
	m_pkSkillList->GetScrollbar()->SetScrollInfo(0,100,0.15f,0);

	ZGuiSkin* pkItemUpSkin = new ZGuiSkin();
	ZGuiSkin* pkItemFocusSkin = new ZGuiSkin();
	ZGuiSkin* pkItemDownSkin = new ZGuiSkin();

	pkItemUpSkin->m_bTransparent = true;
	pkItemFocusSkin->m_bTransparent = true;
	pkItemDownSkin->m_bTransparent = true;
	pkItemDownSkin->m_unBorderSize = 1;
	memset(pkItemDownSkin->m_afBorderColor, 0, sizeof(float)*3); 
	
	m_pkSkillList->SetItemNormalSkin(pkItemUpSkin); 
	m_pkSkillList->SetItemHighLightSkin(pkItemFocusSkin); 
	m_pkSkillList->SetItemSelectedSkin(pkItemDownSkin); 

	ZGuiSkin* pkUpSkinNorm = m_pkApp->GetSkin("DefSBrScrollUpSkin_u");
	ZGuiSkin* pkUpSkinSel = m_pkApp->GetSkin("DefSBrScrollUpSkin_d");

	ZGuiSkin* pkDownSkinNorm = m_pkApp->GetSkin("DefSBrScrollDownSkin_u");
	ZGuiSkin* pkDownSkinSel = m_pkApp->GetSkin("DefSBrScrollDownSkin_d");

	m_pkSkillList->GetScrollbar()->SetScrollButtonUpSkins(pkUpSkinNorm, pkUpSkinSel);
	m_pkSkillList->GetScrollbar()->SetScrollButtonDownSkins(pkDownSkinNorm, pkDownSkinSel);

	// Set textures attack listbox
	m_pkAttackDefList->SetSkin( new ZGuiSkin());			
	m_pkAttackDefList->GetSkin()->m_iBkTexID = 
		m_pkTexMan->Load("/data/textures/gui/stone.bmp", 0);
	m_pkAttackDefList->GetSkin()->m_bTileBkSkin = true;

	m_pkAttackDefList->SetItemNormalSkin(pkItemUpSkin); 
	m_pkAttackDefList->SetItemHighLightSkin(pkItemFocusSkin); 
	m_pkAttackDefList->SetItemSelectedSkin(pkItemDownSkin); 
	m_pkAttackDefList->GetScrollbar()->SetScrollInfo(0,100,0.15f,0);

	m_pkAttackDefList->GetScrollbar()->SetScrollButtonUpSkins(pkUpSkinNorm, pkUpSkinSel);
	m_pkAttackDefList->GetScrollbar()->SetScrollButtonDownSkins(pkDownSkinNorm, pkDownSkinSel);
}

bool StatsDlg::IsVisible()
{
	if(m_pkDialog == NULL)
		return false;

	return m_pkDialog->IsVisible(); 
}

void StatsDlg::OnCommand(ZGuiWnd* pkWndClicked)
{

}

void StatsDlg::ToogleOpen()
{
	if(m_pkCharProperty)
	{
		m_pkCharProperty->RequestUpdateFromServer("data");
		m_pkCharProperty->RequestUpdateFromServer("attributes");
		m_pkCharProperty->RequestUpdateFromServer("skills");
	}

	if(m_pkDialog == NULL)
	{
		 m_pkDialog = m_pkApp->GetWnd("StatsWnd");
		 Init();
	}

	if(m_pkDialog->IsVisible())
	{		
		ZGuiSkin* pkSkin = m_pkApp->GetWnd("CharacterPortrait")->GetSkin() ;

		if(pkSkin == NULL)
		{
			pkSkin = new ZGuiSkin();
			pkSkin->m_iBkTexID = m_pkTexMan->Load("/data/textures/gui/portraits/gubbe1.bmp", 0);
			m_pkApp->GetWnd("CharacterPortrait")->SetSkin(pkSkin);
		}

		m_pkSkillList->RemoveAllItems();
		m_pkAttackDefList->RemoveAllItems();

	}

}

void StatsDlg::SetCharacterProperty(CharacterProperty* pkCharProp)
{
	m_pkCharProperty = pkCharProp;
}










