#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

#include "zguied.h"
#include "resource.h"
#include "../../../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include <windows.h>
#include <Commdlg.h>

HBITMAP preview_bitmap;

HWND m_khWnd, m_khWnd2;
HWND sdl_wnd = NULL;
HWND g_kDlgBoxRight;
HWND g_kDlgBoxBottom;
HWND g_kFontDlg;
HINSTANCE hInstance = NULL;

ZGuiEd g_kZGuiEd("zguied",1024,768,0);

void DrawBitmap (HDC hdc, int x, int y, int sw, int sh, HBITMAP hBitmap);
static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ){ return true; };

ZGuiEd::ZGuiEd(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC) { }

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;

	switch(message)
	{
	case WM_COMMAND:
		g_kZGuiEd.OnCommand(LOWORD(wParam), HIWORD(wParam));
		break;
	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;
			if(hwnd == g_kDlgBoxRight && preview_bitmap != NULL)
			{
				SIZE sz;
				GetBitmapDimensionEx( preview_bitmap, &sz);
				DrawBitmap(hdc, 8, 768-128-8, sz.cx, sz.cy, preview_bitmap);
			}
		EndPaint (hwnd, &ps) ;
		break;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int Win32ThreadMain(void *v)
{
	MSG msg ;

	WNDCLASS wndclass ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = NULL ;
	wndclass.hCursor       = NULL ;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = "GUIToolbox" ;

	if (!RegisterClass (&wndclass))
		return 0;

	m_khWnd = CreateWindowEx(WS_EX_PALETTEWINDOW, "GUIToolbox", "GUIToolbox", 
		WS_VISIBLE | WS_CHILD, 800, 0, 1024-800, 800, sdl_wnd, NULL, hInstance, NULL) ;			

	m_khWnd2 = CreateWindowEx(WS_EX_PALETTEWINDOW, "GUIToolbox", "GUIToolbox2", 
		WS_VISIBLE | WS_CHILD, 0, 600, 800, 1024-768, sdl_wnd, NULL, hInstance, NULL) ;

	HWND h3 = CreateWindowEx(WS_EX_PALETTEWINDOW, "GUIToolbox", "GUIToolbox3", 
		WS_VISIBLE | WS_CHILD, 0, 0, 248, 228, sdl_wnd, NULL, hInstance, NULL) ;

	SetParent(m_khWnd, sdl_wnd);
	SetParent(m_khWnd2, sdl_wnd);

	g_kDlgBoxRight = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOGLEFT), m_khWnd, (DLGPROC) WndProc);
	g_kDlgBoxBottom = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOGBOTTOM), m_khWnd2, (DLGPROC) WndProc);
	g_kFontDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SELECTFONT_DLGBOX), h3, (DLGPROC) WndProc);

	ShowWindow(h3, SW_HIDE);

	SetWindowPos(g_kDlgBoxRight, 0, 0, 0, 1024-800, 768, SWP_NOZORDER);
	SetWindowPos(g_kDlgBoxBottom, 0, 0, 0, 800, 1024-768, SWP_NOZORDER);

	HWND cbox = GetDlgItem(g_kDlgBoxRight, IDC_NEWCNTRL_CB);

	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Wnd" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Button" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Checkbox" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Combobox" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Label" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Listbox" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Radiobutton" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Scrollbar" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Slider" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "TabControl" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Textbox" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Treebox" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Menu" );
	SendMessage(cbox, CB_ADDSTRING,  0, (LPARAM) (LPCSTR) "Progressbar" );

	SendMessage(cbox, CB_SELECTSTRING, -1, (LPARAM) (LPCSTR) "Wnd" );

	CheckDlgButton(g_kDlgBoxBottom, IDC_SKINTYPE_BACKGROUND_RB, BST_CHECKED);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return 0;
}

void ZGuiEd::OnInit()			
{
	m_iTask = TASK_DO_NOTHING;

	m_strNewFileToLoad = "";

	m_pkZFVFileSystem->AddRootPath( "../datafiles/mistlands/",	"/data");
	
  	//create camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetClearViewPort(true);
	m_pkCamera->SetClearColor(Vector4(0.44f,0.44f,0.44f,0));
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	// get basicfs pointer
	m_pkBasicFS = m_pkZeroFps->m_pkBasicFS;

	m_bDisableGuiScaleMode = true;

	// create gui script
	GuiAppLua::Init(&g_kZGuiEd, m_pkScript);
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", NULL, true, true);

	SetTitle("ZeroFps GUI Editor");

	m_iZValueCounter = 1;
	m_bResize = false;
	m_bTestGUI = false;
	m_bForceCaptureToSel = false;

	sdl_wnd = GetFocus();

	m_strCurrTexDir = "data/textures/gui/";
	
	m_pkGui->Activate(false);

	if( GetWidth() != 1024 && GetHeight() != 768)
	{
		m_pkRender->SetDisplay(1024,768,16,0);
		MessageBox(sdl_wnd, "Bad windowsize. Please restart application", "Error", MB_OK);
		m_pkZeroFps->QuitEngine();
		return;
	}

	hInstance = (HINSTANCE) GetWindowLong(sdl_wnd, GWL_HINSTANCE);
	SDL_CreateThread(Win32ThreadMain, NULL);

	NewGUI(false);

	m_iTask = 5; // update texture list
}

HWND GetCtrl(int iID, bool bRightPanel)
{
	HWND wnd;
	
	if(bRightPanel)
		wnd = GetDlgItem(g_kDlgBoxRight, iID);
	else
		wnd = GetDlgItem(g_kDlgBoxBottom, iID);

	return wnd;
}

void DrawBitmap (HDC hdc, int x, int y, int sw, int sh, HBITMAP hBitmap)
{
	BITMAP bm ;
	HDC    hMemDC ;

	hMemDC = CreateCompatibleDC (hdc) ;
	SelectObject (hMemDC, hBitmap) ;
	GetObject (hBitmap, sizeof (BITMAP), &bm) ;

	if(sw > PREVIEW_SIZE || sh > PREVIEW_SIZE)
	{
		SetStretchBltMode(hdc, HALFTONE);
		StretchBlt(hdc, x, y, PREVIEW_SIZE, PREVIEW_SIZE, hMemDC, 0, 0, sw, sh, SRCCOPY) ;
	}
	else
		BitBlt(hdc, x, y, PREVIEW_SIZE, PREVIEW_SIZE, hMemDC, 0, 0, SRCCOPY) ;

	DeleteDC (hMemDC) ;
}


bool TextboxFocus()
{
	HWND kFocusWnd = GetFocus();

	if(kFocusWnd == NULL)
		return false;

	char classname[255];
	GetClassName (kFocusWnd, classname, 255);

	if(strcmp(classname, "Edit") == 0)
		return true;

	if(strcmp(classname, "ListBox") == 0)
		return true;

	if(strcmp(classname, "ComboBox") == 0)
		return true;

	return false;
}

string GetSelItemText(int iID, bool bRightPanel)
{
	int sel;
	char szText[200];
	strcpy(szText, "");

	HWND ctrl = GetCtrl(iID, bRightPanel);

	char classname[255];
	GetClassName (ctrl, classname, 255);

	if(strcmp(classname, "ListBox") == 0)
	{
		if((sel = SendMessage(ctrl, LB_GETCURSEL, 0, 0)) == LB_ERR)
			return "";	

		if(SendMessage(ctrl, LB_GETTEXT, sel, (LPARAM) (LPCSTR) szText) == LB_ERR)
			return "";
	}
	else
	{
		if((sel = SendMessage(ctrl, CB_GETCURSEL, 0, 0)) == CB_ERR)
			return "";

		if(SendMessage(ctrl, CB_GETLBTEXT, sel, (LPARAM) (LPCSTR) szText) == CB_ERR)
			return "";
	}

	return string(szText);
}