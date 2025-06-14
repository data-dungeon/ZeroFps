// Listbox.cpp: implementation of the Listbox class.
//
//////////////////////////////////////////////////////////////////////

#include "zguilistbox.h"
#include "../render/zguirenderer.h"
#include "../basic/zfassert.h"
#include "zguiscrollbar.h"
#include "zguibutton.h"
#include "zgui.h"
#include <typeinfo>

// Static internal IDs for the scrollbars
const int VERT_SCROLLBAR_ID = 620;
const int HORZ_SCROLLBAR_ID = 621;
const int FONT_SIZE = 12;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiListbox::ZGuiListbox(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, 
						 int iID, int iItemHeight, 
						 ZGuiSkin* pkSkinItem, 
						 ZGuiSkin* pkSkinItemSelected, 
						 ZGuiSkin* pkSkinItemHighLight) :
	ZGuiWnd(kRectangle, pkParent, bVisible, iID)
{
	m_iScrollbarWidth = 20;
	m_bIsMenu = false;

	m_pkSelectedItem = NULL;
	m_pkSkinBnUp = pkSkinItem;
	m_pkSkinBnDown = pkSkinItemSelected;
	m_pkSkinBnHLight = pkSkinItemHighLight;

	m_unItemHeight = iItemHeight;

	int w = GetWndRect().Width();
	int h = GetWndRect().Height();
	
	ZGuiWnd::Resize(w,h);

	CreateInternalControls();

	m_kItemArea = GetWndRect();
	m_unOriginalHeight = GetScreenRect().Height();

	RemoveWindowFlag(WF_CANHAVEFOCUS); // f�nster har focus by default
	RemoveWindowFlag(WF_TOPWINDOW); // kan inte anv�ndas som mainwindow
}

ZGuiListbox::~ZGuiListbox()
{
	for( itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
	{
		delete (*it);
		(*it) = NULL;
	}

	m_pkItemList.clear(); 

	m_pkSelectedItem = NULL;

	delete m_pkScrollbarVertical;
	m_pkScrollbarVertical = NULL;
}

bool ZGuiListbox::Render( ZGuiRender* pkRenderer )
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

	if(m_pkScrollbarVertical->IsVisible())
		m_pkScrollbarVertical->Render( pkRenderer );

	if(m_pkSelectedItem)
	{
		if(m_bIsMenu == false)
			m_pkSelectedItem->Select();
	}

	for( itItemList it = m_pkItemList.begin(); it != m_pkItemList.end(); it++)
		(*it)->GetButton()->Render( pkRenderer );

	return true;
}

void ZGuiListbox::SetItemNormalSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnUp = pkSkin;
}

void ZGuiListbox::SetItemSelectedSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnDown = pkSkin;
}

void ZGuiListbox::SetItemHighLightSkin(ZGuiSkin* pkSkin)
{
	m_pkSkinBnHLight = pkSkin;
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
	int y = 0;
	int w = m_iScrollbarWidth;
	int h = rc.Height();

	m_pkScrollbarVertical = new ZGuiScrollbar(Rect(x,0,x+w,GetScreenRect().Height()),
		this,true,VERT_SCROLLBAR_ID); 

	m_pkScrollbarVertical->SetScrollInfo(0,0,1.0f,0); 
}

ZGuiListitem* ZGuiListbox::AddItem(char* strText, unsigned int iIndex, bool bSelect)
{
	ZGuiListitem* pkNewItem;
	pkNewItem = new ZGuiListitem(this, strText, iIndex, 
		m_pkSkinBnUp, m_pkSkinBnDown, m_pkSkinBnHLight);
	pkNewItem->GetButton()->SetGUI(GetGUI());

	m_pkItemList.push_back(pkNewItem);

	int iElements = m_pkItemList.size();
	
	if(iElements > 0)
	{
		int y = (iElements - 1) * m_unItemHeight; 
		m_pkItemList.back()->SetPos(0, y);
		m_pkItemList.back()->Resize(m_kItemArea.Width(), m_unItemHeight);
	}

	UpdateList();

	ZGui* pkGui = GetGUI();
	if(m_pkFont == NULL && pkGui)
		m_pkFont = pkGui->GetBitmapFont(ZG_DEFAULT_GUI_FONT);

	int iWidth = GetScreenRect().Width();
	int iHeight = GetScreenRect().Height();
	int iNewWidth = m_pkFont->GetLength(strText) + 25;

	if(iNewWidth > iWidth && m_bIsMenu == true)
	{
		ZGuiWnd::Resize( iNewWidth, iHeight);
		
		// Also resize combobox if that is the parent
		ZGuiWnd* pkParent = GetParent();
		if(pkParent && typeid(*pkParent)==typeid(ZGuiCombobox))
			pkParent->Resize( iNewWidth, iHeight);

		list<ZGuiListitem*>::iterator it;
		for( it = m_pkItemList.begin();
			 it != m_pkItemList.end(); it++)
			 {
				(*it)->Resize( iNewWidth, m_unItemHeight);
			 }

		m_kItemArea.Right = m_kItemArea.Left + iWidth;
	}

	if(bSelect)
		m_pkSelectedItem = pkNewItem;

	return pkNewItem;
}

bool ZGuiListbox::RemoveItem(ZGuiListitem* pkItemToRemove, bool bSelPrev)
{
	// Inneh�ller listan bara ett (1) alternativ?
	if(m_pkItemList.size() == 1)
	{
		RemoveAllItems();
		return true;
	}	

	bool bHaveFoundItem = false;
	int index_to_select = -1;

	vector<pair<string, int> > temp; // don�t forget the space after '>'!
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it) != pkItemToRemove)
			 {
				 temp.push_back(pair<string, int>(string((*it)->GetText()),(*it)->GetIndex()));
			
				 if(bHaveFoundItem && index_to_select == -1)
					index_to_select = (*it)->GetIndex(); 
			 }
			 else
			 {
				 bHaveFoundItem = true;
			 }
		 }

	RemoveAllItems();

	if(bHaveFoundItem && index_to_select == -1)
		index_to_select = temp.back().second;

	for(unsigned int i=0; i<temp.size(); i++)
	{
		bool bSelect = bSelPrev && temp[i].second == index_to_select;
		AddItem((char*)temp[i].first.c_str(), temp[i].second, bSelect); 
	}

	return true;
}

bool ZGuiListbox::RemoveAllItems()
{
	if(m_pkItemList.empty())
		return true;

	if(m_pkSelectedItem)
		ResetStaticClickWnds(m_pkSelectedItem->GetButton());

	ZGuiWnd::m_pkPrevWndClicked = NULL;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			ZGuiListitem* pkListItem = (*it);
			delete pkListItem;
		 }

	m_pkItemList.clear();

	m_pkSelectedItem = NULL;
	UpdateList();

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
				 if(pkWnd == (*it)->GetButton())
				 {
					// Change skin on previus selected button
					if(m_pkSelectedItem)
						m_pkSelectedItem->Deselect();

					// Select new item
					m_pkSelectedItem = (*it);

					// Send a scroll message to the main winproc...
					int* piParams = new int[2];
					piParams[0] = GetID(); // Listbox ID
					piParams[1] = m_pkSelectedItem->GetIndex(); // list item ID
					
					ZGui* pkGui = GetGUI();
					if(pkGui == NULL)
					{
						pkGui = GetParent()->GetGUI();
						ZFAssert(pkGui, "ZGuiListbox::Notify, GetGUI = NULL");
					}

					callbackfunc cb = pkGui->GetActiveCallBackFunc();
					ZFAssert(cb, "ZGuiListbox::Notify, GetActiveCallBackFunc = NULL");

					ZGuiWnd* pkActivMainWnd = pkGui->GetActiveMainWnd();
					ZFAssert(pkActivMainWnd, "ZGuiListbox::Notify, pkGui->GetActiveMainWnd() = NULL");
					
					cb(pkActivMainWnd, ZGM_SELECTLISTITEM, 2, piParams);
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
			if( rc.Bottom > GetScreenRect().Top && 
				rc.Top < GetScreenRect().Bottom)  
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

ZGuiListitem* ZGuiListbox::GetItem(unsigned int iIndex)
{
	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetIndex() == iIndex)
				 return (*it);
		 }

	return NULL;
}

void ZGuiListbox::IsMenu(bool bMenu)
{
	m_bIsMenu = bMenu;

	if(bMenu == true)
		m_pkScrollbarVertical->Hide();

	// must de-activte autohide because we dont
	// want the scrollbar to show it self if the
	// listbox is a menu.
	m_pkScrollbarVertical->SetAutoHide(!bMenu); 

	UpdateList();
}

void ZGuiListbox::UpdateList()
{
	int iElements = m_pkItemList.size();
	
	// F�r alla elementen plats? Nehe, hur m�nga f�r mycket �r det d�?
	int iElementSize = m_unItemHeight * iElements;
	int iListboxSize = GetScreenRect().Height() - m_unItemHeight; // l�gg p� lite extra f�r att slippa avrundingsfel...
	if(iElementSize <= 0) iElementSize = 1; // don�t devide by zero
	float fThumbSize = (float) iListboxSize / (float) iElementSize;

	m_pkScrollbarVertical->SetScrollInfo(0,iElements,fThumbSize,0);
	
	if(fThumbSize >= 0.99f || iElements == 0)
	{
		m_pkScrollbarVertical->Hide();
	}
	else
	{
		if(m_bIsMenu == false) // don't show scrollbar if menu
			m_pkScrollbarVertical->Show();
	}

	// Resize all items
	int iNewWidth = GetScreenRect().Width();
	if(m_pkScrollbarVertical->IsVisible())
		iNewWidth -= m_iScrollbarWidth;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
			(*it)->Resize( iNewWidth, m_unItemHeight);


	ScrollItems(m_pkScrollbarVertical);
}

void ZGuiListbox::Resize(int Width, int Height)
{
/*	if(Height > m_unOriginalHeight)
		Height = m_unOriginalHeight;*/

	Rect rc = GetWndRect();
	rc.Bottom = rc.Top + Height;
	rc.Right = rc.Left + Width;

	if(m_pkScrollbarVertical)
	{
		if(m_pkScrollbarVertical->IsVisible())
			rc.Right -= m_iScrollbarWidth;	
	}

	ZGuiWnd::Resize(Width, Height); 

	m_kItemArea = rc;

/*	if(m_pkScrollbarVertical)
		m_pkScrollbarVertical->Resize(m_iScrollbarWidth, 
			m_kItemArea.Height()-m_unItemHeight);*/

	UpdateList();
}

void ZGuiListbox::SelNone()
{
	if(m_pkSelectedItem)
		m_pkSelectedItem->Deselect();

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

int ZGuiListbox::Find(char* strString)
{
	int counter=0;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if(strcmp(strString, (*it)->GetText()) == 0)
				 return counter;

			 counter++;
		 }

	return -1;
}

bool ZGuiListbox::SelItem(int iIndex)
{
	if(iIndex < 0 || iIndex > m_pkItemList.size()-1)
		return false;

	list<ZGuiListitem*>::iterator it;
	for( it = m_pkItemList.begin();
		 it != m_pkItemList.end(); it++)
		 {
			 if((*it)->GetIndex() == iIndex)
			 {
				 if(m_pkSelectedItem)
					m_pkSelectedItem->Deselect();

				 m_pkSelectedItem = (*it);

				 if(m_bIsMenu == false)
					m_pkSelectedItem->Select();

				 //ZGuiWnd::m_pkFocusWnd = m_pkSelectedItem->GetButton();
				 break;
			 }
		 }

	UpdateList();

	return true;
}

void ZGuiListbox::GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const
{
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkin, string("Listbox")) );
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnUp, string("Listbox: Button up")) );
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnDown, string("Listbox: Button down")) );
	pkSkinDesc.push_back( SKIN_DESC(m_pkSkinBnHLight, string("Listbox: Button focus")) );

	int iStart = pkSkinDesc.size(); 
	m_pkScrollbarVertical->GetWndSkinsDesc(pkSkinDesc);
	for(unsigned int i=iStart; i<pkSkinDesc.size(); i++)
		pkSkinDesc[i].second.insert(0, "Listbox: ");
}

void ZGuiListbox::CopyNonUniqueData(const ZGuiWnd* pkSrc)
{
	if(pkSrc && typeid(*pkSrc)==typeid(ZGuiListbox))
	{
		if(((ZGuiListbox*)pkSrc)->m_pkScrollbarVertical)
			m_pkScrollbarVertical->CopyNonUniqueData( 
				((ZGuiListbox*)pkSrc)->m_pkScrollbarVertical );
	}

	ZGuiWnd::CopyNonUniqueData(pkSrc);
}




