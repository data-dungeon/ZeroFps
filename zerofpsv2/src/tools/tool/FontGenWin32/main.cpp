#include <windows.h>
#include <stdio.h>
#include "resource2.h"
#include "../../../zerofpsv2/basic/image.h"

bool bShowGrid = true;
bool g_bDrawCharGrid = false;
static LOGFONT    lf;

//int ROW_WIDTH = 10;
//int CELL_SIZE = 16;
//int BITMAP_SIZE = 256;
int IMAGE_WIDTH = 128;
int IMAGE_HEIGHT = 128;
int ROW_HEIGHT = 0;
int VERTICAL_OFFSET = 0;
int HORIZONTAL_OFFSET = 1;

char STARTUPDIR[512];

int g_iSpaceWidth = 0;
int g_iPixelsAboveBaseLine = 0;

char aLetters[] =
{
	'!','"','#','$','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8',
	'9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','a','b','c','d','e','f','g',
	'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','}','|',''
};

const int NUM_LETTERS = sizeof(aLetters) / sizeof(aLetters[1]);

RECT aLetterRects[NUM_LETTERS];

//static COLORREF bkColor = RGB(255,255,255), textColor = RGB(0,0,0);
static COLORREF textColor = RGB(255,255,255), bkColor = RGB(0,0,0);

/*void DrawFont(HDC hdc)
{
	static HPEN blue_pen = CreatePen(PS_SOLID,0,RGB(0,0,255));
	static HPEN red_pen = CreatePen(PS_SOLID,0,RGB(255,0,0));


	SelectObject (hdc, CreateFontIndirect (&lf)) ;
	SelectObject (hdc, blue_pen) ;

	TEXTMETRIC        tm ;
	GetTextMetrics (hdc, &tm) ;

	RECT rc = {0,0,CELL_SIZE,CELL_SIZE};

	int oka=0, x, y;

	int start_char = 32; // ändra till 0 om det är en textur för konsolen

	for(y=0; y<ROW_WIDTH; y++)
	{
		for(x=0; x<ROW_WIDTH; x++)
		{
			char text[2];
			text[0] = start_char + oka;
			text[1] = '\0';
			DrawText(hdc, text, 1, &rc, 0);

			OffsetRect(&rc, CELL_SIZE, 0);

			if(bShowGrid)
			{
				MoveToEx(hdc,x*CELL_SIZE,0,NULL);
				LineTo(hdc,x*CELL_SIZE,ROW_WIDTH*CELL_SIZE);
			}

			oka++;
		}

		if(bShowGrid)
		{
			MoveToEx(hdc,0,y*CELL_SIZE,NULL);
			LineTo(hdc,ROW_WIDTH*CELL_SIZE,y*CELL_SIZE);
		}

		rc.left = 0;
		rc.right = CELL_SIZE;
		OffsetRect(&rc, 0, CELL_SIZE);
	}

	if(bShowGrid)
	{
		if(x*CELL_SIZE > 512)
			BITMAP_SIZE = 1024;
		else
		if(x*CELL_SIZE > 256)
			BITMAP_SIZE = 512;
		else
		if(x*CELL_SIZE > 128)
			BITMAP_SIZE = 256;
		else
		if(x*CELL_SIZE > 64)
			BITMAP_SIZE = 128;
		else
		if(x*CELL_SIZE > 32)
			BITMAP_SIZE = 64;
		else
		if(x*CELL_SIZE > 16)
			BITMAP_SIZE = 32;
		else
			BITMAP_SIZE = 16;

		SelectObject (hdc, red_pen) ;

		MoveToEx(hdc,BITMAP_SIZE,0,NULL);
		LineTo(hdc,BITMAP_SIZE,BITMAP_SIZE);

		MoveToEx(hdc,0,BITMAP_SIZE,NULL);
		LineTo(hdc,BITMAP_SIZE,BITMAP_SIZE);
	}
}*/

void DrawFont2(HDC hdc, HWND hwnd)
{
	static HPEN blue_pen = CreatePen(PS_SOLID,0,RGB(0,0,255));
	static HPEN red_pen = CreatePen(PS_SOLID,0,RGB(255,0,0));

	HFONT font = CreateFontIndirect(&lf);
	SelectObject (hdc, font) ;
	SelectObject (hdc, blue_pen) ;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm); 
	g_iPixelsAboveBaseLine = tm.tmAscent;

	

	RECT rc = {0,0,0,0};

	int i;
	int oka=0;
	int rows = 0;
	int max_char_height = 0;
	
	for(i=0; i<NUM_LETTERS; i++)
	{
		char text[2] = { aLetters[i], '\0' };

		SIZE s;
		GetTextExtentPoint32(hdc, text, 1, &s);
		
		if(max_char_height < s.cy)
			max_char_height = s.cy;
	}

	SIZE s;
	char text[2] = { ' ', '\0' };
	GetTextExtentPoint32(hdc, text, 1, &s);
	g_iSpaceWidth = s.cx;

	ROW_HEIGHT = max_char_height;
	max_char_height += VERTICAL_OFFSET;

	bool bFinished = false;
	while(1)
	{
		if(bFinished)
			break;

		int row_width = 0;
		rows++;

		while(1)
		{
			if(oka == NUM_LETTERS)
			{
				bFinished = true;
				break;
			}

			char text[2] = { aLetters[oka], '\0' };

			SIZE char_size;
			GetTextExtentPoint32(hdc, text, 1, &char_size);
			row_width += (char_size.cx + HORIZONTAL_OFFSET);

			if(row_width > IMAGE_WIDTH)
				break;

			rc.right = rc.left + char_size.cx; 
			rc.bottom = rc.top + char_size.cy;

			aLetterRects[oka] = rc;

			DrawText(hdc, text, 1, &rc, 0);

			if(g_bDrawCharGrid)
			{
				Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}

			SelectObject(hdc, GetStockObject(NULL_BRUSH));			
			OffsetRect(&rc, char_size.cx+HORIZONTAL_OFFSET, 0);

			oka++;
		}

		rc.left = 0;
		rc.right = 0;
		OffsetRect(&rc, 0, max_char_height);
	}

	IMAGE_HEIGHT = (rows * max_char_height) + 0;
	
	if(IMAGE_HEIGHT < 8) IMAGE_HEIGHT = 8;
	else if(IMAGE_HEIGHT < 16) IMAGE_HEIGHT = 16;
	else if(IMAGE_HEIGHT < 32) IMAGE_HEIGHT = 32;
	else if(IMAGE_HEIGHT < 64) IMAGE_HEIGHT = 64;
	else if(IMAGE_HEIGHT < 128) IMAGE_HEIGHT = 128;
	else if(IMAGE_HEIGHT < 256) IMAGE_HEIGHT = 256;
	else if(IMAGE_HEIGHT < 512) IMAGE_HEIGHT = 512;
	else if(IMAGE_HEIGHT < 1024) IMAGE_HEIGHT = 1024;

	if(bShowGrid)
	{
		SelectObject (hdc, red_pen) ;

		MoveToEx(hdc,IMAGE_WIDTH,0,NULL);
		LineTo(hdc,IMAGE_WIDTH,IMAGE_HEIGHT);

		MoveToEx(hdc,0,IMAGE_HEIGHT,NULL);
		LineTo(hdc,IMAGE_WIDTH+1,IMAGE_HEIGHT);
	}

	static int oldwidth=-1;
	static int oldheight=-1;

	if(oldwidth != IMAGE_WIDTH && oldheight != IMAGE_HEIGHT)
	{
		int new_w = IMAGE_WIDTH+10;
		int new_h = IMAGE_HEIGHT+50;

		SetWindowPos(hwnd, HWND_TOPMOST, 0,0, new_w, new_h, SWP_NOMOVE);
		
		oldwidth = new_w;
		oldheight = new_h;

		UpdateWindow(hwnd);
	}
		
}

#define FILE_FILTER "TGA (*.tga)\0*.tga\0\0"

void SaveFontInfo(HDC hdc, HWND hwnd)
{
	TCHAR strName[MAX_PATH] = TEXT("");

	char strFilePath[512] = "";
	char strInitialDir[MAX_PATH];

	SetCurrentDirectory(STARTUPDIR);
	FILE* infofile = fopen("fontgen.dat", "rt");

	if(infofile)
	{
		fgets(strInitialDir, 512, infofile);
		fclose(infofile);
	}
	else
	{
		GetCurrentDirectory(MAX_PATH, strInitialDir);
	}

	OPENFILENAME ofn = { sizeof(OPENFILENAME), hwnd, NULL,
		FILE_FILTER, NULL, 5, 0, strName, 
		MAX_PATH, strFilePath, MAX_PATH, 
		strInitialDir, strFilePath, 
		OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, 
		0, 1, NULL, 0, NULL, NULL };

	if(!GetSaveFileName( &ofn ) )
		return;

	if(strstr(ofn.lpstrFileTitle, ".tga") != NULL)
		ofn.lpstrFileTitle[strlen(ofn.lpstrFileTitle)-4] = '\0';

	UpdateWindow(hwnd);
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	char szFileName[512];
	sprintf(szFileName, "%s.fnt", ofn.lpstrFileTitle);

	FILE* pkFile = fopen(szFileName, "wb");
	
	fwrite(&IMAGE_WIDTH, sizeof(int), 1, pkFile); // bitmap width
	fwrite(&IMAGE_HEIGHT, sizeof(int), 1, pkFile); // bitmap height
	fwrite(&ROW_HEIGHT, sizeof(int), 1, pkFile); // row height
	fwrite(&g_iPixelsAboveBaseLine, sizeof(int), 1, pkFile); // pixels above the base line
	fwrite(&g_iSpaceWidth, sizeof(int), 1, pkFile); // spcae width
	fwrite(&NUM_LETTERS, sizeof(int), 1, pkFile); // num letters
	fwrite(&aLetterRects, sizeof(int)*4, NUM_LETTERS, pkFile); // letters rect
	fclose(pkFile);

	unsigned char bk_r = GetRValue(bkColor);
	unsigned char bk_g = GetGValue(bkColor);
	unsigned char bk_b = GetBValue(bkColor);
	
	Image file;
	file.CreateEmpty(IMAGE_WIDTH,IMAGE_HEIGHT);
	for(int y=0; y<IMAGE_HEIGHT; y++)
		for(int x=0; x<IMAGE_WIDTH; x++)
		{
			COLORREF color = GetPixel(hdc, x, y);

			unsigned char r = GetRValue(color);
			unsigned char g = GetGValue(color);
			unsigned char b = GetBValue(color);
			unsigned char a;

			//if((r == bk_r && g == bk_g && b == bk_b))
			//if(r<250&&g<250&&b<250)
				a = r;
		/*	else
				a = 255;*/

			file.set_pixel(x,IMAGE_HEIGHT-y, r, g, b, a); 
		}

	sprintf(szFileName, "%s.tga", ofn.lpstrFileTitle);

	file.Save(szFileName,true); 
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	GetCurrentDirectory(MAX_PATH, strInitialDir);
	SetCurrentDirectory(STARTUPDIR);
	infofile = fopen("fontgen.dat", "wt");
	fprintf(infofile, strInitialDir);
	fclose(infofile);

}

static LRESULT CALLBACK WndProc (HWND hwnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	static HDC hdcCanvas = CreateCompatibleDC(NULL);
	static HDC hdc;
	PAINTSTRUCT ps;

	CHOOSEFONT cf;
	cf.lStructSize = sizeof (CHOOSEFONT) ;
   cf.hwndOwner   = hwnd ;
   cf.lpLogFont   = &lf ;
	cf.Flags       = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS | CF_LIMITSIZE;
	cf.rgbColors      = 0 ;
	cf.lCustData      = 0 ;
	cf.lpfnHook       = NULL;
	cf.lpTemplateName = NULL ;
	cf.hInstance      = NULL ;
	cf.lpszStyle      = NULL ;
	cf.nFontType      = 0 ;      
	cf.nSizeMin       = 0 ;
	cf.nSizeMax       = 128 ;

	static CHOOSECOLOR cc ;
	static COLORREF    crCustColors[16] ;
	static HBRUSH bkBrush = CreateSolidBrush(bkColor);

	cc.lStructSize    = sizeof (CHOOSECOLOR) ;
	cc.hwndOwner      = NULL ;
	cc.hInstance      = NULL ;
	cc.rgbResult      = bkColor ;
	cc.lpCustColors   = crCustColors ;
	cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
	cc.lCustData      = 0 ;
	cc.lpfnHook       = NULL;
	cc.lpTemplateName = NULL ;

	bool bUpdate = false;

	int sx = GetDeviceCaps(GetDC(NULL),HORZRES);
	int sy = GetDeviceCaps(GetDC(NULL),VERTRES);

	switch(uiMessage)
	{

	case WM_SHOWWINDOW:
		ChooseFont(&cf);
		break;

	case WM_MOVE:
		{
			RECT rcWindow;
			GetWindowRect(hwnd, &rcWindow);
			int sx = GetDeviceCaps(GetDC(NULL),HORZRES);
			int sy = GetDeviceCaps(GetDC(NULL),VERTRES);
			int w = rcWindow.right-rcWindow.left;
			int h = rcWindow.bottom-rcWindow.top;

			if(rcWindow.left < 0)
			{
				rcWindow.left = 0;
				SetWindowPos(hwnd, HWND_TOPMOST, rcWindow.left ,rcWindow.top, 0, 0, SWP_NOSIZE);
				UpdateWindow(hwnd);
			}

			if(rcWindow.top < 0)
			{
				rcWindow.top = 0;
				SetWindowPos(hwnd, HWND_TOPMOST, rcWindow.left ,rcWindow.top, 0, 0, SWP_NOSIZE);
				UpdateWindow(hwnd);
			}

			if(rcWindow.right > sx)
			{
				rcWindow.left = sx-w;
				SetWindowPos(hwnd, HWND_TOPMOST, rcWindow.left ,rcWindow.top, 0, 0, SWP_NOSIZE);
			}
			if(rcWindow.bottom > sy)
			{
				rcWindow.top = sy-h;
				SetWindowPos(hwnd, HWND_TOPMOST, rcWindow.left ,rcWindow.top, 0, 0, SWP_NOSIZE);
			}
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;
		DrawFont2(hdc, hwnd);			
		EndPaint (hwnd, &ps) ;

		//DrawFont2(hdcCanvas);
		break;

	case WM_ERASEBKGND:
		RECT rc;
		SetRect(&rc, 0, 0, sx, sy);
		FillRect((HDC) wParam, &rc, bkBrush);
		SetBkColor((HDC) wParam, bkColor);
		SetTextColor((HDC) wParam, textColor);

		return 1;
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_SAVEFONT:
			SendMessage(hwnd, WM_KEYDOWN, VK_F5, 0);
			break;
		case ID_FILE_EXIT:
			SendMessage(hwnd, WM_KEYDOWN, VK_ESCAPE, 0);
			break;
		case ID_EDIT_CHANGEFONT:
			SendMessage(hwnd, WM_KEYDOWN, VK_F1, 0);
			break;
		case ID_EDIT_GRID:
			SendMessage(hwnd, WM_KEYUP, VK_NUMPAD0, 0);
			break;
		case ID_EDIT_INCREASEWIDTH:
			SendMessage(hwnd, WM_KEYUP, VK_ADD, 0);
			break;
		case ID_EDIT_DECREASEWIDTH:
			SendMessage(hwnd, WM_KEYUP, VK_SUBTRACT, 0);
			break;
		case ID_EDIT_INCREASEROWSPACEX:
			SendMessage(hwnd, WM_KEYDOWN, VK_RIGHT, 0);
			break;
		case ID_EDIT_DECREASEROWSPACEX:
			SendMessage(hwnd, WM_KEYDOWN, VK_LEFT, 0);
			break;
		case ID_EDIT_INCREASEROWSPACEY:
			SendMessage(hwnd, WM_KEYDOWN, VK_DOWN, 0);
			break;
		case ID_EDIT_DECREASEROWSPACEY:
			SendMessage(hwnd, WM_KEYDOWN, VK_UP, 0);
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_UP:
			if(VERTICAL_OFFSET > 0)
				VERTICAL_OFFSET--; bUpdate = true;
			break;

		case VK_DOWN:
			VERTICAL_OFFSET++; bUpdate = true;
			break;

		case VK_LEFT:
			if(HORIZONTAL_OFFSET > 1)
				HORIZONTAL_OFFSET--; bUpdate = true;
			break;

		case VK_RIGHT:
			HORIZONTAL_OFFSET++; bUpdate = true;
			break;

		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case VK_F1:
	
			LOGFONT temp;
			temp = lf;
			cf.lpLogFont = &temp;

			if(ChooseFont(&cf) != 0)
				lf = temp;

			lf.lfQuality = PROOF_QUALITY;

			bUpdate = true;
			break;

		case VK_F2:
			cc.rgbResult = bkColor;
			ChooseColor(&cc);
			bkColor = cc.rgbResult;
			bkBrush = CreateSolidBrush(bkColor);
			bUpdate = true;
			break;

		case VK_F3:
			cc.rgbResult = textColor;
			ChooseColor(&cc);
			textColor = cc.rgbResult;
			bUpdate = true;
			break;

		case VK_F5:
			bool bState = bShowGrid;
			bool bState2 = g_bDrawCharGrid;
			bShowGrid = false;
			g_bDrawCharGrid = false;
			InvalidateRect(hwnd,NULL,TRUE);
			UpdateWindow(hwnd);

			SaveFontInfo(hdc, hwnd);
			
			bShowGrid = bState;
			g_bDrawCharGrid = bState2;
			bUpdate = true;
			break;
		}
		break;

	case WM_KEYUP:
		switch(wParam)
		{
		case VK_ADD:
			IMAGE_WIDTH *= 2;
			bUpdate = true;
			break;

		case VK_SUBTRACT:
			IMAGE_WIDTH /= 2;
			bUpdate = true;
			break;

		case VK_NUMPAD0:
			g_bDrawCharGrid = !g_bDrawCharGrid;
			bUpdate = true;
			break;
		}
		break;
	}

	if(bUpdate)
	{
		InvalidateRect(hwnd,NULL,TRUE);
		UpdateWindow(hwnd);
	}

	return DefWindowProc( hwnd, uiMessage, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
{
	static char szAppName[] = "ZPGFontGen";

	WNDCLASSEX wndClass = 
	{ 
		sizeof(WNDCLASSEX), CS_CLASSDC | CS_VREDRAW | CS_HREDRAW, WndProc, 0, 0, 
		hInstance, 0, LoadCursor(NULL, IDC_ARROW), 
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, szAppName 
	};
	RegisterClassEx( &wndClass );

	int x = GetDeviceCaps(GetDC(NULL),HORZRES) / 2 - 640/2;
	int y = GetDeviceCaps(GetDC(NULL),VERTRES) / 2 - 480/2;
	const int STYLE  = WS_OVERLAPPEDWINDOW ^WS_THICKFRAME ^WS_MAXIMIZEBOX | WS_VISIBLE ;
	HWND hwnd = CreateWindow (szAppName, szAppName, STYLE, x, y, 640, 480, 0, 
		LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)), hInstance, NULL);

	GetCurrentDirectory(MAX_PATH, STARTUPDIR);

	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	DestroyWindow(hwnd);

	return msg.wParam ;
}
