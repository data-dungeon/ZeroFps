// Window.h: interface for the Window class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOW_H__C9899268_8A6F_4E75_A4D7_60CADB518415__INCLUDED_)
#define AFX_WINDOW_H__C9899268_8A6F_4E75_A4D7_60CADB518415__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\engine_x.h"
#include "..\..\basic\rect.h"

#pragma warning( disable : 4786) // truncate long names, who gives a shit...
#include <list>
using namespace std;

class ZGui;
class ZGuiRender;
class ZGuiSkin;

const int NCODE_CLICK_UP	= 78;
const int NCODE_CLICK_DOWN	= 79;
const int NCODE_RELEASE		= 80;
const int NCODE_OVER_CTRL	= 81;
const int NCODE_DEFAULT		= 82; 
const int NCODE_MOVE		= 83;

class ENGINE_API ZGuiWnd  
{
public:
	void Resize(int Width, int Height);
	ZGuiSkin* GetSkin() { return m_pkSkin; }
	virtual void SetSkin(ZGuiSkin* pkSkin, int iMaskTexture = -1, int iBorderMaskTexture = -1);
	virtual void SetTextSkin(ZGuiSkin* kSkin, int iMaskTexture = -1);

	typedef bool (*callbackfunc)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	ZGuiWnd* Find(int x, int y);
	typedef list<ZGuiWnd*>::iterator WINit;
	typedef list<ZGuiWnd*>::reverse_iterator WINrit;
	
	ZGuiWnd(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0);
	virtual ~ZGuiWnd();

	bool AddChild(ZGuiWnd *pkWindow);					// Add a new childwindow
	bool SetParent(ZGuiWnd *pkWindow);					// Set the parent window
	ZGuiWnd* GetParent();								// Get the parent window
	bool SetPos(int x, int y, bool bScreenSpace=false, bool bFreeMovement=false);	// Move the window and change the size
	virtual bool Render(ZGuiRender* pkRender);						// Render this window
	virtual bool Notify(ZGuiWnd* pkWnd, int iCode) {return false;}
	unsigned int GetID() {return m_iID;}
	void SetGUI(ZGui* pkGui);
	ZGui* GetGUI();

	void Enable()	{ m_bEnabled = true;  }
	void Disable()	{ m_bEnabled = false; }

	void Show()		{ m_bVisible = true;  }
	void Hide()		{ m_bVisible = false; }

	bool IsVisible()	{ return m_bVisible; }
	void Move(int dx, int dy);

	virtual void CreateInternalControls() { /* do nothing */ }

	callbackfunc GetCallBack() { return m_pkCallback; }

	Rect GetMoveArea() { return m_kMoveArea; }
	Rect GetWndRect();		// Get the windows area, relative to it�s parent.
	Rect GetScreenRect();	// Get the real screen area.

	void SetMoveArea(Rect kScreenRect,bool bFreeMovement=false); // the rect is in screen space.
	void GetChildrens(list<ZGuiWnd*>& kList);
	virtual void SetText(char* strText);
	char* GetText() { return m_strText; }

protected:
	
	ZGuiWnd* m_pkParent;
	list<ZGuiWnd*> m_kChildList;
	ZGui* m_pkGUI;
	bool m_bVisible, m_bEnabled;
	callbackfunc m_pkCallback;
	ZGuiSkin* m_pkSkin, *m_pkTextSkin;
	char* m_strText;
	int m_iTextLength;

	// Kan vara 0 och �r i s� fall ett statisk f�nster.
	unsigned int m_iID;
	int m_iBkMaskTexture; // -1 inneb�r att det inte finns n�gon alpha textur
	int m_iTextMaskTexture; // -1 inneb�r att det inte finns n�gon alpha textur
	int m_iBorderMaskTexture; // -1 inneb�r att det inte finns n�gon alpha textur

private:
	void UpdatePos(int iPrevPosX, int iPrevPosY, int w, int h, bool bFreeMovement);
	Rect m_kArea, m_kMoveArea;

};

#endif // !defined(AFX_WINDOW_H__C9899268_8A6F_4E75_A4D7_60CADB518415__INCLUDED_)
