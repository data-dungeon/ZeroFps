// Window.h: interface for the Window class.

#ifndef _GUI_ZGUIWINDOW_H_
#define _GUI_ZGUIWINDOW_H_

#include "gui_x.h"
#include "../basic/rect.h"
#include <functional>

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 
#pragma warning( disable : 4786) // truncate long names
#pragma warning( disable : 4251) // needs to have dll-interface

#include <list>
#include <string>
#include <vector>
using namespace std;

class ZGui;
class ZGuiRender;
class ZGuiSkin;
class ZGuiFont;
class ZGuiResourceManager;
class I_Camera;

const int NCODE_CLICK_UP	= 78;
const int NCODE_CLICK_DOWN	= 79;
const int NCODE_RELEASE		= 80;
const int NCODE_OVER_CTRL	= 81;
const int NCODE_DEFAULT		= 82; 
const int NCODE_MOVE		= 83;

// Window flags
#define WF_CLOSEABLE			1 //0x0001
#define WF_CENTER_TEXT		2 //0x0002
#define WF_CANHAVEFOCUS		4 //0x0003
#define WF_TOPWINDOW			8 //0x0004	// Kan detta fönster användas som mainwindow?

/** \brief	This is the basetype for all gui controllers and also the mainwnd type used as a parent for all other gui controllers.
	 \ingroup Gui
*/
class GUI_API ZGuiWnd  
{

public:

	virtual void CopyNonUniqueData(const ZGuiWnd* pkSrc);

	ZGuiWnd(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, 
		int iID=0);

	virtual void SetFont(ZGuiFont* pkFont);
	ZGuiFont* GetFont() { return m_pkFont; }

	virtual void Resize(int Width, int Height, bool bChangeMoveArea=true);
	ZGuiSkin* GetSkin() { return m_pkSkin; }
	virtual void SetSkin(ZGuiSkin* pkSkin);
	void SetSkin(const char* szSkinType, ZGuiSkin* pkSkin); // namnet som skickas in är ett av de namn som finns i SKIN_DESC vektorn...
	
	typedef bool (*callbackfunc)(ZGuiWnd* pkWnd, unsigned int uiMessage, 
		int iNumParams, void *pParams);

	ZGuiWnd* Find(int x, int y);
	typedef list<ZGuiWnd*>::iterator WINit;
	typedef list<ZGuiWnd*>::reverse_iterator WINrit;
	
	bool RemoveChild( ZGuiWnd *pkWindow );
	bool AddChild(ZGuiWnd *pkWindow);					// Add a new childwindow
	bool SetParent(ZGuiWnd *pkWindow);					// Set the parent window
	ZGuiWnd* GetParent(bool bRootParent=false);		// Get the parent window
	virtual bool SetPos(int x, int y, bool bScreenSpace=false, bool bFreeMovement=false);	// Move the window and change the size
	virtual bool Render(ZGuiRender* pkRender);			// Render this window
	virtual bool Notify(ZGuiWnd* pkWnd, int iCode) {return false;}
	unsigned int GetID() {return m_iID;}
	void SetID(int iID) {m_iID = iID;}
	void SetGUI(ZGui* pkGui);

	int GetTabOrderNr() { return m_iTabOrderNumber; }
	void SetTabOrderNr(int iNumber) { m_iTabOrderNumber = iNumber; }

	void Enable();
	void Disable();

	void Show();
	void Hide();

	virtual void SetFocus(bool bSetCapture=true)	 { m_bHaveFocus = true;  }
	virtual void KillFocus() { m_bHaveFocus = false; }
	virtual bool ProcessKBInput(int nKey) { return false; }
	virtual bool IsInternalControl() { return m_bInternalControl; } 
		// tex knappen på en scrollbar.
	virtual void CreateInternalControls() { /* do nothing */ }

	const bool Enabled()	{ return m_bEnabled; }
	const bool IsVisible(bool bSearchParents=true);
	void Move(int dx, int dy, bool bScreenSpace=false, bool bFreeMovement=false);

	Rect GetMoveArea() { return m_kMoveArea; }
	Rect GetWndRect();		// Get the windows area, relative to it´s parent.
	Rect GetScreenRect(bool bIncBorder=false);	// Get the real screen area.

	int m_iZValue;
	virtual void SetZValue(int iValue);
	void SortChilds();

	bool GetWindowFlag(unsigned long ulValue);
	void SetWindowFlag(unsigned long ulValue);
	void RemoveWindowFlag(unsigned long ulValue);
	const char* GetName() { return m_szName; }; 

	void SetInternalControlState(bool IsInternalControl) 
	{ 
		m_bInternalControl = IsInternalControl; 
	}
	void SetMoveArea(Rect kScreenRect,bool bFreeMovement=false); 
		// the rect is in screen space.

	void GetChildrens(list<ZGuiWnd*>& kList);
	virtual void SetText(char* strText, bool bResizeWnd=false);
	virtual char* GetText() { return m_strText; }
	virtual void SetTextColor(unsigned char ucR, unsigned char ucG, unsigned char ucB);
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
		bool operator()(const ZGuiWnd* x, const ZGuiWnd* y) const 
		{ 
			return x->m_iZValue > y->m_iZValue; 
		};
	} SortZCmp;

	typedef pair<ZGuiSkin**, string> SKIN_DESC;

	virtual void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const;
	void SetClipperArea(Rect rc, bool bEnable=true);

	bool m_bUseClipper;
	bool m_bUseAlhpaTest; ///< true om systemet skall ta hänsyn till om man har klickat på en alpha yta
	bool m_bAcceptRightClicks;

   int m_iWndAlignment, m_iResizeType;

	//virtual bool Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight);

	virtual void SetResizeFlags(bool bHorz, bool bVert);

	void SetRenderTarget(I_Camera* pkCam);
	I_Camera* GetRenderTarget() {	return m_pkCamera;  }

protected:

	I_Camera*	m_pkCamera;		// Render taget if any.

	int m_iResolutionX, m_iResolutionY;
	
	ZGuiResourceManager* m_pkResMan;
	ZGuiWnd* m_pkParent;
	list<ZGuiWnd*> m_kChildList;
	ZGui* m_pkGUI;
	bool m_bVisible, m_bEnabled;
	callbackfunc m_pkCallback;
	ZGuiSkin* m_pkSkin;
	ZGuiFont* m_pkFont;
	char* m_strText;
	int m_iTextLength;
	float m_afTextColor[3]; // r,g,b [0-1] (only if font is TGA)
	Rect m_kClipperArea; ///< Pixels outside this area will not be rendered
	bool m_bResizeHorz, m_bResizeVert; ///< om fönstret skall skall skalas om horizontellt/vertikalt när skärmen byter upplösning
	
	char m_szName[50];	// Same name as the resource manager use to find the window.
	unsigned int m_iID;	// Kan vara 0 och är i så fall ett statisk fönster.

	virtual ~ZGuiWnd();	// Set the destructor to protected and and let class ZGui
	friend class ZGui;	// be a friend so that only ZGui can destroy windows
								// (the only way to destroy a window is with the func.
								// named 'UnregisterWindow').

private:
	void UpdatePos(int iPrevPosX, int iPrevPosY, int w, int h, bool bFreeMovement);
	Rect m_kArea, m_kMoveArea;
	bool m_bHaveFocus;
	bool m_bInternalControl;
	int m_iTabOrderNumber;

//	ZGuiResourceManager* m_pkGuiMan;
//	float afBkColorBuffer[3];

};

#endif 


