// Button.cpp: implementation of the Button class.
//
//////////////////////////////////////////////////////////////////////

#include "..\basic\zguiskin.h"
#include "zguibutton.h"
#include "..\render\zguirenderer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiButton::ZGuiButton(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kArea, pkParent, bVisible, iID)
{
	m_bEnabled = true;
	m_kSkinUp=NULL;
	m_kSkinDown=NULL;
	m_kSkinHighLight=NULL;
	m_iMaskTextureUp = -1;
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
			m_pkSkin = m_kSkinDown;
		}
		else
		{
			m_iBkMaskTexture = m_iMaskTextureUp;
			m_pkSkin = m_kSkinUp;
		}
	}
	else
	if(iCode == NCODE_OVER_CTRL)
		m_pkSkin = m_kSkinHighLight;
	else
	{
		m_pkSkin = m_kSkinUp;
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
	if(m_iBkMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(), (m_iBkMaskTexture > 0)); 

	if(m_pkSkin)
		pkRenderer->RenderBorder(GetScreenRect().Contract(m_pkSkin->m_unBorderSize));

	if(m_strText != NULL)
	{
		if(m_pkTextSkin && m_pkTextSkin->m_iBkTexID != -1)
		{
			if(m_iTextMaskTexture > 0)
				pkRenderer->SetMaskTexture(m_iTextMaskTexture);

			pkRenderer->SetSkin(m_pkTextSkin); 
			pkRenderer->RenderText(m_strText, GetScreenRect(), 12, -1, (m_iTextMaskTexture > 0));
		}
	}

	return true;
}

void ZGuiButton::SetButtonUpSkin(ZGuiSkin* pkSkin, int iMaskTexture)
{
	m_pkSkin = m_kSkinUp = pkSkin;
	m_iMaskTextureUp = iMaskTexture;
	m_iBkMaskTexture = m_iMaskTextureUp;
}

void ZGuiButton::SetButtonDownSkin(ZGuiSkin* pkSkin)
{
	m_kSkinDown = pkSkin;
}

void ZGuiButton::SetButtonHighLightSkin(ZGuiSkin* pkSkin)
{
	m_kSkinHighLight = pkSkin;
}