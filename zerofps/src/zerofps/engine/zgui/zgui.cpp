// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

/*

  	int x, y;
	m_pkInput->MouseXY(x, y);

	bool bLeftButtonDown = m_pkInput->Pressed(MOUSELEFT);
	bool bRightButtonDown = m_pkInput->Pressed(MOUSERIGHT);

  */

#include "../../basic/rect.h"
#include "zgui.h"
#include "zguiwindow.h"
#include "../../render/zguirenderer.h"
#include "../input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ZGui::ZGui(/*int uiScreenWidth, int uiScreenHeight, Input* pkInput*/)
{
	//m_pkInput = pkInput;
	m_pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));
	m_pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_pkActiveMainWin = NULL;
	m_pkFocusWnd = NULL;
/*	m_uiScreenWidth = uiScreenWidth;
	m_uiScreenHeight = uiScreenHeight;*/
	m_pkWndClicked = NULL;
	m_pkWndUnderCursor = NULL;
	m_bLeftButtonDown = false;
	m_bRightButtonDown = false;
	m_pnCursorRangeDiffX=m_pnCursorRangeDiffY=0;
}

ZGui::~ZGui()
{

}

/*bool ZGui::Initialize(int uiScreenWidth, int uiScreenHeight, Input* pkInput)
{
	m_uiScreenWidth = uiScreenWidth;
	m_uiScreenHeight = uiScreenHeight;
	m_pkInput = pkInput;
	return true;
}*/

bool ZGui::Update()
{
	if(!OnMouseUpdate())
		return false;
	if(!OnKeyUpdate())
		return false;

	Render();

	return true;
}

bool ZGui::RegisterWindow(ZGuiWnd* pkNewWindow)
{
	if(pkNewWindow == NULL)
		return false;

	pkNewWindow->SetGUI(this);
	m_pkWindows.insert( map<int, ZGuiWnd*>::value_type(pkNewWindow->GetID(), pkNewWindow) ); 

	return true;
}

// Lägg till ett huvudfönster
bool ZGui::AddMainWindow(int iMainWindowID, ZGuiWnd* pkWindow, callback cb, bool bSetAsActive)
{
	// Ett main window skall inte ha någon parent!
	pkWindow->SetParent(NULL);
	
	MAIN_WINDOW* pkNewMainWindow = new MAIN_WINDOW;
	pkNewMainWindow->iID = iMainWindowID;
	pkNewMainWindow->pkCallback = cb;
	pkNewMainWindow->pkWin = pkWindow;

	static int iZValueCounter;
	pkNewMainWindow->iZValue = iZValueCounter++;

	m_pkMainWindows.push_back(pkNewMainWindow);
	m_pkMainWindows.sort();

	if(bSetAsActive)
		m_pkActiveMainWin = pkNewMainWindow;

	// Gå igenom alla childs och lägg till de till listan

	list<ZGuiWnd*> kChildList;
	ZGuiWnd* pkSearchWnd = pkWindow;
	pkSearchWnd->GetChildrens(kChildList);
	
	for(WIN w=kChildList.begin(); 
		w != kChildList.end(); w++)
	{ 
		RegisterWindow((*w));
	}

	m_pkFocusWnd = pkWindow;

	return true;
}

ZGuiWnd* ZGui::GetWindow(unsigned int iID)
{
	if(iID == 0) // statiskt fönster
		return NULL;

	map<int, ZGuiWnd*>::iterator itWnd;
	itWnd = m_pkWindows.find(iID);

	if(itWnd == m_pkWindows.end())
		return NULL;
	 
	return itWnd->second;
}

/*bool ZGui::SetRenderer(ZGuiRender* pkRenderer)
{
	m_pkRenderer = pkRenderer;
	m_pkActiveMainWin = NULL;
	return true;
}*/

// Rendera det aktiva fönstret (och alla dess childs)
bool ZGui::Render()
{
	m_pkRenderer->StartRender();
	
/*	if(m_pkActiveMainWin)
		if(m_pkActiveMainWin->pkWin)
			return m_pkActiveMainWin->pkWin->Render(m_pkRenderer);
*/
/*	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			(*it)->pkWin->Render(m_pkRenderer);
		 }*/

	// Blit windows	with lowest z order first.
	int l_iNumWindows = m_pkMainWindows.size();
	int l_iCurrZOrder = 0;

	while(1)
	{
		for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
			if((*it)->iZValue == l_iCurrZOrder)
			{
				(*it)->pkWin->Render(m_pkRenderer);
				l_iCurrZOrder++;
				break;
			}

			if(l_iCurrZOrder > l_iNumWindows-1 )
				break;
	}

	m_pkRenderer->EndRender(); 

	return true;
}

bool ZGui::SetActiveMainWindow(int iMainWindowID)
{
	// Find the window
	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if( (*it)->iID == iMainWindowID)
				m_pkActiveMainWin = (*it);
		 }

	return true;
}

bool ZGui::SetMainWindowCallback( int iMainWindowID, callback cb )
{
	// Find the window
	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if( (*it)->iID == iMainWindowID)
				m_pkActiveMainWin->pkCallback = cb;
		 }

	return true;	
}

ZGui::MAIN_WINDOW* ZGui::ChangeMainWindow(int x, int y)
{
	MAIN_WINDOW* best = m_pkActiveMainWin;

	// Find the window
	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if( (*it)->pkWin->GetScreenRect().Inside(x,y) )
			{
				if( best->pkWin->GetScreenRect().Inside(x,y) )
				{
					if( (*it)->iZValue > best->iZValue )
						best = (*it);
				}
				else
				{
					best = (*it);
				}
			}
		 }	

	return best;
}


void ZGui::RearrangeWnds(MAIN_WINDOW* p_iIDWndToSelect)
{
	// Har samma fönster valts är det bara att returnera.
	if(m_pkActiveMainWin->iID == p_iIDWndToSelect->iID)
		return;

	MAIN_WINDOW* pActiveWnd = m_pkActiveMainWin;
	MAIN_WINDOW* pWndToBeActivated = p_iIDWndToSelect;

	// Minska Z-order på alla fönster som har högre z-order än pWndToBeActivated
	int Min = pWndToBeActivated->iZValue;	

	for( list<MAIN_WINDOW*>::reverse_iterator it = m_pkMainWindows.rbegin();
		 it != m_pkMainWindows.rend(); it++ )
		 {
			if((*it)->iZValue > Min)
				(*it)->iZValue = (*it)->iZValue-1;
		 }

	m_pkActiveMainWin = p_iIDWndToSelect;

	// Sätt z-order till max på det fönster som har valts.
	m_pkActiveMainWin->iZValue = m_pkMainWindows.size()-1;
}

bool ZGui::OnMouseUpdate(/*int x, int y, bool bLeftButtonDown, bool bRightButtonDown*/)
{

  	int x, y;
	m_pkInput->MouseXY(x, y);

	bool bLeftButtonDown = m_pkInput->Pressed(MOUSELEFT);
	bool bRightButtonDown = m_pkInput->Pressed(MOUSERIGHT);

	if(m_pkActiveMainWin == NULL)
		return false;

	// Skall vi byta main window?
	MAIN_WINDOW* wnd;
	if((bLeftButtonDown && m_bLeftButtonDown==false) && (wnd = ChangeMainWindow(x,y)))
	{
		RearrangeWnds(wnd);
	}

	static ZGuiWnd* pkPrevWndUnderCursor = NULL;

	ZGuiWnd* pkFocusWindow = m_pkActiveMainWin->pkWin->Find(x, y);

	if(pkFocusWindow == NULL)
		pkFocusWindow = m_pkActiveMainWin->pkWin;

	m_pkWndUnderCursor = pkFocusWindow;

	// Har vänster musknapp klickats (men inte släppt)
	if(m_bLeftButtonDown == false && bLeftButtonDown == true)
	{
		if(pkFocusWindow)
		{
			m_pkWndClicked = pkFocusWindow;

			Rect rc = m_pkWndClicked->GetScreenRect();
			
			if(m_pkWndClicked->GetMoveArea() == rc)
			{
				m_pkWndClicked->Notify(m_pkWndClicked, NCODE_CLICK_DOWN);
			}
			else
			{
				m_pnCursorRangeDiffX = x-rc.Left; 
				m_pnCursorRangeDiffY = y-rc.Top; 
			}
		}
		else
		{
			m_pkWndClicked = NULL;
		}
	}

	// Är vänster musknapp nertryckt?
	if(bLeftButtonDown == true && m_pkWndClicked != NULL)
	{
		// Skall fönstret flyttas?
		if(!(m_pkWndClicked->GetMoveArea() == m_pkWndClicked->GetScreenRect()))
		{
			m_pkWndClicked->Notify(m_pkWndClicked, NCODE_MOVE);
			m_pkWndClicked->SetPos(x-m_pnCursorRangeDiffX,y-m_pnCursorRangeDiffY,true,false);

			// Notify the main window that the window is moving
			if(m_bLeftButtonDown == true)
			{
				int* pkParams = new int[5];
				pkParams[0] = m_pkWndClicked->GetID();
				pkParams[1] = m_pkWndClicked->GetScreenRect().Left;
				pkParams[2] = m_pkWndClicked->GetScreenRect().Top;
				pkParams[3] = m_pkWndClicked->GetScreenRect().Right;
				pkParams[4] = m_pkWndClicked->GetScreenRect().Bottom;
				m_pkActiveMainWin->pkCallback(m_pkWndClicked, ZGM_MOVING, 5, pkParams);
				delete[] pkParams;
			}
		}
	}

	// Har vänster musknapp släpts (men inte klickats)?
	if(m_bLeftButtonDown == true && bLeftButtonDown == false)
	{
		if(pkFocusWindow && m_pkWndClicked != NULL)
		{
			// Är markören fortfarande innanför fönstrets gränser?
			if(m_pkWndClicked->GetScreenRect().Inside(x, y))
			{
				// Notify the main window that the window have been clicked
				int* pkParams = new int[1];
				pkParams[0] = m_pkWndClicked->GetID(); // control id
				m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWin, ZGM_COMMAND, 1, pkParams);
				delete[] pkParams;

				SetFocus(m_pkWndClicked);
				
				m_pkWndClicked->Notify(m_pkWndClicked, NCODE_CLICK_UP);
				m_pkWndClicked = NULL;
			}
			else
			{
				m_pkWndClicked->Notify(m_pkWndClicked, NCODE_RELEASE);
			}
		}
	}

	// Är vänster musknapp inte nertryckt?
	if( m_bLeftButtonDown == false && bLeftButtonDown == false)
	{
		// Är det samma fönstret under musmarkören som innan?
		if(m_pkWndUnderCursor != NULL)
		{
			if(m_pkWndUnderCursor != pkPrevWndUnderCursor)
			{
				m_pkWndUnderCursor->Notify(m_pkWndUnderCursor, NCODE_OVER_CTRL);

				if(pkPrevWndUnderCursor)
					pkPrevWndUnderCursor->Notify(pkPrevWndUnderCursor, NCODE_DEFAULT);
				
				pkPrevWndUnderCursor = m_pkWndUnderCursor;
			}
		}
	}

	m_bLeftButtonDown = bLeftButtonDown;
	m_bRightButtonDown = bRightButtonDown;

	return true;
}

bool ZGui::OnKeyUpdate(/*unsigned long nKey*/)
{
	unsigned long nKey = m_pkInput->GetQueuedKey();

	if(m_pkFocusWnd != NULL)
	{
		m_pkFocusWnd->ProcessKBInput(nKey);
		return true;
	}

	return true;
}

void ZGui::SetFocus(ZGuiWnd* pkWnd)
{
	// Hitta det fönster som tidigare hade fokus och 
	// ta bort fokuset från denna.
	if(m_pkFocusWnd)
	{		
		if(m_pkFocusWnd)
			m_pkFocusWnd->KillFocus();
	}
}









