// Check box.cpp: implementation of the Checkbox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguicheckbox.h"
#include "../../render/zguirenderer.h"
#include "zguilabel.h"
#include "../../basic/zguifont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiCheckbox::ZGuiCheckbox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_bChecked = false;
	m_bEnabled = true;
	m_iMaskTexUnchecked = -1; 
	m_iMaskTexChecked = -1;

	m_pkLabel = new ZGuiLabel(Rect(0, 0, kRectangle.Width(), kRectangle.Height()), this, true, 0);
	m_pkLabel->RemoveWindowFlag(WF_CANHAVEFOCUS);
	m_pkLabel->Move(20,0); 
}

ZGuiCheckbox::~ZGuiCheckbox()
{
	ResetStaticClickWnds(m_pkLabel);
	delete m_pkLabel;
}

bool ZGuiCheckbox::Render( ZGuiRender* pkRenderer )
{
	if(m_pkFont)
		pkRenderer->SetFont(m_pkFont);

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

void ZGuiCheckbox::SetText(char* strText)
{
	int iWidth = 0;

	ZGuiFont* pkFont = m_pkFont;

	if(pkFont)
	{
		for(int i=0; i<strlen(strText); i++)
		{
			char letter = strText[i];
			iWidth += ( pkFont->m_aChars[letter].iSizeX + 
				pkFont->m_cPixelGapBetweenChars );
		}
	}
	else
	{
		for(int i=0; i<strlen(strText); i++)
			iWidth += 16;
	}

	iWidth += 10;

	m_pkLabel->SetText(strText);
	m_pkLabel->Resize(iWidth,16); 
	m_pkLabel->Enable();
}

char* ZGuiCheckbox::GetText() 
{ 
	return m_pkLabel->GetText(); 
} 

bool ZGuiCheckbox::IsChecked()
{
	return m_bChecked;
}
