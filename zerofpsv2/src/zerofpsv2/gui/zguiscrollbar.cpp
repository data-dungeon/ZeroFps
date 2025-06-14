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
	m_iScrollPosXBefore = 0;
	m_iScrollPosYBefore = 0;
	m_bAutoHideScrollbar = true;
	m_usThumbSize = 20;
	m_fPageSize = 1.0f;
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte anv�ndas som mainwindow

	SetScrollInfo(0,0,0.25f,0); // s�tt till n�got bara...
}

ZGuiScrollbar::~ZGuiScrollbar()
{
	delete m_pkThumbButton;
}

bool ZGuiScrollbar::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;
	
	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect()); 
	pkRenderer->RenderBorder(GetScreenRect()); 
	m_pkThumbButton->Render( pkRenderer );
	m_pkArrowBnUp->Render( pkRenderer );
	m_pkArrowBnDown->Render( pkRenderer );
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
	if(pkSkinUp)
	{
		m_pkArrowBnUp->SetButtonUpSkin(pkSkinUp);
		m_pkArrowBnUp->SetButtonHighLightSkin(pkSkinUp);
	}

	if(pkSkinDown)
		m_pkArrowBnUp->SetButtonDownSkin(pkSkinDown);	
}

void ZGuiScrollbar::SetScrollButtonDownSkins(ZGuiSkin* pkSkinUp, ZGuiSkin* pkSkinDown)
{
	if(pkSkinUp)
	{
		m_pkArrowBnDown->SetButtonUpSkin(pkSkinUp);
		m_pkArrowBnDown->SetButtonHighLightSkin(pkSkinUp);
	}

	if(pkSkinDown)
		m_pkArrowBnDown->SetButtonDownSkin(pkSkinDown);	
}

void ZGuiScrollbar::GetScrollInfo(unsigned int& min, unsigned int& max, 
								  unsigned int& pos)
{
	max = m_nMax;
	min = m_nMin;
	pos = m_nPos;
}

// page_size �r i procent och skall ligga i intervallet [0-1]
// �r v�rdet st�rre s� trunkeras det kring ramen
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
		rc.Left += m_pkArrowBnUp->GetScreenRect().Width();
		rc.Right -= m_pkArrowBnUp->GetScreenRect().Width();

		int size = (int)(page_size*rc.Width());
		
		real_bn_height = size;
		
		m_pkThumbButton->Resize(size, rc.Height());
		m_usThumbSize = (unsigned int)Max(size, rc.Height());
	}
	else
	{
		rc.Top += m_pkArrowBnUp->GetScreenRect().Height();
		rc.Bottom -= m_pkArrowBnUp->GetScreenRect().Height();

		int size = (int)(page_size*rc.Height());
		
		real_bn_height = size;
		
		int test = size;
		if(test < 10)
			test = 10;
		m_pkThumbButton->Resize(rc.Width(),test);
		
		m_usThumbSize = (unsigned int)Max(size, rc.Width());
	}

	float x=0, y=0;

	if(m_bHorzintal)
	{
		int bn_width = real_bn_height; 

		float size = (float) (max - min);
		if(size <= 0) size = 1; // don�t devide by zero

		x = ((float) pos / (float) (size)) * (rc.Width()/*-m_iScrollButtonHeight*2*/)  - bn_width/2;	
	}
	else
	{
		int bn_width = real_bn_height; 

		float size = (float) (max - min);
		if(size <= 0) size = 1; // don�t devide by zero

		y = ((float) pos / (float) (size)) * (rc.Height()/*-m_iScrollButtonHeight*2*/)  - bn_width/2;	
	}

	Rect rcMove = GetScreenRect();

	int bn_size;

	if(m_bHorzintal)
	{
		bn_size = m_pkArrowBnUp->GetScreenRect().Width();
		rcMove.Left += bn_size;
		rcMove.Right -= bn_size;
		m_pkThumbButton->SetMoveArea(rcMove);
		m_pkThumbButton->SetPos((int)x+bn_size+(real_bn_height/2),(int)y, false, false);
	}
	else
	{
		bn_size = m_pkArrowBnUp->GetScreenRect().Height();
		rcMove.Top += bn_size;
		rcMove.Bottom -= bn_size;
		m_pkThumbButton->SetMoveArea(rcMove);
		m_pkThumbButton->SetPos((int)x,(int)y+bn_size+(real_bn_height/2), false, false);
	}
	
	if(m_bAutoHideScrollbar)
	{
		if(page_size > 0.99f)
		{
			Hide();
			m_pkThumbButton->Hide();
			m_pkArrowBnUp->Hide();
			m_pkArrowBnDown->Hide();
		}
		else
		{
			Show();
			m_pkThumbButton->Show();
			m_pkArrowBnUp->Show();
			m_pkArrowBnDown->Show();
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

			float fProcentAvMax;

			int  scroll_pos, bn_size;

			if(m_bHorzintal == false) // om det �r en vertikal scrollbar
			{
				bn_size = m_pkArrowBnUp->GetScreenRect().Height();

				rcArea.Top += bn_size;
				rcArea.Bottom -= bn_size;

				int size = rcArea.Height();
				int y = rcButton.Top - rcArea.Top;

				fProcentAvMax = (float) y / (float) size;

				m_iScrollPosYBefore = (int)m_nPos;
				scroll_pos = m_iScrollPosYBefore;
			}
			else
			{
				bn_size = m_pkArrowBnUp->GetScreenRect().Width();

				rcArea.Left += bn_size;
				rcArea.Right -= bn_size;

				int size = rcArea.Width();
				int x = rcButton.Left - rcArea.Left;

				fProcentAvMax = (float) x / (float) size;

				m_iScrollPosXBefore = (int)m_nPos;
				scroll_pos = m_iScrollPosXBefore;
			}

			//static int m_iScrollPosBefore;
			
			m_nPos = (unsigned int) (fProcentAvMax * (float)(m_nMax-m_nMin+1));

			int change = abs(scroll_pos-(int)m_nPos);

			if(m_nPos < (unsigned int) scroll_pos)
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

	const int SCROLLBUTTONHEIGHT = 16;
	
	if(m_bHorzintal)
	{
		m_usThumbSize = GetWndRect().Height();

		rcUp.Left	= 0;
		rcUp.Top		= 0;
		rcUp.Right	= SCROLLBUTTONHEIGHT;
		rcUp.Bottom	= GetWndRect().Height();

		rcDown.Left		= GetWndRect().Width()-SCROLLBUTTONHEIGHT;
		rcDown.Top		= 0;
		rcDown.Right	= GetWndRect().Width();
		rcDown.Bottom	= GetWndRect().Height();

		rcThumb.Left	= SCROLLBUTTONHEIGHT;
		rcThumb.Top		= 0;
		rcThumb.Right	= SCROLLBUTTONHEIGHT+m_usThumbSize;
		rcThumb.Bottom	= SCROLLBUTTONHEIGHT;
	}
	else
	{
		m_usThumbSize = GetWndRect().Width()-(SCROLLBUTTONHEIGHT*2);

		rcUp.Left	= 0;
		rcUp.Top		= 0;
		rcUp.Right	= GetWndRect().Width();
		rcUp.Bottom	= SCROLLBUTTONHEIGHT;

		rcDown.Left		= 0;
		rcDown.Top		= GetWndRect().Height()-SCROLLBUTTONHEIGHT;
		rcDown.Right	= GetWndRect().Width();
		rcDown.Bottom	= GetWndRect().Height();

		rcThumb.Left	= 0;
		rcThumb.Top		= SCROLLBUTTONHEIGHT;
		rcThumb.Right	= m_usThumbSize;
		rcThumb.Bottom	= SCROLLBUTTONHEIGHT+m_usThumbSize;
	}

	m_pkThumbButton = new ZGuiButton(rcThumb, this, true, SCROLLTHUMB_ID);
	m_pkThumbButton->SetInternalControlState(true);

	m_pkArrowBnUp = new ZGuiButton(rcUp, this, true, SCROLLUP_ID);
	m_pkArrowBnUp->SetInternalControlState(true);

	m_pkArrowBnDown = new ZGuiButton(rcDown, this, true, SCROLLDOWN_ID);
	m_pkArrowBnDown->SetInternalControlState(true);

	//SetScrollInfo(0,100,0.15f,0);
}

void ZGuiScrollbar::SetZValue(int iValue)
{
	ZGuiWnd::SetZValue(iValue);
	m_pkThumbButton->SetZValue(iValue-1);
	m_pkArrowBnUp->SetZValue(iValue-2);
	m_pkArrowBnDown->SetZValue(iValue-3);
}

void ZGuiScrollbar::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC( (ZGuiSkin**)&m_pkSkin, string("Scrollbar")) );
	
	int iStart = pkSkinDesc.size(); 
	m_pkThumbButton->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Scrollbar: ");

	////////////////
	// Ny kod 04-04-13
	iStart = pkSkinDesc.size(); 
	m_pkArrowBnUp->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Scrollbar: Top: ");

	iStart = pkSkinDesc.size(); 
	m_pkArrowBnDown->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Scrollbar: Bottom: ");
	////////////////

}

void ZGuiScrollbar::Resize(int iWidth, int iHeight, bool bChangeMoveArea)
{
	Rect rcMoveArea, rc;
	const int SCROLLBUTTONHEIGHT = 16;

	if(iWidth == -1) iWidth = GetScreenRect().Width();
	if(iHeight == -1) iHeight = GetScreenRect().Height();

	ZGuiWnd::Resize(iWidth, iHeight, bChangeMoveArea);
	
	if(m_bHorzintal)
	{
		rc = m_pkArrowBnDown->GetScreenRect();

		m_pkArrowBnDown->SetPos(GetScreenRect().Width()-
			SCROLLBUTTONHEIGHT,0, false, true);

		rcMoveArea = GetScreenRect();
		rcMoveArea.Left += SCROLLBUTTONHEIGHT;
		rcMoveArea.Right -= SCROLLBUTTONHEIGHT;
		m_pkThumbButton->SetMoveArea(rcMoveArea);
		
		m_pkArrowBnDown->Resize(SCROLLBUTTONHEIGHT, iHeight);
		m_pkArrowBnUp->Resize(SCROLLBUTTONHEIGHT, iHeight);
		m_pkThumbButton->Resize(iHeight, iHeight, false);
		m_pkThumbButton->SetPos(0,0);
	}
	else
	{
		rc = m_pkArrowBnDown->GetScreenRect();

		m_pkArrowBnDown->SetPos(0,GetScreenRect().Height()-
			SCROLLBUTTONHEIGHT, false, true);

		rcMoveArea = GetScreenRect();
		rcMoveArea.Top += SCROLLBUTTONHEIGHT;
		rcMoveArea.Bottom -= SCROLLBUTTONHEIGHT;
		m_pkThumbButton->SetMoveArea(rcMoveArea);
		
		m_pkArrowBnDown->Resize(iWidth, SCROLLBUTTONHEIGHT);
		m_pkArrowBnUp->Resize(iWidth, SCROLLBUTTONHEIGHT);

		m_pkThumbButton->Resize(iWidth, iWidth, false);
		m_pkThumbButton->SetPos(0,0);
	}
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
	m_bHorzintal = !m_bHorzintal;

	int max_size = max(GetScreenRect().Width(),GetScreenRect().Height());
	int min_size = min(GetScreenRect().Width(),GetScreenRect().Height());

	if(m_bHorzintal == false)
		Resize(min_size, max_size);
	else
		Resize(max_size, min_size);
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

//bool ZGuiScrollbar::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
//{
//	ZGuiWnd::Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//
//	m_pkThumbButton->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//	m_pkArrowBnUp->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//	m_pkArrowBnDown->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
//
//	return true;
//}

int ZGuiScrollbar::GetArrowButtonHeight()
{
	return m_pkArrowBnUp->GetScreenRect().Height(); 
}

int ZGuiScrollbar::GetArrowButtonWidth()
{
	return m_pkArrowBnUp->GetScreenRect().Width();
}
