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
		
		map<string,string> kDataList;
		pkCharStats->GetData(kDataList); 

		map<string,string>::iterator itData = kDataList.begin();

		for( ; itData != kDataList.end(); itData++)
		{
			printf("itData %s = %s\n", itData->first, itData->second);
		}
	}
}

void StatsDlg::Init()
{
	int screen_w = m_pkApp->GetWidth(); 
	int screen_h = m_pkApp->GetHeight();

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

	m_pkAttackDefList->SetSkin( new ZGuiSkin());			
	m_pkAttackDefList->GetSkin()->m_iBkTexID = 
		m_pkTexMan->Load("/data/textures/gui/stone.bmp", 0);
	m_pkAttackDefList->GetSkin()->m_bTileBkSkin = true;

	m_pkAttackDefList->SetItemNormalSkin(pkItemUpSkin); 
	m_pkAttackDefList->SetItemHighLightSkin(pkItemFocusSkin); 
	m_pkAttackDefList->SetItemSelectedSkin(pkItemDownSkin); 
	m_pkAttackDefList->GetScrollbar()->SetScrollInfo(0,100,0.15f,0);
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
		m_pkCharProperty->RequestUpdateFromServer("data");

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

		m_pkApp->GetWnd("CharacterAttribSTYLabel")->SetText("10");
		m_pkApp->GetWnd("CharacterAttribCONLabel")->SetText("10");
		m_pkApp->GetWnd("CharacterAttribDEXLabel")->SetText("10");
		m_pkApp->GetWnd("CharacterAttribINTLabel")->SetText("10");
		m_pkApp->GetWnd("CharacterAttribPIELabel")->SetText("10");

		m_pkApp->GetWnd("CharacterCounterHPLabel")->SetText("100");
		m_pkApp->GetWnd("CharacterCounterMPLabel")->SetText("100");

		m_pkSkillList->RemoveAllItems();
		m_pkAttackDefList->RemoveAllItems();

		m_pkSkillList->AddItem("Shild = 10", 0, 0);
		m_pkSkillList->AddItem("Sword = 10", 0, 0);
		m_pkSkillList->AddItem("Jump = 10", 0, 0);

		m_pkAttackDefList->AddItem("Slash mod = 20%", 0, 0); 
	}

}

void StatsDlg::SetCharacterProperty(CharacterProperty* pkCharProp)
{
	m_pkCharProperty = pkCharProp;
}