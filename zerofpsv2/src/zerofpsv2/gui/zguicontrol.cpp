// GuiControl.cpp: implementation of the GuiControl class.
//
//////////////////////////////////////////////////////////////////////

#include "zguicontrol.h"
//#include "../basic/basic.pkg"
#include "../render/zguirenderer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiControl::ZGuiControl(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{

}

ZGuiControl::~ZGuiControl()
{

}

bool ZGuiControl::Render(ZGuiRender* pkRender)
{
/*	if(m_bVisible == true)
		pkRender->RenderBorder(GetScreenRect());*/
	return true;
}


