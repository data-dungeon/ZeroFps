// skilldlg.cpp: implementation of the SkillDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "skilldlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkillDlg::SkillDlg(ZGuiApp* pkApp)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));

	m_pkApp = pkApp;
	Init();
}

SkillDlg::~SkillDlg()
{

}

void SkillDlg::Update()
{
	
}

void SkillDlg::Init()
{
	int screen_w = m_pkApp->GetWidth(); 
	int screen_h = m_pkApp->GetHeight();

	//
	// Create main window
	//

	m_pkApp->CreateWnd(Wnd, "SelectSkillsMainWnd", "MainWnd", "", screen_w-336, screen_h-400, 336, 320, 0);
	m_pkDialog = m_pkApp->GetWnd("SelectSkillsMainWnd");
	m_pkDialog->SetSkin(new ZGuiSkin());
	m_pkDialog->GetSkin()->m_bTransparent = true;

	m_pkApp->CreateWnd(Scrollbar, "SelectSkillsScrollbar", "SelectSkillsMainWnd", "", 320, 20, 16, 320, 0);
	((ZGuiScrollbar*)m_pkApp->GetWnd("SelectSkillsScrollbar"))->SetScrollInfo(0,100,0.15f,0);

	m_pkApp->CreateWnd(Label, "SelectSkillsLabel", "SelectSkillsMainWnd", "", 0, 0, 320, 20, 0);
	m_pkApp->GetWnd("SelectSkillsLabel")->SetText("Select skill: ");

	//
	// Create skillbuttons
	//

	ZGuiSkin* pkButtonSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	ZGuiSkin* pkButtonSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	for(int y=0; y<SKILL_ROWS; y++)
		for(int x=0; x<SKILL_COLS; x++)
		{
			char szName[50];
			sprintf(szName, "asfafffreesdf%i", y*10+x);
			m_pkApp->CreateWnd(Button, szName, "SelectSkillsMainWnd", "", x*64, 20+y*64, 64, 64, 0);

			ZGuiButton* pkButton = (ZGuiButton*)m_pkApp->GetWnd(szName);
			pkButton->SetButtonUpSkin(pkButtonSkinUp);
			pkButton->SetButtonHighLightSkin(pkButtonSkinUp);
			pkButton->SetButtonDownSkin(pkButtonSkinDown);
			pkButton->m_bUseAlhpaTest = false;

			m_pkSkillButtons[y][x] = pkButton;
		}	

	m_pkDialog->Hide();
}

void SkillDlg::OnCommand(ZGuiWnd* pkWndClicked)
{
	for(int y=0; y<SKILL_ROWS; y++)
		for(int x=0; x<SKILL_COLS; x++)
		{
			if(pkWndClicked == m_pkSkillButtons[y][x])
			{
				ToogleOpen();
				break;
			}
		}
}

void SkillDlg::ToogleOpen()
{
	if(!m_pkDialog->IsVisible())
	{
		m_pkAudioSys->StartSound( "/data/sound/open_window.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
		m_pkDialog->Show();
	}
	else
	{
		m_pkAudioSys->StartSound( "/data/sound/close_window.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
		m_pkDialog->Hide();
	}
}