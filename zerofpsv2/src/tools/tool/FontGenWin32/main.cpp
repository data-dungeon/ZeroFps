#include <windows.h>
#include <stdio.h>
#include "../../../zerofpsv2/basic/image.h"

bool bShowGrid = true;
bool g_bDrawCharGrid = false;
static LOGFONT    lf;

int ROW_WIDTH = 10;
int CELL_SIZE = 16;
int BITMAP_SIZE = 256;
int TEST_WIDTH = 128;
int BITMAP_HEIGHT = 128;
int ROW_HEIGHT = 0;
int VERTICAL_OFFSET = 0;
int HORIZONTAL_OFFSET = 1;

POINT mclick = {9999,9999};

char aLetters[] =
{
	'!','"','#','$','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8',
	'9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','a','b','c','d','e','f','g',
	'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','}'
};

const int NUM_LETTERS = sizeof(aLetters) / sizeof(aLetters[1]);

RECT aLetterRects[NUM_LETTERS];

static COLORREF bkColor = RGB(255,255,255), textColor = RGB(0,0,0);

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

void DrawFont2(HDC hdc)
{
	static HPEN blue_pen = CreatePen(PS_SOLID,0,RGB(0,0,255));
	static HPEN red_pen = CreatePen(PS_SOLID,0,RGB(255,0,0));

	SelectObject (hdc, CreateFontIndirect (&lf)) ;
	SelectObject (hdc, blue_pen) ;

	RECT rc = {0,0,CELL_SIZE,CELL_SIZE};

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
			if(oka == 90)
			{
				bFinished = true;
				break;
			}

			char text[2] = { aLetters[oka], '\0' };

			SIZE char_size;
			GetTextExtentPoint32(hdc, text, 1, &char_size);
			row_width += (char_size.cx + HORIZONTAL_OFFSET);

			if(row_width > TEST_WIDTH)
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
		rc.right = CELL_SIZE;
		OffsetRect(&rc, 0, max_char_height);
	}

	ROW_HEIGHT = max_char_height;
	BITMAP_HEIGHT = (rows * ROW_HEIGHT) + 0;
	
	if(BITMAP_HEIGHT < 8) BITMAP_HEIGHT = 8;
	else if(BITMAP_HEIGHT < 16) BITMAP_HEIGHT = 16;
	else if(BITMAP_HEIGHT < 32) BITMAP_HEIGHT = 32;
	else if(BITMAP_HEIGHT < 64) BITMAP_HEIGHT = 64;
	else if(BITMAP_HEIGHT < 128) BITMAP_HEIGHT = 128;
	else if(BITMAP_HEIGHT < 256) BITMAP_HEIGHT = 256;
	else if(BITMAP_HEIGHT < 512) BITMAP_HEIGHT = 512;
	else if(BITMAP_HEIGHT < 1024) BITMAP_HEIGHT = 1024;

	if(bShowGrid)
	{
		SelectObject (hdc, red_pen) ;

		MoveToEx(hdc,TEST_WIDTH,0,NULL);
		LineTo(hdc,TEST_WIDTH,BITMAP_HEIGHT);

		MoveToEx(hdc,0,BITMAP_HEIGHT,NULL);
		LineTo(hdc,TEST_WIDTH+1,BITMAP_HEIGHT);
	}
}

#define FILE_FILTER "TGA (*.tga)\0*.tga\0\0"

void SaveFontInfo(HDC hdc, HWND hwnd)
{
	TCHAR strName[MAX_PATH] = TEXT("");

	char strFilePath[512] = "";
	char strInitialDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, strInitialDir);

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

	char szFileName[512];
	sprintf(szFileName, "%s.fnt", ofn.lpstrFileTitle);

	FILE* pkFile = fopen(szFileName, "wb");
	fwrite(&ROW_HEIGHT, sizeof(int), 1, pkFile); // row height
	fwrite(&aLetterRects, sizeof(RECT), 90, pkFile); // letters
	fclose(pkFile);

	unsigned char bk_r = GetRValue(bkColor);
	unsigned char bk_g = GetGValue(bkColor);
	unsigned char bk_b = GetBValue(bkColor);
	
	Image file;
	file.CreateEmpty(TEST_WIDTH,BITMAP_HEIGHT);
	for(int y=0; y<BITMAP_HEIGHT; y++)
		for(int x=0; x<TEST_WIDTH; x++)
		{
			COLORREF color = GetPixel(hdc, x, y);

			unsigned char r = GetRValue(color);
			unsigned char g = GetGValue(color);
			unsigned char b = GetBValue(color);
			unsigned char a;

			if((r == bk_r && g == bk_g && b == bk_b))
				a = 0;
			else
				a = 255;

			file.set_pixel(x,BITMAP_HEIGHT-y, r, g, b, a); 
		}

	sprintf(szFileName, "%s.tga", ofn.lpstrFileTitle);

	file.Save(szFileName,true); 
}

static LRESULT CALLBACK WndProc (HWND khWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	PAINTSTRUCT ps;

	CHOOSEFONT cf;
	cf.lStructSize = sizeof (CHOOSEFONT) ;
   cf.hwndOwner   = khWnd ;
   cf.lpLogFont   = &lf ;
	cf.Flags       = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_LIMITSIZE;
	cf.rgbColors      = 0 ;
	cf.lCustData      = 0 ;
	cf.lpfnHook       = NULL ;
	cf.lpTemplateName = NULL ;
	cf.hInstance      = NULL ;
	cf.lpszStyle      = NULL ;
	cf.nFontType      = 0 ;      
	cf.nSizeMin       = 0 ;
	cf.nSizeMax       = 128 ;

	static CHOOSECOLOR cc ;
	static COLORREF    crCustColors[16] ;
	static HBRUSH bkBrush = CreateSolidBrush(RGB(255,255,255));

	cc.lStructSize    = sizeof (CHOOSECOLOR) ;
	cc.hwndOwner      = NULL ;
	cc.hInstance      = NULL ;
	cc.rgbResult      = bkColor ;
	cc.lpCustColors   = crCustColors ;
	cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
	cc.lCustData      = 0 ;
	cc.lpfnHook       = NULL ;
	cc.lpTemplateName = NULL ;

	bool bUpdate = false;

	char text[50];
	RECT rc;

	int sx = GetDeviceCaps(GetDC(NULL),HORZRES);
	int sy = GetDeviceCaps(GetDC(NULL),VERTRES);

	switch(uiMessage)
	{
	case WM_SHOWWINDOW:
		ChooseFont(&cf);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		hdc = BeginPaint (khWnd, &ps) ;
		DrawFont2(hdc);

		SelectObject (hdc, GetStockObject(DEFAULT_GUI_FONT) ) ;

		SetTextColor(hdc, RGB(0,0,0));

		SetRect(&rc, sx-120,sy-260,sx,sy);
		sprintf(text, "F5 = save TGA %s");
		DrawText(hdc, text, strlen(text), &rc, 0);

		SetRect(&rc, sx-120,sy-240,sx,sy);
		sprintf(text, "Esc = quit");
		DrawText(hdc, text, strlen(text), &rc, 0);

		SetRect(&rc, sx-120,sy-220,sx,sy);
		sprintf(text, "F1 = change font");
		DrawText(hdc, text, strlen(text), &rc, 0);

		SetRect(&rc, sx-120,sy-200,sx,sy);
		sprintf(text, "F2 = change bk color");
		DrawText(hdc, text, strlen(text), &rc, 0);

		SetRect(&rc, sx-120,sy-180,sx,sy);
		sprintf(text, "F3 = change font color");
		DrawText(hdc, text, strlen(text), &rc, 0);

		SetRect(&rc, sx-120,sy-160,sx,sy);
		sprintf(text, "+/- = resize width");
		DrawText(hdc, text, strlen(text), &rc, 0);

		SetRect(&rc, sx-120,sy-140,sx,sy);
		sprintf(text, "0 and ',' = Toggle grid");
		DrawText(hdc, text, strlen(text), &rc, 0);

		OffsetRect(&rc, 0, 20);
		sprintf(text, "HORZ OFFSET: %i", HORIZONTAL_OFFSET);
		DrawText(hdc, text, strlen(text), &rc, 0);

		OffsetRect(&rc, 0, 20);
		sprintf(text, "VERT OFFSET: %i", VERTICAL_OFFSET);
		DrawText(hdc, text, strlen(text), &rc, 0);

		OffsetRect(&rc, 0, 20);
		sprintf(text, "ROW HEIGHT: %i", ROW_HEIGHT);
		DrawText(hdc, text, strlen(text), &rc, 0);

		OffsetRect(&rc, 0, 20);
		sprintf(text, "WIDTH: %i", TEST_WIDTH);
		DrawText(hdc, text, strlen(text), &rc, 0);

		OffsetRect(&rc, 0, 20);
		sprintf(text, "HEIGHT: %i", BITMAP_HEIGHT);
		DrawText(hdc, text, strlen(text), &rc, 0);
			
		EndPaint (khWnd, &ps) ;
		break;

	case WM_ERASEBKGND:
		RECT rc;
		SetRect(&rc, 0, 0, sx, sy);
		FillRect((HDC) wParam, &rc, bkBrush);
		SetBkColor((HDC) wParam, bkColor);
		SetTextColor((HDC) wParam, textColor);
		return 1;
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
			DestroyWindow(khWnd);
			break;

		case VK_F1:
			ChooseFont(&cf);
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
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			SaveFontInfo(hdc, khWnd);
			
			bShowGrid = bState;
			g_bDrawCharGrid = bState2;
			bUpdate = true;
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		mclick.x = 55;
		mclick.y = 55;
		break;

	case WM_KEYUP:
		switch(wParam)
		{
		case VK_ADD:
			TEST_WIDTH *= 2;
			bUpdate = true;
			break;

		case VK_SUBTRACT:
			TEST_WIDTH /= 2;
			bUpdate = true;
			break;

		case VK_NUMPAD0:
			bShowGrid =! bShowGrid;
			bUpdate = true;
			break;

		case VK_DECIMAL:
			g_bDrawCharGrid = !g_bDrawCharGrid;
			bUpdate = true;
			break;
		}
		break;
	}

	if(bUpdate)
	{
		InvalidateRect(khWnd,NULL,TRUE);
		UpdateWindow(khWnd);
	}

	return DefWindowProc( khWnd, uiMessage, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
{
	static char szAppName[] = "ChatClient";

	WNDCLASSEX wndClass = 
	{ 
		sizeof(WNDCLASSEX), CS_CLASSDC | CS_VREDRAW | CS_HREDRAW, WndProc, 0, 0, 
		hInstance, 0, LoadCursor(NULL, IDC_ARROW), 
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, szAppName 
	};
	RegisterClassEx( &wndClass );

	int sx = GetDeviceCaps(GetDC(NULL),HORZRES);
	int sy = GetDeviceCaps(GetDC(NULL),VERTRES);
	const int STYLE  = WS_POPUPWINDOW  ^WS_BORDER | WS_VISIBLE ;
	HWND khWnd = CreateWindow (szAppName, szAppName, STYLE, 0, 0, sx, sy, 0, 0, hInstance, NULL);

	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	DestroyWindow(khWnd);

	return msg.wParam ;
}
