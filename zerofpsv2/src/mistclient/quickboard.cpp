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
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));

	m_pkApp = pkApp;
	Init();
}

QuickBoard::~QuickBoard()
{
	for(int i=0; i<MAX_NUM_QUICK_ITEMS; i++)
	{
		delete m_vkQuickSlots[i];
	}
}

void QuickBoard::Init()
{
	int screen_w = m_pkApp->GetWidth();
	int screen_h = m_pkApp->GetHeight();

	m_pkApp->CreateWnd(Wnd, "QuickSlotMainWnd", "PanelBkWnd", "", 0, -46, 272, 48, 0);

	m_pkDialog = m_pkApp->GetWnd("QuickSlotMainWnd");

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

	for(int i=0; i<MAX_NUM_QUICK_ITEMS; i++)
	{
		char szName[50];

		QuickSlot* pkNewButton = new QuickSlot;

		sprintf(szName, "QuickActionLabel%i", i);
		m_pkApp->CreateWnd(Label, szName, "QuickSlotMainWnd", "", 12+36*i, 12, 32, 32, 0);
		pkNewButton->pkLabel = (ZGuiLabel*)m_pkApp->GetWnd(szName);
		pkNewButton->pkLabel->m_bUseAlhpaTest = false;
		pkNewButton->pkLabel->SetSkin( new ZGuiSkin() );

		pkNewButton->pkLabel->Hide();

		sprintf(szName, "QuickActionButton%i", i);
		m_pkApp->CreateWnd(Button, szName, "QuickSlotMainWnd", "", 12+36*i, 12, 32, 32, 0);
		pkNewButton->pkButton = (ZGuiButton*)m_pkApp->GetWnd(szName);
		pkNewButton->pkButton->m_bUseAlhpaTest = false;

		pkNewButton->pkButton->SetButtonUpSkin(pkButtonSkinUp);
		pkNewButton->pkButton->SetButtonHighLightSkin(pkButtonSkinUp);
		pkNewButton->pkButton->SetButtonDownSkin(pkButtonSkinDown);
		pkNewButton->pkButton->Disable(); // disable
		
		pkNewButton->iPos = i;

		pkNewButton->pkButton->m_bAcceptRightClicks = true;
		
		m_vkQuickSlots.push_back(pkNewButton);
	}
}

void QuickBoard::AddSlot(char *szIcon, char* szIconAlpha)
{
	int index = -1;
	for(int i=0; i<MAX_NUM_QUICK_ITEMS; i++)
		if(!m_vkQuickSlots[i]->pkLabel->IsVisible())
		{
			index = i;
			break;
		}

	if(index == -1)
		return; // fix later..

	m_vkQuickSlots[index]->pkLabel->Show();
	m_vkQuickSlots[index]->pkButton->Enable(); // enable
				
	ZGuiSkin* pkLabelSkin = m_vkQuickSlots[index]->pkLabel->GetSkin();

	if(szIcon)
		pkLabelSkin->m_iBkTexID = m_pkTexMan->Load(szIcon, 0);

	if(szIconAlpha)
		pkLabelSkin->m_iBkTexAlphaID = m_pkTexMan->Load(szIconAlpha, 0);
	else
		pkLabelSkin->m_iBkTexAlphaID = -1;
}

void QuickBoard::OnCommand(ZGuiWnd* pkWndClicked, bool bRightMBnClicked)
{
	for(int i=0; i<MAX_NUM_QUICK_ITEMS; i++)
	{
		if(m_vkQuickSlots[i]->pkButton == pkWndClicked)
		{
			if(!bRightMBnClicked)
			{
				// Left button pressed (normal click)
			}
			else
			{
				// Right button pressed
				RemoveSlot(i);

				m_pkAudioSys->StartSound( "/data/sound/click1.wav",
						m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
			}
	
			break;
		}	
	}
}

void QuickBoard::Update()
{

}

void QuickBoard::RemoveSlot(int iIndex)
{
	if(iIndex >= 0 && iIndex < MAX_NUM_QUICK_ITEMS)
	{
		ZGuiSkin* pkSkin = m_vkQuickSlots[iIndex]->pkLabel->GetSkin();
		pkSkin->m_iBkTexID = -1;
		pkSkin->m_iBkTexAlphaID = -1;

		m_vkQuickSlots[iIndex]->pkLabel->Hide();
		m_vkQuickSlots[iIndex]->pkButton->Disable(); // disable
	}
}
