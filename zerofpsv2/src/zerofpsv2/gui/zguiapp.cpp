// guibuilder.cpp: implementation of the ZGuiApp class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiapp.h"
#include "../basic/zfassert.h"
#include "../basic/zfresourcedb.h"
#include "../engine/res_texture.h"
#include "../render/texturemanager.h"
#include "../script/zfscript.h"
#include "zguiresourcemanager.h"
#include <typeinfo>
#include "../basic/zguifont.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiApp::ZGuiApp(ZGui::callback oMainWndProc)
{
	m_bGuiHaveFocus = false;
	m_uiWindowIDCounter = 1;
	m_pkScriptResHandle = NULL;
	m_oMainWndProc = oMainWndProc;
	m_szLastRadioBGroup = NULL;
	CreateNewRadiobuttonGroup("DefGUIRadioGroup", 1);
}

ZGuiApp::~ZGuiApp()
{
	map<string, ZGuiSkin*>::iterator itSkins = m_kSkins.begin();
	while(itSkins != m_kSkins.end())
	{
//		delete itSkins->second;
		itSkins++;
	}

// OBS! denna resurs hinner inte laddas ur eftersom expire delayen ligger på 5 sek.
//	if(m_pkScriptResHandle)
//		delete m_pkScriptResHandle;

/*	map<int, ZGuiWnd*>::iterator itWnds = m_kWindows.begin();
	while(itWnds != m_kWindows.end())
	{
		bool bIsWnd = typeid(*itWnds->second) == 
			typeid(ZGuiWnd) ? true : false;

		// Förstör bara de fönster som är "windows" eftersom funktionen
		// UnregisterWindow själv tar hand om att förstöra alla barn.
		if(itWnds->first == 10)
		{
			m_pkGui->UnregisterWindow(itWnds->second);
		}

		itWnds++;
	}*/
}

ZGuiWnd* ZGuiApp::GetWnd(int iID)
{
	map<int, ZGuiWnd*>::iterator res = m_kWindows.find(iID);
	if(res == m_kWindows.end())
		return NULL;
	return res->second;
}

ZGuiWnd* ZGuiApp::GetWnd(string strName)
{
	return m_pkResMan->Wnd(strName);
}


ZGuiSkin* ZGuiApp::GetSkin(string strName)
{
	map<string, ZGuiSkin*>::iterator res = m_kSkins.find(strName);
	if(res == m_kSkins.end())
		return NULL;
	return res->second;
}

ZGuiWnd* ZGuiApp::CreateWnd(GuiType eType, char* szResourceName, char* szText, ZGuiWnd* pkParent, 
								int x, int y, int w, int h, unsigned long uiFlags)
{
	
	int iID = m_uiWindowIDCounter;

	m_uiWindowIDCounter++;

	if(m_pkResMan->Wnd(szResourceName))
	{
		//printf("ZGuiApp::CreateWnd: WindowID already exist\n");

		ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResourceName);

		pkWnd->Show();
		m_pkGui->SetFocus(pkWnd);
		
		return pkWnd;
	}

	ZGuiWnd* pkWnd;

	const int LISTBOX_ITEM_HEIGHT = 20;
	const int COMBOBOX_ITEM_HEIGHT = 20;

	// Om parent fönstret är en TabControl så är uiFlags = sidnummret och parent fönstret
	// måste då tas fram på nytt.
	if( GetType(pkParent) == TabControl)
	{
		pkParent = ((ZGuiTabCtrl*)(pkParent))->GetPage(uiFlags);
		if(pkParent == NULL)
		{
			printf("Failed to create window! No such tab page\n");
		}
	}
	
	switch(eType)
	{
	case Wnd:
		pkWnd = new ZGuiWnd( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Button:
		pkWnd = new ZGuiButton( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Label:
		pkWnd = new ZGuiLabel( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Radiobutton:
		pkWnd = new ZGuiRadiobutton( Rect(x,y,x+w,y+h), pkParent, iID, 
			m_iLastRadioBGroup, m_szLastRadioBGroup, NULL, true);
		break;
	case Checkbox:
		pkWnd = new ZGuiCheckbox( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Scrollbar:
		pkWnd = new ZGuiScrollbar( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Slider:
		pkWnd = new ZGuiSlider( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Listbox:
		pkWnd = new ZGuiListbox( Rect(x,y,x+w,y+h), pkParent, true, iID, 
			LISTBOX_ITEM_HEIGHT, NULL, NULL, NULL);
		if(uiFlags & READ_ONLY) 
			((ZGuiListbox*) pkWnd)->SetEnable(false);
		break;
	case Combobox:
		pkWnd = new ZGuiCombobox( Rect(x,y,x+w,y+h), pkParent, true, iID,
			COMBOBOX_ITEM_HEIGHT, NULL, NULL, NULL, GetSkin("DefCBTopItemSkin"));
		break;
	case Textbox:
		pkWnd = new ZGuiTextbox( Rect(x,y,x+w,y+h), pkParent, true, iID, 
			uiFlags & EB_IS_MULTILINE);
		if(uiFlags & READ_ONLY) 
			((ZGuiTextbox*) pkWnd)->SetReadOnly(true);
		break;
	case Treebox:
		pkWnd = new ZGuiTreebox( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case TabControl:
		pkWnd = new ZGuiTabCtrl( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	}
	
	switch(eType)
	{
	case Wnd:
		pkWnd->SetSkin(GetSkin("DefWndSkin"));
		break;
	case Button:
		static_cast<ZGuiButton*>(pkWnd)->SetButtonUpSkin(GetSkin("DefBnUSkin"));
		static_cast<ZGuiButton*>(pkWnd)->SetButtonDownSkin(GetSkin("DefBnDSkin"));
		static_cast<ZGuiButton*>(pkWnd)->SetButtonHighLightSkin(GetSkin("DefBnFSkin"));
		break;
	case Label:
		break;
	case Radiobutton:
		static_cast<ZGuiRadiobutton*>(pkWnd)->SetButtonUnselectedSkin(GetSkin("DefRBnUSkin"));
		static_cast<ZGuiRadiobutton*>(pkWnd)->SetButtonSelectedSkin(GetSkin("DefRBnDSkin"));
		break;
	case Checkbox:
		static_cast<ZGuiCheckbox*>(pkWnd)->SetButtonUncheckedSkin(GetSkin("DefCBnUSkin"));
		static_cast<ZGuiCheckbox*>(pkWnd)->SetButtonCheckedSkin(GetSkin("DefCBnDSkin"));
		break;
	case Scrollbar:
		static_cast<ZGuiCheckbox*>(pkWnd)->SetSkin(GetSkin("DefSBrBkSkin"));
		static_cast<ZGuiScrollbar*>(pkWnd)->SetThumbButtonSkins(GetSkin("DefSBrNSkin"), 
			GetSkin("DefSBrFSkin"));
		static_cast<ZGuiScrollbar*>(pkWnd)->SetScrollButtonUpSkins(
			GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"));
		static_cast<ZGuiScrollbar*>(pkWnd)->SetScrollButtonDownSkins(
			GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d"));
		break;
	case Slider:
		static_cast<ZGuiSlider*>(pkWnd)->SetSliderSkin(GetSkin("DefSliderSkin"));
		static_cast<ZGuiSlider*>(pkWnd)->SetBkSkin(GetSkin("DefSliderBkSkin"));
		break;
	case Listbox:
		static_cast<ZGuiListbox*>(pkWnd)->SetSkin( GetSkin("DefLBBkSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetItemNormalSkin(   GetSkin("DefLBitemUSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetItemSelectedSkin( GetSkin("DefLBitemDSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetItemHighLightSkin(GetSkin("DefLBitemFSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin"),			
			GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"),
			GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d"));
		break;
	case Combobox:
		static_cast<ZGuiCombobox*>(pkWnd)->SetSkin( GetSkin("DefCBBkSkin") );
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetSkin( GetSkin("DefCBBkSkin") );
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetItemNormalSkin(   GetSkin("DefCBitemUSkin"));
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetItemSelectedSkin( GetSkin("DefCBitemDSkin"));
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetItemHighLightSkin(GetSkin("DefCBitemFSkin"));
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin"),
			GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"),
			GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d") );
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->GetScrollbar()->SetScrollButtonDownSkins( 
			GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d") );
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->GetScrollbar()->SetScrollButtonUpSkins( 
			GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d") );
		break;
	case Textbox:
		static_cast<ZGuiTextbox*>(pkWnd)->SetSkin(GetSkin("DefTextboxSkin"));
		static_cast<ZGuiTextbox*>(pkWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin"),
			GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"),
			GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d"));
		break;
	case Treebox:
		static_cast<ZGuiTreebox*>(pkWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin"), 
			GetSkin("DefSBrScrollUpSkin_u"), GetSkin("DefSBrScrollUpSkin_d"),
			GetSkin("DefSBrScrollDownSkin_u"), GetSkin("DefSBrScrollDownSkin_d"));
		static_cast<ZGuiTreebox*>(pkWnd)->SetSkin(GetSkin("DefTreeboxBkSkin"));
		static_cast<ZGuiTreebox*>(pkWnd)->InsertBranchSkin(0, GetSkin("DefTreeNodeChildSkin"));
		static_cast<ZGuiTreebox*>(pkWnd)->InsertBranchSkin(1, GetSkin("DefTreeNodeParentClosedSkin"));
		static_cast<ZGuiTreebox*>(pkWnd)->InsertBranchSkin(2, GetSkin("DefTreeNodeParentOpenSkin"));
		break;
	case TabControl:
		static_cast<ZGuiTabCtrl*>(pkWnd)->SetSkin(GetSkin("DefTabCtrlBkSkin"));	
		static_cast<ZGuiTabCtrl*>(pkWnd)->SetNextButtonSkin( GetSkin("DefTabBnNextUSkin"), 
			GetSkin("DefTabBnNextDSkin"), GetSkin("DefTabBnNextUSkin"));
		static_cast<ZGuiTabCtrl*>(pkWnd)->SetPrevButtonSkin( GetSkin("DefTabBnPrevUSkin"), 
			GetSkin("DefTabBnPrevDSkin"), GetSkin("DefTabBnPrevUSkin"));
		static_cast<ZGuiTabCtrl*>(pkWnd)->InsertTabSkin(0, GetSkin("DefTabPageBackSkin"));
		static_cast<ZGuiTabCtrl*>(pkWnd)->InsertTabSkin(1, GetSkin("DefTabPageFrontSkin"));
		break;
	}

	if(eType == Wnd /*&& pkParent == NULL*/) // <- OBS! Kommer göra att GUI't mistclient inte funkar tror jag...
	{
		if(!m_pkGui->AddMainWindow(iID, pkWnd, szResourceName, m_oMainWndProc, true))
		{
			m_pkGui->SetFocus(pkWnd);
			return NULL;
		}
	}
	else
	{
		if(szResourceName)
		{
			m_pkGui->RegisterWindow(pkWnd, szResourceName);
		}
	}

	if(szText)
	{
		pkWnd->SetText(szText);
	}

	m_kWindows.insert(map<int, ZGuiWnd*>::value_type(iID, pkWnd));

	pkWnd->SetGUI(m_pkGui);
	pkWnd->SetFont(m_pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT));  

	//
	// Change position and size on the window according to resolution.
	//

//	pkWnd->Rescale(800,600, GetWidth(),GetHeight());

	pkWnd->Show(); 
	
	return pkWnd;
}


ZGuiWnd* ZGuiApp::CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
								char* szLabel, int x, int y, int w, int h, 
								unsigned long uiFlags)
{
	ZGuiWnd* pkParent = m_pkResMan->Wnd(string(szParentName)); // GetWnd(parentID);
	return CreateWnd(eType, szWndName, szLabel, pkParent, x, y, w, h, uiFlags);
}

ZGuiSkin* ZGuiApp::AddSkinFromScript(char *szName, ZFScriptSystem *pkScript, ZGuiSkin* pkSkin)
{
	ZGuiSkin* pkNewSkin;
	
	if(pkSkin == NULL)
		pkNewSkin = new ZGuiSkin();		// LEAK - MistServer, Nothing loaded. (FIXED!)
	else
		pkNewSkin = pkSkin;

	char szData[250];
	double dData;
	double dColMult = 1.0 / 255.0;

	lua_State* pkLuaState = GetGuiScript()->m_pkLuaState;

	// Textures
	if(pkScript->GetGlobal(pkLuaState, szName, "tex1", szData))
		pkNewSkin->m_iBkTexID = GetTexID(szData);
	if(pkScript->GetGlobal(pkLuaState, szName, "tex2", szData))
		pkNewSkin->m_iHorzBorderTexID = GetTexID(szData);
	if(pkScript->GetGlobal(pkLuaState, szName, "tex3", szData))
		pkNewSkin->m_iVertBorderTexID = GetTexID(szData);
	if(pkScript->GetGlobal(pkLuaState, szName, "tex4", szData))
		pkNewSkin->m_iBorderCornerTexID = GetTexID(szData);

	// Alpha maps
	if(pkScript->GetGlobal(pkLuaState, szName, "tex1a", szData))
		pkNewSkin->m_iBkTexAlphaID = GetTexID(szData);
	if(pkScript->GetGlobal(pkLuaState, szName, "tex2a", szData))
		pkNewSkin->m_iHorzBorderTexAlphaID = GetTexID(szData);
	if(pkScript->GetGlobal(pkLuaState, szName, "tex3a", szData))
		pkNewSkin->m_iVertBorderTexAlphaID = GetTexID(szData);
	if(pkScript->GetGlobal(pkLuaState, szName, "tex4a", szData))
		pkNewSkin->m_iBorderCornerTexAlphaID = GetTexID(szData);

	// Color Bk
	if(pkScript->GetGlobal(pkLuaState, szName, "bkR", dData))
		pkNewSkin->m_afBkColor[0] = dColMult * dData;
	if(pkScript->GetGlobal(pkLuaState, szName, "bkG", dData))
		pkNewSkin->m_afBkColor[1] = dColMult * dData;
	if(pkScript->GetGlobal(pkLuaState, szName, "bkB", dData))
		pkNewSkin->m_afBkColor[2] = dColMult * dData;

	// Color Border
	if(pkScript->GetGlobal(pkLuaState, szName, "borderR", dData))
		pkNewSkin->m_afBorderColor[0] = dColMult * dData;
	if(pkScript->GetGlobal(pkLuaState, szName, "borderG", dData))
		pkNewSkin->m_afBorderColor[1] = dColMult * dData;
	if(pkScript->GetGlobal(pkLuaState, szName, "borderB", dData))
		pkNewSkin->m_afBorderColor[2] = dColMult * dData;

	// Border size, Tile texture, Transparency
	if(pkScript->GetGlobal(pkLuaState, szName, "bd_size", dData))
		pkNewSkin->m_unBorderSize = (unsigned short) dData;
	if(pkScript->GetGlobal(pkLuaState, szName, "tile", dData))
		pkNewSkin->m_bTileBkSkin = dData > 0 ? true : false;
	if(pkScript->GetGlobal(pkLuaState, szName, "trans", dData))
		pkNewSkin->m_bTransparent = dData > 0 ? true : false;

	return pkNewSkin;
}

void ZGuiApp::InitTextures(ZFScriptSystem* pkScript)
{
	// first texture loaded do not show up (?). fulhack fix: load crap texture.
	int crap = m_pkTexMan->Load("/data/textures/gui/crap.bmp", 0);

	typedef map<string, ZGuiSkin*>::value_type strSkin;

	char* szDefNames[] =
	{
		"DefWndSkin",						"DefBnUSkin",
		"DefBnDSkin",						"DefBnFSkin",
		"DefRBnUSkin",						"DefRBnDSkin",
		"DefCBnUSkin",						"DefCBnDSkin",
		"DefSBrNSkin",						"DefSBrFSkin",
		"DefSBrBkSkin",					"DefSliderSkin",
		"DefSliderBkSkin",				"DefLBitemUSkin",
		"DefLBitemDSkin",					"DefLBitemFSkin",
		"DefLBBkSkin",						"DefCBitemUSkin",
		"DefCBitemDSkin",					"DefCBitemFSkin",
		"DefCBBkSkin",						"DefCBTopItemSkin",
		"DefTextboxSkin",					"DefTreeboxBkSkin",
		"DefTreeNodeChildSkin",			"DefTreeNodeParentClosedSkin",
		"DefTreeNodeParentOpenSkin",  "DefTabCtrlBkSkin",
		"DefTabBnPrevUSkin",				"DefTabBnPrevDSkin",
		"DefTabBnNextUSkin",				"DefTabBnNextDSkin",
		"DefTabPageBackSkin",			"DefTabPageFrontSkin",
		"DefSBrScrollDownSkin_u",		"DefSBrScrollDownSkin_d",
		"DefSBrScrollUpSkin_u",			"DefSBrScrollUpSkin_d",
	};

	for(int i=0; i<sizeof(szDefNames)/sizeof(szDefNames[0]); i++) 
		m_kSkins.insert( strSkin(szDefNames[i], AddSkinFromScript(szDefNames[i], pkScript) ) );
}

void ZGuiApp::InitializeGui(ZGui* pkGui, TextureManager* pkTexMan, 
									 ZFScriptSystem* pkScript,
									 ZGuiResourceManager* pkResMan,
									 char* szFontTexture, 
									 char* szScriptFile)
{

	m_pkGui = pkGui;
	m_pkTexMan = pkTexMan;
	m_pkResMan = pkResMan;

	m_pkTexMan->Load("data/textures/gui/slask.bmp", 0); // första misslyckas, vet inte varför..

	/*m_pkGui->SetCursor(0,0, m_pkTexMan->Load("data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/cursor_a.bmp", 0), 32, 32);*/

	ZGuiFont* pkDefaultFont = new ZGuiFont(16,16,0,ZG_DEFAULT_GUI_FONT);				// LEAK - MistServer, Nothing loaded. (FIXED)

//	char defFontPath[] = szFontTexture; //"data/textures/text/paternoster8.bmp";
	
	pkDefaultFont->CreateFromFile(szFontTexture);		

	m_pkGui->SetDefaultFont(pkDefaultFont);

	// Start skript filen för GUI:t.
	// Läs in tex filnamn på textuerer osv.

	if(m_pkScriptResHandle)
		delete m_pkScriptResHandle;

	m_pkScriptResHandle = new ZFResourceHandle;												// LEAK - MistServer, Nothing loaded. 
	if(!m_pkScriptResHandle->SetRes(szScriptFile))
	{
		printf("Failed to load gui_create.lua\n");
		return;
	}

	InitTextures(pkScript);

	// Låt skriptfilen skapa alla fönster.
	pkScript->Call(m_pkScriptResHandle, "CreateMainWnds", 0, 0);

}

int ZGuiApp::GetTexID(char *szFile)
{
	char szPath[256];
	sprintf(szPath, "%s%s", "/data/textures/gui/", szFile);
	return m_pkTexMan->Load(szPath, 0);
}


bool ZGuiApp::CreateNewRadiobuttonGroup(const char *szName, int id)
{
	if(m_szLastRadioBGroup)
		delete[] m_szLastRadioBGroup;

	m_szLastRadioBGroup = new char[strlen(szName)+1];
	strcpy(m_szLastRadioBGroup, szName);

	m_iLastRadioBGroup = id;

	return true;
}

/*void ZGuiApp::AddListItem(int iListboxID, char *szText)
{
	ZGuiWnd* pkWnd = GetWnd(iListboxID);

	bool bCombobox = GetType(pkWnd) == Combobox ? true : false;

	if(bCombobox == false)
	{
		ZGuiListbox* pkListBox = static_cast<ZGuiListbox*>(pkWnd);
		int iIndex = pkListBox->GetItemCount(); 
		pkListBox->AddItem(szText, iIndex, false); 
	}
	else
	{
		ZGuiCombobox* pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
		pkComboBox->AddItem(szText, -1, false); 
	}
}*/

void ZGuiApp::AddListItem(char *szListboxResName, char *szText)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(szListboxResName);

	bool bCombobox = GetType(pkWnd) == Combobox ? true : false;

	if(bCombobox == false)
	{
		ZGuiListbox* pkListBox = static_cast<ZGuiListbox*>(pkWnd);
		int iIndex = pkListBox->GetItemCount(); 
		pkListBox->AddItem(szText, iIndex, false); 
	}
	else
	{
		ZGuiCombobox* pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
		pkComboBox->AddItem(szText, -1, false); 
	}
}

void ZGuiApp::AddTreeItem(char* szTreeBox, const char* szID, 
								  const char* szIDParent, char* szText, 
								  unsigned char iNodeSkinNormal, unsigned char iNodeSkinSelected)
{
	ZGuiTreebox* pkTreeBox = static_cast<ZGuiTreebox*>(m_pkResMan->Wnd(string(szTreeBox)));

	if(szIDParent != NULL)
	{
		string strParent = string(szIDParent);
		pkTreeBox->AddItem(strParent, szText, iNodeSkinNormal, iNodeSkinSelected, szID);
	}
	else
	{
		pkTreeBox->AddItem(pkTreeBox->Root(), szText, iNodeSkinNormal, iNodeSkinSelected, szID);
	}
}

void ZGuiApp::AddTabPage(char* szTabBox, char* szPageResName, char* szPageLabel)
{
	ZGuiTabCtrl* pkTabBox = static_cast<ZGuiTabCtrl*>(m_pkResMan->Wnd(string(szTabBox))); 
	pkTabBox->InsertPage(szPageResName, pkTabBox->GetNumPages(), szPageLabel, 0);
	pkTabBox->SetCurrentPage(0); 
}

ZGuiWnd* ZGuiApp::GetTabPage(char* szTabBox, int iPage)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(string(szTabBox));

	bool bIsTabCtrl = (GetType(pkWnd) == TabControl);

	ZFAssert(pkWnd, "ZGuiApp::GetTabPage: Window is not a tab control!");

	return static_cast<ZGuiTabCtrl*>(pkWnd)->GetPage(iPage);	
}

void ZGuiApp::SetText(char* szWnd, char* szText, bool bResize)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
		pkWnd->SetText(szText, bResize);
}

void ZGuiApp::SetTextInt(char* szWnd, int iNumber, bool bResize)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		char szText[25];
		sprintf(szText, "%i", iNumber);
		pkWnd->SetText(szText, bResize);
	}
}

void ZGuiApp::SetTextFloat(char* szWnd, float fNumber, bool bResize)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		char szText[25];
		sprintf(szText, "%f", fNumber);
		pkWnd->SetText(szText, bResize);
	}
}

char* ZGuiApp::GetText(char* szWnd)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
		return pkWnd->GetText(); 	
	
	return NULL;
}

char* ZGuiApp::GetSelItem(char* szWnd)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		if(GetType(pkWnd) == Listbox)
		{
			ZGuiListitem* pkItem = ((ZGuiListbox*) pkWnd)->GetSelItem();

			if(pkItem)
				return pkItem->GetText();
		}
		else
		if(GetType(pkWnd) == Combobox)
		{
			ZGuiListitem* pkItem = ((ZGuiCombobox*) pkWnd)->GetListbox()->GetSelItem();

			if(pkItem)
				return pkItem->GetText();
		}
	}
	
	return NULL;
}

int ZGuiApp::GetTextInt(char* szWnd, bool* pkSuccess)
{
	ZGuiWnd* pkWnd;

	if(pkSuccess)
		*pkSuccess = false;

	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		char* szText = pkWnd->GetText();
		if(szText != NULL)
		{
			if(pkSuccess)
				*pkSuccess = true;
			return atoi(szText);
		}
	}	

	return 0;
}

float ZGuiApp::GetTextFloat(char* szWnd, bool *pkSuccess)
{
	if(pkSuccess)
		*pkSuccess = false;

	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		char* szText = pkWnd->GetText();
		if(szText != NULL)
		{
			if(pkSuccess)
				*pkSuccess = true;
			return (float) atof(szText);
		}
	}

	return 0;
}

bool ZGuiApp::IsButtonChecked(char* szWnd)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		if(GetType(pkWnd) == Radiobutton)
		{
			return ((ZGuiRadiobutton*)pkWnd)->GetButton()->IsChecked();
		}
		else
		if(GetType(pkWnd) == Checkbox)
		{
			return ((ZGuiCheckbox*)pkWnd)->IsChecked();
		}
	}

	return false;
}

GuiType ZGuiApp::GetType(ZGuiWnd *pkWnd)
{
	if(pkWnd == NULL)
		return GuiType_Error;

	const type_info& t = typeid(*pkWnd);

	if(t==typeid(ZGuiTextbox))
		return Textbox;
	else
	if(t==typeid(ZGuiScrollbar))
		return Scrollbar;
	else
	if(t==typeid(ZGuiRadiobutton))
		return Radiobutton;
	else
	if(t==typeid(ZGuiListbox))
		return Listbox;
	else
	if(t==typeid(ZGuiLabel))
		return Label;
	else
	if(t==typeid(ZGuiCheckbox))
		return Checkbox;
	else
	if(t==typeid(ZGuiButton))
		return Button;
	else
	if(t==typeid(ZGuiWnd))
		return Wnd;
	else
	if(t==typeid(ZGuiCombobox))
		return Combobox;
	else
	if(t==typeid(ZGuiTreebox))
		return Treebox;
	else
	if(t==typeid(ZGuiTabCtrl))
		return TabControl;
	else
	if(t==typeid(ZGuiSlider))
		return Slider;	

	return GuiType_Error;
}

void ZGuiApp::CloseWindow(char* szResName)
{
	if(szResName == NULL)
		return;

	ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResName);
	
	if(pkWnd)
		pkWnd->Hide();
}

bool ZGuiApp::ChangeSkin(ZFScriptSystem* pkScript, char* szID, 
								 char *szSkinName, char* szSkinType)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(string(szID));

	if(pkWnd == NULL)
		return false;

	ZGuiSkin* pkSkin = new ZGuiSkin;

	if(pkSkin)
		AddSkinFromScript(szSkinName, pkScript, pkSkin);
	else
	{
		printf("Failed to change skin!\n");
		return false;
	}

	GuiType eType = GetType(pkWnd);

	if( eType == Wnd || eType == Label )
		pkWnd->SetSkin(pkSkin);
	else
	if( eType == Button )
	{
		if(strcmp(szSkinType, "Button up") == 0)
			static_cast<ZGuiButton*>(pkWnd)->SetButtonUpSkin(pkSkin);
		else
		if(strcmp(szSkinType, "Button down") == 0)
			static_cast<ZGuiButton*>(pkWnd)->SetButtonDownSkin(pkSkin);
		else
		if(strcmp(szSkinType, "Button focus") == 0)
			static_cast<ZGuiButton*>(pkWnd)->SetButtonHighLightSkin(pkSkin);
	}
	else
	if( eType == Checkbox)
	{
		if(strcmp(szSkinType, "Button up") == 0)
			static_cast<ZGuiCheckbox*>(pkWnd)->SetButtonUncheckedSkin(pkSkin);
		else
		if(strcmp(szSkinType, "Button down") == 0)
			static_cast<ZGuiCheckbox*>(pkWnd)->SetButtonCheckedSkin(pkSkin);
	}

	return true;
}

void ZGuiApp::ResizeWnd(char *szResName, int w, int h)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResName);
	if(pkWnd)
	{
		Rect rc = pkWnd->GetScreenRect();

		if(w == -1)
			w = rc.Right - rc.Left;

		if(h)
			h = rc.Bottom - rc.Top;

		pkWnd->Resize(w, h); 
	}
	else
	{
		printf("Failed to find window to resize!\n");
	}
}

bool ZGuiApp::IsWndVisible(char* szResName)
{

	ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResName);
	if(pkWnd != NULL)
	{
		if(pkWnd->IsVisible())
			return true;
	}
	
	return false;
}

void ZGuiApp::ClearListbox(char *szResName)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResName);

	bool bCombobox = GetType(pkWnd) == Combobox ? true : false;

	if(bCombobox == false)
	{
		ZGuiListbox* pkListBox = static_cast<ZGuiListbox*>(pkWnd);
		pkListBox->RemoveAllItems();
	}
	else
	{
		ZGuiCombobox* pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
		pkComboBox->RemoveAllItems();
	}
}

int ZGuiApp::GetWndID(char* szResName)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResName);
	if(pkWnd)
		return pkWnd->GetID();
	
	return -1;
}

int ZGuiApp::GetWidth()
{
	return m_pkGui->m_iResX;
}

int ZGuiApp::GetHeight()
{
	return m_pkGui->m_iResY;
}

ZFScript* ZGuiApp::GetGuiScript()
{
	return (ZFScript*) m_pkScriptResHandle->GetResourcePtr();  
}

