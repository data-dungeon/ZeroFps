// quickboard.cpp: implementation of the QuickBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "quickboard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QuickBoard::QuickBoard(ZGuiApp* pkApp)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	m_pkApp = pkApp;
	Init();
}

QuickBoard::~QuickBoard()
{
	for(int i=0; i<MAX_NUM_QUICK_ITEMS; i++)
	{
		delete m_vkQuickItems[i];
	}
}

void QuickBoard::Init()
{
	int screen_w = m_pkApp->GetWidth();
	int screen_h = m_pkApp->GetHeight();

	m_pkApp->CreateWnd(Wnd, "QuickItemMainWnd", "MainWnd", "", 0, screen_h-155, 272, 48, 0);

	m_pkDialog = m_pkApp->GetWnd("QuickItemMainWnd");

	m_pkDialog->m_bUseAlhpaTest = false;

	m_pkDialog->SetSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/action_wnd.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/action_wnd_a.bmp", 0),0));

	ZGuiSkin* pkButtonSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	ZGuiSkin* pkButtonSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	for(int i=0; i<7; i++)
	{
		char szName[50];

		QuickItem* pkNewButton = new QuickItem;

		sprintf(szName, "QuickActionLabel%i", i);
		m_pkApp->CreateWnd(Label, szName, "QuickItemMainWnd", "", 12+36*i, 12, 32, 32, 0);
		pkNewButton->pkLabel = (ZGuiLabel*)m_pkApp->GetWnd(szName);
		pkNewButton->pkLabel->m_bUseAlhpaTest = false;

		pkNewButton->pkLabel->SetSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/items/spellbook.bmp", 0),
			m_pkTexMan->Load("/data/textures/gui/items/spellbook_a.bmp", 0),0));

		pkNewButton->pkLabel->Hide();

		sprintf(szName, "QuickActionButton%i", i);
		m_pkApp->CreateWnd(Button, szName, "QuickItemMainWnd", "", 12+36*i, 12, 32, 32, 0);
		pkNewButton->pkButton = (ZGuiButton*)m_pkApp->GetWnd(szName);
		pkNewButton->pkButton->m_bUseAlhpaTest = false;

		pkNewButton->pkButton->SetButtonUpSkin(pkButtonSkinUp);
		pkNewButton->pkButton->SetButtonHighLightSkin(pkButtonSkinUp);
		pkNewButton->pkButton->SetButtonDownSkin(pkButtonSkinDown);
		pkNewButton->pkButton->Disable(); // disable
		
		pkNewButton->iPos = i;
		
		m_vkQuickItems.push_back(pkNewButton);
	}
}

void QuickBoard::AddQuickItem(char *szItemName)
{
	int index = -1;
	for(int i=0; i<MAX_NUM_QUICK_ITEMS; i++)
		if(!m_vkQuickItems[i]->pkLabel->IsVisible())
		{
			index = i;
			break;
		}

	if(index == -1)
		return; // fix later..

	m_vkQuickItems[index]->pkLabel->Show();
	m_vkQuickItems[index]->pkButton->Enable(); // enable
				
	ZGuiSkin* pkLabelSkin = m_vkQuickItems[index]->pkLabel->GetSkin();

	pkLabelSkin->m_iBkTexID = m_pkTexMan->Load("/data/textures/gui/items/meat.bmp", 0);
	pkLabelSkin->m_iBkTexAlphaID = m_pkTexMan->Load("/data/textures/gui/items/meat_a.bmp", 0);
}

void QuickBoard::OnCommand(ZGuiWnd* pkWndClicked)
{

}

void QuickBoard::Update()
{

}
