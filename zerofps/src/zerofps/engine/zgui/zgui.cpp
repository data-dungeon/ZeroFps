// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

// test

#include "../../basic/rect.h"
#include "zgui.h"
#include "zguiwindow.h"
#include "../../render/zguirenderer.h"
#include "../input.h"
#include "../../basic/zfassert.h"
#include "../zerofps.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGui::ZGui()
{
	m_iHighestZWndValue = 10;

	m_bActive = false;
	m_pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));
	m_pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkActiveMainWin = NULL;
	m_pkPrevMainWnd = NULL;
	m_bLeftButtonDown = false;
	m_bRightButtonDown = false;
	m_pnCursorRangeDiffX=m_pnCursorRangeDiffY=0;
	m_pkCursor = new ZGuiCursor();
	m_pkCursorSkin = new ZGuiSkin(-1, -1, -1, -1, 255, 255, 255, 0, 0, 0, 0);
	
	m_pkCursor->SetPos(0,0);
	m_pkCursor->SetSize(16,16);
	m_pkCursor->SetSkin(m_pkCursorSkin);

	ZGuiFont* pkDefaultFont = new ZGuiFont(16, 16, 0, ZG_DEFAULT_GUI_FONT);
	pkDefaultFont->CreateFromFile("../data/textures/text/font.bmp");
	m_pkFonts.insert( map<int, ZGuiFont*>::value_type(pkDefaultFont->m_iID, pkDefaultFont)); 
}

ZGui::~ZGui()
{

}

bool ZGui::RegisterWindow(ZGuiWnd* pkNewWindow)
{
	if(pkNewWindow == NULL)
		return false;

	if(GetWindow(pkNewWindow->GetID()))
	{
		ZFAssert(0, "Tried to register a window with a ID that already exist.");
	}

	pkNewWindow->SetGUI(this);
	if(pkNewWindow->GetFont() == NULL)
	{
		// Add stanadard font
		map<int, ZGuiFont*>::iterator itFont;
		itFont = m_pkFonts.find(ZG_DEFAULT_GUI_FONT);
		if(itFont != m_pkFonts.end())
			pkNewWindow->SetFont(itFont->second);
	}
	m_pkWindows.insert( map<int, ZGuiWnd*>::value_type(pkNewWindow->GetID(), pkNewWindow) ); 

	return true;
}

bool ZGui::UnregisterWindow(ZGuiWnd* pkWindow)
{
	if(pkWindow == NULL)
		return false;

	map<int, ZGuiWnd*>::iterator itWnd;
	itWnd = m_pkWindows.find(pkWindow->GetID());

	if(itWnd == m_pkWindows.end())
		return false;

	m_pkWindows.erase(itWnd);

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
	//m_pkMainWindows.sort();

	if(bSetAsActive)
		m_pkActiveMainWin = pkNewMainWindow;

	// Gå igenom alla childs och lägg till de till listan.
	list<ZGuiWnd*> kChildList;
	ZGuiWnd* pkSearchWnd = pkWindow;
	pkSearchWnd->GetChildrens(kChildList);
	
	for(WIN w=kChildList.begin(); 
		w != kChildList.end(); w++)
	{ 
		RegisterWindow((*w));
	}

	return true;
}

bool ZGui::RemoveMainWindow(int iMainWindowID)
{



	return 1;




	for( list<MAIN_WINDOW*>::iterator itMain = m_pkMainWindows.begin();
		 itMain != m_pkMainWindows.end(); itMain++ )
		 {
			if( (*itMain)->iID == iMainWindowID)
			{
				ZGuiWnd* pkWndMain = (*itMain)->pkWin;

				list<ZGuiWnd*> kChildList;
				pkWndMain->GetChildrens(kChildList);

				WIN it = kChildList.begin(); 

/*				if(kChildList.size() > 0)
				{
					//UnregisterWindow( (*it) );

					delete (*it);
					(*it) = NULL;

					kChildList.erase( it ); 

					//pkWndMain->RemoveChild( (*it) );	
				}*/

				delete (*itMain);
				(*itMain) = NULL;

				m_pkMainWindows.erase(itMain);				

				if(m_pkMainWindows.size() == 0)
					m_pkActiveMainWin = NULL;
				else
					m_pkActiveMainWin = m_pkMainWindows.back();

				break;
			}
		 }

	return true;
}

ZGuiWnd* ZGui::GetWindow(unsigned int iID)
{
	if(iID == 0) // statiskt fönster
		return NULL;

	map<int, ZGuiWnd*>::iterator itWnd;
	itWnd = m_pkWindows.find(iID);

	if(itWnd == m_pkWindows.end())
	{
		return NULL;
	}
	 
	return itWnd->second;
}

ZGuiWnd* ZGui::GetMainWindow(int iID)
{
	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if( (*it)->iID == iID)
				return (*it)->pkWin;
		 }

	return NULL;
}

// Rendera det aktiva fönstret (och alla dess childs)
bool ZGui::Render()
{
	m_pkRenderer->StartRender();

	m_pkRenderer->SetFont(GetBitmapFont(ZG_DEFAULT_GUI_FONT));
	
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

	// Draw cursor
	m_pkCursor->Render();

	m_pkRenderer->EndRender(); 

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
			if( (*it)->pkWin->IsVisible() && (*it)->pkWin->GetScreenRect().Inside(x,y) )
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

	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if((*it)->iZValue > Min)
				(*it)->iZValue = (*it)->iZValue-1;
		 }

	m_pkPrevMainWnd = m_pkActiveMainWin;
	m_pkActiveMainWin = p_iIDWndToSelect;

	// Sätt z-order till max på det fönster som har valts.
	m_pkActiveMainWin->iZValue = m_pkMainWindows.size()-1;
}

bool ZGui::OnMouseUpdate()
{
  	int x, y;
	m_pkInput->MouseXY(x,y);
	m_pkCursor->SetPos(x,y);

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

	ZGuiWnd* pkFocusWindow = m_pkActiveMainWin->pkWin->Find(x, y);

	if(pkFocusWindow == NULL)
		pkFocusWindow = m_pkActiveMainWin->pkWin;

	ZGuiWnd::m_pkWndUnderCursor = pkFocusWindow;

	// Har vänster musknapp klickats (men inte släppt)
	if(m_bLeftButtonDown == false && bLeftButtonDown == true)
	{		
		if(pkFocusWindow)
		{
			ZGuiWnd::m_pkWndClicked = pkFocusWindow;

			Rect rc = ZGuiWnd::m_pkWndClicked->GetScreenRect();
			
			if(ZGuiWnd::m_pkWndClicked->GetMoveArea() == rc)
			{
				m_iHighestZWndValue++;
				ZGuiWnd::m_pkWndClicked->SetZValue(m_iHighestZWndValue);
				ZGuiWnd* pkParent = ZGuiWnd::m_pkWndClicked->GetParent();
				
				if(pkParent) 
					pkParent->SortChilds();
				
				ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked, NCODE_CLICK_DOWN);
			}
			else
			{
				m_pnCursorRangeDiffX = x-rc.Left; 
				m_pnCursorRangeDiffY = y-rc.Top; 
			}
		}
		else
		{
			ZGuiWnd::m_pkWndClicked = NULL;
		}
	}

	// Är vänster musknapp nertryckt?
	if(bLeftButtonDown == true && ZGuiWnd::m_pkWndClicked != NULL)
	{	
		// Skall fönstret flyttas?
		if(!(ZGuiWnd::m_pkWndClicked->GetMoveArea() == ZGuiWnd::m_pkWndClicked->GetScreenRect()))
		{
			ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked, NCODE_MOVE);
			ZGuiWnd::m_pkWndClicked->SetPos(x-m_pnCursorRangeDiffX,y-m_pnCursorRangeDiffY,true,false);

			// Notify the main window that the window is moving
			if(m_bLeftButtonDown == true)
			{
				int* pkParams = new int[5];
				pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID();
				pkParams[1] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Left;
				pkParams[2] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Top;
				pkParams[3] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Right;
				pkParams[4] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Bottom;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked, ZGM_MOVING, 5, pkParams);
				delete[] pkParams;
			}
		}
	}

	// Har vänster musknapp släpts (men inte klickats)?
	if(m_bLeftButtonDown == true && bLeftButtonDown == false)
	{
		if(pkFocusWindow && ZGuiWnd::m_pkWndClicked != NULL)
		{
			// Informera fönstret innan att det har tappat fokus.
			if(ZGuiWnd::m_pkWndUnderCursor)
			{
				if(ZGuiWnd::m_pkPrevWndClicked && ZGuiWnd::m_pkPrevWndClicked != ZGuiWnd::m_pkWndUnderCursor)
				{
					if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
						ZGuiWnd::m_pkPrevWndClicked->Notify(ZGuiWnd::m_pkWndClicked, NCODE_RELEASE);
				}

				if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
					ZGuiWnd::m_pkPrevWndClicked = ZGuiWnd::m_pkWndUnderCursor;
			}

			// Är markören fortfarande innanför fönstrets gränser?
			if(ZGuiWnd::m_pkWndClicked->GetScreenRect().Inside(x, y))
			{
				// Notify the main window that the window have been clicked
				int* pkParams = new int[1];
				pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID(); // control id
				m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWin, ZGM_COMMAND, 1, pkParams);
				delete[] pkParams;

				SetFocus(ZGuiWnd::m_pkWndClicked);
				
				ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked, NCODE_CLICK_UP);
				ZGuiWnd::m_pkWndClicked = NULL;
			}
			else
			{
				if(ZGuiWnd::m_pkPrevWndUnderCursor)
					ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkPrevWndUnderCursor, NCODE_RELEASE);
			}
		}
	}

	// Är vänster musknapp inte nertryckt?
	if( m_bLeftButtonDown == false && bLeftButtonDown == false)
	{
		// Är det samma fönstret under musmarkören som innan?
		if(ZGuiWnd::m_pkWndUnderCursor != NULL)
		{
			if(ZGuiWnd::m_pkWndUnderCursor != ZGuiWnd::m_pkPrevWndUnderCursor)
			{
				ZGuiWnd::m_pkWndUnderCursor->Notify(ZGuiWnd::m_pkWndUnderCursor, NCODE_OVER_CTRL);

				if(ZGuiWnd::m_pkPrevWndUnderCursor)
					ZGuiWnd::m_pkPrevWndUnderCursor->Notify(ZGuiWnd::m_pkPrevWndUnderCursor, NCODE_DEFAULT);
				
				ZGuiWnd::m_pkPrevWndUnderCursor = ZGuiWnd::m_pkWndUnderCursor;
			}
		}
	}

	m_bLeftButtonDown = bLeftButtonDown;
	m_bRightButtonDown = bRightButtonDown;

	return true;
}

bool ZGui::OnKeyUpdate()
{
	int iKey = m_pkInput->GetQueuedKey();

	if(IgnoreKey(iKey))
		return true;

	if(iKey >= 0)
	{
		if(m_pkInput->Pressed(KEY_RSHIFT) || m_pkInput->Pressed(KEY_LSHIFT))
		{
			if(iKey > 96 && iKey < 123)
				iKey -= 32;
			if(iKey > 48 && iKey < 58)
				iKey -= 16;
			if(iKey == '0')
				iKey = '=';
			if(iKey == ',') 
				iKey = ';';
			if(iKey == '.')
				iKey = ':';
			if(iKey == '-')
				iKey = '_';
			
		}

		if(iKey == KEY_F10)
		{
			m_pkZeroFps->ToggleGui();
		}
		else
		if(iKey == KEY_ESCAPE)
		{
			if(m_pkActiveMainWin)
			{
				if(m_pkActiveMainWin->pkWin->GetWindowFlag(WF_CLOSEABLE))
				{
					m_pkActiveMainWin->pkWin->Hide();

					// Notify the window that it are going to hidden
					int* pkParams = new int[1];
					pkParams[0] = false; // window are beeing hidden
					m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWin, ZGM_SHOWWINDOW, 1, pkParams);
					delete[] pkParams;
				}
			}
		}
		if(ZGuiWnd::m_pkFocusWnd != NULL)
		{
			ZGuiWnd::m_pkFocusWnd->ProcessKBInput(iKey);
		}
	}

	return true;
}

void ZGui::SetFocus(ZGuiWnd* pkWnd)
{
	// Hitta det fönster som tidigare hade fokus och 
	// ta bort fokuset från denna.
	if(ZGuiWnd::m_pkFocusWnd)
	{		
		if(ZGuiWnd::m_pkFocusWnd)
			ZGuiWnd::m_pkFocusWnd->KillFocus();
	}

	m_pkInput->SetInputEnabled(true);

	ZGuiWnd::m_pkFocusWnd = pkWnd;
	ZGuiWnd::m_pkFocusWnd->SetFocus();
}

void ZGui::SetCursor(int TextureID, int MaskTextureID, int Width, int Height)
{
	m_pkCursorSkin->m_iBkTexID = TextureID;
	m_pkCursor->SetSkin(m_pkCursorSkin, MaskTextureID);
	m_pkCursor->SetSize(Width,Height);

	int x, y;
	m_pkInput->MouseXY(x,y);
	m_pkCursor->SetPos(x,y);
}

bool ZGui::IsActive()
{
	return m_bActive;
}

bool ZGui::Update()
{
	if(m_bActive == true)
	{
		m_pkInput->SetInputEnabled(true);

		OnMouseUpdate();
		OnKeyUpdate();

		m_pkInput->SetInputEnabled(false);

		Render();
	}

	return true;
}

bool ZGui::Activate(bool bActive)
{
	m_bActive = bActive;
	ShowCursor(m_bActive);
	return m_bActive;
}


void ZGui::ShowMainWindow(int iID, bool bShow)
{
	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if( (*it)->iID == iID)
			{
				if(bShow == true)
				{
					(*it)->pkWin->Show();
					RearrangeWnds((*it));
				}
				else
				{
					(*it)->pkWin->Hide();
					
					if(m_pkPrevMainWnd)
					{
						if(m_pkPrevMainWnd->pkWin->IsVisible())
							RearrangeWnds(m_pkPrevMainWnd);
					}
				}

				break;
			}
		 }
}

bool ZGui::IgnoreKey(int Key)
{
	switch(Key) 
	{
		case KEY_RSHIFT:
		case KEY_RCTRL:
		case KEY_LSHIFT:
		case KEY_LCTRL:
		case KEY_F12:
		case KEY_F11:
		//case KEY_F10: can´t ignore this key, it´s used to close the menu system.
		case KEY_F9:
		case KEY_F8:
		case KEY_F7:
		case KEY_F6:
		case KEY_F5:
		case KEY_F4:
		case KEY_F3:
		case KEY_F2:
		case KEY_F1:
		case KEY_PAGEUP:
		case KEY_PAGEDOWN:
		case KEY_HOME:
		case KEY_END:
		case KEY_TAB:
		case KEY_DELETE:
		case KEY_INSERT:
			return true;

		default:
			return false;
	};

}

ZGuiFont* ZGui::AddBitmapFont(char* strBitmapName, char cCharsOneRow, char cCellSize, char cPixelGapBetweenChars, int iID)
{
	ZGuiFont* pkNewFont = new ZGuiFont(cCharsOneRow,cCellSize,cPixelGapBetweenChars,iID);
	if(pkNewFont->CreateFromFile(strBitmapName))
	{
		m_pkFonts.insert( map<int, ZGuiFont*>::value_type(pkNewFont->m_iID, pkNewFont) ); 
		return pkNewFont;
	}

	return NULL;
}

ZGuiFont* ZGui::GetBitmapFont(int iID)
{
	map<int, ZGuiFont*>::iterator itFont;
	itFont = m_pkFonts.find(iID);
	if(itFont != m_pkFonts.end())
		return itFont->second;
	
	return NULL;
}
