// Window.cpp: implementation of the Window class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiwindow.h"
#include "../../render/zguirenderer.h"
#include "../../basic/zguiskin.h"

ZGuiWnd* ZGuiWnd::m_pkPrevWndUnderCursor = NULL;
ZGuiWnd* ZGuiWnd::m_pkPrevWndClicked = NULL;
ZGuiWnd* ZGuiWnd::m_pkFocusWnd = NULL;

ZGuiWnd* ZGuiWnd::m_pkWndClicked = NULL;
ZGuiWnd* ZGuiWnd::m_pkWndUnderCursor = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiWnd::ZGuiWnd(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID)
{
	m_iZValue = 0;
	m_pkGUI = NULL;
	m_pkCallback = NULL;
	m_kArea = kRectangle; 
	m_iID = iID;
	m_strText = NULL;
	m_iTextLength = 0;
	m_pkSkin = NULL;
	m_pkTextSkin = NULL;
	m_iTextMaskTexture = -1;
	m_iBkMaskTexture = -1;
	m_iBorderMaskTexture = -1;
	m_bVisible = bVisible;
	m_bInternalControl = false;
	m_bEnabled = true;
	
	m_pkParent = pkParent;
	if(pkParent != NULL)
	{
		SetParent(pkParent);
		pkParent->AddChild(this);

		int bsz = 0;
		if(m_pkSkin)
			bsz = m_pkSkin->m_unBorderSize;

		m_kArea += pkParent->GetScreenRect();

/*		// Får inte befinna sig utanför sin föräldrers gränser.
		if(m_kArea.Bottom + bsz > pkParent->GetScreenRect().Bottom)
		{
			int height = m_kArea.Height(); 
			m_kArea.Bottom = pkParent->GetScreenRect().Bottom;
			m_kArea.Top = m_kArea.Bottom-height;
		}
		if(m_kArea.Right + bsz > pkParent->GetScreenRect().Right)
		{
			int width = m_kArea.Width(); 
			m_kArea.Right = pkParent->GetScreenRect().Right;
			m_kArea.Left = m_kArea.Right-width;
		}*/
	}

/*	if(iID != 520 && iID != 620)*/
		m_kMoveArea = m_kArea;
}

ZGuiWnd::~ZGuiWnd()
{
	ZGuiWnd* pkParent = GetParent();
	if(pkParent)
		pkParent->RemoveChild(this);

	for( WINit w = m_kChildList.begin();
		 w != m_kChildList.end(); w++)
		 {
			if((*w) != NULL)
			{
				delete (*w);
				(*w) = NULL;
			}
		 }

	m_kChildList.clear();
}

// Används tex. för att lägga till en textbox
// till en listbox.
bool ZGuiWnd::AddChild( ZGuiWnd *pkWindow )
{
	pkWindow->SetParent( this );
	m_kChildList.push_back( pkWindow );
    return true;
}

bool ZGuiWnd::RemoveChild( ZGuiWnd *pkWindow )
{
	for( WINit w = m_kChildList.begin();
		 w != m_kChildList.end(); w++)
		 {
			 if(pkWindow == (*w))
			 {
				 m_kChildList.erase(w);
				 return true;
			 }
		 }

    return false;
}

bool ZGuiWnd::SetParent( ZGuiWnd *pkWindow )
{
    m_pkParent = pkWindow;
    return true;
}

ZGuiWnd *ZGuiWnd::GetParent()
{
	return m_pkParent;
}

void ZGuiWnd::GetChildrens(list<ZGuiWnd*>& kList)
{
	kList = m_kChildList;
}

bool ZGuiWnd::SetPos(int x, int y, bool bScreenSpace, bool bFreeMovement)
{
	int iPrevPosX = m_kArea.Left, 
		iPrevPosY = m_kArea.Top;

	ZGuiWnd* pkParent = GetParent();
	if(pkParent != NULL && bScreenSpace == false)
	{
		x += GetParent()->m_kArea.Left;
		y += GetParent()->m_kArea.Top;
	}

	int w = m_kArea.Width();
	int h = m_kArea.Height();

	m_kArea.Left = x;
	m_kArea.Top = y;
	m_kArea.Right = x+w;
	m_kArea.Bottom = y+h;

	UpdatePos(iPrevPosX, iPrevPosY, w, h, bFreeMovement);

	return true;
}

void ZGuiWnd::UpdatePos(int iPrevPosX, int iPrevPosY, int w, int h, bool bFreeMovement)
{
	if(bFreeMovement == false)
	{
		int bsz = 0;//m_kSkin.m_unBorderSize;

		// Don´t move outside the windows move area
		if(m_kArea.Left < m_kMoveArea.Left+bsz)
		{
			m_kArea.Left = m_kMoveArea.Left+bsz;
			m_kArea.Right = m_kArea.Left+w;
		}
		if(m_kArea.Right > m_kMoveArea.Right-bsz-bsz)
		{
			m_kArea.Right = m_kMoveArea.Right-bsz;
			m_kArea.Left = m_kArea.Right-w; 
		}
		if(m_kArea.Top < m_kMoveArea.Top+bsz)
		{
			m_kArea.Top = m_kMoveArea.Top+bsz;
			m_kArea.Bottom = m_kArea.Top+h;
		}
		if(m_kArea.Bottom > m_kMoveArea.Bottom-bsz)
		{
			m_kArea.Bottom = m_kMoveArea.Bottom-bsz;
			m_kArea.Top = m_kArea.Bottom-h;
		}
	}

	int iOffsetX = m_kArea.Left-iPrevPosX;
	int iOffsetY = m_kArea.Top-iPrevPosY;

	if(bFreeMovement == true)
	{
		m_kMoveArea.Left += iOffsetX;
		m_kMoveArea.Right += iOffsetX;

		m_kMoveArea.Top += iOffsetY;
		m_kMoveArea.Bottom += iOffsetY;
	}

	// Move childrens
	for( WINit win = m_kChildList.begin();
		 win != m_kChildList.end(); win++)
		 {
			 Rect rc = (*win)->GetScreenRect();
			 (*win)->SetPos(rc.Left+iOffsetX, rc.Top+iOffsetY, true, true); 
		 }
}

void ZGuiWnd::SetMoveArea(Rect rc, bool bFreeMovement)
{
	m_kMoveArea = rc;

	ZGuiWnd* pkParent = GetParent();

	if(pkParent != NULL)
	{
/*		m_kMoveArea.Left += m_pkParent->m_kArea.Left;
		m_kMoveArea.Right += m_pkParent->m_kArea.Left;
		m_kMoveArea.Top += m_pkParent->m_kArea.Top;
		m_kMoveArea.Bottom += m_pkParent->m_kArea.Top;*/

		int min_x = pkParent->m_kArea.Left;
		int max_x = pkParent->m_kArea.Right;
		int min_y = pkParent->m_kArea.Top;
		int max_y = pkParent->m_kArea.Bottom;

		if(m_kMoveArea.Left < min_x)
			m_kMoveArea.Left = min_x;
		if(m_kMoveArea.Right > max_x)
			m_kMoveArea.Right = max_x;
		if(m_kMoveArea.Top < min_y)
			m_kMoveArea.Top = min_y;
		if(m_kMoveArea.Bottom > max_y)
			m_kMoveArea.Bottom = max_y;
	}
}

bool ZGuiWnd::Render(ZGuiRender* pkRenderer)
{
	if(m_bVisible == false)
		return true;

	pkRenderer->SetSkin(m_pkSkin); 

	if(m_iBkMaskTexture != -1)
	{
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);
		pkRenderer->RenderQuad(m_kArea, true);
	}
	else
	{
		pkRenderer->RenderQuad(m_kArea);
	}

	if(m_iBorderMaskTexture != -1)
	{
		pkRenderer->SetMaskTexture(m_iBorderMaskTexture);
		pkRenderer->RenderBorder(m_kArea, true);
	}
	else
	{
		pkRenderer->RenderBorder(m_kArea);
	}

	// Render childrens back to front
	for( WINrit w = m_kChildList.rbegin();
		 w != m_kChildList.rend(); w++)
		 {
			if( (*w)->m_bVisible == true)
			{
				(*w)->Render( pkRenderer );
			}
		 }

	return true;
	
}

ZGuiWnd* ZGuiWnd::Find(int x, int y)
{
	if(m_bVisible == false || m_bEnabled == false)
		return NULL;

	ZGuiWnd *pkFind = NULL;

	WINit child = m_kChildList.begin();
	while( (pkFind == NULL)&&(child!=m_kChildList.end()) )
	{
		pkFind = (*child)->Find(x, y);
		child++;
	}

	if( (pkFind==NULL) && m_kArea.Inside(x,y))
	{
		return this;
	}
	else
	{
		if(pkFind)
		{
			if(pkFind->GetScreenRect().Inside(x,y))
				return pkFind;
		}
		
		return NULL;
	}

	return NULL;
}

void ZGuiWnd::SetSkin(ZGuiSkin* pkSkin, int iBkMaskTexture, int iBorderMaskTexture)
{
	m_pkSkin = pkSkin;
	m_iBkMaskTexture = iBkMaskTexture;
	m_iBorderMaskTexture = iBorderMaskTexture;
}

void ZGuiWnd::SetTextSkin(ZGuiSkin* pkSkin, int iMaskTexture)
{
	m_pkTextSkin = pkSkin;
	m_iTextMaskTexture = iMaskTexture;
}

void ZGuiWnd::SetGUI(ZGui* pkGui)
{
	m_pkGUI = pkGui;
}

void ZGuiWnd::SetText(char* strText)
{
	if(strText == NULL)
		return;

	int iLength = strlen(strText)+1;

	if(iLength > 1)
	{
		if(m_strText != NULL)
		{
			delete[] m_strText;
			m_iTextLength = 0;
		}

		m_iTextLength = iLength;
		m_strText = new char[m_iTextLength];
		strcpy(m_strText, strText);
	}
}

// Get the windows area, relative to it´s parent.
// ie. If a window at coordinate (500,500) is a
// child to a window at coordinate (300, 300),
// then the windows WndRect is (200,200,x,x)
Rect ZGuiWnd::GetWndRect()		
{
	Rect kArea = m_kArea;
	if(m_pkParent != NULL)
	{
		kArea.Left	 -= m_pkParent->m_kArea.Left; 
		kArea.Top	 -= m_pkParent->m_kArea.Top;
		kArea.Right	 -= m_pkParent->m_kArea.Left; 
		kArea.Bottom -= m_pkParent->m_kArea.Top;
	}

	return kArea;
}

// Get the real screen area.
Rect ZGuiWnd::GetScreenRect()	
{
	return m_kArea;
}

void ZGuiWnd::Resize(int Width, int Height)
{

	m_kArea.Right = m_kArea.Left + Width;
	m_kArea.Bottom = m_kArea.Top + Height;

	m_kMoveArea = m_kArea;
}

void ZGuiWnd::Move(int dx, int dy, bool bScreenSpace, bool bFreeMovement)
{
	int iPrevPosX = m_kArea.Left, 
		iPrevPosY = m_kArea.Top;

	m_kArea.Left += dx;
	m_kArea.Right += dx;
	m_kArea.Top += dy;
	m_kArea.Bottom += dy;

	m_kMoveArea.Left += dx;
	m_kMoveArea.Right += dx;
	m_kMoveArea.Top += dy;
	m_kMoveArea.Bottom += dy;

	int w = m_kArea.Width();
	int h = m_kArea.Height();

	UpdatePos(iPrevPosX, iPrevPosY, w, h, bFreeMovement);
}

ZGui* ZGuiWnd::GetGUI()
{
	ZGui* pkGui = m_pkGUI;
	ZGuiWnd* pkSearch = this;
	
	while(pkSearch != NULL)
	{
		if(pkGui != NULL)
			return pkGui;

		pkSearch = pkSearch->GetParent();
		if(pkSearch)
			pkGui = pkSearch->m_pkGUI;
	}

	return pkGui;
}

void ZGuiWnd::SetZValue(int iValue)
{
	m_iZValue = iValue;
}

void ZGuiWnd::SortChilds()
{
	m_kChildList.sort(ZValue_Cmp);
}