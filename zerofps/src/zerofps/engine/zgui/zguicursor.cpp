// zguicursor.cpp: implementation of the ZGuiCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "../../render/zguirenderer.h"
#include "zguicursor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiCursor::ZGuiCursor()
{
	m_pkSkin = NULL;
	m_iMaskTexture = -1;
	m_pkRenderer = static_cast<ZGuiRender*>(g_ZFObjSys.GetObjectPtr("ZGuiRender"));
	m_kRect = Rect(0,0,16,16);
}

ZGuiCursor::~ZGuiCursor()
{

}

void ZGuiCursor::SetPos(int x, int y)
{
	int w = m_kRect.Width();
	int h = m_kRect.Height();

	m_kRect.Left = x;
	m_kRect.Top = y;
	m_kRect.Right = x+w;
	m_kRect.Bottom = y+h;
}

void ZGuiCursor::Render()
{
	if(m_bVisible == false)
		return;

	if(m_pkSkin != NULL)
		m_pkRenderer->SetSkin(m_pkSkin);

	bool bMask = (m_iMaskTexture == -1) ? false : true;

	if(bMask)
		m_pkRenderer->SetMaskTexture(m_iMaskTexture);

	m_pkRenderer->RenderQuad(m_kRect, bMask);
}

void ZGuiCursor::SetSkin(ZGuiSkin *pkSkin, int iMaskTexture)
{
	m_pkSkin = pkSkin;
	m_iMaskTexture = iMaskTexture;
}

void ZGuiCursor::SetSize(int Width, int Height)
{
	m_kRect.Right = m_kRect.Left + Width;
	m_kRect.Bottom = m_kRect.Top + Height;
}
