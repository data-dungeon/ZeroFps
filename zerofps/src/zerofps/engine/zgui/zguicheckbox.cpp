// Check box.cpp: implementation of the Checkbox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguicheckbox.h"
#include "../../render/zguirenderer.h"
#include "zguilabel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiCheckbox::ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_bChecked = false;
	m_bEnabled = true;
	m_iMaskTexUnchecked, m_iMaskTexChecked = -1;

	m_pkLabel = new ZGuiLabel(kRectangle, this, true, 0);
	m_pkLabel->Move(20,0); 
}

ZGuiCheckbox::~ZGuiCheckbox()
{

}

bool ZGuiCheckbox::Render( ZGuiRender* pkRenderer )
{
	if(m_iBkMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(), (m_iBkMaskTexture > 0)); 

	m_pkLabel->Render(pkRenderer); 
	return false;
}

void ZGuiCheckbox::SetButtonCheckedSkin(ZGuiSkin* pkSkin, int iMaskTexture)
{
	m_pkSkinButtonChecked = pkSkin;
	m_iMaskTexChecked = iMaskTexture;
}

void ZGuiCheckbox::SetButtonUncheckedSkin(ZGuiSkin* pkSkin, int iMaskTexture)
{
	m_pkSkin = m_pkSkinButtonUnchecked = pkSkin;
	m_iBkMaskTexture = m_iMaskTexUnchecked = iMaskTexture;
}

bool ZGuiCheckbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(iCode == NCODE_CLICK_UP)
	{
		m_bChecked = !m_bChecked;

		if(m_bChecked == false)
		{
			m_pkSkin = m_pkSkinButtonUnchecked;
			m_iBkMaskTexture = m_iMaskTexUnchecked;
		}
		else
		{
			m_pkSkin = m_pkSkinButtonChecked;
			m_iBkMaskTexture = m_iMaskTexChecked;
		}
	}

	printf("Hej från ZGuiCheckbox::Notify\n");

	m_pkParent->Notify(this, iCode); 

	return false;
}

void ZGuiCheckbox::CheckButton()
{
	m_bChecked = true;
	m_pkSkin = m_pkSkinButtonChecked;
	m_iBkMaskTexture = m_iMaskTexChecked;
}

void ZGuiCheckbox::UncheckButton()
{
	m_bChecked = false;
	m_pkSkin = m_pkSkinButtonUnchecked;
	m_iBkMaskTexture = m_iMaskTexUnchecked;
}

void ZGuiCheckbox::SetTextSkin(ZGuiSkin* kSkin, int iMaskTexture)
{
	m_pkLabel->SetTextSkin(kSkin, iMaskTexture);
}

void ZGuiCheckbox::SetText(char* strText)
{
	m_pkLabel->SetText( strText);
}




