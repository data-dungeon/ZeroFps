// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

#include "../../basic/rect.h"
#include "zgui.h"
#include "zguiwindow.h"
#include "../../render/zguirenderer.h"
#include "../input.h"
#include "../../basic/zfassert.h"
#include "../zerofps.h"
#include <typeinfo>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGui::ZGui()
{
	m_iHighestZWndValue = 10;

	m_bActive = false;
	m_pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));
	m_pkRenderer = static_cast<ZGuiRender*>(
		g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkResManager = static_cast<ZGuiResourceManager*>(
		g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
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
	m_pkFonts.insert( map<int, ZGuiFont*>::value_type(pkDefaultFont->m_iID, 
		pkDefaultFont)); 
}

ZGui::~ZGui()
{

}

bool ZGui::RegisterWindow(ZGuiWnd* pkNewWindow)
{
	if(pkNewWindow == NULL)
		return false;

/*	if(GetWindow(pkNewWindow->GetID()))
	{
		ZFAssert(0, "Tried to register a window with a ID that already exist.");
	}*/

	pkNewWindow->SetGUI(this);
	if(pkNewWindow->GetFont() == NULL)
	{
		// Add stanadard font
		map<int, ZGuiFont*>::iterator itFont;
		itFont = m_pkFonts.find(ZG_DEFAULT_GUI_FONT);
		if(itFont != m_pkFonts.end())
			pkNewWindow->SetFont(itFont->second);
	}
	m_pkWindows.insert( map<int, ZGuiWnd*>::value_type(pkNewWindow->GetID(), 
		pkNewWindow) ); 

	return true;
}

bool ZGui::UnregisterWindow(ZGuiWnd* pkWindow)
{
	if(pkWindow == NULL)
		return false;

	// Ta först bort eventuellt main window
	for( list<MAIN_WINDOW*>::iterator itMain = m_pkMainWindows.begin();
		 itMain != m_pkMainWindows.end(); itMain++ )
		 {
			if( (*itMain)->pkWnd == pkWindow)
			{
				if(m_pkActiveMainWin == (*itMain))
					m_pkActiveMainWin = NULL;

				delete (*itMain);
				m_pkMainWindows.erase(itMain);	
				
				MAIN_WINDOW* best = NULL;
				int heighest = -1;
				for( list<MAIN_WINDOW*>::iterator itMain2 = 
					m_pkMainWindows.begin();
					 itMain2 != m_pkMainWindows.end(); itMain2++ )
					 {
						if((*itMain2)->iZValue > heighest && 
							(*itMain2)->pkWnd->IsVisible())
						{
							best = (*itMain2);
							heighest = best->iZValue;
						}
					 }

				if(best)
					SetFocus(best->pkWnd);

				break;
			}
		 }

	map<int, ZGuiWnd*>::iterator itWnd;
	itWnd = m_pkWindows.find(pkWindow->GetID());

	if(itWnd != m_pkWindows.end())
	{
		delete pkWindow;
		pkWindow = NULL;
		m_pkWindows.erase(itWnd);
	}
	else
	{
		cout << "Failed to remove window!" << endl;
	}

	return true;
}

// Lägg till ett huvudfönster
bool ZGui::AddMainWindow(int iMainWindowID, ZGuiWnd* pkWindow, 
						 callback cb, bool bSetAsActive)
{
	// Ett main window skall inte ha någon parent!
	pkWindow->SetParent(NULL);
	
	MAIN_WINDOW* pkNewMainWindow = new MAIN_WINDOW;
	pkNewMainWindow->iID = iMainWindowID;
	pkNewMainWindow->pkCallback = cb;
	pkNewMainWindow->pkWnd = pkWindow;
	pkNewMainWindow->iZValue = 0;

	m_pkMainWindows.push_back(pkNewMainWindow);

	RegisterWindow(pkWindow);

	// Gå igenom alla childs och lägg till de till listan.
	list<ZGuiWnd*> kChildList;
	ZGuiWnd* pkSearchWnd = pkWindow;
	pkSearchWnd->GetChildrens(kChildList);
	
	for(WIN w=kChildList.begin(); 
		w != kChildList.end(); w++)
		RegisterWindow((*w));

	if(bSetAsActive)
		SetFocus(pkNewMainWindow->pkWnd);

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
				return (*it)->pkWnd;
		 }

	return NULL;
}

// Rendera det aktiva fönstret (och alla dess childs)
bool ZGui::Render()
{
	m_pkRenderer->StartRender();

	m_pkRenderer->SetFont(GetBitmapFont(ZG_DEFAULT_GUI_FONT));
	
	// Blit windows	with lowest z order first.
	for( list<MAIN_WINDOW*>::reverse_iterator it = m_pkMainWindows.rbegin();
	 it != m_pkMainWindows.rend(); it++ )
			(*it)->pkWnd->Render(m_pkRenderer);

	// Render a yellow frame around the window that have focus.
	if(ZGuiWnd::m_pkFocusWnd)
	{
		static ZGuiSkin FOCUS_BORDER(-1,-1,-1,-1,0,0,0,128,128,0,2);
		m_pkRenderer->SetSkin(&FOCUS_BORDER);
		m_pkRenderer->RenderBorder(ZGuiWnd::m_pkFocusWnd->GetScreenRect());
	}

	// Draw cursor
	if(m_pkCursor->IsVisible())
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

ZGui::MAIN_WINDOW* ZGui::FindMainWnd(int x, int y)
{
	MAIN_WINDOW* best = m_pkActiveMainWin;

	// Find the window
	for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++ )
		 {
			if( (*it)->pkWnd->IsVisible() && 
				(*it)->pkWnd->GetScreenRect().Inside(x,y) )
			{
				if( best->pkWnd->GetScreenRect().Inside(x,y) )
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
	if( (bLeftButtonDown && m_bLeftButtonDown==false) && 
		(wnd = FindMainWnd(x,y)))
	{
		if(wnd != m_pkActiveMainWin)
		{
			cout << "apa" << endl;
			SetFocus(wnd->pkWnd);
		}
	}

	ZGuiWnd* pkFocusWindow = m_pkActiveMainWin->pkWnd->Find(x, y);

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
				static long iHighestZWndValue=10;
				iHighestZWndValue++;
				ZGuiWnd::m_pkWndClicked->SetZValue(iHighestZWndValue);
				ZGuiWnd* pkParent = ZGuiWnd::m_pkWndClicked->GetParent();
				
				if(pkParent) 
					pkParent->SortChilds();
				
				ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked, 
					NCODE_CLICK_DOWN);
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
		if(!(ZGuiWnd::m_pkWndClicked->GetMoveArea() == 
			ZGuiWnd::m_pkWndClicked->GetScreenRect()))
		{
			ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked, NCODE_MOVE);
			ZGuiWnd::m_pkWndClicked->SetPos(x-m_pnCursorRangeDiffX,y-
				m_pnCursorRangeDiffY,true,false);

			// Notify the main window that the window is moving
			if(m_bLeftButtonDown == true)
			{
				int* pkParams = new int[5];
				pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID();
				pkParams[1] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Left;
				pkParams[2] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Top;
				pkParams[3] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Right;
				pkParams[4] = ZGuiWnd::m_pkWndClicked->GetScreenRect().Bottom;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked, 
					ZGM_MOVING, 5, pkParams);
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
				if( ZGuiWnd::m_pkPrevWndClicked && 
					ZGuiWnd::m_pkPrevWndClicked != ZGuiWnd::m_pkWndUnderCursor)
					if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
						ZGuiWnd::m_pkPrevWndClicked->Notify(
							ZGuiWnd::m_pkWndClicked, NCODE_RELEASE);

				if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
					ZGuiWnd::m_pkPrevWndClicked = ZGuiWnd::m_pkWndUnderCursor;
			}

			// Är markören fortfarande innanför fönstrets gränser?
			if(ZGuiWnd::m_pkWndClicked->GetScreenRect().Inside(x, y))
			{
				SetFocus(ZGuiWnd::m_pkWndClicked);

				ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked, 
					NCODE_CLICK_UP);
				
				// Notify the main window that the window have been clicked
				int* pkParams = new int[1];
				pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID(); // control id
				m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWnd, 
					ZGM_COMMAND, 1, pkParams);
				delete[] pkParams;

				ZGuiWnd::m_pkWndClicked = NULL;
			}
			else
			{
				if(ZGuiWnd::m_pkPrevWndUnderCursor)
					ZGuiWnd::m_pkWndClicked->Notify(
						ZGuiWnd::m_pkPrevWndUnderCursor, NCODE_RELEASE);
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
				ZGuiWnd::m_pkWndUnderCursor->Notify(
					ZGuiWnd::m_pkWndUnderCursor, NCODE_OVER_CTRL);

				if(ZGuiWnd::m_pkPrevWndUnderCursor)
					ZGuiWnd::m_pkPrevWndUnderCursor->Notify(
					ZGuiWnd::m_pkPrevWndUnderCursor, NCODE_DEFAULT);
				
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

	if(iKey < 0)
		return true;

	// Kolla först om vi skall köra ett keycommand...
	map<pair<ZGuiWnd*, int>, ZGuiWnd*>::iterator itKey;
	itKey = m_KeyCommandTable.find(pair<ZGuiWnd*, int>
		(ZGuiWnd::m_pkFocusWnd, iKey));
	if(itKey != m_KeyCommandTable.end())
	{		
		// Skicka ett Command medelande till valt fönster.
		int* pkParams = new int[1];
		int id = itKey->second->GetID(); // control id
		pkParams[0] = id;
		m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWnd, ZGM_COMMAND, 
			1, pkParams);
		delete[] pkParams;
	}

	if(IgnoreKey(iKey))
		return true;

	if(iKey >= 0)
	{
		if(m_pkInput->Pressed(KEY_RSHIFT) || m_pkInput->Pressed(KEY_LSHIFT))
		{

			if(iKey == '7')
				iKey = '/';
			else
			if(iKey == '0')
				iKey = '=';
			else
			if(iKey == ',') 
				iKey = ';';
			else
			if(iKey == '.')
				iKey = ':';
			else
			if(iKey == '-')
				iKey = '_';
			else
			if(iKey > 48 && iKey < 58)
				iKey -= 16;
			else
			if(iKey > 96 && iKey < 123)
				iKey -= 32;
		}

		if(iKey == KEY_F10)
		{
			m_pkZeroFps->ToggleGui();
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
	// Kolla först om detta fönster kan ha keyboard fokus.
	if( !pkWnd->GetWindowFlag( WF_CANHAVEFOCUS ) || pkWnd == NULL)
		return;
	
	// Hitta det fönster som tidigare hade fokus och 
	// ta bort fokuset från denna.
	if(ZGuiWnd::m_pkFocusWnd)
	{		
		if(ZGuiWnd::m_pkFocusWnd)
			ZGuiWnd::m_pkFocusWnd->KillFocus();
	}

	ZGuiWnd::m_pkFocusWnd = pkWnd;
	ZGuiWnd::m_pkFocusWnd->SetFocus();
	
	if(typeid(*pkWnd)==typeid(ZGuiWnd))
	{
		for( list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
			 it != m_pkMainWindows.end(); it++ )
			 if((*it)->pkWnd == pkWnd)
			 {
				 int iPreviusActiveMainWndID = -1;
				 bool bActiveMainWndHaveChanged = false;

				 if( m_pkActiveMainWin == NULL || 
					 m_pkActiveMainWin->pkWnd != pkWnd)
					 bActiveMainWndHaveChanged = true;

				 if(m_pkActiveMainWin)
					 iPreviusActiveMainWndID = m_pkActiveMainWin->iID;

				 m_pkActiveMainWin = (*it);

				 if(bActiveMainWndHaveChanged)
				 {						 
					 m_iHighestZWndValue++; printf("%i\n", m_iHighestZWndValue);
					 m_pkActiveMainWin->iZValue = m_iHighestZWndValue;
					 m_pkMainWindows.sort(SortZCmp);

					 // Notify the a new window have focus
					int* pkParams = new int[1];
					pkParams[0] = iPreviusActiveMainWndID;
					m_pkActiveMainWin->pkCallback(pkWnd,ZGM_SETFOCUS,1,pkParams);
					delete[] pkParams;
				 }

				 break;
			 }
	}
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
					(*it)->pkWnd->Show();
					SetFocus((*it)->pkWnd);
				}
				else
				{
					(*it)->pkWnd->Hide();

					m_pkMainWindows.sort(SortZCmp);

					MAIN_WINDOW* best = NULL;
					int heighest = -1;
					for( list<MAIN_WINDOW*>::iterator itMain2 = 
						m_pkMainWindows.begin();
						 itMain2 != m_pkMainWindows.end(); itMain2++ )
							if((*itMain2)->iZValue > heighest && 
								(*itMain2)->pkWnd->IsVisible())
							{
								best = (*itMain2);
								heighest = best->iID;
							}

					if(best)
						SetFocus(best->pkWnd);
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

ZGuiFont* ZGui::AddBitmapFont(char* strBitmapName, char cCharsOneRow, 
							  char cCellSize, char cPixelGapBetweenChars, 
							  int iID)
{
	ZGuiFont* pkNewFont = new ZGuiFont(cCharsOneRow,cCellSize,
		cPixelGapBetweenChars,iID);
	if(pkNewFont->CreateFromFile(strBitmapName))
	{
		m_pkFonts.insert( map<int, ZGuiFont*>::value_type(pkNewFont->m_iID, 
			pkNewFont) ); 
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

void ZGui::AddKeyCommand(int Key, ZGuiWnd *pkFocusWnd, ZGuiWnd *pkTriggerWnd)
{
	m_KeyCommandTable.insert(map< pair<ZGuiWnd*, int>, ZGuiWnd* >
		::value_type(pair<ZGuiWnd*, int>(pkFocusWnd,Key), pkTriggerWnd) ); 
}

void ZGui::ShowCursor(bool bShow)
{
	m_pkCursor->Show(bShow);
}

void ZGui::SetDefaultFont(ZGuiFont* pkFont)
{
	map<int, ZGuiFont*>::iterator itFont;
	itFont = m_pkFonts.find(ZG_DEFAULT_GUI_FONT);
	if(itFont != m_pkFonts.end())
	{
		delete (itFont->second);
		m_pkFonts.erase(itFont);
		m_pkFonts.insert( map<int, ZGuiFont*>::value_type(
			pkFont->m_iID, pkFont)); 
	}
}
