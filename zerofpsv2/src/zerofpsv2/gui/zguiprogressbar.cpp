// zguislider.cpp: implementation of the ZGuiProgressbar class.
//
//////////////////////////////////////////////////////////////////////

#include "zgui.h"
#include "zguilabel.h"
#include "zguiprogressbar.h"
#include "../render/zguirenderer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiProgressbar::ZGuiProgressbar(Rect kRect, ZGuiWnd* pkParent, bool bVisible, int iID, 
											int iMin, int iMax, ProgressbarDir eDir,
											ProgressbarTextOrientation eTextOrientation) 
											: ZGuiWnd(kRect, pkParent, bVisible, iID)
{
	m_iMin = iMin;
	m_iMax = iMax;
	m_iPos = 0;

	m_eDir = eDir;
	m_eTextOrient = eTextOrientation;

	m_bShowText = true;

	CreateInternalControls();
}

ZGuiProgressbar::~ZGuiProgressbar()
{
	delete m_pkLabelFront;
	delete m_pkLabelBkgrd;
}

void ZGuiProgressbar::CreateInternalControls()
{
	Rect rc(0,0,GetScreenRect().Width(),GetScreenRect().Height());
	m_pkLabelFront = new ZGuiLabel(rc,this,true,0);
	m_pkLabelBkgrd = new ZGuiLabel(rc,this,true,0);
}

void ZGuiProgressbar::SetPos(int iPos)
{
	if(iPos > m_iMax)
		iPos = m_iMax;

	if(iPos < m_iMin)
		iPos = m_iMin;

	float f;
	
	switch(m_eDir)
	{
	case PBDIR_LEFT_TO_RIGHT:
		f =(float)GetScreenRect().Width()*((float)iPos/(float)(m_iMax-m_iMin));
		m_pkLabelFront->Resize((int)f,-1,true);
		break;

	case PBDIR_RIGHT_TO_LEFT:
		f =(float)GetScreenRect().Width()-(float)GetScreenRect().Width()*((float)iPos/(float)(m_iMax-m_iMin));
		m_pkLabelFront->Resize((int)f,-1,true);
		break;

	case PBDIR_TOP_TO_BOTTOM:
		f =(float)GetScreenRect().Height()-(float)GetScreenRect().Height()*((float)iPos/(float)(m_iMax-m_iMin));
		m_pkLabelFront->Resize(-1,(int)f,true);
		break;

	case PBDIR_BOTTOM_TO_TOP:
		f =(float)GetScreenRect().Height()*((float)iPos/(float)(m_iMax-m_iMin));
		m_pkLabelFront->Resize(-1,(int)f,true);
		break;
	}

	m_iPos = iPos;
}

int ZGuiProgressbar::GetPos()
{
	return m_iPos;
}

void ZGuiProgressbar::GetRange(int& riMin, int& riMax)
{
	riMin = m_iMin;
	riMax = m_iMax;
}

void ZGuiProgressbar::SetRange(int iMin, int iMax)
{
	m_iMax = iMin < iMax ? iMin : iMax;
	m_iMax = iMax > iMin ? iMax : iMin;
}

void ZGuiProgressbar::SetDir(ProgressbarDir eDir)
{
	m_eDir = eDir;
}

ProgressbarDir ZGuiProgressbar::GetDir()
{
	return m_eDir;
}

void ZGuiProgressbar::SetSkin(ZGuiSkin* pkFront, ZGuiSkin* pkBack)
{ 
	if(pkBack)
		m_pkLabelBkgrd->SetSkin(pkBack);

	if(pkFront)
		m_pkLabelFront->SetSkin(pkFront);
}

void ZGuiProgressbar::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	int iStart = pkSkinDesc.size(); 
	m_pkLabelBkgrd->GetWndSkinsDesc(pkSkinDesc);
	m_pkLabelFront->GetWndSkinsDesc(pkSkinDesc);
	
	pkSkinDesc[iStart].second = "Progressbar: Back";
	pkSkinDesc[iStart+1].second = "Progressbar: Front";
}

bool ZGuiProgressbar::Render( ZGuiRender* pkRenderer )
{
	m_pkLabelBkgrd->Render(pkRenderer);
	m_pkLabelFront->Render(pkRenderer);

	static float oka=100;
	oka-=0.1f;
	if(oka<0)
		oka=100;
	SetPos((int)oka);

	if(m_pkFont && m_bShowText)
	{
		char text[128];

		if(m_strText && strlen(m_strText) > 0)
			sprintf(text, "%s = %i/%i\n", m_strText, m_iPos, m_iMax-m_iMin);
		else
			sprintf(text, "%i/%i\n", m_iPos, m_iMax-m_iMin);
		
		int text_h = m_pkFont->m_iRowHeight;
		int text_w = m_pkFont->GetLength(text);

		Rect rcTextRect = GetScreenRect();

		switch(m_eTextOrient)
		{
		case PBTEXTORIENT_CENTER:
			rcTextRect.Top += (rcTextRect.Bottom-rcTextRect.Top)/2-(text_h/2);
			rcTextRect.Bottom = rcTextRect.Top + text_h;
			rcTextRect.Left += (rcTextRect.Right-rcTextRect.Left)/2-(text_w/2);
			rcTextRect.Right = rcTextRect.Left + text_w;
			break;

		case PBTEXTORIENT_LEFT:
			rcTextRect.Top += (rcTextRect.Bottom-rcTextRect.Top)/2-(text_h/2);
			rcTextRect.Bottom = rcTextRect.Top + text_h;
			rcTextRect.Right = rcTextRect.Left;
			rcTextRect.Left -= text_w;
			break;

		case PBTEXTORIENT_RIGHT:
			rcTextRect.Top += (rcTextRect.Bottom-rcTextRect.Top)/2-(text_h/2);
			rcTextRect.Bottom = rcTextRect.Top + text_h;
			rcTextRect.Left = rcTextRect.Right;
			rcTextRect.Right += text_w;
			break;

		case PBTEXTORIENT_OVER:
			rcTextRect.Left += (rcTextRect.Right-rcTextRect.Left)/2-(text_w/2);
			rcTextRect.Right = rcTextRect.Left + text_w;
			rcTextRect.Bottom = rcTextRect.Top;
			rcTextRect.Top -= text_h;
			break;

		case PBTEXTORIENT_UNDER:
			rcTextRect.Left += (rcTextRect.Right-rcTextRect.Left)/2-(text_w/2);
			rcTextRect.Right = rcTextRect.Left + text_w;
			rcTextRect.Top = rcTextRect.Bottom;
			rcTextRect.Bottom += text_h;
			break;
		}

		pkRenderer->SetFont(m_pkFont);		
		pkRenderer->RenderText(text, rcTextRect, -1, m_afTextColor);
	}

	return true;
}

void ZGuiProgressbar::SetTextOrientation(ProgressbarTextOrientation eOrient)
{
	m_eTextOrient = eOrient;	
}

ProgressbarTextOrientation ZGuiProgressbar::GetTextOrientation()
{
	return m_eTextOrient;
}