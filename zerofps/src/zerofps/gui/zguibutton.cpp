// Button.cpp: implementation of the Button class.
//
//////////////////////////////////////////////////////////////////////

#include "../basic/zguiskin.h"
#include "zguibutton.h"
#include "../render/zguirenderer.h"
#include "zgui.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiButton::ZGuiButton(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kArea, pkParent, bVisible, iID)
{
	m_bEnabled = true;
	m_pkSkinBnUp=NULL;
	m_pkSkinBnDown=NULL;
	m_pkSkinBnHLight=NULL;
	RemoveWindowFlag(WF_CANHAVEFOCUS); // knappar har inte focus by default
}

ZGuiButton::~ZGuiButton()
{

}

bool ZGuiButton::Notify(ZGuiWnd* pkWindow, int iCode)
{
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

	if(m_pkGUI)
	{
		int curr_res_x, curr_res_y;
		m_pkGUI->GetResolution(curr_res_x, curr_res_y);
		Rescale(m_iResolutionX, m_iResolutionY, curr_res_x, curr_res_y);
	}

	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 

	if(m_pkSkin)
		pkRenderer->RenderBorder(GetScreenRect().Contract(m_pkSkin->m_unBorderSize));

	if(m_strText != NULL)
	{
		Rect rcTextRect = GetScreenRect();
		if(GetWindowFlag(WF_CENTER_TEXT) == true)
		{
			int textwidth = 12;
			int buttonWidth = 20;
			int x_diff = buttonWidth/2-textwidth/2;
	
			rcTextRect.Left += x_diff;
			rcTextRect.Right += x_diff;

/*			int textheight = 12;
			int buttonHeight = 20;
			int y_diff = buttonHeight/2-textheight/2;

			rcTextRect.Top += y_diff;
			rcTextRect.Bottom += y_diff;*/
		}

		pkRenderer->RenderText(m_strText, rcTextRect, -1);
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
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnUp, string("Button up")) );
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnDown, string("Button down")) );
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnHLight, string("Button focus")) );
}






