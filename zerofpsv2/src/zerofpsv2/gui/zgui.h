// Gui.h: interface for the Gui class.

#ifndef _GUI_ZGUI_H_
#define _GUI_ZGUI_H_

#include "gui_x.h"
#include "zguiwindow.h"
#include "zguibutton.h"
#include "zguicheckbox.h"
#include "zguilabel.h"
#include "zguiscrollbar.h"
#include "zguilistbox.h"
#include "zguicombobox.h"
#include "zguiradiobutton.h"
#include "zguicursor.h"
#include "zguitextbox.h"
#include "zguitreebox.h"
#include "zguitabctrl.h"
#include "zguislider.h"
#include "zguiapp.h"

#include "../basic/zfsystem.h"
#include "../basic/image.h"
#include "../basic/zguiskin.h"
#include "../basic/zfassert.h"
#include "../basic/zfini.h"

#pragma warning( disable : 4786) // truncate long names
#pragma warning( disable : 4251) // needs to have dll-interface

#include <list>
#include <vector>
#include <map>
using namespace std;

class ZGuiRender;
class ZGuiResourceManager;

/*
 * Window Messages
 */

#define ZGM_NULL                     0x0000
#define ZGM_DESTROY                  0x0002
#define ZGM_SETFOCUS						 0x0007
#define ZGM_CBN_SELENDOK				 0x0009
#define ZGM_KEYDOWN						 0x0100
#define ZGM_KEYUP							 0x0101
#define ZGM_COMMAND						 0x0111
#define ZGM_SHOWWINDOW					 0x0018
#define ZGM_MOUSEMOVE					 0x0200
#define ZGM_LBUTTONDOWN					 0x0201
#define ZGM_LBUTTONUP					 0x0202
#define ZGM_LBUTTONDBLCLK				 0x0203
#define ZGM_RBUTTONDOWN					 0x0204
#define ZGM_RBUTTONUP					 0x0205
#define ZGM_RBUTTONDBLCLK				 0x0206
#define ZGM_MOVING						 0x0216
#define ZGM_SCROLL						 0x0222
#define ZGM_EN_CHANGE					 0x0300
#define ZGM_SELECTLISTITEM				 0x0311
#define ZGM_SELECTTREEITEM				 0x0312
#define ZGM_TCN_SELCHANGE				 0x5501
#define ZGM_SCN_SETPOS					 0x5601

#define ZG_DEFAULT_GUI_FONT				 1

class GUI_API ZGui : public ZFSubSystem 
{
public:	
	enum ZndInfo {WNDPROC};

	void DrawLine(Point p1, Point p2);
	void DrawPoint(Point pos, unsigned char r, unsigned char g, unsigned char b);
	void DrawRect(int x, int y, int w, int h, 
		unsigned char r, unsigned char g, unsigned char b);
	
	ZGuiWnd* GetWndCapture();
	void KillWndCapture();
	void SetCaptureToWnd(ZGuiWnd* pkWnd);

	ZGuiWnd* GetMainWindowFromPoint(int x, int y);
	bool MouseHoverWnd();
	bool ChangeWndRegName(ZGuiWnd* pkWndToRename, const char* pkNewName);
	ZGuiSkin* GetFocusBorderSkin() { return m_pkFocusBorderSkin; }
	void SetDefaultFont(ZGuiFont* pkFont);
	void AddKeyCommand(int Key, ZGuiWnd* pkFocusWnd, ZGuiWnd* pkTriggerWnd);
	ZGuiFont* AddBitmapFont(char* strBitmapName, char cCharsOneRow, char cCellSize, char cPixelGapBetweenChars, int iID);
	ZGuiFont* GetBitmapFont(int iID);

	void ShowMainWindow(ZGuiWnd* pkMainWnd, bool bShow);
	bool Activate(bool bActive);
	void SetCursor(int x, int y, int TextureID, int MaskTextureID=-1, int Width=16, int Height=16);
	void ShowCursor(bool bShow, int x=-1, int y=-1);

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);
	typedef list<ZGuiWnd*>::iterator WINit;
	typedef pair<pair<string,string>, ZGuiSkin*> tSkinInf; //window_name, wnd_desc, skin
	typedef pair<Point,color_rgb> tRGBPoint;
	typedef pair<Rect,color_rgb> tRGBRect;

	ZGui(int iResX, int iResY);
	~ZGui();

	bool Update(float m_fGameTime, int iKeyPressed, bool bLastKeyStillPressed,
		bool bShiftIsPressed, int x, int y, bool bLBnPressed, bool bRBnPressed);
	bool IsActive();

	bool Render(); // Render the active main window

	ZGuiResourceManager* GetResMan();

	ZGuiWnd* GetWindow(unsigned int iID);

//	bool Resize(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight);
	bool AddMainWindow( int iID, ZGuiWnd* pkWindow, char* szName, callback cb, bool bSetAsActive);		// Add a new main window
	bool LoadDialog( char* szResourceFile, char* szWndResName, callback cb);

	bool UnregisterWindow(ZGuiWnd* pkWindow);
	bool RegisterWindow(ZGuiWnd* pkNewWindow, char* szName); // must be called if the window are created after the parent are created...
	ZGuiWnd* GetMainWindow(int iID);
	int GetMainWindowID(char* strWindowName);
	ZGuiWnd* GetActiveMainWnd() { if(m_pkActiveMainWin) return m_pkActiveMainWin->pkWnd; return NULL; }
	callback GetActiveCallBackFunc() 
	{ 
		if(m_pkActiveMainWin) 
			return m_pkActiveMainWin->pkCallback; 
		else
			return NULL;
	}
	void SetFocus(ZGuiWnd* pkWnd);

	struct MAIN_WINDOW
	{
		ZGuiWnd* pkWnd;
		callback pkCallback;
		int iID;
		int iZValue;
	};

	struct SORTZ_CMP : public binary_function<MAIN_WINDOW*, MAIN_WINDOW*, bool> 
	{
		bool operator()(MAIN_WINDOW* x, MAIN_WINDOW* y) 
		{ 
			return x->iZValue > y->iZValue; 
		};
	} SortZCmp;


	void* GetSpecialWndData(ZGuiWnd* pkWnd, ZndInfo type);

	MAIN_WINDOW* FindMainWnd(int x, int y);
	void GetResolution(int& x, int& y);

	int m_iMouseX, m_iMouseY;
	bool m_bMouseLeftPressed;
	
	bool m_bHaveInputFocus; //! om guit har focus eller inte

	void SetRes(int iResX, int iResY); // should only be called one time before app start!
	static int m_iResX;
	static int m_iResY;

private:
	void SetInputFocus(ZGuiWnd* pkClickWnd, bool bSet);
	//bool ResizeWnd(ZGuiWnd* pkWnd, int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight);
	bool SetSkins(vector<tSkinInf>& kAllSkinsArray, ZGuiWnd* pkWnd);
	bool RunKeyCommand(int iKey);
	void CreateDefaultSkins();
	ZGuiWnd* FindNextTabWnd(ZGuiWnd* pkCurrentWnd, bool bNext);
	long m_iHighestZWndValue;
	bool IgnoreKey(int Key);
	
	void OnKeyPress(int iKey);
	bool OnKeyUpdate(int iKeyPressed, bool bLastButtonStillPressed, bool bShiftIsPressed, float fGameTime);
	bool OnMouseUpdate(int x, int y, bool bLBnPressed, bool bRBnPressed, float fGameTime);
	
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	ZGuiCursor* m_pkCursor;
	ZGuiResourceManager* m_pkResManager;
	bool m_bHoverWindow;
	
	list<MAIN_WINDOW*> m_pkMainWindows; // A list of main windows
	MAIN_WINDOW* m_pkActiveMainWin;	// Pointer to the active main window

	map<int, ZGuiWnd*> m_pkWindows;

	bool m_bLeftButtonDown; // previus state of the left mouse button.
	bool m_bRightButtonDown; // previus state of the left mouse button.

	int m_pnCursorRangeDiffX, m_pnCursorRangeDiffY;
	ZGuiSkin* m_pkCursorSkin;
	ZGuiSkin* m_pkFocusBorderSkin;
	bool m_bActive;

	map<int, ZGuiFont*> m_pkFonts;
	map<pair<ZGuiWnd*, int>, ZGuiWnd*> m_KeyCommandTable;

	ZGuiWnd* m_pkCapturedWindow;

	vector<Point> m_kLinesToDraw;
	vector<tRGBPoint> m_kPointsToDraw;
	vector<tRGBRect> m_kRectsToDraw;

	bool StartUp();
	bool ShutDown();
	bool IsValid();

};

#endif 


