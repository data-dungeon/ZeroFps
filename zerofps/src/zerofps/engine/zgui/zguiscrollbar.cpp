// zguiscrollbar.cpp: implementation of the ZGuiScrollbar class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiscrollbar.h"
#include "zguibutton.h"
#include "../../render/zguirenderer.h"
#include "zgui.h"

// Static internal IDs for the scrollbar button
const int SCROLLTHUMB_ID = 520;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiScrollbar::ZGuiScrollbar(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiControl(kArea, pkParent, bVisible, iID)
{
	CreateInternalControls();
	m_bEnabled = true;
	m_iScrollChange = 0;
}

ZGuiScrollbar::~ZGuiScrollbar()
{

}

bool ZGuiScrollbar::Render( ZGuiRender* pkRenderer )
{
	if(m_iBkMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(),(m_iBkMaskTexture > 0)); 
	pkRenderer->RenderBorder(GetScreenRect(),(m_iBkMaskTexture > 0)); 
	m_pkThumbButton->Render( pkRenderer );
	return true;
}

void ZGuiScrollbar::SetThumbButtonSkins(ZGuiSkin* pkSkinNormal, ZGuiSkin* pkSkinHighLight)
{
	m_pkThumbButton->SetButtonUpSkin(pkSkinNormal);
	m_pkThumbButton->SetButtonDownSkin(pkSkinNormal);
	m_pkThumbButton->SetButtonHighLightSkin(pkSkinHighLight);
}

// page_size är i procent och skall ligga i intervallet [0-1]
// är värdet större så sätts det till 1 och är det mindre, ja då blir det 0 (!)
void ZGuiScrollbar::SetScrollInfo(unsigned int min, unsigned int max, float page_size, unsigned int pos)
{
	if(page_size < 0.0f) page_size = 0.0f;
	if(page_size > 1.0f) page_size = 1.0f;
	if(min > max || max < min) swap(max,min);
	if(pos > max) max = max;
	if(pos < min) pos = min;

	Rect rc = GetWndRect();

	if(m_bHorzintal)
		m_pkThumbButton->Resize((int)(page_size*rc.Width()),rc.Height());
	else
		m_pkThumbButton->Resize(rc.Width(),(int)(page_size*rc.Height()));

	float x=0, y=0;

	if(m_bHorzintal)
	{
		int bn_width = m_pkThumbButton->GetWndRect().Width(); 
		x = ((float) pos / (float) (max - min)) * rc.Width()  - bn_width/2;	
	}
	else
	{
		int bn_width = m_pkThumbButton->GetWndRect().Height(); 
		y = ((float) pos / (float) (max - min)) * rc.Height()  - bn_width/2;	
	}

	m_pkThumbButton->SetMoveArea(GetScreenRect());
	m_pkThumbButton->SetPos((int)x, (int)y, false, false);

	m_nMax = max, m_nMin = min, m_nPos = pos;
}

bool ZGuiScrollbar::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(pkWnd->GetID() == SCROLLTHUMB_ID)
	{
		int iThumPos = m_pkThumbButton->GetWndRect().Top;
		int iThumMax = GetWndRect().Bottom;
		float fProcentAvMax = (float) iThumPos / (float) iThumMax;

		static bool NEW_CLICK = true;
		static int POS_BEFORE;

		NEW_CLICK = false;
		POS_BEFORE = (int)m_nPos;

		m_nPos = (unsigned int) (fProcentAvMax * (m_nMax-m_nMin+1));

		int change = abs(POS_BEFORE-(int)m_nPos);

		if(m_nPos < (unsigned int) POS_BEFORE)
			m_iScrollChange = change;
		else
			m_iScrollChange = -change;

		NEW_CLICK = true;
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return false;
}

void ZGuiScrollbar::CreateInternalControls()
{
	m_bHorzintal = GetWndRect().Width() > GetWndRect().Height() ? true : false;

	int m_unThumbSize;
	Rect rcThumb;
	
	if(m_bHorzintal)
	{
		m_unThumbSize = GetWndRect().Height();
	}
	else
	{
		m_unThumbSize = GetWndRect().Width();
	}

	rcThumb.Left	= 0;
	rcThumb.Top		= 0;
	rcThumb.Right	= m_unThumbSize;
	rcThumb.Bottom	= m_unThumbSize;

	m_pkThumbButton = new ZGuiButton(rcThumb, this, true, SCROLLTHUMB_ID);

	SetScrollInfo(0,100,0.15f,0);
}



