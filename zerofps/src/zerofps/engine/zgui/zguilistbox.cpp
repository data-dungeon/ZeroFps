// Listbox.cpp: implementation of the Listbox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguilistbox.h"
#include "../../render/zguirenderer.h"
#include "zguiscrollbar.h"
#include "zguibutton.h"
#include "zgui.h"

// Static internal IDs for the scrollbars
const int VERT_SCROLLBAR_ID = 620;
const int HORZ_SCROLLBAR_ID = 621;
const int FONT_SIZE = 12;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiListbox::ZGuiListbox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID, int iItemHeight,
						 ZGuiSkin* pkSkinItem, ZGuiSkin* pkSkinItemSelected, ZGuiSkin* pkSkinItemHighLight) :
	ZGuiControl(kRectangle, pkParent, bVisible, iID)
{
	m_iScrollbarWidth = 20;
	m_bIsMenu = false;
	//kRectangle.Bottom = 50;

	m_pkSelectedItem = NULL;
	m_pkSkinItem = pkSkinItem;
	m_pkSkinItemSelected = pkSkinItemSelected;
	m_pkSkinItemHighLight = pkSkinItemHighLight;

	m_unItemHeight = iItemHeight;

	int w = GetWndRect().Width();
	int h = GetWndRect().Height();
	
	// h=m_unItemHeight*10;//GetWndRect().Height() % m_unItemHeight;
	ZGuiWnd::Resize(w,h);

	CreateInternalControls();

	m_kItemArea = GetWndRect();

	if(m_pkScrollbarVertical != NULL)
		m_kItemArea.Right -= m_pkScrollbarVertical->GetWndRect().Width(); 

	m_unOriginalHeight = GetScreenRect().Height();
}

ZGuiListbox::~ZGuiListbox()
{
	for( itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
	{
		delete (*it);
	}

	m_pkItemList.clear(); 
}

bool ZGuiListbox::Render( ZGuiRender* pkRenderer )
{
	if(!IsVisible())
		return true;

	if(m_iBkMaskTexture > 0)
		pkRenderer->SetMaskTexture(m_iBkMaskTexture);

	pkRenderer->SetSkin(m_pkSkin);
	pkRenderer->RenderQuad(GetScreenRect(),(m_iBkMaskTexture > 0)); 
	pkRenderer->RenderBorder(GetScreenRect(),(m_iBkMaskTexture > 0)); 

	if(m_pkScrollbarVertical->IsVisible())
		m_pkScrollbarVertical->Render( pkRenderer );

	if(m_pkSelectedItem)
		m_pkSelectedItem->Select();

	for( itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
		{
			Rect rc = (*it)->GetButton()->GetWndRect();

			if(m_pkScrollbarVertical->IsVisible() == false)
				(*it)->GetButton()->Resize(rc.Width()+20, rc.Height());

			(*it)->GetButton()->Render( pkRenderer );

			if(m_pkScrollbarVertical->IsVisible() == false)
				(*it)->GetButton()->Resize(rc.Width(), rc.Height());
		}

	return true;
}

void ZGuiListbox::SetItemNormalSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinItem = pkSkin;
}

void ZGuiListbox::SetItemSelectedSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinItemSelected = pkSkin;
}

void ZGuiListbox::SetItemHighLightSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinItemHighLight = pkSkin;
}

void ZGuiListbox::SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
								   ZGuiSkin* pkSkinThumbButtonHighLight)
{
	m_pkScrollbarVertical->SetSkin(pkSkinScrollArea);
	m_pkScrollbarVertical->SetThumbButtonSkins(pkSkinThumbButton, pkSkinThumbButtonHighLight);
}

void ZGuiListbox::CreateInternalControls()
{
	Rect rc = GetWndRect();
	int x = rc.Width()-m_iScrollbarWidth;
	int y = rc.Top;
	int w = m_iScrollbarWidth;
	int h = rc.Height()-rc.Top;

	//h -= (rc.Height() % m_unItemHeight);

	m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,0,x+w,GetScreenRect().Height()),
		this,true,VERT_SCROLLBAR_ID); 

	m_pkScrollbarVertical->SetScrollInfo(0,0,1.0f,0); 
}

bool ZGuiListbox::AddItem(char* strText, unsigned int iID)
{
	ZGuiListitem* pkNewItem;
	
	if(m_bIsMenu == false)
	{
		pkNewItem = new ZGuiListitem(this, strText, iID, 
			m_pkSkinItem, m_pkSkinItemSelected, m_pkSkinItemHighLight/*, 
			m_pkSkinItemFont, m_iTextMaskTexture*/);
		pkNewItem->GetButton()->SetGUI(GetGUI());
		pkNewItem->m_bMenuItem = false;
	}
	else
	{
		pkNewItem = new ZGuiListitem(this, strText, iID, 
			m_pkSkinItem, m_pkSkinItem, m_pkSkinItemSelected/*, 
			m_pkSkinItemFont, m_iTextMaskTexture*/);
		pkNewItem->GetButton()->SetGUI(GetGUI());
		pkNewItem->m_bMenuItem = true;
	}

	m_pkItemList.push_back(pkNewItem);

	UpdateList();

	int iWidth = GetScreenRect().Width();
	int iHeight = GetScreenRect().Height();
	int iNewWidth = FONT_SIZE * (strlen(strText)+1);

	if(iNewWidth > iWidth && m_bIsMenu == true)
	{
		ZGuiWnd::Resize( iNewWidth, iHeight);

		list<ZGuiListitem*>::iterator it;
		for( it = m_pkItemList.begin();
			 it != m_pkItemList.end(); it++)
			 {
				(*it)->Resize( iNewWidth, m_unItemHeight);
			 }

		m_kItemArea.Right = m_kItemArea.Left + iWidth;
	}

	//Resize( GetScreenRect().Width(), m_pkItemList.size() * m_unItemHeight);

	return true;
}

bool ZGuiListbox::RemoveItem(unsigned int iID)
{
	Rect rcRemoveItem, rcSearchItem;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetID() == iID)
			 {
				 ZGuiButton* pkButton = (*it)->GetButton();

				 if(pkButton)
				 {
					 rcRemoveItem = pkButton->GetScreenRect();

					 delete (*it);
					 m_pkItemList.erase(it);
				 }
				 break;
			 }
		 }

	// Flytta upp alla element som är under denna.
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 rcSearchItem = (*it)->GetButton()->GetScreenRect();
			 if(rcSearchItem.Top > rcRemoveItem.Top)
			 {
				 (*it)->Move(0,-m_unItemHeight);
			 }
		 }

	UpdateList();
	return false;
}

bool ZGuiListbox::RemoveAllItems()
{
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			delete (*it);
		 }

	m_pkSelectedItem = NULL;

	m_pkItemList.clear();
	UpdateList();

	ZGuiWnd::m_pkPrevWndUnderCursor = NULL;
	ZGuiWnd::m_pkPrevWndClicked = NULL;
	ZGuiWnd::m_pkFocusWnd = NULL;
	ZGuiWnd::m_pkWndUnderCursor = NULL;
	ZGuiWnd::m_pkWndClicked = NULL;

	return true;
}

bool ZGuiListbox::Notify(ZGuiWnd* pkWnd, int iCode)
{
	if(pkWnd->GetID() == VERT_SCROLLBAR_ID)
	{
		ScrollItems(m_pkScrollbarVertical);
		return true;
	}

	if(iCode == NCODE_CLICK_DOWN)
	{
		list<ZGuiListitem*>::iterator it;
		for( it = m_pkItemList.begin();
			 it != m_pkItemList.end(); it++)
			 {
				 if(pkWnd->GetID() == (*it)->GetID())
				 {
					// Change skin on previus selected button
					if(m_pkSelectedItem)
						m_pkSelectedItem->Deselect();

					// Select new item
					m_pkSelectedItem = (*it);

					// Send a scroll message to the main winproc...
					int* piParams = new int[2];
					piParams[0] = GetID(); // Listbox ID
					piParams[1] = m_pkSelectedItem->GetID(); // list item ID
					GetGUI()->GetActiveCallBackFunc()(
						GetGUI()->GetActiveMainWnd(), ZGM_SELECTLISTITEM, 2, piParams);
					delete[] piParams;

					break;
				 }
			 }
	}

	if(m_pkParent)
		m_pkParent->Notify(this, iCode);

	return true;
}

void ZGuiListbox::ScrollItems(ZGuiScrollbar* pkScrollbar)
{
	// Move all items
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			(*it)->Move(0,pkScrollbar->m_iScrollChange*(int)m_unItemHeight);

			Rect rc = (*it)->GetButton()->GetScreenRect();
			if(rc.Bottom > GetScreenRect().Top && rc.Top < GetScreenRect().Bottom)
			{
				(*it)->GetButton()->Show();
			}
			else
			{
				(*it)->GetButton()->Hide();
			}
		 }	

	// Reset parameter
	pkScrollbar->m_iScrollChange = 0;
}

ZGuiListitem* ZGuiListbox::GetSelItem()
{
	return m_pkSelectedItem;
}

ZGuiListitem* ZGuiListbox::GetItem(unsigned int iID)
{
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetID() == iID)
				 return (*it);
		 }

	return NULL;
}

void ZGuiListbox::IsMenu(bool bMenu)
{
	m_bIsMenu = bMenu;
}

void ZGuiListbox::UpdateList()
{
	int iElements = m_pkItemList.size();
	
	if(iElements > 0)
	{
		int y = (iElements - 1) * m_unItemHeight; 
		m_pkItemList.back()->SetPos(0, y);
		m_pkItemList.back()->Resize(m_kItemArea.Width(), m_unItemHeight);
	}

	// Får alla elementen plats? Nehe, hur många för mycket är det då?
	int iElementSize = m_unItemHeight * iElements;
	int iListboxSize = GetWndRect().Height() - m_unItemHeight; // lägg på lite extra för att slippa avrundingsfel...
	float fThumbSize = (float) iListboxSize / (float) iElementSize;

	m_pkScrollbarVertical->SetScrollInfo(0,iElements,fThumbSize,0);
	ScrollItems(m_pkScrollbarVertical);

	if(fThumbSize >= 0.99f || iElements == 0)
	{
		m_pkScrollbarVertical->Hide();
	}
	else
	{
		m_pkScrollbarVertical->Show();
	}
}

void ZGuiListbox::Resize(int Width, int Height)
{
	if(Height > m_unOriginalHeight)
		Height = m_unOriginalHeight;

	Rect rc = GetWndRect();
	rc.Bottom = rc.Top + Height;
	rc.Right = rc.Left + Width-m_iScrollbarWidth;

	ZGuiWnd::Resize(Width, Height); 

	m_kItemArea = rc;

	if(m_pkScrollbarVertical)
		m_pkScrollbarVertical->Resize(m_iScrollbarWidth, 
			m_kItemArea.Height()-m_unItemHeight);

	UpdateList();
}

void ZGuiListbox::SelNone()
{
	m_pkSelectedItem = NULL;
}

void ZGuiListbox::SetZValue(int iValue)
{
	ZGuiWnd::SetZValue(iValue);
	m_pkScrollbarVertical->SetZValue(iValue-1);
}

int ZGuiListbox::GetItemCount()
{
	return m_pkItemList.size();
}
