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
#include "../engine/i_zerofps.h"
#include "../engine/camera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int ZGui::m_iResX = 800; //1024;
int ZGui::m_iResY = 600; //768;

ZGui::ZGui(int iResX, int iResY) : ZFSubSystem("Gui") 
{
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
	//RegisterVariable("r_showfpscounter",&m_iShowFPSCounter,CSYS_INT);
}

bool ZGui::StartUp()	
{ 
	m_bRenderEnabled = true;

	RegisterVariable("r_drawgui",			&m_bRenderEnabled,				CSYS_BOOL);	
	
	m_pkRenderer = static_cast<ZGuiRender*>(
		g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_pkResManager = static_cast<ZGuiResourceManager*>(
		g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(
		g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkZeroFps = static_cast<I_ZeroFps*>(
		g_ZFObjSys.GetObjectPtr("ZeroFps"));
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

		// Ta bort alla barn fönster som tillhör detta fönster.
		// Men bara om det är ett (main) fönster. I annat fall
		// sköter kontrollerna det själva.
		//if(pkWindow->GetParent() == NULL)
		if( pkWindow->GetParent() == NULL || typeid(*pkWindow->GetParent())==typeid(ZGuiTabCtrl) ) 
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
		char szError[50];
		sprintf(szError, "Tried to add a main window with id (%i) and name (%s) that already " \
			"exist!\n", iMainWindowID, szName);
		printf(szError);
		ZFAssert(0, szError );
	}

	// Ett main window skall inte ha någon parent!
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
bool ZGui::Render(int fps)
{
	if(!m_bRenderEnabled)
		return true;


	m_pkRenderer->StartRender();
	
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

		if(pkWnd->m_pkCamera && pkWnd->IsVisible()) 
		{
			//m_pkRenderer->EndRender(); 
			m_pkZeroFps->Draw_RenderTarget(pkWnd->m_pkCamera);
			//m_pkRenderer->StartRender();
		}
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

			 if(ClickedWndAlphaTex(x,y,pkWnd) == false)
				 continue;

			 candidates.push_back( (*it) );			 
		 }	

	if(candidates.empty())
		return NULL;

	candidates.sort(SortZCmp);



	return candidates.front();
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

	if(pkWnd)
		pkWnd->SetFocus();
	
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
					m_iHighestZWndValue++;
					m_pkActiveMainWin->pkWnd->m_iZValue = m_iHighestZWndValue;
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

bool ZGui::Update(float fGameTime, int iKeyPressed, bool bLastKeyStillPressed,
				  bool bShiftIsPressed, int x, int y, bool bLBnPressed, 
				  bool bRBnPressed, bool bMBnPressed)
{
	if(m_bActive == true)
	{
		//if(m_pkCursor && m_pkCursor->IsVisible())	
			OnMouseUpdate(x, y, bLBnPressed, bRBnPressed, bMBnPressed, fGameTime);

			KeyboardInput(iKeyPressed, bShiftIsPressed, fGameTime);

			if(bLBnPressed == false && bRBnPressed == false)
				m_bHandledMouse = false;

			if(m_bForceGUICapture)
				m_bHandledMouse = true;
		
		m_pkToolTip->Update(x,y,(bLBnPressed|bRBnPressed|bMBnPressed),fGameTime);
	}

	

	m_kPointsToDraw.clear();
	m_kRectsToDraw.clear(); 
	m_kLinesToDraw.clear(); 

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
	// Kolla först om vi skall köra ett keycommand...
	map<pair<ZGuiWnd*,int>,ZGuiWnd*>::iterator itKey;
	itKey = m_KeyCommandTable.find(pair<ZGuiWnd*,int>
		(ZGuiWnd::m_pkFocusWnd,iKey));

	if(itKey != m_KeyCommandTable.end() && m_pkActiveMainWin)
	{		
		// Skicka ett Command medelande till valt fönster.
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

void ZGui::OnKeyPress(int iKey)
{
	if(ZGuiWnd::m_pkFocusWnd)
	{	
		if(iKey != -1)
		{
			bool bIsTextbox = typeid(*ZGuiWnd::m_pkFocusWnd) == 
				typeid(ZGuiTextbox) ? true : false;

			bool bMultiLine = false;
			if(bIsTextbox)
			{
				bMultiLine = ((ZGuiTextbox*) ZGuiWnd::m_pkFocusWnd)->IsMultiLine();
			}

			if((iKey==KEY_DOWN || iKey==KEY_UP) && !(bIsTextbox && bMultiLine))
			{
				// Find the child with the next tab nr...
				ZGuiWnd* pkNext = FindNextTabWnd(ZGuiWnd::m_pkFocusWnd,
					(iKey==KEY_DOWN));

				if(pkNext)
					SetFocus( pkNext );
			}

			// Send a WM Command message when Return or space ar being hit
			if( (iKey == KEY_RETURN || iKey == KEY_SPACE) 
				&& !(bIsTextbox && bMultiLine) )
			{
				ZGuiWnd::m_pkFocusWnd->Notify(ZGuiWnd::m_pkFocusWnd,
					NCODE_CLICK_DOWN);
				ZGuiWnd::m_pkFocusWnd->Notify(ZGuiWnd::m_pkFocusWnd,
					NCODE_CLICK_UP);
				ZGuiWnd::m_pkFocusWnd->Notify(ZGuiWnd::m_pkFocusWnd,
					NCODE_RELEASE);
			}
		}

		static int sLastPressedKey = -100;

		if(iKey == -1 && sLastPressedKey != -100)
		{			
			// En knapp har tryckts ner.
			int kParams[1] = {sLastPressedKey};
			m_pkActiveMainWin->pkCallback(ZGuiWnd::m_pkFocusWnd,
				ZGM_KEYPRESS,1,(int*) kParams);

			sLastPressedKey = -100;
		}
		
		if(iKey != -1)
		{
			sLastPressedKey = iKey;
		}

		RunKeyCommand(iKey);
	}
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
		ZGuiSkin* pkSkin = *(&kSkinList[i])->first;
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

void ZGui::GetResolution(int& res_x, int& res_y)
{
   res_x = m_iResX;
   res_y = m_iResY;

	//m_pkRenderer->GetScreenSize(res_x, res_y);
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

void ZGui::KeyboardInput(int key, bool shift, float time)
{
	if(key != -1 /*&& m_bHandledMouse != true*/)
	{
		FormatKey(key, shift);
	//	m_bHandledMouse = false; // Ge alltid fokusen till APP :)
	}

	const float REPEAT_DELAY_SEC = 0.5f;

	static int s_iLastKeyPressed = -1;
	static float s_fPressTime;

	bool bPrint = false;

	if(key == -1)
	{
		// tagenten har släppts
		s_iLastKeyPressed = -1;
	}
	else
	if(key == s_iLastKeyPressed)
	{
		// om det är samma tagent som innan...
		if(time-s_fPressTime > REPEAT_DELAY_SEC && shift == false)
		{
			bPrint = true;
			s_fPressTime = 0;
		}
	}
	else
	{
		if(!shift || (shift && s_iLastKeyPressed == -1))
		{
			if( key - 32 != s_iLastKeyPressed) 
			{	// hindra att ett extra tecken skrivs ut om shift varit intryckt innann.
				if( !((key == '1' && s_iLastKeyPressed == '!') || (key == '2' && s_iLastKeyPressed == '"') ||
					 (key == '3' && s_iLastKeyPressed == '#') || (key == '4' && s_iLastKeyPressed == '¤') ||
					 (key == '5' && s_iLastKeyPressed == '%') || (key == '6' && s_iLastKeyPressed == '&') ||
					 (key == '7' && s_iLastKeyPressed == '/') || (key == '8' && s_iLastKeyPressed == '(') ||
					 (key == '9' && s_iLastKeyPressed == ')') || (key == '0' && s_iLastKeyPressed == '=') ))
				{
					bPrint = true;
					s_fPressTime = time;
					s_iLastKeyPressed = key;
				}
			}
		}
	}

	static bool FULlosning = false;
	if(shift && !FULlosning)
	{
		if(ZGuiWnd::m_pkFocusWnd && s_iLastKeyPressed != key)
		{
			ZGuiWnd::m_pkFocusWnd->ProcessKBInput(key);
			FULlosning = true;
			s_iLastKeyPressed = key;
		}
	}
	else
	{
		FULlosning = false;
	}

	if(bPrint)
	{
		if(key == 8)
			key = KEY_BACKSPACE;
		if(key == 9)
			key = KEY_TAB;
		if(key == 13)
			key = KEY_RETURN;
		
		// om det är en ny tagent...
		if(ZGuiWnd::m_pkFocusWnd)
			ZGuiWnd::m_pkFocusWnd->ProcessKBInput(key);
	}

	OnKeyPress(key);
}

bool ZGui::ClickedWndAlphaTex(int mx, int my, ZGuiWnd *pkWndClicked)
 {
	if(m_bDisableAlphatest)
	{
		printf("Disabled\n");
		return true;
	}

	if(pkWndClicked == NULL)
		return false;

	if(pkWndClicked->m_bUseAlhpaTest == false)
		return true;

	// Ignore some controlls...
	ZGuiWnd* pkParent = pkWndClicked->GetParent();
	if(pkParent)
	{
		if( typeid(*pkParent)==typeid(ZGuiListbox))
			return true;
		if( typeid(*pkParent)==typeid(ZGuiTreebox) )
			return true;
		if( typeid(*pkParent)==typeid(ZGuiCheckbox) ) 
			return true;

	}

	ZGuiSkin* pkSkin = pkWndClicked->GetSkin();

	if(pkSkin == NULL)
		return false;

	if(pkSkin->m_bTransparent)
	{
		list<ZGuiWnd*> vkChildList;
		pkWndClicked->GetChildrens(vkChildList);

		for(list<ZGuiWnd*>::iterator it = vkChildList.begin(); it!=vkChildList.end(); it++)
		{
			 if((*it)->GetScreenRect().Inside(mx,my) && (*it)->IsVisible())
			 {
				 return true;
			 }
		}

		return false;
	}

	bool bIsTGA = false;
	int alpha_tex = pkSkin->m_iBkTexAlphaID;


	if(m_pkTexMan->TextureIsTGA(pkSkin->m_iBkTexID))
	{
		alpha_tex = pkSkin->m_iBkTexID;
		bIsTGA = true;
	}
	
	if(alpha_tex > 0)
	{
		m_pkZShaderSystem->Push("ZGui::ClickedWndAlphaTex");
		
		m_pkTexMan->BindTexture( alpha_tex );

		int horz_offset, vert_offset;
		horz_offset = mx - pkWndClicked->GetScreenRect().Left;
		vert_offset = my - pkWndClicked->GetScreenRect().Top;

		float x_offset = (float) horz_offset / pkWndClicked->GetScreenRect().Width();
		float y_offset = (float) vert_offset / pkWndClicked->GetScreenRect().Height();

		m_pkTexMan->EditStart( alpha_tex );
		Image* pkSurface = m_pkTexMan->EditGetImage( alpha_tex );

		if(pkSurface == NULL)
		{
			printf("Failed to call GetImage from texturemanager!\n");
			m_pkZShaderSystem->Pop();
			return false;
		}

		float tex_w = (float) pkSurface->m_iWidth;
		float tex_h = (float) pkSurface->m_iHeight;

		int mod = 0;
		if(bIsTGA)
			mod = (int) -tex_h;

		float dx = (int)(tex_w*x_offset);
		float dy = mod+(int)(tex_h*y_offset);

		if(dx < 0) dx = 0;
		if(dy < 0) dy = 0;

		//unsigned long pixel;
		color_rgba kColor;
		if(!pkSurface->get_pixel(dx, dy, kColor))
		{
			printf("Image::get_pixel Failed\n");
		}
		m_pkTexMan->EditEnd( alpha_tex );

		m_pkZShaderSystem->Pop();
		
		if(bIsTGA) 
		{
			if(kColor.a == 0) // En alpha pixel. Vi har INTE klickat på fönstret.
				return false;
			else
				return true; // Ej alpha pixel. Vi har klickat på fönstret.
		}

		if(kColor.r == 0 && kColor.g == 0 && kColor.b == 0 /*&& kColor.a == 0*/)
		{
			return true;
		}
		else 
		{
			//printf("Mouse click passing true\n");
			return false;
		}
	}
	else
	{
		return true;
	}

//	return false;
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
	if(m_pkActiveMenu && m_bClickedMenu == false)
	{
		m_pkActiveMenu->HideAll(); 
		m_pkActiveMenu = NULL;
		m_bClickedMenu = true;
	}
}

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

	GUIScaleMode eScaleMode;
	m_pkRenderer->GetScaleMode(eScaleMode);

	if(eScaleMode == GUIScaleProjMatBeforeRendering)
		TranslateMousePos(x,y);

	// Register public variables needed by the editbox.
	m_iMouseX = x; m_iMouseY = y;
	m_bMouseLeftPressed = bLBnPressed;

	if(m_bUseHardwareMouse == false)
		m_pkCursor->SetPos(x,y);

	//////////////////////////////////////////////////////////////////////////////
	// Pgr av att musmarkören som används i DM inte har sin klickpunkt i övre
	// vänstra hörnet utan 2 pixlar längre ner, kompenserar vi för det här.
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
			if((wnd = FindMainWnd(x,y)) != NULL) //if(wnd = FindMainWnd(x,y) ) -- Ändring 040701
			{
				if(wnd != m_pkActiveMainWin)
				{
					SetFocus(wnd->pkWnd);

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
		m_pkActiveMainWin->pkWnd = m_pkCapturedWindow;
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
	// en kontroll är captured och ej är ett fönster.
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

	bool bLeftPressed =  (m_bLeftButtonDown  == false && bLeftButtonDown  == true);
	bool bRightPressed = (m_bRightButtonDown == false && bRightButtonDown == true);
	

	// Har vänster musknapp klickats (men inte släppt)
	if( bLeftPressed || bRightPressed )
	{		

		if(pkFocusWindow)
		{			
			if(pkFocusWindow->m_bUseAlhpaTest)
			{
				if(ClickedWndAlphaTex(x,y,pkFocusWindow) == false)
				{
					m_bHandledMouse = false;

					// Stäng eventuell meny
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

				ZGuiWnd* test; 
//				if( (test=m_pkActiveMainWin->pkWnd->Find(x,y))) //!ClickedWndAlphaTex(x,y, ZGuiWnd::m_pkWndClicked))
				if( (test=m_pkActiveMainWin->pkWnd->Find(x,y)) != NULL) // 040701
				{
					if(ClickedWndAlphaTex(x,y,test)==true)
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

				if(pkParent && typeid(*pkParent)==typeid(ZGuiScrollbar))
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

	

	// Är vänster musknapp nertryckt?
	if( bLeftButtonDown == true && ZGuiWnd::m_pkWndClicked)
	{
		// Skall fönstret flyttas?
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

	// Har vänster musknapp släpts (men inte klickats)?
	if(bLeftReleased || bRightReleased)
	{
		m_bHandledMouse = false;

		// Byt tillbaks till button_up skin om musknappen har släppts (fast inte på knappen).
		if(ZGuiWnd::m_pkWndClicked)
			if( typeid(*ZGuiWnd::m_pkWndClicked)==typeid(ZGuiButton) )
				((ZGuiButton*) ZGuiWnd::m_pkWndClicked)->SetSkin(
					((ZGuiButton*) ZGuiWnd::m_pkWndClicked)->GetButtonUpSkin()); 

		if(pkFocusWindow && ZGuiWnd::m_pkWndClicked != NULL)
		{
			// Informera fönstret innan att det har tappat fokus.
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

			// Är markören fortfarande innanför fönstrets gränser?
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
						if	 ( typeid(*pkParent)!=typeid(ZGuiListbox) && 
							   typeid(*pkParent)!=typeid(ZGuiTreeboxNode)  ) // tillfällig ful lösning för att listboxitems inte skall generera COMMAND messages..
						{
							ZGuiWnd* pkMainWnd = m_pkActiveMainWin->pkWnd;

							// Menyalternativ har sitt egen förälderfönster.
							if(pkParent && typeid(*pkParent)==typeid(ZGuiMenu))
								pkMainWnd = pkParent;

							pkParams[0] = ZGuiWnd::m_pkWndClicked->GetID(); // control id
							pkParams[1] = (pkFocusWindow->m_bAcceptRightClicks && bRightReleased); // höger musknapp har triggat knapp kommandot
							m_pkActiveMainWin->pkCallback(pkMainWnd, ZGM_COMMAND,2,pkParams);

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







