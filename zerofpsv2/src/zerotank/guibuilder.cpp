// guibuilder.cpp: implementation of the GuiBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "guibuilder.h"
#include "../zerofpsv2/basic/zfassert.h"
#include "../zerofpsv2/engine/res_texture.h"
#include "../zerofpsv2/render/texturemanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuiBuilder::GuiBuilder(ZGui* pkGui, TextureManager* pkTexMan,
					   ZGui::callback oMainWndProc)
{
	m_pkGui = pkGui;
	m_oMainWndProc = oMainWndProc;
	m_pkTexMan = pkTexMan;

	m_szLastRadioBGroup = NULL;
	CreateNewRadiobuttonGroup("DefGUIRadioGroup", 1);
}

GuiBuilder::~GuiBuilder()
{

}

ZGuiWnd* GuiBuilder::GetWnd(int iID)
{
	map<int, ZGuiWnd*>::iterator res = m_kWindows.find(iID);
	if(res == m_kWindows.end())
		return NULL;
	return res->second;
}

ZGuiSkin* GuiBuilder::GetSkin(string strName)
{
	map<string, ZGuiSkin*>::iterator res = m_kSkins.find(strName);
	if(res == m_kSkins.end())
		return NULL;
	return res->second;
}

bool GuiBuilder::Create(GuiType eType, char* szResourceName, char* szText, int iID, 
						int parentID, int x, int y, int w, int h, unsigned long uiFlags)
{
	ZFAssert(GetWnd(iID) == NULL, "GuiBuilder::CreateWnd: WindowID already exist"); 
	ZGuiWnd* pkWnd;
	
	switch(eType)
	{
	case Wnd:
		pkWnd = new ZGuiWnd( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Button:
		pkWnd = new ZGuiButton( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Label:
		pkWnd = new ZGuiLabel( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Radiobutton:
		pkWnd = new ZGuiRadiobutton( Rect(x,y,x+w,y+h), GetWnd(parentID), iID, 
			m_iLastRadioBGroup, m_szLastRadioBGroup, NULL, true);
		break;
	case Checkbox:
		pkWnd = new ZGuiCheckbox( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Scrollbar:
		pkWnd = new ZGuiScrollbar( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Slider:
		pkWnd = new ZGuiSlider( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
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
	
	return true;
}

void GuiBuilder::InitTextures()
{
	// first texture loaded do not show up (?). fulhack fix: load crap texture.
	int crap = m_pkTexMan->Load("/data/textures/gui/crap.bmp", 0);

	typedef map<string, ZGuiSkin*>::value_type strSkin;

	m_kSkins.insert(strSkin("DefWndSkin", new ZGuiSkin(GetTexID("defwnd.bmp"),0)));
	m_kSkins.insert(strSkin("DefBnUSkin", new ZGuiSkin(GetTexID("bn_u.bmp"),0)));
	m_kSkins.insert(strSkin("DefBnDSkin", new ZGuiSkin(GetTexID("bn_d.bmp"),0)));
	m_kSkins.insert(strSkin("DefBnFSkin", new ZGuiSkin(GetTexID("bn_f.bmp"),0)));
	m_kSkins.insert(strSkin("DefRBnUSkin", new ZGuiSkin(GetTexID("rbn_u.bmp"),GetTexID("rbn_a.bmp"),0)));
	m_kSkins.insert(strSkin("DefRBnDSkin", new ZGuiSkin(GetTexID("rbn_d.bmp"),GetTexID("rbn_a.bmp"),0)));
	m_kSkins.insert(strSkin("DefCBnUSkin", new ZGuiSkin(GetTexID("cbn_u.bmp"),0)));
	m_kSkins.insert(strSkin("DefCBnDSkin", new ZGuiSkin(GetTexID("cbn_d.bmp"),0)));
	m_kSkins.insert(strSkin("DefSBrNSkin", new ZGuiSkin(GetTexID("sb_n.bmp"),0)));
	m_kSkins.insert(strSkin("DefSBrFSkin", new ZGuiSkin(GetTexID("sb_f.bmp"),0)));
	m_kSkins.insert(strSkin("DefSBrBkSkin", new ZGuiSkin(GetTexID("sb_bk.bmp"),1)));

	m_kSkins.insert(strSkin("DefSliderSkin", new ZGuiSkin(GetTexID("slider_a.bmp"),
		GetTexID("slider.bmp"))));
	m_kSkins.insert(strSkin("DefSliderBkSkin", new ZGuiSkin(255,0,0,  0,0,255, 0)));

}

void GuiBuilder::Initialize()
{
	InitTextures();

	// Load gui cursor and hide the os cursor.
	m_pkGui->SetCursor(0,0, m_pkTexMan->Load("/data/textures/gui/cursor.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/cursor_a.bmp", 0), 32, 32);

	SDL_ShowCursor(SDL_DISABLE);
}

int GuiBuilder::GetTexID(char *szFile)
{
	char szPath[256];
	sprintf(szPath, "%s%s", "/data/textures/gui/", szFile);
	return m_pkTexMan->Load(szPath, 0);
}


bool GuiBuilder::CreateNewRadiobuttonGroup(const char *szName, int id)
{
	if(m_szLastRadioBGroup)
		delete[] m_szLastRadioBGroup;

	m_szLastRadioBGroup = new char[strlen(szName)+1];
	strcpy(m_szLastRadioBGroup, szName);

	m_iLastRadioBGroup = id;

	return true;
}
