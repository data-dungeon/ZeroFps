// Gui.h: interface for the Gui class.

#ifndef _GUI_ZGUI_H_
#define _GUI_ZGUI_H_

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 
#pragma warning( disable : 4786) // truncate long names
#pragma warning( disable : 4251) // needs to have dll-interface

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
#include "zguitooltip.h"
#include "zguimenu.h"

#include "../basic/zfsystem.h"
#include "../basic/image.h"
#include "../basic/zguiskin.h"
#include "../basic/zifanimation.h"
#include "../basic/zfassert.h"
#include "../basic/zfini.h"

#include "../render/zshadersystem.h"

#include <list>
#include <vector>
#include <map>
using namespace std;

class ZGuiRender;
class ZGuiResourceManager;
class TextureManager;

/*
 * Window Messages
 */

#define ZGM_NULL                     0x0000
#define ZGM_DESTROY                  0x0002
#define ZGM_SETFOCUS						 0x0007
#define ZGM_CBN_SELENDOK				 0x0009
#define ZGM_KEYPRESS						 0x0100
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
#define ZG_DEFAULT_COLOR_FONT1			 2

/** \brief	Gui SubSystem
	 \ingroup Gui
*/
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
	void AddKeyCommand(int Key, ZGuiWnd* pkFocusWnd, ZGuiWnd* pkTriggerWnd);
	
	void ShowMainWindow(ZGuiWnd* pkMainWnd, bool bShow);
	bool Activate(bool bActive);
	void SetCursor(int x, int y, int TextureID, int MaskTextureID=-1, int Width=16, int Height=16);
	void ShowCursor(bool bShow);

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);
	typedef list<ZGuiWnd*>::iterator WINit;
	typedef pair<pair<string,string>, ZGuiSkin*> tSkinInf; //window_name, wnd_desc, skin
	typedef pair<Point,color_rgb> tRGBPoint;
	typedef pair<Rect,color_rgb> tRGBRect;

	ZGui(int iResX, int iResY);
	~ZGui();

	bool UpdateMouse(int x, int y, bool bLBnPressed, bool bRBnPressed, bool bMBnPressed, float fTime);

   struct KEY_INFO { int key; bool pressed; bool shift; };
   void UpdateKeys( vector<KEY_INFO>& kKeysPressed, float time);

	bool IsActive();

	bool Render(int fps); // Render the active main window

	ZGuiResourceManager* GetResMan();

	ZGuiWnd* GetWindow(unsigned int iID);

	ZGuiWnd* GetMenu() { return m_pkActiveMenu; }

	void SetLineColor(int r, int g, int b);
	bool AddMainWindow( int iID, ZGuiWnd* pkWindow, char* szName, callback cb, bool bSetAsActive);		// Add a new main window
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
		bool operator()(const MAIN_WINDOW* x, const MAIN_WINDOW* y) const
		{ 
			return x->pkWnd->m_iZValue > y->pkWnd->m_iZValue; 
		};
	} SortZCmp;

	ZGuiToolTip* GetToolTip();

	void* GetSpecialWndData(ZGuiWnd* pkWnd, ZndInfo type);

	MAIN_WINDOW* FindMainWnd(int x, int y);
	void GetResolution(int& x, int& y);
	bool ClickedWndAlphaTex(int mx, int my, ZGuiWnd* pkWndClicked);

	int m_iMouseX, m_iMouseY;
	bool m_bMouseLeftPressed;

	bool m_bDisableAlphatest;
	bool m_bUseHardwareMouse;  // true = snabb, go mus

	void SetWndForeground(ZGuiWnd* pkWnd);
	void SetRes(int iResX, int iResY); // should only be called one time before app start!
	static int m_iResX;
	static int m_iResY;

	bool m_bHandledMouse;		//DVOID WAS HERE =D, litet hack för att veta om guit har hanterat inputen
	bool m_bForceGUICapture; // Anropas av textboxen
	int m_iShowFPSCounter;

	void ShowFPSCounter(bool bShow);

	ZGuiWnd* m_pkFpsWnd;
	ZGuiLabel* m_pkFpsLabel;

	bool PlaceWndFrontBack(ZGuiWnd* pkMainWnd, bool bFront);

private:

	void FormatKey(int& iKey, bool bShiftIsPressed);
	bool SetSkins(vector<tSkinInf>& kAllSkinsArray, ZGuiWnd* pkWnd);
	bool RunKeyCommand(int iKey);
	ZGuiWnd* FindNextTabWnd(ZGuiWnd* pkCurrentWnd, bool bNext);
	long m_iHighestZWndValue;
	unsigned char m_acLineColor[3];

	bool OnMouseUpdate(int x, int y, bool bLBnPressed, 
      bool bRBnPressed, bool bMBnPressed, float fGameTime);
	
	void TranslateMousePos(int &x, int &y);
	
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	ZGuiCursor* m_pkCursor;
	ZGuiResourceManager* m_pkResManager;
	TextureManager* m_pkTexMan;
	ZShaderSystem*	m_pkZShaderSystem;
	ZGuiToolTip* m_pkToolTip;
	ZGuiMenu* m_pkActiveMenu; // den meny som senast klickades
	bool m_bClickedMenu;
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

	bool		m_bRenderEnabled;
	
	map<pair<ZGuiWnd*, int>, ZGuiWnd*> m_KeyCommandTable;

	ZGuiWnd* m_pkCapturedWindow;

	vector<Point> m_kLinesToDraw;
	vector<tRGBPoint> m_kPointsToDraw;
	vector<tRGBRect> m_kRectsToDraw;

	bool StartUp();
	bool ShutDown();
	bool IsValid();

public:
	void CloseActiveMenu(void);
};

#endif 


