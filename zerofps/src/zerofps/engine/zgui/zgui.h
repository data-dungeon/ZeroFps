// Gui.h: interface for the Gui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUI_H__9DDC0983_F616_469F_BDE9_BCC084BEB4BE__INCLUDED_)
#define AFX_GUI_H__9DDC0983_F616_469F_BDE9_BCC084BEB4BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/zguiskin.h"
#include "../../basic/zguifont.h"
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

#pragma warning( disable : 4786) // truncate long names
#include <list>
#include <vector>
#include <map>
using namespace std;

class ZGuiControl;
class ZGuiRender;
class Input;
class ZeroFps;
class ZGuiResourceManager;

/*
 * Window Messages
 */

#define ZGM_NULL                         0x0000
#define ZGM_DESTROY                      0x0002
#define ZGM_SETFOCUS					 0x0007
#define ZGM_CBN_SELENDOK				 0x0009
#define ZGM_KEYDOWN						 0x0100
#define ZGM_KEYUP						 0x0101
#define ZGM_COMMAND						 0x0111
#define ZGM_SHOWWINDOW					 0x0018
#define ZGM_MOUSEMOVE					 0x0200
#define ZGM_LBUTTONDOWN					 0x0201
#define ZGM_LBUTTONUP					 0x0202
#define ZGM_LBUTTONDBLCLK				 0x0203
#define ZGM_MOVING						 0x0216
#define ZGM_SCROLL						 0x0222
#define ZGM_EN_CHANGE					 0x0300
#define ZGM_SELECTLISTITEM				 0x0311

#define ZG_DEFAULT_GUI_FONT				 1

class ENGINE_API ZGui  
{
public:	

	ZGuiSkin* GetFocusBorderSkin() { return m_pkFocusBorderSkin; }
	void SetDefaultFont(ZGuiFont* pkFont);
	void AddKeyCommand(int Key, ZGuiWnd* pkFocusWnd, ZGuiWnd* pkTriggerWnd);
	ZGuiFont* AddBitmapFont(char* strBitmapName, char cCharsOneRow, char cCellSize, char cPixelGapBetweenChars, int iID);
	ZGuiFont* GetBitmapFont(int iID);

	void ShowMainWindow(/*int iID*/ZGuiWnd* pkMainWnd, bool bShow);
	bool Activate(bool bActive);
	void SetCursor(int TextureID, int MaskTextureID=-1, int Width=16, int Height=16);
	void ShowCursor(bool bShow);

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);
	typedef list<ZGuiWnd*>::iterator WINit;
	typedef pair<pair<string,string>, ZGuiSkin*> tSkinInf; //window_name, wnd_desc, skin

	ZGui();
	~ZGui();

	bool Update();
	bool IsActive();

	ZGuiResourceManager* GetResMan();

	ZGuiWnd* GetWindow(unsigned int iID);

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

private:
	bool SetSkins(vector<tSkinInf>& kAllSkinsArray, ZGuiWnd* pkWnd);
	bool RunKeyCommand(int iKey);
	void FormatKey(int& iKey);
	void CreateDefaultSkins();
	ZGuiWnd* FindNextTabWnd(ZGuiWnd* pkCurrentWnd, bool bNext);
	long m_iHighestZWndValue;
	bool IgnoreKey(int Key);
	bool Render(); // Render the active main window
	void OnKeyPress(int iKey);
	bool OnKeyUpdate();
	bool OnMouseUpdate();
	MAIN_WINDOW* FindMainWnd(int x, int y);
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	ZGuiCursor* m_pkCursor;
	ZGuiResourceManager* m_pkResManager;
	Input* m_pkInput;
	
	list<MAIN_WINDOW*> m_pkMainWindows; // A list of main windows
	MAIN_WINDOW* m_pkActiveMainWin;	// Pointer to the active main window

	map<int, ZGuiWnd*> m_pkWindows;

	bool m_bLeftButtonDown; // previus state of the left mouse button.
	bool m_bRightButtonDown; // previus state of the left mouse button.

	int m_pnCursorRangeDiffX, m_pnCursorRangeDiffY;
	ZGuiSkin* m_pkCursorSkin;
	ZGuiSkin* m_pkFocusBorderSkin;
	bool m_bActive;

	ZeroFps* m_pkFps;

	map<int, ZGuiFont*> m_pkFonts;
	map<pair<ZGuiWnd*, int>, ZGuiWnd*> m_KeyCommandTable;

};

#endif // !defined(AFX_GUI_H__9DDC0983_F616_469F_BDE9_BCC084BEB4BE__INCLUDED_)


