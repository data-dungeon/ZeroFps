// Gui.cpp: implementation of the Gui class.
//
//////////////////////////////////////////////////////////////////////
  
#include "zgui.h"
#include "zguiwindow.h"
#include "../render/zguirenderer.h"
#include "../render/texturemanager.h"
#include "zguiresourcemanager.h"
#include <typeinfo>
#include <queue>
#include "../basic/globals.h"
#include "../basic/keys.h"
#include "./zgui.h"
#include "../engine/i_camera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int ZGui::m_iResX = 800; //1024;
int ZGui::m_iResY = 600; //768;

ZGui::ZGui(int iResX, int iResY) : ZFSubSystem("Gui") 
{
	m_bClearScreen = false;

	m_iResX = iResX;
	m_iResY = iResY;

	m_pkCapturedWindow = NULL;
	m_bHoverWindow = false;
	m_iHighestZWndValue = 10;
	m_pkActiveMenu = NULL;
	m_bClickedMenu = false;

	m_bActive = false;

	m_bHandledMouse = false;

	m_acLineColor[0] = 255;
	m_acLineColor[1] = 0;
	m_acLineColor[2] = 0;

	m_bDisableAlphatest = false;

	m_pkFpsLabel = NULL;
	m_pkFpsWnd = NULL;

	m_iShowFPSCounter = 1;
	m_bRenderEnabled = true;

	m_iScaleMode = 0;
	
	RegisterVariable("g_showfpscounter",&m_iShowFPSCounter,CSYS_INT);
   RegisterVariable("g_drawgui",			&m_bRenderEnabled, CSYS_BOOL);
	RegisterVariable("g_scalemode",		&m_iScaleMode, CSYS_INT);
}

bool ZGui::StartUp()	
{ 	
	m_pkRenderer = static_cast<ZGuiRender*>(
		g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_pkResManager = static_cast<ZGuiResourceManager*>(
		g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(
		g_ZFObjSys.GetObjectPtr("TextureManager"));
	//m_pkZeroFps = static_cast<I_ZeroFps*>(
	//	g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(
		g_ZFObjSys.GetObjectPtr("ZShaderSystem"));		
		
	m_pkActiveMainWin = NULL;
	m_bLeftButtonDown = false;
	m_bRightButtonDown = false;
	m_pnCursorRangeDiffX=m_pnCursorRangeDiffY=0;
	m_pkCursor = new ZGuiCursor();
	m_pkCursorSkin = new ZGuiSkin();
	m_pkCursorSkin->m_bTileBkSkin = false;
	m_pkCursorSkin->m_unBorderSize = 0;
	m_pkCursorSkin->m_afBkColor[0] = 1;
	m_pkCursorSkin->m_afBkColor[1] = 1;
	m_pkCursorSkin->m_afBkColor[2] = 1;
	m_pkCursor->SetPos(0,0);
	m_pkCursor->SetSize(32,32);
	m_pkCursor->SetSkin(m_pkCursorSkin);

	m_pkFocusBorderSkin = new ZGuiSkin(0,0,0, 128,128,128, 2);

	m_pkToolTip = new ZGuiToolTip();
	m_pkToolTip->SetSkin(new ZGuiSkin(255,255,198, 128,128,128, 1));

	m_bUseHardwareMouse = true;
	m_bForceGUICapture = false;

	return true; 
}

bool ZGui::ShutDown() 
{ 
	map<int, PICK_MAP>::iterator it;
	for(it=m_kPickMap.begin(); it!=m_kPickMap.end(); it++)
	{
		delete[] it->second.m_pbAlphaState;
	}

	return true; 
}

bool ZGui::IsValid()	{ return true; }

ZGui::~ZGui()
{

}

bool ZGui::RegisterWindow(ZGuiWnd* pkNewWindow, char* szName)
{
	if(pkNewWindow == NULL)
		return false;

	strcpy(pkNewWindow->m_szName, szName);
	m_pkResManager->Add(string(szName), pkNewWindow);

	pkNewWindow->SetGUI(this);
	if(pkNewWindow->GetFont() == NULL)
		pkNewWindow->SetFont(m_pkResManager->Font("defguifont"));
	
	m_pkWindows.insert(map<int,ZGuiWnd*>::value_type(pkNewWindow->GetID(),
		pkNewWindow)); 

	return true;
}

bool ZGui::UnregisterWindow(ZGuiWnd* pkWindow)
{
	if(pkWindow == NULL)
		return false;

	// Ta bort fï¿½stret i listan hos managern
	m_pkResManager->RemoveWnd(string(pkWindow->m_szName));

	// Ta fï¿½st bort eventuellt main window
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
						if((*itMain2)->pkWnd->m_iZValue > heighest && 
							(*itMain2)->pkWnd->IsVisible())
						{
							best = (*itMain2);
							heighest = best->pkWnd->m_iZValue;
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

		// Ta bort alla barn fï¿½ster som tillhï¿½ detta fï¿½ster.
		// Men bara om det ï¿½ ett (main) fï¿½ster. I annat fall
		// skï¿½er kontrollerna det sjï¿½va.
		//if(pkWindow->GetParent() == NULL)
		ZGuiWnd* pkWndParent = pkWindow->GetParent();
		if( pkWndParent == NULL || typeid(*pkWndParent)==typeid(ZGuiTabCtrl) ) 
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
	//	cout << "Failed to remove window!" << endl;
	}

	return true;
}

// Lï¿½g till ett huvudfï¿½ster
bool ZGui::AddMainWindow(int iMainWindowID,ZGuiWnd* pkWindow, char* szName,
						 callback cb,bool bSetAsActive)
{
	if(GetMainWindow(iMainWindowID))
	{
		char szError[50];
		sprintf(szError, "Tried to add a main window with id (%i) and name (%s) that already " \
			"exist!\n", iMainWindowID, szName);
		printf(szError);
		ZFAssert(0, szError );
	}

	// Ett main window skall inte ha nï¿½on parent!
	pkWindow->SetParent(NULL);
	
	MAIN_WINDOW* pkNewMainWindow	= new MAIN_WINDOW;
	pkNewMainWindow->iID			= iMainWindowID;
	pkNewMainWindow->pkCallback		= cb;
	pkNewMainWindow->pkWnd			= pkWindow;
	pkNewMainWindow->iZValue		= 0;

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
	if(iID == 0) // statiskt fï¿½ster
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



// Rendera det aktiva fï¿½stret (och alla dess childs)
bool ZGui::Render(int fps)
{
	if(!m_bRenderEnabled)
		return true;

// 	StartProfileTimer("r__Gui");		
		
	m_pkRenderer->StartRender(m_bClearScreen);
	
	// Blit windows with lowest z order first.
	for(list<MAIN_WINDOW*>::reverse_iterator it = m_pkMainWindows.rbegin();
	 it != m_pkMainWindows.rend(); it++)
	 {
		 ZGuiWnd* pkWnd = (*it)->pkWnd;

		 if(pkWnd->m_bUseClipper)
		 {
			 m_pkRenderer->EnableClipper(true); 
			 m_pkRenderer->SetClipperArea(pkWnd->m_kClipperArea);
		 }

		(*it)->pkWnd->Render(m_pkRenderer);

		if(pkWnd->m_bUseClipper)
			 m_pkRenderer->EnableClipper(false);		
	 }

	//// Draw points
	//m_pkRenderer->RenderPoints(m_kPointsToDraw); 

	//// Draw rects
	//m_pkRenderer->RenderRects(m_kRectsToDraw); 

	//// Draw lines
	m_pkRenderer->RenderLines(m_kLinesToDraw,
		m_acLineColor[0],m_acLineColor[1],m_acLineColor[2],1.0f);

	 if(m_pkToolTip)
	 {
		 ZGuiWnd* pkWnd = m_pkToolTip->GetWnd();

		 if(pkWnd && pkWnd->IsVisible() ) 
			 pkWnd->Render(m_pkRenderer);
	 }

	 // Draw active menu on top
	 if(m_pkActiveMenu)
		 m_pkActiveMenu->Render(m_pkRenderer); 

	// Draw cursor
	if(!m_bUseHardwareMouse && m_pkCursor->IsVisible())
		m_pkCursor->Render();

	ZGuiWnd* m_pkFpsWnd = m_pkResManager->Wnd("zguiapp_fps_wnd");
	ZGuiWnd* m_pkFpsLabel = m_pkResManager->Wnd("zguiapp_fps_label");

	// Render fps label (also fix for strange bug with rendering cursor last 
	// (can't do that, this fuck up the shadowblob)).
	if(m_iShowFPSCounter)
	{
		if(m_pkFpsWnd != NULL)
		{
			char szFps[20];
			sprintf(szFps, "Fps: %i", fps);
			m_pkFpsLabel->SetText(szFps); 
			m_pkFpsWnd->Render( m_pkRenderer );
		}
	}

	m_pkRenderer->EndRender(); 

// 	StopProfileTimer("r__Gui");	
	
	m_kPointsToDraw.clear();
	m_kRectsToDraw.clear(); 
	m_kLinesToDraw.clear(); 

	return true;
}

void ZGui::ShowFPSCounter(bool bShow)
{
	m_iShowFPSCounter = bShow;

	if(m_pkFpsWnd == NULL)
		return;

	if(m_iShowFPSCounter)
	{
		m_pkFpsWnd->SetPos(800-60,5);
		//m_pkFpsWnd->Show();
	}
	else
	{
		m_pkFpsWnd->SetPos(800,5);
		//m_pkFpsWnd->Hide();
	}
}

/*ZGui::MAIN_WINDOW* ZGui::FindMainWnd(int x,int y)
{
	MAIN_WINDOW* best = m_pkActiveMainWin;

	// Find the window
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		it != m_pkMainWindows.end(); it++)
		 {
			 ZGuiWnd* pkWnd = (*it)->pkWnd;

			if(pkWnd->IsVisible() && pkWnd->GetScreenRect().Inside(x,y))
			{
				bool bInsideClipperArea = true;

				if((*it)->pkWnd->m_bUseClipper)
				{
					bInsideClipperArea = (*it)->pkWnd->m_kClipperArea.Inside(x,y);
				}

				if(best->pkWnd->GetScreenRect().Inside(x,y) && bInsideClipperArea)
				{
					if((*it)->iZValue > best->iZValue)
					{
						best = (*it);
					}
				}
				else
				{
					best = (*it);
				}
			}
		 }	

	return best;
}*/

ZGui::MAIN_WINDOW* ZGui::FindMainWnd(int x,int y)
{
	list<MAIN_WINDOW*> candidates;

	// Find the window
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		it != m_pkMainWindows.end(); it++)
		 {
			 ZGuiWnd* pkWnd = (*it)->pkWnd;

			 if(pkWnd->GetScreenRect().Inside(x,y) == false)
				 continue;

			 if(pkWnd->IsVisible() == false)
				 continue;

			 if(AlphaPixelAtPos(x,y,pkWnd))
				 continue;

			 candidates.push_back( (*it) );			 
		 }	

	if(candidates.empty())
		return NULL;

	candidates.sort(SortZCmp);



	return candidates.front();
}

void ZGui::SetFocus(ZGuiWnd* pkWnd, bool bSetCapture)
{	
   if(pkWnd == NULL)
      return;

	if(pkWnd->m_bEnabled == false)
		return;

	// Hitta det fï¿½ster som tidigare hade fokus och 
	// ta bort fokuset frï¿½ denna.
	if(ZGuiWnd::m_pkFocusWnd)
	{		
		if(ZGuiWnd::m_pkFocusWnd)
			ZGuiWnd::m_pkFocusWnd->KillFocus();
	}

	// Kolla fï¿½st om detta fï¿½ster kan ha keyboard fokus.
	if(!pkWnd->GetWindowFlag(WF_CANHAVEFOCUS) || pkWnd == NULL)
	{
		// Fï¿½sï¿½ sï¿½ta fokus pï¿½dess root parent istï¿½let.
		ZGuiWnd* pkRootParent = pkWnd->GetParent();
		if(pkRootParent)
			SetFocus(pkRootParent, bSetCapture);
		return;
	}

	ZGuiWnd::m_pkFocusWnd = pkWnd;

	if(pkWnd)
		pkWnd->SetFocus(bSetCapture);
	
	if(pkWnd->GetWindowFlag(WF_TOPWINDOW))
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
// Kommenterade ut 2004-sep-19
//					m_iHighestZWndValue++;
//					m_pkActiveMainWin->pkWnd->m_iZValue = m_iHighestZWndValue;
//					m_pkActiveMainWin->iZValue = m_iHighestZWndValue;
/////////////////////////

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

ZGuiSkin** ZGui::GetCursorSkin()
{
	return &m_pkCursorSkin;
}

void ZGui::SetCursor(int x, int y, int TextureID,int MaskTextureID,int Width,int Height)
{
	m_pkCursorSkin->m_iBkTexID = TextureID;
	m_pkCursorSkin->m_iBkTexAlphaID = MaskTextureID;
	m_pkCursor->SetSize(Width,Height);
	m_pkCursor->SetSkin(m_pkCursorSkin);
	m_pkCursor->SetPos(x,y); 
}

bool ZGui::IsActive()
{
	return m_bActive;
}

bool ZGui::UpdateMouse(int x, int y, bool bLBnPressed, bool bRBnPressed, bool bMBnPressed, float fTime)
{
	m_fTime = fTime; // register time

	if(m_bActive == true && m_pkCursor->IsVisible())
	{
		bool bMenuOpen = m_pkActiveMenu && m_pkActiveMenu->IsOpen();

		//if(m_pkCursor && m_pkCursor->IsVisible())	
			OnMouseUpdate(x, y, bLBnPressed, bRBnPressed, bMBnPressed, fTime);

		if(bLBnPressed == false && bRBnPressed == false)
			m_bHandledMouse = false;

		if(m_bForceGUICapture)
			m_bHandledMouse = true;

		if(bMenuOpen && (m_pkActiveMenu && m_pkActiveMenu->IsOpen() == false))
		{
			m_pkActiveMenu = NULL;
			m_bHandledMouse = true;
		}
		
		m_pkToolTip->Update(x,y,(bLBnPressed|bRBnPressed|bMBnPressed),fTime);
	}

	return true;
}

void ZGui::UpdateKeys(vector<KEY_INFO>& kKeysPressed, float time)
{
	m_iKeyPressed = -1; // reset keypress

   const float REPEAT_DELAY = 0.5f;
   const float REPEAT_RATE = 0.02f;

   bool bIsTextbox = false;

   if(ZGuiWnd::m_pkFocusWnd)
   {
      if( typeid(*ZGuiWnd::m_pkFocusWnd) == typeid(ZGuiTextbox) )
         bIsTextbox = true;

		//printf("ZGuiWnd::m_pkFocusWnd = %s\n", ZGuiWnd::m_pkFocusWnd->GetName());
   }

   if(!kKeysPressed.empty())
   {
      KEY_INFO press_key = kKeysPressed.back();

		m_iKeyPressed = press_key.key; // register last key press

      if(press_key.pressed)
      {
			int kParams[1] = {press_key.key};
			m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
				ZGM_KEYPRESS,1,(int*) kParams);
         
         if(RunKeyCommand(press_key.key))
				return;
      }
   }

   if(/*bIsTextbox*/ZGuiWnd::m_pkFocusWnd)
   {
      static KEY_INFO last_key = {-1,0,0};
      static float last_key_press_time = 0;
      static float repeat_time = 0;

      for(int i=0; i<kKeysPressed.size(); i++)
      {
         if(kKeysPressed[i].pressed)
         {
            last_key = kKeysPressed[i];
            last_key_press_time = time;

            int fkey = last_key.key;
            FormatKey(fkey, last_key.shift);
            if(fkey != 0)
				{
					if(fkey == KEY_V && last_key.ctrl)
					{
						if(bIsTextbox)
						{
							string text;
							if(GetClipboardText(text))
								ZGuiWnd::m_pkFocusWnd->SetText((char*)text.c_str());
						}
					}
					else
						ZGuiWnd::m_pkFocusWnd->ProcessKBInput(fkey);
				}
         }
         else
         {
            if(last_key.key == kKeysPressed[i].key)
               last_key.key = -1;
         }
      }

      if(last_key.key > 0 && (time - last_key_press_time) > REPEAT_DELAY)
      {
         if(time - repeat_time > REPEAT_RATE)
         {
            int fkey = last_key.key;
            FormatKey(fkey, last_key.shift);

            if(fkey != 0)			
               ZGuiWnd::m_pkFocusWnd->ProcessKBInput(fkey); 

            repeat_time = time;
         }
      }
   }


}

bool ZGui::Activate(bool bActive)
{
	m_bActive = bActive;
	return m_bActive;
}

void ZGui::SetActiveMenu(ZGuiMenu* pkMenu)
{
	m_pkActiveMenu = pkMenu;
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
						if((*itMain2)->pkWnd->m_iZValue > heighest && 
							(*itMain2)->pkWnd->IsVisible())
						{
							best = (*itMain2);
							heighest = best->pkWnd->m_iZValue;
						}
					 }

				if(best)
					SetFocus(best->pkWnd);
			}

			break;
		}
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

bool ZGui::RunKeyCommand(int iKey)
{
	// Kolla fï¿½st om vi skall kï¿½a ett keycommand...
	map<pair<ZGuiWnd*,int>,ZGuiWnd*>::iterator itKey;
	itKey = m_KeyCommandTable.find(pair<ZGuiWnd*,int>
		(ZGuiWnd::m_pkFocusWnd,iKey));

	if(itKey != m_KeyCommandTable.end() && m_pkActiveMainWin)
	{		
		// Skicka ett Command medelande till valt fï¿½ster.
		int* pkParams = new int[2];
		int id = itKey->second->GetID(); // control id
		pkParams[0] = id;
		pkParams[1] = 0;

		ZGuiWnd* pkMain = itKey->second->GetParent();

		if(pkMain == NULL)
			pkMain = m_pkActiveMainWin->pkWnd;

		m_pkActiveMainWin->pkCallback(pkMain,ZGM_COMMAND, 2, pkParams);

		delete[] pkParams;

		SetFocus(itKey->second);

		return true;
	}

	return false;
}

bool ZGui::SetSkins(vector<tSkinInf>& kAllSkinsTempArray, ZGuiWnd* pkWnd)
{
	vector<ZGuiWnd::SKIN_DESC> kSkinList;
	pkWnd->GetWndSkinsDesc(kSkinList);

	string strWndName = pkWnd->GetName();

	unsigned int skins_this_wnd = kSkinList.size();

	// Loopa igenom alla skins som detta fï¿½ster har
	for(unsigned int i=0; i<skins_this_wnd; i++)
	{
		ZGuiSkin* pkSkin = *(&kSkinList[i])->first;
		string strDesc = kSkinList[i].second;

		// Loopa igenom alla skins som finns och vï¿½j ut ett som passar.
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

	// Finns det redan ett fï¿½ster med det namnet och detta fï¿½ster
	// ï¿½ inte det fï¿½ster som skall dï¿½as om?
	if(pkExistingWnd != NULL && pkWndToRename != pkExistingWnd) 
	{
		return false; // det finns redan ett fï¿½ster med detta namn.
	}

	// Finns det redan ett fï¿½ster med det namnet och detta fï¿½ster
	// ï¿½ det fï¿½ster som skall dï¿½as om?
	if(pkExistingWnd != NULL && pkWndToRename == pkExistingWnd) 
	{
		if(strcmp(pkExistingWnd->GetName(), pkWndToRename->GetName()) == 0) 
			return true; // namnet har inte ï¿½drats
	}

	if(pkExistingWnd == NULL)
	{
		// Radera (ej deallokera) fï¿½stret i guimanagern.
		if(m_pkResManager->RemoveWnd(pkWndToRename->GetName()) == false)
			return false;

		// Lï¿½g till fï¿½stret pï¿½nytt, fast med ett annat namn.
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

void ZGui::GetResolution(int& res_x, int& res_y)
{
   res_x = m_iResX;
   res_y = m_iResY;
}

ZGuiWnd* ZGui::GetMainWindowFromPoint(int x, int y)
{
	MAIN_WINDOW* best = NULL;

	// Find the window
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		 it != m_pkMainWindows.end(); it++)
		 {
			if( (*it)->pkWnd->IsVisible() && 
				(*it)->pkWnd->GetScreenRect().Inside(x,y))
			{
				if(best != NULL && best->pkWnd->GetScreenRect().Inside(x,y))
				{
					if((*it)->pkWnd->m_iZValue > best->pkWnd->m_iZValue)
						best = (*it);
				}
				else
				{
					best = (*it);
				}
			}
		 }

	if(best == NULL)
		return NULL;

	return best->pkWnd; 
}

void ZGui::SetCaptureToWnd(ZGuiWnd* pkWnd)
{
	m_pkCapturedWindow = pkWnd;
}

void ZGui::KillWndCapture()
{
	m_pkCapturedWindow = NULL;
}

ZGuiWnd* ZGui::GetWndCapture()
{
	return m_pkCapturedWindow;
}

void ZGui::DrawLine(Point p1, Point p2)
{
	m_kLinesToDraw.push_back(p1);
	m_kLinesToDraw.push_back(p2);
}

void ZGui::DrawPoint(Point pos, 
					 unsigned char r, unsigned char g, unsigned char b)
{
	tRGBPoint kElement;
	kElement.first = pos;
	color_rgb color = {r,g,b};
	kElement.second = color;
	m_kPointsToDraw.push_back(kElement);
}

void ZGui::DrawRect(int x, int y, int w, int h, 
					unsigned char r, unsigned char g, unsigned char b)
{
	tRGBRect kElement;
	kElement.first = Rect(x,y,x+w,y+h);
	color_rgb color = {r,g,b};
	kElement.second = color;
	m_kRectsToDraw.push_back(kElement);
}

void ZGui::SetRes(int iResX, int iResY)
{
	m_iResX = iResX;
	m_iResY = iResY;
}

bool ZGui::AlphaPixelAtPos(int mx, int my, ZGuiWnd *pkWndClicked)
{
	if(!m_bMouseLeftPressed && !m_bMouseRightPressed) // 2005 feb 26: lägger till för att unvika slöhet!
		return false;

	if(m_bDisableAlphatest)
	{
		printf("Disabled\n");
		return false;
	}

	if(pkWndClicked == NULL)
		return true;

	if(pkWndClicked->m_bUseAlhpaTest == false)
		return false;

	// Ignore some controlls...
	ZGuiWnd* pkParent = pkWndClicked->GetParent();
	if(pkParent)
	{
		if( typeid(*pkParent)==typeid(ZGuiListbox))
			return false;
		if( typeid(*pkParent)==typeid(ZGuiTreebox) )
			return false;
		if( typeid(*pkParent)==typeid(ZGuiCheckbox) ) 
			return false;
	}

	ZGuiSkin* pkSkin = pkWndClicked->GetSkin();
	if(pkSkin == NULL)
		return false;

	if(pkSkin->m_bTransparent)
	{
		list<ZGuiWnd*> vkChildList;
		pkWndClicked->GetChildrens(vkChildList);

		for(list<ZGuiWnd*>::iterator it = vkChildList.begin(); it!=vkChildList.end(); it++)
			 if((*it)->GetScreenRect().Inside(mx,my) && (*it)->IsVisible())
				 return AlphaPixelAtPos(mx,my,(*it));

		return true;
	}

	bool bIsTGA = pkSkin->m_iBkTexAlphaID == -1 ? true : false;

	// Check if in map.
	map<int, PICK_MAP>::iterator itPick;
	itPick = m_kPickMap.find(pkSkin->m_iBkTexID);
	if(itPick == m_kPickMap.end())
		return false;

	bool bTransparentPixelUnderCursor = false;

	int horz_offset, vert_offset;
	horz_offset = mx - pkWndClicked->GetScreenRect().Left;
	vert_offset = my - pkWndClicked->GetScreenRect().Top;

	float x_offset = (float) horz_offset / pkWndClicked->GetScreenRect().Width();
	float y_offset = (float) vert_offset / pkWndClicked->GetScreenRect().Height();

	float tex_w = (float) itPick->second.w; 
	float tex_h = (float) itPick->second.h; 

	float dx = (int)(tex_w*x_offset);
	float dy = (int)(tex_h*y_offset);

	dy = tex_h - dy;

	if(dx < 0) dx = 0;
	if(dy < 0) dy = 0;

	if( dy < itPick->second.h && dx < itPick->second.w)
	{
		int offset = int(dy) * itPick->second.w + int(dx);
		bTransparentPixelUnderCursor =  itPick->second.m_pbAlphaState[ offset ];
	}

	//if(bTransparentPixelUnderCursor)
	//	printf("Alpha pixel under cursor!\n");	
	//else
	//	printf("No Alpha pixel under cursor!\n");

	return bTransparentPixelUnderCursor;
}

void ZGui::SetWndForeground(ZGuiWnd *pkWnd)
{
/*	m_iHighestZWndValue++;
	m_pkActiveMainWin->iZValue = m_iHighestZWndValue;
	m_pkMainWindows.sort(SortZCmp);	*/
}

ZGuiToolTip* ZGui::GetToolTip()
{
	return m_pkToolTip; 
}

//void ZGui::OnScreenSizeChange(int iPrevWidth, int iPrevHeight, int iNewWidth, int iNewHeight)
//{
//	map<string,ZGuiWnd*> kWindows;
//	m_pkResManager->GetWindows(kWindows); 
//
//	map<string,ZGuiWnd*>::iterator it = kWindows.begin();
//	for( ; it != kWindows.end(); it++)
//	{
//		(*it).second->Rescale(iPrevWidth, iPrevHeight, iNewWidth, iNewHeight);
//	}
//}

void ZGui::TranslateMousePos(int &x, int &y)
{
	float dif_x=0, dif_y=0;

	if(m_iResX == 1024 && m_iResY == 768)
	{
		dif_x = 225.0f * (float)((1.0f/m_iResX) * x); 
		dif_y = 172.0f * (float)((1.0f/m_iResY) * y); 
	}
	else
	if(m_iResX == 1280 && m_iResY == 960)
	{
		dif_x = 480.0f * (float)((1.0f/m_iResX) * x); 
		dif_y = 362.0f * (float)((1.0f/m_iResY) * y); 
	}
	else
	if(m_iResX == 1280 && m_iResY == 1024)
	{
		dif_x = 482.0f * (float)((1.0f/m_iResX) * x); 
		dif_y = 430.0f * (float)((1.0f/m_iResY) * y); 
	}
	else
	if(m_iResX == 1600 && m_iResY == 1200)
	{
		dif_x = 800.0f * (float)((1.0f/m_iResX) * x); 
		dif_y = 600.0f * (float)((1.0f/m_iResY) * y); 
	}

	x = x-(int)dif_x;
	y = y-(int)dif_y;
}

void ZGui::SetLineColor(int r, int g, int b)
{
	m_acLineColor[0] = r;
	m_acLineColor[1] = g;
	m_acLineColor[2] = b;
}

void ZGui::FormatKey(int& iKey, bool bShiftIsPressed)
{
	#ifdef WIN32

		if(bShiftIsPressed)
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

       /*  if(iKey == 39) // ï¿½            iKey = 'A';
         if(iKey == 91) // ï¿½            iKey = 'A';
         if(iKey == 59) // ï¿½            iKey = 'O';*/
		}
		else
		{
			if(iKey == '-')
				iKey = '+';
			if(iKey == '/')
				iKey = '-';
			if(iKey == '\\')
				iKey = '\'';
//			if(iKey == '=')
//				iKey = '';
/*         if(iKey == 39) // ï¿½            iKey = 'a';
         if(iKey == 91) // ï¿½            iKey = 'a';
         if(iKey == 59) // ï¿½            iKey = 'o';*/
		}	

	#endif // #ifdef WIN32

	#ifndef WIN32

		if(bShiftIsPressed)
		{
			if(iKey == '+')
				iKey = '?';	
			else
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

void ZGui::CloseActiveMenu(void)
{
	//if(m_pkActiveMenu && m_bClickedMenu == false)
	//{
	//	m_pkActiveMenu->HideAll(); 
	//	m_pkActiveMenu = NULL;
	//	m_bClickedMenu = true;
	//}
}

// Brï¿½ funktionen som man krï¿½s av att titta i... Grï¿½funktionen nummer ett.

bool ZGui::OnMouseUpdate(int x, int y, bool bLBnPressed, 
                         bool bRBnPressed, bool bMBnPressed,
                         float fGameTime)
{
	m_bClickedMenu = false;

	if(bMBnPressed) // ignorera mitten knappen och ge spelet fokus
	{
		m_bHandledMouse = false;
		return true;
	}

	// Register public variables needed by the editbox.
	m_iMouseX = x; m_iMouseY = y;
	m_bMouseLeftPressed = bLBnPressed;
	m_bMouseRightPressed = bRBnPressed;

	if(m_bUseHardwareMouse == false)
		m_pkCursor->SetPos(x,y);

	//////////////////////////////////////////////////////////////////////////////
	// Pgr av att musmarkï¿½en som anvï¿½ds i DM inte har sin klickpunkt i ï¿½re
	// vï¿½stra hï¿½net utan 2 pixlar lï¿½gre ner, kompenserar vi fï¿½ det hï¿½.
	//////////////////////////////////////////////////////////////////////////////
	x+=2; y+=2;
	m_iMouseX = x; m_iMouseY = y;
	//////////////////////////////////////////////////////////////////////////////

	bool bLeftButtonDown = bLBnPressed;
	bool bRightButtonDown = bRBnPressed;

	if(m_pkActiveMainWin == NULL)
	{
		m_bHandledMouse = false;
		return false;
	}

	if(m_pkCapturedWindow == NULL)
	{
		bool bClicked = false;

		if( bLeftButtonDown && m_bLeftButtonDown==false)
			bClicked = true;

		if( bRightButtonDown && m_bRightButtonDown==false)
			bClicked = true;

		// Skall vi byta main window?
		MAIN_WINDOW* wnd; // = FindMainWnd(x,y);
		if( bClicked )
		{ 
			if(m_pkActiveMenu && !m_pkActiveMenu->HooverItem(x,y))
			{
				m_pkActiveMenu->HideAll();
			}

			if((wnd = FindMainWnd(x,y)) != NULL) //if(wnd = FindMainWnd(x,y) ) -- ï¿½dring 040701
			{
				if(wnd != m_pkActiveMainWin)
				{
					SetFocus(wnd->pkWnd);

					PlaceWndFrontBack(wnd->pkWnd, true); // lade till 22 okt 2004

					ZGuiWnd* test = wnd->pkWnd->Find(x,y);

					m_bHandledMouse = true;

					if(test)
					{
						if(test->GetSkin() && test->GetSkin()->m_bTransparent == true)
						{
							m_bHandledMouse = false;
						}
					}



					return true;					
				}
			}
			else
			{
				//printf("Failed to find main wnd\n");
			}
		}
	}
	else
	{


		bool bClicked = false;

		if( bLeftButtonDown && m_bLeftButtonDown==false)
			bClicked = true;

		if( bRightButtonDown && m_bRightButtonDown==false)
			bClicked = true;

		if(bClicked )
		{
			//MAIN_WINDOW* wnd = FindMainWnd(x,y);
			//printf("wnd find = %s\n", wnd->pkWnd->GetName());
		}

		//m_pkActiveMainWin->pkWnd = m_pkCapturedWindow;
      if(bLeftButtonDown && m_bLeftButtonDown==false) // annars kï¿½s den alltid
      {
         SetFocus(m_pkCapturedWindow);
      }


	}

	if(m_pkActiveMainWin == NULL)
	{
		m_bHandledMouse = false;
		return false;
	}

	if(!m_pkActiveMainWin->pkWnd) 
	{
		m_bHandledMouse = false;
		return false; 
	}
	
	ZGuiWnd* pkFocusWindow;
	
	// Kolla vilken kontroll som har klickats, men inte om
	// en kontroll ï¿½ captured och ej ï¿½ ett fï¿½ster.
	if(m_pkCapturedWindow == NULL || (m_pkCapturedWindow != NULL && 
		typeid(*m_pkCapturedWindow) == typeid(ZGuiWnd)))
	{
		pkFocusWindow = m_pkActiveMainWin->pkWnd->Find(x,y);
	}
	else 
		pkFocusWindow = m_pkCapturedWindow;
	
	// Registrer if mouse pointer moves over a window.
	if(pkFocusWindow == NULL)
		m_bHoverWindow = false;
	else
		m_bHoverWindow = true;
	
	ZGuiWnd::m_pkWndUnderCursor = pkFocusWindow;


	bool bLeftPressed =  (m_bLeftButtonDown  == false && bLeftButtonDown  == true);
	bool bRightPressed = (m_bRightButtonDown == false && bRightButtonDown == true);
	
	// Har vï¿½ster musknapp klickats (men inte slï¿½pt)
	if( bLeftPressed || bRightPressed )
	{		
		if(pkFocusWindow)
		{			
			if(pkFocusWindow->m_bUseAlhpaTest)
			{
				if(AlphaPixelAtPos(x,y,pkFocusWindow))
				{
					m_bHandledMouse = false;

					// Stï¿½g eventuell meny
					CloseActiveMenu();

					return true;
				}
			}

			if(pkFocusWindow->m_bUseClipper && !pkFocusWindow->m_kClipperArea.Inside(x,y))
			{
				m_bHandledMouse = false;
				return true;
			}

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
				
				if(bLeftPressed || (pkFocusWindow->m_bAcceptRightClicks && bRightPressed) )
				{
					ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked,
						NCODE_CLICK_DOWN);

					if(ZGuiWnd::m_pkWndClicked)
					{
						ZGuiWnd* pkClickDownParent = ZGuiWnd::m_pkWndClicked->GetParent();

						if( pkClickDownParent)
						{
							if(typeid(*pkClickDownParent)==typeid(ZGuiMenu))
							{
								m_pkActiveMenu = (ZGuiMenu*) pkClickDownParent;
								m_bClickedMenu = true;
							}
						}
					}
				}

				ZGuiWnd* test; 
//				if( (test=m_pkActiveMainWin->pkWnd->Find(x,y))) //!ClickedWndAlphaTex(x,y, ZGuiWnd::m_pkWndClicked))
				if( (test=m_pkActiveMainWin->pkWnd->Find(x,y)) != NULL) // 040701
				{
					if(AlphaPixelAtPos(x,y,test)==false)
					{
						if(test->GetSkin() && test->GetSkin()->m_bTransparent == false)
						{
							m_bHandledMouse = true;
						}
						else
						{
							if(typeid(*test)==typeid(ZGuiLabel))
								m_bHandledMouse = true;
						}
					}
				}

				// Send a Left Button Down Message...
				int* pkParams = new int[2];
				pkParams[0] = x; pkParams[1] = y;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
					bLeftPressed ? ZGM_LBUTTONDOWN : ZGM_RBUTTONDOWN, 2,pkParams);
				delete[] pkParams;
				
			}
			else
			{
				m_pnCursorRangeDiffX = x-rc.Left; 
				m_pnCursorRangeDiffY = y-rc.Top;

				ZGuiWnd* pkParent = pkFocusWindow->GetParent();

				//if(pkParent && typeid(*pkParent)==typeid(ZGuiScrollbar))
				{
					m_bHandledMouse = true;
				}
			}
		}
		else
		{
			
			if(ZGuiWnd::m_pkFocusWnd)
				ZGuiWnd::m_pkFocusWnd->KillFocus();

			ZGuiWnd::m_pkWndClicked = NULL;
			ZGuiWnd::m_pkFocusWnd = NULL;
		}

		CloseActiveMenu();
	}

	

	// ï¿½ vï¿½ster musknapp nertryckt?
	if( bLeftButtonDown == true && ZGuiWnd::m_pkWndClicked)
	{
		// Skall fï¿½stret flyttas?
		if(!(ZGuiWnd::m_pkWndClicked->GetMoveArea() == ZGuiWnd::m_pkWndClicked->GetScreenRect()))
		{
			ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked,NCODE_MOVE);
			ZGuiWnd::m_pkWndClicked->SetPos(x-m_pnCursorRangeDiffX,y-
				m_pnCursorRangeDiffY,true,false);

			// Notify the main window that the window is moving
			if(m_bLeftButtonDown == true)
			{
				m_bHandledMouse = true;

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

	bool bLeftReleased =  (m_bLeftButtonDown  == true && bLeftButtonDown  == false);
	bool bRightReleased = (m_bRightButtonDown == true && bRightButtonDown == false);

	bool bSentCommandMessage = false;

	// Har vï¿½ster musknapp slï¿½ts (men inte klickats)?
	if(bLeftReleased || bRightReleased)
	{
		m_bHandledMouse = false;

		// Byt tillbaks till button_up skin om musknappen har slï¿½pts (fast inte pï¿½knappen).
		if(ZGuiWnd::m_pkWndClicked)
			if( typeid(*ZGuiWnd::m_pkWndClicked)==typeid(ZGuiButton) )
				((ZGuiButton*) ZGuiWnd::m_pkWndClicked)->SetSkin(
					((ZGuiButton*) ZGuiWnd::m_pkWndClicked)->GetButtonUpSkin()); 

		if(pkFocusWindow && ZGuiWnd::m_pkWndClicked != NULL)
		{
			// Informera fï¿½stret innan att det har tappat fokus.
			if(ZGuiWnd::m_pkWndUnderCursor && (bLeftReleased || 
				(pkFocusWindow->m_bAcceptRightClicks && bRightReleased) ))
			{
				if(ZGuiWnd::m_pkPrevWndClicked && 
					ZGuiWnd::m_pkPrevWndClicked != ZGuiWnd::m_pkWndUnderCursor)
					if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
						ZGuiWnd::m_pkPrevWndClicked->Notify(
							ZGuiWnd::m_pkWndClicked,NCODE_RELEASE);

				if(!ZGuiWnd::m_pkWndClicked->IsInternalControl())
					ZGuiWnd::m_pkPrevWndClicked = ZGuiWnd::m_pkWndUnderCursor;
			}

			// ï¿½ markï¿½en fortfarande innanfï¿½ fï¿½strets grï¿½ser?
			if(ZGuiWnd::m_pkWndClicked->GetScreenRect().Inside(x,y))
			{
				//SetFocus(ZGuiWnd::m_pkWndClicked);

				if(bLeftReleased || (pkFocusWindow->m_bAcceptRightClicks && bRightReleased))
					ZGuiWnd::m_pkWndClicked->Notify(ZGuiWnd::m_pkWndClicked,
						NCODE_CLICK_UP);

				int* pkParams = new int[2];

				// Notify the main window that the window have been clicked
				if(bLeftReleased || (pkFocusWindow->m_bAcceptRightClicks && bRightReleased))
				{
					ZGuiWnd* pkParent = ZGuiWnd::m_pkWndClicked->GetParent();

					if( pkParent)
					{
						if( typeid(*ZGuiWnd::m_pkWndClicked) == typeid(ZGuiButton) )
						{
						//printf("ZGuiWnd::m_pkWndClicked = %s\n", ZGuiWnd::m_pkWndClicked->GetName());

						//const type_info& parent_type = typeid(*pkParent);

						//if	 ( parent_type!=typeid(ZGuiCombobox) && 
						//	   parent_type!=typeid(ZGuiListbox) &&
						//	   parent_type!=typeid(ZGuiTreeboxNode) && 
						//		parent_type!=typeid(ZGuiMenu) &&
						//		typeid(*pkFocusWindow)!=typeid(ZGuiCheckbox) &&
						//		typeid(ZGuiWnd::m_pkWndClicked)!=typeid(ZGuiCheckbox)) // tillfï¿½lig ful lï¿½ning fï¿½ att listboxitems inte skall generera COMMAND messages..
						//{
							ZGuiWnd* pkMainWnd = pkParent; //m_pkActiveMainWin->pkWnd; // Lade till 9 nov 2004 fï¿½ att controllers pï¿½en tabctrl inte fï¿½ msg annars.

							// Menyalternativ har sitt egen fï¿½ï¿½derfï¿½ster.
							if(pkParent && typeid(*pkParent)==typeid(ZGuiMenu))
								pkMainWnd = pkParent;

							// Har man klickat pï¿½en checkbox via en label sï¿½ï¿½ klickfï¿½stret checkboxen.
							if(pkParent && typeid(*pkParent)==typeid(ZGuiCheckbox))
								ZGuiWnd::m_pkWndClicked = pkParent;

							pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID(); // control id
							pkParams[1] = (pkFocusWindow->m_bAcceptRightClicks && bRightReleased); // hï¿½er musknapp har triggat knapp kommandot
							m_pkActiveMainWin->pkCallback(pkMainWnd, ZGM_COMMAND,2,pkParams);

							bSentCommandMessage = true;

							m_bHandledMouse = true;
						}
					}
					delete[] pkParams;
				}

				// Send a Left Button Up Message...
				pkParams = new int[2];
				pkParams[0] = x; pkParams[1] = y;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
					bLeftReleased ? ZGM_LBUTTONUP : ZGM_RBUTTONUP,2,pkParams);
				delete[] pkParams;

				static bool s_bClickedOnes = false;
				static float s_fClickTime = fGameTime;

				// Test for double click.
				if(s_bClickedOnes == true && fGameTime-s_fClickTime<0.25f)
				{
					pkParams = new int[2];
					pkParams[0] = x; pkParams[1] = y;
					m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
						bLeftReleased ? ZGM_LBUTTONDBLCLK : ZGM_RBUTTONDBLCLK,2,pkParams);
					s_bClickedOnes = false;
					delete[] pkParams;
				}

				if(s_bClickedOnes == false)
					s_bClickedOnes = true;

				s_fClickTime = fGameTime; 

				ZGuiWnd::m_pkWndClicked = NULL;
			}
			else
			{
				if(ZGuiWnd::m_pkPrevWndUnderCursor && (bLeftReleased || 
					(pkFocusWindow->m_bAcceptRightClicks && bRightReleased)))
					ZGuiWnd::m_pkWndClicked->Notify(
						ZGuiWnd::m_pkPrevWndUnderCursor,NCODE_RELEASE);

				// Send a Left Button Up Message...
				int* pkParams = new int[1];
				pkParams = new int[2];
				pkParams[0] = x; pkParams[1] = y;
				m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkWndClicked,
					bLeftReleased ? ZGM_LBUTTONUP : ZGM_RBUTTONUP,2,pkParams);
				delete[] pkParams;
			}
		}
	}

	// ï¿½ vï¿½ster musknapp inte nertryckt?
	if(m_bLeftButtonDown == false && bLeftButtonDown == false)
	{
		// ï¿½ det samma fï¿½stret under musmarkï¿½en som innan?
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

	// Send a Mouse Move Message...
	//static int s_iPrevX=-1;
	//static int s_iPrevY=-1;
	if(/*(s_iPrevX != x || s_iPrevY != y) && */bSentCommandMessage == false && ZGuiWnd::m_pkFocusWnd != NULL)
	{
		int* pkParams = new int[3];
		pkParams[0] = (int) bLBnPressed; pkParams[1] = x; pkParams[2] = y;
		m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
			ZGM_MOUSEMOVE,3,pkParams);
		delete[] pkParams;
		//s_iPrevX = x;
		//s_iPrevY = y;
	}

	m_bLeftButtonDown = bLeftButtonDown;
	m_bRightButtonDown = bRightButtonDown;



	return true;
}

//enum eMainWndZ { Front = 9999998, Back = -9999998 };
//enum eMainWndZ { Front = 9999999, Back = -9999999 };
bool ZGui::PlaceWndFrontBack(ZGuiWnd* pkWnd, bool bFront) 
{
	if(pkWnd == NULL)
		return false;

	MAIN_WINDOW* pkMainWnd = NULL;

	int iZLow = 9999998+1;
	int iZHigh = -9999998-1;

	// Find the window
	for(list<MAIN_WINDOW*>::iterator it = m_pkMainWindows.begin();
		it != m_pkMainWindows.end(); it++)
		{
			MAIN_WINDOW* pkCurr = (*it);

			if(pkWnd == pkCurr->pkWnd)
			{
				pkMainWnd = pkCurr;
			}

			if(pkCurr->pkWnd->m_iZValue > iZHigh)
				iZHigh = pkCurr->pkWnd->m_iZValue;
			if(pkCurr->pkWnd->m_iZValue < iZLow)
				iZLow = pkCurr->pkWnd->m_iZValue;
		}

	if(pkMainWnd == NULL)
		return false;

	if(bFront)
	{
      if(m_iHighestZWndValue >= iZHigh)
      {
         iZHigh = m_iHighestZWndValue + 1;
         m_iHighestZWndValue = iZHigh;
      }

		pkMainWnd->iZValue = iZHigh+1;
		pkMainWnd->pkWnd->m_iZValue = iZHigh+1;
	}
	else
	{
		pkMainWnd->iZValue = iZLow-1;
		pkMainWnd->pkWnd->m_iZValue = iZLow-1;
	}

	m_pkMainWindows.sort(SortZCmp);

	return true;
}

bool ZGui::CreatePickMapForImage(int iTexID, string strImageFile)
{
	// Find file exten and skip all files that are not TGA.
	char *ext = strrchr( strImageFile.c_str(), '.');
	if(ext == NULL)		
		return false;
	if(strcmp(ext,".tga") != 0) 
		return false;

	// Check if already in map.
	map<int, PICK_MAP>::iterator itPick;
	itPick = m_kPickMap.find(iTexID);
	if(itPick != m_kPickMap.end())
		return true;

	ZFVFile kFile;
	if(!kFile.Open(strImageFile.c_str(),0,false))
	{
		printf("1: Failed to open image [%s] file for pickmap\n", strImageFile.c_str());
		return false;
	}

	Image kImage;
	if(!kImage.load(kFile.m_pkFilePointer, strImageFile.c_str()))
	{
		printf("2: Failed to load image [%s] for pickmap\n", strImageFile.c_str());
		return false;
	}

	PICK_MAP new_key;
	new_key.w = kImage.m_iWidth;
	new_key.h = kImage.m_iHeight;
	new_key.m_pbAlphaState = new bool[kImage.m_iWidth * kImage.m_iHeight];
	new_key.strFile = strImageFile;
	
	for(int y=0; y<new_key.h; y++)
		for(int x=0; x<new_key.w; x++)
		{
			color_rgba c;
			kImage.get_pixel(x,y,c);
			new_key.m_pbAlphaState[ y * new_key.w + x ] = (c.a == 0) ? true : false;
		}

	m_kPickMap[iTexID] = new_key;

	return true;
}