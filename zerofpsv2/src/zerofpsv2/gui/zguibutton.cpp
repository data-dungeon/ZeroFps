// Button.cpp: implementation of the Button class.
//
//////////////////////////////////////////////////////////////////////
 
#include "../basic/zguiskin.h"
#include "zguibutton.h"
#include "../render/zguirenderer.h"
#include "zgui.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////// //////////////////////////////////////////////////////////

ZGuiButton::ZGuiButton(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	m_bEnabled = true;
	m_pkSkinBnUp=NULL;
	m_pkSkinBnDown=NULL;
	m_pkSkinBnHLight=NULL;
	RemoveWindowFlag(WF_CANHAVEFOCUS); // knappar har inte focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
	m_bCenterTextHorz = true;
}

ZGuiButton::~ZGuiButton()
{

}

bool ZGuiButton::Notify(ZGuiWnd* pkWindow, int iCode)
{
	if(m_bEnabled == false)
	{
		m_pkSkin = m_pkSkinBnUp;
		return false;
	}

	if(iCode == NCODE_CLICK_DOWN)
	{
		if(GetMoveArea() == GetScreenRect())
		{
			m_pkSkin = m_pkSkinBnDown;
		}
		else
		{
			m_pkSkin = m_pkSkinBnUp;
		}
	}
	else
	if(iCode == NCODE_OVER_CTRL)
		m_pkSkin = m_pkSkinBnHLight;
	else
	{
		m_pkSkin = m_pkSkinBnUp;
	}

	if(iCode == NCODE_CLICK_UP)
	{
	
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return false;
}

bool ZGuiButton::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 

	if(m_pkSkin)
		pkRenderer->RenderBorder(GetScreenRect().Contract(m_pkSkin->m_unBorderSize));

	if(m_strText != NULL && m_pkFont)
	{
		Rect rcTextRect = GetScreenRect();

		if(m_bCenterTextHorz)
		{
			int text_w = m_pkFont->GetLength(m_strText);
			int x = rcTextRect.Left + rcTextRect.Width() / 2 - text_w / 2;
			rcTextRect.Left = x;
			rcTextRect.Right = x + text_w;
		}
		
		pkRenderer->RenderText(m_strText, rcTextRect, -1, m_afTextColor);
	}

	return true;
}

void ZGuiButton::SetButtonUpSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnUp = pkSkin;
	m_pkSkin = m_pkSkinBnUp;
}

void ZGuiButton::SetButtonDownSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnDown = pkSkin;
}

void ZGuiButton::SetButtonHighLightSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnHLight = pkSkin;
}

void ZGuiButton::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*) m_pkSkinBnUp, string("Button up")) );
	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*) m_pkSkinBnDown, string("Button down")) );
	pkSkinDesc.push_back( SKIN_DESC(&(ZGuiSkin*) m_pkSkinBnHLight, string("Button focus")) );
}






