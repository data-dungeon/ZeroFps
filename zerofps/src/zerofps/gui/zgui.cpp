// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/rect.h"
#include "zgui.h"
#include "zguiwindow.h"
#include "../render/zguirenderer.h"
#include "../engine/input.h"
#include "../basic/zfassert.h"
#include "../engine/zerofps.h"
#include <typeinfo>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ZGui::ZGui()
{
	m_bHoverWindow = false;
	m_iHighestZWndValue = 10;

	m_bActive = false;
	m_pkInput = static_cast<Input*>(g_ZFObjSys.GetObjectPtr("Input"));
	m_pkRenderer = static_cast<ZGuiRender*>(
		g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkResManager = static_cast<ZGuiResourceManager*>(
		g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkActiveMainWin = NULL;
	m_bLeftButtonDown = false;
	m_bRightButtonDown = false;
	m_pnCursorRangeDiffX=m_pnCursorRangeDiffY=0;
	m_pkCursor = new ZGuiCursor();
	m_pkCursorSkin = new ZGuiSkin();
	m_pkCursor->SetPos(0,0);
	m_pkCursor->SetSize(16,16);
	m_pkCursor->SetSkin(m_pkCursorSkin);

	ZGuiFont* pkDefaultFont = new ZGuiFont(16,16,0,ZG_DEFAULT_GUI_FONT);
	pkDefaultFont->CreateFromFile("../data/textures/text/ms_sans_serif8.bmp");
	m_pkFonts.insert(map<int,ZGuiFont*>::value_type(pkDefaultFont->m_iID,
		pkDefaultFont)); 

	m_pkFocusBorderSkin = new ZGuiSkin(0,0,0, 128,128,128, 2);
}


ZGui::~ZGui()
{

}

bool ZGui::RegisterWindow(ZGuiWnd* pkNewWindow, char* szName)
{
	if(pkNewWindow == NULL)
		return false;

	strcpy(pkNewWindow->m_szName, szName);
	m_pkResManager->Add(string(szName), pkNewWindow);

/*	if(GetWindow(pkNewWindow->GetID()))
	{
		ZFAssert(0,"Tried to register a window with a ID that already exist.");
	}*/

	pkNewWindow->SetGUI(this);
	if(pkNewWindow->GetFont() == NULL)
	{
		// Add stanadard font
		map<int,ZGuiFont*>::iterator itFont;
		itFont = m_pkFonts.find(ZG_DEFAULT_GUI_FONT);
		if(itFont != m_pkFonts.end())
			pkNewWindow->SetFont(itFont->second);
	}
	m_pkWindows.insert(map<int,ZGuiWnd*>::value_type(pkNewWindow->GetID(),
		pkNewWindow)); 

	return true;
}

bool ZGui::UnregisterWindow(ZGuiWnd* pkWindow)
{
	if(pkWindow == NULL)
		return false;

	// Ta bort fönstret i listan hos managern
	m_pkResManager->RemoveWnd(string(pkWindow->m_szName));

	// Ta först bort eventuellt main window
	for(list<MAIN_WINDOW*>::iterator itMain = m_pkMainWindows.begin();
		 itMain != m_pkMainWindows.end(); itMain++)
		 {
			if((*itMain)->pkWnd == pkWindow)
			{
				if(m_pkActiveMainWin == (*itMain))
					m_pkActiveMainWin = NULL;

				delete (*itMain);
				m_pkMainWindows.erase(itMain);	
				
				MAIN_WINDOW* best = NULL;
				int heighest = -1;
				for(list<MAIN_WINDOW*>::iterator itMain2 = 
					m_pkMainWindows.begin();
					 itMain2 != m_pkMainWindows.end(); itMain2++)
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

	map<int,ZGuiWnd*>::iterator itWnd;
	itWnd = m_pkWindows.find(pkWindow->GetID());

	if(itWnd != m_pkWindows.end())
	{
		list<ZGuiWnd*> vkChildList;
		pkWindow->GetChildrens(vkChildList);

		// Ta bort alla barn fönster som tillhör detta fönster.
		// Men bara om det är ett (main) fönster. I annat fall
		// sköter kontrollerna det själva.
		if(pkWindow->GetParent() == NULL)
		{
			for(WINit w = vkChildList.begin(); w != vkChildList.end(); w++)
			{
				ZGuiWnd* pkChild = (*w);
				if(pkChild)
				{
					ZGuiWnd::ResetStaticClickWnds(pkChild);
					m_pkResManager->RemoveWnd(string(pkChild->m_szName));
					delete pkChild;
					pkChild = NULL;
				}
			}
			vkChildList.clear();
		}

		if(pkWindow)
		{
			ZGuiWnd::ResetStaticClickWnds(pkWindow);
			delete pkWindow;
			pkWindow = NULL;
		}
		m_pkWindows.erase(itWnd);
	}
	else
	{
		cout << "Failed to remove window!" << endl;
	}

	return true;
}

// Lägg till ett huvudfönster
bool ZGui::AddMainWindow(int iMainWindowID,ZGuiWnd* pkWindow, char* szName,
						 callback cb,bool bSetAsActive)
{
	if(GetMainWindow(iMainWindowID))
	{
		string strError = "Tried to add a main window with a id that already exist!";
		ZFAssert(0, strError.c_str() );
	}

	// Ett main window skall inte ha någon parent!
	pkWindow->SetParent(NULL);
	

	MAIN_WINDOW* pkNewMainWindow = new MAIN_WINDOW;
	pkNewMainWindow->iID = iMainWindowID;
	pkNewMainWindow->pkCallback = cb;
	pkNewMainWindow->pkWnd = pkWindow;
	pkNewMainWindow->iZValue = 0;

	m_pkMainWindows.push_back(pkNewMainWindow);

	RegisterWindow(pkWindow, szName);

	if(bSetAsActive)
		SetFocus(pkNewMainWindow->pkWnd);

	return true;
}

ZGuiResourceManager* ZGui::GetResMan()
{
	return m_pkResManager;
}

///////////////////////////////////////////////////////////////////////////////
// Name:		GetSpecialWndData
// Description: To get special protected data from ZGuiWnd and to avoid 
//				creating alot of functions in ZGuiWnd (that is a virtual baseclass).
//
void* ZGui::GetSpecialWndData(ZGuiWnd* pkWnd, ZndInfo type)
{
	switch(type)
	{
	case WNDPROC:
		return (void*)((ZGuiWnd::callbackfunc) pkWnd->m_pkCallback);
		break;
	}

	return NULL;
}

ZGuiWnd* ZGui::GetWindow(unsigned int iID)
{
	if(iID == 0) // statiskt fönster
		return NULL;

	map<int,ZGuiWnd*>::iterator itWnd;
	itWnd = m_pkWindows.find(iID);

	if(itWnd == m_pkWindows.end())
	{
		return NULL;
	}
	 
	return itWnd->second;
}

ZGuiWnd* ZGui::GetMainWindow(int iID)
{
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++)
		 {
			if((*it)->iID == iID)
				return (*it)->pkWnd;
		 }

	return NULL;
}

int ZGui::GetMainWindowID(char* strWindow)
{
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++)
		 {
			if(strcmp((*it)->pkWnd->GetName(), strWindow)==0)
				return (*it)->iID;
		 }

	return -1;
}

// Rendera det aktiva fönstret (och alla dess childs)
bool ZGui::Render()
{
	m_pkRenderer->StartRender();

	m_pkRenderer->SetFont(GetBitmapFont(ZG_DEFAULT_GUI_FONT));
	
	// Blit windows	with lowest z order first.
	for(list<MAIN_WINDOW*>::reverse_iterator it = m_pkMainWindows.rbegin();
	 it != m_pkMainWindows.rend(); it++)
			(*it)->pkWnd->Render(m_pkRenderer);

	// Render a yellow frame around the window that have focus.
	if(ZGuiWnd::m_pkFocusWnd)
	{
		m_pkRenderer->SetSkin(m_pkFocusBorderSkin);
		m_pkRenderer->RenderBorder(ZGuiWnd::m_pkFocusWnd->GetScreenRect());
	}

	// Draw cursor
	if(m_pkCursor->IsVisible())
		m_pkCursor->Render();

	m_pkRenderer->EndRender(); 

	return true;
}

ZGui::MAIN_WINDOW* ZGui::FindMainWnd(int x,int y)
{
	MAIN_WINDOW* best = m_pkActiveMainWin;

	// Find the window
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++)
		 {
			if((*it)->pkWnd->IsVisible() && 
				(*it)->pkWnd->GetScreenRect().Inside(x,y))
			{
				if(best->pkWnd->GetScreenRect().Inside(x,y))
				{
					if((*it)->iZValue > best->iZValue)
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
  	int x,y;
	m_pkInput->MouseXY(x,y);
	m_pkCursor->SetPos(x,y);

	static int cx = -1, cy = -1;
	if(!(cx == x && cy == y))
	{
		//printf("Mouse cursor (%i, %i)\n", x, y);
		cx = x; cy = y;
	}


	bool bLeftButtonDown = m_pkInput->Pressed(MOUSELEFT);
	bool bRightButtonDown = m_pkInput->Pressed(MOUSERIGHT);

	if(m_pkActiveMainWin == NULL)
		return false;

	// Skall vi byta main window?
	MAIN_WINDOW* wnd;
	if((bLeftButtonDown && m_bLeftButtonDown==false) && 
		(wnd = FindMainWnd(x,y)))
	{
		if(wnd != m_pkActiveMainWin)
		{
			SetFocus(wnd->pkWnd);
			return true;
		}
	}

	if(!m_pkActiveMainWin->pkWnd) 
		return false;

	ZGuiWnd* pkFocusWindow = m_pkActiveMainWin->pkWnd->Find(x,y);
	
	// Registrer if mouse pointer moves over a window.
	if(pkFocusWindow == NULL)
		m_bHoverWindow = false;
	else
		m_bHoverWindow = true;
	
	ZGuiWnd::m_pkWndUnderCursor = pkFocusWindow;

	// Send a Mouse Move Message...
	static int s_iPrevX=-1;
	static int s_iPrevY=-1;
	if(s_iPrevX != x || s_iPrevY != y && ZGuiWnd::m_pkFocusWnd)
	{
		int* pkParams = new int[3];
		pkParams[0] = (int) bLeftButtonDown; pkParams[1] = x; pkParams[2] = y;
		m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
			ZGM_MOUSEMOVE,3,pkParams);
		delete[] pkParams;
		s_iPrevX = x;
		s_iPrevY = y;
	}

	// Har vänster musknapp klickats (men inte släppt)
	if(m_bLeftButtonDown == false && bLeftButtonDown == true)
	{		
		if(pkFocusWindow)
		{
			ZGuiWnd::m_pkWndClicked = pkFocusWindow;
			SetFocus(ZGuiWnd::m_pkWndClicked);

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

				// Send a Left Button Down Message...
				int* pkParams = new int[2];
				pkParams[0] = x; pkParams[1] = y;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
					ZGM_LBUTTONDOWN,2,pkParams);
				delete[] pkParams;
			}
			else
			{
				m_pnCursorRangeDiffX = x-rc.Left; 
				m_pnCursorRangeDiffY = y-rc.Top; 
			}
		}
		else
		{
			if(ZGuiWnd::m_pkFocusWnd)
				ZGuiWnd::m_pkFocusWnd->KillFocus();

			ZGuiWnd::m_pkWndClicked = NULL;
			ZGuiWnd::m_pkFocusWnd = NULL;
		}
	}

	// Är vänster musknapp nertryckt?
	if(bLeftButtonDown == true && ZGuiWnd::m_pkWndClicked != NULL)
	{	
		// Skall fönstret flyttas?
		if(!(ZGuiWnd::m_pkWndClicked->GetMoveArea() == 
			ZGuiWnd::m_pkWndClicked->GetScreenRect()))
		{
			ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked,NCODE_MOVE);
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
					ZGM_MOVING,5,pkParams);
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
				if(ZGuiWnd::m_pkPrevWndClicked && 
					ZGuiWnd::m_pkPrevWndClicked != ZGuiWnd::m_pkWndUnderCursor)
					if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
						ZGuiWnd::m_pkPrevWndClicked->Notify(
							ZGuiWnd::m_pkWndClicked,NCODE_RELEASE);

				if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
					ZGuiWnd::m_pkPrevWndClicked = ZGuiWnd::m_pkWndUnderCursor;
			}

			// Är markören fortfarande innanför fönstrets gränser?
			if(ZGuiWnd::m_pkWndClicked->GetScreenRect().Inside(x,y))
			{
				//SetFocus(ZGuiWnd::m_pkWndClicked);

				ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked,
					NCODE_CLICK_UP);
				
				// Notify the main window that the window have been clicked
				int* pkParams = new int[1];
				pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID(); // control id
				m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWnd,
					ZGM_COMMAND,1,pkParams);
				delete[] pkParams;

				// Send a Left Button Up Message...
				pkParams = new int[2];
				pkParams[0] = x; pkParams[1] = y;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
					ZGM_LBUTTONUP,2,pkParams);
				delete[] pkParams;

				static bool s_bClickedOnes = false;
				static float s_fClickTime = m_pkFps->GetGameTime();

				// Test for double click.
				if(s_bClickedOnes == true && m_pkFps->GetGameTime()-
					s_fClickTime<0.25f)
				{
					pkParams = new int[2];
					pkParams[0] = x; pkParams[1] = y;
					m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
						ZGM_LBUTTONDBLCLK,2,pkParams);
					s_bClickedOnes = false;
					delete[] pkParams;
				}

				if(s_bClickedOnes == false)
					s_bClickedOnes = true;

				s_fClickTime = m_pkFps->GetGameTime();

				ZGuiWnd::m_pkWndClicked = NULL;
			}
			else
			{
				if(ZGuiWnd::m_pkPrevWndUnderCursor)
					ZGuiWnd::m_pkWndClicked->Notify(
						ZGuiWnd::m_pkPrevWndUnderCursor,NCODE_RELEASE);
			}
		}
	}

	// Är vänster musknapp inte nertryckt?
	if(m_bLeftButtonDown == false && bLeftButtonDown == false)
	{
		// Är det samma fönstret under musmarkören som innan?
		if(ZGuiWnd::m_pkWndUnderCursor != NULL)
		{
			if(ZGuiWnd::m_pkWndUnderCursor != ZGuiWnd::m_pkPrevWndUnderCursor)
			{
				ZGuiWnd::m_pkWndUnderCursor->Notify(
					ZGuiWnd::m_pkWndUnderCursor,NCODE_OVER_CTRL);

				if(ZGuiWnd::m_pkPrevWndUnderCursor)
					ZGuiWnd::m_pkPrevWndUnderCursor->Notify(
					ZGuiWnd::m_pkPrevWndUnderCursor,NCODE_DEFAULT);
				
				ZGuiWnd::m_pkPrevWndUnderCursor = ZGuiWnd::m_pkWndUnderCursor;
			}
		}
	}

	m_bLeftButtonDown = bLeftButtonDown;
	m_bRightButtonDown = bRightButtonDown;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnKeyUpdate
// Description: Skicka vidare knapp-händelser till kontroller sammt
//				enabla keyrepeat (och sabba för folk som kan skriva fort...)
//
bool ZGui::OnKeyUpdate()
{
	static bool s_bKeyrepeatActivated = false;
	static int s_iLastKeyPressed;
	int iKeyPressed = m_pkInput->GetQueuedKey();

	// Registrera senast knappnedtryck.
	if(iKeyPressed != -1)
	{
		FormatKey(iKeyPressed);

		if(s_iLastKeyPressed != iKeyPressed)
			s_bKeyrepeatActivated = false;

		s_iLastKeyPressed = iKeyPressed;
		OnKeyPress(s_iLastKeyPressed);
	}

	static float s_fKeyrepeatCheckTime = m_pkFps->GetGameTime();
	static float s_fLastRepeatTime = m_pkFps->GetGameTime();
	
	// Kolla om den sist nedtryckta knappen fortfarande är nedtryckt.
	if(m_pkInput->Pressed(s_iLastKeyPressed))
	{
		float fCurrTime = m_pkFps->GetGameTime();

		const float REPEAT_DELAY = 0.50f, REPEAT_RATE = 0.05f;

		if(s_bKeyrepeatActivated == false)
		{
			// Är det dags att aktivera Key Repeat?
			if(fCurrTime - s_fKeyrepeatCheckTime > REPEAT_DELAY)
			{
				s_bKeyrepeatActivated = true;
				s_fKeyrepeatCheckTime = fCurrTime;
				s_fLastRepeatTime = fCurrTime;
			}
		}
		else
		{
			// Är det dags att skriva ett nytt tecken?
			if(fCurrTime - s_fLastRepeatTime > REPEAT_RATE)
			{
				OnKeyPress(s_iLastKeyPressed);
				s_fLastRepeatTime = fCurrTime;
			}
		}
	}
	else
	{
		s_bKeyrepeatActivated = false;
		s_fKeyrepeatCheckTime = m_pkFps->GetGameTime();
		s_fLastRepeatTime = m_pkFps->GetGameTime();
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

	// Kolla först om detta fönster kan ha keyboard fokus.
	if(!pkWnd->GetWindowFlag(WF_CANHAVEFOCUS) || pkWnd == NULL)
	{
		// Försök sätta fokus på dess root parent istället.
		ZGuiWnd* pkRootParent = pkWnd->GetParent(true);
		if(pkRootParent)
			SetFocus(pkRootParent);
		return;
	}

	ZGuiWnd::m_pkFocusWnd = pkWnd;
	ZGuiWnd::m_pkFocusWnd->SetFocus();
	
	if(typeid(*pkWnd)==typeid(ZGuiWnd))
	{
		for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
			 it != m_pkMainWindows.end(); it++)
			 if((*it)->pkWnd == pkWnd)
			 {
				 int iPreviusActiveMainWndID = -1;
				 bool bActiveMainWndHaveChanged = false;

				 if(m_pkActiveMainWin == NULL || 
					 m_pkActiveMainWin->pkWnd != pkWnd)
					 bActiveMainWndHaveChanged = true;

				 if(m_pkActiveMainWin)
					 iPreviusActiveMainWndID = m_pkActiveMainWin->iID;

				 m_pkActiveMainWin = (*it);

				 if(bActiveMainWndHaveChanged)
				 {						 
					m_iHighestZWndValue++;
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

void ZGui::SetCursor(int TextureID,int MaskTextureID,int Width,int Height)
{
	m_pkCursorSkin->m_iBkTexID = TextureID;
	m_pkCursorSkin->m_iBkTexAlphaID = MaskTextureID;
	m_pkCursor->SetSkin(m_pkCursorSkin);
	m_pkCursor->SetSize(Width,Height);

	int x,y;
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
	// Måste skapa alla defult skins här av någon konstig
	// anledning (skulle egentligen ha skapats i konstruktorn)
	// Glöm alltså inte bort att anropa den här funktionen
	// från applikationen!!!
	static bool s_bDefaultSkinsCreated = false;
	if(s_bDefaultSkinsCreated == false)
	{
		CreateDefaultSkins();
		s_bDefaultSkinsCreated = true;
	}

	m_bActive = bActive;
	return m_bActive;
}

void ZGui::ShowMainWindow(ZGuiWnd* pkMainWnd, bool bShow)
{
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		it != m_pkMainWindows.end(); it++)
		if((*it)->pkWnd == pkMainWnd)
		{
			if(bShow == true)
			{
				(*it)->pkWnd->Show();
				SetFocus((*it)->pkWnd);
			}
			else
			{
				(*it)->pkWnd->Hide();

				MAIN_WINDOW* best = NULL;
				int heighest = -1;
				for(list<MAIN_WINDOW*>::iterator itMain2 = 
					m_pkMainWindows.begin();
					 itMain2 != m_pkMainWindows.end(); itMain2++)
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
			}

			break;
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
		case KEY_TAB:
		case KEY_INSERT:
			return true;

		default:
			return false;
	};

}

ZGuiFont* ZGui::AddBitmapFont(char* strBitmapName,char cCharsOneRow,
							  char cCellSize,char cPixelGapBetweenChars,
							  int iID)
{
	ZGuiFont* pkNewFont = new ZGuiFont(cCharsOneRow,cCellSize,
		cPixelGapBetweenChars,iID);
	if(pkNewFont->CreateFromFile(strBitmapName))
	{
		m_pkFonts.insert(map<int,ZGuiFont*>::value_type(pkNewFont->m_iID,
			pkNewFont)); 
		return pkNewFont;
	}

	return NULL;
}

ZGuiFont* ZGui::GetBitmapFont(int iID)
{
	map<int,ZGuiFont*>::iterator itFont;
	itFont = m_pkFonts.find(iID);
	if(itFont != m_pkFonts.end())
		return itFont->second;
	
	return NULL;
}

void ZGui::AddKeyCommand(int Key,ZGuiWnd *pkFocusWnd,ZGuiWnd *pkTriggerWnd)
{
	m_KeyCommandTable.insert(map< pair<ZGuiWnd*,int>,ZGuiWnd* >
		::value_type(pair<ZGuiWnd*,int>(pkFocusWnd,Key),pkTriggerWnd)); 
}

void ZGui::ShowCursor(bool bShow)
{
	m_pkCursor->Show(bShow);
}

void ZGui::SetDefaultFont(ZGuiFont* pkFont)
{
	map<int,ZGuiFont*>::iterator itFont;
	itFont = m_pkFonts.find(ZG_DEFAULT_GUI_FONT);
	if(itFont != m_pkFonts.end())
	{
		delete (itFont->second);
		m_pkFonts.erase(itFont);
		m_pkFonts.insert(map<int,ZGuiFont*>::value_type(
			pkFont->m_iID,pkFont)); 
	}
}

ZGuiWnd* ZGui::FindNextTabWnd(ZGuiWnd *pkCurrentWnd, bool bNext)
{
	// Get Main Window...
	ZGuiWnd* pkRootWnd = pkCurrentWnd->GetParent(true);
	if(pkRootWnd == NULL) // det var redan ett main window
		pkRootWnd = pkCurrentWnd;

	ZGuiWnd* pkNext = NULL;
	int iThisTabNr = pkCurrentWnd->GetTabOrderNr();
	int iCmpNr = bNext ? 100000 : -10000;
	list<ZGuiWnd*> vkChilds;
	pkRootWnd->GetChildrens(vkChilds);

	for( WINit it = vkChilds.begin(); 
		 it != vkChilds.end(); it++ )
	{
		if((*it)->GetWindowFlag(WF_CANHAVEFOCUS))
		{
			if(bNext)
			{
				if((*it)->GetTabOrderNr() > iThisTabNr)
				{
					if((*it)->GetTabOrderNr() < iCmpNr)
					{
						pkNext = (*it);
						iCmpNr = (*it)->GetTabOrderNr();
					}
				}
			}
			else
			{
				if((*it)->GetTabOrderNr() < iThisTabNr)
				{
					if((*it)->GetTabOrderNr() > iCmpNr)
					{
						pkNext = (*it);
						iCmpNr = (*it)->GetTabOrderNr();
					}
				}
			}
		}
	}

	if(pkNext == NULL)
		return pkCurrentWnd;

	return pkNext;
}

void ZGui::CreateDefaultSkins()
{
	TextureManager* pkTexMan = static_cast<TextureManager*>
		(g_ZFObjSys.GetObjectPtr("TextureManager"));
	
	int bn_down = pkTexMan->Load("file:../data/textures/button_down.bmp", 0);
	int bn_focus = pkTexMan->Load("file:../data/textures/button_focus.bmp", 0);
	int rbn_up = pkTexMan->Load("file:../data/textures/radiobn_up.bmp", 0);
	int rbn_down = pkTexMan->Load("file:../data/textures/radiobn_down.bmp", 0);
	int rbn_a = pkTexMan->Load("file:../data/textures/radiobn_a.bmp",0);
	int cbn_up = pkTexMan->Load("file:../data/textures/checkbox_off.bmp",0);
	int cbn_down = pkTexMan->Load("file:../data/textures/checkbox_on.bmp",0);
	int bn_up = pkTexMan->Load("file:../data/textures/button_up.bmp", 0);

	m_pkResManager->Add(string("DEF_WND_SKIN"),(ZGuiSkin*)new ZGuiSkin(128,128,128, 192,192,192, 1));
	m_pkResManager->Add(string("DEF_TITLEBAR_SKIN"),(ZGuiSkin*)new ZGuiSkin(0,0,128, 192,192,192, 1));
	m_pkResManager->Add(string("DEF_BN_UP_SKIN"),new ZGuiSkin(bn_up,false));
	m_pkResManager->Add(string("DEF_BN_DOWN_SKIN"),(ZGuiSkin*)new ZGuiSkin(bn_down,false));
	m_pkResManager->Add(string("DEF_BN_FOCUS_SKIN"),(ZGuiSkin*)new ZGuiSkin(bn_focus,false));
	m_pkResManager->Add(string("DEF_TEXTBOX_SKIN"),(ZGuiSkin*)new ZGuiSkin(255,255,255, 0,0,0, 1));
	m_pkResManager->Add(string("DEF_LABEL_SKIN"),(ZGuiSkin*)new ZGuiSkin(0,0,0, 0,0,0, 1,true));
	m_pkResManager->Add(string("DEF_LISTBOX_SKIN"), (ZGuiSkin*)new ZGuiSkin(0,0,255,0,0,0,1));
	m_pkResManager->Add(string("DEF_SCROLLBAR_SKIN"),(ZGuiSkin*)new ZGuiSkin(255,255,255, 0,0,0, 1));
	m_pkResManager->Add(string("DEF_RBN_UP_SKIN"),(ZGuiSkin*)new ZGuiSkin(rbn_up,rbn_a,false));
	m_pkResManager->Add(string("DEF_RBN_DOWN_SKIN"),(ZGuiSkin*)new ZGuiSkin(rbn_down,rbn_a,false));
	m_pkResManager->Add(string("DEF_CBN_UP_SKIN"),(ZGuiSkin*)new ZGuiSkin(cbn_up,false));
	m_pkResManager->Add(string("DEF_CBN_DOWN_SKIN"),(ZGuiSkin*)new ZGuiSkin(cbn_down,false));
}

void ZGui::FormatKey(int &iKey)
{
	#ifdef WIN32
	
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
			if(iKey == '/')
				iKey = '_';
			else
			if(iKey == '=')
				iKey = '`';
			else
			if(iKey == '-')
				iKey = '?';
			else
			if(iKey == '\\')
				iKey = '*';
			else
			if(iKey > 48 && iKey < 58)
				iKey -= 16;
			else
			if(iKey > 96 && iKey < 123)
				iKey -= 32;
		}
		else
		{
			if(iKey == '-')
				iKey = '+';
			if(iKey == '/')
				iKey = '-';
			if(iKey == '\\')
				iKey = '\'';
			if(iKey == '=')
				iKey = '´';
		}	

	#endif // #ifdef WIN32

	#ifndef WIN32

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
			if(iKey == '/')
				iKey = '_';
			else
			if(iKey == '=')
				iKey = '`';
			else
			if(iKey == '-')
				iKey = '_';
			else
			if(iKey == '\'')
				iKey = '*';
			else
			if(iKey > 48 && iKey < 58)
				iKey -= 16;
			else
			if(iKey > 96 && iKey < 123)
				iKey -= 32;
		}
		else
		{
		
		
		}	

	#endif // #ifndef LINUX
}

bool ZGui::RunKeyCommand(int iKey)
{
	// Kolla först om vi skall köra ett keycommand...
	map<pair<ZGuiWnd*,int>,ZGuiWnd*>::iterator itKey;
	itKey = m_KeyCommandTable.find(pair<ZGuiWnd*,int>
		(ZGuiWnd::m_pkFocusWnd,iKey));
	if(itKey != m_KeyCommandTable.end() && m_pkActiveMainWin)
	{		
		// Skicka ett Command medelande till valt fönster.
		int* pkParams = new int[1];
		int id = itKey->second->GetID(); // control id
		pkParams[0] = id;
		m_pkActiveMainWin->pkCallback(m_pkActiveMainWin->pkWnd,ZGM_COMMAND,
			1,pkParams);
		delete[] pkParams;
		return true;
	}

	return false;
}

void ZGui::OnKeyPress(int iKey)
{
	if(ZGuiWnd::m_pkFocusWnd)
	{	
		if(!RunKeyCommand(iKey))
			ZGuiWnd::m_pkFocusWnd->ProcessKBInput(iKey);

		if(iKey == KEY_DOWN || iKey == KEY_UP)
		{
			// Find the child with the next tab nr...
			ZGuiWnd* pkNext = FindNextTabWnd(ZGuiWnd::m_pkFocusWnd,
				(iKey==KEY_DOWN));

			if(pkNext)
				SetFocus( pkNext );
		}

		// Send a WM Command message when Return or space ar being hit
		if( iKey == KEY_RETURN || iKey == KEY_SPACE)
		{
			ZGuiWnd::m_pkFocusWnd->Notify(ZGuiWnd::m_pkFocusWnd,
				NCODE_CLICK_DOWN);
			ZGuiWnd::m_pkFocusWnd->Notify(ZGuiWnd::m_pkFocusWnd,
				NCODE_CLICK_UP);
			ZGuiWnd::m_pkFocusWnd->Notify(ZGuiWnd::m_pkFocusWnd,
				NCODE_RELEASE);

			int kParams[1] = { ZGuiWnd::m_pkFocusWnd->GetID() };
			m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
				ZGM_KEYDOWN,1,(int*) kParams);
		}

		// En knapp har tryckts ner.
		int kParams[1] = {iKey};
		m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
			ZGM_KEYDOWN,1,(int*) kParams);
	}

}

///////////////////////////////////////////////////////////////////////////////
// Name: LoadDialog
// Description: Load a window (szWndResName), with all childrens and skins, 
//				from a resource file (szResourceFile).
//
bool ZGui::LoadDialog(char* szResourceFile, char* szWndResName, callback cb)
{
	ZFIni *pkINI = static_cast<ZFIni*>(g_ZFObjSys.GetObjectPtr("ZFIni"));
	TextureManager* pkTexMan = static_cast<TextureManager*>
		(g_ZFObjSys.GetObjectPtr("TextureManager"));

	// Load ini file
	pkINI->Open(szResourceFile, false);

	// Get sections from ini file.
	vector<string> vkSections;
	pkINI->GetSectionNames(vkSections);

	unsigned int i=0; 

	// Ladda in alla skins till en temp vektor.
	vector<tSkinInf> kAllSkinsTempArray;

	for( i=0; i<vkSections.size(); i++)
	{
		tSkinInf kNewSkinInfo;

		char* szWindowName = pkINI->GetValue(
			(char*)vkSections[i].c_str(),"window_name");

		// We have found the last skin. 
		if(szWindowName == NULL)
			break;

		kNewSkinInfo.first.first=szWindowName;
		kNewSkinInfo.first.second=
			pkINI->GetValue((char*)vkSections[i].c_str(),"wnd_desc");
		
		ZGuiSkin* pkSkin = new ZGuiSkin;

		char* tex[8] =
		{
			pkINI->GetValue((char*)vkSections[i].c_str(), "bk_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "horz_bd_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "vert_bd_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "corner_bd_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "bk_a_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "horz_bd_a_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "vert_bd_a_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "corner_bd_a_tex"),
		};

		pkSkin->m_iBkTexID = strcmp(tex[0],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[0],0);
		pkSkin->m_iHorzBorderTexID = strcmp(tex[1],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[1],0);
		pkSkin->m_iVertBorderTexID = strcmp(tex[2],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[2],0);
		pkSkin->m_iBorderCornerTexID = strcmp(tex[3],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[3],0);
		pkSkin->m_iBkTexAlphaID = strcmp(tex[4],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[4],0);
		pkSkin->m_iHorzBorderTexAlphaID = strcmp(tex[5],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[5],0);
		pkSkin->m_iVertBorderTexAlphaID = strcmp(tex[6],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[6],0);
		pkSkin->m_iBorderCornerTexAlphaID = strcmp(tex[7],"(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[7],0);

		int bk_r  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bk_r"));
		int bk_g  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bk_g"));
		int bk_b  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bk_b"));
		int bd_r  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_r"));
		int bd_g  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_g"));
		int bd_b  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_b"));
		int bd_sz = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_size"));
		int tile  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "tile_skin"));
		int trans = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "transparent"));

		pkSkin->m_afBkColor[0] = (1.0f/255)*bk_r;
		pkSkin->m_afBkColor[1] = (1.0f/255)*bk_g;
		pkSkin->m_afBkColor[2] = (1.0f/255)*bk_b;

		pkSkin->m_afBorderColor[0] = (1.0f/255)*bd_r;
		pkSkin->m_afBorderColor[1] = (1.0f/255)*bd_g;
		pkSkin->m_afBorderColor[2] = (1.0f/255)*bd_b;

		pkSkin->m_unBorderSize = (unsigned short) bd_sz;
		pkSkin->m_bTileBkSkin = (tile == 0) ? false : true;
		pkSkin->m_bTransparent = (trans == 0) ? false : true;

		kNewSkinInfo.second = pkSkin;

		kAllSkinsTempArray.push_back( kNewSkinInfo );
	}

	// Leta reda på start section
	int iStartSection = -1;
	for( i=0; i<vkSections.size(); i++)
	{
		char *szType = pkINI->GetValue(vkSections[i].c_str(), "type");
		char *szName = pkINI->GetValue(vkSections[i].c_str(), "name");

		if(szType != NULL && szName != NULL)
		{
			if(strcmp(szType,"Window")==0 && strcmp(szName,szWndResName)==0)
			{
				iStartSection = i;
				break;
			}
		}
	}

	if(iStartSection == -1)
	{
		printf("Failed to load resorce file %s\n", szResourceFile);
		return false;
	}

	enum WndType
	{
		WINDOW,					// 1
		LABEL,					// 2
		TEXTBOX,				// 3
		BUTTON,					// 4
		CHECKBOX,				// 5
		RADIOBUTTON,			// 6
		RADIOBUTTON_NEW_GROUP,	// 7
		COMBOBOX,				// 8
		LISTBOX,				// 9
		SCROLLBAR,				// 10
		NONE
	};

	// Ladda in alla fönster till en temp vektor
	vector<ZGuiWnd*> kControllers;
	
	typedef pair<ZGuiWnd*, string> WNDNAME;
	vector<WNDNAME> kMainWindows; // window and name

	for( i=iStartSection; i<vkSections.size(); i++)
	{
		char* szType = pkINI->GetValue((char*)vkSections[i].c_str(), "type");
		
		// Find correct window type.
		WndType eWndType;
		if(strcmp(szType, "Textbox")==0)
			eWndType = TEXTBOX;
		else
		if(strcmp(szType, "Scrollbar")==0)
			eWndType = SCROLLBAR;
		else
		if(strcmp(szType, "Radiobutton")==0)
			eWndType = RADIOBUTTON;
		else
		if(strcmp(szType, "Listbox")==0)
			eWndType = LISTBOX;
		else
		if(strcmp(szType, "Label")==0)
			eWndType = LABEL;
		else
		if(strcmp(szType, "Checkbox")==0)
			eWndType = CHECKBOX;
		else
		if(strcmp(szType, "Button")==0)
			eWndType = BUTTON;
		else
		if(strcmp(szType, "Window")==0)
			eWndType = WINDOW;
		else
		if(strcmp(szType, "Combobox")==0)
			eWndType = COMBOBOX;
		else
			continue;

		// Get properties.
		int wnd_id = atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"id"));
		char* name = pkINI->GetValue((char*)vkSections[i].c_str(), "name");
		int parentwnd_id = atoi(pkINI->GetValue(
			(char*)vkSections[i].c_str(), "parent_id"));
		char* parent_name = pkINI->GetValue((char*)vkSections[i].c_str(),
			"parent_name");
		bool bVisible = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), 
			"visible")) == 0 ? false : true;
		bool bEnabled = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), 
			"enabled")) == 0 ? false : true;
		Rect rc( atoi(pkINI->GetValue((char*)vkSections[i].c_str(),
			"area_left")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "area_top")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "area_right")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "area_bottom")));
		Rect rc_m( atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"move_area_left")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "move_area_top")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "move_area_right")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "move_area_bottom")));
		int iTabOrder=atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"tab_order"));
		char* text = pkINI->GetValue((char*)vkSections[i].c_str(), "text");

		//ZGuiWnd* pkParent = NULL;
		ZGuiWnd* pkNewWnd = NULL;
		ZGuiWnd* pkParent = NULL;
		ZGuiRadiobutton* pkPrevRadiobutton = NULL;

		if(eWndType == WINDOW)
		{
			// Create main window.
			pkNewWnd = new ZGuiWnd(rc, NULL, bVisible, wnd_id);
			pkNewWnd->SetMoveArea(rc_m);

			WNDNAME kNewMainWnd;
			kNewMainWnd.first = pkNewWnd;
			kNewMainWnd.second = string(name);
			kMainWindows.push_back(kNewMainWnd);

			int main_wnd_id = atoi(pkINI->GetValue(vkSections[i].c_str(),"mainwnd_id"));
			AddMainWindow(main_wnd_id, pkNewWnd, name, cb, false);
		}
		else
		{
			pkParent = m_pkResManager->Wnd(parent_name);
			ZFAssert(pkParent, "ZGui::LoadDialog, Failed to find parent!");
	
			// Create controller.
			switch(eWndType)
			{
			case TEXTBOX:
				{
					bool bMultiLine = false;
					pkNewWnd = new ZGuiTextbox(rc,pkParent,bVisible,wnd_id,bMultiLine);
				}
				break;
			case SCROLLBAR:
				pkNewWnd = new ZGuiScrollbar(rc,pkParent,bVisible,wnd_id);
				((ZGuiScrollbar*) pkNewWnd)->SetThumbButtonSkins(
					new ZGuiSkin(), new ZGuiSkin()); 
				break;
			case RADIOBUTTON:
				{
					int radio_group = atoi(pkINI->GetValue(vkSections[i].c_str(),
						"radio_group"));
					char* radio_group_name = pkINI->GetValue(vkSections[i].c_str(),
						"radio_group_name");
					pkNewWnd = new ZGuiRadiobutton(rc,pkParent,wnd_id,radio_group,
						radio_group_name,pkPrevRadiobutton,bVisible);
					((ZGuiRadiobutton*) pkNewWnd)->SetButtonSelectedSkin(new ZGuiSkin());
					((ZGuiRadiobutton*) pkNewWnd)->SetButtonUnselectedSkin(new ZGuiSkin());
					((ZGuiRadiobutton*) pkNewWnd)->GetButton()->SetLabelSkin(new ZGuiSkin());
				}
				break;
			case LISTBOX:
				pkNewWnd = new ZGuiListbox(rc,pkParent,bVisible,wnd_id,20,
					new ZGuiSkin(),new ZGuiSkin(),new ZGuiSkin());
				((ZGuiListbox*)pkNewWnd)->SetScrollbarSkin(
					new ZGuiSkin(),new ZGuiSkin(),new ZGuiSkin());
				break;
			case LABEL:
				pkNewWnd = new ZGuiLabel(rc,pkParent,bVisible,wnd_id); 
				break;
			case CHECKBOX:
				pkNewWnd = new ZGuiCheckbox(rc,pkParent,bVisible,wnd_id); 
				((ZGuiCheckbox*) pkNewWnd)->SetButtonCheckedSkin(new ZGuiSkin());
				((ZGuiCheckbox*) pkNewWnd)->SetButtonUncheckedSkin(new ZGuiSkin());
				break;
			case BUTTON:
				pkNewWnd = new ZGuiButton(rc,pkParent,bVisible,wnd_id); 
				((ZGuiButton*) pkNewWnd)->SetButtonUpSkin(new ZGuiSkin());
				((ZGuiButton*) pkNewWnd)->SetButtonDownSkin(new ZGuiSkin());
				((ZGuiButton*) pkNewWnd)->SetButtonHighLightSkin(new ZGuiSkin());
				break;
			case COMBOBOX:
				{
					pkNewWnd = new ZGuiCombobox(rc,pkParent,bVisible,wnd_id,20,
						new ZGuiSkin(),new ZGuiSkin(),new ZGuiSkin(),new ZGuiSkin());
					((ZGuiCombobox*)pkNewWnd)->SetScrollbarSkin(
						new ZGuiSkin(),new ZGuiSkin(),new ZGuiSkin());
					
					char* szRows = pkINI->GetValue(vkSections[i].c_str(),
						"num_visible_rows");

					if(szRows)
					{
						int iNumVisRows;
						iNumVisRows = atoi(szRows);
						((ZGuiCombobox*)pkNewWnd)->SetNumVisibleRows(iNumVisRows); 
					}

					int iIsMenu;
					iIsMenu = atoi(pkINI->GetValue(vkSections[i].c_str(),
						"is_menu"));
					((ZGuiCombobox*)pkNewWnd)->IsMenu(iIsMenu == 1); 
				}
				break;
			default:
				continue;
				break;
			}

			RegisterWindow(pkNewWnd, name);
		}		

		if(eWndType != BUTTON && eWndType != RADIOBUTTON)
			pkNewWnd->SetSkin(new ZGuiSkin());

		if(strcmp(text, "(null)") != 0) 
			pkNewWnd->SetText(text);
		
		int antal_skins = kAllSkinsTempArray.size();

		SetSkins(kAllSkinsTempArray, pkNewWnd);
	}

	// Clean up texture memory in temp buffer.
	for(i=0; i<kAllSkinsTempArray.size(); i++)
		delete kAllSkinsTempArray[i].second;

	return true;
}

bool ZGui::SetSkins(vector<tSkinInf>& kAllSkinsTempArray, ZGuiWnd* pkWnd)
{
	vector<ZGuiWnd::SKIN_DESC> kSkinList;
	pkWnd->GetWndSkinsDesc(kSkinList);

	string strWndName = pkWnd->GetName();

	unsigned int skins_this_wnd = kSkinList.size();

	// Loopa igenom alla skins som detta fönster har
	for(unsigned int i=0; i<skins_this_wnd; i++)
	{
		ZGuiSkin* pkSkin = kSkinList[i].first;
		string strDesc = kSkinList[i].second;

		// Loopa igenom alla skins som finns och välj ut ett som passar.
		for(unsigned int j=0; j<kAllSkinsTempArray.size(); j++)
		{
			if( kAllSkinsTempArray[j].first.first == strWndName &&
				kAllSkinsTempArray[j].first.second == strDesc )
			{
				if(pkSkin == NULL)
					continue;

				*pkSkin = *kAllSkinsTempArray[j].second;
				break;
			}
		}
	}

	return true;
}

bool ZGui::ChangeWndRegName(ZGuiWnd* pkWndToRename, const char* pkNewName)
{	
	ZGuiWnd* pkExistingWnd = m_pkResManager->Wnd(string(pkNewName));

	// Finns det redan ett fönster med det namnet och detta fönster
	// är inte det fönster som skall döpas om?
	if(pkExistingWnd != NULL && pkWndToRename != pkExistingWnd) 
	{
		return false; // det finns redan ett fönster med detta namn.
	}

	// Finns det redan ett fönster med det namnet och detta fönster
	// är det fönster som skall döpas om?
	if(pkExistingWnd != NULL && pkWndToRename == pkExistingWnd) 
	{
		if(strcmp(pkExistingWnd->GetName(), pkWndToRename->GetName()) == 0) 
			return true; // namnet har inte ändrats
	}

	if(pkExistingWnd == NULL)
	{
		// Radera (ej deallokera) fönstret i guimanagern.
		if(m_pkResManager->RemoveWnd(pkWndToRename->GetName()) == false)
			return false;

		// Lägg till fönstret på nytt, fast med ett annat namn.
		strcpy(pkWndToRename->m_szName, pkNewName);
		if(!m_pkResManager->Add(string(pkNewName), pkWndToRename))
			return false;
	}

	return true;
}

bool ZGui::MouseHoverWnd()
{
	return m_bHoverWindow;
}

bool ZGui::Resize(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
{
	map<string, ZGuiWnd*> akWindows;
	m_pkResManager->GetWindows(akWindows);

	float fScaleX = (float) iNewWidth / (float) iOldWidth;
	float fScaleY = (float) iNewHeight / (float) iOldHeight;

//	float fOldXprocentAvSkarm, fOldYprocentAvSkarm;

	//Rect rc, rcNew;
	map<string, ZGuiWnd*>::iterator itWindows;
	for(itWindows = akWindows.begin();
		itWindows != akWindows.end(); itWindows++)
		{		
/*			// 
			// Change area
			// 
			rc = (*itWindows).second->m_kArea;
			
			fOldXprocentAvSkarm = (float) rc.Left / (float) iOldWidth;
			fOldYprocentAvSkarm = (float) rc.Top / (float) iOldHeight;

			rcNew = Rect(
				(int) (fOldXprocentAvSkarm * (float) iNewWidth ),
				(int) (fOldYprocentAvSkarm * (float) iNewHeight), 0,0);

			rcNew.Right =  rcNew.Left + (int) (fScaleX * (float) rc.Width()); 
			rcNew.Bottom =  rcNew.Top + (int) (fScaleY * (float) rc.Height());
			
			(*itWindows).second->m_kArea = rcNew;

			// 
			// Change move area
			// 
			rc = (*itWindows).second->m_kMoveArea;
			
			fOldXprocentAvSkarm = (float) rc.Left / (float) iOldWidth;
			fOldYprocentAvSkarm = (float) rc.Top / (float) iOldHeight;

			rcNew = Rect(
				(int) (fOldXprocentAvSkarm * (float) iNewWidth ),
				(int) (fOldYprocentAvSkarm * (float) iNewHeight),0,0);

			rcNew.Right =  rcNew.Left + (int) (fScaleX * (float) rc.Width()); 
			rcNew.Bottom =  rcNew.Top + (int) (fScaleY * (float) rc.Height());
			
			(*itWindows).second->m_kMoveArea = rcNew;*/

			ResizeWnd((*itWindows).second, fScaleX, fScaleY, iOldWidth, iOldHeight, 
				iNewWidth, iNewHeight);
		}

	return true;
}

bool ZGui::ResizeWnd(ZGuiWnd *pkWnd, float fScaleX, float fScaleY, int iOldWidth, 
					 int iOldHeight, int iNewWidth, int iNewHeight)
{

	Rect rc, rcNew;
	float fOldXprocentAvSkarm, fOldYprocentAvSkarm;

	// 
	// Change area
	// 
	rc = pkWnd->m_kArea;
	
	fOldXprocentAvSkarm = (float) rc.Left / (float) iOldWidth;
	fOldYprocentAvSkarm = (float) rc.Top / (float) iOldHeight;

	rcNew = Rect(
		(int) (fOldXprocentAvSkarm * (float) iNewWidth ),
		(int) (fOldYprocentAvSkarm * (float) iNewHeight), 0,0);

	rcNew.Right =  rcNew.Left + (int) (fScaleX * (float) rc.Width()); 
	rcNew.Bottom =  rcNew.Top + (int) (fScaleY * (float) rc.Height());
	
	pkWnd->m_kArea = rcNew;

	// 
	// Change move area
	// 
	rc = pkWnd->m_kMoveArea;
	
	fOldXprocentAvSkarm = (float) rc.Left / (float) iOldWidth;
	fOldYprocentAvSkarm = (float) rc.Top / (float) iOldHeight;

	rcNew = Rect(
		(int) (fOldXprocentAvSkarm * (float) iNewWidth ),
		(int) (fOldYprocentAvSkarm * (float) iNewHeight),0,0);

	rcNew.Right =  rcNew.Left + (int) (fScaleX * (float) rc.Width()); 
	rcNew.Bottom =  rcNew.Top + (int) (fScaleY * (float) rc.Height());
	
	pkWnd->m_kMoveArea = rcNew;

	// Move childrens
	for( WINit win = pkWnd->m_kChildList.begin();
		 win != pkWnd->m_kChildList.end(); win++)
		 {
			 if((*win)->IsInternalControl())
			 {
				 ResizeWnd((*win), fScaleX, fScaleY, iOldWidth, 
					 iOldHeight, iNewWidth, iNewHeight);
			 }
		 }

	return true;
}
