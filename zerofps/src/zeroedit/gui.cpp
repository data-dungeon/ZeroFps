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
			printf("hej");
			m_pkEdit->pkGui->GetMainWindow(ID_MAINWND2)->Hide();
			break;
		}
		break;

	case ZGM_CBN_SELENDOK:
		{
			int iID; iID = ((int*)pkParams)[0];
			ZGuiCombobox *cbox = ((ZGuiCombobox*)m_pkEdit->pkGui->GetWindow(iID));

			if(cbox)
			{
				int iItemID = ((int*)pkParams)[1];

				switch(iItemID)
				{
				case IDM_CLOSE:
					m_pkEdit->pkFps->QuitEngine();
					break;

				case IDM_OPEN:
					CreateFilePathBox(0,0,500,500);
					m_pkEdit->pkGui->GetMainWindow(ID_MAINWND2)->Show();
					break;
				}
			}
		}
		break;


	case ZGM_SELECTLISTITEM:
		{
			int iIDListBox = ((int*)pkParams)[0];
			int iIDListItem = ((int*)pkParams)[1];

			printf("%i\n", iIDListBox);

			ZGuiWnd* pkWnd = m_pkEdit->pkGui->GetWindow(iIDListBox);

			if(pkWnd != NULL)
			{			
				ZGuiListbox* pkListbox = (ZGuiListbox*) pkWnd;
				ZGuiListitem* pkItem = pkListbox->GetItem(iIDListItem);

				if(pkItem != NULL)
				{
					string szFileName = pkItem->GetText();

					char strText[150];

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

						for(int i=0; i<vkDirectories.size(); i++)
						{
							if(vkDirectories[i] == szFileName)
							{
								m_szSearchBoxPath.append("\\");
								m_szSearchBoxPath.append(szFileName);
								printf("hej\n");
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

	ZGuiWnd* pkMainWnd1 = new ZGuiWnd(Rect(0,0,m_pkEdit->m_iWidth,20),NULL,true,ID_MAINWND1);
	pkMainWnd1->SetSkin(GetSkin("menu"));
	pkMainWnd1->SetMoveArea(Rect(0,0,m_pkEdit->m_iWidth,m_pkEdit->m_iHeight));

	Rect rc = Rect(0,0,128,32);

	rc = Rect(0,0,200,200);
	ZGuiCombobox* pkMenuCBox = new ZGuiCombobox(rc, pkMainWnd1, true, iLastIDNr++, 20,
		GetSkin("font"), tex_font_a, GetSkin("menu"), GetSkin("dark_blue"), 
		GetSkin("dark_blue"), GetSkin("menu"), -1);
	pkMenuCBox->SetLabelText("File");
	pkMenuCBox->DisableLabelText();
	pkMenuCBox->AddItem("Quit", IDM_CLOSE);
	pkMenuCBox->AddItem("Open...", IDM_OPEN);
	pkMenuCBox->AddItem("item 3", iLastIDNr++);

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

int Gui::CreateFilePathBox(int x, int y, int w, int h)
{
	if( m_pkEdit->pkGui->GetWindow(ID_FILEPATH_WND) || 
		m_pkEdit->pkGui->GetWindow(ID_FILEPATH_WND_LABEL_PATH) )
	{
		return false;
	}

	ZGuiWnd* pkMainWnd2 = new ZGuiWnd(Rect(x,y,w,h),NULL,false,ID_MAINWND2);
	pkMainWnd2->SetSkin(GetSkin("blue"));
	pkMainWnd2->SetMoveArea(Rect(0,0,m_pkEdit->m_iWidth,m_pkEdit->m_iHeight));

	m_pkEdit->pkGui->AddMainWindow(ID_MAINWND2, pkMainWnd2, m_pkWndProc, false);

	ZGuiWnd* pkMainWindow = m_pkEdit->pkGui->GetMainWindow(ID_MAINWND2);
	if(pkMainWindow == NULL)
		return -1;

	pkMainWindow->Show();

	int iID = ID_FILEPATH_WND;
	int tex_font_a = m_pkEdit->pkTexMan->Load("file:../data/textures/text/font_a.bmp", 0);

	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(0,20,w,h-50),pkMainWindow,true,iID,20,
		GetSkin("font"), tex_font_a, GetSkin("menu_item"), GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"));
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));

	iID = ID_FILEPATH_WND_LABEL_PATH;
	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(0,0,w-20,20), pkMainWindow, true, iID);
	pkLabel->SetSkin(GetSkin("dark_blue"));
	pkLabel->SetTextSkin(GetSkin("font"), tex_font_a);

	iID = ID_FILEPATH_WND_LABEL_FILE;
	ZGuiLabel* pkLabel2 = new ZGuiLabel(Rect(0,h,w,h-20), pkMainWindow, true, iID);
	pkLabel2->SetSkin(GetSkin("dark_blue"));
	pkLabel2->SetTextSkin(GetSkin("font"), tex_font_a);

	iID = ID_FILEPATH_WND_CLOSE;
	ZGuiButton* pkCloseButton = new ZGuiButton(Rect(w-20,0,w,20),pkMainWindow,true,iID);
	pkCloseButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkCloseButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkCloseButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkCloseButton->SetTextSkin(GetSkin("font"), tex_font_a);
	pkCloseButton->SetText("x");
	
	FillPathList(pkListbox, m_szSearchBoxPath);

	m_pkEdit->pkGui->RegisterWindow(pkListbox);
	m_pkEdit->pkGui->RegisterWindow(pkLabel);
	m_pkEdit->pkGui->RegisterWindow(pkLabel2);
	m_pkEdit->pkGui->RegisterWindow(pkCloseButton);

	return iID;
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
		for( int i=0; i<vkFiles.size(); i++)
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

int Gui::CreatePropertyBox()
{
	return 0;
}
