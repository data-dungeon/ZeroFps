// guibuilder.cpp: implementation of the ZGuiApp class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiapp.h"
#include "../basic/zfassert.h"
#include "../basic/zfresourcedb.h"
#include "../engine/res_texture.h"
#include "../render/texturemanager.h"
#include "../render/glguirender.h"
#include "zguiresourcemanager.h"
#include <typeinfo>
#include "../basic/zguifont.h"
#include "zguiapp.h"

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
			m_pkGuiSys->UnregisterWindow(itWnds->second);
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
								int x, int y, int w, int h, unsigned long uiFlags,
								WndAlignent eAlignment, WndResizeType eResizeType)
{
	
	int iID = m_uiWindowIDCounter;

	m_uiWindowIDCounter++;

	if(m_pkResMan->Wnd(szResourceName))
	{
		//printf("ZGuiApp::CreateWnd: WindowID already exist\n");

		ZGuiWnd* pkWnd = m_pkResMan->Wnd(szResourceName);

		pkWnd->Show();
		m_pkGuiSys->SetFocus(pkWnd);
		
		return pkWnd;
	}

	ZGuiWnd* pkWnd;

	const int LISTBOX_ITEM_HEIGHT = 20;
	const int COMBOBOX_ITEM_HEIGHT = 20;

	GUIScaleMode eScaleMode;
	m_pkRenderer->GetScaleMode(eScaleMode);

	if(eScaleMode == GUIScaleManually)
	{
		int iNewWidth = -1, iNewHeight = -1;

		if(eResizeType == ResizeWidth || eResizeType == Resize)
		{
			float width_mod = (float) GetWidth() / 800.0f;
			iNewWidth = (int) (800.0f*width_mod);
		}

		if(eResizeType == ResizeHeight || eResizeType == Resize)
		{
			float height_mod = (float) GetHeight() / 600.0f;
			iNewHeight = (int) (600.0f*height_mod);
		}

		if(iNewWidth != -1) w = iNewWidth;
		if(iNewHeight != -1) h = iNewHeight;
	}

	// Om parent fönstret är en TabControl så är uiFlags = sidnummret och parent fönstret
	// måste då tas fram på nytt.
	if( GetWndType(pkParent) == TabControl)
	{
		AddTabPage((char*)pkParent->GetName(), szResourceName, szText);
		return ((ZGuiTabCtrl*)(pkParent))->GetPage(uiFlags);

/*		pkParent = ((ZGuiTabCtrl*)(pkParent))->GetPage(uiFlags);

		printf("pkParent Name = %s\n", pkParent->GetName());
		if(pkParent == NULL)
		{
			printf("Failed to create window! No such tab page\n");
		}*/
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
	case Menu:
		pkWnd = new ZGuiMenu( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	default:
		return NULL;
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
			GetSkin("DefSBrNSkin"),		GetSkin("DefSBrFSkin"),			
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

	// Skapa nya texturer och kopiera de gamla mot nya unika...
	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	pkWnd->GetWndSkinsDesc(vkSkinDesc);
	for(unsigned int i=0; i<vkSkinDesc.size(); i++)
	{
		ZGuiSkin* pkPrevSkin = *vkSkinDesc[i].first; 

		if(pkPrevSkin)
		{
				//WARNING!!!!!!!!!!   VALGRIND KLAGAR PÅ ATT DENNA INTE AVALOKERAS!!!					
			ZGuiSkin* pkNewSkin = new ZGuiSkin(pkPrevSkin);		
			*vkSkinDesc[i].first = pkNewSkin;
		}
		else
		{
			*vkSkinDesc[i].first = new ZGuiSkin; 
			(*vkSkinDesc[i].first)->m_bTransparent = true;
		}
	}

	if(eType == Wnd) 
	{
		if(!m_pkGuiSys->AddMainWindow(iID, pkWnd, szResourceName, m_oMainWndProc, true))
		{
			m_pkGuiSys->SetFocus(pkWnd);
			return NULL;
		}
	}
	else
	{
		if(szResourceName)
		{
			m_pkGuiSys->RegisterWindow(pkWnd, szResourceName);
		}
	}

	if(szText)
	{
		pkWnd->SetText(szText);
	}

	m_kWindows.insert(map<int, ZGuiWnd*>::value_type(iID, pkWnd));

	pkWnd->SetGUI(m_pkGuiSys);
	pkWnd->SetFont(m_pkResMan->Font("defguifont"));  


//	pkWnd->Rescale(800,600, GetWidth(),GetHeight());

	pkWnd->Show(); 

	if(uiFlags & CREATE_WND_HIDDEN) 
		pkWnd->Hide();

	if(eScaleMode == GUIScaleManually)
	{
		float parent_width = 800, parent_height = 600;

		if(pkWnd->GetParent() != NULL)
		{
			parent_width = (float) pkWnd->GetParent()->GetScreenRect().Width();
			parent_height = (float) pkWnd->GetParent()->GetScreenRect().Height();
		}

		//if(pkWnd->GetParent() == NULL || pkWnd->GetParent() == GetWnd("GuiMainWnd") )
		{
			int new_x=-1, new_y=-1;
			Rect rc = pkWnd->GetScreenRect();

			switch(eAlignment)
			{
			case BottomRight:
				new_x = GetWidth() - (800-rc.Right) - rc.Width();
				new_y = GetHeight() - (600-rc.Bottom) - rc.Height();
				break;
			case TopRight:
				new_x = GetWidth() - (800-rc.Right) - rc.Width();
				new_y = rc.Top;
				break;
			case BottomLeft:
				new_x = rc.Left;
				new_y = GetHeight() - (600-rc.Bottom) - rc.Height();
				break;
			case CenterHorz:
				new_x = (((rc.Left + (rc.Right-rc.Left)/2) / 800.0f) * 
					(float) GetWidth()) - (rc.Right-rc.Left)/2; 
				new_y = rc.Top;
				break;
			case CenterVert:
				new_x = rc.Left;
				new_y = (((rc.Top + (rc.Bottom-rc.Top)/2) / 600.0f) * 
					(float) GetHeight()) - (rc.Bottom-rc.Top)/2; 
				break;
			case Center:
				new_x = (((rc.Left + (rc.Right-rc.Left)/2) / 800.0f) * 
					(float) GetWidth()) - (rc.Right-rc.Left)/2; 
				new_y = (((rc.Top + (rc.Bottom-rc.Top)/2) / 600.0f) * 
					(float) GetHeight()) - (rc.Bottom-rc.Top)/2; 
				break;
			}

			if(!(new_x==-1 && new_y==-1))
				pkWnd->SetPos(new_x, new_y, true, true); 
		}
	}
	
	return pkWnd;
}


ZGuiWnd* ZGuiApp::CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
								char* szLabel, int x, int y, int w, int h, unsigned long uiFlags, 
								WndAlignent eAlignment, WndResizeType eResizeType)
{
	ZGuiWnd* pkParent = m_pkResMan->Wnd(string(szParentName)); // GetWnd(parentID);
	return CreateWnd(eType, szWndName, szLabel, pkParent, x, y, w, h, 
		uiFlags, eAlignment, eResizeType);
}

ZGuiSkin* ZGuiApp::AddSkinFromScript(char *szName, ZGuiSkin* pkSkin)
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
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex1", szData))
		pkNewSkin->m_iBkTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex2", szData))
		pkNewSkin->m_iHorzBorderTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex3", szData))
		pkNewSkin->m_iVertBorderTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex4", szData))
		pkNewSkin->m_iBorderCornerTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;

	// Alpha maps
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex1a", szData))
		pkNewSkin->m_iBkTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex2a", szData))
		pkNewSkin->m_iHorzBorderTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex3a", szData))
		pkNewSkin->m_iVertBorderTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex4a", szData))
		pkNewSkin->m_iBorderCornerTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;

	// Color Bk
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bkR", dData))
		pkNewSkin->m_afBkColor[0] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bkG", dData))
		pkNewSkin->m_afBkColor[1] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bkB", dData))
		pkNewSkin->m_afBkColor[2] = (float) (dColMult * dData);

	// Color Border
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "borderR", dData))
		pkNewSkin->m_afBorderColor[0] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "borderG", dData))
		pkNewSkin->m_afBorderColor[1] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "borderB", dData))
		pkNewSkin->m_afBorderColor[2] = (float) (dColMult * dData);

	// Border size, Tile texture, Transparency
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bd_size", dData))
		pkNewSkin->m_unBorderSize = (unsigned short) dData;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tile", dData))
		pkNewSkin->m_bTileBkSkin = dData > 0 ? true : false;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "trans", dData))
		pkNewSkin->m_bTransparent = dData > 0 ? true : false;

	// Number of 90 degree rots on bk skin
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "rots90deg", dData))
		pkNewSkin->m_ucRots90Degree = dData;

	// Spritemap rect
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "rcBkTile", szData))
	{
		char szFormatet[128];
		strcpy(szFormatet, szData);

		// Ta bort alla skräptecken ur datat
		char temp[128], c=0, l=strlen(szFormatet);
		for(int j=0; j<l; j++)
			if(szFormatet[j] != ' ')
				temp[c++] = szFormatet[j];
		temp[c] = 0;
		strcpy(szFormatet, temp);

		// Hämta x,y,w,h från textdatat
		char x[5] = "\0\0\0\0", y[5] = "\0\0\0\0", w[5]= "\0\0\0\0", h[5]= "\0\0\0\0";
		char* rect[] = { x, y, w, h };

		int a=0, b=0;
		for(int j=0; j<strlen(szFormatet); j++)
			if(szFormatet[j] != ',')
				rect[a][b++] = szFormatet[j];
			else
			{
				a++;
				b=0;
			}

		pkNewSkin->m_rcBkTile.Left   = atoi(rect[0]);
		pkNewSkin->m_rcBkTile.Top    = atoi(rect[1]);
		pkNewSkin->m_rcBkTile.Right  = atoi(rect[2]);
		pkNewSkin->m_rcBkTile.Bottom = atoi(rect[3]);
	}

	return pkNewSkin;
}

ZGuiSkin* ZGuiApp::AddSkinFromScript2(char *szName, lua_State* pkLuaState, 
												  ZGuiSkin* pkSkin)
{
	ZGuiSkin* pkNewSkin;
	
	if(pkSkin == NULL)
		pkNewSkin = new ZGuiSkin();		// LEAK - MistServer, Nothing loaded. (FIXED!)
	else
		pkNewSkin = pkSkin;

	char szData[250];
	double dData;
	double dColMult = 1.0 / 255.0;

	//lua_State* pkLuaState = ((ZFScript*) pkResHandle->GetResourcePtr())->m_pkLuaState; //GetGuiScript()->m_pkLuaState;

	// Textures
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex1", szData))
		pkNewSkin->m_iBkTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex2", szData))
		pkNewSkin->m_iHorzBorderTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex3", szData))
		pkNewSkin->m_iVertBorderTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex4", szData))
		pkNewSkin->m_iBorderCornerTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;

	// Alpha maps
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex1a", szData))
		pkNewSkin->m_iBkTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex2a", szData))
		pkNewSkin->m_iHorzBorderTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex3a", szData))
		pkNewSkin->m_iVertBorderTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex4a", szData))
		pkNewSkin->m_iBorderCornerTexAlphaID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;

	// Color Bk
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bkR", dData))
		pkNewSkin->m_afBkColor[0] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bkG", dData))
		pkNewSkin->m_afBkColor[1] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bkB", dData))
		pkNewSkin->m_afBkColor[2] = (float) (dColMult * dData);

	// Color Border
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "borderR", dData))
		pkNewSkin->m_afBorderColor[0] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "borderG", dData))
		pkNewSkin->m_afBorderColor[1] = (float) (dColMult * dData);
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "borderB", dData))
		pkNewSkin->m_afBorderColor[2] = (float) (dColMult * dData);

	// Border size, Tile texture, Transparency
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "bd_size", dData))
		pkNewSkin->m_unBorderSize = (unsigned short) dData;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tile", dData))
		pkNewSkin->m_bTileBkSkin = dData > 0 ? true : false;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "trans", dData))
		pkNewSkin->m_bTransparent = dData > 0 ? true : false;

	// Number of 90 degree rots on bk skin
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "rots90deg", dData))
		pkNewSkin->m_ucRots90Degree = dData;

	return pkNewSkin;
}

void ZGuiApp::InitDefaultSkins(/*ZFScriptSystem* pkScript*/)
{
	// first texture loaded do not show up (?). fulhack fix: load crap texture.
//	int crap = m_pkTextureMan->Load("/data/textures/gui/crap.bmp", 0);

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
		m_kSkins.insert( strSkin(szDefNames[i], AddSkinFromScript(szDefNames[i]) ) );
}

void ZGuiApp::InitGui(ZFScriptSystem* pkScriptSys, char* szFontTexture, 
							 char* szScriptFile, char* szMenuFile,
							 bool bUseHardwareMouse)
{
	// Spara undan viktiga pekare till system.
	m_pkGuiSys = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkTextureMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkScriptSystem = pkScriptSys;
	m_pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));

	//	m_pkTextureMan->Load("data/textures/gui/slask.bmp", 0); // första misslyckas, vet inte varför..

	ZGuiFont* pkDefaultFont = new ZGuiFont("defguifont");				// LEAK - MistServer, Nothing loaded. (FIXED)
	pkDefaultFont->Create("data/textures/text/defguifont.fnt",
				m_pkTextureMan->Load("data/textures/text/defguifont.tga"));
	m_pkResMan->Add("defguifont", pkDefaultFont);

	if(m_pkScriptResHandle)
		delete m_pkScriptResHandle;

	m_pkScriptResHandle = new ZFResourceHandle;												// LEAK - MistServer, Nothing loaded. 
	if(!m_pkScriptResHandle->SetRes(szScriptFile))
	{
		printf("Failed to load gui_create.lua\n");
		return;
	}

	InitDefaultSkins(/*pkScriptSys*/);

	ZGuiWnd* pkWnd = CreateWnd(Wnd, "GuiMainWnd", "", "", 0, 0, /*800*/GetWidth(), /*600*/GetHeight(), 0);
	ZGuiSkin* main_skin = new ZGuiSkin();
	main_skin->m_bTransparent = true;
	pkWnd->SetSkin(main_skin);

	// Låt skriptfilen skapa alla fönster.
	//pkScript->Call(m_pkScriptResHandle, "CreateMainWnds", 0, 0);

	// Create the menu for the application
	if(szMenuFile != NULL)
		CreateMenu(szMenuFile/*, pkScriptSys*/);

	// Setup cursor
	m_pkGuiSys->m_bUseHardwareMouse = bUseHardwareMouse;
	
	if(m_pkGuiSys->m_bUseHardwareMouse == false)
	{
		m_pkGuiSys->ShowCursor(true);
	}

	// Create Fps wnd

	m_pkGuiSys->m_pkFpsWnd = CreateWnd(Wnd, "zguiapp_fps_wnd", "", "", 800-60, 5, 55, 20, 0, TopRight);
	//m_pkGuiSys->m_pkFpsWnd = CreateWnd(Wnd, "zguiapp_fps_wnd", "", "", 800, 5, 55, 20, 0);
	m_pkGuiSys->m_pkFpsWnd->SetSkin(new ZGuiSkin());
	m_pkGuiSys->m_pkFpsLabel = (ZGuiLabel*) CreateWnd(Label, "zguiapp_fps_label", 
		"zguiapp_fps_wnd", "", 0, 0, 55, 20, 0);

	m_pkGuiSys->m_pkFpsLabel->SetSkin(new ZGuiSkin());
	m_pkGuiSys->ShowFPSCounter(m_pkGuiSys->m_iShowFPSCounter);
	
}

int ZGuiApp::GetTexID(char *szFile)
{
	char szPath[256];
	sprintf(szPath, "%s%s", "/data/textures/gui/", szFile);
	return m_pkTextureMan->Load(szPath, 0);
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

	bool bCombobox = GetWndType(pkWnd) == Combobox ? true : false;

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

	if(pkWnd == NULL)
	{
		printf("Failed to Add list item %s to listbox %s (no such window?)\n", 
			szText, szListboxResName);
		return;
	}

	bool bCombobox = GetWndType(pkWnd) == Combobox ? true : false;

	if(bCombobox == false)
	{
		if(GetWndType(pkWnd) != Listbox)
		{
			printf("Failed to Add list item %s to listbox %s. Wrong window type.\n", 
				szText, szListboxResName);
			return;			
		}

		ZGuiListbox* pkListBox = static_cast<ZGuiListbox*>(pkWnd);
		int iIndex = pkListBox->GetItemCount(); 
		pkListBox->AddItem(szText, iIndex, false); 
	}
	else
	{

		if(GetWndType(pkWnd) != Combobox)
		{
			printf("Failed to Add list item %s to combobox %s. Wrong window type.\n", 
				szText, szListboxResName);
			return;			
		}

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

//	bool bIsTabCtrl = (GetWndType(pkWnd) == TabControl);

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
		if(GetWndType(pkWnd) == Listbox)
		{
			ZGuiListitem* pkItem = ((ZGuiListbox*) pkWnd)->GetSelItem();

			if(pkItem)
				return pkItem->GetText();
		}
		else
		if(GetWndType(pkWnd) == Combobox)
		{
			ZGuiListitem* pkItem = ((ZGuiCombobox*) pkWnd)->GetListbox()->GetSelItem();

			if(pkItem)
				return pkItem->GetText();
		}
	}
	
	return NULL;
}

bool ZGuiApp::SelListItem(char* szWnd, char* szItem)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		if(GetWndType(pkWnd) == Listbox)
		{
			return ((ZGuiListbox*)pkWnd)->SelItem(szItem);
		}
		else
		if(GetWndType(pkWnd) == Combobox)
		{
			ZGuiListbox* pkListbox = ((ZGuiCombobox*) pkWnd)->GetListbox();
			bool bSuccess = pkListbox->SelItem(szItem);
			
			if(bSuccess)
			{
				((ZGuiCombobox*) pkWnd)->SetLabelText(szItem);
			}
			
			return bSuccess;
		}
	}
	
	return false;
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
		if(GetWndType(pkWnd) == Radiobutton)
		{
			return ((ZGuiRadiobutton*)pkWnd)->GetButton()->IsChecked();
		}
		else
		if(GetWndType(pkWnd) == Checkbox)
		{
			return ((ZGuiCheckbox*)pkWnd)->IsChecked();
		}
	}

	return false;
}

GuiType ZGuiApp::GetWndType(ZGuiWnd *pkWnd)
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
	else
	if(t==typeid(ZGuiMenu))
		return Menu;	

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

bool ZGuiApp::ChangeSkin(lua_State* pkLuaState, char* szID, 
								 char *szSkinName, char* szSkinType)
{
	ZGuiWnd* pkWnd = m_pkResMan->Wnd(string(szID));

	if(pkWnd == NULL)
		return false;

	ZGuiSkin* pkSkin = new ZGuiSkin;

	if(pkSkin)
		AddSkinFromScript2(szSkinName, /*pkScript, */pkLuaState, pkSkin);
	else
	{
		printf("Failed to change skin!\n");
		return false;
	}

	bool bSkinChanged = false;

	vector< pair<ZGuiSkin**, string> >kSkins;
	pkWnd->GetWndSkinsDesc( kSkins );

	// Gå igenom alla skins och lägg till dem om de inte redan har lagts till.
	vector< pair<ZGuiSkin**, string> >::iterator itSkin = kSkins.begin(); 
	for( ; itSkin != kSkins.end(); itSkin++)
	{
		if(strcmp(szSkinType, itSkin->second.c_str()) == 0)
		{
			(*itSkin->first) = pkSkin;
			bSkinChanged = true;
			break;
		}
	}

	if(!bSkinChanged)
		printf("Failed to change skin to %s on window %s!\n", szSkinName, szID);

	GuiType eType = GetWndType(pkWnd);

	if( eType == Button || eType == Checkbox || eType == Scrollbar)
	{
		if(strcmp(szSkinType, "Button up") == 0)
			pkWnd->SetSkin(pkSkin);
		if(strcmp(szSkinType, "Checkbox: Button up") == 0)
			pkWnd->SetSkin(pkSkin);
		if(strcmp(szSkinType, "Scrollbar: Button up") == 0)
			((ZGuiScrollbar*)pkWnd)->SetThumbButtonSkins(pkSkin, pkSkin);
		if(strcmp(szSkinType, "Scrollbar: Top: Button up") == 0)
			((ZGuiScrollbar*)pkWnd)->SetScrollButtonUpSkins(pkSkin, pkSkin);
		if(strcmp(szSkinType, "Scrollbar: Bottom: Button up") == 0)
			((ZGuiScrollbar*)pkWnd)->SetScrollButtonDownSkins(pkSkin, pkSkin);
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

	bool bCombobox = GetWndType(pkWnd) == Combobox ? true : false;

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
	return m_pkGuiSys->m_iResX;
}

int ZGuiApp::GetHeight()
{
	return m_pkGuiSys->m_iResY;
}

ZFScript* ZGuiApp::GetGuiScript()
{
	return (ZFScript*) m_pkScriptResHandle->GetResourcePtr();  
}


bool ZGuiApp::LoadGuiFromScript(char* szFileName)
{
	ZFResourceHandle kScriptResHandle;

	if(!kScriptResHandle.SetRes(szFileName))
	{
		printf("Failed to load gui resource file: %s\n", szFileName);
		return false;
	}

	m_pkScriptSystem->Call(&kScriptResHandle, "GUICreate", 0, 0);

	return true;
}

bool ZGuiApp::CreateMenu(char* szFileName)
{
	// Skapa själva menyn
	ZGuiMenu* pkMenu = (ZGuiMenu*) CreateWnd(Menu, "MainMenu", "GuiMainWnd", "", 
		0,0, 800, 20, 0, TopLeft, ResizeWidth);

	// Öppna INI filen
	ZFIni kINI;
	if(!kINI.Open(szFileName, false))
	{
		cout << "Failed to load ini file for menu!\n" << endl;
		return false;
	}

	// Hämta sektionsnamnen och spara ner i en vektor.
	vector<string> akSections;
	kINI.GetSectionNames(akSections);
	unsigned int uiNumSections = akSections.size(), i=0;
	
	// No items in file.
	if(uiNumSections < 1)
		return true;

	// Skapa alla huvudmenyer
	for(i=0; i<uiNumSections; i++)
	{
		char* opensubmenu = kINI.GetValue(akSections[i].c_str(), "OpenSubMenu");
		if(opensubmenu != NULL && strcmp(opensubmenu, "1") == 0)
		{
			char szTitle[50];
			sprintf(szTitle, "%s", kINI.GetValue(akSections[i].c_str(), "Title"));

			char* szParent = kINI.GetValue(akSections[i].c_str(), "Parent");
			if(strcmp(szParent, "NULL")==0)
				szParent = NULL;

			pkMenu->AddItem(szTitle, (char*)akSections[i].c_str(), szParent, true);
		}
	}

	// Skapa alla menyalternativ
	for(i=0; i<uiNumSections; i++)
	{
		char* parent = kINI.GetValue(akSections[i].c_str(), "OpenSubMenu");
		if(parent != NULL && strcmp(parent, "1") == 0) 
			continue;

		if(parent == NULL || strcmp(parent, "0") == 0)
		{
			char szTitle[50];
			sprintf(szTitle, "%s", kINI.GetValue(akSections[i].c_str(), "Title"));

			char* szParent = kINI.GetValue(akSections[i].c_str(), "Parent");
			if(strcmp(szParent, "NULL")==0)
				szParent = NULL;

			pkMenu->AddItem(szTitle, (char*)akSections[i].c_str(), szParent);
		}
	}

	pkMenu->ResizeMenu();

	return true;
}

bool ZGuiApp::BuildFileTree(char* szTreeBoxName, char* szRootPath, char* szExtension)
{
	ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	// kolla inparametrar
	if(szRootPath == NULL || szTreeBoxName == NULL)
		return false;

	// sista tecknet får inte vara ett '/' tecken
	if(szRootPath[strlen(szRootPath)] == '/')
		szRootPath[strlen(szRootPath)] = '\0';

	set<string> kSearchedFiles;
	list<string> dir_list;
	string strPrevNode;

	dir_list.push_back(string(szRootPath));
	strPrevNode = "RootNode";
	
	while(1)
	{
		list<string> vkFileNames;
		string currentFolder = dir_list.back();

		// Hämta filerna i den aktuella katalogen och sortera listan.
		vector<string> t;
		m_pkFileSys->ListDir(&t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);
			string id = currentFolder + string("/") + strLabel;

			bool bIsFolder = strLabel.find(".") == string::npos;

			if(kSearchedFiles.find(id) == kSearchedFiles.end())
			{			
				if(bIsFolder)
				{
					string d = currentFolder + string("/") + strLabel;
					dir_list.push_back(d);

					AddTreeItem(szTreeBoxName, id.c_str(), 
						strPrevNode.c_str(), (char*) strLabel.c_str(), 1, 2);
				}
				else
				{
					if(szExtension == NULL || id.find(szExtension) != string::npos)
						AddTreeItem(szTreeBoxName, id.c_str(), 
							strPrevNode.c_str(), (char*) strLabel.c_str(), 0, 1);
				}

				kSearchedFiles.insert(id);
			}
		}

		// ej klivit in i ett nytt dir? gå tillbaks
		if(dir_list.back() == currentFolder)
		{
			// sista?
			if(currentFolder == szRootPath)
				break;

			dir_list.pop_back();
		}

		strPrevNode = dir_list.back();
	}

	return true;

}

bool ZGuiApp::SORT_FILES::operator()(string x, string y) 
{
	//return (y.find(".") == string::npos);
	return (y > x);
}

bool ZGuiApp::ShowWnd(char* szWndResName, bool bShow)
{
	ZGuiWnd* pkWnd = GetWnd(szWndResName);

	if(pkWnd)
	{
		if(bShow)
			pkWnd->Show();
		else
			pkWnd->Hide();

		return true;
	}

	return false;
}
