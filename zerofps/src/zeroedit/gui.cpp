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

static bool MENUPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->MenuProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

static bool OPENFILEPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->m_pkFileDlgbox->DlgProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

static bool WORKPANELPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->WorkPanelProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

//////////////////////////

Gui::Gui(ZeroEdit* pkEdit)
{
	m_iScreenCX = pkEdit->m_iWidth / 2;
	m_iScreenCY = pkEdit->m_iHeight / 2;
	m_pkEdit = pkEdit;
	m_pkGui = m_pkEdit->pkGui;
	m_pkFileDlgbox = NULL;
	m_pkWorkPanel = NULL;
	m_uiNumMenuItems = 0;

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

bool Gui::WndProc( ZGuiWnd* pkWindow, unsigned int uiMessage, 
				   int iNumberOfParams, void *pkParams )
{
	Rect rc;

	int iControllID;

	unsigned long flags;
	flags = 0;

	switch(uiMessage)
	{
	case ZGM_COMMAND:
		iControllID = ((int*)pkParams)[0];

		switch(iControllID)
		{
		case ID_FILEPATH_OPEN_BN:
			char cmd[512];
			char path[512];
			
			switch(m_kSearchTask)
			{
			case LOAD_MAP:
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

				sprintf(cmd, "set g_template %s", 
					m_pkFileDlgbox->m_szCurrentFile.c_str()); 
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);	
				break;

			case SAVE_TEMPLATE:
				sprintf(path, "%s", m_pkFileDlgbox->m_szCurrentFile.c_str());
					
				m_pkEdit->pkFps->m_pkConsole->Execute(cmd);	

				sprintf(cmd, "savetemplate %s %s", path, 
					m_pkFileDlgbox->m_szCurrentFile.c_str()); 
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
	}
	return true;
}

bool Gui::WorkPanelProc( ZGuiWnd* pkWindow, unsigned int uiMessage, 
						 int iNumberOfParams, void *pkParams )
{
	switch(uiMessage)
	{
	case ZGM_TCN_SELCHANGE:
		int iPage; iPage = ((int*)pkParams)[0];
		switch(iPage)
		{
		case 1: // Elevation tool
			if(IsButtonChecked("Raise", "ElevationModeRadioGroup"))
				m_pkEdit->m_iMode = RAISE;
			else
			if(IsButtonChecked("Lower", "ElevationModeRadioGroup"))
				m_pkEdit->m_iMode = LOWER;
			else
			if(IsButtonChecked("Smooth", "ElevationModeRadioGroup"))
				m_pkEdit->m_iMode = SMOOTH;
			else
				m_pkEdit->m_iMode = RAISE;
			break;
		case 2: // Paint texture
			m_pkEdit->m_iMode = TEXTURE;
			break;
		case 3: // Add object
			m_pkEdit->m_iMode = ADDOBJECT;
			break;
		}
		break;
	case ZGM_COMMAND:
		int iControllID; iControllID = ((int*)pkParams)[0];
		switch(iControllID)
		{
		case ID_ELEVATIONMODE_RADIOGROUP:	// raise
			m_pkEdit->m_iMode = RAISE;
			break;
		case ID_ELEVATIONMODE_RADIOGROUP+1: // lower
			m_pkEdit->m_iMode = LOWER;
			break;
		case ID_ELEVATIONMODE_RADIOGROUP+2: // smooth
			m_pkEdit->m_iMode = SMOOTH;
			break;
		case ID_PENSIZE_RADIOGROUP:
			m_pkEdit->m_iPencilSize = 4; // small
			break;
		case ID_PENSIZE_RADIOGROUP+1:
			m_pkEdit->m_iPencilSize = 8; // medium
			break;
		case ID_PENSIZE_RADIOGROUP+2:
			m_pkEdit->m_iPencilSize = 14; // large
			break;
		case ID_PENSIZE_RADIOGROUP+3:
			m_pkEdit->m_iPencilSize = 20; // extra-large
			break;
		case ID_LOADMADFILE_BN:
			Object* pkCurObject;
			if((pkCurObject=m_pkEdit->m_pkCurentChild) != NULL)
			{
				Property* pkMadProp;
				if((pkMadProp=pkCurObject->GetProperty("MadProperty")) == NULL)
					if((pkMadProp=pkCurObject->AddProperty("MadProperty")) == NULL)
						break;
				string szMadFile = ((ZGuiListbox*)Get(
					"MadFileList"))->GetSelItem()->GetText();
				pkMadProp->SetValue("m_kMadFile", "../data/mad/" + szMadFile);

				// Plocka bort bollen.
				Property* pkModelProp;
				if((pkModelProp=pkCurObject->GetProperty("ModelProperty")) != NULL)
					pkCurObject->RemoveProperty(pkModelProp);
			}
			break;
		case ID_CREATEMADFILE_BN:
			Object *pkNewObject = new BallObject();					
			pkNewObject->SetPos(m_pkEdit->m_kDrawPos);	
			pkNewObject->SetPos(m_pkEdit->m_kDrawPos);					
			
			pkNewObject->AttachToClosestZone();
			m_pkEdit->m_pkCurentChild=pkNewObject;

			int* pkParams = new int[1];
			pkParams[0] = (int) ID_LOADMADFILE_BN; // control id
			WorkPanelProc(pkWindow, uiMessage,1,pkParams);
			delete[] pkParams;

			((EditPropertyDlg*)GetDlg("PropertyDlg"))->m_pkCurrentChild = pkNewObject;
			UpdatePropertybox();
			break;
		}
		break;

	case ZGM_EN_CHANGE:
		char* szText = NULL;
		int value = -1;
		enum Color { red, green, blue, alhpha, error };
		Color eColor = error; 
		ZGuiWnd* pkEBox = NULL;

		switch(((int*)pkParams)[0])
		{
		case ID_MASKCOLOR_RED_EB:
			szText = (pkEBox = Get("MaskColorREB"))->GetText(); eColor = red; break;
		case ID_MASKCOLOR_GREEN_EB:
			szText = (pkEBox = Get("MaskColorGEB"))->GetText(); eColor = green;break;
		case ID_MASKCOLOR_BLUE_EB: 
			szText = (pkEBox = Get("MaskColorBEB"))->GetText(); eColor = blue; break;
		case ID_MASKCOLOR_ALPHA_EB:
			szText = (pkEBox = Get("MaskColorAEB"))->GetText(); eColor = alhpha; break;
		case ID_DRAWRATE_EB:
			if((szText = (pkEBox = Get("DrawRateEB"))->GetText()))
			{
				bool bBad = false;
				int v = atoi(szText);
				if(v < 0) 
					{ bBad = true; v = 0; } 
				else 
					if(v > 1000) 
					{ v = 1000; bBad = true; }

				if(bBad)
				{
					char szNewText[50];
					sprintf(szNewText, "%i", v);
					pkEBox->SetText(szNewText);
				}
				m_pkEdit->m_fDrawRate = (float) v / 1000;
				szText = NULL;
			}
			break;
		}

		if(szText == NULL)
			break;

		int *res = NULL;
		switch(eColor)
		{
			case red:    res = &(m_pkEdit->m_iMaskColorR = atoi(szText)); break;
			case green:  res = &(m_pkEdit->m_iMaskColorG = atoi(szText)); break;
			case blue:   res = &(m_pkEdit->m_iMaskColorB = atoi(szText)); break;
			case alhpha: res = &(m_pkEdit->m_iMaskColorA = atoi(szText)); break;
		}

		if(res != NULL)
		{
			char* szNewText = NULL;

			if(*res < 0)   
			{
				*res = 0; szNewText = new char[20]; 
				sprintf(szNewText, "%i", *res);
			}
			if(*res > 255) 
			{
				*res = 255; szNewText = new char[20]; 
				sprintf(szNewText, "%i", *res);
			}

			if(szNewText != NULL)
				pkEBox->SetText(szNewText);
		}
		break;
	}
	return true;
}

bool Gui::MenuProc( ZGuiWnd* pkWindow, unsigned int uiMessage, 
				    int iNumberOfParams, void *pkParams )
{
	Rect rc;

	unsigned long flags;
	flags = 0;

	switch(uiMessage)
	{
	case ZGM_LBUTTONDOWN:

		m_pkEdit->pkFps->DevPrint_Show(true);

		unsigned int i;
		for(i=0; i<m_uiNumMenuItems; i++)
			if(m_pkMenuInfo[i].cb->GetListbox()->IsVisible())
			{
				m_pkEdit->pkFps->DevPrint_Show(false);
				break;
			}

		break;

	case ZGM_CBN_SELENDOK:
		{
			int iID = ((int*)pkParams)[0];
			ZGuiWnd *win = ((ZGuiWnd*)m_pkGui->GetWindow(iID));

			if(win)
			{
				ZGuiCombobox *cbox = (ZGuiCombobox*) win;
				int index = ((int*)pkParams)[1];
				RunMenuCommand(cbox, index);
			}
		}
		break;
	}
	return true;
}


bool Gui::CreateWindows()
{	
	CreateMenu(m_pkEdit->pkIni, "../data/gui_resource_files/menu.txt");
	CreateTestWnd();

	DlgBox * pkEditPropBox = new EditPropertyDlg(this, 
		m_pkEdit->pkPropertyFactory, 
		m_pkEdit->pkObjectMan, 
		m_pkEdit->m_pkCurentChild, 
		m_pkEdit->pkInput, MAINWINPROC);

	if(!CreateWorkPanel())
	{
		printf("Failed to create work panel!\n");
		return false;
	}

	m_kDialogs.insert(map<string,DlgBox*>::value_type(
		string("PropertyDlg"), pkEditPropBox));

	return true;
}

bool Gui::InitSkins()
{
	int piss = m_pkEdit->pkTexMan->Load("file:../data/textures/piss.bmp", 0); // första misslyckas, I don't know...
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
	int arrow_prev_up = m_pkEdit->pkTexMan->Load("file:../data/textures/prev_arrow_up.bmp", 0);
	int arrow_prev_down = m_pkEdit->pkTexMan->Load("file:../data/textures/prev_arrow_down.bmp", 0);
	int arrow_next_up = m_pkEdit->pkTexMan->Load("file:../data/textures/next_arrow_up.bmp", 0);
	int arrow_next_down = m_pkEdit->pkTexMan->Load("file:../data/textures/next_arrow_down.bmp", 0);
	int cbox_off = m_pkEdit->pkTexMan->Load("file:../data/textures/checkbox_off.bmp",0);
	int cbox_on = m_pkEdit->pkTexMan->Load("file:../data/textures/checkbox_on.bmp",0);
	int slider = m_pkEdit->pkTexMan->Load("file:../data/textures/slider.bmp",0);
	int slider_a = m_pkEdit->pkTexMan->Load("file:../data/textures/slider_a.bmp",0);

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
		new ZGuiSkin(128, 128, 128, 92, 92, 92, 1)) ); 
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
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("arrow_prev_up"), 
		new ZGuiSkin(arrow_prev_up, false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("arrow_prev_down"), 
		new ZGuiSkin(arrow_prev_down, false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("arrow_next_up"), 
		new ZGuiSkin(arrow_next_up, false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("arrow_next_down"), 
		new ZGuiSkin(arrow_next_down, false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("tabbn_back"), 
		new ZGuiSkin(128,128,128,92,92,92,1)));
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("tabbn_front"), 
		new ZGuiSkin(214,211,206,0,0,0,0)));
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("cbox_off"), 
		new ZGuiSkin(cbox_off,false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("cbox_on"), 
		new ZGuiSkin(cbox_on,false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("slider"), 
		new ZGuiSkin(slider, slider_a, false)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("slider_bk_r"), 
		new ZGuiSkin(255, 0, 0, 0, 0, 0, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("slider_bk_g"), 
		new ZGuiSkin(0, 255, 0, 0, 0, 0, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("slider_bk_b"), 
		new ZGuiSkin(0, 0, 255, 0, 0, 0, 1)) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("slider_bk_a"), 
		new ZGuiSkin(255, 0, 255, 0, 0, 0, 1)) ); 

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

ZGuiTabCtrl* Gui::CreateTabbedDialog(char* szName, int iWndID, int iMainWndID, 
									int x, int y, int w, int h, 
									vector<string> kTabNames, 
									ZGuiCallBack pkProc)
{
	ZGuiTabCtrl* pkTab = new ZGuiTabCtrl(Rect(x,y,x+w,y+h),NULL,true,iWndID);
	pkTab->SetSkin(GetSkin("menu"));
	pkTab->SetFont(m_pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT));
	pkTab->SetNextButtonSkin( GetSkin("arrow_next_up"), 
		GetSkin("arrow_next_down"), GetSkin("arrow_next_up"));
	pkTab->SetPrevButtonSkin( GetSkin("arrow_prev_up"), 
		GetSkin("arrow_prev_down"), GetSkin("arrow_prev_up"));
	pkTab->InsertTabSkin(0, GetSkin("tabbn_back"));
	pkTab->InsertTabSkin(1, GetSkin("tabbn_front"));
	
	for(unsigned int i=0; i<kTabNames.size(); i++)
		pkTab->InsertPage(i, (char*) kTabNames[i].c_str(), 0);

	pkTab->SetCurrentPage(0);

	m_pkGui->AddMainWindow(iMainWndID,pkTab,szName,pkProc,false);

	return pkTab;
}

ZGuiButton* Gui::CreateButton(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
							  int h, char *pkName)
{
	ZGuiButton* pkButton = new ZGuiButton(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonHighLightSkin(GetSkin("bn_focus"));
	pkButton->SetButtonDownSkin(GetSkin("bn_down"));
	pkButton->SetButtonUpSkin(GetSkin("bn_up"));
	pkButton->SetText(pkName);
	pkButton->SetGUI(m_pkGui);

	return pkButton;
}

ZGuiCheckbox* Gui::CreateCheckbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
								  int h, bool bChecked, char *pkText, char* pkResName)
{
	ZGuiCheckbox* pkButton = new ZGuiCheckbox(Rect(x,y,x+w,y+h),pkParent,true,iID);
	pkButton->SetButtonCheckedSkin(GetSkin("cbox_on"));
	pkButton->SetButtonUncheckedSkin(GetSkin("cbox_off"));
	pkButton->SetText(pkText);
	pkButton->SetGUI(m_pkGui);

	if(bChecked == true)
		pkButton->CheckButton();

	if(pkResName != NULL)
		Register(pkButton, pkResName);

	return pkButton;
}

int Gui::CreateRadiobuttons(ZGuiWnd* pkParent, vector<string>& vkNames,
							 char* strRadioGroupName, int start_id, int x, 
							 int y, int size)
{
	int rbn_a = m_pkEdit->pkTexMan->Load("file:../data/textures/radiobn_a.bmp", 0);

	int GroupID = start_id;
	ZGuiRadiobutton* pkPrev = NULL;
	Rect rc = Rect(x,y,x+size,y+size);
	const int antal = vkNames.size();
	int iRadiobuttonHeight = 20;

	for(int i=0; i<antal; i++)
	{
		ZGuiRadiobutton* pkGroupbutton = new ZGuiRadiobutton(rc,pkParent,
			start_id+i,start_id,strRadioGroupName,pkPrev,true);
		pkGroupbutton->SetButtonUnselectedSkin(GetSkin("rbn_down"));
		pkGroupbutton->SetButtonSelectedSkin(GetSkin("rbn_up"));
		pkGroupbutton->SetGUI(m_pkGui);
		pkGroupbutton->SetText((char*)vkNames[i].c_str(),true);
		
		if(pkPrev == NULL)
		{
			Register(pkGroupbutton, strRadioGroupName);
		}

		pkPrev = pkGroupbutton;
		rc = rc.Move(0,iRadiobuttonHeight);
	}

	return iRadiobuttonHeight*antal; // return height of all radiobuttons.
}

ZGuiListbox* Gui::CreateListbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, int h)
{
	ZGuiListbox* pkListbox = new ZGuiListbox(Rect(x,y,x+w,y+h),pkParent,true,iID,20,
		GetSkin("menu_item"), GetSkin("menu_item_sel"), GetSkin("menu_item_hl"));
	pkListbox->SetSkin( GetSkin("dark_blue") );
	pkListbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	pkListbox->SetGUI(m_pkGui);
	return pkListbox;
}

ZGuiCombobox* Gui::CreateCombobox(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
								  int h, bool bMenu)
{
	ZGuiCombobox* pkCombobox = new ZGuiCombobox(Rect(x,y,x+w,y+h), pkParent, true, iID, 
		20,GetSkin("menu"),GetSkin("dark_blue"),GetSkin("dark_blue"),GetSkin("menu"));

	if(bMenu == false)
	{
		pkCombobox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
			GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	}

	pkCombobox->SetGUI(m_pkGui);

	return pkCombobox;
}

ZGuiTextbox* Gui::CreateTextbox(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
								int h, bool bMulitLine, char* szText, char* szRegName,
								bool bNumberOnly)
{
	ZGuiTextbox* pkTextbox = new ZGuiTextbox(Rect(x,y,x+w,y+h), pkParent, true, 
		iID, bMulitLine);
	pkTextbox->SetSkin(GetSkin("white"));
	pkTextbox->SetScrollbarSkin(GetSkin("menu_item_sel"), 
		GetSkin("menu_item_hl"), GetSkin("menu_item_hl"));
	pkTextbox->SetGUI(m_pkGui);
	pkTextbox->SetFont(m_pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT));

	if(szText != NULL)
		pkTextbox->SetText(szText);

	if(szRegName != NULL)
		Register(pkTextbox, szRegName);

	if(bNumberOnly)
		pkTextbox->m_bNumberOnly = true;

	return pkTextbox;
}

ZGuiLabel* Gui::CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
							int h, char* strText)
{
	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h), pkParent, true, iID);
	
	if(strText)
		pkLabel->SetText(strText);

	pkLabel->SetGUI(m_pkGui);

	return pkLabel;
}

bool Gui::CheckRadioButton(char* szGroupName, char *szButtonText)
{
	ZGuiWnd* pkGroup = Get(szGroupName);

	if(pkGroup == NULL)
		return false;

	ZGuiRadiobutton* pkSearchButton = (ZGuiRadiobutton*) pkGroup;

	while(1)
	{
		char* szText = pkSearchButton->GetButton()->GetText();

		if(szText)
		{
			if(strcmp(szText, szButtonText) == 0)
				pkSearchButton->GetButton()->CheckButton();
			else
				pkSearchButton->GetButton()->UncheckButton();
		}

		pkSearchButton = pkSearchButton->GetNext();
		if(pkSearchButton == NULL)
			break;
	}

	return true;
}

void Gui::AddItemsToList(ZGuiWnd *pkWnd, bool bCombobox, char **items, int iNumber, 
						 bool bSelectLast)
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

void Gui::AddItemToList(ZGuiWnd *pkWnd, bool bCombobox, const char *item, int index, 
						bool bSelect)
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
	return m_pkGui->RegisterWindow(pkWnd, strName);
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

void Gui::UpdatePropertybox()
{
	DlgBox* pkPropDlg = GetDlg("PropertyDlg");
	if(pkPropDlg == NULL)
		return;

	if(pkPropDlg->IsOpen())
	{
		pkPropDlg->Close(false);
		pkPropDlg->Open();
	}
}

void Gui::ClosePropertybox()
{
	DlgBox* pkPropDlg = GetDlg("PropertyDlg");
	if(pkPropDlg == NULL)
		return;

	if(pkPropDlg && pkPropDlg->IsOpen())
	{
		m_pkGui->ShowMainWindow( Get("PropertyDlg"), false);
	}
}

void Gui::OpenPropertybox()
{
	DlgBox* pkPropDlg = GetDlg("PropertyDlg");
	if(pkPropDlg == NULL)
		return;

	if(pkPropDlg)
	{
		if(pkPropDlg->IsOpen())
			return;
	}

	m_pkGui->ShowMainWindow( Get("PropertyDlg"), false);
}

bool Gui::HaveFocus()
{
	static bool bGiveGuiFocus = false;
	static bool bWindowClicked = false;
	
	bool bMouseHoverWnd = m_pkGui->MouseHoverWnd();

	// Move window?
	if(bMouseHoverWnd && m_pkEdit->pkInput->Pressed(MOUSELEFT) && !bWindowClicked)
	{
		bGiveGuiFocus = true;
		bWindowClicked = true;
	}

	// Clicking outside window
	if(m_pkEdit->pkInput->Pressed(MOUSELEFT) && !bWindowClicked)
	{
		bGiveGuiFocus = false;
		bWindowClicked = false;
	}

	// Releasing mouse button after moving window
	if(bWindowClicked == true && !m_pkEdit->pkInput->Pressed(MOUSELEFT))
	{
		bGiveGuiFocus = false;
		bWindowClicked = false;
	}

	if(ZGuiWnd::m_pkFocusWnd != NULL)
	{
		// Type text?
		if( typeid(*ZGuiWnd::m_pkFocusWnd) == typeid(ZGuiTextbox) )
		{
			bGiveGuiFocus = true;
		}
	}

	return bGiveGuiFocus;
}

void Gui::CreateTestWnd()
{
	int id = 5000;
	int w = 400, h = 500;
	int x = m_pkEdit->m_iWidth/2 - w/2;
	int y = m_pkEdit->m_iHeight/2 - h/2;

	ZGuiWnd* pkWnd = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,
		false,id++);

	pkWnd->SetSkin(new ZGuiSkin(0,0,0,0,0,0,0));

	pkWnd->SetMoveArea(Rect(0,0,1024,768));

	m_pkGui->AddMainWindow(id++,pkWnd,"TestWnd",MAINWINPROC,false);

	ZGuiTreebox* pkTreebox = new ZGuiTreebox(Rect(50,50,50+300,50+400), 
		pkWnd, true, id++);

	pkTreebox->AddItem(NULL, "Apa", 0);

	pkTreebox->SetSkin(new ZGuiSkin(255,255,255,0,0,0,0));

	m_pkGui->RegisterWindow(pkTreebox, "TestTreeBox");
}

bool Gui::CreateMenu(ZFIni* pkIni, char* szFileName)
{
	ZGuiFont* pkFont = m_pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT);
	if(pkFont == NULL)
	{
		printf("Failed to find font for menu!\n");
		return false;
	}

	ZGuiWnd* pkMenu = new ZGuiWnd(Rect(0,0,1024,20),NULL,true,ID_MAINWND_MENU);
	pkMenu->SetSkin(GetSkin("menu"));
	pkMenu->SetZValue(102321);

	m_pkGui->AddMainWindow(IDM_MENU_WND, pkMenu, "MainMenu", MENUPROC, true);

	if(!pkIni->Open(szFileName, false))
	{
		cout << "Failed to load ini file for menu!\n" << endl;
		return false;
	}

	vector<string> akSections;
	pkIni->GetSectionNames(akSections);

	unsigned int uiNumSections = akSections.size();
	
	// No items in file.
	if(uiNumSections < 1)
		return true;

	Rect rcMenu;
	unsigned int i=0, iMenuOffset=0, iMenuWidth=0, iMenuIDCounter=45781;
	char szParentName[50];

	// Skapa alla parents
	for(i=0; i<uiNumSections; i++)
	{
		char* parent = pkIni->GetValue(akSections[i].c_str(), "Parent");
		if(parent == NULL)
			continue;

		if(strcmp(parent, "NULL") == 0)
		{
			char szTitle[50];
			sprintf(szTitle, " %s", pkIni->GetValue(akSections[i].c_str(), "Title"));
			iMenuWidth = pkFont->GetLength(szTitle) + 6; // move rc right

			rcMenu = Rect(iMenuOffset,0,iMenuOffset+iMenuWidth,20);

			ZGuiCombobox* pkMenuCBox = new ZGuiCombobox(rcMenu,pkMenu,
				true,iMenuIDCounter++,20,GetSkin("menu"),GetSkin("dark_blue"),
				GetSkin("dark_blue"), GetSkin("menu"));

			pkMenuCBox->SetGUI(m_pkGui);
			pkMenuCBox->SetLabelText(szTitle);
			pkMenuCBox->SetNumVisibleRows(1);
			pkMenuCBox->IsMenu(true);

			iMenuOffset += iMenuWidth;
			rcMenu = rcMenu.Move(iMenuOffset,0);

			m_pkGui->RegisterWindow(pkMenuCBox, (char*)akSections[i].c_str());
		}
	}

	ZGuiWnd* pkParent;
	vector<MENU_INFO> kTempVector;

	// Skapa alla childrens.
	char szCommando[512];
	int item_counter = 0;
	for(i=0; i<uiNumSections; i++)
	{
		char* parent = pkIni->GetValue(akSections[i].c_str(), "Parent");
		if(parent == NULL)
			continue;

		strcpy(szParentName, parent);
		if(strcmp(szParentName, "NULL") != 0)
		{
			pkParent = m_pkEdit->pkGuiMan->Wnd(string(szParentName));

			if(pkParent != NULL)
			{
				char szTitle[50];
				sprintf(szTitle, " %s", pkIni->GetValue(akSections[i].c_str(), "Title"));
				((ZGuiCombobox*) pkParent)->AddItem(szTitle, item_counter++);

				MENU_INFO mi;
				mi.cb = (ZGuiCombobox*) pkParent;
				mi.iIndex = item_counter-1;
				char* szCmd = pkIni->GetValue(akSections[i].c_str(), "Cmd");
				if(szCmd != NULL)
					strcpy(szCommando, szCmd);
				else
					strcpy(szCommando, "No commando!");

				mi.szCommando = new char[strlen(szCommando)+1];
				strcpy(mi.szCommando, szCommando);
				kTempVector.push_back(mi);
			}
		}
	}

	// Copy from tempvektor.
	m_uiNumMenuItems = kTempVector.size();
	m_pkMenuInfo = new MENU_INFO[m_uiNumMenuItems];
	for(i=0; i<m_uiNumMenuItems; i++)
	{
		m_pkMenuInfo[i].cb = kTempVector[i].cb;
		m_pkMenuInfo[i].iIndex = kTempVector[i].iIndex;
		m_pkMenuInfo[i].szCommando = new char[strlen(kTempVector[i].szCommando)+1];
		strcpy(m_pkMenuInfo[i].szCommando, kTempVector[i].szCommando);
		delete[] kTempVector[i].szCommando;
	}

	return true;
}


void Gui::RunMenuCommand(ZGuiCombobox* pkCombox, int iIndex)
{
	if(m_pkMenuInfo == NULL || m_uiNumMenuItems == 0)
		return;

	for(unsigned int i=0; i<m_uiNumMenuItems; i++)
	{
		if( m_pkMenuInfo[i].cb == pkCombox && 
			m_pkMenuInfo[i].iIndex == iIndex)
		{
			char* cmd = m_pkMenuInfo[i].szCommando;
			m_pkEdit->pkFps->m_pkConsole->Execute(cmd);
			break;
		}
	}
}

void Gui::CloseMenu()
{
	for(unsigned int i=0; i<m_uiNumMenuItems; i++)
		if( m_pkMenuInfo[i].cb)
			m_pkMenuInfo[i].cb->GetListbox()->Hide();
}

ZGuiWnd* Gui::GetMenu()
{
	return Get("MainMenu");
}

bool Gui::OpenDlg(string strResName)
{
	map<string,DlgBox*>::iterator it = m_kDialogs.find(strResName);
	if(it != m_kDialogs.end())
	{
		it->second->Open(-1,-1);
	}

	return true;
}

DlgBox* Gui::GetDlg(string strResName)
{
	map<string,DlgBox*>::iterator res = m_kDialogs.find(strResName);

	if(res != m_kDialogs.end())
		return res->second;

	return NULL;
}

bool Gui::OpenFileDlg(SEARCH_TASK eTask)
{
	unsigned long flags;
	flags = 0;

	if(m_pkFileDlgbox)
	{
		delete m_pkFileDlgbox;
		m_pkFileDlgbox = NULL;
	}

	switch(eTask)
	{
	case LOAD_MAP:
		flags = DIRECTORIES_ONLY;
		break;

	case LOAD_TEMPLATE:
		break;

	case SAVE_TEMPLATE:
		flags = SAVE_FILES; 
		break;

	default:
		return false;
		break;
	}

	m_pkFileDlgbox = new FileOpenDlg(m_pkGui, 
		m_pkEdit->pkFps->m_pkBasicFS, MAINWINPROC, flags);
	m_pkFileDlgbox->Create(100,100,500,500,OPENFILEPROC);

	m_kSearchTask = eTask;

	return true;
}

bool Gui::CreateWorkPanel()
{
	int w = 224, y = 20, h = 224, x = m_pkEdit->m_iWidth-w;

	vector<string> akTabNames;
	akTabNames.push_back("Marker");
	akTabNames.push_back("Elevation");
	akTabNames.push_back("Texture");
	akTabNames.push_back("Object");

	m_pkWorkPanel = CreateTabbedDialog("WorkPanel",12314124,2321323,
		x,y,w,h,akTabNames,WORKPANELPROC);

	ZGuiWnd* pkPage;
	vector<string> vkNames;

	// Create page 1: - Marker
	pkPage = m_pkWorkPanel->GetPage(0);

	vkNames.clear();
	vkNames.push_back("Small");
	vkNames.push_back("Medium");
	vkNames.push_back("Large");
	vkNames.push_back("Extra-Large");

	CreateLabel(pkPage, 0, 5, 5, 50, 20, "Pen size");
	int iHeight = CreateRadiobuttons(pkPage, vkNames, 
		"PenSizeRadioGroup", ID_PENSIZE_RADIOGROUP, 5, 25, 16);
	CheckRadioButton("PenSizeRadioGroup", "Medium");

	char szRate[50]; sprintf(szRate, "%i", (int) (1000.0f * m_pkEdit->m_fDrawRate));
	CreateLabel(pkPage, 0, 5, 25+iHeight, 50, 20, "Draw rate:");
	CreateTextbox(pkPage, ID_DRAWRATE_EB, 80, 25+iHeight, 30, 20, 
		false, szRate, "DrawRateEB");

	// Create page 2: - Elevation tool
	pkPage = m_pkWorkPanel->GetPage(1);

	vkNames.clear();
	vkNames.push_back("Raise");
	vkNames.push_back("Lower");
	vkNames.push_back("Smooth");

	iHeight = CreateRadiobuttons(pkPage, vkNames, 
		"ElevationModeRadioGroup", ID_ELEVATIONMODE_RADIOGROUP, 5, 5, 16);
	CheckRadioButton("ElevationModeRadioGroup", "Raise");

	CreateCheckbox(pkPage, ID_AUTOSMOOTHGROUND_CHB, 5, 5+iHeight, 16, 16, true,
		"Auto-smooth", "AutoSmoothGroundChB");

	// Create page 3: - Paint terrain
	pkPage = m_pkWorkPanel->GetPage(2);

	CreateLabel(pkPage, 0, 5,  50, 10, 16, "R");
	CreateLabel(pkPage, 0, 5,  70, 10, 16, "G");
	CreateLabel(pkPage, 0, 5,  90, 10, 16, "B");
	CreateLabel(pkPage, 0, 5, 110, 10, 16, "A");
	char aszColor[4][50];
	sprintf(aszColor[0], "%i", m_pkEdit->m_iMaskColorR);
	sprintf(aszColor[1], "%i", m_pkEdit->m_iMaskColorG);
	sprintf(aszColor[2], "%i", m_pkEdit->m_iMaskColorB);
	sprintf(aszColor[3], "%i", m_pkEdit->m_iMaskColorA);
	CreateTextbox(pkPage, ID_MASKCOLOR_RED_EB,   25,  50, 30, 16, false, 
		aszColor[0], "MaskColorREB", true); 
	CreateTextbox(pkPage, ID_MASKCOLOR_GREEN_EB, 25,  70, 30, 16, false, 
		aszColor[1], "MaskColorGEB", true); 
	CreateTextbox(pkPage, ID_MASKCOLOR_BLUE_EB,  25,  90, 30, 16, false, 
		aszColor[2], "MaskColorBEB", true);
	CreateTextbox(pkPage, ID_MASKCOLOR_ALPHA_EB, 25, 110, 30, 16, false, 
		aszColor[3], "MaskColorAEB", true);

	CreateSlider(pkPage, 12345, 75,  52, 100, 20, true, NULL, "slider_bk_r");
	CreateSlider(pkPage, 12346, 75,  72, 100, 20, true, NULL, "slider_bk_g");
	CreateSlider(pkPage, 12347, 75,  92, 100, 20, true, NULL, "slider_bk_b");
	CreateSlider(pkPage, 12348, 75, 112, 100, 20, true, NULL, "slider_bk_a");

	CreateLabel(pkPage, 0, 5, 5, 209, 20, "Map");
	ZGuiCombobox* pkTextureCB = CreateCombobox(pkPage, ID_TERRAINTEXTURE_CB, 
		50, 5, 150, 20, false);
	pkTextureCB->AddItem("Texture 1", 0);
	pkTextureCB->AddItem("Texture 2", 1);
	pkTextureCB->AddItem("Texture 3", 2);

	// Create page 2: - Add object
	pkPage = m_pkWorkPanel->GetPage(3);

	CreateLabel(pkPage, 0, 5, 5, 50, 20, "MAD objects");
	ZGuiListbox* pkMadList = CreateListbox(pkPage, ID_MADOBJECTS_LB, 5, 25, 209, 140);
	Register(pkMadList, "MadFileList");
	
	vkNames.clear(); 
	m_pkEdit->pkBasicFS->ListDir(&vkNames, "../data/mad", false);
	for(unsigned int i=1; i<vkNames.size(); i++)
		AddItemToList(pkMadList, false, vkNames[i].c_str(), i, false);

	CreateButton(pkPage, ID_LOADMADFILE_BN, 5, 170, 50, 20, "Change");
	CreateButton(pkPage, ID_CREATEMADFILE_BN, 60, 170, 50, 20, "Create");
	return true;
}

bool Gui::IsButtonChecked(char *szName, char* szGroupName)
{
	if(szGroupName != NULL)
	{
		ZGuiWnd* pkGroup = Get(szGroupName);

		if(pkGroup == NULL)
			return false;

		ZGuiRadiobutton* pkSearchButton = (ZGuiRadiobutton*) pkGroup;
		
		while(1)
		{
			char* szText = pkSearchButton->GetButton()->GetText();

			if(szText)
				if(strcmp(szText, szName) == 0)
					return pkSearchButton->GetButton()->IsChecked();

			pkSearchButton = pkSearchButton->GetNext();
			if(pkSearchButton == NULL)
				break;
		}

		return false;
	}

	ZGuiWnd* pkButton = Get(szName);

	if(pkButton == NULL)
		return false;

	return ((ZGuiCheckbox*) pkButton)->IsChecked();
}

bool Gui::CreateSlider(ZGuiWnd *pkParent, int iID, int x, int y, int w, int h, 
					   bool bHorizontal, char *szResName, char* szBkSkin)
{
	Rect rc;

	if(bHorizontal)
	{
		rc.Left = x;
		rc.Top  = y;
		rc.Right = x+16;
		rc.Bottom = y+16;
	}

	ZGuiButton* pkButton = new ZGuiButton(rc,pkParent,true,iID);
	pkButton->SetMoveArea(pkButton->GetScreenRect()+Rect(0,0,w,0));
	pkButton->SetButtonHighLightSkin(GetSkin("slider"));
	pkButton->SetButtonDownSkin(GetSkin("slider"));
	pkButton->SetButtonUpSkin(GetSkin("slider"));
	pkButton->SetGUI(m_pkGui);

	rc.Left += 8;
	rc.Right += w-8;
	rc.Top = y;
	rc.Bottom = y+4;

	ZGuiLabel* pkLabel = new ZGuiLabel(rc,pkParent,true,0);
	pkLabel->SetSkin(GetSkin(szBkSkin));
	pkLabel->SetGUI(m_pkGui);

	return true;
}
