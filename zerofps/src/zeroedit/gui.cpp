// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

#include "gui.h"
#include "fileopendlg.h"
#include "editpropertydlg.h"
#include "resource_id.h"
#include <algorithm>
#include <typeinfo>

extern ZeroEdit Editor;

// Window proc wrappers
//////////////////////////

static bool MAINWINPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->WndProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

static bool OPENFILEPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->m_pkFileDlgbox->DlgProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

//////////////////////////

Gui::Gui(ZeroEdit* pkEdit)
{
	m_iScreenCX = pkEdit->m_iWidth / 2;
	m_iScreenCY = pkEdit->m_iHeight / 2;
	m_pkEdit = pkEdit;
	m_pkFileDlgbox = NULL;
	m_pkEditPropDlgBox = NULL;
	
	InitSkins();
	CreateWindows();

	int cursor_tex = pkEdit->pkTexMan->Load("file:../data/textures/cursor.bmp", 0);
	int cursor_tex_a = pkEdit->pkTexMan->Load("file:../data/textures/cursor_a.bmp", 0);
	pkEdit->pkGui->SetCursor(cursor_tex, cursor_tex_a, 32, 32);
	pkEdit->pkFps->m_bGuiTakeControl = true;

	m_bGuiHaveFocus = false;
}

Gui::~Gui()
{

}

bool Gui::WndProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
{
	Rect rc;

	int iControllID;

	unsigned long flags;
	flags = 0;

	switch(uiMessage)
	{
	case ZGM_SETFOCUS:
		if(pkWindow)
		{
			//if(typeid(*pkWindow)==typeid(ZGuiTextbox))
	/*		{
				m_bKeyboardCaptured = true;		
			}
			else
			{
				m_bKeyboardCaptured = false;
			}*/

//			m_bKeyboardCaptured = true;
		}
		break;

	case ZGM_COMMAND:
		iControllID = ((int*)pkParams)[0];

		switch(iControllID)
		{
		case ID_FILEPATH_OPEN_BN:
			char cmd[512];
			char path[512];
			
			switch(m_kSearchTask)
			{
			case MAP:
				sprintf(cmd, "load %s", m_pkFileDlgbox->m_szCurrentDir.c_str()); 
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);
				break;

			case LOAD_TEMPLATE:
				// Som det är nu så letar den alltid i BIN katalagen.
				// Egentligen skall man även skicka med m_pkFileDlgbox->m_szSearchPath.c_str(),
				// men detta går nämligen inte eftersom Execute klipper strängen när den 
				// hittar en space-tecken.
				sprintf(path, "%s", m_pkFileDlgbox->m_szCurrentFile.c_str());

				sprintf(cmd, "loadtemplate %s", path); 
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);	

				sprintf(cmd, "set g_template %s", m_pkFileDlgbox->m_szCurrentFile.c_str()); 
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);	
				break;

			case SAVE_TEMPLATE:
				sprintf(path, "%s", m_pkFileDlgbox->m_szCurrentFile.c_str());
				sprintf(cmd, "maketemplate %s", m_pkFileDlgbox->m_szCurrentFile.c_str()); 
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);	

				sprintf(cmd, "savetemplate %s %s", path, m_pkFileDlgbox->m_szCurrentFile.c_str()); 
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);	
				break;
			}

			CaptureInput(false);
			break;

			case ID_FILEPATH_WND_CLOSE:
			case ID_FILEPATH_CANCEL_BN:
				CaptureInput(false);
				break;
		}
		break;

	case ZGM_CBN_SELENDOK:
		{
			int iID = ((int*)pkParams)[0];
			ZGuiWnd *win = ((ZGuiWnd*)m_pkEdit->pkGui->GetWindow(iID));

			if(win)
			{
				ZGuiCombobox *cbox = (ZGuiCombobox*) win;

				int iItemID = ((int*)pkParams)[1];

				switch(iItemID)
				{
				case IDM_CLOSE:
					if(cbox == m_pkEdit->pkGuiMan->Wnd("MainMenuCB1"))
						m_pkEdit->pkFps->QuitEngine();
					break;

				case IDM_LOAD_TEMPLATE:
				case IDM_LOAD_HEIGHTMAP:

					switch(iItemID)
					{
					case IDM_LOAD_HEIGHTMAP:
						m_kSearchTask = MAP;
						flags = DIRECTORIES_ONLY; 
						break;
					case IDM_LOAD_TEMPLATE:
						m_kSearchTask = LOAD_TEMPLATE;
						break;
					}

					if(m_pkFileDlgbox)
					{
						delete m_pkFileDlgbox;
						m_pkFileDlgbox = NULL;
					}

					m_pkFileDlgbox = new FileOpenDlg(m_pkEdit->pkGui, 
						m_pkEdit->pkFps->m_pkBasicFS, MAINWINPROC, flags);
					m_pkFileDlgbox->Create(100,100,500,500,OPENFILEPROC);
					CaptureInput(true);
					break;

				case IDM_SAVE_TEMPLATE:

					switch(iItemID)
					{
					case IDM_SAVE_TEMPLATE:
						m_kSearchTask = SAVE_TEMPLATE;
						flags = SAVE_FILES; 
						break;
					}

					if(m_pkFileDlgbox)
					{
						delete m_pkFileDlgbox;
						m_pkFileDlgbox = NULL;
					}

					m_pkFileDlgbox = new FileOpenDlg(m_pkEdit->pkGui, 
						m_pkEdit->pkFps->m_pkBasicFS, MAINWINPROC, flags);
					m_pkFileDlgbox->Create(100,100,500,500,OPENFILEPROC);
					CaptureInput(true);
					break;

				case IDM_CREATE_NEW_PROPERTY:

					if(m_pkEditPropDlgBox)
					{
						delete m_pkEditPropDlgBox;
						m_pkEditPropDlgBox = NULL;
					}

					m_pkEditPropDlgBox = new EditPropertyDlg(this, 
						m_pkEdit->pkPropertyFactory, 
						m_pkEdit->pkObjectMan, 
						m_pkEdit->m_pkCurentChild, MAINWINPROC);
					break;
				}
			}
		}
		break;
	}
	return true;
}

bool Gui::CreateWindows()
{	
	ZGuiWnd* pkMenu = new ZGuiWnd(Rect(0,0,m_pkEdit->m_iWidth,20),NULL,true,ID_MAINWND_MENU);
	pkMenu->SetSkin(GetSkin("menu"));
	pkMenu->SetZValue(102321);

	Rect rc = Rect(0,0,128,32);

	rc = Rect(0,0,200,20);
	ZGuiCombobox* pkMenuCBox = new ZGuiCombobox(rc, pkMenu, true, ID_MAINWND_MENU_CB, 20,
		GetSkin("menu"), GetSkin("dark_blue"), GetSkin("dark_blue"), GetSkin("menu"));
	pkMenuCBox->SetGUI(m_pkEdit->pkGui);
	pkMenuCBox->SetLabelText("File");
	pkMenuCBox->IsMenu(true);
	pkMenuCBox->AddItem("Load map...", IDM_LOAD_HEIGHTMAP);
	pkMenuCBox->AddItem("Load template...", IDM_LOAD_TEMPLATE);
	pkMenuCBox->AddItem("Save template...", IDM_SAVE_TEMPLATE);
	pkMenuCBox->AddItem("Edit property...", IDM_CREATE_NEW_PROPERTY);
	pkMenuCBox->AddItem("Quit", IDM_CLOSE);
	Register(pkMenuCBox, "MainMenuCB1");

/*	ZGuiButton* pkCloseGuiButton = CreateButton(pkMenu, ID_CLOSE_GUI_BN, 
		m_pkEdit->m_iWidth-20, 0, 20, 20, "x");
	pkCloseGuiButton->SetWindowFlag(WF_CENTER_TEXT);
	Register( pkCloseGuiButton, "CloseGuiBN");*/

//	m_pkEdit->pkGui->AddKeyCommand(KEY_ESCAPE, pkMenu, pkCloseGuiButton);

	m_pkEdit->pkGui->AddMainWindow(IDM_MENU_WND, pkMenu, "MainMenu", MAINWINPROC, true);

	return true;
}

bool Gui::InitSkins()
{
	//int piss = m_pkEdit->pkTexMan->Load("file:../data/textures/piss.bmp", 0); // första misslyckas, vet inte varför...
	int bn_up = m_pkEdit->pkTexMan->Load("file:../data/textures/button_up.bmp", 0);
	int bn_down = m_pkEdit->pkTexMan->Load("file:../data/textures/button_down.bmp", 0);
	int bn_focus = m_pkEdit->pkTexMan->Load("file:../data/textures/button_focus.bmp", 0);
	int rbn_up = m_pkEdit->pkTexMan->Load("file:../data/textures/radiobn_up.bmp", 0);
	int rbn_down = m_pkEdit->pkTexMan->Load("file:../data/textures/radiobn_down.bmp", 0);
	int rbn_a = m_pkEdit->pkTexMan->Load("file:../data/textures/radiobn_a.bmp",0);
	int bk1 = m_pkEdit->pkTexMan->Load("file:../data/textures/bk1.bmp", 0);
	int bd1 = m_pkEdit->pkTexMan->Load("file:../data/textures/border_vert.bmp", 0);
	int bd2 = m_pkEdit->pkTexMan->Load("file:../data/textures/border_horz.bmp", 0);
	int bd3 = m_pkEdit->pkTexMan->Load("file:../data/textures/border_corner.bmp", 0);
	int bda = m_pkEdit->pkTexMan->Load("file:../data/textures/border_corner_a.bmp", 0);

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("main"), 
		new ZGuiSkin(bk1,bd1,bd2,bd3,-1,-1,-1,bda,16,true) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("blue"), 
		new ZGuiSkin(255, 0, 0, 128, 4)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("red"), 
		new ZGuiSkin(255, 0, 0, 128, 0, 0, 4)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("bn_up"), 
		new ZGuiSkin(bn_up, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("bn_down"), 
		new ZGuiSkin(bn_down, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("bn_focus"), 
		new ZGuiSkin(bn_focus, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu_item"), 
		new ZGuiSkin(bn_up, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu_item_hl"), 
		new ZGuiSkin(bn_up,-1,-1,-1,-1,-1,-1,-1,255,0,0,0,0,0,0,false,false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu_item_sel"), 
		new ZGuiSkin(bn_up,-1,-1,-1, -1,-1,-1,-1, 128,0,0, 0,0,0, 0, false,false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu"), 
		new ZGuiSkin(128, 128, 128, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("dark_blue"), 
		new ZGuiSkin(0, 0, 128, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("white"), 
		new ZGuiSkin(255, 255, 255, 0, 0, 0, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("titlebar"), 
		new ZGuiSkin(64, 64, 128, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("rbn_down"), 
		new ZGuiSkin(rbn_up, rbn_a, false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("rbn_up"), 
		new ZGuiSkin(rbn_down, rbn_a, false)) ); 

	return true;
}

ZGuiSkin* Gui::GetSkin(char* strName)
{
	static ZGuiSkin* fail_skin;
	static bool init = false;

	if(init == false)
	{
		int tex = m_pkEdit->pkTexMan->Load("file:../data/textures/notex.bmp", 0);
		fail_skin = new ZGuiSkin(tex, -1, -1, -1, 255, 255, 255, 255, 255, 255);
		init = true;
	}

	map<string, ZGuiSkin*>::iterator ret = m_kSkinMap.find(string(strName));

	if(ret != m_kSkinMap.end())
		return ret->second;
	
	return fail_skin;
}

int Gui::GetTexture(char* strName)
{
	static int fail_texture;
	static bool init = false;

	if(init == false)
	{
		fail_texture = m_pkEdit->pkTexMan->Load("file:../data/textures/notex.bmp", 0);
		init = true;
	}

	map<string, int>::iterator ret = m_kTextureMap.find(string(strName));

	if(ret != m_kTextureMap.end())
		return ret->second;
	
	return fail_texture;
}

ZGuiButton* Gui::CreateButton(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char *pkName)
{
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetText(pkName);
	pkButton->SetGUI(m_pkEdit->pkGui);

	return pkButton;
}

void Gui::CreateRadiobuttons(ZGuiWnd* pkParent, vector<string>& vkNames,
							 char* strRadioGroupName, int start_id, int x, int y, int size)
{
	int rbn_a = m_pkEdit->pkTexMan->Load("file:../data/textures/radiobn_a.bmp", 0);

	int GroupID = start_id;
	ZGuiRadiobutton* pkPrev = NULL;
	Rect rc = Rect(x,y,x+size,y+size);
	const int antal = vkNames.size();

	for(int i=0; i<antal; i++)
	{
		ZGuiRadiobutton* pkGroupbutton = new ZGuiRadiobutton(rc,pkParent,
			start_id+i,start_id,pkPrev,true);
		pkGroupbutton->SetButtonUnselectedSkin(GetSkin("rbn_down"));
		pkGroupbutton->SetButtonSelectedSkin(GetSkin("rbn_up"));
		pkGroupbutton->SetGUI(m_pkEdit->pkGui);
		pkGroupbutton->SetText((char*)vkNames[i].c_str(),true);
		
		if(pkPrev == NULL)
		{
			Register(pkGroupbutton, strRadioGroupName);
		}

		pkPrev = pkGroupbutton;
		rc = rc.Move(0,20);
	}
}

ZGuiListbox* Gui::CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h)
{
	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,iID,20,
		GetSkin("menu_item"), GetSkin("menu_item_sel"), GetSkin("menu_item_hl"));
	pkListbox->SetSkin( GetSkin("dark_blue") );
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	pkListbox->SetGUI(m_pkEdit->pkGui);
	return pkListbox;
}

ZGuiCombobox* Gui::CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMenu)
{
	ZGuiCombobox* pkCombobox = new ZGuiCombobox(Rect(x,y,x+w,y+h), pkParent, true, iID, 20,
		GetSkin("menu"), GetSkin("dark_blue"), GetSkin("dark_blue"), GetSkin("menu"));

	if(bMenu == false)
	{
		pkCombobox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
			GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	}

	pkCombobox->SetGUI(m_pkEdit->pkGui);

	return pkCombobox;
}

ZGuiTextbox* Gui::CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMulitLine)
{
	ZGuiTextbox* pkTextbox = new ZGuiTextbox(Rect(x,y,x+w,y+h), pkParent, true, iID, bMulitLine);
	pkTextbox->SetSkin(GetSkin("white"));
	pkTextbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	pkTextbox->SetGUI(m_pkEdit->pkGui);

	return pkTextbox;
}

ZGuiLabel* Gui::CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char* strText)
{
	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h), pkParent, true, iID);
	
	if(strText)
		pkLabel->SetText(strText);

	pkLabel->SetGUI(m_pkEdit->pkGui);

	return pkLabel;
}

void Gui::AddItemsToList(ZGuiWnd *pkWnd, bool bCombobox, char **items, int iNumber, bool bSelectLast)
{
	if(bCombobox)
	{
		ZGuiCombobox* pkCombobox = (ZGuiCombobox*) pkWnd;
		for(int i=0; i<iNumber; i++)
		{
			pkCombobox->AddItem(items[i], i, bSelectLast && i==iNumber-1);
		}
	}
	else
	{
		ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
		for(int i=0; i<iNumber; i++)
			pkListbox->AddItem(items[i], i, bSelectLast && i==iNumber-1);
	}
}

void Gui::AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int index, bool bSelect)
{
	if(bCombobox)
	{
		ZGuiCombobox* pkCombobox = (ZGuiCombobox*) pkWnd;
		pkCombobox->AddItem((char*)item, index, bSelect);
	}
	else
	{
		ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
		pkListbox->AddItem((char*)item, index, bSelect);
	}
}

bool Gui::Register(ZGuiWnd *pkWnd, char* strName)
{
	//return (m_pkEdit->pkGuiMan->Add(string(strName), pkWnd) != NULL);
	return m_pkEdit->pkGui->RegisterWindow(pkWnd, strName);
}

bool Gui::Register(ZGuiSkin *pkSkin, char* strName)
{
	return (m_pkEdit->pkGuiMan->Add(string(strName), pkSkin) != NULL);
}

bool Gui::Register(ZGuiFont *pkFont, char* strName)
{
	return (m_pkEdit->pkGuiMan->Add(string(strName), pkFont) != NULL);
}

ZGuiWnd* Gui::Get(char* strName)
{
	return m_pkEdit->pkGuiMan->Wnd(string(strName));
}

void Gui::CaptureInput(bool bCapture)
{
	m_bGuiHaveFocus = bCapture;
}