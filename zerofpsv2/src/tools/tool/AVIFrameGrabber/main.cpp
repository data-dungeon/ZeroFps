
#include <windows.h>
#include "AVIFrameGrabber.h"
#include "bitmapmanager.h"
#include "resource.h"
#include <stdio.h>

#define FILE_FILTER_LOAD "AVI (*.avi)\0*.avi\0\0"
#define FILE_FILTER_SAVE "ZIF (*.zif)\0*.zif\0\0"

HINSTANCE g_kHInstance;
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK SettingsDlg (HWND, UINT, WPARAM, LPARAM) ;

AVIFrameGrabber g_kAVIGrabber;
BitmapManager g_kBitmapManager;

HDC hdcMem = CreateCompatibleDC(0);

int rows = 5;
int cols = 5;
int preview_size = 128;
int num_bitmaps = 0;
int startframe = 0;
int animationstart = 0;
int animationend = 100;
int frame_width=256;
int frame_height=256;

HWND ctrlWnd;
HBITMAP bitmaps[256];
char szAVIFileName[512];

void MakeFrame(int frame, FILE* pkFile);
void Rebuild(HWND hwnd);
void CreateAnimation(int animationstart, int animationend, HWND hwnd);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("AVIFrameGrabber") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1) ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
          return 0 ;

	 g_kHInstance = hInstance;
     
     hwnd = CreateWindow (szAppName, TEXT ("AVI Frame Grabber"), 
		 WS_OVERLAPPEDWINDOW | WS_MAXIMIZE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 
		 NULL, NULL, hInstance, NULL) ;        

	 ctrlWnd = CreateDialog(g_kHInstance, MAKEINTRESOURCE(IDD_SETTINGS), hwnd, SettingsDlg);

	 RECT rc;
	 GetWindowRect(hwnd, &rc);

	 SetWindowPos(ctrlWnd, NULL, rc.right-200, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
     
     ShowWindow(hwnd, SW_SHOWMAXIMIZED);
     UpdateWindow(hwnd);

	WPARAM wParam = MAKEWPARAM(ID_FILE_OPEN40002, 0);
	LPARAM lParam = MAKELPARAM(0, 0);
	SendMessage(hwnd, WM_COMMAND, wParam, lParam);
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

void DrawBitmap (HDC hdc, int x, int y, HBITMAP hBitmap, int frame)
{
     BITMAP bm ;
     HDC    hMemDC ;
     
     hMemDC = CreateCompatibleDC (hdc) ;
     SelectObject (hMemDC, hBitmap) ;
     GetObject (hBitmap, sizeof (BITMAP), &bm) ;
     
	 SetStretchBltMode(hdc, HALFTONE);
	 StretchBlt(hdc, x, y, preview_size, preview_size, hMemDC, 0, 0, 
		 frame_width, frame_height, SRCCOPY) ;

	 char text[20];
	 sprintf(text, "%i", frame); 
	 TextOut(hdc, x,y, text, strlen(text));
     
     DeleteDC (hMemDC) ;
}

bool GetFileName(char* szFileName, HWND hwnd, bool bSave)
{
	char strFileBuffer[512] = "";
	char strFilePath[512] = "";
	char strInitialDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, strInitialDir);

	char title[50];
	if(bSave)
		strcpy(title, "Save as");
	else
		strcpy(title, "Open");

	char* filter = (bSave == true) ? FILE_FILTER_SAVE : FILE_FILTER_LOAD;

    OPENFILENAME ofn = { sizeof(OPENFILENAME), hwnd, NULL,
						 filter, NULL, 0, 1, strFileBuffer, 
						 MAX_PATH, strFilePath, MAX_PATH, 
                         strInitialDir, title, 
						 OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, 
						 0, 1, NULL, 0, NULL, NULL };

	if(bSave)
	{
		if( FALSE == GetSaveFileName( &ofn ) )
			return false;	
	}
	else
	{
		if( FALSE == GetOpenFileName( &ofn ) )
			return false;
	}

	sprintf(szFileName, "%s", ofn.lpstrFileTitle);

	return true;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	HDC         hdc ;
	PAINTSTRUCT ps ;

	switch (message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			for(int i=0; i<256; i++)
				DeleteObject(bitmaps[i]);

			DestroyWindow(hwnd);
			break;

		case ID_FILE_SETTING:
			CreateDialog(g_kHInstance, MAKEINTRESOURCE(IDD_SETTINGS), hwnd, SettingsDlg);
			break;

		case ID_FILE_OPEN40002:

			char szFileName[512];
			if(GetFileName(szFileName, hwnd, false))
			{
				g_kAVIGrabber.CloseAVI();

				strcpy(szAVIFileName, szFileName);
				if(g_kAVIGrabber.OpenAVI(szAVIFileName)==false)
				{
					MessageBox(hwnd, "Failed to open AVI file", "Error", MB_ICONERROR);
					return 0;
				}

				SetDlgItemInt(ctrlWnd, IDC_FRAMES_TOTAL, g_kAVIGrabber.GetNumFrames(), FALSE); 

				char szTitle[150];
				sprintf(szTitle, "AVI Frame Grabber - %s", szAVIFileName);
				SetWindowText(hwnd, szTitle);

				Rebuild(hwnd);
			}

			break;
		}
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		if(num_bitmaps > 0)
		{
			int frame = 0;
			for(int y=0; y<rows; y++)
				for(int x=0; x<cols; x++)
				{
					DrawBitmap(hdc, x*(preview_size+1), y*(preview_size+1), 
						bitmaps[frame], startframe+frame);
					frame++;
				}
		}

		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

BOOL CALLBACK SettingsDlg (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
	int sz,c,r,fr,as,ae,fh,fw;

	switch (message)
	{
	case WM_INITDIALOG :
		SetDlgItemInt(hDlg, IDC_PICTURE_PREVIEW_SIZE, preview_size, FALSE);
		SetDlgItemInt(hDlg, IDC_COLS, cols, FALSE);
		SetDlgItemInt(hDlg, IDC_ROWS, rows, FALSE);
		SetDlgItemInt(hDlg, IDC_STARTFRAME, startframe, FALSE);
		SetDlgItemInt(hDlg, IDC_ANIMATIONSTART, animationstart, FALSE);
		SetDlgItemInt(hDlg, IDC_ANIMATIONEND, animationend, FALSE);
		SetDlgItemInt(hDlg, IDC_FRAMES_TOTAL, g_kAVIGrabber.GetNumFrames(), FALSE); 
		SetDlgItemInt(hDlg, IDC_FRAMEWIDTH, frame_width, FALSE); 
		SetDlgItemInt(hDlg, IDC_FRAMEHEIGHT, frame_height, FALSE); 

		SetDlgItemInt(hDlg, IDC_MEGABYTE, 
			((animationend - animationstart)*frame_width*frame_height*3+16) / 1000000, FALSE);
		return TRUE ;

	case WM_CLOSE:
		EndDialog (hDlg, 0) ;
		break;

	case WM_COMMAND :
		switch (LOWORD (wParam))
		{
		case IDOK :

			sz = GetDlgItemInt(hDlg, IDC_PICTURE_PREVIEW_SIZE, NULL, FALSE);
			c = GetDlgItemInt(hDlg, IDC_COLS, NULL, FALSE);
			r = GetDlgItemInt(hDlg, IDC_ROWS, NULL, FALSE);
			fr = GetDlgItemInt(hDlg, IDC_STARTFRAME, NULL, FALSE);
			as = GetDlgItemInt(hDlg, IDC_ANIMATIONSTART, NULL, FALSE);
			ae = GetDlgItemInt(hDlg, IDC_ANIMATIONEND, NULL, FALSE);
			fw = GetDlgItemInt(hDlg, IDC_FRAMEWIDTH, NULL, FALSE);
			fh = GetDlgItemInt(hDlg, IDC_FRAMEHEIGHT, NULL, FALSE);

			int prevlastframe;
			prevlastframe = startframe+(cols*rows);

			bool bOnlyRefresh;
			if(sz+(c*r) <= prevlastframe && sz >= startframe)
				bOnlyRefresh=true;
			else
				bOnlyRefresh=false;
			
			if(cols*rows < 256)
			{
				cols = c;
				rows = r;
			}

			if(sz < 512 && sz > 0)
				preview_size = sz;

			if(as > 0)
				animationstart = as;

			if(ae > 0)
				animationend = ae;

			if(fw==2||fw==8||fw==16||fw==32||fw==64||fw==128||fw==256||fw==512)
				frame_width = fw;
			if(fh==2||fh==8||fh==16||fh==32||fh==64||fh==128||fh==256||fh==512)
				frame_height = fh;

			startframe = fr;

			HWND parent;
			parent = GetParent(hDlg);

			if(bOnlyRefresh)
			{
				bOnlyRefresh=true;
				InvalidateRect(parent, NULL, TRUE);
				UpdateWindow(parent);
			}
			else
			{
				Rebuild(parent);
			}
			return TRUE ;

		case IDC_CREATEANIMATION:
			animationstart = GetDlgItemInt(hDlg, IDC_ANIMATIONSTART, NULL, FALSE);
			animationend = GetDlgItemInt(hDlg, IDC_ANIMATIONEND, NULL, FALSE);

			fw = GetDlgItemInt(hDlg, IDC_FRAMEWIDTH, NULL, FALSE);
			fh = GetDlgItemInt(hDlg, IDC_FRAMEHEIGHT, NULL, FALSE);

			if(fw==2||fw==8||fw==16||fw==32||fw==64||fw==128||fw==256||fw==512)
				frame_width = fw;
			if(fh==2||fh==8||fh==16||fh==32||fh==64||fh==128||fh==256||fh==512)
				frame_height = fh;

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			CreateAnimation(animationstart, animationend, hDlg);
			Rebuild(GetParent(hDlg)); // must rebuild
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;

		case IDC_ANIMATIONSTART:
		case IDC_ANIMATIONEND:
		case IDC_FRAMEWIDTH:
		case IDC_FRAMEHEIGHT:
			{
				int start = GetDlgItemInt(hDlg, IDC_ANIMATIONSTART, NULL, FALSE);
				int end = GetDlgItemInt(hDlg, IDC_ANIMATIONEND, NULL, FALSE);
				int w = GetDlgItemInt(hDlg, IDC_FRAMEWIDTH, NULL, FALSE);
				int h = GetDlgItemInt(hDlg, IDC_FRAMEHEIGHT, NULL, FALSE);

				int frames = end - start;
				int size = (frames*w*h*3+16) / 1000000;

				SetDlgItemInt(hDlg, IDC_MEGABYTE, size, FALSE);
			}
			break;
		}
		break ;
	}
	return FALSE ;
}


void MakeFrame(int frame, FILE* pkFile)
{
	const char szFileName[] = "frames/frame.bmp";

	char* pixels = g_kAVIGrabber.GetFramePixels(frame);

	int width = g_kAVIGrabber.GetWidth();
	int height = g_kAVIGrabber.GetHeight();

	g_kBitmapManager.SaveFile(szFileName, pixels, width, height);

	bitmaps[num_bitmaps] = (HBITMAP) LoadImage(g_kHInstance, szFileName, IMAGE_BITMAP, 
		frame_width, frame_height, LR_LOADFROMFILE);

	BITMAPINFO bitmapinfo;
	ZeroMemory(&bitmapinfo,sizeof(bitmapinfo));
	bitmapinfo.bmiHeader.biBitCount = 24;
	bitmapinfo.bmiHeader.biClrImportant = 0;
	bitmapinfo.bmiHeader.biClrUsed = 0;
	bitmapinfo.bmiHeader.biCompression = 0;
	bitmapinfo.bmiHeader.biHeight = frame_height;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biSize = 40;
	bitmapinfo.bmiHeader.biSizeImage = 0;
	bitmapinfo.bmiHeader.biWidth = frame_width;
	bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biYPelsPerMeter = 0;

	//char data[frame_width*frame_height*3];
	char* pData = new char[frame_width*frame_height*3];
	GetDIBits(hdcMem, bitmaps[num_bitmaps], 0, (WORD) bitmapinfo.bmiHeader.biHeight, 
		(void**)(pData), &bitmapinfo, DIB_RGB_COLORS);

	if(pkFile == NULL)
		g_kBitmapManager.SaveFile(szFileName, (char*)pData, frame_width, frame_height);
	else
		fwrite(pData, frame_width*frame_height*3, 1, pkFile);
	
	delete[] pData;
}

void Rebuild(HWND hwnd)
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	for(int i=0; i<256; i++)
		DeleteObject(bitmaps[i]);

	num_bitmaps = 0;

	int frame=0, max = g_kAVIGrabber.GetNumFrames();
	for(int y=0; y<rows; y++)
		for(int x=0; x<cols; x++)
		{
			if(frame < max)
			{
				MakeFrame(startframe+frame, NULL);
				frame++;
				num_bitmaps++;
			}
		}

	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);

	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CreateAnimation(int animationstart, int animationend, HWND hwnd)
{
	char szFileName[512];
	GetFileName(szFileName, hwnd, true);

	if(strstr(szFileName, ".zif") == NULL)
		strcat(szFileName, ".zif");

	g_kAVIGrabber.OpenAVI(szAVIFileName);
	SetDlgItemInt(ctrlWnd, IDC_FRAMES_TOTAL, g_kAVIGrabber.GetNumFrames(), FALSE); 

	FILE* pkFile = fopen(szFileName, "wb");
	if(pkFile == NULL)
		return;

	int w = frame_width;
	int h = frame_height;
	int frames_per_second = g_kAVIGrabber.GetFramesPerSecond();
	int frames = animationend-animationstart;

	fwrite(&w, sizeof(int), 1, pkFile);
	fwrite(&h, sizeof(int), 1, pkFile);
	fwrite(&frames_per_second, sizeof(int), 1, pkFile);
	fwrite(&frames, sizeof(int), 1, pkFile);

	for(int i=0; i<256; i++)
		DeleteObject(bitmaps[i]);

	num_bitmaps = 0;

	for(int i=animationstart; i<animationend; i++)
		MakeFrame(i, pkFile);

	fclose(pkFile);
}

