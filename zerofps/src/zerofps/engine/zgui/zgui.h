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

#pragma warning( disable : 4786) // truncate long names, who gives a shit...
#include <list>
#include <map>
using namespace std;

class ZGuiControl;
class ZGuiRender;
class Input;

/*
 * Window Messages
 */

#define ZGM_NULL                         0x0000
#define ZGM_DESTROY                      0x0002
#define ZGM_MOVING						 0x0216
#define ZGM_COMMAND						 0x0111
#define ZGM_SCROLL						 0x0222
#define ZGM_SELECTLISTITEM				 0x0311
#define ZGM_CBN_SELENDOK				 9

class ENGINE_API ZGui  
{
public:
	bool ToogleGui();
	void SetCursor(int TextureID, int MaskTextureID=-1, int Width=16, int Height=16);
	void ShowCursor(bool bShow) { m_pkCursor->Show(bShow); }

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);
	typedef list<ZGuiWnd*>::iterator WIN;

	ZGui(/*int uiScreenWidth=800, int uiScreenHeight=600, Input* pkInput=NULL*/);
	~ZGui();

	//bool Initialize(int uiScreenWidth, int uiScreenHeight, Input* pkInput);
	bool Update();

	ZGuiWnd* GetWindow(unsigned int iID);

	bool RemoveMainWindow(int iMainWindowID);
	bool AddMainWindow( int iID, ZGuiWnd* pkWindow, callback cb = NULL, bool bSetAsActive = false);		// Add a new main window
	bool SetActiveMainWindow(int iID);									// Select a new active main window

	bool SetMainWindowCallback( int iID, callback cb = NULL );					// Set a callback function for a specific window

	bool UnregisterWindow(ZGuiWnd* pkWindow);
	bool RegisterWindow(ZGuiWnd* pkNewWindow); // must be called if the window are created after the parent are created...
	ZGuiWnd* GetMainWindow(int iID);
	ZGuiWnd* GetActiveMainWnd() { if(m_pkActiveMainWin) return m_pkActiveMainWin->pkWin; return NULL; }
	callback GetActiveCallBackFunc() 
	{ 
		if(m_pkActiveMainWin) 
			return m_pkActiveMainWin->pkCallback; 
		else
			return NULL;
	}

	struct MAIN_WINDOW
	{
		ZGuiWnd* pkWin;
		callback pkCallback;
		int iID;
		int iZValue;
	};

private:
	bool Render(); // Render the active main window
	void SetFocus(ZGuiWnd* pkWnd);
	bool OnKeyUpdate(/*unsigned long nKey*/);
	bool OnMouseUpdate(/*int x, int y, bool bLeftButtonDown, bool bRightButtonDown*/);
	void RearrangeWnds(MAIN_WINDOW* p_iIDWndToSelect);
	MAIN_WINDOW* ChangeMainWindow(int x, int y);
	ZGuiRender* m_pkRenderer;		// Pointer to the gui render object
	ZGuiCursor* m_pkCursor;
	Input* m_pkInput;
	
	list<MAIN_WINDOW*> m_pkMainWindows; // A list of main windows
	MAIN_WINDOW* m_pkActiveMainWin;	// Pointer to the active main window

	map<int, ZGuiWnd*> m_pkWindows;
	//int m_uiScreenWidth, m_uiScreenHeight;
	bool m_bLeftButtonDown; // previus state of the left mouse button.
	bool m_bRightButtonDown; // previus state of the left mouse button.

//	ZGuiWnd* m_pkWndClicked, *m_pkWndUnderCursor;
//	ZGuiWnd* m_pkFocusWnd; // window with the keyboard focus

	int m_pnCursorRangeDiffX, m_pnCursorRangeDiffY;
	ZGuiSkin* m_pkCursorSkin;
	bool m_bActive;
};

#endif // !defined(AFX_GUI_H__9DDC0983_F616_469F_BDE9_BCC084BEB4BE__INCLUDED_)


