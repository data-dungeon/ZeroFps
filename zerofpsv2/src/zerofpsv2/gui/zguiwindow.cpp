// Window.cpp: implementation of the Window class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiwindow.h"
#include "../render/zguirenderer.h"
#include "../basic/zguiskin.h"
#include "../basic/zguifont.h"
#include "zguiresourcemanager.h"
#include "zgui.h"
#include <typeinfo>

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
	m_iResolutionX = ZGui::m_iResX;
	m_iResolutionY = ZGui::m_iResY;

	m_ulFlags = 0;
	m_iZValue = 0;
	m_pkGUI = NULL;
	m_pkCallback = NULL;
	m_kArea = kRectangle; 
	m_iID = iID;
	
	strcpy(m_szName, "None");

	m_strText = NULL;
	m_iTextLength = 0;
	m_pkSkin = NULL;
	m_bVisible = bVisible;
	m_bInternalControl = false;
	m_bEnabled = true;
	m_pkFont = NULL;
	m_iTabOrderNumber = 0;

/*	m_pkGuiMan=static_cast<ZGuiResourceManager*>
		(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));	*/		
	
	m_pkParent = pkParent;
	if(pkParent != NULL)
	{
		SetParent(pkParent);
		pkParent->AddChild(this);
		m_kArea += pkParent->GetScreenRect();
	}

	m_kMoveArea = m_kArea;

	SetWindowFlag(WF_CANHAVEFOCUS); // fönster har focus by default
	SetWindowFlag(WF_TOPWINDOW); // ett vanligt fönster kan vara ett top fönster

	//afBkColorBuffer[0]=afBkColorBuffer[1]=afBkColorBuffer[2]=-1.0f;

	m_bUseClipper = false;
	m_kClipperArea = Rect(0,0,800,600);
}

ZGuiWnd::~ZGuiWnd()
{
	// Om detta fönster har en förälder,
	// låt denna förälder ta bort detta barn.
	
	ZGuiWnd* pkParent = GetParent();

	while(pkParent)
	{
		if(pkParent)
		{
			pkParent->RemoveChild(this);
			pkParent->SetFocus();
			ZGuiWnd::m_pkFocusWnd = pkParent;
			pkParent = pkParent->GetParent();
		}
		else
		{
			break;
		}
	}

	// Nollställ eventuella statiska pekare.
	ResetStaticClickWnds(this);
}

// Används tex. för att lägga till en textbox till en listbox.
bool ZGuiWnd::AddChild( ZGuiWnd *pkWindow )
{
	pkWindow->SetParent( this );
	m_kChildList.push_back( pkWindow );
    return true;
}

bool ZGuiWnd::RemoveChild( ZGuiWnd *pkWindow )
{
	for( WINit w = m_kChildList.begin(); w != m_kChildList.end(); w++)
		if(pkWindow == (*w))
		{
			ResetStaticClickWnds((*w));
			m_kChildList.erase(w);
			return true;
		}

    return false;
}

bool ZGuiWnd::SetParent( ZGuiWnd *pkWindow )
{
    m_pkParent = pkWindow;
    return true;
}

ZGuiWnd *ZGuiWnd::GetParent(bool bRootParent)
{
	if(bRootParent==false)
		return m_pkParent;
	else
	{
		ZGuiWnd* pkParent = GetParent(false);
		ZGuiWnd* next = pkParent;
		while(next != NULL) 
		{
			pkParent = next;
			next = next->GetParent(false);		
		}
		return pkParent;
	}
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
	if(pkParent && bScreenSpace == false)
	{
		x += pkParent->m_kArea.Left;
		y += pkParent->m_kArea.Top;
	}

	int w = m_kArea.Width();
	int h = m_kArea.Height();

	m_kArea.Left = x;
	m_kArea.Top = y;
	m_kArea.Right = x+w;
	m_kArea.Bottom = y+h;

	UpdatePos(iPrevPosX, iPrevPosY, w, h, bFreeMovement);

/*	if(m_bUseClipper)
	{
		int x_offset = m_kArea.Left-iPrevPosX;
		int y_offset = m_kArea.Top-iPrevPosY;

		m_kClipperArea.Left += x_offset;
		m_kClipperArea.Right += x_offset;
		m_kClipperArea.Top += y_offset;
		m_kClipperArea.Bottom += y_offset;
	}*/

	return true;
}

void ZGuiWnd::UpdatePos(int iPrevPosX, int iPrevPosY, int w, int h, 
						bool bFreeMovement)
{
	if(bFreeMovement == false)
	{
		// Don´t move outside the windows move area
		if(m_kArea.Left < m_kMoveArea.Left)
		{
			m_kArea.Left = m_kMoveArea.Left;
			m_kArea.Right = m_kArea.Left+w;
		}
		if(m_kArea.Right > m_kMoveArea.Right)
		{
			m_kArea.Right = m_kMoveArea.Right;
			m_kArea.Left = m_kArea.Right-w; 
		}
		if(m_kArea.Top < m_kMoveArea.Top)
		{
			m_kArea.Top = m_kMoveArea.Top;
			m_kArea.Bottom = m_kArea.Top+h;
		}
		if(m_kArea.Bottom > m_kMoveArea.Bottom)
		{
			m_kArea.Bottom = m_kMoveArea.Bottom;
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

	if(m_pkGUI)
	{
		int curr_res_x, curr_res_y;
		m_pkGUI->GetResolution(curr_res_x, curr_res_y);
		Rescale(m_iResolutionX, m_iResolutionY, curr_res_x, curr_res_y);
	}

	pkRenderer->SetSkin(m_pkSkin); 

	pkRenderer->RenderQuad(m_kArea);
	pkRenderer->RenderBorder(m_kArea);

	// Render childrens back to front
	for( WINrit w = m_kChildList.rbegin(); w != m_kChildList.rend(); w++)
			if( (*w)->m_bVisible == true)
			{
				if((*w)->m_bUseClipper)
				{
					pkRenderer->EnableClipper(true); 
					pkRenderer->SetClipperArea((*w)->m_kClipperArea);
				}

				(*w)->Render( pkRenderer );

				if((*w)->m_bUseClipper)
					pkRenderer->EnableClipper(false);
			}

	return true;
	
}

ZGuiWnd* ZGuiWnd::Find(int x, int y)
{
	if(m_bVisible == false || m_bEnabled == false)
		return NULL;

	ZGuiWnd *pkFind = NULL;

	WINit child = m_kChildList.begin();
	while( (pkFind == NULL) && (child!=m_kChildList.end()) )
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
			if(pkFind->GetScreenRect().Inside(x,y) && pkFind->IsVisible())
				return pkFind;
		}
	}

	return NULL;
}

void ZGuiWnd::SetSkin(ZGuiSkin* pkSkin)
{
	m_pkSkin = pkSkin;
}

void ZGuiWnd::SetFont(ZGuiFont* pkFont)
{
	m_pkFont = pkFont;
}

void ZGuiWnd::SetGUI(ZGui* pkGui)
{
	m_pkGUI = pkGui;
}

void ZGuiWnd::SetText(char* szText, bool bResizeWnd)
{
	if(szText == NULL)
	{
		delete[] m_strText;
		m_iTextLength = 1;
		m_strText = new char[m_iTextLength];
		m_strText[0] = '\0';
		return;
	}

	if(m_strText != NULL)
	{
		delete[] m_strText;
		m_iTextLength = 0;
	}

	m_iTextLength = strlen(szText)+1;
	m_strText = new char[m_iTextLength+1];
	strcpy(m_strText, szText);

	if(bResizeWnd)
	{
		ZGui* pkGui = GetGUI();
		if(!m_pkFont && pkGui)
			m_pkFont = pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT);

		int usTextLength = 0;

		if(m_pkFont != NULL)
			usTextLength = m_pkFont->GetLength(szText);

		int usFullsize = usTextLength;
		
		Resize(usFullsize,16); 
	}
}

// Get the windows area, relative to it´s parent.
// ie. If a window at coordinate (500,500) is a
// child to a window at coordinate (300, 300),
// then the windows WndRect is (200,200,x,x)
Rect ZGuiWnd::GetWndRect()		
{
	Rect kArea = m_kArea;
	if(m_pkParent)
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

void ZGuiWnd::Resize(int Width, int Height, bool bChangeMoveArea)
{
	m_kArea.Right = m_kArea.Left + Width;
	m_kArea.Bottom = m_kArea.Top + Height;

	if(bChangeMoveArea)
		m_kMoveArea = m_kArea;
}


bool ZGuiWnd::Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight)
{
	ZGui* pkGui = GetGUI();
	if(pkGui == NULL)
		return false;

	// Have resolution been changed for this window?
	int curr_res_x, curr_res_y;
	pkGui->GetResolution(curr_res_x, curr_res_y);
	if(m_iResolutionX == curr_res_x && m_iResolutionY == curr_res_y)
		return true; // same, break

	// Store the new resoluion
	m_iResolutionX = iNewWidth;
	m_iResolutionY = iNewHeight;

	float fScaleX = (float) iNewWidth / (float) iOldWidth;
	float fScaleY = (float) iNewHeight / (float) iOldHeight;

	Rect rc, rcNew;
	float fOldXprocentAvSkarm, fOldYprocentAvSkarm;

	// 
	// Change area
	// 
	rc = m_kArea;
	
	fOldXprocentAvSkarm = (float) rc.Left / (float) iOldWidth;
	fOldYprocentAvSkarm = (float) rc.Top / (float) iOldHeight;

	rcNew = Rect(
		(int) (fOldXprocentAvSkarm * (float) iNewWidth ),
		(int) (fOldYprocentAvSkarm * (float) iNewHeight), 0,0);

	rcNew.Right  =  rcNew.Left + (int) (fScaleX * (float) rc.Width()); 
	rcNew.Bottom =  rcNew.Top  + (int) (fScaleY * (float) rc.Height());
	
	m_kArea = rcNew;

	// 
	// Change move area
	// 
	rc = m_kMoveArea;
	
	fOldXprocentAvSkarm = (float) rc.Left / (float) iOldWidth;
	fOldYprocentAvSkarm = (float) rc.Top / (float) iOldHeight;

	rcNew = Rect(
		(int) (fOldXprocentAvSkarm * (float) iNewWidth ),
		(int) (fOldYprocentAvSkarm * (float) iNewHeight),0,0);

	rcNew.Right  =  rcNew.Left + (int) (fScaleX * (float) rc.Width()); 
	rcNew.Bottom =  rcNew.Top  + (int) (fScaleY * (float) rc.Height());
	
	m_kMoveArea = rcNew;

	// Also scale childrens
	for( WINit win = m_kChildList.begin();
		 win != m_kChildList.end(); win++)
		 {
			(*win)->Rescale(iOldWidth, iOldHeight, iNewWidth, iNewHeight);
		 }

	return true;
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
	m_kChildList.sort(SortZCmp);
}

void ZGuiWnd::SetWindowFlag(unsigned long ulValue)
{
	m_ulFlags |= ulValue;
}

void ZGuiWnd::RemoveWindowFlag(unsigned long ulValue)
{
	m_ulFlags &= ~ulValue;
}

bool ZGuiWnd::GetWindowFlag(unsigned long ulValue)
{
	return ((m_ulFlags & ulValue) == ulValue) ? true : false;
}

void ZGuiWnd::Hide()
{
	m_bVisible = false;
}

void ZGuiWnd::Show()
{
	m_bVisible = true;  
}

void ZGuiWnd::ResetStaticClickWnds(ZGuiWnd* pkWnd)
{
	if(pkWnd == ZGuiWnd::m_pkPrevWndUnderCursor)
		ZGuiWnd::m_pkPrevWndUnderCursor = NULL;
	if(pkWnd == ZGuiWnd::m_pkPrevWndClicked)
		ZGuiWnd::m_pkPrevWndClicked = NULL;
	if(pkWnd == ZGuiWnd::m_pkFocusWnd)
		ZGuiWnd::m_pkFocusWnd = NULL;
	if(pkWnd == ZGuiWnd::m_pkWndUnderCursor)
		ZGuiWnd::m_pkWndUnderCursor = NULL;
	if(pkWnd == ZGuiWnd::m_pkWndClicked)
		ZGuiWnd::m_pkWndClicked = NULL;
}

void ZGuiWnd::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	string strType = "Error";
	const type_info& t = typeid(*this);

	if(t==typeid(ZGuiTextbox))
		strType = "TextBox";
	else
	if(t==typeid(ZGuiScrollbar))
		strType = "Scrollbar";
	else
	if(t==typeid(ZGuiRadiobutton))
		strType = "RadioButton";
	else
	if(t==typeid(ZGuiListbox))
		strType = "ListBox";
	else
	if(t==typeid(ZGuiLabel))
		strType = "Label";
	else
	if(t==typeid(ZGuiCheckbox))
		strType = "CheckBox";
	else
	if(t==typeid(ZGuiButton))
		strType = "Button";
	else
	if(t==typeid(ZGuiWnd))
		strType = "Window";
	else
	if(t==typeid(ZGuiCombobox))
		strType = "ComboBox";

	pkSkinDesc.push_back( SKIN_DESC(m_pkSkin, strType) );
}

void ZGuiWnd::Enable()
{
	m_bEnabled = true;
}

void ZGuiWnd::Disable()
{
	m_bEnabled = false;
}

///////////////////////////////////////////////////////////////////////////////
// Name: Copy
// Description: Deep copy non unique data from one window to another.
//				Function is virtual and should be overloaded!
//
void ZGuiWnd::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	vector<SKIN_DESC> kDstSkins;
	vector<SKIN_DESC> kSrcSkins;
	
	GetWndSkinsDesc(kDstSkins);
	pkSrc->GetWndSkinsDesc(kSrcSkins);
	
	for(unsigned int i=0; i<kDstSkins.size(); i++)
	{
		*kDstSkins[i].first = *kSrcSkins[i].first;
	}

	m_ulFlags = pkSrc->m_ulFlags;
	m_bEnabled = pkSrc->m_bEnabled;
	m_iTextLength = pkSrc->m_iTextLength;
	
	if(pkSrc->m_strText)
	{
		m_strText = new char[strlen(pkSrc->m_strText)+1];
		strcpy(m_strText, pkSrc->m_strText);
	}

	m_pkFont = pkSrc->m_pkFont;
	m_kArea = pkSrc->m_kArea;
	m_kMoveArea = pkSrc->m_kMoveArea;
	//m_pkGuiMan = pkSrc->m_pkGuiMan;
	m_pkParent = pkSrc->m_pkParent;
	m_pkGUI =pkSrc->m_pkGUI;
	m_pkCallback = pkSrc->m_pkCallback;
}

void ZGuiWnd::SetClipperArea(Rect rc)
{
	m_kClipperArea = rc;
}
