// Label.cpp: implementation of the Label class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/zguiskin.h"
#include "zguilabel.h"
#include "../render/zguirenderer.h"
#include "zgui.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiLabel::ZGuiLabel(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_bEnabled = false; // labels are static by default
	RemoveWindowFlag(WF_CANHAVEFOCUS); // fönster har focus by default
}

ZGuiLabel::~ZGuiLabel()
{

}

bool ZGuiLabel::Render( ZGuiRender* pkRenderer )
{
	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_strText != NULL)
	{
		if(m_pkFont)
			pkRenderer->SetFont(m_pkFont);

		pkRenderer->RenderText(m_strText, GetScreenRect(), -1, false);
	}
	return true;
} 

bool ZGuiLabel::Notify(ZGuiWnd* pkWnd, int iCode)
{
	// Notify the parent...
	if(m_bEnabled)
	{
		if(iCode == NCODE_CLICK_UP)
		{
			m_pkParent->Notify(this, iCode); 
		}
	}
	return false;
}





