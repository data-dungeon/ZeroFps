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
#define ZGM_SHOWWINDOW					 0x0018
#define ZGM_MOVING						 0x0216
#define ZGM_COMMAND						 0x0111
#define ZGM_SCROLL						 0x0222
#define ZGM_SELECTLISTITEM				 0x0311
#define ZGM_CBN_SELENDOK				 9

#define ZG_DEFAULT_GUI_FONT				 1

class ENGINE_API ZGui  
{
public:	
	void SetDefaultFont(ZGuiFont* pkFont);
	void AddKeyCommand(int Key, ZGuiWnd* pkFocusWnd, ZGuiWnd* pkTriggerWnd);
	ZGuiFont* AddBitmapFont(char* strBitmapName, char cCharsOneRow, char cCellSize, char cPixelGapBetweenChars, int iID);
	ZGuiFont* GetBitmapFont(int iID);

	void ShowMainWindow(int iID, bool bShow);
	bool Activate(bool bActive);
	void SetCursor(int TextureID, int MaskTextureID=-1, int Width=16, int Height=16);
	void ShowCursor(bool bShow);

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);
	typedef list<ZGuiWnd*>::iterator WIN;

	ZGui();
	~ZGui();

	bool Update();
	bool IsActive();

	ZGuiWnd* GetWindow(unsigned int iID);

	bool AddMainWindow( int iID, ZGuiWnd* pkWindow, callback cb = NULL, bool bSetAsActive = false);		// Add a new main window
	bool SetMainWindowCallback( int iID, callback cb = NULL );					// Set a callback function for a specific window

	bool UnregisterWindow(ZGuiWnd* pkWindow);
	bool RegisterWindow(ZGuiWnd* pkNewWindow); // must be called if the window are created after the parent are created...
	ZGuiWnd* GetMainWindow(int iID);
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

	/*	bool operator<(const MAIN_WINDOW* x)
		{
			return this->iZValue < x->iZValue;
		}*/
	};

	struct SORTZ_CMP : public binary_function<MAIN_WINDOW*, MAIN_WINDOW*, bool> 
	{
		bool operator()(MAIN_WINDOW* x, MAIN_WINDOW* y) 
		{ 
			return x->iZValue > y->iZValue; 
		};
	} SortZCmp;

private:
	long m_iHighestZWndValue;
	bool IgnoreKey(int Key);
	bool Render(); // Render the active main window
	bool OnKeyUpdate();
	bool OnMouseUpdate();
	MAIN_WINDOW* FindMainWnd(int x, int y);
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	ZGuiCursor* m_pkCursor;
	ZGuiResourceManager* m_pkResManager;
	Input* m_pkInput;
	
	list<MAIN_WINDOW*> m_pkMainWindows; // A list of main windows
	MAIN_WINDOW* m_pkActiveMainWin;	// Pointer to the active main window
	MAIN_WINDOW* m_pkPrevMainWnd;

	map<int, ZGuiWnd*> m_pkWindows;

	bool m_bLeftButtonDown; // previus state of the left mouse button.
	bool m_bRightButtonDown; // previus state of the left mouse button.

	int m_pnCursorRangeDiffX, m_pnCursorRangeDiffY;
	ZGuiSkin* m_pkCursorSkin;
	bool m_bActive;

	ZeroFps* m_pkZeroFps;

	map<int, ZGuiFont*> m_pkFonts;
	map<pair<ZGuiWnd*, int>, ZGuiWnd*> m_KeyCommandTable;

};

#endif // !defined(AFX_GUI_H__9DDC0983_F616_469F_BDE9_BCC084BEB4BE__INCLUDED_)


