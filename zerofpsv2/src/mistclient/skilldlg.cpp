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

	m_iTopRow = 0;
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

	m_pkApp->CreateWnd(Wnd, "SelectSkillsMainWnd", "MainWnd", "", screen_w-336, screen_h-400-64, 336, screen_h-(screen_h-400-64), 0);
	m_pkDialog = m_pkApp->GetWnd("SelectSkillsMainWnd");
	m_pkDialog->SetSkin(new ZGuiSkin());
	m_pkDialog->GetSkin()->m_iBkTexAlphaID = m_pkTexMan->Load("/data/textures/gui/white.bmp",0);
	m_pkDialog->GetSkin()->m_iBkTexID = m_pkTexMan->Load("/data/textures/gui/black.bmp",0);

	//
	// Create scrollbar
	//

	m_pkApp->CreateWnd(Scrollbar, "SelectSkillsScrollbar", "SelectSkillsMainWnd", "", 320, 20+64, 16, 320, 0);
	((ZGuiScrollbar*)m_pkApp->GetWnd("SelectSkillsScrollbar"))->SetScrollInfo(0,100,0.10f,0);

	//
	// Create label
	//

	m_pkApp->CreateWnd(Label, "SelectSkillsLabel", "SelectSkillsMainWnd", "", 0, 64, 320, 20, 0);
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

	Rect rcClipperArea = Rect(
		screen_w-336, screen_h-400+20,
		screen_w-336+64*5, screen_h-400+64*5+20);

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

			pkButton->SetClipperArea(rcClipperArea); 
			pkButton->m_bUseClipper = true; 

			m_pkSkillButtons[y][x] = pkButton;
		}	

	m_pkApp->CreateWnd(Label, "ApaTest", "SelectSkillsMainWnd", "", 0, 64+20, 64, 64, 0);
	m_pkApp->GetWnd("ApaTest")->SetSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spells/lightingball.bmp", 0),0));
	m_pkApp->GetWnd("ApaTest")->SetClipperArea(rcClipperArea); 
	m_pkApp->GetWnd("ApaTest")->m_bUseClipper = true; 
	m_pkApp->GetWnd("ApaTest")->GetSkin()->m_bTileBkSkin = true; 

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
		m_pkDialog->SetFocus();
		m_pkGui->SetFocus( m_pkDialog);
	}
	else
	{
		m_pkAudioSys->StartSound( "/data/sound/close_window.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
		m_pkDialog->Hide();
		m_pkGui->SetFocus(m_pkApp->GetWnd("PanelBkWnd") );
	}
}

string SkillDlg::GetWndByID(int iID)
{
	ZGuiWnd* pkParent = m_pkDialog;

	list<ZGuiWnd*> childs;
	pkParent->GetChildrens(childs);

	list<ZGuiWnd*>::iterator itChild = childs.begin() ;

	ZGuiWnd* pkClickWnd = NULL;
	
	for( ; itChild != childs.end(); itChild++)
	{
		if((*itChild)->GetID() == (unsigned int) iID)
		{
			pkClickWnd = *itChild;
			return string(pkClickWnd->GetName());
		}
	}

	return string("");
}

void SkillDlg::OnScroll(int iID, int iPos)
{
	static int prev_pos = 0;
	int offset;

	if(prev_pos == iPos)
		return;
	else
	{
		offset = iPos-prev_pos;
		prev_pos = iPos;
	}

	int screen_h = m_pkApp->GetHeight();
	string strScrollbarName = GetWndByID(iID);

	ZGuiWnd* pkApa = m_pkApp->GetWnd("ApaTest");

	if(strScrollbarName == "SelectSkillsScrollbar")
	{
		for(int y=0; y<SKILL_ROWS; y++)
			for(int x=0; x<SKILL_COLS; x++)
			{
				m_pkSkillButtons[y][x]->Move(0, -offset*8, true, true);

				Rect rcButton = m_pkSkillButtons[y][x]->GetScreenRect();

				if(rcButton.Bottom <= (screen_h-400-64))
				{
					m_pkSkillButtons[y][x]->Move(0, 64*SKILL_ROWS , true, true);
				}

				if(rcButton.Top >= (screen_h-400+64*(SKILL_ROWS-1)))
				{
					m_pkSkillButtons[y][x]->Move(0, -64*SKILL_ROWS, true, true);
				}

				m_pkSkillButtons[y][x]->SetMoveArea(m_pkSkillButtons[y][x]->GetScreenRect(), true); 
			}

		pkApa->Move(0, -offset*8, true, true);
	}
}

