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

	m_pkApp->CreateWnd(Label, "SpellBookLabel", "SpellBookMainWnd", "Spellbook Level 1", 122, 71, 350, 16, 0);

	for(int i=0; i<10; i++)
	{
		sprintf(szName, "Level1SpellBn%i", i);
		m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", szLabel[i], 48+i*28, 20, 24, 16, 0);

		m_pkLevelPages[i] = m_pkApp->GetWnd(szName);

		((ZGuiButton*)m_pkApp->GetWnd(szName))->SetButtonUpSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_u.bmp", 0),0));
		((ZGuiButton*)m_pkApp->GetWnd(szName))->SetButtonHighLightSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_u.bmp", 0),0));
		((ZGuiButton*)m_pkApp->GetWnd(szName))->SetButtonDownSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_d.bmp", 0),0));
	}

	m_pkDialog = m_pkApp->GetWnd("SpellBookMainWnd");

	m_pkDialog->SetSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbookwnd.bmp", 0),0));

	m_pkDialog->Hide();
}

void SpellDlg::OnCommand(ZGuiWnd* pkWndClicked)
{
	printf("pkWndClicked = %s\n", pkWndClicked->GetName());

	for(int i=0; i<10; i++)
	{
		if(pkWndClicked == m_pkLevelPages[i])
		{
			char szLabel[25];
			sprintf(szLabel, "Spellbook Level %i", i);
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
