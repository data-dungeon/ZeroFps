#include <windows.h>
#include <stdio.h>

bool bShowGrid = false;
static LOGFONT    lf;

int ROW_WIDTH = 16;
int CELL_SIZE = 16;
int BITMAP_SIZE = 256;

void DrawFont(HDC hdc)
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
	static COLORREF bkColor = RGB(255,255,255), textColor = RGB(0,0,0);
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

	char text[50];
	RECT rc;

	int sx = GetDeviceCaps(GetDC(NULL),HORZRES);
	int sy = GetDeviceCaps(GetDC(NULL),VERTRES);

	switch(uiMessage)
	{
	case WM_CREATE:
		ChooseFont(&cf);
		break;

	case WM_DESTROY:
		{
			FILE* f = fopen("data.txt", "w+t");
			fprintf(f, "Row width: %i\nCell size: %i\n", ROW_WIDTH, CELL_SIZE);
			fclose(f);
			PostQuitMessage(0);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint (khWnd, &ps) ;
			DrawFont(hdc);

			SelectObject (hdc, GetStockObject(DEFAULT_GUI_FONT) ) ;

			SetRect(&rc, sx-120,sy-200,sx,sy);
			sprintf(text, "Press PrintScr to save");
			SetTextColor(hdc, RGB(128,0,0));
			DrawText(hdc, text, strlen(text), &rc, 0);
			SetTextColor(hdc, RGB(0,0,0));

			SetRect(&rc, sx-120,sy-180,sx,sy);
			sprintf(text, "Esc = quit");
			DrawText(hdc, text, strlen(text), &rc, 0);

			SetRect(&rc, sx-120,sy-160,sx,sy);
			sprintf(text, "F1 = change font");
			DrawText(hdc, text, strlen(text), &rc, 0);

			SetRect(&rc, sx-120,sy-140,sx,sy);
			sprintf(text, "F2 = change bk color");
			DrawText(hdc, text, strlen(text), &rc, 0);

			SetRect(&rc, sx-120,sy-120,sx,sy);
			sprintf(text, "F3 = change font color");
			DrawText(hdc, text, strlen(text), &rc, 0);

			SetRect(&rc, sx-120,sy-100,sx,sy);
			sprintf(text, "+/- = resize cells");
			DrawText(hdc, text, strlen(text), &rc, 0);

			SetRect(&rc, sx-120,sy-80,sx,sy);
			sprintf(text, "0 (Num) = Toggle grid");
			DrawText(hdc, text, strlen(text), &rc, 0);

			SetRect(&rc, sx-120,sy-60,sx,sy);
			sprintf(text, "CELL_SIZE: %i", CELL_SIZE);
			DrawText(hdc, text, strlen(text), &rc, 0);

			OffsetRect(&rc, 0, 20);
			sprintf(text, "ROW_WIDTH: %i", ROW_WIDTH);
			DrawText(hdc, text, strlen(text), &rc, 0);

			OffsetRect(&rc, 0, 20);
			sprintf(text, "BITMAP_SIZE: %i", BITMAP_SIZE);
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
		case VK_ESCAPE:
			DestroyWindow(khWnd);
			break;

		case VK_F1:
			ChooseFont(&cf);
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			break;

		case VK_F2:
			cc.rgbResult = bkColor;
			ChooseColor(&cc);
			bkColor = cc.rgbResult;
			bkBrush = CreateSolidBrush(bkColor);
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			break;

		case VK_F3:
			cc.rgbResult = textColor;
			ChooseColor(&cc);
			textColor = cc.rgbResult;
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			break;
		}
		break;

	case WM_KEYUP:
		switch(wParam)
		{
		case VK_ADD:
			CELL_SIZE++;
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			break;

		case VK_SUBTRACT:
			CELL_SIZE--;
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			break;

		case VK_NUMPAD0:
			bShowGrid =! bShowGrid;
			InvalidateRect(khWnd,NULL,TRUE);
			UpdateWindow(khWnd);
			break;
		}
		break;
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

  //  MSG  msg;
  //  while( WM_QUIT != msg.message  )
  //  {
  //      GetMessage( &msg, NULL, 0U, 0U );
		//TranslateMessage( &msg );
		//DispatchMessage( &msg );
  //  }	


	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	DestroyWindow(khWnd);

	return msg.wParam ;

//	DestroyWindow(khWnd);

	return 0;
}
