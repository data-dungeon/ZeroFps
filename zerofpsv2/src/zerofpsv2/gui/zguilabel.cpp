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
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{
	m_bCenterTextHorz = false;
	m_bEnabled = false; // labels are static by default
	RemoveWindowFlag(WF_CANHAVEFOCUS); // fönster har focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
}

ZGuiLabel::~ZGuiLabel()
{

}

bool ZGuiLabel::Render( ZGuiRender* pkRenderer )
{
	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 

	if(m_pkGUI)
	{
		int curr_res_x, curr_res_y;
		m_pkGUI->GetResolution(curr_res_x, curr_res_y);
//		Rescale(m_iResolutionX, m_iResolutionY, curr_res_x, curr_res_y);
	}

	if(m_strText != NULL)
	{
		Rect r = GetScreenRect();

		//if(m_pkFont)
		//{
		//	pkRenderer->SetFont(m_pkFont);

		//	if(m_bCenterTextHorz)
		//	{
		//		int tw = m_pkFont->GetLength(m_strText);
		//		r.Left = r.Left + r.Width()/2 - tw/2;
		//	}
		//}

		if(m_pkFont)
		{
			pkRenderer->SetFont(m_pkFont);

			if(m_bCenterTextHorz)
			{
				int tw = m_pkFont->GetLength(m_strText);
				r.Left = r.Left + r.Width()/2 - tw/2;
			}
		}

		//pkRenderer->RenderText(m_strText, r, -1, 0, false, iLetters, iRows, m_afTextColor);
		pkRenderer->RenderText(m_strText, r, -1, m_afTextColor);
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





