// skilldlg.cpp: implementation of the SkillDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "skilldlg.h"
#include "quickboard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkillDlg::SkillDlg(ZGuiApp* pkApp, QuickBoard* pkQuickBoard)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkQuickBoard = pkQuickBoard;

	m_iTopRow = 0;
	m_pkApp = pkApp;
	Init();
}

SkillDlg::~SkillDlg()
{

}

void SkillDlg::Update()
{
	if(m_pkCharProperty)
	{
		CharacterStats* pkCharStats = m_pkCharProperty->GetCharStats();
		
		map<string,StatDescriber> kSkillList;
		pkCharStats->GetSkills(kSkillList); 

		map<string,StatDescriber>::iterator itSkill = kSkillList.begin();

		for( ; itSkill != kSkillList.end(); itSkill++)
		{
			AddSlot(itSkill->first);
		}

	}
}

void SkillDlg::Init()
{
	int screen_w = m_pkApp->GetWidth(); 
	int screen_h = m_pkApp->GetHeight();

	//
	// Create main window
	//

	m_pkApp->CreateWnd(Wnd, "SelectSkillsMainWnd", "MainWnd", "", screen_w-336, screen_h-400, 336, 64*5+20, 0);
	m_pkDialog = m_pkApp->GetWnd("SelectSkillsMainWnd");
	m_pkDialog->SetSkin(new ZGuiSkin());
	m_pkDialog->GetSkin()->m_bTransparent = true;

	//
	// Create scrollbar
	//

	m_pkApp->CreateWnd(Scrollbar, "SelectSkillsScrollbar", "SelectSkillsMainWnd", "", 320, 20, 16, 320, 0);
	((ZGuiScrollbar*)m_pkApp->GetWnd("SelectSkillsScrollbar"))->SetScrollInfo(0,100,0.10f,0);

	//
	// Create label
	//

	m_pkApp->CreateWnd(Label, "SelectSkillsLabel", "SelectSkillsMainWnd", "", 0, 0, 320, 20, 0);
	m_pkApp->GetWnd("SelectSkillsLabel")->SetText("Select skill: ");

	//
	// Create skillbuttons
	//

	ZGuiSkin* pkButtonSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);
	pkButtonSkinUp->m_bTileBkSkin = true;

	ZGuiSkin* pkButtonSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);
	pkButtonSkinDown->m_bTileBkSkin = true;

	m_rcClipperArea = Rect(
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

			pkButton->SetClipperArea(m_rcClipperArea); 
			pkButton->m_bUseClipper = true; 

			m_pkSkillButtons[y][x] = pkButton;
		}	

	//m_pkApp->CreateWnd(Label, "ApaTest", "SelectSkillsMainWnd", "", 0, 20, 64, 64, 0);
	

	m_pkDialog->Hide();
}

void SkillDlg::OnCommand(ZGuiWnd* pkWndClicked)
{
	if(!pkWndClicked->IsVisible())
		return;

	ZGuiButton* pkClickButton = NULL;
	
	for(int y=0; y<SKILL_ROWS; y++)
		for(int x=0; x<SKILL_COLS; x++)
		{
			if(pkWndClicked == m_pkSkillButtons[y][x])
			{		
				Rect rc = pkWndClicked->GetScreenRect(); 
				SkillDlg::SkillSlot* pkSkill = GetSkillFromCursorPos(rc.Left+8, rc.Top+8);
				m_pkQuickBoard->AddSlot( QuickBoard::Skill, pkSkill->strName.c_str() );
				ToogleOpen();
				break;
			}
		}
}

void SkillDlg::ToogleOpen()
{
	if(!m_pkDialog->IsVisible())
	{
		m_pkCharProperty->RequestUpdateFromServer("skills"); 

		m_pkAudioSys->StartSound( "/data/sound/open_window.wav",
				m_pkAudioSys->GetListnerPos(),Vector3(0,0,0),false);
		m_pkDialog->Show();
		m_pkDialog->SetFocus();
		m_pkGui->SetFocus( m_pkDialog);
	}
	else
	{
		m_pkAudioSys->StartSound( "/data/sound/close_window.wav",
				m_pkAudioSys->GetListnerPos(),Vector3(0,0,0),false);
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

void SkillDlg::SetCharacterProperty(CharacterProperty* pkCharProp)
{
	m_pkCharProperty = pkCharProp;
}

bool SkillDlg::IsVisible()
{
	return m_pkDialog->IsVisible(); 
}

void SkillDlg::AddSlot(string strName)
{
	// returna om den redan fanns
	if(m_vkSkillSlots.find(strName) != m_vkSkillSlots.end())
		return;

	char szIconName[100], szLabelName[50];
	sprintf(szLabelName, "%sSkillLabel", strName.c_str());
	sprintf(szIconName, "data/textures/gui/skills/%s.bmp", strName.c_str());

	int x, y;
	GetFreeSlotPos(x,y);

	m_pkApp->CreateWnd(Label, szLabelName, "SelectSkillsMainWnd", "", x, 20, 64, 64, 0);

	ZGuiLabel* pkLabel = (ZGuiLabel*) m_pkApp->GetWnd(szLabelName);

	pkLabel->SetSkin(new ZGuiSkin(m_pkTexMan->Load(szIconName, 0),0));
	pkLabel->SetClipperArea(m_rcClipperArea); 
	pkLabel->m_bUseClipper = true; 
	pkLabel->GetSkin()->m_bTileBkSkin = true; 

	SkillSlot* pkNewSlot = new SkillSlot();
	pkNewSlot->pkLabel = pkLabel;
	pkNewSlot->strName = strName;

	m_vkSkillSlots.insert(map<string, SkillSlot*>::value_type(strName, pkNewSlot)); 

	printf("Adding slot \"%s\" to skill dialog\n", strName.c_str());
}

void SkillDlg::GetFreeSlotPos(int &x, int &y)
{
	int antal = m_vkSkillSlots.size();

	y = antal / SKILL_COLS;
	x = antal-(y*SKILL_COLS);

	x *= 64;
	y *= 64;

	y+=20;
}

SkillDlg::SkillSlot* SkillDlg::GetSkillFromCursorPos(int x, int y)
{
	map<string, SkillSlot*>::iterator itSkills = m_vkSkillSlots.begin();

	for( ; itSkills != m_vkSkillSlots.end(); itSkills++)
	{
		ZGuiLabel* pkLabel = itSkills->second->pkLabel;
		Rect rc = pkLabel->GetScreenRect();
		
		if(x >= rc.Left && x <= rc.Right && y >= rc.Top && y <= rc.Bottom)
			return itSkills->second;
	}

	return NULL;
}
