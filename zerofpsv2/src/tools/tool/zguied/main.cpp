#ifndef _DONT_MAIN					// <- OBS! Flytta inte på denna. Måste ligga i
	#define _MAINAPPLICATION_		// just denna fil och inte på flera ställen.
	#define _DONT_MAIN
#endif

#include "zguied.h"
#include "resource.h"
#include "../../../zerofpsv2/engine_systems/script_interfaces/si_gui.h"
#include <windows.h>
#include <Commdlg.h>
#include <commctrl.h>
#include <SDL/SDL_thread.h>

HBITMAP preview_bitmap;

HWND m_khWnd, m_khWnd2;
HWND sdl_wnd = NULL;
HWND g_kDlgBoxRight;
HWND g_kDlgBoxBottom;
HWND g_kFontDlg;
HWND g_kOpenScriptDlg;
HINSTANCE hInstance = NULL;

HFONT m_hStandardFont = CreateFont( 16, 0, 0, 0, FW_NORMAL, FALSE,
				  FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
				  VARIABLE_PITCH, "Verdana" );

HFONT m_hImageListFont = CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE,
				  FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
				  VARIABLE_PITCH, "Arial" );

HFONT m_hWndListFont = CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE,
				  FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
				  VARIABLE_PITCH, "Verdana" );

vector<HWND> g_vkToolTips;

ZGuiEd g_kZGuiEd("zguied",1024,768,0);

void CreateTooltip (HWND hwnd, char* text);
void DrawBitmap (HDC hdc, int x, int y, int sw, int sh, HBITMAP hBitmap);
void ActivateHelp(bool bActivate);
int GetWindowSize(int iID, bool bRightPanel, bool bWidth);
void SelectAColor(unsigned char& r, unsigned char& g, unsigned char& b);
static bool GUIPROC( ZGuiWnd* win, unsigned int msg, int numparms, void *params ){ return true; };

ZGuiEd::ZGuiEd(char* aName,int iWidth,int iHeight,int iDepth) 
	: Application(aName,iWidth,iHeight,iDepth), ZGuiApp(GUIPROC) { }

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;

	RECT rc;
	GetWindowRect(GetCtrl(IDC_SET_TEXTURE_BN,1), &rc);

	RECT rcParent;
	GetWindowRect(GetParent(GetCtrl(IDC_SET_TEXTURE_BN,1)), &rcParent);

	static COLORREF wnd_listcolor = RGB(27,105,41);
	static COLORREF image_listcolor = RGB(255,128,128);
	static COLORREF skin_listcolor = RGB(255,255,128);
	static COLORREF font_listcolor = RGB(255,255,0);
	static COLORREF script_listcolor = RGB(128,128,0);

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
				DrawBitmap(hdc, 8, 768-128-30, sz.cx, sz.cy, preview_bitmap);
			}
		EndPaint (hwnd, &ps) ;
		break;

/*	case WM_CTLCOLORLISTBOX:

		if((HWND)lParam == GetDlgItem(g_kOpenScriptDlg, IDC_SELECTFILE_LIST))
		{
			LOGBRUSH LogBrush; 
			LogBrush.lbStyle = BS_SOLID;       
			LogBrush.lbColor = script_listcolor;       
			LogBrush.lbHatch = (LONG) NULL;      

			SetTextColor( (HDC) wParam, RGB(0,0,0) );       
			SetBkColor( (HDC) wParam, script_listcolor);        
			return( (LPARAM) CreateBrushIndirect( &LogBrush ) ); 
		}

		if((HWND)lParam == GetDlgItem(g_kDlgBoxRight, IDC_TEXTURE_LIST))
		{
			LOGBRUSH LogBrush; 
			LogBrush.lbStyle = BS_SOLID;       
			LogBrush.lbColor = image_listcolor;       
			LogBrush.lbHatch = (LONG) NULL;      

			SetTextColor( (HDC) wParam, RGB(0,0,0) );       
			SetBkColor( (HDC) wParam, image_listcolor);        
			return( (LPARAM) CreateBrushIndirect( &LogBrush ) ); 
		}

		if((HWND)lParam == GetDlgItem(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST))
		{
			LOGBRUSH LogBrush; 
			LogBrush.lbStyle = BS_SOLID;       
			LogBrush.lbColor = skin_listcolor;       
			LogBrush.lbHatch = (LONG) NULL;      

			SetTextColor( (HDC) wParam, RGB(0,0,255) );       
			SetBkColor( (HDC) wParam, skin_listcolor);        
			return( (LPARAM) CreateBrushIndirect( &LogBrush ) ); 
		}

		if((HWND)lParam == GetDlgItem(g_kDlgBoxRight, IDC_WINDOW_LIST))
		{
			LOGBRUSH LogBrush; 
			LogBrush.lbStyle = BS_SOLID;       
			LogBrush.lbColor = wnd_listcolor;       
			LogBrush.lbHatch = (LONG) NULL;      

			SetTextColor( (HDC) wParam, RGB(255,255,255) );       
			SetBkColor( (HDC) wParam, wnd_listcolor);        
			return( (LPARAM) CreateBrushIndirect( &LogBrush ) ); 
		}

		break;*/
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int Win32ThreadMain(void *v)
{
	INITCOMMONCONTROLSEX iccex; 
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

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

	HWND h4 = CreateWindowEx(WS_EX_PALETTEWINDOW, "GUIToolbox", "GUIToolbox3", 
		WS_VISIBLE | WS_CHILD, 0, 0, 248, 228, sdl_wnd, NULL, hInstance, NULL) ;

	SetParent(m_khWnd, sdl_wnd);
	SetParent(m_khWnd2, sdl_wnd);

	g_kDlgBoxRight = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOGLEFT), m_khWnd, (DLGPROC) WndProc);
	g_kDlgBoxBottom = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOGBOTTOM), m_khWnd2, (DLGPROC) WndProc);
	g_kFontDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SELECTFONT), h3, (DLGPROC) WndProc);
	g_kOpenScriptDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SELECTSCRIPT), h4, (DLGPROC) WndProc);

	ShowWindow(h3, SW_HIDE);

	SetWindowPos(g_kDlgBoxRight, 0, 0, 0, 1024-800, 768, SWP_NOZORDER);
	SetWindowPos(g_kDlgBoxBottom, 0, 0, 0, 800, 1024-768, SWP_NOZORDER);
	SetWindowPos(h4, 0, 800-650, 0, 650, 400, SWP_NOZORDER);

	ShowWindow(h4, SW_HIDE);

	SendDlgItemMessage( g_kOpenScriptDlg, IDC_SELECTFILE_LIST, WM_SETFONT, 
		(WPARAM) m_hStandardFont, MAKELPARAM(TRUE,0) );

	SendDlgItemMessage( g_kDlgBoxRight, IDC_TEXTURE_LIST, WM_SETFONT, 
		(WPARAM) m_hImageListFont, MAKELPARAM(TRUE,0) );

	SendDlgItemMessage( g_kDlgBoxRight, IDC_WINDOW_LIST, WM_SETFONT, 
		(WPARAM) m_hWndListFont, MAKELPARAM(TRUE,0) );

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

	CheckDlgButton(g_kDlgBoxRight, IDC_SKINTYPE_BACKGROUND_RB, BST_CHECKED);

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_LOADGUI), 
		"To open a GUI file, select it from the combobox and press this button\r\n");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_SAVE_SCRIPT), 
		"To Save a File\r\n\r\n" \
		"1) Type a filename in the combobox\r\n" \
		"2) Press this button\r\n" \
	   "3) Select a path in the listbox that shows up\r\n" \
		"4) Press the button again to save. Or press Esc to cancel.\r\n" \
		"(When you save a backup file will be created with the same name but with another extension (*.bak%i).");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_SET_TEXTURE_BN), 
		"Press this button to change texture on the selected element in the SkinList.\r\n\r\n" \
		"You must first select a element in the SkinList. Select one of the border " \
		"buttons if you want to change image of the border.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_REMOVE_TEXTURE_BN), 
		"Press this button to remove texture (only use colors) on the selected " \
		"element in the SkinList.\r\n\r\n" \
		"You must first select a element in the SkinList. Select one of the border " \
		"buttons if you want to change image of the border.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_RESIZE_TO_IMAGE), 
		"Press this button to resize selected widget to same size as the image selected in the list.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST), "Skin List\r\n\r\n" \
		"Select the element of the widget that you want to change the appearance of.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_TEXTURE_LIST), 
		"Texture List\r\n\r\nDouble click on folders " \
		"to open them. Double click on \"..\" to go to previus folder.\r\n\r\n" \
		"It list compatible image files (bmp, tga and jpg). Root path is " \
		"\"..data/textures/gui\", you cant browse further then that.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_FILELIST_CB), 
		"File List\r\n\r\n" \
		"It list all GUI script files (*.Lua) in \"..data/script/gui\".\r\n" \
		"To save, type in a name here and press the Save button");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_FORCE_CAPTURE_TO_SEL_CB), 
		"Press this button to only affect the current widget.\r\n" \
		"Use this to prevent clicking on other widgets that are on top.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_ONLY_OF_WND_TYPE_CB), 
		"Press this button to affect only widgets of the current type.\r\n" \
		"Select the current type in the Widget Type List.\r\n" \
		"Use this to prevent clicking on other widgets that are on top.\r\n" \
		"The Widget List will only display widgets of that type and only " \
		"widgets of that type will respond to mouse input.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_WINDOW_LIST), 
		"Widget List\r\n\r\n" \
		"By selecting a item in the list, you select a widget. Sometimes this is " \
		"easier then working with mouse.\r\n\r\n" \
		"This is also the only way to select a page in a Tab Control or a widget that is hidden.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_COPY_WND_BN), 
		"Press this button to create a copy of a widget.\r\n" \
		"You can also do this by pressing Ctrl + C or by holding Ctrl while clicking on a widget.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_PASTE_WND_BN), 
		"Press this button to paste a copy of a widget.\r\n" \
		"You can also do this by pressing Ctrl + V.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WINDOW_NAMEID_EB), 
		"The resource name of the widget.\r\n" \
		"Use a long name that describe the widget. The name must be unique.\r\n" \
		"Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_TEXTLABEL_EB), 
		"The text displayed on the widget.\r\n" \
		"Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_PARENT_WINDOW_NAMEID2), 
		"The resource name of the parent window. You can't change this.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WINDOWTYPE_EB), 
		"The widget type. You can't change this.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_SKINTYPE_BACKGROUND_RB), 
		"Select the background element of the widget to be affected.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_SKINTYPE_HORZBORDER_RB), 
		"Select the horizontal border element of the widget to be affected.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_SKINTYPE_VERTBORDER_RB), 
		"Select the vertical border element of the widget to be affected.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_SKINTYPE_CORNERBORDER_RB), 
		"Select the corner element of the widget to be affected.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_ROTATION_EB), 
		"Degree Rotation (0-360) on background image.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_TRANSPARENT_CB), 
		"Selected skin element will be invisible and will not recive input.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_TILE_CB), 
		"Repeat background image if checked or stretch otherwise.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RGB_COLOR_R_EB), 
		"Red color element (0-255) for either background image or border. Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RGB_COLOR_G_EB), 
		"Green color element (0-255) for either background image or border. Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RGB_COLOR_B_EB), 
		"Blue color element (0-255) for either background image or border. Press Enter to set.");
	
	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_FREE_MOVEMENT_CB), 
		"Check this button to enable free movement of window. Only work for Windows.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_COPY_ALL_SKINS), 
		"Copy all skins in list to a buffer. Then you can select another widget and paste " \
		"the same skins onto that. Only works if both have the same skin elments (i.e a Button " \
		"Up skin can be pasted to a Listbox, but not to slider).");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_COPY_SKIN), 
		"Copy selected skin to a buffer. Then you can select another widget and paste " \
		"it to its selected element. Works on all skins.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_PASTE_SKIN), 
		"Paste selected skins to another window.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_POSX_EB), 
		"Position of widget. If widget is a window then coordinate is in screenspace (0-800). " \
		"Else (0,0) is at TopLeft corner. Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_POSY_EB), 
		"Position of widget. If widget is a window then coordinate is in screenspace (0-600). " \
		"Else (0,0) is at TopLeft corner. Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_BORDER_EB), 
		"Size of border (in pixels) for the selected skin element. Buttons have its border inside " \
		"and all other have the border outside the clicking rectangle. Press Enter to set.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_CREATE_WND_BN), 
		"Press this button to create a new widget. The widgets will be a child to the selected " \
		"window. If the selected widget is a Tab Control, you can add pages to it by selecting it " \
		"as a parent and then creating a new Wnd.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_TESTGUI_BN), 
		"Press this button to test the GUI. Press it again to stop testing.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_NEWGUI_BN), 
		"Press this button to clear all widgets and skins and create a new GUI screen.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_DISABLE_MOVE_CB), 
		"Press this button to disable movement of widgets. Use this to avoid moving things by mistake " \
		"(if you only want to select or resize widgets for example).");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_RESIZE_WND), 
		"Press this button active resize mode so you can resize widgets more easly. " \
		"You can also resize widgets by pressing Left Shift.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_VISIBLE_CHECK), 
		"Press this button to temporary toggle visibility of a widget in the editor.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_HIDDEN_FROM_START_CB), 
		"Press this button to toogle visibility of a widget.");

	CreateTooltip(GetDlgItem(g_kDlgBoxRight, IDC_BRING_TO_FRONT), 
		"Press this button to place a widget in front of all other. Use it to bring up windows " \
		"that are hidden behind other windows. Not neccesery to set this for every window, the " \
		"editor will rearrange all widgets automatically anyway on save.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT), 
		"The distance from Top Left corner of the screen to window are always " \
		"the same no matter how big or small the screen is.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPRIGHT), 
		"The distance from Top Right corner of the screen to window are always " \
		"the same no matter how big or small the screen is.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMLEFT), 
		"The distance from Bottom Left corner of the screen to window are always " \
		"the same no matter how big or small the screen is.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMRIGHT), 
		"The distance from Bottom Right corner of the screen to window are always " \
		"the same no matter how big or small the screen is.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTER), 
		"The window are always centered on the screen.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERVERT), 
		"The window are always centered vertically on the screen.");
	
	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERHORZ), 
		"The window are always centered horizontally on the screen.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RESIZETYPE_DONT), 
		"The window will not be resized when the resolution thange from 800x600.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RESIZETYPE_BOTH), 
		"If the screen size is not 800x600, the window will change size so it will have the same propotion in any resolution.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RESIZETYPE_WIDTH), 
		"If the screen size is not 800x600, the window will change width so it will have the same propotion in any resolution.");

	CreateTooltip(GetDlgItem(g_kDlgBoxBottom, IDC_RESIZETYPE_HEIGHT), 
		"If the screen size is not 800x600, the window will change height so it will have the same propotion in any resolution.");
	
	HBITMAP hIcon;
	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_topleft.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPLEFT), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_topright.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_TOPRIGHT), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_bottomright.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMRIGHT), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_bottomleft.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_BOTTOMLEFT), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_centerhorz.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERHORZ), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_centervert.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTERVERT), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	hIcon = (HBITMAP) LoadImage(NULL, 
		"../datafiles/sysdata/textures/gui/wndalignent_center.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	SendMessage(GetDlgItem(g_kDlgBoxBottom, IDC_WNDALIGNMENT_CENTER), 
		BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) (HANDLE) hIcon);

	if( GetDeviceCaps(GetDC(NULL), HORZRES) <= 1024 || GetDeviceCaps(GetDC(NULL), VERTRES) <= 768)
		ShowWindow(GetDlgItem(g_kDlgBoxRight, IDC_CLOSE_BUTTON), SW_SHOW);

	
	
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

	m_pkZFVFileSystem->AddRootPath( "../datafiles/dm/",	"/data");
	m_pkZFVFileSystem->AddRootPath( "../datafiles/mistlands/",	"/data");
	
  	//create camera
	m_pkCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),70,1.333,0.25,250);	
	m_pkCamera->SetClearViewPort(true);
	m_pkCamera->SetClearColor(Vector4(0.44f,0.44f,0.44f,0));
	m_pkZeroFps->AddRenderCamera(m_pkCamera);

	// get basicfs pointer
	m_pkBasicFS = m_pkZeroFps->m_pkBasicFS;

	m_bDisableGuiScaleMode = true;

	m_bAlphaTextureMode = false;

	// create gui script
	GuiAppLua::
		Init(&g_kZGuiEd, m_pkScript);
	InitGui(m_pkScript, "defguifont", "data/script/gui/defskins.lua", NULL, true, true);

	SetTitle("ZeroFps GUI Editor");

	m_iZValueCounter = 1;
	m_bResize = false;
	m_bTestGUI = false;
	m_bForceCaptureToSel = false;

	sdl_wnd = GetFocus();

	m_strCurrTexDir = "data/textures/gui/";
	SetDlgItemText(g_kDlgBoxRight, IDC_CURRENT_PATH_EB, m_strCurrTexDir.c_str());
	
	m_pkGui->Activate(false);

	int horizontal_res = GetDeviceCaps(GetDC(NULL), HORZRES);
	int vertical_res = GetDeviceCaps(GetDC(NULL), VERTRES);
	int iFullscreen = m_pkRender->GetFullscreen();

	if( GetWidth() != 1024 || GetHeight() != 768)
	{
		string strMessage = 
			"The window is to small. " \
			"Press OK and restart the application again.\r\n" \
			"If you see this message again, " \
			"you must manually open zguied.ini in the bin folder and change " \
			"the key \"r_width\" and \"r_height\" to at least 1024x768.\r\n\r\n" \
			"You must also have a resolution of least 1280x1024 in windows " \
			"or else the applcation are forced to run in fullscreen mode.";

		if(horizontal_res < 1024 || vertical_res < 768)
		{
			iFullscreen = 1;
		}
		else
		{			
			iFullscreen = 0;
		}

		m_pkRender->SetDisplay(1024,768,16,iFullscreen);

		MessageBox(sdl_wnd, strMessage.c_str(), "Error", MB_OK);
		m_pkZeroFps->QuitEngine();			
		
		return;
	}

	if(iFullscreen == false &&
		(horizontal_res < 1024 || vertical_res < 768))
	{
		if(IDYES == MessageBox(sdl_wnd, "To use this program in window mode, you must change to higher resoulution in Windows.\r\n" \
				"Do you want to run i fullscreen? (you must restart first)", "Error", MB_YESNO))
			m_pkRender->SetDisplay(1024,768,16,1);

		m_pkZeroFps->QuitEngine();
		return;
	}

	hInstance = (HINSTANCE) GetWindowLong(sdl_wnd, GWL_HINSTANCE);
	SDL_CreateThread(Win32ThreadMain, NULL);

	NewGUI(false);

	m_iTask = 5; // update texture list

	SendMessage(sdl_wnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)));

	if( (horizontal_res < 1024 || vertical_res < 768) )
		ShowWindow(sdl_wnd, SW_SHOWMAXIMIZED);

	if(m_pkRender->GetFullscreen())
		ShowWindow(GetDlgItem(g_kDlgBoxRight, IDC_CLOSE_BUTTON), SW_SHOW);		
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

	float width = sw;
	float height = sh;

	if(width > height)
	{
		float mod = width / 128.0f;
		height = height / mod;
		width = 128;
	}
	else
	{
		float mod = height / 128.0f;
		width = width / mod;
		height = 128;
	}

	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdc, x, y, width, height, hMemDC, 0, 0, sw, sh, SRCCOPY) ;

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



void CreateTooltip (HWND hwnd, char* text)
{	
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_BALLOON,		
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd, NULL, hInstance, NULL);

	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	RECT rect; 
	GetClientRect (hwnd, &rect);

	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.hinst = hInstance;
	ti.uId = 0;
	ti.lpszText = text;
	ti.rect.left = rect.left;    
	ti.rect.top = rect.top;
	ti.rect.right = rect.right;
	ti.rect.bottom = rect.bottom;

	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
	SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 300);
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, (LPARAM) MAKELONG(1000*20, 0));
	SendMessage(hwndTT, TTM_ACTIVATE, FALSE, 0);	

	g_vkToolTips.push_back(hwndTT);	
} 

void ActivateHelp(bool bActivate)
{
	for(int i=0; i<g_vkToolTips.size(); i++)
	{
		SendMessage(g_vkToolTips[i], TTM_ACTIVATE, bActivate ? TRUE : FALSE, 0);	
	}
}

int GetWindowSize(int iID, bool bRightPanel, bool bWidth)
{
	HWND ctrl = GetCtrl(iID, bRightPanel);

	if(ctrl)
	{
		RECT rc;
		GetWindowRect(ctrl, &rc);

		if(bWidth)
			return rc.right - rc.left;
		else
			return rc.bottom - rc.top;
	}

	return -1;
}

void SelectAColor(unsigned char& r, unsigned char& g, unsigned char& b)
{
	static COLORREF defcolor = RGB(0,0,0);
	static CHOOSECOLOR cc ;
	static COLORREF    crCustColors[16] ;
	static HBRUSH bkBrush = CreateSolidBrush(defcolor);

	cc.lStructSize    = sizeof (CHOOSECOLOR) ;
	cc.hwndOwner      = NULL ;
	cc.hInstance      = NULL ;
	cc.rgbResult      = defcolor ;
	cc.lpCustColors   = crCustColors ;
	cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
	cc.lCustData      = 0 ;
	cc.lpfnHook       = NULL;
	cc.lpTemplateName = NULL ;

	ChooseColor(&cc);

	r = GetRValue(cc.rgbResult);
	g = GetGValue(cc.rgbResult);
	b = GetBValue(cc.rgbResult);
}