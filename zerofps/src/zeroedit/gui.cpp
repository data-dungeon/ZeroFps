// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

//#include "zeroedit.h"
#include "gui.h"
#include "fileopendlg.h"
#include "resource_id.h"
#include <algorithm>

extern ZeroEdit Editor;

// Window proc wrappers
//////////////////////////

static bool WINPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->WndProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

static bool PROPERTYPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->PropertyProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

//////////////////////////

Gui::Gui(ZeroEdit* pkEdit)
{
	m_iScreenCX = pkEdit->m_iWidth / 2;
	m_iScreenCY = pkEdit->m_iHeight / 2;
	m_pkEdit = pkEdit;
	m_pkFileDlgbox = NULL;

	InitSkins();
	CreateWindows();

	int cursor_tex = pkEdit->pkTexMan->Load("file:../data/textures/cursor.bmp", 0);
	int cursor_tex_a = pkEdit->pkTexMan->Load("file:../data/textures/cursor_a.bmp", 0);
	pkEdit->pkGui->SetCursor(cursor_tex, cursor_tex_a, 32, 32);
}

Gui::~Gui()
{

}

bool Gui::PropertyProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
{
	int iControllID;

	switch(uiMessage)
	{
	case ZGM_COMMAND:
		iControllID = ((int*)pkParams)[0];

		switch(iControllID)
		{
		case ID_PROPERTY_CANCEL:
		case ID_PROPERTY_WND_CLOSE:
			if(OnCloseEditProperty(false))
				m_pkEdit->pkGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, false);
			break;
		case ID_PROPERTY_OK:
			if(OnCloseEditProperty(true))
				m_pkEdit->pkGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, false);
			break;
		}
		break;

	case ZGM_CBN_SELENDOK:
		{
			int iID = ((int*)pkParams)[0];
			ZGuiWnd *win = ((ZGuiWnd*)m_pkEdit->pkGui->GetWindow(iID));

			if(iID == ID_OBJECTS_CB)
			{
				ZGuiListitem* pkSelItem = ((ZGuiCombobox*)(win))->GetListbox()->GetSelItem();

				if(pkSelItem)
				{
					Object* pkObject = m_pkEdit->pkFps->m_pkObjectMan->GetObject(pkSelItem->GetText());

					if(pkObject)
					{
						ZGuiWnd* pkWnd = m_pkEdit->pkGui->GetWindow(ID_PROPERTIES_CB);

						if(pkWnd)
						{
							ZGuiCombobox* pkCB = (ZGuiCombobox*) pkWnd;
							pkCB->RemoveAllItems();

							list<Property*> pkList;
							pkObject->GetPropertys(&pkList, PROPERTY_TYPE_ALL, PROPERTY_SIDE_ALL);

							list<Property*>::iterator s = pkList.begin();
							list<Property*>::iterator e = pkList.end();
							int counter=0;

							for( ; s != e; s++ )
							{
								pkCB->SetLabelText((*s)->m_acName);
								AddItemToList(pkCB, true, (*s)->m_acName, counter++);
							}
						}
					}
				}
			}
		}
		break;
	}
	return true;
}

bool Gui::WndProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
{
	Rect rc;

	int iControllID;

	switch(uiMessage)
	{
	case ZGM_COMMAND:
		iControllID = ((int*)pkParams)[0];

		switch(iControllID)
		{
		case ID_CLOSE:
			m_pkEdit->pkFps->QuitEngine();
			break;

		case ID_CLOSE_GUI_BN:
			m_pkEdit->pkFps->ToggleGui();
			break;

		case ID_FILEPATH_OPEN_BN:
			char cmd[512];
			sprintf(cmd, "load %s", m_pkFileDlgbox->m_szCurrentDir.c_str()); 
			m_pkEdit->pkFps->m_pkConsole->Execute(cmd);
			m_pkEdit->pkGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, false);
			m_pkEdit->pkFps->ToggleGui();
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
				int w, h;

				switch(iItemID)
				{
				case IDM_CLOSE:
					m_pkEdit->pkFps->QuitEngine();
					break;

				case IDM_LOAD_HEIGHTMAP:
					{
						if(m_pkFileDlgbox)
						{
							delete m_pkFileDlgbox;
							m_pkFileDlgbox = NULL;
						}

						m_pkFileDlgbox = new FileOpenDlg(this, m_pkEdit->pkFps->m_pkBasicFS, WINPROC);
					}
					break;

				case IDM_CREATE_NEW_PROPERTY:
					w = 500;
					h = 500;
					CreateEditPropertyDialog(0,0,w,h);
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

	rc = Rect(0,0,200,200);
	ZGuiCombobox* pkMenuCBox = new ZGuiCombobox(rc, pkMenu, true, ID_MAINWND_MENU_CB, 20,
		GetSkin("menu"), GetSkin("dark_blue"), GetSkin("dark_blue"), GetSkin("menu"), -1);
	pkMenuCBox->SetLabelText("File");
	pkMenuCBox->IsMenu(true);
	pkMenuCBox->AddItem("Quit", IDM_CLOSE);
	pkMenuCBox->AddItem("Load map...", IDM_LOAD_HEIGHTMAP);
	pkMenuCBox->AddItem("Edit property...", IDM_CREATE_NEW_PROPERTY);

	ZGuiButton* pkCloseButton = CreateButton(pkMenu, ID_CLOSE_GUI_BN, 
		m_pkEdit->m_iWidth-20, 0, 20, 20, "x");

	pkCloseButton->SetWindowFlag(WF_CENTER_TEXT);
	Register( pkCloseButton, "CloseGuiBN");

	m_pkEdit->pkGui->AddMainWindow(IDM_MENU_WND, pkMenu, WINPROC, true);

	return true;
}

bool Gui::InitSkins()
{
	//int piss = m_pkEdit->pkTexMan->Load("file:../data/textures/piss.bmp", 0); // första misslyckas, vet inte varför...
	int bn_up = m_pkEdit->pkTexMan->Load("file:../data/textures/button_up.bmp", 0);
	int bn_down = m_pkEdit->pkTexMan->Load("file:../data/textures/button_down.bmp", 0);
	int bn_focus = m_pkEdit->pkTexMan->Load("file:../data/textures/button_focus.bmp", 0);

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("main"), 
		new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 255, 0, 0, 8)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("blue"), 
		new ZGuiSkin(-1, -1, -1, -1, 0, 0, 255, 0, 0, 128, 4)) ); 
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
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("menu"), 
		new ZGuiSkin(-1, -1, -1, -1, 128, 128, 128, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("dark_blue"), 
		new ZGuiSkin(-1, -1, -1, -1, 0, 0, 128, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("white"), 
		new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 0, 0, 0, 0)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("titlebar"), 
		new ZGuiSkin(-1, -1, -1, -1, 64, 64, 128, 0, 0, 0, 0)) ); 

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

	return pkButton;
}

ZGuiListbox* Gui::CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h)
{
	//h += (20 % h); // avrunda till närmsta 20 tal

	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,iID,20,
		GetSkin("menu_item"), GetSkin("menu_item_sel"), GetSkin("menu_item_hl"));
	pkListbox->SetSkin( GetSkin("dark_blue") );
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	return pkListbox;
}

ZGuiCombobox* Gui::CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMenu)
{
//	h += (20 % h); // avrunda till närmsta 20 tal

	ZGuiCombobox* pkCombobox = new ZGuiCombobox(Rect(x,y,x+w,y+h), pkParent, true, iID, 20,
		GetSkin("menu"), GetSkin("dark_blue"), GetSkin("dark_blue"), GetSkin("menu"), -1);

	if(bMenu == false)
	{
		pkCombobox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
			GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	}
	return pkCombobox;
}

ZGuiTextbox* Gui::CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, bool bMulitLine)
{
/*	h += (20 % h); // avrunda till närmsta 20 tal*/

	ZGuiTextbox* pkTextbox = new ZGuiTextbox(Rect(x,y,x+w,y+h), pkParent, true, iID, bMulitLine);
	pkTextbox->SetSkin(GetSkin("white"));
	pkTextbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));

	return pkTextbox;
}

ZGuiLabel* Gui::CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char* strText)
{
	//h += (20 % h); // avrunda till närmsta 20 tal

	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h), pkParent, true, iID);
	
	if(strText)
		pkLabel->SetText(strText);

	return pkLabel;
}

ZGuiWnd* Gui::CreateEditPropertyDialog(int x, int y, int w, int h)
{
	if( m_pkEdit->pkGui->GetMainWindow(ID_PROPERTY_WND_MAIN))
	{
		m_pkEdit->pkGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, true);
		OnOpenEditProperty();
		return false;
	}

	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_PROPERTY_WND);
	pkMainWindow->SetSkin(GetSkin("blue"));
	pkMainWindow->SetMoveArea(Rect(0,0,m_pkEdit->m_iWidth,m_pkEdit->m_iHeight));
	pkMainWindow->SetWindowFlag(WF_CLOSEABLE);

	int y_pos;

	CreateButton(pkMainWindow, ID_PROPERTY_WND_CLOSE, w-20, 0, 20, 20, "x")->SetWindowFlag(WF_CENTER_TEXT);
	CreateLabel(pkMainWindow, 0, 20, 20, 16*5, 20, "Name:");
	Register( CreateTextbox(pkMainWindow, ID_NAME_TEXTBOX, 16*6, 20, 200, 20), "ObjectNameEB" );

	CreateLabel(pkMainWindow, 0, 20, 60, 16*4, 20, "Pos:");
	Register( CreateTextbox(pkMainWindow, ID_POSX_TEXTBOX, 16*6, 60, 16*6, 20), "ObjectPosXEB");
	Register( CreateTextbox(pkMainWindow, ID_POSY_TEXTBOX, 16*6+16*7, 60, 16*6, 20), "ObjectPosYEB"); 
	Register( CreateTextbox(pkMainWindow, ID_POSZ_TEXTBOX, 16*6+16*7*2, 60, 16*6, 20), "ObjectPosZEB");

	y_pos = 100;

	ZGuiCombobox* cb;

	CreateLabel(pkMainWindow, 0, 20, y_pos, 16*9-5, 20, "Props:");

	Register(cb = CreateCombobox(pkMainWindow, ID_PROPERTIES_CB, 16*6, 
		y_pos, 16*6+16*7*2, h-(y_pos+60), false), "PropertyCB");

	cb->SetNumVisibleRows(20);
	
	CreateButton(pkMainWindow, ID_PROPERTY_OK, w-100, h-50, 80, 20, "OK")->SetWindowFlag(WF_CENTER_TEXT);
	CreateButton(pkMainWindow, ID_PROPERTY_CANCEL, w-100, h-25, 80, 20, "Cancel")->SetWindowFlag(WF_CENTER_TEXT);

	m_pkEdit->pkGui->AddMainWindow(ID_PROPERTY_WND_MAIN, pkMainWindow, PROPERTYPROC, true);

	OnOpenEditProperty();

	return pkMainWindow;
}

void Gui::AddItemsToList(ZGuiWnd *pkWnd, bool bCombobox, char **items, int iNumber)
{
	int test = 2;

	if(bCombobox)
	{
		ZGuiCombobox* pkCombobox = (ZGuiCombobox*) pkWnd;
		for(int i=0; i<iNumber; i++)
			pkCombobox->AddItem(items[i], i);
	}
	else
	{
		ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
		for(int i=0; i<iNumber; i++)
			pkListbox->AddItem(items[i], i);
	}
}

void Gui::AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int id)
{
	if(bCombobox)
	{
		ZGuiCombobox* pkCombobox = (ZGuiCombobox*) pkWnd;
		pkCombobox->AddItem((char*)item, id);
	}
	else
	{
		ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
		pkListbox->AddItem((char*)item, id);
	}
}

bool Gui::Register(ZGuiWnd *pkWnd, char* strName)
{
	return (m_pkEdit->pkGuiMan->Add(string(strName), pkWnd) != NULL);
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

void Gui::OnOpenEditProperty()
{
	ZGuiWnd* pkPropertysCB = Get("PropertyCB");
	ZGuiWnd* pkNameEB = Get("ObjectNameEB");
	ZGuiWnd* pkPosXEB = Get("ObjectPosXEB");
	ZGuiWnd* pkPosYEB = Get("ObjectPosYEB");
	ZGuiWnd* pkPosZEB = Get("ObjectPosZEB");

	if(pkPropertysCB == NULL)
	{
		cout << "Failed to find combox named [PropertyCB]" << endl;
		return;
	}

	if(pkNameEB == NULL)
	{
		cout << "Failed to find editbox named [ObjectNameEB]" << endl;
		return;
	}

	if(m_pkEdit->m_pkCurentChild)
	{
		char text[50];
		Vector3 p = m_pkEdit->m_pkCurentChild->GetPos();
		
		((ZGuiTextbox*) pkNameEB)->SetText((char*)m_pkEdit->m_pkCurentChild->GetName().c_str());

		sprintf(text, "%0.3f", p.x); ((ZGuiTextbox*) pkPosXEB)->SetText(text);
		sprintf(text, "%0.3f", p.y); ((ZGuiTextbox*) pkPosYEB)->SetText(text);
		sprintf(text, "%0.3f", p.z); ((ZGuiTextbox*) pkPosZEB)->SetText(text);

		list<Property*> akPropertys;

		m_pkEdit->m_pkCurentChild->GetPropertys(&akPropertys, PROPERTY_TYPE_ALL, PROPERTY_SIDE_ALL);

		list<Property*>::iterator s = akPropertys.begin();
		list<Property*>::iterator e = akPropertys.end();
 
		for(int counter ; s != e; s++, counter++ )
		{
			if(pkPropertysCB)
			{
				((ZGuiCombobox*) pkPropertysCB)->RemoveAllItems();
				AddItemToList(pkPropertysCB, true, (*s)->m_acName, counter++);
			}
		}	
	}
}

bool Gui::OnCloseEditProperty(bool bSave)
{
	ZGuiWnd* pkNameEB = Get("ObjectNameEB");

/*	if(pkNameEB)
	{
		char strText[512];
		pkNameEB->GetText(strText);
	}*/
	return true;
}
