// zguiscrollbar.cpp: implementation of the ZGuiScrollbar class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiscrollbar.h"
#include "zguibutton.h"
#include "../render/zguirenderer.h"
#include "zgui.h"
#include <typeinfo>
#include "../basic/globals.h"

// Static internal IDs for the scrollbar button
const int SCROLLTHUMB_ID = 520;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiScrollbar::ZGuiScrollbar(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID) :
	ZGuiWnd(kArea, pkParent, bVisible, iID)
{
	CreateInternalControls();
	m_bEnabled = true;
	m_iScrollChange = 0;
	m_bAutoHideScrollbar = true;
	m_usThumbSize = 20;
	m_fPageSize = 1.0f;
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte användas som mainwindow
}

ZGuiScrollbar::~ZGuiScrollbar()
{
	delete m_pkThumbButton;
}

bool ZGuiScrollbar::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	if(m_pkGUI)
	{
		int curr_res_x, curr_res_y;
		m_pkGUI->GetResolution(curr_res_x, curr_res_y);
		Rescale(m_iResolutionX, m_iResolutionY, curr_res_x, curr_res_y);
	}
		
	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 
	m_pkThumbButton->Render( pkRenderer );
	m_pkScrollUp->Render( pkRenderer );
	m_pkScrollDown->Render( pkRenderer );
	return true;
}

void ZGuiScrollbar::SetThumbButtonSkins(ZGuiSkin* pkSkinNormal, ZGuiSkin* pkSkinHighLight)
{
	m_pkThumbButton->SetButtonUpSkin(pkSkinNormal);
	m_pkThumbButton->SetButtonDownSkin(pkSkinHighLight);
	m_pkThumbButton->SetButtonHighLightSkin(pkSkinHighLight);
}

void ZGuiScrollbar::SetScrollButtonUpSkins(ZGuiSkin* pkSkinUp, ZGuiSkin* pkSkinDown)
{
	m_pkScrollUp->SetButtonUpSkin(pkSkinUp);
	m_pkScrollUp->SetButtonHighLightSkin(pkSkinUp);
	m_pkScrollUp->SetButtonDownSkin(pkSkinDown);	
}

void ZGuiScrollbar::SetScrollButtonDownSkins(ZGuiSkin* pkSkinUp, ZGuiSkin* pkSkinDown)
{
	m_pkScrollDown->SetButtonUpSkin(pkSkinUp);
	m_pkScrollDown->SetButtonHighLightSkin(pkSkinUp);
	m_pkScrollDown->SetButtonDownSkin(pkSkinDown);	
}

void ZGuiScrollbar::GetScrollInfo(unsigned int& min, unsigned int& max, 
								  unsigned int& pos)
{
	max = m_nMax;
	min = m_nMin;
	pos = m_nPos;
}

// page_size är i procent och skall ligga i intervallet [0-1]
// är värdet större så trunkeras det kring ramen
void ZGuiScrollbar::SetScrollInfo(unsigned int min, unsigned int max, 
											 float page_size, unsigned int pos)
{


	m_nMax = max, m_nMin = min, m_nPos = pos;
	m_fPageSize = page_size;

	if(page_size < 0.0f) page_size = 0.0f;
	if(page_size > 1.0f) page_size = 1.0f;
	if(min > max || max < min) swap(max,min);
	if(pos > max) max = max;
	if(pos < min) pos = min;

	Rect rc = GetWndRect();

	int real_bn_height = m_pkThumbButton->GetWndRect().Height(); 

	if(m_bHorzintal)
	{
		rc.Left += SCROLL_BUTTON_HEIGHT;
		rc.Right -= SCROLL_BUTTON_HEIGHT;

		int size = (int)(page_size*rc.Width());
		
		real_bn_height = size;
		
		m_pkThumbButton->Resize(size, rc.Height());
		m_usThumbSize = max(size, rc.Height());
	}
	else
	{
		rc.Top += SCROLL_BUTTON_HEIGHT;
		rc.Bottom -= SCROLL_BUTTON_HEIGHT;

		int size = (int)(page_size*rc.Height());
		
		real_bn_height = size;
		
		m_pkThumbButton->Resize(rc.Width(),size);
		m_usThumbSize = max(size, rc.Width());
	}

	float x=0, y=0;

	if(m_bHorzintal)
	{
		int bn_width = real_bn_height; 

		float size = (float) (max - min);
		if(size <= 0) size = 1; // don´t devide by zero

		x = ((float) pos / (float) (size)) * (rc.Width()/*-SCROLL_BUTTON_HEIGHT*2*/)  - bn_width/2;	
	}
	else
	{
		int bn_width = real_bn_height; 

		float size = (float) (max - min);
		if(size <= 0) size = 1; // don´t devide by zero

		y = ((float) pos / (float) (size)) * (rc.Height()/*-SCROLL_BUTTON_HEIGHT*2*/)  - bn_width/2;	
	}

	Rect rcMove = GetScreenRect();

	if(m_bHorzintal)
	{
		rcMove.Left += SCROLL_BUTTON_HEIGHT;
		rcMove.Right -= SCROLL_BUTTON_HEIGHT;
		m_pkThumbButton->SetMoveArea(rcMove);
		m_pkThumbButton->SetPos((int)x+SCROLL_BUTTON_HEIGHT+(real_bn_height/2),(int)y, false, false);
	}
	else
	{
		rcMove.Top += SCROLL_BUTTON_HEIGHT;
		rcMove.Bottom -= SCROLL_BUTTON_HEIGHT;
		m_pkThumbButton->SetMoveArea(rcMove);
		m_pkThumbButton->SetPos((int)x,(int)y+SCROLL_BUTTON_HEIGHT+(real_bn_height/2), false, false);
	}
	
	if(m_bAutoHideScrollbar)
	{
		if(page_size > 0.99f)
		{
			Hide();
			m_pkThumbButton->Hide();
			m_pkScrollUp->Hide();
			m_pkScrollDown->Hide();
		}
		else
		{
			Show();
			m_pkThumbButton->Show();
			m_pkScrollUp->Show();
			m_pkScrollDown->Show();
		}
	}

}

bool ZGuiScrollbar::Notify(ZGuiWnd* pkWnd, int iCode)
{
	bool bSend = false;

	if(iCode == NCODE_MOVE)
	{
		if(pkWnd->GetID() == SCROLLTHUMB_ID)
		{
			Rect rcButton = pkWnd->GetScreenRect();

			Rect rcArea = GetScreenRect();
			rcArea.Top += SCROLL_BUTTON_HEIGHT;
			rcArea.Bottom -= SCROLL_BUTTON_HEIGHT;

			int size = rcArea.Height();
			int y = rcButton.Top - rcArea.Top;

			float fProcentAvMax = (float) y / (float) size;

			static int POS_BEFORE;
			POS_BEFORE = (int)m_nPos;

			m_nPos = (unsigned int) (fProcentAvMax * (float)(m_nMax-m_nMin+1));

			int change = abs(POS_BEFORE-(int)m_nPos);

			if(m_nPos < (unsigned int) POS_BEFORE)
				m_iScrollChange = change;
			else
				m_iScrollChange = -change;

			bSend = true;
		}

	}
	else
	if(iCode == NCODE_CLICK_DOWN)
	{
		if(pkWnd->GetID() == SCROLLUP_ID)
		{
			if(m_nPos > m_nMin)
			{
				m_nPos--;
				m_iScrollChange=-1;
				bSend = true;
				SetScrollInfo(m_nMin, m_nMax, m_fPageSize, m_nPos);
			}
			else
			{
				m_iScrollChange=0;
			}
		}
		else
		if(pkWnd->GetID() == SCROLLDOWN_ID)
		{
			if(m_nPos < m_nMax)
			{
				m_nPos++;
				m_iScrollChange=1;
				bSend = true;
				SetScrollInfo(m_nMin, m_nMax, m_fPageSize, m_nPos);
			}
			else
			{
				m_iScrollChange=0;
			}
		}
	}

	if(bSend)
	{
		// Send a scroll message to the main winproc...
		int* piParams = new int[3];
		piParams[0] = GetID(); // id
		piParams[1] = m_bHorzintal; // horizontal or vertical
		piParams[2] = m_nPos;
		GetGUI()->GetActiveCallBackFunc()(
			GetGUI()->GetActiveMainWnd(), ZGM_SCROLL, 1, piParams);

		delete[] piParams;
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return false;
}

void ZGuiScrollbar::CreateInternalControls()
{
	m_bHorzintal = GetWndRect().Width() > GetWndRect().Height() ? true : false;
	Rect rcThumb, rcUp, rcDown;
	
	if(m_bHorzintal)
	{
		m_usThumbSize = GetWndRect().Height();

		rcUp.Left	= 0;
		rcUp.Top		= 0;
		rcUp.Right	= SCROLL_BUTTON_HEIGHT;
		rcUp.Bottom	= GetWndRect().Height();

		rcDown.Left		= GetWndRect().Width()-SCROLL_BUTTON_HEIGHT;
		rcDown.Top		= 0;
		rcDown.Right	= GetWndRect().Width();
		rcDown.Bottom	= GetWndRect().Height();

		rcThumb.Left	= SCROLL_BUTTON_HEIGHT;
		rcThumb.Top		= 0;
		rcThumb.Right	= SCROLL_BUTTON_HEIGHT+m_usThumbSize;
		rcThumb.Bottom	= SCROLL_BUTTON_HEIGHT;
	}
	else
	{
		m_usThumbSize = GetWndRect().Width()-(SCROLL_BUTTON_HEIGHT*2);

		rcUp.Left	= 0;
		rcUp.Top		= 0;
		rcUp.Right	= GetWndRect().Width();
		rcUp.Bottom	= SCROLL_BUTTON_HEIGHT;

		rcDown.Left		= 0;
		rcDown.Top		= GetWndRect().Height()-SCROLL_BUTTON_HEIGHT;
		rcDown.Right	= GetWndRect().Width();
		rcDown.Bottom	= GetWndRect().Height();

		rcThumb.Left	= 0;
		rcThumb.Top		= SCROLL_BUTTON_HEIGHT;
		rcThumb.Right	= m_usThumbSize;
		rcThumb.Bottom	= SCROLL_BUTTON_HEIGHT+m_usThumbSize;
	}

	m_pkThumbButton = new ZGuiButton(rcThumb, this, true, SCROLLTHUMB_ID);
	m_pkThumbButton->SetInternalControlState(true);

	m_pkScrollUp = new ZGuiButton(rcUp, this, true, SCROLLUP_ID);
	m_pkScrollUp->SetInternalControlState(true);

	m_pkScrollDown = new ZGuiButton(rcDown, this, true, SCROLLDOWN_ID);
	m_pkScrollDown->SetInternalControlState(true);

	//SetScrollInfo(0,100,0.15f,0);
}

void ZGuiScrollbar::SetZValue(int iValue)
{
	ZGuiWnd::SetZValue(iValue);
	m_pkThumbButton->SetZValue(iValue-1);
	m_pkScrollUp->SetZValue(iValue-2);
	m_pkScrollDown->SetZValue(iValue-3);
}

void ZGuiScrollbar::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkin, string("Scrollbar")) );
	
	int iStart = pkSkinDesc.size(); 
	m_pkThumbButton->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Scrollbar: ");
}

void ZGuiScrollbar::Resize(int iWidth, int iHeight, bool bChangeMoveArea)
{
/*	if(m_bHorzintal)
		iWidth = m_usThumbSize; // dont allow horizontal resize
	else
		iHeight = m_usThumbSize; // dont allow horizontal resize*/

	ZGuiWnd::Resize(iWidth, iHeight, bChangeMoveArea);
	m_pkThumbButton->SetMoveArea(GetScreenRect());
}

void ZGuiScrollbar::SetAutoHide(bool bActivate)
{
	m_bAutoHideScrollbar = bActivate;
}

bool ZGuiScrollbar::AutoHideOn()
{
	return m_bAutoHideScrollbar;
}

void ZGuiScrollbar::ToogleHorizontal()
{
	int size = max(GetScreenRect().Width(), 
		GetScreenRect().Height());
	int bn_size = max(m_pkThumbButton->GetScreenRect().Width(), 
		m_pkThumbButton->GetScreenRect().Height());

	if(m_bHorzintal == false)
	{
		Resize(size,m_usThumbSize);
		m_pkThumbButton->Resize(bn_size, m_usThumbSize);
	}
	else
	{
		Resize(m_usThumbSize,size);
		m_pkThumbButton->Resize(m_usThumbSize,bn_size);
	}

	m_pkThumbButton->SetMoveArea(GetScreenRect());
	m_pkThumbButton->SetPos(0,0);

	m_bHorzintal = !m_bHorzintal;
}

void ZGuiScrollbar::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiScrollbar))
	{
		m_iScrollChange = ((ZGuiScrollbar*)pkSrc)->m_iScrollChange;
		m_nMax = ((ZGuiScrollbar*)pkSrc)->m_nMax;
		m_nMin = ((ZGuiScrollbar*)pkSrc)->m_nMin;
		m_nPos = ((ZGuiScrollbar*)pkSrc)->m_nPos;
		m_usThumbSize = ((ZGuiScrollbar*)pkSrc)->m_usThumbSize;
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
	m_pkThumbButton->SetMoveArea(GetScreenRect());
	m_pkThumbButton->SetPos(0,0,false,true);

	// Change size of scrollbar.
	Resize(
		((ZGuiWnd*)pkSrc)->GetScreenRect().Width(),
		((ZGuiWnd*)pkSrc)->GetScreenRect().Height());

	// Change horizontal/vertical mode and resize thumb button.
	while(m_bHorzintal != ((ZGuiScrollbar*)pkSrc)->m_bHorzintal)
		ToogleHorizontal();

}

void ZGuiScrollbar::SetScrollPos(unsigned int pos)
{
	if(pos >= m_nMin && pos <= m_nMax)
		SetScrollInfo(m_nMin, m_nMax, m_fPageSize, pos);

	Notify(m_pkThumbButton, NCODE_MOVE);
}
