// ZGuiMenu.cpp: implementation of the ZGuiMenu class.
//
//////////////////////////////////////////////////////////////////////
 
#include "../basic/zguiskin.h"
#include "zguimenu.h"
#include "../render/zguirenderer.h"
#include "zgui.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////// //////////////////////////////////////////////////////////

ZGuiMenu::ZGuiMenu(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_bEnabled = true;
	RemoveWindowFlag(WF_CANHAVEFOCUS); // knappar har inte focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
}

ZGuiMenu::~ZGuiMenu()
{

}

bool ZGuiMenu::Notify(ZGuiWnd* pkWindow, int iCode)
{
	return false;
}

bool ZGuiMenu::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	return true;
}

void ZGuiMenu::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
//	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*) m_pkSkinBnUp, string("Button up")) );
}






