// guibuilder.cpp: implementation of the GuiApp class.
//
//////////////////////////////////////////////////////////////////////

#include "guiapp.h"
#include "../zerofpsv2/basic/zfassert.h"
#include "../zerofpsv2/engine/res_texture.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/script/zfscript.h"
#include <typeinfo>
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuiApp::GuiApp(ZGui::callback oMainWndProc)
{
	m_oMainWndProc = oMainWndProc;
	m_szLastRadioBGroup = NULL;
	CreateNewRadiobuttonGroup("DefGUIRadioGroup", 1);
}

GuiApp::~GuiApp()
{
	map<string, ZGuiSkin*>::iterator itSkins = m_kSkins.begin();
	while(itSkins != m_kSkins.end())
	{
		delete itSkins->second;
		itSkins++;
	}

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

ZGuiWnd* GuiApp::GetWnd(int iID)
{
	map<int, ZGuiWnd*>::iterator res = m_kWindows.find(iID);
	if(res == m_kWindows.end())
		return NULL;
	return res->second;
}

ZGuiSkin* GuiApp::GetSkin(string strName)
{
	map<string, ZGuiSkin*>::iterator res = m_kSkins.find(strName);
	if(res == m_kSkins.end())
		return NULL;
	return res->second;
}

bool GuiApp::CreateWnd(GuiType eType, char* szResourceName, char* szText, int iID, 
					   ZGuiWnd* pkParent, int x, int y, int w, int h, unsigned long uiFlags)
{
	ZFAssert(GetWnd(iID) == NULL, "GuiApp::CreateWnd: WindowID already exist"); 
	ZGuiWnd* pkWnd;

	const int LISTBOX_ITEM_HEIGHT = 20;
	const int COMBOBOX_ITEM_HEIGHT = 20;
	
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
		break;
	case Combobox:
		pkWnd = new ZGuiCombobox( Rect(x,y,x+w,y+h), pkParent, true, iID,
			COMBOBOX_ITEM_HEIGHT, NULL, NULL, NULL, GetSkin("DefCBTopItemSkin"));
		break;
	case Textbox:
		pkWnd = new ZGuiTextbox( Rect(x,y,x+w,y+h), pkParent, true, iID, 
			uiFlags & EB_IS_MULTILINE);
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
		break;
	case Slider:
		static_cast<ZGuiSlider*>(pkWnd)->SetSliderSkin(GetSkin("DefSliderSkin"));
		static_cast<ZGuiSlider*>(pkWnd)->SetBkSkin(GetSkin("DefSliderBkSkin"));
		break;
	case Listbox:
		static_cast<ZGuiListbox*>(pkWnd)->SetSkin( GetSkin("DefLBBkSkin") );
		static_cast<ZGuiListbox*>(pkWnd)->SetItemNormalSkin(   GetSkin("DefLBitemUSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetItemSelectedSkin( GetSkin("DefLBitemDSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetItemHighLightSkin(GetSkin("DefLBitemFSkin"));
		static_cast<ZGuiListbox*>(pkWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin") );
		break;
	case Combobox:
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetSkin( GetSkin("DefCBBkSkin") );
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetItemNormalSkin(   GetSkin("DefCBitemUSkin"));
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetItemSelectedSkin( GetSkin("DefCBitemDSkin"));
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetItemHighLightSkin(GetSkin("DefCBitemFSkin"));
		static_cast<ZGuiCombobox*>(pkWnd)->GetListbox()->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin") );
		break;
	case Textbox:
		static_cast<ZGuiTextbox*>(pkWnd)->SetSkin(GetSkin("DefTextboxSkin"));
		static_cast<ZGuiTextbox*>(pkWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin") );
		break;
	case Treebox:
		static_cast<ZGuiTreebox*>(pkWnd)->SetScrollbarSkin(GetSkin("DefSBrBkSkin"),
			GetSkin("DefSBrNSkin"), GetSkin("DefSBrFSkin") );
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

	if(eType == Wnd)
	{
		if(!m_pkGui->AddMainWindow(iID, pkWnd, szResourceName, m_oMainWndProc, true))
			return false;
	}

	if(szText)
	{
		pkWnd->SetText(szText);
	}

	m_kWindows.insert(map<int, ZGuiWnd*>::value_type(iID, pkWnd));

	pkWnd->SetGUI(m_pkGui);
	pkWnd->SetFont(m_pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT));  
	
	return true;
}

bool GuiApp::CreateWnd(GuiType eType, char* szResourceName, char* szText, int iID, 
					   int parentID, int x, int y, int w, int h, unsigned long uiFlags)
{
	ZGuiWnd* pkParent = GetWnd(parentID);

	return CreateWnd(eType, szResourceName, szText, iID, 
		pkParent, x, y, w, h, uiFlags);
}

char* GuiApp::GetTexName(ZFScript* pkScript, char* szName)
{
	if(!pkScript->GetGlobal(NULL, szName, m_szTexName))
		strcpy(m_szTexName, "\0");

	return m_szTexName;
}

void GuiApp::InitTextures(ZFScript* pkScript)
{
	// first texture loaded do not show up (?). fulhack fix: load crap texture.
	int crap = m_pkTexMan->Load("/data/textures/gui/crap.bmp", 0);

	typedef map<string, ZGuiSkin*>::value_type strSkin;

	ZGuiSkin* skin;

	m_kSkins.insert(strSkin("DefWndSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefWndSkin")),0)));
	m_kSkins.insert(strSkin("DefBnUSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefBnUSkin")),0)));
	m_kSkins.insert(strSkin("DefBnDSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefBnDSkin")),0)));
	m_kSkins.insert(strSkin("DefBnFSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefBnFSkin")),0)));
	m_kSkins.insert(strSkin("DefRBnUSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefRBnUSkin")),GetTexID("rbn_a.bmp"),0)));
	m_kSkins.insert(strSkin("DefRBnDSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefRBnDSkin")),GetTexID("rbn_a.bmp"),0)));
	m_kSkins.insert(strSkin("DefCBnUSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefCBnUSkin")),0)));
	m_kSkins.insert(strSkin("DefCBnDSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefCBnDSkin")),0)));
	m_kSkins.insert(strSkin("DefSBrNSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefSBrNSkin")),0)));
	m_kSkins.insert(strSkin("DefSBrFSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefSBrFSkin")),0)));
	m_kSkins.insert(strSkin("DefSBrBkSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefSBrBkSkin")),1)));
	m_kSkins.insert(strSkin("DefSliderSkin", new ZGuiSkin(GetTexID(GetTexName(pkScript, "DefSliderSkin")),
		GetTexID("slider_a.bmp"), 0)));
	m_kSkins.insert(strSkin("DefSliderBkSkin", new ZGuiSkin(255,0,0,  0,0,255, 0)));

	m_kSkins.insert(strSkin("DefLBitemUSkin", new ZGuiSkin(GetTexID("lb_u.bmp"),0)));
	m_kSkins.insert(strSkin("DefLBitemDSkin", new ZGuiSkin(GetTexID("lb_d.bmp"),0)));
	
	skin = new ZGuiSkin(GetTexID("lb_f.bmp"),1);
	skin->m_afBorderColor[0] = 0.22f; skin->m_afBorderColor[1] = 0.22f;
	skin->m_afBorderColor[2] = 0.22f; skin->m_unBorderSize = 1;
	m_kSkins.insert(strSkin("DefLBitemFSkin", skin));

	skin = new ZGuiSkin(GetTexID("lb_bk.bmp"),1);
	skin->m_afBorderColor[0] = 0.22f; skin->m_afBorderColor[1] = 0.22f;
	skin->m_afBorderColor[2] = 0.22f; skin->m_unBorderSize = 1;
	m_kSkins.insert(strSkin("DefLBBkSkin", skin));

	m_kSkins.insert(strSkin("DefCBitemUSkin", new ZGuiSkin(GetTexID("cb_u.bmp"),0)));
	m_kSkins.insert(strSkin("DefCBitemDSkin", new ZGuiSkin(GetTexID("cb_d.bmp"),0)));
	
	skin = new ZGuiSkin(GetTexID("cb_f.bmp"),1);
	skin->m_afBorderColor[0] = 0.22f; skin->m_afBorderColor[1] = 0.22f;
	skin->m_afBorderColor[2] = 0.22f; skin->m_unBorderSize = 1;
	m_kSkins.insert(strSkin("DefCBitemFSkin", skin));

	skin = new ZGuiSkin(GetTexID("cb_bk.bmp"),1);
	skin->m_afBorderColor[0] = 0.22f; skin->m_afBorderColor[1] = 0.22f;
	skin->m_afBorderColor[2] = 0.22f; skin->m_unBorderSize = 1;
	m_kSkins.insert(strSkin("DefCBBkSkin", skin));

	m_kSkins.insert(strSkin("DefCBTopItemSkin", new ZGuiSkin(GetTexID("cb_topItem.bmp"),1)));

	skin = new ZGuiSkin(GetTexID("textbox.bmp"),1);
	skin->m_afBorderColor[0] = 0.22f; skin->m_afBorderColor[1] = 0.22f;
	skin->m_afBorderColor[2] = 0.22f; skin->m_unBorderSize = 1;
	m_kSkins.insert(strSkin("DefTextboxSkin", skin));

	skin = new ZGuiSkin(GetTexID("treebox_bk.bmp"),1);
	skin->m_afBorderColor[0] = 0.22f; skin->m_afBorderColor[1] = 0.22f;
	skin->m_afBorderColor[2] = 0.22f; skin->m_unBorderSize = 1;
	m_kSkins.insert(strSkin("DefTreeboxBkSkin", skin));

	m_kSkins.insert(strSkin("DefTreeNodeChildSkin", new ZGuiSkin(GetTexID("tn_c.bmp"),0)));
	m_kSkins.insert(strSkin("DefTreeNodeParentClosedSkin", new ZGuiSkin(GetTexID("tn_pc.bmp"),0)));
	m_kSkins.insert(strSkin("DefTreeNodeParentOpenSkin", new ZGuiSkin(GetTexID("tn_po.bmp"),0)));

	m_kSkins.insert(strSkin("DefTabCtrlBkSkin", new ZGuiSkin(128, 128, 128, 92, 92, 92, 1)));

	m_kSkins.insert(strSkin("DefTabBnPrevUSkin", new ZGuiSkin(GetTexID("tab_bnPrev_u.bmp"),0)));
	m_kSkins.insert(strSkin("DefTabBnPrevDSkin", new ZGuiSkin(GetTexID("tab_bnPrev_d.bmp"),0)));

	m_kSkins.insert(strSkin("DefTabBnNextUSkin", new ZGuiSkin(GetTexID("tab_bnNext_u.bmp"),0)));
	m_kSkins.insert(strSkin("DefTabBnNextDSkin", new ZGuiSkin(GetTexID("tab_bnNext_d.bmp"),0)));

	m_kSkins.insert(strSkin("DefTabPageBackSkin", new ZGuiSkin(128,128,128,92,92,92,1)));
	m_kSkins.insert(strSkin("DefTabPageFrontSkin", new ZGuiSkin(214,211,206,0,0,0,0)));
}

void GuiApp::InitializeGui(ZGui* pkGui, TextureManager* pkTexMan, ZFScript* pkScript)
{
	// Start skript filen för GUI:t.
	// Läs in tex filnamn på textuerer osv.
	pkScript->RunScript("gui_script.lua");

	m_pkGui = pkGui;
	m_pkTexMan = pkTexMan;

	InitTextures(pkScript);

	m_pkGui->SetCursor(0,0, m_pkTexMan->Load("/data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/cursor_a.bmp", 0), 32, 32);
	SDL_ShowCursor(SDL_DISABLE);

	// Låt skriptfilen skapa alla fönster.
	pkScript->CallScript("InitGUI", 0, 0);
}

int GuiApp::GetTexID(char *szFile)
{
	char szPath[256];
	sprintf(szPath, "%s%s", "/data/textures/gui/", szFile);
	return m_pkTexMan->Load(szPath, 0);
}


bool GuiApp::CreateNewRadiobuttonGroup(const char *szName, int id)
{
	if(m_szLastRadioBGroup)
		delete[] m_szLastRadioBGroup;

	m_szLastRadioBGroup = new char[strlen(szName)+1];
	strcpy(m_szLastRadioBGroup, szName);

	m_iLastRadioBGroup = id;

	return true;
}

void GuiApp::AddListItem(int iListboxID, char *szText, bool bCombobox)
{
	if(bCombobox == false)
	{
		ZGuiListbox* pkListBox = static_cast<ZGuiListbox*>(GetWnd(iListboxID));
		int iIndex = pkListBox->GetItemCount(); 
		pkListBox->AddItem(szText, iIndex, false); 
	}
	else
	{
		ZGuiCombobox* pkComboBox = static_cast<ZGuiCombobox*>(GetWnd(iListboxID));
		pkComboBox->AddItem(szText, -1, false); 
	}

}

void GuiApp::AddTreeItem(int iTreeboxID, const char* szID, const char* szIDParent, char* szText,
						 unsigned char iNodeSkinNormal, unsigned char iNodeSkinSelected)
{
	ZGuiTreebox* pkTreeBox = static_cast<ZGuiTreebox*>(GetWnd(iTreeboxID));

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

void GuiApp::AddTabPage(int iTabCtrlID, char* szName)
{
	ZGuiTabCtrl* pkTreeBox = static_cast<ZGuiTabCtrl*>(GetWnd(iTabCtrlID)); 
	pkTreeBox->InsertPage(pkTreeBox->GetNumPages(), szName, 0);
	pkTreeBox->SetCurrentPage(0); 
}

ZGuiWnd* GuiApp::GetTabPage(int iTabCtrlID, int iPage)
{
	ZGuiWnd* pkWnd = GetWnd(iTabCtrlID);

	bool bIsTabCtrl = (GetType(pkWnd) == TabControl);

	ZFAssert(pkWnd, "GuiApp::GetTabPage: Window is not a tab control!");

	return static_cast<ZGuiTabCtrl*>(pkWnd)->GetPage(iPage);	
}

void GuiApp::SetText(int iWndID, char* szText, bool bResize)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = GetWnd(iWndID)))
		pkWnd->SetText(szText, bResize);
}

void GuiApp::SetTextInt(int iWndID, int iNumber, bool bResize)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = GetWnd(iWndID)))
	{
		char szText[25];
		sprintf(szText, "%i", iNumber);
		pkWnd->SetText(szText, bResize);
	}
}

void GuiApp::SetTextFloat(int iWndID, float fNumber, bool bResize)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = GetWnd(iWndID)))
	{
		char szText[25];
		sprintf(szText, "%f", fNumber);
		pkWnd->SetText(szText, bResize);
	}
}

char* GuiApp::GetText(int iWndID)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = GetWnd(iWndID)))
		return pkWnd->GetText(); 	
	
	return NULL;
}

int GuiApp::GetTextInt(int iWndID, bool* pkSuccess)
{
	ZGuiWnd* pkWnd;

	if(pkSuccess)
		*pkSuccess = false;

	if((pkWnd = GetWnd(iWndID)))
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

float GuiApp::GetTextFloat(int iWndID, bool *pkSuccess)
{
	if(pkSuccess)
		*pkSuccess = false;

	ZGuiWnd* pkWnd;
	if((pkWnd = GetWnd(iWndID)))
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

bool GuiApp::IsButtonChecked(int iWndID)
{
	ZGuiWnd* pkWnd;
	if((pkWnd = GetWnd(iWndID)))
	{
		if(GetType(pkWnd) == Radiobutton)
			return true;
	}

	return false;
}

GuiType GuiApp::GetType(ZGuiWnd *pkWnd)
{
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
