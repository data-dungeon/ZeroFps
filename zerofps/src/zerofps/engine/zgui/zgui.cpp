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
	pkDefaultFont->CreateFromFile("../data/textures/text/font.bmp");
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

void ZGui::ShowMainWindow(/*int iID*/ZGuiWnd* pkMainWnd, bool bShow)
{
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		it != m_pkMainWindows.end(); it++)
		if((*it)->pkWnd == pkMainWnd)//iID == iID)
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
		//case KEY_F10: can´t ignore this iKeyPressed,it´s used to close the menu system.
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

	//int piss = pkTexMan->Load("file:../data/textures/piss.bmp", 0); // första misslyckas, vet inte varför...
	
	
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
		// En knapp har tryckts ner.
		int kParams[1] = {iKey};
		m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
			ZGM_KEYDOWN,1,(int*) kParams);
			
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
	}

}
