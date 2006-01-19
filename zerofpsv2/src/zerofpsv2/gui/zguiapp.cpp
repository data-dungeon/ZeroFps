// guibuilder.cpp: implementation of the ZGuiApp class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiapp.h"
#include "../basic/zfassert.h"
#include "../basic/zfresourcedb.h"
//#include "../engine/res_texture.h"
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
	m_kDesignResolution.x = 800;
	m_kDesignResolution.y = 600;

	m_bGuiHaveFocus = false;
	m_uiWindowIDCounter = 1;
	m_pkScriptResHandle = NULL;
	m_oMainWndProc = oMainWndProc;
	m_szLastRadioBGroup = NULL;
   m_iScaleMode = 0;
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
	if(m_pkScriptResHandle)
		delete m_pkScriptResHandle;

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

ZGuiWnd* ZGuiApp::CreateWnd(GuiType eType, char* szResourceName, char* szText, 
									 ZGuiWnd* pkParent, int x, int y, int w, int h, int iFlags,
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

	if(m_iScaleMode == MANUALLY_SCALE)
	{
		float width_mod, height_mod;
		int iNewWidth = -1, iNewHeight = -1;

		if(eResizeType == ResizeWidth || eResizeType == Resize)
		{
			width_mod = (float) GetWidth() / (float)m_kDesignResolution.x;
			iNewWidth = (int) ((float)m_kDesignResolution.x*width_mod);
		}

		if(eResizeType == ResizeHeight || eResizeType == Resize)
		{
			height_mod = (float) GetHeight() / (float)m_kDesignResolution.y;
			iNewHeight = (int) ((float)m_kDesignResolution.y*height_mod);
		}

		//if(iNewWidth != -1) w = iNewWidth;
		//if(iNewHeight != -1) h = iNewHeight;

		if(iNewWidth != -1) w = (int) (width_mod * (float) w);
		if(iNewHeight != -1) h = (int) (height_mod * (float) h);
	}
	else
	if(m_iScaleMode == AUTO_SCALE)
	{
		float dx = x, dy = y, dw = w, dh = h;

		int iResX, iResY;
		m_pkRenderer->GetScreenSize(iResX, iResY);

		float fResModX =  (float)iResX / m_kDesignResolution.x;
		float fResModY =  (float)iResY / m_kDesignResolution.y;

		dx = x*fResModX;
		dy = y*fResModY;

		dw = w*fResModX;
		dh = h*fResModY;

		x = int(dx);
		y = int(dy);
		w = int(dw);
		h = int(dh);
	}

	// Om parent fönstret är en TabControl så är iFlags = sidnummret och parent fönstret
	// måste då tas fram på nytt.
	if( GetWndType(pkParent) == TabControl)
	{
		AddTabPage((char*)pkParent->GetName(), szResourceName, szText);
		return ((ZGuiTabCtrl*)(pkParent))->GetPage(iFlags);

/*		pkParent = ((ZGuiTabCtrl*)(pkParent))->GetPage(iFlags);

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

		if(iFlags & CREATE_NEW_GROUP)
		{
			char szRGCName[50]; 
			sprintf(szRGCName, "DefGUIRadioGroup%i", m_iLastRadioBGroup);
			CreateNewRadiobuttonGroup(szRGCName, -1); // by passing in -1 we alse raise m_iLastRadioBGroup by 1.
		}
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
		if(iFlags & READ_ONLY) 
			((ZGuiListbox*) pkWnd)->SetEnable(false);
		break;
	case Combobox:
		pkWnd = new ZGuiCombobox( Rect(x,y,x+w,y+h), pkParent, true, iID,
			COMBOBOX_ITEM_HEIGHT, NULL, NULL, NULL, GetSkin("DefCBTopItemSkin"));
		break;
	case Textbox:
		pkWnd = new ZGuiTextbox( Rect(x,y,x+w,y+h), pkParent, true, iID, 
			iFlags & EB_IS_MULTILINE);
		if(iFlags & READ_ONLY) 
			((ZGuiTextbox*) pkWnd)->SetReadOnly(true);
		break;
	case Treebox:
		pkWnd = new ZGuiTreebox( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case TabControl:
		pkWnd = new ZGuiTabCtrl( Rect(x,y,x+w,y+h), pkParent, true, iID);
		break;
	case Menu:
		pkWnd = new ZGuiMenu( Rect(x,y,x+w,y+h), pkParent, true, iID, iFlags & MENU_IS_POPUP);
		break;
	case Progressbar:
		pkWnd = new ZGuiProgressbar( Rect(x,y,x+w,y+h), pkParent, true, iID);
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
	case Progressbar:
		static_cast<ZGuiProgressbar*>(pkWnd)->SetSkin( GetSkin("DefProgressbarFrontSkin"),
			GetSkin("DefProgressbarBkgrSkin") );
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
				//WARNING!!!!!!!!!!   VALGRIND KLAGAR PÅ ATT DENNA INTE AVALOKERAS!!! /... wtf is valgrind?					
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

   pkWnd->m_iWndAlignment = eAlignment;
   pkWnd->m_iResizeType = eResizeType;

	if(szText)
	{
		pkWnd->SetText(szText);
	}

	m_kWindows.insert(map<int, ZGuiWnd*>::value_type(iID, pkWnd));

	pkWnd->SetGUI(m_pkGuiSys);
	pkWnd->SetFont(m_pkResMan->Font("defguifont"));  


//	pkWnd->Rescale(800,600, GetWidth(),GetHeight());

	pkWnd->Show(); 

	if(iFlags & CREATE_WND_HIDDEN) 
		pkWnd->Hide();

	if(m_iScaleMode == MANUALLY_SCALE)
	{
		float parent_width = m_kDesignResolution.x, parent_height = m_kDesignResolution.y;

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
				new_x = GetWidth() - (m_kDesignResolution.x-rc.Right) - rc.Width();
				new_y = GetHeight() - (m_kDesignResolution.y-rc.Bottom) - rc.Height();
				break;
			case TopRight:
				new_x = GetWidth() - (m_kDesignResolution.x-rc.Right) - rc.Width();
				new_y = rc.Top;
				break;
			case BottomLeft:
				new_x = rc.Left;
				new_y = GetHeight() - (m_kDesignResolution.y-rc.Bottom) - rc.Height();
				break;
			case CenterHorz:
				new_x = int((((rc.Left + (rc.Right-rc.Left)/2) / (float) m_kDesignResolution.x) * 
					(float) GetWidth()) - (rc.Right-rc.Left)/2); 
				new_y = rc.Top;
				break;
			case CenterVert:
				new_x = rc.Left;
				new_y = int((((rc.Top + (rc.Bottom-rc.Top)/2) / (float) m_kDesignResolution.x) * 
					(float) GetHeight()) - (rc.Bottom-rc.Top)/2); 
				break;
			case Center:
				new_x = int((((rc.Left + (rc.Right-rc.Left)/2) / (float)m_kDesignResolution.x) * 
					(float) GetWidth()) - (rc.Right-rc.Left)/2); 
				new_y = int((((rc.Top + (rc.Bottom-rc.Top)/2) / (float)m_kDesignResolution.y) * 
					(float) GetHeight()) - (rc.Bottom-rc.Top)/2); 
				break;
			}

			if(!(new_x==-1 && new_y==-1))
				pkWnd->SetPos(new_x, new_y, true, true); 
		}
	}
	//else
	//{
	//	if(pkWnd->GetSkin())
	//	{
	//		float s = GetScaleX() > GetScaleY() ? GetScaleX() : GetScaleY();
	//		pkWnd->GetSkin()->m_unBorderSize =  (float) pkWnd->GetSkin()->m_unBorderSize * s;
	//	}
	//}

	return pkWnd;
}


ZGuiWnd* ZGuiApp::CreateWnd(GuiType eType, char* szWndName, char* szParentName, 
								char* szLabel, int x, int y, int w, int h, int iFlags, 
								WndAlignent eAlignment, WndResizeType eResizeType)
{
	ZGuiWnd* pkParent = m_pkResMan->Wnd(string(szParentName)); // GetWnd(parentID);
	return CreateWnd(eType, szWndName, szLabel, pkParent, x, y, w, h, 
		iFlags, eAlignment, eResizeType);
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
		pkNewSkin->m_ucRots90Degree = (unsigned char)dData;

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

	bool bHaveAlpha = false;

	// Textures
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tex1", szData))
	{		
		string strFullName = "data/textures/gui/" + string(szData);
		map<string, string>::iterator res = m_kGuiImagePaths.find(string(szData));
		if(res != m_kGuiImagePaths.end())
			strFullName = res->second;

		pkNewSkin->m_iBkTexID = strcmp(szData, "0") != 0 ? GetTexID(szData) : -1;
		if(pkNewSkin->m_iBkTexID != -1)
		{
			m_pkGuiSys->CreatePickMapForImage(pkNewSkin->m_iBkTexID, strFullName);
			char *ext = strrchr( strFullName.c_str(), '.');
			if(ext != NULL && strcmp(ext,".tga") == 0)		
				bHaveAlpha = true;
		}
	}
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

	if(pkNewSkin->m_iBkTexAlphaID != -1 || pkNewSkin->m_iHorzBorderTexAlphaID != -1 || 
		pkNewSkin->m_iVertBorderTexAlphaID != -1 || pkNewSkin->m_iBorderCornerTexAlphaID != -1)
		bHaveAlpha = true;

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

	// Ändra storlek på ramen baserat på upplösning
	if(pkNewSkin->m_unBorderSize > 0)
	{
		float s = GetScaleX() > GetScaleY() ? GetScaleX() : GetScaleY();
		pkNewSkin->m_unBorderSize = (unsigned short) ((float) pkNewSkin->m_unBorderSize * s);
	}

	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "tile", dData))
		pkNewSkin->m_bTileBkSkin = dData > 0 ? true : false;
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "trans", dData))
		pkNewSkin->m_bTransparent = dData > 0 ? true : false;

	// Number of 90 degree rots on bk skin
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "rots90deg", dData))
		pkNewSkin->m_ucRots90Degree = (unsigned char)dData;

	// Number of 90 degree rots on bk skin
	if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "degree", dData))
	{
		float fAngle = dData;
		float fMultipel = Math::TWO_PI / 360.0f;	// PI+PI
		pkNewSkin->m_fRotDegree = fMultipel * fAngle;
	}

	// Have alpha
	pkNewSkin->m_bHaveAlpha = bHaveAlpha;

	// Spritemap rect
	//if(m_pkScriptSystem->GetGlobal(pkLuaState, szName, "rcBkTile", szData))
	//{
	//	char szFormatet[128];
	//	strcpy(szFormatet, szData);

	//	// Ta bort alla skräptecken ur datat
	//	char temp[128], c=0, l=strlen(szFormatet);
	//	for(int j=0; j<l; j++)
	//		if(szFormatet[j] != ' ')
	//			temp[c++] = szFormatet[j];
	//	temp[c] = 0;
	//	strcpy(szFormatet, temp);

	//	// Hämta x,y,w,h från textdatat
	//	char x[15] = "\0\0\0\0", y[15] = "\0\0\0\0", w[15]= "\0\0\0\0", h[15]= "\0\0\0\0";
	//	char* rect[100] = { x, y, w, h };

	//	int a=0, b=0;
	//	for(int j=0; j<strlen(szFormatet); j++)
	//		if(szFormatet[j] != ',')
	//			rect[a][b++] = szFormatet[j];
	//		else
	//		{
	//			a++;
	//			b=0;
	//		}

	//	pkNewSkin->m_rcBkTile.Left   = atoi(rect[0]);
	//	pkNewSkin->m_rcBkTile.Top    = atoi(rect[1]);
	//	pkNewSkin->m_rcBkTile.Right  = atoi(rect[2]);
	//	pkNewSkin->m_rcBkTile.Bottom = atoi(rect[3]);
	//}

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
		"DefProgressbarFrontSkin",	   "DefProgressbarBkgrSkin",
	};

	for(int i=0; i<sizeof(szDefNames)/sizeof(szDefNames[0]); i++) 
		m_kSkins.insert( strSkin(szDefNames[i], AddSkinFromScript(szDefNames[i]) ) );
}

// Måste anropas före allt annat!
void ZGuiApp::InitGui(ZSSScriptSystem* pkScriptSys, char* szFontName, 
							 char* szScriptFile, char* szMenuFile,
							 bool bUseHardwareMouse, int iScaleMode)
{
	// Spara undan viktiga pekare till system.
	m_pkGuiSys = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkTextureMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkScriptSystem = pkScriptSys;
	m_pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));

	FindGuiTextureByResolution();

	if(iScaleMode == -1)
		m_iScaleMode = m_pkGuiSys->m_kiScaleMode.GetInt(); // Sätt till scale mode i ZGui eftersom den får sin data från INI fil.
	else
		m_iScaleMode = iScaleMode;

	//	m_pkTextureMan->Load("data/textures/gui/slask.bmp", 0); // första misslyckas, vet inte varför..

	//ZGuiFont* pkDefaultFont = new ZGuiFont("defguifont");				// LEAK - MistServer, Nothing loaded. (FIXED)
	//pkDefaultFont->Create("data/textures/text/defguifont.fnt",
	//			m_pkTextureMan->Load("data/textures/text/defguifont.tga"));
	//m_pkResMan->Add("defguifont", pkDefaultFont);
	ZGuiFont* pkDefaultFont = new ZGuiFont("defguifont");				// LEAK - MistServer, Nothing loaded. (FIXED)

   char szFontData[512], szFontTex[512];
   sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", szFontName);
   sprintf(szFontTex, "gui/fonts/%s.tga", szFontName);
	pkDefaultFont->Create(szFontData, m_pkTextureMan->Load(szFontTex));
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
		CreateMenu(szMenuFile, "MainMenu", false);

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
	m_pkGuiSys->ShowFPSCounter(m_pkGuiSys->m_kiShowFPSCounter.GetBool());

	m_pkGuiSys->Activate(true);


	
}

int ZGuiApp::GetTexID(char *szFile)
{
	//char szPath[256];
	//sprintf(szPath, "%s%s", "/data/textures/gui/", szFile);
	//return m_pkTextureMan->Load(szPath, 0);
	return LoadGuiTextureByRes(szFile);
}


bool ZGuiApp::CreateNewRadiobuttonGroup(const char *szName, int id)
{
	if(m_szLastRadioBGroup)
		delete[] m_szLastRadioBGroup;

	m_szLastRadioBGroup = new char[strlen(szName)+1];
	strcpy(m_szLastRadioBGroup, szName);

	if(id!=-1)
		m_iLastRadioBGroup = id;	
	else
		m_iLastRadioBGroup++;


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

void ZGuiApp::AddListItem(char *szListboxResName, char *szText, bool bSelect)
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
		pkListBox->AddItem(szText, iIndex, bSelect);  
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
		pkComboBox->AddItem(szText, -1, bSelect); 
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

bool ZGuiApp::SelListItemByIndex(char* szWnd, int iIndex)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		if(GetWndType(pkWnd) == Listbox)
		{
			return ((ZGuiListbox*)pkWnd)->SelItem(iIndex);
		}
		else
		if(GetWndType(pkWnd) == Combobox)
		{
			ZGuiListbox* pkListbox = ((ZGuiCombobox*) pkWnd)->GetListbox();
			bool bSuccess = pkListbox->SelItem(iIndex);
			
			if(bSuccess)
			{
				((ZGuiCombobox*) pkWnd)->SetLabelText(pkListbox->GetItem(iIndex)->GetText());
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

void ZGuiApp::CheckButton(const char* szWnd, bool bCheck)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = m_pkResMan->Wnd(szWnd)))
	{
		if(GetWndType(pkWnd) == Radiobutton)
		{
			if(bCheck)
				((ZGuiRadiobutton*)pkWnd)->Check(); 
			//return ((ZGuiRadiobutton*)pkWnd)->GetButton()->IsChecked();
		}
		else
		if(GetWndType(pkWnd) == Checkbox)
		{
			if(bCheck)
				((ZGuiCheckbox*)pkWnd)->CheckButton();
			else
				((ZGuiCheckbox*)pkWnd)->UncheckButton();
		}
	}
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
	else
	if(t==typeid(ZGuiProgressbar))
		return Progressbar;	

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

	if( eType == Button || eType == Checkbox || eType == Scrollbar || eType == Slider ||
		 eType == Treebox)
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

		if(strcmp(szSkinType, "Slider: Button up") == 0)
			((ZGuiSlider*)pkWnd)->GetButton()->SetSkin(pkSkin);

		if(strcmp(szSkinType, "Treebox: H.Scrollbar: Bottom: Button up") == 0)
			((ZGuiTreebox*)pkWnd)->GetScrollbar(false)->GetArrowButton(false)->SetButtonUpSkin(pkSkin);
		if(strcmp(szSkinType, "Treebox: H.Scrollbar: Top: Button up") == 0)
			((ZGuiTreebox*)pkWnd)->GetScrollbar(false)->GetArrowButton(true)->SetButtonUpSkin(pkSkin);


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

	if(pkWnd == NULL)
		return;

	GuiType eType = GetWndType(pkWnd);

	if(eType == Combobox)
	{
		ZGuiCombobox* pkComboBox = static_cast<ZGuiCombobox*>(pkWnd);
		pkComboBox->RemoveAllItems();
	}
	else
	if(eType == Listbox)
	{
		ZGuiListbox* pkListBox = static_cast<ZGuiListbox*>(pkWnd);
		pkListBox->RemoveAllItems();
	}
	else
	if(eType == Treebox)
	{
		ZGuiTreebox* pkTreebox = static_cast<ZGuiTreebox*>(pkWnd);
		pkTreebox->Clear();
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

int ZGuiApp::LoadGuiTextureByRes(string strFile)
{
	string strFullName = "gui/" + strFile;

	map<string, string>::iterator res = m_kGuiImagePaths.find(strFile);
	if(res != m_kGuiImagePaths.end())
		strFullName = res->second;

	//printf("Found tex: %s\n", strFullName.c_str());

	return m_pkTextureMan->Load(strFullName.c_str(), 0);
}

bool ZGuiApp::LoadGuiFromScript(const char* szFileName)
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

bool ZGuiApp::CreateMenu(char* szFileName, char* szName, bool bPopup)
{
   int iMenuHeight;

   if(m_pkResMan->Font("defguifont"))
      iMenuHeight = m_pkResMan->Font("defguifont")->m_iRowHeight + 2;
   else
      iMenuHeight = 20;

	// Skapa själva menyn
	int iFlags = bPopup ? MENU_IS_POPUP : 0;
	ZGuiMenu* pkMenu = (ZGuiMenu*) CreateWnd(Menu, szName, "GuiMainWnd", "", 
		0, 0, GetWidth(), iMenuHeight, iFlags, TopLeft, ResizeWidth);

	pkMenu->ClearAll();

	// Öppna INI filen
	ZFIni kINI;
	if(!kINI.Open(szFileName, false))
	{
		cerr << "Warning: Failed to load ini file for menu!\n" << endl;
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
		if( strcmp( akSections[i].c_str(), "Menu_Edit_ObjectRotation_Mode_RotY") == 0)
		{
			vector<string> seklist;
			kINI.GetKeyNames( akSections[i].c_str(), seklist);

			for(int i=0; i<seklist.size(); i++)
				printf("seklist[i] = %s\n", seklist[i].c_str());

		}
		
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

			char* val;
			if((val=kINI.GetValue(akSections[i].c_str(), "UseCheckMark")))
			{
				if(strcmp(val,"1") == 0)
					pkMenu->UseCheckMark((char*)akSections[i].c_str(), true);
			}
			if((val=kINI.GetValue(akSections[i].c_str(), "SetCheckMark")))
			{
				if(strcmp(val,"1") == 0)
					pkMenu->SetCheckMark((char*)akSections[i].c_str(), true);
			}
			if((val=kINI.GetValue(akSections[i].c_str(), "CheckMarkGroup")))
			{
				pkMenu->SetCheckMarkGroup((char*)akSections[i].c_str(), atoi(val));
			}
				
		}
	}

	pkMenu->ResizeMenu();

	return true;
}

bool ZGuiApp::BuildFileTree(char* szTreeBoxName, char* szRootPath, char* szExtension)
{
	ZSSVFileSystem* pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

	// kolla inparametrar
	if(szRootPath == NULL || szTreeBoxName == NULL)
		return false;

	if(GetWnd(szTreeBoxName) == NULL)
		return false;

	if(GetWndType(GetWnd(szTreeBoxName)) != Treebox)
		return false;

	// Clear treebox
	((ZGuiTreebox*)GetWnd(szTreeBoxName))->Clear(); 

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
		pkFileSys->ListDir(t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);

			if(strLabel == "CVS")
				continue;

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

bool ZGuiApp::ShowWnd(char* szWndResName, bool bShow, bool bPlaceFront, bool bSetCapture)
{
	ZGuiWnd* pkWnd = GetWnd(szWndResName);

	if(pkWnd)
	{
		if(bShow)
      {
			pkWnd->Show();
         if(bPlaceFront) {m_pkGuiSys->PlaceWndFrontBack(pkWnd, true); m_pkGuiSys->SetFocus(pkWnd); }
         if(bSetCapture) m_pkGuiSys->SetCaptureToWnd(pkWnd);
      }
		else
      {
         if(m_pkGuiSys->GetWndCapture() == pkWnd)
            m_pkGuiSys->KillWndCapture();

			pkWnd->Hide();
      }

		return true;
	}

	return false;
}


bool ZGuiApp::SetFont(string strWnd, string strFont, int r, int g, int b, int glyph)
{
	ZGuiFont* pkFont = m_pkResMan->Font(strFont);

	if(pkFont == NULL)
	{
		char szFontData[512], szFontTex[512];
		sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", strFont.c_str());
		sprintf(szFontTex, "gui/fonts/%s.tga", strFont.c_str());

		pkFont = new ZGuiFont((char*)strFont.c_str());
		if(pkFont->Create(szFontData, m_pkTextureMan->Load(szFontTex, 0), glyph))
			m_pkResMan->Add(strFont, pkFont);	
		else
			return false;
	}

	ZGuiWnd* pkWnd = GetWnd(strWnd);

	if(pkWnd == NULL)
		return false;

	pkWnd->SetFont(pkFont); 
	pkWnd->SetTextColor(r,g,b);

	return true;	
}

float ZGuiApp::GetScaleX()
{
	if(m_iScaleMode == MANUALLY_SCALE || m_iScaleMode == DISABLE_SCALE)
		return 1;

	return ( (float) GetWidth() / (float) m_kDesignResolution.x );
}

float ZGuiApp::GetScaleY()
{
	if(m_iScaleMode == MANUALLY_SCALE || m_iScaleMode == DISABLE_SCALE)
		return 1;

	return ( (float) GetHeight() / (float) m_kDesignResolution.y );
}

void ZGuiApp::MsgBox(char* text, char* caption, int type)
{
	ZGuiWnd* pkMsgBox = GetWnd("GuiDefMsgBox");
	
	if(pkMsgBox == NULL)
	{
		const Point mrg(4,4);
		int w = 300, h = 150;

		pkMsgBox = CreateWnd(Wnd, "GuiDefMsgBox", "", "", 0,0,w,h,0);
		ZGuiLabel* pkCaption = (ZGuiLabel*) CreateWnd(Label, "DefMsgBoxCaption", "MessageBox", pkMsgBox, mrg.x,mrg.y,w-mrg.x*2,16,0);
		ZGuiLabel* pkText = (ZGuiLabel*) CreateWnd(Label, "DefMsgBoxText", "Press Me", pkMsgBox, mrg.x,20+mrg.y,w-mrg.x*2,h-20-mrg.y*2-30,0);

		ZGuiButton* pkYes = (ZGuiButton*) CreateWnd(Button, "DefMsgBoxYesBn", "Yes", pkMsgBox, w/2-40/2,h-25,40,20,0);
		ZGuiButton* pkNo = (ZGuiButton*) CreateWnd(Button, "DefMsgBoxNoBn", "No", pkMsgBox, w/2-40/2,h-25,40,20,0);
		
		pkText->m_bMultiLine = true;

		pkYes->Hide();
		pkNo->Hide();

		ZGuiSkin* pkBkSkin = new ZGuiSkin();
		pkBkSkin->m_afBkColor[0] = 0.82f;
		pkBkSkin->m_afBkColor[1] = 0.82f;
		pkBkSkin->m_afBkColor[2] = 0.82f;

		pkBkSkin->m_afBorderColor[0] = 0;
		pkBkSkin->m_afBorderColor[1] = 0;
		pkBkSkin->m_afBorderColor[2] = 0;

		pkBkSkin->m_unBorderSize = 2;
		pkMsgBox->SetSkin(pkBkSkin);

		ZGuiSkin* pkCaptionSkin = new ZGuiSkin();
		pkCaptionSkin->m_afBkColor[0] = 0.2f;
		pkCaptionSkin->m_afBkColor[1] = 0.2f;
		pkCaptionSkin->m_afBkColor[2] = 1.0f;
		pkCaptionSkin->m_afBorderColor[0] = 0;
		pkCaptionSkin->m_afBorderColor[1] = 0;
		pkCaptionSkin->m_afBorderColor[2] = 0;
		pkCaptionSkin->m_unBorderSize = 1;
		pkCaption->SetSkin(pkCaptionSkin);
		pkCaption->SetTextColor(255,255,255); 

		ZGuiSkin* pkTextSkin = new ZGuiSkin();
		pkTextSkin->m_bTransparent = true;
		pkTextSkin->m_afBorderColor[0] = 0;
		pkTextSkin->m_afBorderColor[1] = 0;
		pkTextSkin->m_afBorderColor[2] = 0;
		pkTextSkin->m_unBorderSize = 1;

		pkText->SetSkin(pkTextSkin);
		pkText->SetTextColor(0,0,0); 
	}

	Rect rcMsgBox = pkMsgBox->GetScreenRect();

	pkMsgBox->Show();
	pkMsgBox->SetPos(GetWidth()/2-rcMsgBox.Width()/2,GetHeight()/2-rcMsgBox.Height()/2,true,true);

	SetText("DefMsgBoxCaption", caption);
	SetText("DefMsgBoxText", text);

	pkMsgBox->SetMoveArea(Rect(0,0,GetWidth(),GetHeight()),true);
	m_pkGuiSys->SetFocus(pkMsgBox, true);
	m_pkGuiSys->PlaceWndFrontBack(pkMsgBox, true);
	m_pkGuiSys->SetCaptureToWnd(pkMsgBox);

	ZGuiButton* pkYes = (ZGuiButton*) GetWnd("DefMsgBoxYesBn");
	ZGuiButton* pkNo = (ZGuiButton*) GetWnd("DefMsgBoxNoBn");

	if(type == MSGBOX_OK)
	{
		pkNo->Hide();
		pkYes->Show();

		pkYes->SetPos(pkMsgBox->GetScreenRect().Width()/2-40/2,pkMsgBox->GetScreenRect().Height()-25,false,true);
		pkYes->SetText("OK"); 
	}
	else
	if(type == MSGBOX_YESNO)
	{
		pkNo->Show();
		pkYes->Show();

		pkYes->SetPos(rcMsgBox.Width()/2-40/2-45/2,rcMsgBox.Height()-25,false,true);
		pkNo->SetPos(rcMsgBox.Width()/2-40/2+45/2,rcMsgBox.Height()-25,false,true);

		pkYes->SetText("Yes");
		pkNo->SetText("No");
	}
	
}


void ZGuiApp::FindGuiTextureByResolution()
{
	vector<string> kTexMap_640x480;
	vector<string> kTexMap_800x600;
	vector<string> kTexMap_1024x768;
	vector<string> kTexMap_1280x1024;
	vector<string> kTexMap_1600x1200;

	vector<string> vkExtension;
	vkExtension.push_back(".bmp");
	vkExtension.push_back(".tga");

	ZSSVFileSystem* pkFileSys = reinterpret_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

	pkFileSys->ListDirRecursive(kTexMap_640x480, string("data/textures/gui/640x480/"), vkExtension);
	pkFileSys->ListDirRecursive(kTexMap_800x600, string("data/textures/gui/800x600/"), vkExtension);
	pkFileSys->ListDirRecursive(kTexMap_1024x768, string("data/textures/gui/1024x768/"), vkExtension);
	pkFileSys->ListDirRecursive(kTexMap_1280x1024, string("data/textures/gui/1280x1024/"), vkExtension);
	pkFileSys->ListDirRecursive(kTexMap_1600x1200, string("data/textures/gui/1600x1200/"), vkExtension);

	struct TEST
	{
		vector<string>* t;
		string strResFolder;
		int res;
	};

	TEST tt[5] = 
	{
		{&kTexMap_640x480, "gui/640x480/", 640*480 },
		{&kTexMap_800x600, "gui/800x600/", 800*600 },
		{&kTexMap_1024x768, "gui/1024x768/", 1024*768 }, 
		{&kTexMap_1280x1024, "gui/1280x1024/", 1280*1024 }, 
		{&kTexMap_1600x1200, "gui/1600x1200/", 1600*1200 }
	};

	int iCurrentRes = GetWidth() * GetHeight();
	int iBestValue = 99999;

	map<string, int> kFileResValMap;

	for(int t=0; t<5; t++)
	{
		for(int i=0; i<tt[t].t->size(); i++)
		{
			string strFile = (*tt[t].t)[i];
			
			map<string, string>::iterator res = m_kGuiImagePaths.find(strFile);

			if(res == m_kGuiImagePaths.end()) // bilden fanns inte med alls, lägg till den.
			{
				m_kGuiImagePaths.insert( map<string, string>::value_type(strFile, tt[t].strResFolder+strFile) );	
				
				int iValue = abs(iCurrentRes - tt[t].res);
				kFileResValMap.insert( map<string, int>::value_type(strFile, iValue) );	
			}
			else
			// Bilden fanns redan i mappen.
			{
				// Hur nära den riktiga upplösningen är denna bilden? Destå lägre destå bättre.
				int iValue = abs(iCurrentRes - tt[t].res);

				int best_value;

				// Plocka fram den bästa bilden ur värde mappen och jämför.
				map<string, int>::iterator res2 = kFileResValMap.find(strFile);
				if(res2 == kFileResValMap.end()) // finns ej, sätt bästa värdet till nått högt.
				{
					best_value = 999999;
				}
				else 
				{					
					best_value = res2->second;
				}

				if( iValue < best_value )
				{
					if(res2 != kFileResValMap.end())
						kFileResValMap.erase(res2);

					kFileResValMap.insert( map<string, int>::value_type(strFile, iValue) );	

					// Ta bort den gamla sökvägen
					m_kGuiImagePaths.erase(res);

					// Lägg till den nya som är bättre.
					m_kGuiImagePaths.insert( map<string, string>::value_type(strFile, tt[t].strResFolder+strFile) );	
				}
			}
		}
	}
}






