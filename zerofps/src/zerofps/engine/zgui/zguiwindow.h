// Window.h: interface for the Window class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOW_H__C9899268_8A6F_4E75_A4D7_60CADB518415__INCLUDED_)
#define AFX_WINDOW_H__C9899268_8A6F_4E75_A4D7_60CADB518415__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/rect.h"
#include <functional>

#pragma warning( disable : 4786) // truncate long names, who gives a shit...
#include <list>
using namespace std;

class ZGui;
class ZGuiRender;
class ZGuiSkin;
class ZGuiFont;
class ZGuiResourceManager;

const int NCODE_CLICK_UP	= 78;
const int NCODE_CLICK_DOWN	= 79;
const int NCODE_RELEASE		= 80;
const int NCODE_OVER_CTRL	= 81;
const int NCODE_DEFAULT		= 82; 
const int NCODE_MOVE		= 83;

// Window flags
#define WF_CLOSEABLE		0x0001
#define WF_CENTER_TEXT		0x0002

class ENGINE_API ZGuiWnd  
{

public:

	ZGuiWnd(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiWnd();

	virtual void SetFont(ZGuiFont* pkFont);
	ZGuiFont* GetFont() { return m_pkFont; }

	void Resize(int Width, int Height);
	ZGuiSkin* GetSkin() { return m_pkSkin; }
	virtual void SetSkin(ZGuiSkin* pkSkin, int iBkMaskTexture = -1, int iBorderMaskTexture = -1);
	
	typedef bool (*callbackfunc)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	ZGuiWnd* Find(int x, int y);
	typedef list<ZGuiWnd*>::iterator WINit;
	typedef list<ZGuiWnd*>::reverse_iterator WINrit;
	
	bool RemoveChild( ZGuiWnd *pkWindow );
	bool AddChild(ZGuiWnd *pkWindow);					// Add a new childwindow
	bool SetParent(ZGuiWnd *pkWindow);					// Set the parent window
	ZGuiWnd* GetParent();								// Get the parent window
	bool SetPos(int x, int y, bool bScreenSpace=false, bool bFreeMovement=false);	// Move the window and change the size
	virtual bool Render(ZGuiRender* pkRender);						// Render this window
	virtual bool Notify(ZGuiWnd* pkWnd, int iCode) {return false;}
	unsigned int GetID() {return m_iID;}
	void SetGUI(ZGui* pkGui);

	void Enable()	{ m_bEnabled = true;  }
	void Disable()	{ m_bEnabled = false; }

	void Show();
	void Hide();

	virtual void SetFocus()	 { m_bHaveFocus = true;  }
	virtual void KillFocus() { m_bHaveFocus = false; }
	virtual bool ProcessKBInput(int nKey) { return false; }
	virtual bool IsInternalControl() { return m_bInternalControl; } // tex knappen på en scrollbar.
	virtual void CreateInternalControls() { /* do nothing */ }

	bool IsVisible()	{ return m_bVisible; }
	void Move(int dx, int dy, bool bScreenSpace=false, bool bFreeMovement=false);

	Rect GetMoveArea() { return m_kMoveArea; }
	Rect GetWndRect();		// Get the windows area, relative to it´s parent.
	Rect GetScreenRect();	// Get the real screen area.

	int m_iZValue;
	virtual void SetZValue(int iValue);
	void SortChilds();

	bool GetWindowFlag(unsigned long ulValue);
	void SetWindowFlag(unsigned long ulValue);
	void RemoveWindowFlag(unsigned long ulValue);

	void SetInternalControlState(bool IsInternalControl) { m_bInternalControl = IsInternalControl; }
	void SetMoveArea(Rect kScreenRect,bool bFreeMovement=false); // the rect is in screen space.
	void GetChildrens(list<ZGuiWnd*>& kList);
	virtual void SetText(char* strText);
	virtual char* GetText() { return m_strText; }
	ZGui* GetGUI();
	static void ResetStaticClickWnds(ZGuiWnd* pkWnd);

	static ZGuiWnd* m_pkPrevWndUnderCursor;
	static ZGuiWnd* m_pkPrevWndClicked;
	static ZGuiWnd* m_pkWndClicked;
	static ZGuiWnd* m_pkWndUnderCursor;

	static ZGuiWnd* m_pkFocusWnd; // window with the keyboard focus

	unsigned long m_ulFlags; // se lista

	struct SORTZ_CMP : public binary_function<ZGuiWnd*, ZGuiWnd*, bool> 
	{
		bool operator()(ZGuiWnd* x, ZGuiWnd* y) 
		{ 
			return x->m_iZValue > y->m_iZValue; 
		};
	} SortZCmp;

protected:
	
	ZGuiWnd* m_pkParent;
	list<ZGuiWnd*> m_kChildList;
	ZGui* m_pkGUI;
	bool m_bVisible, m_bEnabled;
	callbackfunc m_pkCallback;
	ZGuiSkin* m_pkSkin;//, *m_pkTextSkin;
	ZGuiFont* m_pkFont;
	char* m_strText;
	int m_iTextLength;

	// Kan vara 0 och är i så fall ett statisk fönster.
	unsigned int m_iID;
	int m_iBkMaskTexture; // -1 innebär att det inte finns någon alpha textur
	int m_iBorderMaskTexture; // -1 innebär att det inte finns någon alpha textur

private:
	void UpdatePos(int iPrevPosX, int iPrevPosY, int w, int h, bool bFreeMovement);
	Rect m_kArea, m_kMoveArea;
	bool m_bHaveFocus;
	bool m_bInternalControl;

};

#endif // !defined(AFX_WINDOW_H__C9899268_8A6F_4E75_A4D7_60CADB518415__INCLUDED_)


