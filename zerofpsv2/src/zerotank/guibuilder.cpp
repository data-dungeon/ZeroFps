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
	case Window:
		pkWnd = new ZGuiWnd( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Button:
		pkWnd = new ZGuiButton( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	case Label:
		pkWnd = new ZGuiLabel( Rect(x,y,x+w,y+h), GetWnd(parentID), true, iID);
		break;
	}
	
	switch(eType)
	{
	case Window:
		pkWnd->SetSkin(GetSkin("DefWndSkin"));
		break;
	case Button:
		static_cast<ZGuiButton*>(pkWnd)->SetButtonUpSkin(GetSkin("DefBnUSkin"));
		static_cast<ZGuiButton*>(pkWnd)->SetButtonDownSkin(GetSkin("DefBnDSkin"));
		static_cast<ZGuiButton*>(pkWnd)->SetButtonHighLightSkin(GetSkin("DefBnFSkin"));
		break;
	case Label:
		break;
	}

	if(eType == Window)
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

