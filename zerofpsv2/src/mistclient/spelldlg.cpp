// spelldlg.cpp: implementation of the SpellDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "spelldlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpellDlg::SpellDlg(ZGuiApp* pkApp)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	m_pkApp = pkApp;
	Init();
}

SpellDlg::~SpellDlg()
{

}

void SpellDlg::Update()
{

}

void SpellDlg::Init()
{
	int screen_w = m_pkApp->GetWidth(); 
	int screen_h = m_pkApp->GetHeight();

	m_pkApp->CreateWnd(Wnd, "SpellBookMainWnd", "MainWnd", "", screen_w-400, 0, 400, 484, 0);

	char szName[50];

	char* szLabel[10] =
	{
		"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
	};

	ZGuiSkin* pkButtonSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	ZGuiSkin* pkButtonSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	for(int y=0; y<4; y++)
		for(int x=0; x<5; x++)
		{
			char szName[50];
			sprintf(szName, "asfasdf%i", y*10+x);
			m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", "", 32+x*64, 100+y*64, 64, 64, 0);

			ZGuiButton* pkButton = (ZGuiButton*)m_pkApp->GetWnd(szName);
			pkButton->SetButtonUpSkin(pkButtonSkinUp);
			pkButton->SetButtonHighLightSkin(pkButtonSkinUp);
			pkButton->SetButtonDownSkin(pkButtonSkinDown);
			pkButton->m_bUseAlhpaTest = false;

		}

	m_pkApp->CreateWnd(Textbox, "SpellDescTextbox", "SpellBookMainWnd", 
		"", 32, 370, 300, 60, EB_IS_MULTILINE | READ_ONLY);

	m_pkApp->CreateWnd(Label, "SpellBookLabel", "SpellBookMainWnd", "Spellbook School 1", 32, 71, 350, 16, 0);

	ZGuiSkin* pkFireSchoolSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_fire_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_a.bmp", 0),0);

	ZGuiSkin* pkFireSchoolSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_fire_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_a.bmp", 0),0);

	ZGuiSkin* pkNecromancerSchoolSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_a.bmp", 0),0);

	ZGuiSkin* pkNecromancerSchoolSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_a.bmp", 0),0);

	int i;

	for(i=0; i<10; i++)
	{
		sprintf(szName, "SpellSchoolBn%i", i);
		m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", "", 32+i*32, 10, 32, 32, 0);

		ZGuiButton* pkButton = (ZGuiButton*) m_pkApp->GetWnd(szName);
		m_pkSchoolButtons[i] = pkButton;

		pkButton->m_bUseAlhpaTest = true;
		pkButton->SetButtonUpSkin(pkFireSchoolSkinUp);
		pkButton->SetButtonHighLightSkin(pkFireSchoolSkinUp);
		pkButton->SetButtonDownSkin(pkFireSchoolSkinDown);
	}

	m_pkSchoolButtons[0]->SetButtonUpSkin(pkFireSchoolSkinUp);
	m_pkSchoolButtons[0]->SetButtonHighLightSkin(pkFireSchoolSkinUp);
	m_pkSchoolButtons[0]->SetButtonDownSkin(pkFireSchoolSkinDown);

	m_pkSchoolButtons[1]->SetButtonUpSkin(pkNecromancerSchoolSkinUp);
	m_pkSchoolButtons[1]->SetButtonHighLightSkin(pkNecromancerSchoolSkinUp);
	m_pkSchoolButtons[1]->SetButtonDownSkin(pkNecromancerSchoolSkinDown);

	for(i=0; i<10; i++)
	{
		sprintf(szName, "Level1SpellBn%i", i);
		m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", szLabel[i], 32+i*28, 450, 24, 16, 0);

		m_pkPageButton[i] = (ZGuiButton*) m_pkApp->GetWnd(szName);

		m_pkPageButton[i]->SetButtonUpSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_u.bmp", 0),0));
		m_pkPageButton[i]->SetButtonHighLightSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_u.bmp", 0),0));
		m_pkPageButton[i]->SetButtonDownSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_d.bmp", 0),0));
	}

	m_pkDialog = m_pkApp->GetWnd("SpellBookMainWnd");

	m_pkDialog->SetSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbookwnd.bmp", 0),0));

	m_pkDialog->Hide();
}

void SpellDlg::OnCommand(ZGuiWnd* pkWndClicked)
{
	for(int i=0; i<10; i++)
	{
		if(pkWndClicked == m_pkSchoolButtons[i])
		{
			char szLabel[25];
			sprintf(szLabel, "Spellbook School %i", i+1);
			m_pkApp->GetWnd("SpellBookLabel")->SetText(szLabel);
			break;
		}
	}
}

void SpellDlg::ToogleOpen()
{
	if(m_pkDialog->IsVisible())
		m_pkDialog->Hide();
	else
		m_pkDialog->Show();
}
