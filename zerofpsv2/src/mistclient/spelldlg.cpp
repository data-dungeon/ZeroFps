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
	m_pkApp->CreateWnd(Wnd, "SpellBookMainWnd", "MainWnd", "", 400, 0, 400, 484, 0);

	char szName[50];

	char* szLabel[10] =
	{
		"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
	};

	m_pkApp->CreateWnd(Button, "SpellBookPrevPageBn", "SpellBookMainWnd", "", 20, 61, 16, 16, 0);
	((ZGuiButton*)m_pkApp->GetWnd("SpellBookPrevPageBn"))->SetButtonUpSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbook_prevpage_u.bmp", 0),0));
	((ZGuiButton*)m_pkApp->GetWnd("SpellBookPrevPageBn"))->SetButtonHighLightSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbook_prevpage_u.bmp", 0),0));
	((ZGuiButton*)m_pkApp->GetWnd("SpellBookPrevPageBn"))->SetButtonDownSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbook_prevpage_d.bmp", 0),0));

	m_pkApp->CreateWnd(Button, "SpellBookNextPageBn", "SpellBookMainWnd", "", 360, 62, 16, 16, 0);
	((ZGuiButton*)m_pkApp->GetWnd("SpellBookNextPageBn"))->SetButtonUpSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbook_nextpage_u.bmp", 0),0));
	((ZGuiButton*)m_pkApp->GetWnd("SpellBookNextPageBn"))->SetButtonHighLightSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbook_nextpage_u.bmp", 0),0));
	((ZGuiButton*)m_pkApp->GetWnd("SpellBookNextPageBn"))->SetButtonDownSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbook_nextpage_d.bmp", 0),0));

	for(int i=0; i<10; i++)
	{
		sprintf(szName, "Level1SpellBn%i", i);
		m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", szLabel[i], 36+i*32, 62, 32, 16, 0);

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
}

void SpellDlg::OnCommand(ZGuiWnd* pkWndClicked)
{

}
