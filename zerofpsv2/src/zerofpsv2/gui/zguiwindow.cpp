// Window.cpp: implementation of the Window class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiwindow.h"
#include "../render/zguirenderer.h"
#include "../basic/zguiskin.h"
#include "../basic/zguifont.h"
#include "zguiresourcemanager.h"
#include "zgui.h"
#include "../engine/i_camera.h"
#include <typeinfo>
#include <math.h>

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
	m_bAcceptRightClicks = false; // om det skall gå att klicka på en knapp med höger musknapp
	m_bUseAlhpaTest = true;

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
	m_afTextColor[0] = m_afTextColor[1] = m_afTextColor[2] = 0.0f;
	m_pkSkin = NULL;
	m_bVisible = bVisible;
	m_bInternalControl = false;
	m_bEnabled = true;
	m_iTabOrderNumber = 0;

	m_pkResMan=static_cast<ZGuiResourceManager*>
		(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));			

	m_pkFont = m_pkResMan->Font("defguifont");
	
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
   m_kClipperArea = Rect(0,0,ZGui::m_iResX,ZGui::m_iResY);

	m_bResizeHorz = true;
	m_bResizeVert = true;

	m_pkCamera = NULL;

   m_iWndAlignment = 0;
   m_iResizeType = 0;
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
		if(x != -1) x += pkParent->m_kArea.Left;
		if(y != -1) y += pkParent->m_kArea.Top;
	}

	int w = m_kArea.Width();
	int h = m_kArea.Height();

	if(x != -1) m_kArea.Left = x;
	if(y != -1) m_kArea.Top = y;
	if(x != -1) m_kArea.Right = x+w;
	if(y != -1) m_kArea.Bottom = y+h;

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

	if(m_pkCamera)
	{
		m_pkCamera->SetViewPort( m_kArea.Left, 
			m_pkGUI->m_iResY-m_kArea.Top-m_kArea.Height(), 
			m_kArea.Width(), m_kArea.Height());
	}

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

   int iNumChildrens = m_kChildList.size();

	// Move childrens
	for( WINit win = m_kChildList.begin();
		 win != m_kChildList.end(); win++)
		 {
          ZGuiWnd* pkWnd = (*win);
			 Rect rc = pkWnd->GetScreenRect();
			 pkWnd->SetPos(rc.Left+iOffsetX, rc.Top+iOffsetY, true, true); 
		 }
}

void ZGuiWnd::SetMoveArea(Rect rc, bool bFreeMovement)
{

	m_kMoveArea = rc;

	ZGuiWnd* pkParent = GetParent();

	if(pkParent != NULL && bFreeMovement == false)
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

void ZGuiWnd::SetRenderTarget(I_Camera* pkCam)
{ 
	m_pkCamera = pkCam;

	if(m_pkCamera)
	{
		m_pkCamera->SetViewPort( m_kArea.Left, 
			m_pkGUI->m_iResY-m_kArea.Top-m_kArea.Height(), 
			m_kArea.Width(), m_kArea.Height());
	}
}


bool ZGuiWnd::Render(ZGuiRender* pkRenderer)
{
	if(m_bVisible == false)
		return true;

	pkRenderer->SetSkin(m_pkSkin); 

	pkRenderer->RenderQuad(m_kArea);
	pkRenderer->RenderBorder(m_kArea);

	if(m_pkCamera) 
	{
		pkRenderer->EndRender(); 
			m_pkCamera->RenderView(); 
		pkRenderer->StartRender(false);
	}

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

				if((*w)->m_pkCamera) 
				{
					pkRenderer->EndRender(); 
						(*w)->m_pkCamera->RenderView(); 
					pkRenderer->StartRender(false);
				}

				if((*w)->m_bUseClipper)
					pkRenderer->EnableClipper(false);
			}

	return true;
	
}

void ZGuiWnd::SetTextClr(unsigned char ucR, unsigned char ucG, unsigned char ucB)
{
	m_afTextColor[0] = (float) ucR / 255.0f;
	m_afTextColor[1] = (float) ucG / 255.0f;
	m_afTextColor[2] = (float) ucB / 255.0f;
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
		if(m_pkGUI == NULL || m_pkGUI->m_bDisableAlphatest)
			return this;

		if(m_bUseAlhpaTest==false)
			return this;

		if(m_pkGUI->ClickedWndAlphaTex(x,y,this) == true)
			return this;
	}
	else
	{
		if(pkFind)
		{
			if(pkFind->GetScreenRect().Inside(x,y) && pkFind->IsVisible())
			{
				if(!m_bUseClipper || pkFind->m_kClipperArea.Inside(x,y))
				{
					if(m_pkGUI == NULL || m_pkGUI->m_bDisableAlphatest)
						return pkFind;

					if(m_bUseAlhpaTest==false)
						return pkFind;

					if(m_pkGUI->ClickedWndAlphaTex(x,y,pkFind) == true)
						return pkFind;
				}
			}
		}
	}

	return NULL;
}

void ZGuiWnd::SetSkin(ZGuiSkin* pkSkin)
{
	m_pkSkin = pkSkin;
}

void ZGuiWnd::SetSkin(const char* szSkinType, ZGuiSkin* pkSkin)
{

/*	vector<SKIN_DESC> vkSkinDesc;
	GetWndSkinsDesc(vkSkinDesc);

	for(unsigned int i=0; i<vkSkinDesc.size(); i++)
		if(strcmp(szSkinType, vkSkinDesc[i].second.c_str()) == 0)
		{
			pkSkin = vkSkinDesc[i].first;
			break;
		}*/

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
			m_pkFont = m_pkResMan->Font("defguifont"); //pkGui->GetBitmapFont("defguifont");

		int usTextLength = 0;

		if(m_pkFont != NULL)
			usTextLength = m_pkFont->GetLength(szText);

		int usFullsize = usTextLength;

      int height = GetScreenRect().Height();

      if(height < m_pkFont->m_iRowHeight)
         height = m_pkFont->m_iRowHeight;
		
		Resize(usFullsize,height); 
	}

	ZGuiWnd* pkParent = GetParent();

	if(pkParent)
	{
		const type_info& t = typeid(*pkParent);
		const type_info& t2 = typeid(*this);

		if(t==typeid(ZGuiTabCtrl) && t2 == typeid(ZGuiWnd))
		{
			ZGuiTabCtrl* pkTabCtrl = ((ZGuiTabCtrl*)pkParent);
			pkTabCtrl->OnChangeWndText(this);
		}
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
Rect ZGuiWnd::GetScreenRect(bool bIncBorder)	
{
	if(!bIncBorder)
		return m_kArea;
	else
	{
		if(GetSkin())
			return m_kArea.Contract(-GetSkin()->m_unBorderSize);
		else
			return m_kArea;
	}
}

void ZGuiWnd::Resize(int Width, int Height, bool bChangeMoveArea)
{
	if(Width != -1)
		m_kArea.Right = m_kArea.Left + Width;

	if(Height != -1)
		m_kArea.Bottom = m_kArea.Top + Height;

	if(bChangeMoveArea)
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
	else
	if(t==typeid(ZGuiTreebox))
		strType = "Treebox";

	pkSkinDesc.push_back( SKIN_DESC((ZGuiSkin**)&m_pkSkin, strType) );
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

void ZGuiWnd::SetClipperArea(Rect rc, bool bEnable)
{
	m_kClipperArea = rc;
	m_bUseClipper = bEnable;
}

void ZGuiWnd::SetResizeFlags(bool bHorz, bool bVert)
{
	m_bResizeHorz = bHorz;
	m_bResizeVert = bVert;
}

const bool ZGuiWnd::IsVisible(bool bSearchParents)	
{
   if(bSearchParents)
   {
	   if(m_bVisible == false)
	   	return false;

	   ZGuiWnd* pkParent = GetParent();

	   if(pkParent)
	   	return pkParent->IsVisible();
   }
	
	return m_bVisible; 
}








