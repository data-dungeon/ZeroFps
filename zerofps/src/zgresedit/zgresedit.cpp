#include "zgresedit.h"
#include "resource_id.h"
#include "guibuilder.h"
#include "propertybox.h"
#include "controlbox.h"
#include "skinbox.h"

#define ZGM_LBUTTONDOWN		10000
#define ZGM_LBUTTONUP		10001
#define ZGM_LBUTTONDBLCLK	10002
#define ZGM_MOUSEMOVE		10003
#define ZGM_KEYDOWN			10004
#define ZGM_KEYUP			10005

//
// Global objects
//

static ZGResEdit g_kResEditor("ZGResEdit",1024,768,24);

//
// Global functions
//

static bool WINPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, 
					 int iNumberOfParams, void *pkParams ) 
{
	return g_kResEditor.WinProc(pkWindow, uiMessage, iNumberOfParams, pkParams); 
}

///////////////////////////////////////////////////////////////////////////////

ZGResEdit::ZGResEdit(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth) 
{
	m_kMouseState = IDLE;
	m_pnCursorRangeDiffX=m_pnCursorRangeDiffY=-1;
}

ZGResEdit::~ZGResEdit()
{

}

void ClearConsol()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
}

void ZGResEdit::OnInit()
{
	m_pkCamera=new Camera(Vector3(0,10,0),Vector3(0,0,0),85,1.333,0.25,250);
	m_pkGuiBuilder=new GuiBuilder(pkGui, pkTexMan, pkGuiMan, 
		Rect(0,0,m_iWidth,m_iHeight));

	SDL_ShowCursor(SDL_DISABLE);

	m_pkGuiBuilder->InitSkins();

	// Change gui font
	ZGuiFont* pkDefaultFont = new ZGuiFont(16, 16, 0, ZG_DEFAULT_GUI_FONT);
	pkDefaultFont->CreateFromFile("../data/textures/text/ms_sans_serif8.bmp");
	pkGui->SetDefaultFont(pkDefaultFont);

	if(!Create())
	{
		cout << "Failed to create app!" << endl;
	}

	SetCursor("file:../data/textures/cursor.bmp",
		"file:../data/textures/cursor_a.bmp");

	pkFps->m_bGuiTakeControl = true;
	pkFps->ToggleGui();

	SDL_WM_SetCaption("Resource Editor", "mainicon.ico");
	ClearConsol();
}

void ZGResEdit::OnIdle()
{
	pkFps->SetCamera(m_pkCamera);		
	pkFps->GetCam()->ClearViewPort();

	pkInput->SetInputEnabled(true);

	int x, y;
	pkInput->MouseXY(x,y);

	static bool bClickedOnes = false;
	static float fClickTime = pkFps->GetGameTime();
	static bool bKeyPressed = false;
	static bool bMouseLeftPressed = false;
	static int iPrevX=x, iPrevY=y;

	if(ZGuiWnd::m_pkFocusWnd)
	{
		if(pkInput->Pressed(MOUSELEFT) && bMouseLeftPressed == false)
		{
			int kParams[2] = {x,y};
			WinProc(ZGuiWnd::m_pkFocusWnd, ZGM_LBUTTONDOWN, 2, (int*) kParams);
			bMouseLeftPressed = true;
		}
		if(!pkInput->Pressed(MOUSELEFT) && bMouseLeftPressed == true)
		{
			int kParams[2] = {x,y};
			WinProc(ZGuiWnd::m_pkFocusWnd, ZGM_LBUTTONUP, 2, (int*) kParams);
			bMouseLeftPressed = false;

			// Test for double click.
			if(bClickedOnes == true && pkFps->GetGameTime()-fClickTime<0.25f)
			{
				WinProc(ZGuiWnd::m_pkFocusWnd, ZGM_LBUTTONDBLCLK, 2, (int*) kParams);
				bClickedOnes = false;
			}

			if(bClickedOnes == false)
				bClickedOnes = true;

			fClickTime = pkFps->GetGameTime();
		}

		if(iPrevX != x || iPrevY != y)
		{
			int kParams[3] = {pkInput->Pressed(MOUSELEFT),x,y};
			WinProc(ZGuiWnd::m_pkFocusWnd, ZGM_MOUSEMOVE, 3, (int*) kParams);
			iPrevX = x; iPrevY = y;
		}

		// Check for key update in a very stupid manner
		int iKey = -1;
		for(int i=0; i<256; i++)
			if(pkInput->Pressed(i))
			{
				iKey = i;
				break;
			}
				
		if(iKey != -1 && bKeyPressed == false)
		{
			int kParams[1] = {iKey};
			WinProc(ZGuiWnd::m_pkFocusWnd, ZGM_KEYDOWN, 1, (int*) kParams);
			bKeyPressed = true;			
		}

		if(iKey == -1 && bKeyPressed == true)
		{
			int kParams[1] = {iKey};
			WinProc(ZGuiWnd::m_pkFocusWnd, ZGM_KEYUP, 1, (int*) kParams);
			bKeyPressed = false;			
		}
	}

	pkInput->SetInputEnabled(false);
}

bool ZGResEdit::WinProc(ZGuiWnd* pkWindow, unsigned int uiMessage, 
						int iNumberOfParams, void *pkParams ) 
{
	switch(uiMessage)
	{
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case ID_CTRL_WINDOW_BN:
		case ID_CTRL_WINDOW_BN+1:	case ID_CTRL_WINDOW_BN+2:
		case ID_CTRL_WINDOW_BN+3:	case ID_CTRL_WINDOW_BN+4:
		case ID_CTRL_WINDOW_BN+5:	case ID_CTRL_WINDOW_BN+6:
		case ID_CTRL_WINDOW_BN+7:	case ID_CTRL_WINDOW_BN+8:
			if(pkWindow == pkGuiMan->Wnd("CtrlBoxWnd"))
			m_pkGuiBuilder->CreateNewType(GuiBuilder::CtrlType(
				((int*)pkParams)[0]-ID_CTRL_WINDOW_BN),WINPROC);
			break;
		case ID_CTRLOK_BN:
			m_pkPropertyBox->OnClose(true);
			break;
		case ID_CTRLCANCEL_BN:
			m_pkPropertyBox->OnClose(false);
			break;
		case ID_CTRLBOX_OPENSKINDLG_BN:
			m_pkSkinBox->OnOpen();
			break;
		case ID_SKINBOX_CLOSE_BN:
			m_pkSkinBox->OnClose(false);
			break;
		case ID_SKINBOX_OK_BN:
			m_pkSkinBox->OnClose(true);
			break;
		}
		break;

	case ZGM_SELECTLISTITEM:
		int iListBoxID; iListBoxID = ((int*)pkParams)[0];
		int iSelItemIndex; iSelItemIndex = ((int*)pkParams)[1];

		switch(iListBoxID)
		{
		case ID_SKINBOX_TEXTURELIST_LB:
			ZGuiListbox* pkListbox;
			pkListbox = ((ZGuiListbox*)m_pkGuiBuilder->GetWnd("TexturesLB"));
			
			ZGuiListitem* pkItem;
			pkItem = pkListbox->GetItem(iSelItemIndex);
			m_pkSkinBox->UpdatePreviewImage(pkItem->GetText());
			break;
		}
		break;

	case ZGM_SCROLL:
		int iScrollBarID; iScrollBarID = ((int*)pkParams)[0];
		int iScrollBarPos; iScrollBarPos = ((int*)pkParams)[2];

		switch(iScrollBarID)
		{
		case ID_SKINBKREDCOLOR_SB:
			m_pkSkinBox->SetSkinBkColor(iScrollBarPos,-1,-1);
			break;
		case ID_SKINBKGREENCOLOR_SB:
			m_pkSkinBox->SetSkinBkColor(-1,iScrollBarPos,-1);
			break;
		case ID_SKINBKBLUECOLOR_SB:
			m_pkSkinBox->SetSkinBkColor(-1,-1,iScrollBarPos);
			break;

		case ID_SKINBORDERREDCOLOR_SB:
			m_pkSkinBox->SetSkinBorderColor(iScrollBarPos,-1,-1);
			break;
		case ID_SKINBORDERGREENCOLOR_SB:
			m_pkSkinBox->SetSkinBorderColor(-1,iScrollBarPos,-1);
			break;
		case ID_SKINBORDERBLUECOLOR_SB:
			m_pkSkinBox->SetSkinBorderColor(-1,-1,iScrollBarPos);
			break;
		}

		break;

	case ZGM_LBUTTONDOWN:
		{
			m_iClickPosX = ((int*)pkParams)[0];
			m_iClickPosY = ((int*)pkParams)[1];

			Rect rc = pkWindow->GetScreenRect();

			m_pnCursorRangeDiffX = m_iClickPosX-rc.Left;
			m_pnCursorRangeDiffY = m_iClickPosY-rc.Top;

			int dist_to_horz_border = abs(m_iClickPosX-rc.Right);
			int dist_to_vert_border = abs(m_iClickPosY-rc.Bottom);

			bool bResizeWnd = true;

			if(dist_to_horz_border < 4 && dist_to_vert_border < 4)
				m_kMouseState =	RESIZING_BOTH;
			else
			if(dist_to_horz_border < 4)
				m_kMouseState =	RESIZING_HORZ; 
			else
			if(dist_to_vert_border < 4)
				m_kMouseState =	RESIZING_VERT; 
			else
			{
				if(pkWindow->GetScreenRect().Inside(m_iClickPosX, m_iClickPosY))
					m_kMouseState = MOVING;

				bResizeWnd = false;
			}
			
			if(bResizeWnd)
			{
				m_iResizewnd_old_x = pkWindow->GetScreenRect().Left;
				m_iResizewnd_old_y = pkWindow->GetScreenRect().Top;
			}
		}
		break;

	case ZGM_LBUTTONUP:
		m_kMouseState = IDLE;
		break;

	case ZGM_LBUTTONDBLCLK:
		if(!IsGuiWnd(pkWindow))
		{
			m_pkPropertyBox->SetEditWnd(pkWindow);
			int x = pkWindow->GetScreenRect().Right;
			int y = pkWindow->GetScreenRect().Bottom;
			m_pkPropertyBox->OnOpen(x,y);
		}
		break;

	case ZGM_MOUSEMOVE:
		if(((bool*)pkParams)[0] == true) // Left button down
		{
			switch(m_kMouseState)
			{
			case RESIZING_BOTH:
			case RESIZING_HORZ:
			case RESIZING_VERT:
				if(!IsGuiWnd(pkWindow))
					ResizeWnd(pkWindow, ((int*)pkParams)[1], ((int*)pkParams)[2]);
				break;
			case MOVING:
				int x = ((int*)pkParams)[1]-m_pnCursorRangeDiffX;
				int y = ((int*)pkParams)[2]-m_pnCursorRangeDiffY;
				if(!IsGuiWnd(pkWindow))
					MoveWnd(pkWindow, x, y);
				break;
			}
		}
		break;

	case ZGM_KEYDOWN:
		switch(((int*)pkParams)[0]) // key pressed
		{
		case KEY_DELETE:
			if(!pkGui->UnregisterWindow(pkWindow))
				cout << "Failed to unregister window!" << endl;
		/*	delete pkWindow;
			pkWindow = NULL;*/
			break;
		case KEY_SPACE:
			//ClearScreen();
			break;
		}
		break;

	case ZGM_KEYUP:
		//printf("key ZGM_KEYUP\n");
		break;
	}

	return true; 
}

bool ZGResEdit::Create()
{
	int x, y, w, h;

	w = 150; h = 250; x = m_iWidth-w; y = 0;
	m_pkControlBox = new ControlBox(m_pkGuiBuilder);
	m_pkControlBox->Create(x,y,w,h,WINPROC);

	w = 400; h = 200; x = m_iWidth-w; y = m_iHeight-h;
	m_pkPropertyBox = new PropertyBox(m_pkGuiBuilder);
	m_pkPropertyBox->Create(x,y,w,h,WINPROC);

	w = 550; h = 350; x = 10; y = 10;
	m_pkSkinBox = new SkinBox(m_pkGuiBuilder,pkFps->m_pkBasicFS, pkTexMan);
	m_pkSkinBox->Create(x,y,w,h,WINPROC);

	return true;
}

void ZGResEdit::SetCursor(char* szFileNameBitmap, char* szFileNameBitmapAlpha)
{
	int cursor_tex = pkTexMan->Load(szFileNameBitmap, 0);
	int cursor_tex_a = pkTexMan->Load(szFileNameBitmapAlpha, 0);
	pkGui->SetCursor(cursor_tex, cursor_tex_a, 32, 32);
}

void ZGResEdit::ResizeWnd(ZGuiWnd* pkWnd, int x, int y)
{
	Rect rc = pkWnd->GetScreenRect();
	ZGuiWnd* pkParent = pkWnd->GetParent(true);

	// Make sure that the main window isn´t smaller then it fit all childs.
	if(pkParent == NULL)
	{
		list<ZGuiWnd*> pkChilds;
		int ctrl_highest_x=-1000;
		int ctrl_highest_y=-1000;
		pkWnd->GetChildrens(pkChilds);
		for( WINit win = pkChilds.begin(); win != pkChilds.end(); win++)
			 {
				Rect rc = (*win)->GetScreenRect();
				if(rc.Bottom > ctrl_highest_y)
					ctrl_highest_y = rc.Bottom;
				if(rc.Right > ctrl_highest_x)
					ctrl_highest_x = rc.Right;
			 }

		if(x < ctrl_highest_x) x = ctrl_highest_x;
		if(y < ctrl_highest_y) y = ctrl_highest_y;
	}
	
	switch(m_kMouseState)
	{
	case RESIZING_BOTH:
		if(x-rc.Left > 0 && y-rc.Top > 0)
		{
			int iNewWidth = x-m_iResizewnd_old_x;
			if(pkParent)
			{
				if(iNewWidth + rc.Left > pkParent->GetScreenRect().Right)
					iNewWidth = pkParent->GetScreenRect().Right-rc.Left;
			}
			int iNewHeight = y-m_iResizewnd_old_y;
			if(pkParent)
			{
				if(iNewHeight + rc.Top > pkParent->GetScreenRect().Bottom)
					iNewHeight = pkParent->GetScreenRect().Bottom-rc.Top;
			}
			pkWnd->Resize(iNewWidth,iNewHeight);
		}
		break;
	case RESIZING_HORZ:
		if(x-rc.Left > 0)
		{
			int iNewWidth = x-m_iResizewnd_old_x;
			if(pkParent)
			{
				if(iNewWidth + rc.Left > pkParent->GetScreenRect().Right)
					iNewWidth = pkParent->GetScreenRect().Right-rc.Left;
			}

			pkWnd->Resize(iNewWidth,rc.Height());
		}
		break;
	case RESIZING_VERT:
		if(y-rc.Top > 0)
		{
			int iNewHeight = y-m_iResizewnd_old_y;
			if(pkParent)
			{
				if(iNewHeight + rc.Top > pkParent->GetScreenRect().Bottom)
					iNewHeight = pkParent->GetScreenRect().Bottom-rc.Top;
			}
			pkWnd->Resize(rc.Width(),iNewHeight);
		}
		break;
	}
}

void ZGResEdit::MoveWnd(ZGuiWnd* pkWnd, int x, int y)
{
	Rect rc = pkWnd->GetScreenRect();
	
	switch(m_kMouseState)
	{
	case MOVING:
		ZGuiWnd* pkParent = pkWnd->GetParent(true);

		// Make sure that the window stays inside the parent frame.
		if(pkParent)
		{
			Rect rcBorder = pkParent->GetScreenRect();
			int w = pkWnd->GetScreenRect().Width();
			int h = pkWnd->GetScreenRect().Height();

			if(x+w > rcBorder.Right) x = rcBorder.Right-w;
			if(x < rcBorder.Left) x = rcBorder.Left;
			if(y+h > rcBorder.Bottom) y = rcBorder.Bottom-h;
			if(y < rcBorder.Top) y = rcBorder.Top;
		}
		pkWnd->SetPos(x, y, true, true);
		break;
	}
}

// Ignorera vissa fönster som tillhör själva gui:et för applikationen.
bool ZGResEdit::IsGuiWnd(ZGuiWnd *pkWnd)
{
	static string aGuiWnd[] =
	{
		string("CtrlBoxWnd"),
		string("CtrlPropBoxWnd"),
		string("SkinBoxWnd"),
	};

	for(int i=0; i<sizeof(aGuiWnd)/sizeof(aGuiWnd[0]); i++)
	{
		if(pkWnd == pkGuiMan->Wnd(aGuiWnd[i]))
			return true;

		ZGuiWnd* pkRootParent = pkWnd->GetParent(true);
		if(pkRootParent)
		{
			if(pkRootParent == pkGuiMan->Wnd(aGuiWnd[i]))
				return true;
		}
	}

	return false;
}