// guibuilder.cpp: implementation of the GuiBuilder class.
//
///////////////////////////////////////////////////////////////////////////////

#include "guibuilder.h"
#include "resource_id.h"
#include "../zerofps/engine/zgui/zguiresourcemanager.h"
#include "../zerofps/render/texturemanager.h"
#include "../zerofps/basic/zfassert.h"
#include "../zerofps/engine/input.h"
#include "../zerofps/basic/zguiskin.h"
#include <typeinfo>

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
GuiBuilder::GuiBuilder(ZGui* pkGui,TextureManager* pkTexMan,
					   ZGuiResourceManager* pkGuiMan,Rect rcScreen)
{
	m_pkGui = pkGui;
	m_pkTexMan = pkTexMan;
	m_pkGuiMan = pkGuiMan;
	m_iLastID = LAST_ID;
	m_iLastRadioGroup = LAST_ID+10000;
	m_iLastTabNr = 0;
	m_rcScreen = rcScreen;
	m_bAllocateNewSkins = false;
}

GuiBuilder::~GuiBuilder()
{

}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateMainWindow
// Description: 
//
ZGuiWnd* GuiBuilder::CreateMainWindow(int iMainWndID,int iID,char* szRegName, 
									  int x,int y, int w,int h,
									  ZGuiWndProc oWndProc,
									  char* szSkin,int TabOrderNr)
{
	if(iID == -1)
	{
		iID = m_iLastID++;
		iMainWndID = m_iLastID++;
	}

	// Nollställ taborder räklnaren.
	m_iLastTabNr = 0;

	ZGuiWnd* pkWnd = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,iID);
	pkWnd->SetSkin(GetSkin(szSkin,m_bAllocateNewSkins));
	pkWnd->SetGUI(m_pkGui);
	
	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkWnd, szName);
	else
		strcpy(szName, szRegName);
	m_pkGui->AddMainWindow(iMainWndID,pkWnd,szName,oWndProc,true);
	return pkWnd;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateOKButton
// Description: 
//
ZGuiButton* GuiBuilder::CreateOKButton(ZGuiWnd* pkParent,int iID,
									   char* szRegName, bool bExistbuttonExist,
									   int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZFAssert(pkParent,"Can´t create a OK-button with no parent!");

	int h = 20,w = 50;
	int x = pkParent->GetScreenRect().Width() - w;
	int y = pkParent->GetScreenRect().Height() - h;

	if(bExistbuttonExist)
		x -= 50;
	
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus",m_bAllocateNewSkins));
	pkButton->SetButtonDownSkin(GetSkin("bn_down",m_bAllocateNewSkins));
	pkButton->SetButtonUpSkin(GetSkin("bn_up",m_bAllocateNewSkins));
	pkButton->SetText("OK");
	pkButton->SetWindowFlag(WF_CENTER_TEXT);
	pkButton->SetGUI(m_pkGui);
	pkButton->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);

	// Bind escape key to parent
	m_pkGui->AddKeyCommand(KEY_RETURN,pkParent,pkButton);

	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkButton, szName);
	else
		strcpy(szName, szRegName);
	Register(pkButton, szName);

	return pkButton;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateCancelButton
// Description: 
//
ZGuiButton* GuiBuilder::CreateCancelButton(ZGuiWnd* pkParent,int iID,
										   char* szRegName, 
										   bool bOKButtonExist, int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZFAssert(pkParent,"Can´t create a Cancel-button with no parent!");

	int h = 20,w = 50;

	int x = pkParent->GetScreenRect().Width() - w;

/*	if(bOKButtonExist)
		x -= 40;*/

	int y = pkParent->GetScreenRect().Height() - h;
	
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus",m_bAllocateNewSkins));
	pkButton->SetButtonDownSkin(GetSkin("bn_down",m_bAllocateNewSkins));
	pkButton->SetButtonUpSkin(GetSkin("bn_up",m_bAllocateNewSkins));
	pkButton->SetText("Cancel");
	pkButton->SetWindowFlag(WF_CENTER_TEXT);
	pkButton->SetGUI(m_pkGui);
	pkButton->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);

	char szName[50];
	if(szRegName == NULL)	
		GenUniqueWndName(pkButton, szName);
	else
		strcpy(szName, szRegName);
	Register(pkButton, szName);

	return pkButton;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateCloseButton
// Description: 
//
ZGuiButton* GuiBuilder::CreateCloseButton(ZGuiWnd* pkParent,int iID,
										  char* szRegName, int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZFAssert(pkParent,"Can´t create a closebutton with no parent!");

	int x = pkParent->GetScreenRect().Width() - 16,y = 0,h = 16,w = 16;

	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus",m_bAllocateNewSkins));
	pkButton->SetButtonDownSkin(GetSkin("bn_down",m_bAllocateNewSkins));
	pkButton->SetButtonUpSkin(GetSkin("bn_up",m_bAllocateNewSkins));
	pkButton->SetText("x");
	pkButton->SetWindowFlag(WF_CENTER_TEXT);
	pkButton->SetGUI(m_pkGui);
	pkButton->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);

	// Bind escape key to parent
	m_pkGui->AddKeyCommand(KEY_ESCAPE,pkParent,pkButton);

	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkButton, szName);
	else
		strcpy(szName, szRegName);
	Register(pkButton, szName);

	return pkButton;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateButton
// Description: 
//
ZGuiButton* GuiBuilder::CreateButton(ZGuiWnd* pkParent,int iID,char* szRegName,
									 int x,int y,int w,int h,char *pkName,
									 int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus",m_bAllocateNewSkins));
	pkButton->SetButtonDownSkin(GetSkin("bn_down",m_bAllocateNewSkins));
	pkButton->SetButtonUpSkin(GetSkin("bn_up",m_bAllocateNewSkins));
	pkButton->SetText(pkName);
	pkButton->SetGUI(m_pkGui);
	pkButton->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);

	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkButton, szName);
	else
		strcpy(szName, szRegName);
	Register(pkButton, szName);

	return pkButton;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateScrollbar
// Description: 
//
ZGuiScrollbar* GuiBuilder::CreateScrollbar(ZGuiWnd* pkParent,int iID,
										   char* szRegName,int x,int y,
										   int w,int h,int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiScrollbar* pkScrollbar = new ZGuiScrollbar(Rect(x,y,x+w,y+h),pkParent,
		true,iID);
	pkScrollbar->SetThumbButtonSkins(GetSkin("bn_up",m_bAllocateNewSkins),
		GetSkin("bn_up",m_bAllocateNewSkins));
	pkScrollbar->SetSkin(GetSkin("white",m_bAllocateNewSkins));
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);
	
	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkScrollbar, szName);
	else
		strcpy(szName, szRegName);
	Register(pkScrollbar, szName);

	return pkScrollbar;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateCombobox
// Description: 
//
ZGuiCombobox* GuiBuilder::CreateCombobox(ZGuiWnd* pkParent,int iID,
										 char* szRegName,int x,int y,int w,
										 int h,bool bMenu,int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiCombobox* pkCombobox = new ZGuiCombobox(Rect(x,y,x+w,y+h),pkParent,
		true,iID,20,GetSkin("white",m_bAllocateNewSkins),
		GetSkin("dark_blue",m_bAllocateNewSkins),
		GetSkin("dark_blue",m_bAllocateNewSkins),
		GetSkin("white",m_bAllocateNewSkins));
	pkCombobox->SetSkin( GetSkin("gray",m_bAllocateNewSkins) );

	if(bMenu == false)
	{
		pkCombobox->SetScrollbarSkin(
			GetSkin("menu_item_sel",m_bAllocateNewSkins),
			GetSkin("menu_item_hl",m_bAllocateNewSkins),
			GetSkin("menu_item_hl",m_bAllocateNewSkins));

		pkCombobox->IsMenu(false);
	}
	else
	{
		pkCombobox->IsMenu(true);
	}

	pkCombobox->SetGUI(m_pkGui);
	pkCombobox->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);
	
	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkCombobox, szName);
	else
		strcpy(szName, szRegName);
	Register(pkCombobox, szName);
	
	return pkCombobox;

}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateListbox
// Description: 
//
ZGuiListbox* GuiBuilder::CreateListbox(ZGuiWnd* pkParent,int iID,
									   char* szRegName,int x,int y,int w,int h,
									   int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,
		iID,20,GetSkin("menu_item",m_bAllocateNewSkins),
		GetSkin("menu_item_sel",m_bAllocateNewSkins),
		GetSkin("menu_item_hl",m_bAllocateNewSkins));
	pkListbox->SetSkin( GetSkin("dark_blue",m_bAllocateNewSkins) );
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel",m_bAllocateNewSkins),
		GetSkin("menu_item_hl",m_bAllocateNewSkins),
		GetSkin("menu_item_hl",m_bAllocateNewSkins));

	pkListbox->SetGUI(m_pkGui);
	pkListbox->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);
	
	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkListbox, szName);
	else
		strcpy(szName, szRegName);
	Register(pkListbox, szName);
	
	return pkListbox;

}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateCheckbox
// Description: 
//
ZGuiCheckbox* GuiBuilder::CreateCheckbox(ZGuiWnd* pkParent,int iID,
										 char* szRegName,int x,int y,int w,
										 int h,char *pkName,bool bCheck,
										 int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiCheckbox* pkButton = new ZGuiCheckbox(Rect(x,y,x+w,y+h),pkParent,true,
		iID);
	pkButton->SetButtonUncheckedSkin(GetSkin("cbn_up",m_bAllocateNewSkins));
	pkButton->SetButtonCheckedSkin(GetSkin("cbn_down",m_bAllocateNewSkins));
	pkButton->SetLabelSkin(GetSkin("Label",m_bAllocateNewSkins));
	pkButton->SetText(pkName);
	pkButton->SetGUI(m_pkGui);
	pkButton->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);
	bCheck == true ? pkButton->CheckButton() : pkButton->UncheckButton();

	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkButton, szName);
	else
		strcpy(szName, szRegName);
	Register(pkButton, szName);

	return pkButton;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateRadioButton
// Description: 
//
ZGuiRadiobutton* GuiBuilder::CreateRadioButton(ZGuiWnd* pkParent,int iID,
											   char* szRegName,int GroupID,
											   bool bCreateNewGroup,
											   int x,int y,int w,int h,
											   char *szRbName,bool bCheck,
											   int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;
	if(GroupID == -1)
	{
		if(bCreateNewGroup)
			m_iLastRadioGroup += 100;
		
		GroupID = m_iLastRadioGroup;
	}

	static int iPrevGroupID = -3231963;
	static ZGuiRadiobutton* pkPrev = NULL;

	if(GroupID != iPrevGroupID)
	{
		pkPrev = NULL;
		iPrevGroupID = GroupID;
	}

	ZGuiRadiobutton* pkRadioButton = new ZGuiRadiobutton(Rect(x,y,x+w,y+h),
		pkParent,iID,GroupID,pkPrev,true);
	pkRadioButton->SetButtonUnselectedSkin(GetSkin("rbn_down",m_bAllocateNewSkins));
	pkRadioButton->SetButtonSelectedSkin(GetSkin("rbn_up",m_bAllocateNewSkins));
	pkRadioButton->GetButton()->SetLabelSkin(GetSkin("Label",m_bAllocateNewSkins));
	pkRadioButton->SetText(szRbName);
	pkRadioButton->SetGUI(m_pkGui);
	pkRadioButton->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : 
		TabOrderNr);

	if(bCheck)
		pkRadioButton->GetButton()->CheckButton();

	pkPrev = pkRadioButton;

	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkRadioButton, szName);
	else
		strcpy(szName, szRegName);
	Register(pkRadioButton, szName);

	return pkRadioButton;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateLabel
// Description: 
//
ZGuiLabel* GuiBuilder::CreateLabel(ZGuiWnd* pkParent,int iID,char* szRegName,
								   int x,int y,int w,int h,char* szText,
								   char* szSkin,int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h),pkParent,true,iID);
	
	if(szText)
		pkLabel->SetText(szText);

	if(strlen(szSkin) > 1)
		pkLabel->SetSkin(GetSkin(szSkin,m_bAllocateNewSkins));

	pkLabel->Enable();
	pkLabel->SetGUI(m_pkGui);
	pkLabel->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);
	
	char szName[50];
	if(szRegName == NULL)
		GenUniqueWndName(pkLabel, szName);
	else
		strcpy(szName, szRegName);
	Register(pkLabel, szName);
	
	return pkLabel;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateTextbox
// Description: 
//
ZGuiTextbox* GuiBuilder::CreateTextbox(ZGuiWnd* pkParent,int iID,
									   char* szRegName,int x,int y,int w,int h,
									   bool bMulitLine,char* szSkin,
									   int TabOrderNr)
{
	if(iID == -1)
		iID = m_iLastID++;

	ZGuiTextbox* pkTextbox = new ZGuiTextbox(Rect(x,y,x+w,y+h),pkParent,
		true,iID,bMulitLine);
	pkTextbox->SetSkin(GetSkin(szSkin,m_bAllocateNewSkins));
	pkTextbox->SetScrollbarSkin(
		GetSkin("menu_item_sel",m_bAllocateNewSkins),
		GetSkin("menu_item_hl",m_bAllocateNewSkins),
		GetSkin("menu_item_hl",m_bAllocateNewSkins));
	pkTextbox->SetGUI(m_pkGui);
	pkTextbox->SetTabOrderNr((TabOrderNr == -1) ? m_iLastTabNr++ : TabOrderNr);
	
	char szName[50];
	if(szRegName == NULL)	
		GenUniqueWndName(pkTextbox, szName);
	else
		strcpy(szName, szRegName);
	Register(pkTextbox, szName);
	
	return pkTextbox;
}

///////////////////////////////////////////////////////////////////////////////
// Name: InitSkins
// Description: 
//
bool GuiBuilder::InitSkins()
{
	int piss = m_pkTexMan->Load("file:../data/textures/piss.bmp",0);
	int bn_up = m_pkTexMan->Load("file:../data/textures/button_up.bmp",0);
	int bn_down = m_pkTexMan->Load("file:../data/textures/button_down.bmp",0);
	int bn_focus = m_pkTexMan->Load("file:../data/textures/button_focus.bmp",0);
	int rbn_up = m_pkTexMan->Load("file:../data/textures/radiobn_up.bmp",0);
	int rbn_down = m_pkTexMan->Load("file:../data/textures/radiobn_down.bmp",0);
	int rbn_a = m_pkTexMan->Load("file:../data/textures/radiobn_a.bmp",0);
	int bk1 = m_pkTexMan->Load("file:../data/textures/bk1.bmp",0);
	int bd1 = m_pkTexMan->Load("file:../data/textures/border_horz.bmp",0);
	int bd2 = m_pkTexMan->Load("file:../data/textures/border_vert.bmp",0);
	int bd3 = m_pkTexMan->Load("file:../data/textures/border_corner.bmp",0);
	int bda = m_pkTexMan->Load("file:../data/textures/border_corner_a.bmp",0);
	int cbn_up = m_pkTexMan->Load("file:../data/textures/checkbox_off.bmp",0);
	int cbn_down = m_pkTexMan->Load("file:../data/textures/checkbox_on.bmp",0);
	int iRbSqr_up = m_pkTexMan->Load("file:../data/textures/RadioBnSqr_On.bmp",0);
	int iRbSqr_down = m_pkTexMan->Load("file:../data/textures/RadioBnSqr_Off.bmp",0);

	m_kTextureMap.insert( map<string,int>::value_type(
		string("border_corner_a"),bda)); 

	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("bn_up"),
		new ZGuiSkin(bn_up,false) ) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("bn_down"),
		new ZGuiSkin(bn_down,false) ) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("bn_focus"),
		new ZGuiSkin(bn_focus,false) ) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("menu_item"),
		new ZGuiSkin(bn_up,false) ) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("menu_item_hl"),
		new ZGuiSkin(bn_up,-1,-1,-1, -1,-1,-1,-1, 255,0,0, 0,0,0, 0,false,false) ) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("menu_item_sel"),
		new ZGuiSkin(bn_up,-1,-1,-1, -1,-1,-1,-1, 128,0,0, 0,0,0, 0,false,false) ) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("gray"),
		new ZGuiSkin(214,211,206, 128,128,128, 1)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("dark_blue"),
		new ZGuiSkin(0,0,128, 0,0,0, 0)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("white"),
		new ZGuiSkin(255,255,255,0,0,0,1)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("Label"),
		new ZGuiSkin(0,0,0, 0,0,0, 0,true)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("titlebar"),
		new ZGuiSkin(64,64,128,0,0,0,1)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("rbn_down"),
		new ZGuiSkin(rbn_up,rbn_a,false)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("rbn_up"),
		new ZGuiSkin(rbn_down,rbn_a,false)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("cbn_down"),
		new ZGuiSkin(cbn_down,false)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("cbn_up"),
		new ZGuiSkin(cbn_up,false)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("RbSqrSqrUp"),
		new ZGuiSkin(iRbSqr_up,false)) ); 
	m_kSkinMap.insert( map<string,ZGuiSkin*>::value_type(string("RbSqrSqrDown"),
		new ZGuiSkin(iRbSqr_down,false)) ); 
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: Register
// Description: 
//
bool GuiBuilder::Register(ZGuiWnd *pkWnd, char* szName)
{
	return m_pkGui->RegisterWindow(pkWnd, szName);
	//return (m_pkGuiMan->Add(string(szName),pkWnd) != NULL);
}

bool GuiBuilder::Register(ZGuiSkin *pkSkin, char* szName)
{
	return (m_pkGuiMan->Add(string(szName),pkSkin) != NULL);
}

bool GuiBuilder::Register(ZGuiFont *pkFont, char* szName)
{
	return (m_pkGuiMan->Add(string(szName),pkFont) != NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetWnd
// Description: 
//
ZGuiWnd* GuiBuilder::GetWnd(const char* szName)
{
	return m_pkGuiMan->Wnd(string(szName));
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetSkin
// Description: 
//
ZGuiSkin* GuiBuilder::GetSkin(const char* szName, bool bAllocateNew)
{
	static ZGuiSkin* fail_skin;
	static bool init = false;

	if(init == false)
	{
		int tex = m_pkTexMan->Load("file:../data/textures/notex.bmp",0);
		fail_skin = new ZGuiSkin(tex,false);
		init = true;
	}

	map<string,ZGuiSkin*>::iterator ret = m_kSkinMap.find(string(szName));

	if(ret != m_kSkinMap.end())
	{
		if(bAllocateNew == false)
			return ret->second;
		else
		{
			ZGuiSkin* pkSkin = new ZGuiSkin(*ret->second);
			return pkSkin;
		}
	}
	
	return fail_skin;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetTexture
// Description: 
//
int GuiBuilder::GetTexture(const char* szName)
{
	static int fail_texture;
	static bool init = false;

	if(init == false)
	{
		fail_texture = m_pkTexMan->Load("file:../data/textures/notex.bmp",0);
		init = true;
	}

	map<string,int>::iterator ret = m_kTextureMap.find(string(szName));

	if(ret != m_kTextureMap.end())
		return ret->second;
	
	return fail_texture;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetTextInt
// Description: 
//
bool GuiBuilder::SetTextInt(const char *szWindow,int iValue)
{
	char szText[50];
	memset(szText, 0, 50);
	sprintf(szText,"%i",iValue);
	ZGuiWnd* pkWnd = m_pkGuiMan->Wnd(szWindow);
	
	if(pkWnd == NULL)
		return false;
	
	pkWnd->SetText(szText);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetTextInt
// Description: 
//
int GuiBuilder::GetTextInt(const char *szWindow)
{
	ZGuiWnd* pkWnd = m_pkGuiMan->Wnd(szWindow);
	
	if(pkWnd == NULL)
		return 0;
	
	return atoi(pkWnd->GetText());
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetTextString
// Description: 
//
bool GuiBuilder::SetTextString(const char *szWindow,char* szString)
{
	ZGuiWnd* pkWnd = m_pkGuiMan->Wnd(szWindow);
	
	if(pkWnd == NULL)
		return false;
	
	pkWnd->SetText(szString);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: IsButtonChecked
// Description: 
//
bool GuiBuilder::IsButtonChecked(const char *szCheckBox)
{	
	ZGuiWnd* pkWnd = m_pkGuiMan->Wnd(szCheckBox);

	if(pkWnd == NULL)
		return false;
	
	const type_info& t = typeid(*pkWnd);

	if(t==typeid(ZGuiCheckbox))
	{
		ZGuiCheckbox* pkCB = dynamic_cast<ZGuiCheckbox*>
			(m_pkGuiMan->Wnd(szCheckBox));
		return pkCB->IsChecked();
	}
	else
	if(t==typeid(ZGuiRadiobutton))
	{
		ZGuiRadiobutton* pkCB = dynamic_cast<ZGuiRadiobutton*>
			(m_pkGuiMan->Wnd(szCheckBox));
		return pkCB->GetButton()->IsChecked();
	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetTypeName
// Description: 
//
char* GuiBuilder::GetTypeName(ZGuiWnd *pkWnd)
{
	const type_info& t = typeid(*pkWnd);

	if(t==typeid(ZGuiTextbox))
		return("Textbox");
	else
	if(t==typeid(ZGuiScrollbar))
		return("Scrollbar");
	else
	if(t==typeid(ZGuiRadiobutton))
		return("Radiobutton");
	else
	if(t==typeid(ZGuiListbox))
		return("Listbox");
	else
	if(t==typeid(ZGuiLabel))
		return("Label");
	else
	if(t==typeid(ZGuiCheckbox))
		return("Checkbox");
	else
	if(t==typeid(ZGuiButton))
		return("Button");
	else
	if(t==typeid(ZGuiWnd))
		return("Window");
	else
	if(t==typeid(ZGuiCombobox))
		return("Combobox");

	return "NULL";
}

///////////////////////////////////////////////////////////////////////////////
// Name: GetType
// Description: 
//
CtrlType GuiBuilder::GetType(char* szTypeName)
{
	if(strcmp(szTypeName, "Textbox")==0)
		return TEXTBOX;
	else
	if(strcmp(szTypeName, "Scrollbar")==0)
		return SCROLLBAR;
	else
	if(strcmp(szTypeName, "Radiobutton")==0)
		return RADIOBUTTON;
	else
	if(strcmp(szTypeName, "Listbox")==0)
		return LISTBOX;
	else
	if(strcmp(szTypeName, "Label")==0)
		return LABEL;
	else
	if(strcmp(szTypeName, "Checkbox")==0)
		return CHECKBOX;
	else
	if(strcmp(szTypeName, "Button")==0)
		return BUTTON;
	else
	if(strcmp(szTypeName, "Window")==0)
		return WINDOW;
	else
	if(strcmp(szTypeName, "Combobox")==0)
		return COMBOBOX;

	return NONE;
}

///////////////////////////////////////////////////////////////////////////////
// Name: GenUniqueWndName
// Description: 
//
bool GuiBuilder::GenUniqueWndName(ZGuiWnd *pkWnd, char szName[50])
{
	sprintf(szName, "%s%i", GetTypeName(pkWnd), pkWnd->GetID());
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetColor
// Description: 
//
void GuiBuilder::SetColor(float afColor[3], const unsigned char r, 
						  const unsigned char g, const unsigned char b)
{
	float afDstColor[3] = 
	{ 
		(float) r / 255,
		(float) g / 255,
		(float) b / 255,
	};

	memcpy(afColor, afDstColor, sizeof(float)*3);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetScrollPos
// Description: 
//
void GuiBuilder::SetScrollPos(const char *szScrollbar, float fPos)
{
	ZGuiScrollbar* pkScrollbar = ((ZGuiScrollbar*) GetWnd(szScrollbar));

	if(pkScrollbar)
	{
		unsigned int min, max, pos;
		pkScrollbar->GetScrollInfo(min,max,pos);

		float fThumbSize = 10.0f / (float) max;
		pos = (unsigned int) (fPos * (max-min));

		pkScrollbar->SetScrollInfo(min,max,fThumbSize,pos);
	}
}

CtrlType GuiBuilder::GetWndType(ZGuiWnd* pkWnd)
{
	const type_info& t = typeid(*pkWnd);

	if(t==typeid(ZGuiTextbox))
		return TEXTBOX;
	else
	if(t==typeid(ZGuiScrollbar))
		return SCROLLBAR;
	else
	if(t==typeid(ZGuiRadiobutton))
		return RADIOBUTTON;
	else
	if(t==typeid(ZGuiListbox))
		return LISTBOX;
	else
	if(t==typeid(ZGuiLabel))
		return LABEL;
	else
	if(t==typeid(ZGuiCheckbox))
		return CHECKBOX;
	else
	if(t==typeid(ZGuiButton))
		return BUTTON;
	else
	if(t==typeid(ZGuiWnd))
		return WINDOW;
	else
	if(t==typeid(ZGuiCombobox))
		return COMBOBOX;

	return NONE;
}

bool GuiBuilder::RenameWnd(ZGuiWnd* pkWndToRename, const char *szNewName)
{
	bool bSuccess;

	if(pkWndToRename == NULL)
		bSuccess = false;
	else
		bSuccess = m_pkGui->ChangeWndRegName(pkWndToRename, szNewName);

	if(!bSuccess)
	{
		char error[50];

		if(pkWndToRename)
			sprintf(error, "Failed to rename window [%s] to [%s]\n",
				pkWndToRename->GetName(), szNewName);
		else
			sprintf(error, "Failed to rename window, window is null!\n");
		printf(error);
	}

	return bSuccess;
}
