// zguitooltip.cpp: implementation of the ZGuiToolTip class.
//
//////////////////////////////////////////////////////////////////////

#include "zguitooltip.h"
#include "zgui.h"
#include "zguiresourcemanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiToolTip::ZGuiToolTip(float fDisplayTime) : m_fDisplayTime(fDisplayTime)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkToolTipWnd = NULL;

	m_pkLastToolTipWnd = NULL;
	m_fToolTipDisplayTime = 0.0f;

	m_iPrevCursorX = m_iPrevCursorY = -1;

	if(m_pkToolTipWnd == NULL)
	{
		m_pkToolTipWnd = new ZGuiLabel(Rect(300,300,400,400),  NULL, true, 432342);
		m_pkToolTipWnd->SetSkin(new ZGuiSkin( 255, 0, 0, 255, 0, 0,  2));
	}
}

ZGuiToolTip::~ZGuiToolTip()
{

}

void ZGuiToolTip::Update(int mouse_x, int mouse_y, bool bMouseClick, float fGameTime)
{
	if(m_pkToolTipWnd->GetFont() == NULL )
	{
		m_pkToolTipWnd->SetFont(m_pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT));  
	}

	if(!(mouse_x == m_iPrevCursorX && mouse_y == m_iPrevCursorY))
	{
		if(m_pkLastToolTipWnd)
		{
			if(!m_pkLastToolTipWnd->GetScreenRect().Inside(mouse_x,mouse_y))
				m_pkLastToolTipWnd = NULL;
		}
	}
	
	m_pkToolTipWnd->Hide();

	for(unsigned int i=0; i<m_vkWindowList.size(); i++)
	{
		ZGuiWnd* pkWnd = m_vkWindowList[i].pkWnd;
		ZGuiWnd* pkParent = pkWnd->GetParent();

		if(pkWnd->IsVisible() )
		{
			if(pkWnd->GetScreenRect().Inside(mouse_x,mouse_y))
			{
				m_pkToolTipWnd->Show(); 
				m_pkToolTipWnd->SetText((char*)m_vkWindowList[i].strText.c_str(), true);

				Rect rc = pkWnd->GetScreenRect();

				int w = m_pkToolTipWnd->GetScreenRect().Width();
				int h = m_pkToolTipWnd->GetScreenRect().Height();

				int x = rc.Left + (rc.Right - rc.Left)/2 - w/2;
				int y = rc.Top - h;

				if(x + w > 800)
					x -= (rc.Right-800);

				if(y + h > 600)
					y -= (rc.Bottom-600);

				m_pkToolTipWnd->SetPos( x, y, true, true);

				if(m_pkLastToolTipWnd != pkWnd)
				{
					m_fToolTipDisplayTime = fGameTime;
					m_pkLastToolTipWnd = pkWnd;
				}
				break;
			}
		}
	}

	if(fGameTime - m_fToolTipDisplayTime > m_fDisplayTime)
	{
		m_pkToolTipWnd->Hide();
	}

/*	if(bMouseClick)
	{
		m_fToolTipDisplayTime = fGameTime;
	//	m_pkLastToolTipWnd = NULL;
		m_pkToolTipWnd->Hide();
	}
*/
	m_iPrevCursorX = mouse_x;
	m_iPrevCursorY = mouse_y;
}

void ZGuiToolTip::SetToolTip(ZGuiWnd* pkWnd, string strText)
{
	TOOLTIP_INFO ttinfo;

	ttinfo.pkWnd = pkWnd;
	ttinfo.strText = strText;

	m_vkWindowList.push_back(ttinfo);
}


void ZGuiToolTip::SetSkin(ZGuiSkin kSkin)
{
	ZGuiSkin* pkSkin = m_pkToolTipWnd->GetSkin();

	memcpy(pkSkin, &kSkin, sizeof(ZGuiSkin));
}
