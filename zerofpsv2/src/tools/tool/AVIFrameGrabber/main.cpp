
#include <windows.h>
#include <commctrl.h>
#include "AVIFrameGrabber.h"
#include "bitmapmanager.h"
#include "resource.h"
#include <stdio.h>
#include "jpegdec.h"

#define FILE_FILTER_LOAD "AVI (*.avi)\0*.avi\0\0"
#define FILE_FILTER_SAVE "ZIF (*.zif)\0*.zif\0\0"

HINSTANCE g_kHInstance;
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK SettingsDlg (HWND, UINT, WPARAM, LPARAM) ;

AVIFrameGrabber g_kAVIGrabber;
BitmapManager g_kBitmapManager;
JpgDecoder g_kJpegDec;

HWND hwnd;
HDC hdcMem;

char szFileName[128] = "TestAnimation.zif";

bool bRedraw = false;
int preview_size = 256;
int num_bitmaps = 0;
int startframe = 0;
int animationstart = 0;
int animationend = 1;
int frames = 1;
int* g_pFrameSizesJPG = NULL;

bool print_frame = true;
float fStretchMod = 0.5f;

HWND ctrlWnd;
HBITMAP bitmaps[256];
char szAVIFileName[512];

bool CreateAnimation(int animationstart, int animationend, HWND hwnd);
void PrintSize();


BITMAPINFO bitmapinfo;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("AVIFrameGrabber") ;
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
	//EnableWindow(ctrlWnd, false);
	ShowWindow(ctrlWnd, true);

	RECT rc;
	GetWindowRect(hwnd, &rc);

	SetWindowPos(ctrlWnd, NULL, rc.right-200, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hwnd);

	hdcMem = CreateCompatibleDC (GetDC(hwnd)) ;

	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hwnd);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
     return msg.wParam ;
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
				strcpy(szAVIFileName, szFileName);

				g_kAVIGrabber.CloseAVI();

				if(g_kAVIGrabber.OpenAVI(szAVIFileName)==false)
				{
					MessageBox(hwnd, "Failed to open AVI file", "Error", MB_ICONERROR);
					EnableWindow(ctrlWnd, false);
					ShowWindow(ctrlWnd, SW_HIDE);
					return 0;
				}

				SetDlgItemInt(ctrlWnd, IDC_FRAMES_TOTAL, g_kAVIGrabber.GetNumFrames(), FALSE); 
				EnableWindow(ctrlWnd, true);
				ShowWindow(ctrlWnd, SW_SHOW);
				PrintSize();

				ZeroMemory(&bitmapinfo,sizeof(bitmapinfo));
				bitmapinfo.bmiHeader.biBitCount = 24;
				bitmapinfo.bmiHeader.biClrImportant = 0;
				bitmapinfo.bmiHeader.biClrUsed = 0;
				bitmapinfo.bmiHeader.biCompression = 0;
				bitmapinfo.bmiHeader.biHeight = g_kAVIGrabber.GetHeight();
				bitmapinfo.bmiHeader.biPlanes = 1;
				bitmapinfo.bmiHeader.biSize = 40;
				bitmapinfo.bmiHeader.biSizeImage = 0;
				bitmapinfo.bmiHeader.biWidth = g_kAVIGrabber.GetWidth();
				bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
				bitmapinfo.bmiHeader.biYPelsPerMeter = 0;

				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}

			break;
		}
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;

		if(g_kAVIGrabber.GetNumFrames() > 0)
		{
			SetStretchBltMode(hdc, HALFTONE);

			RECT rcWindow;
			GetWindowRect(hwnd, &rcWindow);

			int dw = (int)((float)g_kAVIGrabber.GetWidth()*fStretchMod);
			int dh = (int)((float)g_kAVIGrabber.GetHeight()*fStretchMod);

			int cols = (rcWindow.right - rcWindow.left) / dw ;
			int rows = (rcWindow.bottom - rcWindow.top) / dh ;

			int frame = startframe;
			int max = g_kAVIGrabber.GetNumFrames();
			
			for(int y=0; y<rows; y++)
				for(int x=0; x<cols; x++)
					if(frame < max)
					{
						char* pixels = g_kAVIGrabber.GetFramePixels(frame);
						StretchDIBits (hdc, x*dw, y*dh, dw, dh, 0, 0, 
							g_kAVIGrabber.GetWidth(), g_kAVIGrabber.GetHeight(), 
							pixels, &bitmapinfo, DIB_RGB_COLORS, SRCCOPY) ;
						frame++;
					}

			if(print_frame)
			{
				char text[20];
				for(int y=0; y<rows; y++)
					for(int x=0; x<cols; x++)
					{
						sprintf(text, "%i", startframe+(cols*y)+x);
						TextOut(hdc,x*dw,y*dh, text, (int) strlen(text));
					}
			}

			bRedraw = false;
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
	static HWND khWndProgressBar;
	int fr,as,ae;

	switch (message)
	{
	case WM_INITDIALOG :
		SetDlgItemInt(hDlg, IDC_PICTURE_PREVIEW_SIZE, preview_size, FALSE);
		SetDlgItemInt(hDlg, IDC_STARTFRAME, startframe, FALSE);
		SetDlgItemInt(hDlg, IDC_ANIMATIONSTART, animationstart, 0);
		SetDlgItemInt(hDlg, IDC_ANIMATIONEND, animationend, 0);
		
		khWndProgressBar = GetDlgItem(hDlg, IDC_PROGRESS1);

		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"400%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"200%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"100%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"75%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"50%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"25%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"15%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_ADDSTRING, 0, (LPARAM)"10%");
		SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_SETCURSEL, 4, 0);

		CheckDlgButton(hDlg,IDC_FORMAT0, BST_CHECKED);

		return TRUE ;

	case WM_CLOSE:
		EndDialog (hDlg, 0) ;
		bRedraw = true;
		break;

	case WM_COMMAND :
		switch (LOWORD (wParam))
		{

		case IDOK :

			fr = GetDlgItemInt(hDlg, IDC_STARTFRAME, NULL, FALSE);
			as = GetDlgItemInt(hDlg, IDC_ANIMATIONSTART, NULL, FALSE);
			ae = GetDlgItemInt(hDlg, IDC_ANIMATIONEND, NULL, FALSE);
			
			if(as > 0)
				animationstart = as;

			if(ae > 0)
				animationend = ae;

			startframe = fr;
			
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			
			return TRUE ;

		case IDC_CREATEANIMATION:
			animationstart = GetDlgItemInt(hDlg, IDC_ANIMATIONSTART, NULL, FALSE);
			animationend = GetDlgItemInt(hDlg, IDC_ANIMATIONEND, NULL, FALSE);

			SendMessage(khWndProgressBar, PBM_SETRANGE, 0, MAKELPARAM(animationstart, animationend)); 
			SendMessage(khWndProgressBar, PBM_SETSTEP, (WPARAM) 1, 0); 

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			CreateAnimation(animationstart, animationend, khWndProgressBar);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			break;

		case IDC_FORMAT0:
		case IDC_FORMAT1:
		case IDC_FORMAT2:
			PrintSize();
			break;
		}

		if( HIWORD(wParam) == EN_UPDATE)
		{
			switch(LOWORD(wParam))
			{
				case IDC_ANIMATIONSTART:
				case IDC_ANIMATIONEND:
					PrintSize();
					break;
			}
		}
		else
		if( HIWORD(wParam) == CBN_SELCHANGE)
		{
			switch(LOWORD(wParam))
			{
				case IDC_PREVIEW_SIZE_CB:
					int currsel;
					currsel = SendDlgItemMessage(hDlg, IDC_PREVIEW_SIZE_CB, CB_GETCURSEL, 0, 0);
					
					switch(currsel)
					{
						case 0: fStretchMod = 4; break;
						case 1: fStretchMod = 2; break;
						case 2: fStretchMod = 1; break;
						case 3: fStretchMod = 0.75f; break;
						case 4: fStretchMod = 0.5f; break;
						case 5: fStretchMod = 0.25f; break;
						case 6: fStretchMod = 0.15f; break;
						case 7: fStretchMod = 0.10f; break;
					}

					break;
			}
		}
	}
	return FALSE ;
}


bool CreateAnimation(int animationstart, int animationend, HWND hwnd)
{
	int num_frames = animationend-animationstart;

	if(num_frames > g_kAVIGrabber.GetNumFrames())
		return false;

	if(animationstart < 0 || animationend > g_kAVIGrabber.GetNumFrames())
		return false;

	FILE* pkFile = fopen(szFileName, "wb");

	int frames_per_second = g_kAVIGrabber.GetFramesPerSecond();
	
	unsigned char by8bitsFormat;

	if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT0) == BST_CHECKED)
		by8bitsFormat = 0;
	if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT1) == BST_CHECKED)
		by8bitsFormat = 1;
	if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT2) == BST_CHECKED)
	{
		by8bitsFormat = 2;

		if(g_pFrameSizesJPG)
			delete[] g_pFrameSizesJPG;

		g_pFrameSizesJPG = new int[num_frames];
	}

	int w = g_kAVIGrabber.GetWidth();
	int h = g_kAVIGrabber.GetHeight();

	fwrite(&w, sizeof(int), 1, pkFile);
	fwrite(&h, sizeof(int), 1, pkFile);
	fwrite(&frames_per_second, sizeof(int), 1, pkFile);
	fwrite(&num_frames, sizeof(int), 1, pkFile);
	fwrite(&by8bitsFormat, sizeof(unsigned char), 1, pkFile);

	// Hoppa in [NUM_FRAMES+headersize(17)] i filen, eftersom där skall skrivas storleken på varje frame
	if(by8bitsFormat == 2)
		fseek(pkFile, 17+ sizeof(int)*num_frames, SEEK_SET);

	int starTime = timeGetTime();

	long file_size_before, frame_counter=0;

	for(int i=animationstart; i<animationend; i++)
	{
		char* pixels = g_kAVIGrabber.GetFramePixels(i);
		
		if(by8bitsFormat == 2)
			file_size_before = ftell(pkFile);

		g_kBitmapManager.SaveFile(pkFile, pixels, w, h, ImageFormat(by8bitsFormat) );
			
		if(by8bitsFormat == 2)
			g_pFrameSizesJPG[frame_counter++] = ftell(pkFile)-file_size_before;
		
		SendMessage(hwnd, PBM_STEPIT, 0, 0);
	}

	// Hoppa tillbaks till strax efter headern på filen och skriv ner storleken på varje frame
	if(by8bitsFormat == 2)
	{
		fseek(pkFile, 17, SEEK_SET);
		fwrite(g_pFrameSizesJPG, num_frames, sizeof(int), pkFile);
	}


	// Testkod för att se så att jpeg decon funkar.
	//fclose(pkFile);
	//pkFile = fopen(szFileName,"rb");
	//fseek(pkFile, 17, SEEK_SET); 
	//unsigned int x, y;
	//if(g_kJpegDec.LoadHeader(pkFile, &x, &y))
	//{
	//	BYTE *our_image_buffer;
	//	g_kJpegDec.Decode();
	//	if (!g_kJpegDec.GetBuffer(x,y,&our_image_buffer)) 
	//	{
	//		__debugbreak();
	//	}
	//	g_kBitmapManager.SaveFile24("BITMAP_FROM_JPG_DATA.bmp", our_image_buffer, w, h);
	//}

	fclose(pkFile);
	 

	return true;

}

void PrintSize()
{
	int frames_per_sec = g_kAVIGrabber.GetFramesPerSecond(); 

	if(frames_per_sec == 0)
		return;

	int start = GetDlgItemInt(ctrlWnd, IDC_ANIMATIONSTART, NULL, FALSE);
	int end = GetDlgItemInt(ctrlWnd, IDC_ANIMATIONEND, NULL, FALSE);

	int w = g_kAVIGrabber.GetWidth();
	int h = g_kAVIGrabber.GetHeight();
	
	int frames = end - start;
	double size_in_meg;

	char text[20];

	if(frames < 10000)
	{
		if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT1) == BST_CHECKED)
			size_in_meg = (double)(frames*w*h+32) / 1048576.0;
		else
		if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT0) == BST_CHECKED)
			size_in_meg = (double)(frames*w*h*3+16) / 1048576.0;
		else
			size_in_meg = 0.0f;

		sprintf(text, "%.02f", size_in_meg);
		SetDlgItemText(ctrlWnd, IDC_MEGABYTE, text);
	}
	else
	{
		_int64 apa;

		apa = (_int64)frames;
		apa = apa*(_int64)w;
		apa = apa*(_int64)h;

		if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT0) == BST_CHECKED)
			apa = apa*3;

		if(IsDlgButtonChecked(ctrlWnd, IDC_FORMAT0) == BST_CHECKED)
			apa = apa + 16;
		else
			apa = apa + 32;

		apa = apa / 1048576;
		
		sprintf(text, "%i", (int) apa);
		SetDlgItemText(ctrlWnd, IDC_MEGABYTE, text);
	}

	sprintf(text, "%.02f", (double)(end-start) / (double)frames_per_sec);
	SetDlgItemText(ctrlWnd, IDC_TIME, text);

	sprintf(text, "%i", frames_per_sec);
	SetDlgItemText(ctrlWnd, IDC_FRAMES_PER_SEC, text);

	start = GetDlgItemInt(ctrlWnd, IDC_ANIMATIONSTART, NULL, FALSE);
	end = GetDlgItemInt(ctrlWnd, IDC_ANIMATIONEND, NULL, FALSE);
	SetDlgItemInt(ctrlWnd, IDC_NUMFRAMES, end-start, 0);
}






