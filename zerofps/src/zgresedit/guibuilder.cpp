// guibuilder.cpp: implementation of the GuiBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "guibuilder.h"
#include "resource_id.h"
#include "../zerofps/engine/zgui/zguiresourcemanager.h"
#include "../zerofps/render/texturemanager.h"
#include "../zerofps/basic/zfassert.h"
#include "../zerofps/engine/input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuiBuilder::GuiBuilder(ZGui* pkGui, TextureManager* pkTexMan, 
					   ZGuiResourceManager* pkGuiMan, Rect rcScreen)
{
	m_pkGui = pkGui;
	m_pkTexMan = pkTexMan;
	m_pkGuiMan = pkGuiMan;
	m_iLastID = LAST_ID;
	m_pkLastWndCreated = NULL;
	m_iNewCtrPosX = 10;
	m_iNewCtrPosY = 10;
	m_rcScreen = rcScreen;
}

GuiBuilder::~GuiBuilder()
{

}

ZGuiButton* GuiBuilder::CreateOKButton(ZGuiWnd* pkParent, int iID, int x, int y)
{
	ZFAssert(pkParent, "Can´t create a OK-button with no parent!");

	int h = 20, w = 40;

	if(x == -1)
		x = pkParent->GetScreenRect().Width() - w;
	if(y == -1)
		y = pkParent->GetScreenRect().Height() - h;
	
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetText("OK");
	pkButton->SetWindowFlag(WF_CENTER_TEXT);

	// Bind escape key to parent
	m_pkGui->AddKeyCommand(KEY_RETURN, pkParent, pkButton);

	return pkButton;
}

ZGuiButton* GuiBuilder::CreateCancelButton(ZGuiWnd* pkParent, int iID, bool bOKButtonExist)
{
	ZFAssert(pkParent, "Can´t create a Cancel-button with no parent!");

	int h = 20, w = 60;

	int x = pkParent->GetScreenRect().Width() - w;

	if(bOKButtonExist)
		x -= 40;

	int y = pkParent->GetScreenRect().Height() - h;
	
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetText("Cancel");
	pkButton->SetWindowFlag(WF_CENTER_TEXT);

	return pkButton;
}

ZGuiButton* GuiBuilder::CreateCloseButton(ZGuiWnd* pkParent, int iID)
{
	ZFAssert(pkParent, "Can´t create a closebutton with no parent!");

	int x = pkParent->GetScreenRect().Width() - 16, y = 0, h = 16, w = 16;

	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetText("x");
	pkButton->SetWindowFlag(WF_CENTER_TEXT);

	// Bind escape key to parent
	m_pkGui->AddKeyCommand(KEY_ESCAPE, pkParent, pkButton);

	return pkButton;
}

ZGuiButton* GuiBuilder::CreateButton(ZGuiWnd* pkParent, int iID, int x, int y, 
									 int w, int h, char *pkName)
{
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetText(pkName);

	return pkButton;
}

ZGuiCheckbox* GuiBuilder::CreateCheckbox(ZGuiWnd* pkParent, int iID, int x, int y, 
										 int w, int h, char *pkName)
{
	ZGuiCheckbox* pkButton = new ZGuiCheckbox(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonCheckedSkin(GetSkin("cbn_down"),-1);
	pkButton->SetButtonUncheckedSkin(GetSkin("cbn_up"),-1);
	pkButton->SetText(pkName);

	return pkButton;
}

ZGuiScrollbar* GuiBuilder::CreateScrollbar(ZGuiWnd* pkParent, int iID, int x, int y, 
										   int w, int h)
{
	ZGuiScrollbar* pkScrollbar = new ZGuiScrollbar(Rect(x,y,x+w,y+h), pkParent, 
		true, iID);
	pkScrollbar->SetThumbButtonSkins(GetSkin("bn_up"),GetSkin("bn_up"));
	pkScrollbar->SetSkin(GetSkin("white"));
	
	return pkScrollbar;
}

ZGuiCombobox* GuiBuilder::CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, 
										 int w, int h, bool bMenu)
{
	ZGuiCombobox* pkCombobox = new ZGuiCombobox(Rect(x,y,x+w,y+h), pkParent, 
		true, iID, 20, GetSkin("menu_item"), GetSkin("dark_blue"), 
		GetSkin("dark_blue"), GetSkin("white"), -1);

	if(bMenu == false)
	{
		pkCombobox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
			GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	}
	return pkCombobox;
}

ZGuiListbox* GuiBuilder::CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y,
									   int w, int h)
{
	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,iID,20,
		GetSkin("menu_item"), GetSkin("menu_item_sel"), GetSkin("menu_item_hl"));
	pkListbox->SetSkin( GetSkin("dark_blue") );
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	return pkListbox;
}

ZGuiRadiobutton* GuiBuilder::CreateRadioButton(ZGuiWnd* pkParent, int iID, 
											  int GroupID, int x, int y, 
											  int w, int h, char *szName)
{
	static int iPrevGroupID = -3231963;
	static ZGuiRadiobutton* pkPrev = NULL;

	if(GroupID != iPrevGroupID)
	{
		pkPrev = NULL;
		iPrevGroupID = GroupID;
	}

	int rbn_a = m_pkTexMan->Load("file:../data/textures/radiobn_a.bmp", 0);

	ZGuiRadiobutton* pkRadioButton = new ZGuiRadiobutton(Rect(x,y,x+w,y+h),
		pkParent, iID, GroupID, pkPrev, true);
	pkRadioButton->SetButtonUnselectedSkin(GetSkin("rbn_down"),rbn_a);
	pkRadioButton->SetButtonSelectedSkin(GetSkin("rbn_up"),rbn_a);
	pkRadioButton->SetText(szName);
	pkPrev = pkRadioButton;

	return pkRadioButton;
}

ZGuiWnd* GuiBuilder::CreateMainWindow(int iMainWndID, int iID, int x, int y, 
									  int w, int h, ZGuiWndProc oWndProc,
									  char* szSkin)
{
	ZGuiWnd* pkWnd = new ZGuiWnd(Rect(x,y,x+w,y+h), NULL, true, iID);
	pkWnd->SetSkin(GetSkin(szSkin));
	
	m_pkGui->AddMainWindow(iMainWndID, pkWnd, oWndProc, true);
	return pkWnd;
}

ZGuiLabel* GuiBuilder::CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, 
								  int w, int h, char* strText, char* szSkin)
{
	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h), pkParent, true, iID);
	
	if(strText)
		pkLabel->SetText(strText);

	if(strlen(szSkin) > 1)
		pkLabel->SetSkin(GetSkin(szSkin));

	pkLabel->Enable();
	pkLabel->SetWindowFlag(WF_CANHAVEFOCUS); 

	return pkLabel;
}

ZGuiTextbox* GuiBuilder::CreateTextbox(ZGuiWnd* pkParent, int iID, int x, 
									   int y, int w, int h, bool bMulitLine, 
									   char* szSkin)
{
	ZGuiTextbox* pkTextbox = new ZGuiTextbox(Rect(x,y,x+w,y+h), pkParent, true, iID, 
		bMulitLine);
	pkTextbox->SetSkin(GetSkin(szSkin));
	pkTextbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	return pkTextbox;
}

bool GuiBuilder::InitSkins()
{
	int piss = m_pkTexMan->Load("file:../data/textures/piss.bmp", 0);
	int bn_up = m_pkTexMan->Load("file:../data/textures/button_up.bmp", 0);
	int bn_down = m_pkTexMan->Load("file:../data/textures/button_down.bmp", 0);
	int bn_focus = m_pkTexMan->Load("file:../data/textures/button_focus.bmp", 0);
	int rbn_up = m_pkTexMan->Load("file:../data/textures/radiobn_up.bmp", 0);
	int rbn_down = m_pkTexMan->Load("file:../data/textures/radiobn_down.bmp", 0);
	int bk1 = m_pkTexMan->Load("file:../data/textures/bk1.bmp", 0);
	int bd1 = m_pkTexMan->Load("file:../data/textures/border_horz.bmp", 0);
	int bd2 = m_pkTexMan->Load("file:../data/textures/border_vert.bmp", 0);
	int bd3 = m_pkTexMan->Load("file:../data/textures/border_corner.bmp", 0);
	int bda = m_pkTexMan->Load("file:../data/textures/border_corner_a.bmp", 0);
	int cbn_up = m_pkTexMan->Load("file:../data/textures/checkbox_off.bmp", 0);
	int cbn_down = m_pkTexMan->Load("file:../data/textures/checkbox_on.bmp", 0);

	m_kTextureMap.insert( map<string, int>::value_type(string("border_corner_a"), 
		bda)); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("main"), 
		new ZGuiSkin(bk1, bd1, bd2, bd3, 255, 255, 255, 255, 255, 255, 8, true))); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("blue"), 
		new ZGuiSkin(-1, -1, -1, -1, 0, 0, 255, 0, 0, 128, 4)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("lblue"), 
		new ZGuiSkin(-1, -1, -1, -1, 64, 64, 255, 0, 0, 192, 4)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("red"), 
		new ZGuiSkin(-1, -1, -1, -1, 255, 0, 0, 128, 0, 0, 4)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("bn_up"), 
		new ZGuiSkin(bn_up, -1, -1, -1) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("bn_down"), 
		new ZGuiSkin(bn_down, -1, -1, -1) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("bn_focus"), 
		new ZGuiSkin(bn_focus, -1, -1, -1) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu_item"), 
		new ZGuiSkin(bn_up, -1, -1, -1) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu_item_hl"), 
		new ZGuiSkin(bn_up, -1, -1, -1, 255, 0, 0) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu_item_sel"), 
		new ZGuiSkin(bn_up, -1, -1, -1, 128, 0, 0) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("gray"), 
		new ZGuiSkin(-1, -1, -1, -1, 192, 192, 192, 128, 128, 128, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("dark_blue"), 
		new ZGuiSkin(-1, -1, -1, -1, 0, 0, 128, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("white"), 
		new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 0, 0, 0, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("titlebar"), 
		new ZGuiSkin(-1, -1, -1, -1, 64, 64, 128, 0, 0, 0, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("rbn_down"), 
		new ZGuiSkin(rbn_up, -1, -1, -1, 255, 255, 255, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("rbn_up"), 
		new ZGuiSkin(rbn_down, -1, -1, -1, 255, 255, 255, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("cbn_down"), 
		new ZGuiSkin(cbn_up, -1, -1, -1, 255, 255, 255, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("cbn_up"), 
		new ZGuiSkin(cbn_down, -1, -1, -1, 255, 255, 255, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("PreviewSkin"), 
		new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 0, 0, 0, 4)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("BlackFrame"), 
		new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 0, 0, 0, 2)) ); 

	return true;
}

bool GuiBuilder::CreateNewType(CtrlType eType, ZGuiWndProc oWndProc)
{
	ZGuiWnd* pkParent = m_pkLastWndCreated;

	if(pkParent == NULL && eType != WINDOW)
		return false;

	switch(eType)
	{
	case WINDOW:
		ZGuiWnd* pkNewWnd;
		pkNewWnd = CreateMainWindow(m_iLastID++,m_iLastID++,0,0,300,300,oWndProc);
		m_pkLastWndCreated = pkNewWnd;
		break;
	case LABEL:
		ZGuiLabel* pkLabel;
		pkLabel = CreateLabel(pkParent,m_iLastID++,m_iNewCtrPosX,
			m_iNewCtrPosY, 100,20,"Label");
		m_pkGui->RegisterWindow(pkLabel);
		break;
	case TEXTBOX:
		ZGuiTextbox* pkTextbox;
		pkTextbox = CreateTextbox(pkParent,m_iLastID++,m_iNewCtrPosX,
			m_iNewCtrPosY, 50,20,false);	
		pkTextbox->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkTextbox);
		break;
	case BUTTON:
		ZGuiButton* pkButton;
		pkButton = CreateButton(pkParent,m_iLastID++,m_iNewCtrPosX,
			m_iNewCtrPosY, 50,20,"Button");	
		pkButton->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkButton);
		break;
	case CHECKBOX:
		ZGuiCheckbox* pkCheckbox;
		pkCheckbox = CreateCheckbox(pkParent,m_iLastID++,m_iNewCtrPosX,
			m_iNewCtrPosY, 16,16,"Check Box");	
		pkCheckbox->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkCheckbox);
		m_iNewCtrPosY += 20;
		break;
	case RADIOBUTTON:
		ZGuiRadiobutton* pkRadiobutton;
		pkRadiobutton = CreateRadioButton(pkParent,m_iLastID++,4856,
			m_iNewCtrPosX, m_iNewCtrPosY,16,16,
			"Radio Button");	
		pkRadiobutton->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkRadiobutton);
		m_iNewCtrPosY += 16;
		break;
	case LISTBOX:
		ZGuiListbox* pkListbox;
		pkListbox = CreateListbox(pkParent, m_iLastID++, m_iNewCtrPosX, 
			m_iNewCtrPosY, 100, 100);
		pkListbox->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkListbox);
		break;
	case COMBOBOX:
		ZGuiCombobox* pkCombobox;
		pkCombobox = CreateCombobox(pkParent, m_iLastID++, m_iNewCtrPosX, 
			m_iNewCtrPosY, 100, 100, false);
		pkCombobox->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkCombobox);
		break;
	case SCROLLBAR:
		ZGuiScrollbar* pkScrollbar;
		pkScrollbar = CreateScrollbar(pkParent, m_iLastID++, m_iNewCtrPosX, 
			m_iNewCtrPosY, 20, 100);
		pkScrollbar->SetWindowFlag(WF_CANHAVEFOCUS);
		m_pkGui->RegisterWindow(pkScrollbar);
		break;
	}
	m_kSelectedCtrType = eType;

	return true;
}

bool GuiBuilder::Register(ZGuiWnd *pkWnd, char* strName)
{
	return (m_pkGuiMan->Add(string(strName), pkWnd) != NULL);
}

bool GuiBuilder::Register(ZGuiSkin *pkSkin, char* strName)
{
	return (m_pkGuiMan->Add(string(strName), pkSkin) != NULL);
}

bool GuiBuilder::Register(ZGuiFont *pkFont, char* strName)
{
	return (m_pkGuiMan->Add(string(strName), pkFont) != NULL);
}

ZGuiWnd* GuiBuilder::GetWnd(char* strName)
{
	return m_pkGuiMan->Wnd(string(strName));
}

ZGuiSkin* GuiBuilder::GetSkin(char* strName)
{
	static ZGuiSkin* fail_skin;
	static bool init = false;

	if(init == false)
	{
		int tex = m_pkTexMan->Load("file:../data/textures/notex.bmp", 0);
		fail_skin = new ZGuiSkin(tex, -1, -1, -1, 255, 255, 255, 255, 255, 255);
		init = true;
	}

	map<string, ZGuiSkin*>::iterator ret = m_kSkinMap.find(string(strName));

	if(ret != m_kSkinMap.end())
		return ret->second;
	
	return fail_skin;
}

int GuiBuilder::GetTexture(char* strName)
{
	static int fail_texture;
	static bool init = false;

	if(init == false)
	{
		fail_texture = m_pkTexMan->Load("file:../data/textures/notex.bmp", 0);
		init = true;
	}

	map<string, int>::iterator ret = m_kTextureMap.find(string(strName));

	if(ret != m_kTextureMap.end())
		return ret->second;
	
	return fail_texture;
}










