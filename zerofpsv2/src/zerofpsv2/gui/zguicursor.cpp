// zguicursor.cpp: implementation of the ZGuiCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "../render/zguirenderer.h"
#include "../basic/zguiskin.h"
#include "zguicursor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiCursor::ZGuiCursor()
{
	m_bVisible = true;
	m_pkSkin = NULL;
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

	m_pkRenderer->RenderQuad(m_kRect);	

	if(m_pkSkin->m_unBorderSize > 0)
		m_pkRenderer->RenderBorder(m_kRect);
}

void ZGuiCursor::SetSkin(ZGuiSkin *pkSkin)
{
	m_pkSkin = pkSkin;
}

void ZGuiCursor::SetSize(int Width, int Height)
{
	m_kRect.Right = m_kRect.Left + Width;
	m_kRect.Bottom = m_kRect.Top + Height;
}

bool ZGuiCursor::IsVisible()
{
	return m_bVisible;
}

void ZGuiCursor::Show(bool bShow)
{
	m_bVisible = bShow;
}



