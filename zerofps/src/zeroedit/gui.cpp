// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

//#include "zeroedit.h"
#include "gui.h"
#include "resource_id.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Gui::Gui(ZeroEdit* pkEdit, ZGuiCallBack cb)
{
	m_iScreenCX = pkEdit->m_iWidth / 2;
	m_iScreenCY = pkEdit->m_iHeight / 2;
	m_bMenuActive = true;
	m_pkEdit = pkEdit;

	m_pkWndProc = cb;

	InitSkins();
	CreateWindows();

	int cursor_tex = pkEdit->pkTexMan->Load("file:../data/textures/cursor.bmp", 0);
	int cursor_tex_a = pkEdit->pkTexMan->Load("file:../data/textures/cursor_a.bmp", 0);
	pkEdit->pkGui->SetCursor(cursor_tex, cursor_tex_a, 32, 32);
//	pkEdit->pkInput->ToggleGrab();
	ToogleMenu();
}

Gui::~Gui()
{

}

bool Gui::ZGWinProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
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

		case ID_FILEPATH_WND_CLOSE:
			m_pkEdit->pkGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, false);
			break;

		case ID_PROPERTY_WND_CLOSE:
			m_pkEdit->pkGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, false);
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

				case IDM_OPEN:
					w = 500;
					h = 500;
					CreateFilePathDialog(m_iScreenCX-w/2,m_iScreenCY-h/2,w,h);
					break;

				case IDM_CREATE_NEW_PROPERTY:
					w = 500;
					h = 500;
					CreatePropertyDialog(m_iScreenCX-w/2,m_iScreenCY-h/2,w,h);
					break;
				}
			}
		}
		break;


	case ZGM_SELECTLISTITEM:
		{
			int iIDListBox = ((int*)pkParams)[0];
			int iIDListItem = ((int*)pkParams)[1];

			ZGuiWnd* pkWnd = m_pkEdit->pkGui->GetWindow(iIDListBox);

			if(pkWnd != NULL)
			{			
				ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
				ZGuiListitem* pkItem = pkListbox->GetItem(iIDListItem);

				if(pkItem != NULL)
				{
					string szFileName = pkItem->GetText();

					if(szFileName == string("..") )
					{
						int new_path_length = m_szSearchBoxPath.find_last_of('\\'); 
						m_szSearchBoxPath.resize(new_path_length); 
					}
					else
					if(szFileName.find(".") == string::npos)
					{
						vector<string> vkDirectories;
						m_pkEdit->pkFps->m_pkBasicFS->ListDir(&vkDirectories, m_szSearchBoxPath.c_str(), true);

						for(unsigned int i=0; i<vkDirectories.size(); i++)
						{
							if(vkDirectories[i] == szFileName)
							{
								m_szSearchBoxPath.append("\\");
								m_szSearchBoxPath.append(szFileName);
								break;
							}
						}
					}
					else
					{
						ZGuiWnd* pkWnd = m_pkEdit->pkGui->GetWindow(ID_FILEPATH_WND_LABEL_FILE);

						if(pkWnd)
						{
							ZGuiLabel* pkLabel = (ZGuiLabel*) pkWnd;
							pkLabel->SetText((char*)szFileName.c_str()); 
						}
					}

					FillPathList(pkListbox, m_szSearchBoxPath);
				}
			}

		}
		break;

	}
	return true;
}

void Gui::ToogleMenu()
{
	m_bMenuActive = m_pkEdit->pkGui->ToogleGui();

	if(m_bMenuActive)
	{	
		m_pkEdit->pkFps->m_bDrawDevList = false;
	}
	else
	{
		m_pkEdit->pkFps->m_bDrawDevList = true;
	}
}

bool Gui::CreateWindows()
{
	int tex_font_a = m_pkEdit->pkTexMan->Load("file:../data/textures/text/font_a.bmp", 0);

	ZGuiWnd* pkMainWnd1 = new ZGuiWnd(Rect(0,0,m_pkEdit->m_iWidth,20),NULL,true,IDM_MENU_WND);
	pkMainWnd1->SetSkin(GetSkin("menu"));

	Rect rc = Rect(0,0,128,32);

	rc = Rect(0,0,200,200);
	ZGuiCombobox* pkMenuCBox = new ZGuiCombobox(rc, pkMainWnd1, true, iLastIDNr++, 20,
		GetSkin("font"), tex_font_a, GetSkin("menu"), GetSkin("dark_blue"), 
		GetSkin("dark_blue"), GetSkin("menu"), -1);
	pkMenuCBox->SetLabelText("File");
	pkMenuCBox->IsMenu(true);
	pkMenuCBox->AddItem("Quit", IDM_CLOSE);
	pkMenuCBox->AddItem("Open...", IDM_OPEN);
	pkMenuCBox->AddItem("Create new property...", IDM_CREATE_NEW_PROPERTY);

	m_pkEdit->pkGui->AddMainWindow(ID_MAINWND1, pkMainWnd1, m_pkWndProc, true);
	
	m_szSearchBoxPath = m_pkEdit->pkFps->m_pkBasicFS->GetCWD();
	return true;
}

bool Gui::InitSkins()
{
	//int piss = m_pkEdit->pkTexMan->Load("file:../data/textures/piss.bmp", 0); // första misslyckas, vet inte varför...
	int bn_up = m_pkEdit->pkTexMan->Load("file:../data/textures/button_up.bmp", 0);
	int bn_down = m_pkEdit->pkTexMan->Load("file:../data/textures/button_down.bmp", 0);
	int bn_focus = m_pkEdit->pkTexMan->Load("file:../data/textures/button_focus.bmp", 0);
	int font = m_pkEdit->pkTexMan->Load("file:../data/textures/text/font.bmp", 0);

	m_kTextureMap.insert( map<string, int>::value_type(string("font_a"), 
		m_pkEdit->pkTexMan->Load("file:../data/textures/text/font_a.bmp", 0)));

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
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("font"), 
		new ZGuiSkin(font, -1, -1, -1) ) ); 
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

ZGuiButton* Gui::CreateButton(ZGuiWnd* pkParent, int iID, int x, int y, char *pkName)
{
	int w = strlen(pkName) * 16;
	w += (20 % w); // avrunda till närmsta 20 tal
	int h = 20;

	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetTextSkin(GetSkin("font"), GetTexture("font_a"));
	pkButton->SetText(pkName);
	return pkButton;
}

ZGuiListbox* Gui::CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h)
{
	h += (20 % h); // avrunda till närmsta 20 tal

	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,iID,20,
		GetSkin("font"), GetTexture("font_a"), GetSkin("menu_item"), GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"));
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	return pkListbox;
}

ZGuiTextbox* Gui::CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h)
{
	h += (20 % h); // avrunda till närmsta 20 tal

	ZGuiTextbox* pkTextbox = new ZGuiTextbox(Rect(x,y,x+w,y+h), pkParent, true, iID);
	pkTextbox->SetSkin(GetSkin("white"));
	pkTextbox->SetTextSkin(GetSkin("font"), GetTexture("font_a"));

	return pkTextbox;
}

ZGuiLabel* Gui::CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h, char* strText)
{
	h += (20 % h); // avrunda till närmsta 20 tal

	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h), pkParent, true, iID);
	pkLabel->SetSkin(GetSkin("dark_blue"));
	pkLabel->SetTextSkin(GetSkin("font"), GetTexture("font_a"));

	if(strText)
		pkLabel->SetText(strText);

	return pkLabel;
}

ZGuiWnd* Gui::CreateFilePathDialog(int x, int y, int w, int h)
{
	if( m_pkEdit->pkGui->GetMainWindow(ID_FILEPATH_WND_MAIN))
	{
		m_pkEdit->pkGui->ShowMainWindow(ID_FILEPATH_WND_MAIN, true);
		return false;
	}

	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_FILEPATH_WND);
	pkMainWindow->SetSkin(GetSkin("blue"));
	pkMainWindow->SetMoveArea(Rect(0,0,m_pkEdit->m_iWidth,m_pkEdit->m_iHeight));

	ZGuiListbox* pkListbox = CreateListbox(pkMainWindow, ID_FILEPATH_WND, 0, 20, w, h-50);
	FillPathList(pkListbox, m_szSearchBoxPath);

	CreateLabel(pkMainWindow, ID_FILEPATH_WND_LABEL_PATH, 0, 0, w-20, 20, NULL);
	CreateLabel(pkMainWindow, ID_FILEPATH_WND_LABEL_FILE, 0, h-20, w-20, 20, NULL);
	CreateButton(pkMainWindow, ID_FILEPATH_WND_CLOSE, w-20, 0, "x");

	m_pkEdit->pkGui->AddMainWindow(ID_FILEPATH_WND_MAIN, pkMainWindow, m_pkWndProc, true);

	return pkMainWindow;
}

ZGuiWnd* Gui::CreatePropertyDialog(int x, int y, int w, int h)
{
	if( m_pkEdit->pkGui->GetMainWindow(ID_PROPERTY_WND_MAIN))
	{
		m_pkEdit->pkGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, true);
		return false;
	}

	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_PROPERTY_WND);
	pkMainWindow->SetSkin(GetSkin("blue"));
	pkMainWindow->SetMoveArea(Rect(0,0,m_pkEdit->m_iWidth,m_pkEdit->m_iHeight));

	CreateButton(pkMainWindow, ID_PROPERTY_WND_CLOSE, w-20, 0, "x");
	CreateLabel(pkMainWindow, 0, 20, 20, 16*5, 20, "Name:");
	CreateTextbox(pkMainWindow, ID_NAME_TEXTBOX, 16*6, 20, 200, 20);

	m_pkEdit->pkGui->AddMainWindow(ID_PROPERTY_WND_MAIN, pkMainWindow, m_pkWndProc, true);

	return pkMainWindow;
}

bool Gui::FillPathList(ZGuiListbox* pkListbox, string strDir)
{
	ZGuiWnd* pkWnd = m_pkEdit->pkGui->GetWindow(ID_FILEPATH_WND_LABEL_PATH);
	if(pkWnd != NULL)
	{
		const int MAX_LENGTH = 35;

		ZGuiLabel* pkLabel = (ZGuiLabel*) pkWnd;
		
		int length = m_szSearchBoxPath.length(); 
		int start = length - MAX_LENGTH;
		
		string szLabelText;

		if(start < 0)
			start = 0;
		else
			szLabelText = "...";

		szLabelText += m_szSearchBoxPath;
		szLabelText += start;
		
		pkLabel->SetText((char*)szLabelText.c_str());
	}

	vector<string> vkFiles;
	m_pkEdit->pkFps->m_pkBasicFS->ListDir(&vkFiles, strDir.c_str() );

	pkListbox->RemoveAllItems();

	if(vkFiles.size() > 0)
	{
		char name[450];
		for( unsigned int i=0; i<vkFiles.size(); i++)
		{
			sprintf(name, "%s", vkFiles[i]);
			pkListbox->AddItem(name, i); 
		}
	}
	else
	{
		printf("Dir have no files!\n");
		return false;
	}

	return true;
}


