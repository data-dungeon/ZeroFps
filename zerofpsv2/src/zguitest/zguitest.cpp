#include "zguitest.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"

ZGuiTest g_kGuiTest("GuiTest",0,0,0);

static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ) 
{
	switch(msg)
	{
	case ZGM_COMMAND:
		g_kGuiTest.OnCommand(((int*)params)[0], (((int*)params)[1] == 1) ? true : false, win);
		break;
	case ZGM_LBUTTONDOWN:
		break;
	case ZGM_LBUTTONUP:
		break;
	case ZGM_RBUTTONDOWN:    
		break;
	case ZGM_RBUTTONUP:
		break;
	case ZGM_LBUTTONDBLCLK:
		break;
	case ZGM_RBUTTONDBLCLK:
		break;
	case ZGM_MOUSEMOVE:
		break;
	case ZGM_SCROLL:
		break;
	case ZGM_CBN_SELENDOK:
		break;
	case ZGM_KEYPRESS:	
		g_kGuiTest.OnKeyDown(((int*)params)[0], win);
		break;

	}
	return true;
}

ZGuiTest::ZGuiTest(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC)
{ 
	m_iCursorRangeDiffX=m_iCursorRangeDiffY=0;
	m_pkFocusWnd = m_pkResizeWnd = m_pkMoveWnd = NULL;
	m_eEditMode = Move;
}

void ZGuiTest::OnInit()
{
	glEnable(GL_LIGHTING );

	pkLight->SetLighting(true);

	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,100);	

	// create gui script
	GuiAppLua::Init(&g_kGuiTest, pkScript);

	InitializeGui(pkGui, pkTexMan, pkScript, pkGuiMan, 
		"data/textures/text/small.bmp",
		"data/script/gui/gui_res_edit.lua");

	SDL_ShowCursor(SDL_DISABLE);

	pkGui->SetCursor(0,0, pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);

	CreateUI();

	pkFps->m_bClientMode = true;

}

void ZGuiTest::OnIdle()
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();	
	pkFps->UpdateCamera(); 	

	int iKey = pkInput->GetQueuedKey();

	if(iKey == KEY_SPACE)
	{
		if(m_pkToolbar->IsVisible())
			m_pkToolbar->Hide();
		else
			m_pkToolbar->Show();

		if(m_pkEditbar->IsVisible())
			m_pkEditbar->Hide();
		else
			m_pkEditbar->Show();
	}

	int x,y;
	pkInput->MouseXY(x,y);

	static s_bLeftButtonPressed = false;

	if(pkInput->Pressed(MOUSELEFT))
	{
		if(!s_bLeftButtonPressed)
		{
			OnMouseClick(false, x, y);
			s_bLeftButtonPressed = false;
		}

		s_bLeftButtonPressed = true;
	}
	else
	{
		if(s_bLeftButtonPressed)
		{
			OnMouseClick(true, x, y);
			s_bLeftButtonPressed = false;
		}
	}

	if(m_pkMoveWnd && m_eEditMode == Move)
	{
		int screenpos_x = x-m_iCursorRangeDiffX;
		int screenpos_y = y-m_iCursorRangeDiffY;

		Rect rc = m_pkMoveWnd->GetScreenRect();

		if(screenpos_x > 800 - rc.Width())
			screenpos_x = 800 - rc.Width();

		if(screenpos_x < 0)
			screenpos_x = 0;

		if(screenpos_y > 600 - rc.Height())
			screenpos_y = 600 - rc.Height();

		if(screenpos_y < 0)
			screenpos_y = 0;

		m_pkMoveWnd->SetPos(screenpos_x, screenpos_y,true,true); 

		SetTextInt("PosXTextbox", screenpos_x);
		SetTextInt("PosYTextbox", screenpos_y);
		SetTextInt("WidthTextbox", rc.Width());
		SetTextInt("HeightTextbox", rc.Height());
	}

	if(m_pkResizeWnd && m_eEditMode == Resize )
	{
		Rect rc = m_pkResizeWnd->GetScreenRect();
		int w = rc.Width(), h = rc.Height();
		int screen_x = x, screen_y = y;

		switch(m_eCurrentResizeType)
		{
		case RightSide:
			screen_x = rc.Left; screen_y = rc.Top;
			w = x - rc.Left;	if(w < 2) w = 2;
			m_pkResizeWnd->Resize(w,h,true); 
			break;

		case BottomSide:
			screen_x = rc.Left; screen_y = rc.Top;
			h = y - rc.Top;	if(h < 2) h = 2;
			m_pkResizeWnd->Resize(w,h,true); 
			break;

		case LeftSide:
			screen_y = rc.Top;
			w = rc.Right - x;	if(w < 2) w = 2; h = rc.Height();
			m_pkResizeWnd->Resize(w,h,true); 
			if(screen_x > rc.Right-2) screen_x = rc.Right-2;
			m_pkResizeWnd->SetPos(screen_x,rc.Top,true, true); 
			break;

		case TopSide:
			screen_x = rc.Left;
			w = rc.Width(); h = rc.Bottom - y;	if(h < 2) h = 2;
			m_pkResizeWnd->Resize(w,h,true); 
			if(screen_y > rc.Bottom-2) screen_y = rc.Bottom-2;
			m_pkResizeWnd->SetPos(rc.Left,screen_y,true, true); 
			break;
		}

		SetTextInt("PosXTextbox", screen_x);
		SetTextInt("PosYTextbox", screen_y);
		SetTextInt("WidthTextbox", w);
		SetTextInt("HeightTextbox", h);
	}

	if(m_pkFocusWnd)
	{
		Rect rc = m_pkFocusWnd->GetScreenRect();

		pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Right, rc.Top));
		pkGui->DrawLine(Point(rc.Left, rc.Bottom), Point(rc.Right, rc.Bottom));
		pkGui->DrawLine(Point(rc.Left, rc.Top), Point(rc.Left, rc.Bottom));
		pkGui->DrawLine(Point(rc.Right, rc.Top), Point(rc.Right, rc.Bottom));
	}

}

void ZGuiTest::OnHud(void) 
{	
	pkFps->DevPrintf("common","Active Propertys: %d",pkObjectMan->GetActivePropertys());	
	pkFps->DevPrintf("common", "Fps: %f",pkFps->m_fFps);	
	pkFps->DevPrintf("common","Avrage Fps: %f",pkFps->m_fAvrageFps);			
	
	pkFps->m_bGuiMode = false;
	pkFps->ToggleGui();
}

void ZGuiTest::CreateUI()
{
	CreateWnd(Wnd, "Toolbar", "", "", 800/2-300/2, 600/2-100/2, 300, 110, 0);
	(m_pkToolbar = GetWnd("Toolbar"))->SetMoveArea(Rect(0,0,800,600),true);

	CreateWnd(Label, "NameLabel",  "Toolbar",  "Name:", 4,  4, 25, 20, 0);
	CreateWnd(Textbox, "WndNameTextbox", "Toolbar", "Nonmame", 40,  4, 150, 20, 0);

	CreateWnd(Label, "TextLabel",  "Toolbar",  "Title:", 4,  28, 25, 20, 0);
	CreateWnd(Textbox, "WndTitleTextbox", "Toolbar", "Nonmame", 40,  28, 150, 20, 0);

	CreateWnd(Combobox, "SelectWndType", "Toolbar", "", 194, 28, 100, 16, 0);
	AddListItem("SelectWndType", "Wnd");
	AddListItem("SelectWndType", "Button");
	AddListItem("SelectWndType", "Checkbox");
	AddListItem("SelectWndType", "Combobox");
	AddListItem("SelectWndType", "Label");
	AddListItem("SelectWndType", "Listbox");
	AddListItem("SelectWndType", "Radiobutton");
	AddListItem("SelectWndType", "Scrollbar");
	AddListItem("SelectWndType", "Slider");
	AddListItem("SelectWndType", "TabControl");
	AddListItem("SelectWndType", "Textbox");
	AddListItem("SelectWndType", "Treebox");

	CreateWnd(Button, "ToolbarOK", "Toolbar", "OK", 194, 4, 100, 20, 0);

	CreateWnd(Label, "PosXLabel",   "Toolbar", "X", 4,  54, 20, 20, 0);
	CreateWnd(Textbox, "PosXTextbox",   "Toolbar", "100", 20,  54, 40, 20, 0);

	CreateWnd(Label, "PosYLabel",   "Toolbar", "Y", 65, 54, 20, 20, 0);
	CreateWnd(Textbox, "PosYTextbox",   "Toolbar", "100", 85, 54, 40, 20, 0);
	
	CreateWnd(Label, "WidthLabel",  "Toolbar",  "W", 130,  54, 20, 20, 0);
	CreateWnd(Textbox, "WidthTextbox",  "Toolbar", "222", 150,  54, 40, 20, 0);

	CreateWnd(Label, "HeightLabel", "Toolbar",  "H", 195, 54, 20, 20, 0);
	CreateWnd(Textbox, "HeightTextbox", "Toolbar", "222", 215, 54, 40, 20, 0);

	//
	// Create edit bar
	//

	CreateWnd(Wnd, "Editbar", "", "", 800-200, 600-200, 200, 22, 0);
	(m_pkEditbar = GetWnd("Editbar"))->SetMoveArea(Rect(0,0,800,600),true);

	CreateWnd(Checkbox, "ToogleEditMode", "Editbar", "", 0, 0, 24, 22, 0);
	((ZGuiCheckbox*)GetWnd("ToogleEditMode"))->GetUncheckedSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/move.bmp", 0);
	((ZGuiCheckbox*)GetWnd("ToogleEditMode"))->GetCheckedSkin()->
		m_iBkTexID = pkTexMan->Load("data/textures/gui/scale.bmp", 0);
}

void ZGuiTest::OnKeyDown(int iKey, ZGuiWnd* pkWnd)
{

}

void ZGuiTest::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = GetWnd(iID);

	if(pkWndClicked == NULL)
		return; 

	string strMainWndName = pkMainWnd->GetName();
	string strClickWndName = pkWndClicked->GetName();

	if(strMainWndName == "Toolbar")
	{
		if(strClickWndName == "ToolbarOK")
		{
			char* szName = GetText("WndNameTextbox");
			
			CreateWnd(Wnd, szName,  "", "", 0,  0, 100, 100, 0);
			m_pkFocusWnd = GetWnd(szName);

			bool bSuccess;

			SetText(szName, GetText("WndTitleTextbox"));
			m_pkFocusWnd->SetPos(GetTextInt("PosXTextbox",&bSuccess), 
				GetTextInt("PosYTextbox",&bSuccess), true, true);
			m_pkFocusWnd->Resize(GetTextInt("WidthTextbox",&bSuccess), 
				GetTextInt("HeightTextbox",&bSuccess), true);

			if(bSuccess == false)
			{
				printf("failed to get text from a editbox!\n");
			}
		}
	}
	else
	if(strMainWndName == "Editbar")
	{
		if(strClickWndName == "ToogleEditMode")
		{
			if(m_eEditMode == Move)
				m_eEditMode = Resize;
			else
				m_eEditMode = Move;
		}
	}
}

void ZGuiTest::OnMouseClick(bool bReleased, int x, int y)
{
	if(!bReleased)
	{
		ZGuiWnd* pkClickWnd = GetWndFromPoint(x,y);

		if(pkClickWnd)
		{
			if(!IsResWnd(pkClickWnd))
			{
				m_pkMoveWnd = pkClickWnd;
				m_pkFocusWnd = pkClickWnd;
				m_pkResizeWnd = pkClickWnd;
				
				Rect rc = pkClickWnd->GetScreenRect();
				m_iCursorRangeDiffX = x-rc.Left; 
				m_iCursorRangeDiffY = y-rc.Top;

				GetWnd("WndNameTextbox")->SetText( (char*) m_pkFocusWnd->GetName() );
				GetWnd("WndTitleTextbox")->SetText( (char*) m_pkFocusWnd->GetText() );

				int dl = abs(x - rc.Left);
				int dr = abs(x - rc.Right);
				int dt = abs(y - rc.Top);
				int db = abs(y - rc.Bottom);

				if(dl <= dr && dl <= dt && dl <= db)
					m_eCurrentResizeType = LeftSide;
				if(dr <= dl && dr <= dt && dr <= db)
					m_eCurrentResizeType = RightSide;
				if(dt <= dr && dt <= dl && dt <= db)
					m_eCurrentResizeType = TopSide;
				if(db <= dr && db <= dt && db <= dl)
					m_eCurrentResizeType = BottomSide;
			}
		}
		else
		{
			m_pkFocusWnd = NULL;
			m_pkResizeWnd = NULL;
		}
	}
	else
	{
		m_pkMoveWnd = NULL;
		m_pkResizeWnd = NULL;
	}
}

ZGuiWnd* ZGuiTest::GetWndFromPoint(int x, int y)
{
	ZGuiWnd* pkMainWnd = pkGui->GetMainWindowFromPoint(x,y) ;

	if(pkMainWnd)
	{
		return pkMainWnd;
	}

	return NULL;
}

bool ZGuiTest::IsResWnd(ZGuiWnd* pkWnd)
{
	if(pkWnd == m_pkToolbar)
		return true;

	if(pkWnd == m_pkEditbar)
		return true;

	return false;
}